/*****************************************************************************/
/*!
 *  \file        mrv_priv.h \n
 *  \version     1.2 \n
 *  \author      Friebe \n
 *  \brief       Private header of Marvin driver. \n
 *
 *  \note        Some defines are currently not used in the code, \n
 *               so they are added as comment only. \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_priv.h 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#ifndef _MRV_PRIV_H
#define _MRV_PRIV_H

// for doxygen :(
#include "stdinc.h"


/********************************************************************/
//Marvin Registers
/********************************************************************/

// Defintions of block register sizes
#define MRV_ISP_GAMMA_R_Y_ARR_SIZE 17
#define MRV_ISP_GAMMA_G_Y_ARR_SIZE 17
#define MRV_ISP_GAMMA_B_Y_ARR_SIZE 17
#define MRV_ISP_CT_COEFF_ARR_SIZE 9
#define MRV_ISP_GAMMA_OUT_Y_ARR_SIZE 17
#define MRV_ISP_BP_NEW_TABLE_ARR_SIZE 8
#define MRV_ISP_HIST_BIN_ARR_SIZE 16

// - REGISTER LAYOUT ----------------------------------------------------------
typedef struct
{
    UINT32 vi_ccl; // Clock control register (rw) MRV_BASE + 0x0000
    UINT32 vi_custom_reg1;                      // 0x04 reserved
    UINT32 vi_id; // Revision identification register (r) MRV_BASE + 0x0008
    UINT32 vi_custom_reg2;                      // 0x0C reserved
    UINT32 vi_iccl; // Internal clock  control register (rw) MRV_BASE + 0x0010
    UINT32 vi_ircl; // Internal reset control register (rw) MRV_BASE + 0x0014
    UINT32 vi_dpcl; // Data path control register (rw) MRV_BASE + 0x0018

#if (MARVIN_FEATURE_CLOCK_DOMAINS == MARVIN_CLOCK_DOMAINS_2)
    UINT32 vi_toc;                              // 0x1C access timeout control register
#elif (MARVIN_FEATURE_CLOCK_DOMAINS == MARVIN_CLOCK_DOMAINS_1)
    UINT32 notused_mrvbase1;
#else   // MARVIN_FEATURE_CLOCK_DOMAINS
    #error unknown MARVIN_FEATURE_CLOCK_DOMAINS
#endif  // MARVIN_FEATURE_CLOCK_DOMAINS

#if ((MARVIN_FEATURE_DMA_READ == MARVIN_FEATURE_DMA_READ_V1) || \
     (MARVIN_FEATURE_DMA_READ == MARVIN_FEATURE_DMA_READ_V2))
    UINT32 vi_spmcl;                            // 0x20 self path mux control register
    UINT32 notused_mrvbase2[(0x200-0x24)/4];    // memory gap with selfpicture DMA
#else   // MARVIN_FEATURE_DMA_READ
    UINT32 notused_mrvbase2[(0x200-0x20)/4];    // memory gap without selfpicture DMA
#endif  // MARVIN_FEATURE_DMA_READ

#if (MARVIN_FEATURE_IMAGE_EFFECTS >= MARVIN_FEATURE_EXIST)
//    UINT32 ie_ctrl;                             // 0x200
//    UINT32 ie_color_sel;                        // 0x204
//    UINT32 ie_mat_1;                            // 0x208
//    UINT32 ie_mat_2;                            // 0x20C
//    UINT32 ie_mat_3;                            // 0x210
//    UINT32 ie_mat_4;                            // 0x214
//    UINT32 ie_mat_5;                            // 0x218
//    UINT32 ie_tint;                             // 0x21C
//    UINT32 ie_ctrl_shd;                         // 0x220
    UINT32 img_eff_ctrl; // Global control register (rw) MRV_IMGEFF_BASE + 0x0000
    UINT32 img_eff_color_sel; // Color selection register (for color selection effect) (rw) MRV_IMGEFF_BASE + 0x0004
    UINT32 img_eff_mat_1; // 3x3 matrix coefficients for emboss effect (1) (rw) MRV_IMGEFF_BASE + 0x0008
    UINT32 img_eff_mat_2; // 3x3 matrix coefficients for emboss effect (2) (rw) MRV_IMGEFF_BASE + 0x000c
    UINT32 img_eff_mat_3; // 3x3 matrix coefficients for emboss(3) effect / sketch(1) effect (rw) MRV_IMGEFF_BASE + 0x0010
    UINT32 img_eff_mat_4; // 3x3 matrix coefficients for sketch effect (2) (rw) MRV_IMGEFF_BASE + 0x0014
    UINT32 img_eff_mat_5; // 3x3 matrix coefficients for sketch effect (3) (rw) MRV_IMGEFF_BASE + 0x0018
    UINT32 img_eff_tint; // Chrominance increment values of a tint (used for sepia effect) (rw) MRV_IMGEFF_BASE + 0x001c
    UINT32 img_eff_ctrl_shd; // Shadow register for control register (r) MRV_IMGEFF_BASE + 0x0020
#if (MARVIN_FEATURE_IMAGE_EFFECTS >= MARVIN_FEATURE_IE_V3)
    UINT32 img_eff_sharpen; // Factor and threshold for sharpen effect (rw) MRV_IMGEFF_BASE + 0x0024
    UINT32 notused_imgeff[(0x300-0x228)/4];     // memory gap with image effects
#else // (MARVIN_FEATURE_IMAGE_EFFECTS >= MARVIN_FEATURE_IE_V3)
    UINT32 notused_imgeff[(0x300-0x224)/4];     // memory gap with image effects
#endif
#else   // MARVIN_FEATURE_IMAGE_EFFECTS
    UINT32 notused_imgeff[(0x300-0x200)/4];     // memory gap without image effects
#endif  // MARVIN_FEATURE_IMAGE_EFFECTS

#if (MARVIN_FEATURE_SUPERIMPOSE == MARVIN_FEATURE_EXIST)
//    UINT32 si_ctrl;                             // 0x300
//    UINT32 si_offset_x;                         // 0x304
//    UINT32 si_offset_y;                         // 0x308
//    UINT32 si_color_y;                          // 0x30C
//    UINT32 si_color_cb;                         // 0x310
//    UINT32 si_color_cr;                         // 0x314
    UINT32 super_imp_ctrl; // Global control register (rw) MRV_SI_BASE + 0x0000
    UINT32 super_imp_offset_x; // Offset x register (rw) MRV_SI_BASE + 0x0004
    UINT32 super_imp_offset_y; // Offset y register (rw) MRV_SI_BASE + 0x0008
    UINT32 super_imp_color_y; // Y component of transparent key color (rw) MRV_SI_BASE + 0x000c
    UINT32 super_imp_color_cb; // Cb component of transparent key color (rw) MRV_SI_BASE + 0x0010
    UINT32 super_imp_color_cr; // Cr component of transparent key color (rw) MRV_SI_BASE + 0x0014
    UINT32 notused_simp[(0x400-0x318)/4];       // memory gap with superimpose
#else   // MARVIN_FEATURE_SUPERIMPOSE
    UINT32 notused_simp[(0x400-0x300)/4];       // memory gap without superimpose
#endif  // MARVIN_FEATURE_SUPERIMPOSE

    UINT32 isp_ctrl; // global control register (rw – partly write-only) MRV_ISP_BASE + 0x0000
    UINT32 isp_acq_prop; // ISP acquisition properties (rw) MRV_ISP_BASE + 0x0004
    UINT32 isp_acq_h_offs; // horizontal input offset (rw) MRV_ISP_BASE + 0x0008
    UINT32 isp_acq_v_offs; // vertical input offset (rw) MRV_ISP_BASE + 0x000c
    UINT32 isp_acq_h_size; // horizontal input size (rw) MRV_ISP_BASE + 0x0010
    UINT32 isp_acq_v_size; // vertical input size (rw) MRV_ISP_BASE + 0x0014
    UINT32 isp_acq_nr_frames; // Number of frames to be captured (rw) MRV_ISP_BASE + 0x0018
//    UINT32 isp_gamma_dx0;                       // 0x41C input gamma curve dx1...dx8 increments
//    UINT32 isp_gamma_dx1;                       // 0x420 input gamma curve dx9...dx16 increments
//    UINT32 isp_gamma_r_y0;                      // 0x424 input gamma curve y0 value (R)
//    UINT32 isp_gamma_r_y1;                      // 0x428 input gamma curve y1 value (R)
// ...
//    UINT32 isp_gamma_r_y16;                     // 0x464 input gamma curve y16 value (R)
//    UINT32 isp_gamma_g_y0;                      // 0x468 input gamma curve y0 value (G)
//    UINT32 isp_gamma_g_y1;                      // 0x46C input gamma curve y1 value (G)
// ...
//    UINT32 isp_gamma_g_y16;                     // 0x4A8 input gamma curve y16 value (G)
//    UINT32 isp_gamma_b_y0;                      // 0x4AC input gamma curve y0 value (B)
//    UINT32 isp_gamma_b_y1;                      // 0x4B0 input gamma curve y1 value (B)
// ...
//    UINT32 isp_gamma_b_y16;                     // 0x4EC input gamma curve y16 value (B)
    UINT32 isp_gamma_dx_lo; // De-Gamma Curve definition lower x increments (sampling points) (rw) MRV_ISP_BASE + 0x001c
    UINT32 isp_gamma_dx_hi; // De-Gamma Curve definition higher x increments (sampling points) (rw) MRV_ISP_BASE + 0x0020
    UINT32 isp_gamma_r_y[MRV_ISP_GAMMA_R_Y_ARR_SIZE]; // De-Gamma Curve definition y red (rw) 0x0024+n (n=0..16)
    UINT32 isp_gamma_g_y[MRV_ISP_GAMMA_G_Y_ARR_SIZE]; // De-Gamma Curve definition y green (rw) 0x0068+n (n=0..16)
    UINT32 isp_gamma_b_y[MRV_ISP_GAMMA_B_Y_ARR_SIZE]; // De-Gamma Curve definition y blue (rw) 0x00AC+n (n=0..16)

#if ((MARVIN_FEATURE_BLACK_LEVEL >= MARVIN_FEATURE_BLACK_LEVEL_V1) && \
     (MARVIN_FEATURE_BLACK_LEVEL <= MARVIN_FEATURE_BLACK_LEVEL_V3))
    UINT32 isp_bls_prop;                        // 0x4F0 black level subtraction properties
    UINT32 isp_bls_h_window;                    // 0x4F4 black level horizontal window definition
    UINT32 isp_bls_v_window;                    // 0x4F8 black level vertical window definition
    UINT32 isp_bls_samples;                     // 0x4FC number of samples for black level subtraction
    UINT32 isp_bls_ref_value_ab;                // 0x500 black level value A and B for manual control
    UINT32 isp_bls_ref_value_cd;                // 0x504 black level value C and D for manual control
    UINT32 isp_bls_mean_value_ab;               // 0x508 measured black level value for A and B
    UINT32 isp_bls_mean_value_cd;               // 0x50C measured black level value for C and D
#else   // MARVIN_FEATURE_BLACK_LEVEL
    UINT32 notused_ispbls1[(0x510-0x4F0)/4];    // memory gap (without BLS version 1 - 3)
#endif  // MARVIN_FEATURE_BLACK_LEVEL

    UINT32 isp_awb_prop;   // Auto white balance properties (rw) MRV_ISP_BASE + 0x0110
    UINT32 isp_awb_h_offs; // Auto white balance horizontal offset of measure window (rw) MRV_ISP_BASE + 0x0114
    UINT32 isp_awb_v_offs; // Auto white balance vertical offset of measure window (rw) MRV_ISP_BASE + 0x0118
    UINT32 isp_awb_h_size; // Auto white balance horizontal window size (rw) MRV_ISP_BASE + 0x011c
    UINT32 isp_awb_v_size; // Auto white balance vertical window size (rw) MRV_ISP_BASE + 0x0120
    UINT32 isp_awb_frames; // Auto white balance mean value over multiple frames (rw) MRV_ISP_BASE + 0x0124
    UINT32 isp_awb_ref;    // Auto white balance reference Cb/Cr values (rw) MRV_ISP_BASE + 0x0128
    UINT32 isp_awb_thresh; // Auto white balance threshold values (rw) MRV_ISP_BASE + 0x012c

    UINT32 notused_ispawb2[(0x538-0x530)/4];    // memory gap (without hardware automatic measurement)

    UINT32 isp_awb_gain_g; // Auto white balance gain green (rw) MRV_ISP_BASE + 0x0138
    UINT32 isp_awb_gain_rb; // Auto white balance gain red and blue (rw) MRV_ISP_BASE + 0x013c

    UINT32 isp_awb_white_cnt; // Auto white balance white pixel count (r) MRV_ISP_BASE + 0x0140
    UINT32 isp_awb_mean; // Auto white balance measured mean value (r) MRV_ISP_BASE + 0x0144

#if (MARVIN_FEATURE_AUTO_EXPOSURE == MARVIN_FEATURE_AUTO_EXPOSURE_V1)
    UINT32 isp_ae_h_offs;                       // 0x548 auto exposure measurement window horizontal offset
    UINT32 isp_ae_v_offs;                       // 0x54C auto exposure measurement window vertical offset
    UINT32 isp_ae_h_size;                       // 0x550 auto exposure measurement window horizontal size
    UINT32 isp_ae_v_size;                       // 0x554 auto exposure measurement window vertical size
    UINT32 isp_ae_frames;                       // 0x558 number of frames for mean value calculation
    UINT32 isp_ae_hist_range;                   // 0x55C auto exposure luminance histogram range
    UINT32 isp_ae_occ_y;                        // 0x560 auto exposure luminance histogram occurrence
    UINT32 isp_ae_mean_y;                       // 0x564 auto exposure mean luminance value
    UINT32 isp_ae_max_y;                        // 0x568 auto exposure maximum luminance value
    UINT32 isp_ae_min_y;                        // 0x56C auto exposure minimum luminance value
#else   // MARVIN_FEATURE_AUTO_EXPOSURE
    UINT32 notused_ispae[(0x570-0x548)/4];      // memory gap (without exposure measurement V1)
#endif  // MARVIN_FEATURE_AUTO_EXPOSURE

    UINT32 isp_cc_coeff_0; // Color conversion coefficient 0 (rw) MRV_ISP_BASE + 0x0170
    UINT32 isp_cc_coeff_1; // Color conversion coefficient 1 (rw) MRV_ISP_BASE + 0x0174
    UINT32 isp_cc_coeff_2; // Color conversion coefficient 2 (rw) MRV_ISP_BASE + 0x0178
    UINT32 isp_cc_coeff_3; // Color conversion coefficient 3 (rw) MRV_ISP_BASE + 0x017c
    UINT32 isp_cc_coeff_4; // Color conversion coefficient 4 (rw) MRV_ISP_BASE + 0x0180
    UINT32 isp_cc_coeff_5; // Color conversion coefficient 5 (rw) MRV_ISP_BASE + 0x0184
    UINT32 isp_cc_coeff_6; // Color conversion coefficient 6 (rw) MRV_ISP_BASE + 0x0188
    UINT32 isp_cc_coeff_7; // Color conversion coefficient 7 (rw) MRV_ISP_BASE + 0x018c
    UINT32 isp_cc_coeff_8; // Color conversion coefficient 8 (rw) MRV_ISP_BASE + 0x0190

#if ((MARVIN_FEATURE_OUTPUT_FORMATTER >= MARVIN_FEATURE_OUTPUT_FORMATTER_V1) && \
     (MARVIN_FEATURE_OUTPUT_FORMATTER <= MARVIN_FEATURE_OUTPUT_FORMATTER_V2))
    UINT32 isp_out_h_offs; // Horizontal offset of output window (rw) MRV_ISP_BASE + 0x0194
    UINT32 isp_out_v_offs; // Vertical offset of output window (rw) MRV_ISP_BASE + 0x0198
    UINT32 isp_out_h_size; // Output horizontal picture size (rw) MRV_ISP_BASE + 0x019c
    UINT32 isp_out_v_size; // Output vertical picture size (rw) MRV_ISP_BASE + 0x01a0
#else   // MARVIN_FEATURE_OUTPUT_FORMATTER
    UINT32 notused_ispout1[(0x5A4-0x594)/4];    // memory gap (without output formatter)
#endif  // MARVIN_FEATURE_OUTPUT_FORMATTER

    UINT32 isp_demosaic; // Demosaic parameters (rw) MRV_ISP_BASE + 0x01a4
    UINT32 isp_flags_shd; // Shadow flags (current status of certain signals) (r) MRV_ISP_BASE + 0x01a8

#if ((MARVIN_FEATURE_OUTPUT_FORMATTER >= MARVIN_FEATURE_OUTPUT_FORMATTER_V1) && \
     (MARVIN_FEATURE_OUTPUT_FORMATTER <= MARVIN_FEATURE_OUTPUT_FORMATTER_V2))
    UINT32 isp_out_h_offs_shd; // current horizontal offset of output window (shadow register) (r) MRV_ISP_BASE + 0x01ac
    UINT32 isp_out_v_offs_shd; // current vertical offset of output window (shadow register) (r) MRV_ISP_BASE + 0x01b0
    UINT32 isp_out_h_size_shd; // current output horizontal picture size (shadow register) (r) MRV_ISP_BASE + 0x01b4
    UINT32 isp_out_v_size_shd; // current output vertical picture size (shadow register) (r) MRV_ISP_BASE + 0x01b8
#else   // MARVIN_FEATURE_OUTPUT_FORMATTER
    UINT32 notused_ispout2[(0x5BC-0x5AC)/4];    // memory gap (without output formatter)
#endif  // MARVIN_FEATURE_OUTPUT_FORMATTER

    UINT32 isp_imsc; // Interrupt mask (rw) MRV_ISP_BASE + 0x01bc
    UINT32 isp_ris; // Raw interrupt status (r) MRV_ISP_BASE + 0x01c0
    UINT32 isp_mis; // Masked interrupt status (r) MRV_ISP_BASE + 0x01c4
    UINT32 isp_icr; // Interrupt clear register (w) MRV_ISP_BASE + 0x01c8
    UINT32 isp_isr; // Interrupt set register (w) MRV_ISP_BASE + 0x01cc

#if ((MARVIN_FEATURE_XTALK >= MARVIN_FEATURE_XTALK_10BITS) && \
     (MARVIN_FEATURE_XTALK <= MARVIN_FEATURE_XTALK_11BITS))
//    UINT32 isp_ct_coeff_0;                      // 0x5D0 crosstalk matrix coefficient 0
//    UINT32 isp_ct_coeff_1;                      // 0x5D4 crosstalk matrix coefficient 1
// ...
//    UINT32 isp_ct_coeff_8;                      // 0x5F0 crosstalk matrix coefficient 8
    UINT32 isp_ct_coeff[MRV_ISP_CT_COEFF_ARR_SIZE]; // cross-talk configuration register (color correction matrix) (rw) 0x01D0+n (n=0..8)
#else   // MARVIN_FEATURE_XTALK
    UINT32 notused_ispxtalk[(0x5F4-0x5D0)/4];   // memory gap (without CrossTalk)
#endif  // MARVIN_FEATURE_XTALK

#if (MARVIN_FEATURE_GAMMAOUT == MARVIN_FEATURE_EXIST)
//    UINT32 isp_gamma_out_y0;                    // 0x5F8 output gamma curve y0 value
//    UINT32 isp_gamma_out_y1;                    // 0x5FC output gamma curve y1 value
// ...
//    UINT32 isp_gamma_out_y16;                   // 0x638 output gamma curve y16 value
    UINT32 isp_gamma_out_mode; // gamma segmentation mode register for output gamma (rw) MRV_ISP_BASE + 0x01f4
    UINT32 isp_gamma_out_y[MRV_ISP_GAMMA_OUT_Y_ARR_SIZE]; // Gamma Out Curve definition y (rw) 0x01F8+n (n=0..16)
#else   // MARVIN_FEATURE_GAMMAOUT
    UINT32 notused_ispgammaout[(0x63C-0x5F4)/4];// memory gap (without gamma out)
#endif  // MARVIN_FEATURE_GAMMAOUT

#if (MARVIN_FEATURE_ISP_ERRORFLAGS == MARVIN_FEATURE_EXIST)
    UINT32 isp_err; // ISP error register (r) MRV_ISP_BASE + 0x023c
    UINT32 isp_err_clr; // ISP error clear register (w) MRV_ISP_BASE + 0x0240
#else   // MARVIN_FEATURE_ISP_ERRORFLAGS
    UINT32 notused_isperr[(0x644-0x63C)/4];     // memory gap (without error flags)
#endif  // MARVIN_FEATURE_ISP_ERRORFLAGS

#if (MARVIN_FEATURE_FRAMECOUNTER == MARVIN_FEATURE_EXIST)
    UINT32 isp_frame_count; // Frame counter (r) MRV_ISP_BASE + 0x0244
#else   // MARVIN_FEATURE_FRAMECOUNTER
    UINT32 notused_ispfcount[(0x648-0x644)/4];  // memory gap (without frame counter)
#endif  // MARVIN_FEATURE_FRAMECOUNTER

#if (MARVIN_FEATURE_XTALK_OFFSET == MARVIN_FEATURE_EXIST)
    UINT32 isp_ct_offset_r; // cross-talk offset red (rw) MRV_ISP_BASE + 0x0248
    UINT32 isp_ct_offset_g; // cross-talk offset green (rw) MRV_ISP_BASE + 0x024c
    UINT32 isp_ct_offset_b; // cross-talk offset blue (rw) MRV_ISP_BASE + 0x0250
    UINT32 notused_ispctoffs[(0x660-0x654)/4];   // memory gap (with ct offset)
#else   // MARVIN_FEATURE_XTALK_OFFSET
    UINT32 notused_ispctoffs[(0x660-0x648)/4];   // memory gap (without ct offset)
#endif  // MARVIN_FEATURE_XTALK_OFFSET

#if (MARVIN_FEATURE_FLASH_LIGHT == MARVIN_FEATURE_EXIST)
    UINT32 isp_flash_cmd; // Flash command (w) MRV_FLASH_BASE + 0x0000
    UINT32 isp_flash_config; // Flash config (rw) MRV_FLASH_BASE + 0x0004
    UINT32 isp_flash_prediv; // Flash Counter Pre-Divider (rw) MRV_FLASH_BASE + 0x0008
    UINT32 isp_flash_delay; // Flash Delay (rw) MRV_FLASH_BASE + 0x000c
    UINT32 isp_flash_time; // Flash time (rw) MRV_FLASH_BASE + 0x0010
    UINT32 isp_flash_maxp; // Maximum value for flash or preflash (rw) MRV_FLASH_BASE + 0x0014
    UINT32 notused_ispflash[(0x680-0x678)/4];   // memory gap (with flash light)
#else   // MARVIN_FEATURE_FLASH_LIGHT
    UINT32 notused_ispflash[(0x680-0x660)/4];   // memory gap (without flash light)
#endif  // MARVIN_FEATURE_FLASH_LIGHT

#if (MARVIN_FEATURE_SHUTTER == MARVIN_FEATURE_EXIST)
    UINT32 isp_sh_ctrl; // mechanical shutter control (rw) MRV_SHUT_BASE + 0x0000
    UINT32 isp_sh_prediv; // Mech. Shutter Counter Pre-Divider (rw) MRV_SHUT_BASE + 0x0004
    UINT32 isp_sh_delay; // Delay register (rw) MRV_SHUT_BASE + 0x0008
    UINT32 isp_sh_time; // Time register (rw) MRV_SHUT_BASE + 0x000c
    UINT32 notused_ispsh[(0x800-0x690)/4];      // memory gap (with shutter)
#else   // MARVIN_FEATURE_SHUTTER
    UINT32 notused_ispsh[(0x800-0x680)/4];      // memory gap (without shutter)
#endif  // MARVIN_FEATURE_SHUTTER

    UINT32 c_proc_ctrl; // Global control register (rw) MRV_CPROC_BASE + 0x0000
    UINT32 c_proc_contrast; // Color Processing contrast register (rw) MRV_CPROC_BASE + 0x0004
    UINT32 c_proc_brightness; // Color Processing brightness register (rw) MRV_CPROC_BASE + 0x0008
    UINT32 c_proc_saturation; // Color Processing saturation register (rw) MRV_CPROC_BASE + 0x000c
    UINT32 c_proc_hue; // Color Processing hue register (rw) MRV_CPROC_BASE + 0x0010
    UINT32 notused_cproc[(0xC00-0x814)/4];      // memory gap

#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
    UINT32 mrsz_ctrl; // global control register (rw) MRV_MRSZ_BASE + 0x0000
    UINT32 mrsz_scale_hy; // horizontal luminance scale factor register (rw) MRV_MRSZ_BASE + 0x0004
    UINT32 mrsz_scale_hcb; // horizontal Cb scale factor register (rw) MRV_MRSZ_BASE + 0x0008
    UINT32 mrsz_scale_hcr; // horizontal Cr scale factor register (rw) MRV_MRSZ_BASE + 0x000c
    UINT32 mrsz_scale_vy; // vertical luminance scale factor register (rw) MRV_MRSZ_BASE + 0x0010
    UINT32 mrsz_scale_vc; // vertical chrominance scale factor register (rw) MRV_MRSZ_BASE + 0x0014
    UINT32 mrsz_phase_hy; // horizontal luminance phase register (rw) MRV_MRSZ_BASE + 0x0018
    UINT32 mrsz_phase_hc; // horizontal chrominance phase register (rw) MRV_MRSZ_BASE + 0x001c
    UINT32 mrsz_phase_vy; // vertical luminance phase register (rw) MRV_MRSZ_BASE + 0x0020
    UINT32 mrsz_phase_vc; // vertical chrominance phase register (rw) MRV_MRSZ_BASE + 0x0024
    UINT32 mrsz_scale_lut_addr; // Address pointer of up-scaling look up table (rw) MRV_MRSZ_BASE + 0x0028
    UINT32 mrsz_scale_lut; // Entry of up-scaling look up table (rw) MRV_MRSZ_BASE + 0x002c
    UINT32 mrsz_ctrl_shd; // global control shadow register (r) MRV_MRSZ_BASE + 0x0030
    UINT32 mrsz_scale_hy_shd; // horizontal luminance scale factor shadow register (r) MRV_MRSZ_BASE + 0x0034
    UINT32 mrsz_scale_hcb_shd; // horizontal Cb scale factor shadow register (r) MRV_MRSZ_BASE + 0x0038
    UINT32 mrsz_scale_hcr_shd; // horizontal Cr scale factor shadow register (r) MRV_MRSZ_BASE + 0x003c
    UINT32 mrsz_scale_vy_shd; // vertical luminance scale factor shadow register (r) MRV_MRSZ_BASE + 0x0040
    UINT32 mrsz_scale_vc_shd; // vertical chrominance scale factor shadow register (r) MRV_MRSZ_BASE + 0x0044
    UINT32 mrsz_phase_hy_shd; // horizontal luminance phase shadow register (r) MRV_MRSZ_BASE + 0x0048
    UINT32 mrsz_phase_hc_shd; // horizontal chrominance phase shadow register (r) MRV_MRSZ_BASE + 0x004c
    UINT32 mrsz_phase_vy_shd; // vertical luminance phase shadow register (r) MRV_MRSZ_BASE + 0x0050
    UINT32 mrsz_phase_vc_shd; // vertical chrominance phase shadow register (r) MRV_MRSZ_BASE + 0x0054
    UINT32 notused_mrsz[(0x1000-0x0C58)/4];     // memory gap
#else  // MARVIN_FEATURE_MSCALE_UP
    UINT32 mrsz_ctrl; // global control register (rw) MRV_MRSZ_BASE + 0x0000
    UINT32 mrsz_scale_hy; // horizontal luminance scale factor register (rw) MRV_MRSZ_BASE + 0x0004
    UINT32 mrsz_scale_hc; // horizontal chrominance scale factor register (rw) MRV_MRSZ_BASE + 0x0008
    UINT32 mrsz_scale_vy; // vertical luminance scale factor register (rw) MRV_MRSZ_BASE + 0x000c
    UINT32 mrsz_scale_vc; // vertical chrominance scale factor register (rw) MRV_MRSZ_BASE + 0x0010
    UINT32 mrsz_phase_hy; // horizontal luminance phase register (rw) MRV_MRSZ_BASE + 0x0014
    UINT32 mrsz_phase_hc; // horizontal chrominance phase register (rw) MRV_MRSZ_BASE + 0x0018
    UINT32 mrsz_phase_vy; // vertical luminance phase register (rw) MRV_MRSZ_BASE + 0x001c
    UINT32 mrsz_phase_vc; // vertical chrominance phase register (rw) MRV_MRSZ_BASE + 0x0020
    UINT32 mrsz_ctrl_shd; // global control shadow register (r) MRV_MRSZ_BASE + 0x0024
    UINT32 mrsz_scale_hy_shd; // horizontal luminance scale factor shadow register (r) MRV_MRSZ_BASE + 0x0028
    UINT32 mrsz_scale_hc_shd; // horizontal chromninance scale factor shadow register (r) MRV_MRSZ_BASE + 0x002c
    UINT32 mrsz_scale_vy_shd; // vertical luminance scale factor shadow register (r) MRV_MRSZ_BASE + 0x0030
    UINT32 mrsz_scale_vc_shd; // vertical chrominance scale factor shadow register (r) MRV_MRSZ_BASE + 0x0034
    UINT32 mrsz_phase_hy_shd; // horizontal luminance phase shadow register (r) MRV_MRSZ_BASE + 0x0038
    UINT32 mrsz_phase_hc_shd; // horizontal chrominance phase shadow register (r) MRV_MRSZ_BASE + 0x003c
    UINT32 mrsz_phase_vy_shd; // vertical luminance phase shadow register (r) MRV_MRSZ_BASE + 0x0040
    UINT32 mrsz_phase_vc_shd; // vertical chrominance phase shadow register (r) MRV_MRSZ_BASE + 0x0044
    UINT32 notused_mrsz[(0x1000-0xC48)/4];      // memory gap
#endif  // MARVIN_FEATURE_MSCALE_UP

#if (MARVIN_FEATURE_SSCALE == MARVIN_FEATURE_EXIST)
  #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
    UINT32 srsz_ctrl; // global control register (rw) MRV_SRSZ_BASE + 0x0000
    UINT32 srsz_scale_hy; // horizontal luminance scale factor register (rw) MRV_SRSZ_BASE + 0x0004
    UINT32 srsz_scale_hcb; // horizontal chrominance scale factor register (rw) MRV_SRSZ_BASE + 0x0008
    UINT32 srsz_scale_hcr; // horizontal chrominance scale factor register (rw) MRV_SRSZ_BASE + 0x000c
    UINT32 srsz_scale_vy; // vertical luminance scale factor register (rw) MRV_SRSZ_BASE + 0x0010
    UINT32 srsz_scale_vc; // vertical chrominance scale factor register (rw) MRV_SRSZ_BASE + 0x0014
    UINT32 srsz_phase_hy; // horizontal luminance phase register (rw) MRV_SRSZ_BASE + 0x0018
    UINT32 srsz_phase_hc; // horizontal chrominance phase register (rw) MRV_SRSZ_BASE + 0x001c
    UINT32 srsz_phase_vy; // vertical luminance phase register (rw) MRV_SRSZ_BASE + 0x0020
    UINT32 srsz_phase_vc; // vertical chrominance phase register (rw) MRV_SRSZ_BASE + 0x0024
    UINT32 srsz_scale_lut_addr; // Address pointer of up-scaling look up table (rw) MRV_SRSZ_BASE + 0x0028
    UINT32 srsz_scale_lut; // Entry of up-scaling look up table (rw) MRV_SRSZ_BASE + 0x002c
    UINT32 srsz_ctrl_shd; // global control shadow register (r) MRV_SRSZ_BASE + 0x0030
    UINT32 srsz_scale_hy_shd; // horizontal luminance scale factor shadow register (r) MRV_SRSZ_BASE + 0x0034
    UINT32 srsz_scale_hcb_shd; // horizontal Cb scale factor shadow register (r) MRV_SRSZ_BASE + 0x0038
    UINT32 srsz_scale_hcr_shd; // horizontal Cr scale factor shadow register (r) MRV_SRSZ_BASE + 0x003c
    UINT32 srsz_scale_vy_shd; // vertical luminance scale factor shadow register (r) MRV_SRSZ_BASE + 0x0040
    UINT32 srsz_scale_vc_shd; // vertical chrominance scale factor shadow register (r) MRV_SRSZ_BASE + 0x0044
    UINT32 srsz_phase_hy_shd; // horizontal luminance phase shadow register (r) MRV_SRSZ_BASE + 0x0048
    UINT32 srsz_phase_hc_shd; // horizontal chrominance phase shadow register (r) MRV_SRSZ_BASE + 0x004c
    UINT32 srsz_phase_vy_shd; // vertical luminance phase shadow register (r) MRV_SRSZ_BASE + 0x0050
    UINT32 srsz_phase_vc_shd; // vertical chrominance phase shadow register (r) MRV_SRSZ_BASE + 0x0054
    UINT32 notused_srsz[(0x1400-0x1058)/4];     // memory gap
  #else   // MARVIN_FEATURE_SSCALE_UP
    UINT32 srsz_ctrl;                           // 0x1000 self-picture resize control register
    UINT32 srsz_scale_hy;                       // 0x1004 self-picture resize horizontal luminance scale factor
    UINT32 srsz_scale_hc;                       // 0x1008 self-picture resize horizontal chrominance scale factor
    UINT32 srsz_scale_vy;                       // 0x100C self-picture resize vertical luminance scale factor
    UINT32 srsz_scale_vc;                       // 0x1010 self-picture resize vertical chrominance scale factor
    UINT32 srsz_phase_hy;                       // 0x1014 self-picture resize horizontal luminance phase offset
    UINT32 srsz_phase_hc;                       // 0x1018 self-picture resize horizontal chrominance phase offset
    UINT32 srsz_phase_vy;                       // 0x101C self-picture resize vertical luminance phase offset
    UINT32 srsz_phase_vc;                       // 0x1020 self-picture resize vertical chrominance phase offset
    UINT32 srsz_ctrl_shd;                       // 0x1024 self-picture resize control register shadow register
    UINT32 srsz_scale_hy_shd;                   // 0x1028 self-picture resize horizontal luminance scale factor shadow register
    UINT32 srsz_scale_hc_shd;                   // 0x102C self-picture resize horizontal chrominance scale factor shadow register
    UINT32 srsz_scale_vy_shd;                   // 0x1030 self-picture resize vertical luminance scale factor shadow register
    UINT32 srsz_scale_vc_shd;                   // 0x1034 self-picture resize vertical chrominance scale factor shadow register
    UINT32 srsz_phase_hy_shd;                   // 0x1038 self-picture resize horizontal luminance phase offset shadow register
    UINT32 srsz_phase_hc_shd;                   // 0x103C self-picture resize horizontal chrominance phase offset shadow register
    UINT32 srsz_phase_vy_shd;                   // 0x1040 self-picture resize vertical luminance phase offset shadow register
    UINT32 srsz_phase_vc_shd;                   // 0x1044 self-picture resize vertical chrominance phase offset shadow register
    UINT32 notused_srsz[(0x1400-0x1048)/4];     // memory gap
  #endif  // MARVIN_FEATURE_SSCALE_UP
#else  // MARVIN_FEATURE_SSCALE
    UINT32 notused_srsz[(0x1400-0x1000)/4];     // memory gap
#endif // MARVIN_FEATURE_SSCALE

#if (MARVIN_FEATURE_SMALL_OUTUNIT == MARVIN_FEATURE_EXIST)
    UINT32 emp_out_ctrl; // global control register (rw) MRV_SOUT_BASE + 0x0000
//    UINT32 sml_out_ctrl;                        // 0x1400 small output control register
    UINT32 notused_mi3[(0x14C8-0x1404)/4];      // memory gap
    UINT32 emp_out_imsc; // Interrupt Mask (rw) MRV_SOUT_BASE + 0x00c8
    UINT32 emp_out_ris; // Raw Interrupt Status (r) MRV_SOUT_BASE + 0x00cc
    UINT32 emp_out_mis; // Masked Interrupt Status (r) MRV_SOUT_BASE + 0x00d0
    UINT32 emp_out_icr; // Interrupt Clear Register (w) MRV_SOUT_BASE + 0x00d4
    UINT32 emp_out_isr; // Interrupt Set Register (w) MRV_SOUT_BASE + 0x00d8
//    UINT32 sml_out_imsc;                        // 0x14C8 small output interrupt mask register
//    UINT32 sml_out_ris;                         // 0x14CC small output raw interrupt status
//    UINT32 sml_out_mis;                         // 0x14D0 small output masked interrupt status
//    UINT32 sml_out_icr;                         // 0x14D4 small output interrupt clear register
//    UINT32 sml_out_isr;                         // 0x14D8 small output interrupt set register
    UINT32 notused_mi4[(0x1600-0x14DC)/4];      // memory gap
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    UINT32 mi_ctrl;                             // 0x1600 memory interface control register
    UINT32 mi_init;                             // 0x1604 main picture initialization registers
    UINT32 mi_mp_y_base_ad_init;                // 0x1608 main picture Y component or JPEG ring buffer base address
    UINT32 mi_mp_y_size_init;                   // 0x160C main picture Y component or JPEG ring buffer size
    UINT32 mi_mp_y_offs_cnt_init;               // 0x1610 main picture Y component or JPEG start offset
    UINT32 mi_mp_y_offs_cnt_start;              // 0x1614 current main picture Y component or JPEG start offset
    UINT32 mi_mp_y_irq_offs_init;               // 0x1618 main picture Y component or JPEG ring buffer interrupt offset
    UINT32 mi_mp_cb_base_ad_init;               // 0x161C main picture Cb component ring buffer base address
    UINT32 mi_mp_cb_size_init;                  // 0x1620 main picture Cb component buffer size
    UINT32 mi_mp_cb_offs_cnt_init;              // 0x1624 main picture Cb component start offset
    UINT32 mi_mp_cb_offs_cnt_start;             // 0x1628 current main picture Cb component start offset
    UINT32 mi_mp_cr_base_ad_init;               // 0x162C main picture Cr component ring buffer base address
    UINT32 mi_mp_cr_size_init;                  // 0x1630 main picture Cr component buffer size
    UINT32 mi_mp_cr_offs_cnt_init;              // 0x1634 main picture Cr component start offset
    UINT32 mi_mp_cr_offs_cnt_start;             // 0x1638 current main picture Cr component start offset
    UINT32 mi_sp_y_base_ad_init;                // 0x163C self-picture picture Y component or JPEG ring buffer base address
    UINT32 mi_sp_y_size_init;                   // 0x1640 self-picture picture Y component or JPEG ring buffer size
    UINT32 mi_sp_y_offs_cnt_init;               // 0x1644 self-picture picture Y component or JPEG start offset
    UINT32 mi_sp_y_offs_cnt_start;              // 0x1648 current self-picture picture Y component or JPEG start offset
    UINT32 mi_sp_y_llength;                     // 0x164C self-picture line length
    UINT32 mi_sp_cb_base_ad_init;               // 0x1650 self-picture Cb component ring buffer base address
    UINT32 mi_sp_cb_size_init;                  // 0x1654 self-picture Cb component buffer size
    UINT32 mi_sp_cb_offs_cnt_init;              // 0x1658 self-picture Cb component start offset
    UINT32 mi_sp_cb_offs_cnt_start;             // 0x165C current main picture Cb component start offset
    UINT32 mi_sp_cr_base_ad_init;               // 0x1660 self-picture Cr component ring buffer base address
    UINT32 mi_sp_cr_size_init;                  // 0x1664 self-picture Cr component buffer size
    UINT32 mi_sp_cr_offs_cnt_init;              // 0x1668 self-picture Cr component start offset
    UINT32 mi_sp_cr_offs_cnt_start;             // 0x166C current main picture Cr component start offset
    UINT32 mi_byte_cnt;                         // 0x1670 byte counter for JPEG and raw data
    UINT32 mi_ctrl_shd;                         // 0x1674 MI control shadow register
    UINT32 mi_mp_y_base_ad_shd;                 // 0x1678 main picture Y component or JPEG ring buffer base address
    UINT32 mi_mp_y_size_shd;                    // 0x167C main picture Y component or JPEG ring buffer size shadow
    UINT32 mi_mp_y_offs_cnt_shd;                // 0x1680 main picture Y component or JPEG start offset shadow register
    UINT32 mi_mp_y_irq_offs_shd ;               // 0x1684 main picture Y component or JPEG ring buffer interrupt
    UINT32 mi_mp_cb_base_ad_shd;                // 0x1688 main picture Cb component ring buffer base address shadow register
    UINT32 mi_mp_cb_size_shd;                   // 0x168C main picture Cb component ring buffer size shadow register
    UINT32 mi_mp_cb_offs_cnt_shd;               // 0x1690 main picture Cb component ring buffer start offset shadow register
    UINT32 mi_mp_cr_base_ad_shd;                // 0x1694 main picture Cr component ring buffer base address shadow register
    UINT32 mi_mp_cr_size_shd;                   // 0x1698 main picture Cr component ring buffer size shadow register
    UINT32 mi_mp_cr_offs_cnt_shd;               // 0x169C main picture Cr component ring buffer start offset shadow register
    UINT32 mi_sp_y_base_ad_shd;                 // 0x16A0 self-picture picture Y component or JPEG ring buffer base address
    UINT32 mi_sp_y_size_shd;                    // 0x16A4 self-picture picture Y component or JPEG ring buffer size shadow
    UINT32 mi_sp_y_offs_cnt_shd;                // 0x16A8 self-picture picture Y component or JPEG start offset shadow register
    UINT32 mi_sp_llength_shd;                   // 0x16AC self-picture line length shadow register
    UINT32 mi_sp_cb_base_ad_shd;                // 0x16B0 self-picture Cb component ring buffer base address shadow register
    UINT32 mi_sp_cb_size_shd;                   // 0x16B4 self-picture Cb component buffer size shadow register
    UINT32 mi_sp_cb_offs_cnt_shd;               // 0x16B8 self-picture Cb component start offset shadow register
    UINT32 mi_sp_cr_base_ad_shd;                // 0x16BC self-picture Cr component ring buffer base address shadow register
    UINT32 mi_sp_cr_size_shd;                   // 0x16C0 self-picture Cr component buffer size shadow register
    UINT32 mi_sp_cr_offs_cnt_shd;               // 0x16C4 self-picture Cr component start offset shadow register
    UINT32 mi_imsc;                             // 0x16C8 MI interrupt mask register
    UINT32 mi_ris;                              // 0x16CC MI raw interrupt status
    UINT32 mi_mis;                              // 0x16D0 MI masked interrupt status
    UINT32 mi_icr;                              // 0x16D4 MI interrupt clear register
    UINT32 mi_isr;                              // 0x16D8 MI interrupt set register
    UINT32 notused_mi5[(0x1800-0x16DC)/4];      // memory gap
    #elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V4)
    UINT32 mi_ctrl; // Global control register (rw) MRV_SOUT_BASE + 0x0200 + 0x0000
    UINT32 mi_init; // Control register for address init and skip function (w) MRV_SOUT_BASE + 0x0200 + 0x0004
    UINT32 mi_mp_y_base_ad_init; // Base address for main picture Y component, JPEG or raw data (rw) MRV_SOUT_BASE + 0x0200 + 0x0008
    UINT32 mi_mp_y_size_init; // Size of main picture Y component, JPEG or raw data (rw) MRV_SOUT_BASE + 0x0200 + 0x000c
    UINT32 mi_mp_y_offs_cnt_init; // Offset counter init value for main picture Y, JPEG or raw data (rw) MRV_SOUT_BASE + 0x0200 + 0x0010
    UINT32 mi_mp_y_offs_cnt_start; // Offset counter start value for main picture Y, JPEG or raw data (r) MRV_SOUT_BASE + 0x0200 + 0x0014
    UINT32 mi_mp_y_irq_offs_init; // Fill level interrupt offset value for main picture Y, JPEG or raw data (rw) MRV_SOUT_BASE + 0x0200 + 0x0018
    UINT32 mi_mp_cb_base_ad_init; // Base address for main picture Cb component ring buffer (rw) MRV_SOUT_BASE + 0x0200 + 0x001c
    UINT32 mi_mp_cb_size_init; // Size of main picture Cb component ring buffer (rw) MRV_SOUT_BASE + 0x0200 + 0x0020
    UINT32 mi_mp_cb_offs_cnt_init; // Offset counter init value for main picture Cb component ring buffer (rw) MRV_SOUT_BASE + 0x0200 + 0x0024
    UINT32 mi_mp_cb_offs_cnt_start; // Offset counter start value for main picture Cb component ring buffer (r) MRV_SOUT_BASE + 0x0200 + 0x0028
    UINT32 mi_mp_cr_base_ad_init; // Base address for main picture Cr component ring buffer (rw) MRV_SOUT_BASE + 0x0200 + 0x002c
    UINT32 mi_mp_cr_size_init; // Size of main picture Cr component ring buffer (rw) MRV_SOUT_BASE + 0x0200 + 0x0030
    UINT32 mi_mp_cr_offs_cnt_init; // Offset counter init value for main picture Cr component ring buffer (rw) MRV_SOUT_BASE + 0x0200 + 0x0034
    UINT32 mi_mp_cr_offs_cnt_start; // Offset counter start value for main picture Cr component ring buffer (r) MRV_SOUT_BASE + 0x0200 + 0x0038
    UINT32 mi_sp_y_base_ad_init; // Base address for self picture Y component ring buffer (rw) MRV_SOUT_BASE + 0x0200 + 0x003c
    UINT32 mi_sp_y_size_init; // Size of self picture Y component ring buffer (rw) MRV_SOUT_BASE + 0x0200 + 0x0040
    UINT32 mi_sp_y_offs_cnt_init; // Offset counter init value for self picture Y component ring buffer (rw) MRV_SOUT_BASE + 0x0200 + 0x0044
    UINT32 mi_sp_y_offs_cnt_start; // Offset counter start value for self picture Y component ring buffer (r) MRV_SOUT_BASE + 0x0200 + 0x0048
    UINT32 mi_sp_y_llength; // Line length of self picture Y component (rw) MRV_SOUT_BASE + 0x0200 + 0x004c
    UINT32 mi_sp_cb_base_ad_init; // Base address for self picture Cb component ring buffer (rw) MRV_SOUT_BASE + 0x0200 + 0x0050
    UINT32 mi_sp_cb_size_init; // Size of self picture Cb component ring buffer (rw) MRV_SOUT_BASE + 0x0200 + 0x0054
    UINT32 mi_sp_cb_offs_cnt_init; // Offset counter init value for self picture Cb component ring buffer (rw) MRV_SOUT_BASE + 0x0200 + 0x0058
    UINT32 mi_sp_cb_offs_cnt_start; // Offset counter start value for self picture Cb component ring buffer (r) MRV_SOUT_BASE + 0x0200 + 0x005c
    UINT32 mi_sp_cr_base_ad_init; // Base address for self picture Cr component ring buffer (rw) MRV_SOUT_BASE + 0x0200 + 0x0060
    UINT32 mi_sp_cr_size_init; // Size of self picture Cr component ring buffer (rw) MRV_SOUT_BASE + 0x0200 + 0x0064
    UINT32 mi_sp_cr_offs_cnt_init; // Offset counter init value for self picture Cr component ring buffer (rw) MRV_SOUT_BASE + 0x0200 + 0x0068
    UINT32 mi_sp_cr_offs_cnt_start; // Offset counter start value for self picture Cr component ring buffer (r) MRV_SOUT_BASE + 0x0200 + 0x006c
    UINT32 mi_byte_cnt; // Counter value of JPEG or RAW data bytes (r) MRV_SOUT_BASE + 0x0200 + 0x0070
    UINT32 mi_ctrl_shd; // global control internal shadow register (r) MRV_SOUT_BASE + 0x0200 + 0x0074
    UINT32 mi_mp_y_base_ad_shd; // Base address shadow register for main picture Y component, JPEG or raw data ring buffer (r) MRV_SOUT_BASE + 0x0200 + 0x0078
    UINT32 mi_mp_y_size_shd; // Size shadow register of main picture Y component, JPEG or raw data (r) MRV_SOUT_BASE + 0x0200 + 0x007c
    UINT32 mi_mp_y_offs_cnt_shd; // Current offset counter of main picture Y component, JPEG or raw data ring buffer (r) MRV_SOUT_BASE + 0x0200 + 0x0080
    UINT32 mi_mp_y_irq_offs_shd; // Shadow register of fill level interrupt offset value for main picture Y component, JPEG or raw data (r) MRV_SOUT_BASE + 0x0200 + 0x0084
    UINT32 mi_mp_cb_base_ad_shd; // Base address shadow register for main picture Cb component ring buffer (r) MRV_SOUT_BASE + 0x0200 + 0x0088
    UINT32 mi_mp_cb_size_shd; // Size shadow register of main picture Cb component ring buffer (r) MRV_SOUT_BASE + 0x0200 + 0x008c
    UINT32 mi_mp_cb_offs_cnt_shd; // Current offset counter of main picture Cb component ring buffer (r) MRV_SOUT_BASE + 0x0200 + 0x0090
    UINT32 mi_mp_cr_base_ad_shd; // Base address shadow register for main picture Cr component ring buffer (r) MRV_SOUT_BASE + 0x0200 + 0x0094
    UINT32 mi_mp_cr_size_shd; // Size shadow register of main picture Cr component ring buffer (r) MRV_SOUT_BASE + 0x0200 + 0x0098
    UINT32 mi_mp_cr_offs_cnt_shd; // Current offset counter of main picture Cr component ring buffer (r) MRV_SOUT_BASE + 0x0200 + 0x009c
    UINT32 mi_sp_y_base_ad_shd; // Base address shadow register for self picture Y component ring buffer (r) MRV_SOUT_BASE + 0x0200 + 0x00a0
    UINT32 mi_sp_y_size_shd; // Size shadow register of self picture Y component ring buffer (r) MRV_SOUT_BASE + 0x0200 + 0x00a4
    UINT32 mi_sp_y_offs_cnt_shd; // Current offset counter of self picture Y component ring buffer (r) MRV_SOUT_BASE + 0x0200 + 0x00a8

    UINT32 notused_so1;                         // 0x16AC

    UINT32 mi_sp_cb_base_ad_shd; // Base address shadow register for self picture Cb component ring buffer (r) MRV_SOUT_BASE + 0x0200 + 0x00b0
    UINT32 mi_sp_cb_size_shd; // Size shadow register of self picture Cb component ring buffer (r) MRV_SOUT_BASE + 0x0200 + 0x00b4
    UINT32 mi_sp_cb_offs_cnt_shd; // Current offset counter of self picture Cb component ring buffer (r) MRV_SOUT_BASE + 0x0200 + 0x00b8
    UINT32 mi_sp_cr_base_ad_shd; // Base address shadow register for self picture Cr component ring buffer (r) MRV_SOUT_BASE + 0x0200 + 0x00bc
    UINT32 mi_sp_cr_size_shd; // Size shadow register of self picture Cr component ring buffer (r) MRV_SOUT_BASE + 0x0200 + 0x00c0
    UINT32 mi_sp_cr_offs_cnt_shd; // Current offset counter of self picture Cr component ring buffer (r) MRV_SOUT_BASE + 0x0200 + 0x00c4

    UINT32 mi_dma_y_pic_start_ad; // Y component image start address (rw) MRV_SOUT_BASE + 0x0200 + 0x00c8
    UINT32 mi_dma_y_pic_width; // Y component image width (rw) MRV_SOUT_BASE + 0x0200 + 0x00cc
    UINT32 mi_dma_y_llength; // Y component original line length (rw) MRV_SOUT_BASE + 0x0200 + 0x00d0
    UINT32 mi_dma_y_pic_size; // Y component image size (rw) MRV_SOUT_BASE + 0x0200 + 0x00d4
    UINT32 mi_dma_cb_pic_start_ad; // Cb component image start address (rw) MRV_SOUT_BASE + 0x0200 + 0x00d8

    UINT32 notused_so2[(0x16E8-0x16DC)/4];      // memory gap
    UINT32 mi_dma_cr_pic_start_ad; // Cr component image start address (rw) MRV_SOUT_BASE + 0x0200 + 0x00e8
    UINT32 notused_so3[(0x16F8-0x16EC)/4];      // memory gap
    UINT32 mi_imsc; // Interrupt Mask (‘1’: interrupt active, ‘0’: interrupt masked) (rw) MRV_SOUT_BASE + 0x0200 + 0x00f8
    UINT32 mi_ris; // Raw Interrupt Status (r) MRV_SOUT_BASE + 0x0200 + 0x00fc
    UINT32 mi_mis; // Masked Interrupt Status (r) MRV_SOUT_BASE + 0x0200 + 0x0100
    UINT32 mi_icr; // Interrupt Clear Register (w) MRV_SOUT_BASE + 0x0200 + 0x0104
    UINT32 mi_isr; // Interrupt Set Register (w) MRV_SOUT_BASE + 0x0200 + 0x0108

  #if (MARVIN_FEATURE_MI_STATUSFLAGS == MARVIN_FEATURE_EXIST)
    UINT32 mi_status; // MI Status Register (r) MRV_SOUT_BASE + 0x0200 + 0x010c
    UINT32 mi_status_clr; // MI Status Clear Register (w) MRV_SOUT_BASE + 0x0200 + 0x0110
  #else   // MARVIN_FEATURE_MI_STATUSFLAGS
    UINT32 notused_so5[(0x1714-0x170C)/4];      // memory gap
  #endif  // MARVIN_FEATURE_MI_STATUSFLAGS

    UINT32 mi_sp_y_pic_width; // Y component image width (rw) MRV_SOUT_BASE + 0x0200 + 0x0114
    UINT32 mi_sp_y_pic_height; // Y component image height (rw) MRV_SOUT_BASE + 0x0200 + 0x0118
    UINT32 mi_sp_y_pic_size; // Y component image size (rw) MRV_SOUT_BASE + 0x0200 + 0x011c
    UINT32 mi_dma_ctrl; // DMA control register (rw) MRV_SOUT_BASE + 0x0200 + 0x0120
    UINT32 mi_dma_start; // DMA start register (w) MRV_SOUT_BASE + 0x0200 + 0x0124
    UINT32 mi_dma_status; // DMA status register (r) MRV_SOUT_BASE + 0x0200 + 0x0128
    UINT32 notused_so6[(0x1800-0x172C)/4];      // memory gap
    #else // (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V4)
    #error "not supported MI version with small output unit"
    #endif // (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V4)
#else  // MARVIN_FEATURE_SMALL_OUTUNIT
    UINT32 mi_ctrl; // Global control register (rw) MRV_MI_BASE + 0x0000
    UINT32 mi_init; // Control register for address init and skip function (w) MRV_MI_BASE + 0x0004
    UINT32 mi_mp_y_base_ad_init; // Base address for main picture Y component, JPEG or raw data (rw) MRV_MI_BASE + 0x0008
//    UINT32 mi_mp_y_base_adr;                    // 0x1408 main picture Y component or JPEG ring buffer base address
    UINT32 mi_mp_y_size_init; // Size of main picture Y component, JPEG or raw data (rw) MRV_MI_BASE + 0x000c
//    UINT32 mi_mp_y_size;                        // 0x140C main picture Y component or JPEG ring buffer size
    UINT32 mi_mp_y_offs_cnt_init; // Offset counter init value for main picture Y, JPEG or raw data (rw) MRV_MI_BASE + 0x0010
//    UINT32 mi_mp_y_offs;                        // 0x1410 main picture Y component or JPEG start offset
    UINT32 mi_mp_y_offs_cnt_start; // Offset counter start value for main picture Y, JPEG or raw data (r) MRV_MI_BASE + 0x0014
//    UINT32 mi_mp_y_offs_start;                  // 0x1414 current main picture Y component or JPEG start offset
    UINT32 mi_mp_y_irq_offs_init; // Fill level interrupt offset value for main picture Y, JPEG or raw data (rw) MRV_MI_BASE + 0x0018
//    UINT32 mi_mp_irq_offs;                      // 0x1418 main picture Y component or JPEG ring buffer interrupt offset
    UINT32 mi_mp_cb_base_ad_init; // Base address for main picture Cb component ring buffer (rw) MRV_MI_BASE + 0x001c
//    UINT32 mi_mp_cb_base_adr;                   // 0x141C main picture Cb component ring buffer base address
    UINT32 mi_mp_cb_size_init; // Size of main picture Cb component ring buffer (rw) MRV_MI_BASE + 0x0020
//    UINT32 mi_mp_cb_size;                       // 0x1420 main picture Cb component buffer size
    UINT32 mi_mp_cb_offs_cnt_init; // Offset counter init value for main picture Cb component ring buffer (rw) MRV_MI_BASE + 0x0024
//    UINT32 mi_mp_cb_offs;                       // 0x1424 main picture Cb component start offset
    UINT32 mi_mp_cb_offs_cnt_start; // Offset counter start value for main picture Cb component ring buffer (r) MRV_MI_BASE + 0x0028
//    UINT32 mi_mp_cb_offs_start;                 // 0x1428 current main picture Cb component start offset
    UINT32 mi_mp_cr_base_ad_init; // Base address for main picture Cr component ring buffer (rw) MRV_MI_BASE + 0x002c
//    UINT32 mi_mp_cr_base_adr;                   // 0x142C main picture Cr component ring buffer base address
    UINT32 mi_mp_cr_size_init; // Size of main picture Cr component ring buffer (rw) MRV_MI_BASE + 0x0030
//    UINT32 mi_mp_cr_size;                       // 0x1430 main picture Cr component buffer size
    UINT32 mi_mp_cr_offs_cnt_init; // Offset counter init value for main picture Cr component ring buffer (rw) MRV_MI_BASE + 0x0034
//    UINT32 mi_mp_cr_offs;                       // 0x1434 main picture Cr component start offset
    UINT32 mi_mp_cr_offs_cnt_start; // Offset counter start value for main picture Cr component ring buffer (r) MRV_MI_BASE + 0x0038
//    UINT32 mi_mp_cr_offs_start;                 // 0x1438 current main picture Cr component start offset
    UINT32 mi_sp_y_base_ad_init; // Base address for self picture Y component ring buffer (rw) MRV_MI_BASE + 0x003c
//    UINT32 mi_sp_y_base_adr;                    // 0x143C self-picture picture Y component or JPEG ring buffer base address
    UINT32 mi_sp_y_size_init; // Size of self picture Y component ring buffer (rw) MRV_MI_BASE + 0x0040
//    UINT32 mi_sp_y_size;                        // 0x1440 self-picture picture Y component or JPEG ring buffer size
    UINT32 mi_sp_y_offs_cnt_init; // Offset counter init value for self picture Y component ring buffer (rw) MRV_MI_BASE + 0x0044
//    UINT32 mi_sp_y_offs;                        // 0x1444 self-picture picture Y component or JPEG start offset
    UINT32 mi_sp_y_offs_cnt_start; // Offset counter start value for self picture Y component ring buffer (r) MRV_MI_BASE + 0x0048
//    UINT32 mi_sp_y_offs_start;                  // 0x1448 current self-picture picture Y component or JPEG start offset
    UINT32 mi_sp_y_llength; // Line length of self picture Y component (rw) MRV_MI_BASE + 0x004c
//    UINT32 mi_sp_llength;                       // 0x144C self-picture line length
    UINT32 mi_sp_cb_base_ad_init; // Base address for self picture Cb component ring buffer (rw) MRV_MI_BASE + 0x0050
//    UINT32 mi_sp_cb_base_adr;                   // 0x1450 self-picture Cb component ring buffer base address
    UINT32 mi_sp_cb_size_init; // Size of self picture Cb component ring buffer (rw) MRV_MI_BASE + 0x0054
//    UINT32 mi_sp_cb_size;                       // 0x1454 self-picture Cb component buffer size
    UINT32 mi_sp_cb_offs_cnt_init; // Offset counter init value for self picture Cb component ring buffer (rw) MRV_MI_BASE + 0x0058
//    UINT32 mi_sp_cb_offs;                       // 0x1458 self-picture Cb component start offset
    UINT32 mi_sp_cb_offs_cnt_start; // Offset counter start value for self picture Cb component ring buffer (r) MRV_MI_BASE + 0x005c
//    UINT32 mi_sp_cb_offs_start;                 // 0x145C current main picture Cb component start offset
    UINT32 mi_sp_cr_base_ad_init; // Base address for self picture Cr component ring buffer (rw) MRV_MI_BASE + 0x0060
//    UINT32 mi_sp_cr_base_adr;                   // 0x1460 self-picture Cr component ring buffer base address
    UINT32 mi_sp_cr_size_init; // Size of self picture Cr component ring buffer (rw) MRV_MI_BASE + 0x0064
//    UINT32 mi_sp_cr_size;                       // 0x1464 self-picture Cr component buffer size
    UINT32 mi_sp_cr_offs_cnt_init; // Offset counter init value for self picture Cr component ring buffer (rw) MRV_MI_BASE + 0x0068
//    UINT32 mi_sp_cr_offs;                       // 0x1468 self-picture Cr component start offset
    UINT32 mi_sp_cr_offs_cnt_start; // Offset counter start value for self picture Cr component ring buffer (r) MRV_MI_BASE + 0x006c
//    UINT32 mi_sp_cr_offs_start;                 // 0x146C current main picture Cr component start offset
    UINT32 mi_byte_cnt; // Counter value of JPEG or RAW data bytes (r) MRV_MI_BASE + 0x0070
    UINT32 mi_ctrl_shd; // global control internal shadow register (r) MRV_MI_BASE + 0x0074
    UINT32 mi_mp_y_base_ad_shd; // Base address shadow register for main picture Y component, JPEG or raw data ring buffer (r) MRV_MI_BASE + 0x0078
//    UINT32 mi_mp_y_base_adr_shd;                // 0x1478 main picture Y component or JPEG ring buffer base address
    UINT32 mi_mp_y_size_shd; // Size shadow register of main picture Y component, JPEG or raw data (r) MRV_MI_BASE + 0x007c
    UINT32 mi_mp_y_offs_cnt_shd; // Current offset counter of main picture Y component, JPEG or raw data ring buffer (r) MRV_MI_BASE + 0x0080
//    UINT32 mi_mp_y_offs_shd;                    // 0x1480 main picture Y component or JPEG start offset shadow register
    UINT32 mi_mp_y_irq_offs_shd; // Shadow register of fill level interrupt offset value for main picture Y component, JPEG or raw data (r) MRV_MI_BASE + 0x0084
    UINT32 mi_mp_cb_base_ad_shd; // Base address shadow register for main picture Cb component ring buffer (r) MRV_MI_BASE + 0x0088
//    UINT32 mi_mp_cb_base_adr_shd;               // 0x1488 main picture Cb component ring buffer base address shadow register
    UINT32 mi_mp_cb_size_shd; // Size shadow register of main picture Cb component ring buffer (r) MRV_MI_BASE + 0x008c
    UINT32 mi_mp_cb_offs_cnt_shd; // Current offset counter of main picture Cb component ring buffer (r) MRV_MI_BASE + 0x0090
//    UINT32 mi_mp_cb_offs_shd;                   // 0x1490 main picture Cb component ring buffer start offset shadow register
    UINT32 mi_mp_cr_base_ad_shd; // Base address shadow register for main picture Cr component ring buffer (r) MRV_MI_BASE + 0x0094
//    UINT32 mi_mp_cr_base_adr_shd;               // 0x1494 main picture Cr component ring buffer base address shadow register
    UINT32 mi_mp_cr_size_shd; // Size shadow register of main picture Cr component ring buffer (r) MRV_MI_BASE + 0x0098
    UINT32 mi_mp_cr_offs_cnt_shd; // Current offset counter of main picture Cr component ring buffer (r) MRV_MI_BASE + 0x009c
//    UINT32 mi_mp_cr_offs_shd;                   // 0x149C main picture Cr component ring buffer start offset shadow register
    UINT32 mi_sp_y_base_ad_shd; // Base address shadow register for self picture Y component ring buffer (r) MRV_MI_BASE + 0x00a0
//    UINT32 mi_sp_y_base_adr_shd;                // 0x14A0 self-picture picture Y component or JPEG ring buffer base address
    UINT32 mi_sp_y_size_shd; // Size shadow register of self picture Y component ring buffer (r) MRV_MI_BASE + 0x00a4
    UINT32 mi_sp_y_offs_cnt_shd; // Current offset counter of self picture Y component ring buffer (r) MRV_MI_BASE + 0x00a8
//    UINT32 mi_sp_y_offs_shd;                    // 0x14A8 self-picture picture Y component or JPEG start offset shadow register

  #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    UINT32 notused_mi1;                         // 0x14AC
  #else   // MARVIN_FEATURE_MI
    UINT32 mi_sp_llength_shd;                   // 0x14AC self-picture line length shadow register
  #endif  // MARVIN_FEATURE_MI

    UINT32 mi_sp_cb_base_ad_shd; // Base address shadow register for self picture Cb component ring buffer (r) MRV_MI_BASE + 0x00b0
//    UINT32 mi_sp_cb_base_adr_shd;               // 0x14B0 self-picture Cb component ring buffer base address shadow register
    UINT32 mi_sp_cb_size_shd; // Size shadow register of self picture Cb component ring buffer (r) MRV_MI_BASE + 0x00b4
    UINT32 mi_sp_cb_offs_cnt_shd; // Current offset counter of self picture Cb component ring buffer (r) MRV_MI_BASE + 0x00b8
//    UINT32 mi_sp_cb_offs_shd;                   // 0x14B8 self-picture Cb component start offset shadow register
    UINT32 mi_sp_cr_base_ad_shd; // Base address shadow register for self picture Cr component ring buffer (r) MRV_MI_BASE + 0x00bc
//    UINT32 mi_sp_cr_base_adr_shd;               // 0x14BC self-picture Cr component ring buffer base address shadow register
    UINT32 mi_sp_cr_size_shd; // Size shadow register of self picture Cr component ring buffer (r) MRV_MI_BASE + 0x00c0
    UINT32 mi_sp_cr_offs_cnt_shd; // Current offset counter of self picture Cr component ring buffer (r) MRV_MI_BASE + 0x00c4
//    UINT32 mi_sp_cr_offs_shd;                   // 0x14C4 self-picture Cr component start offset shadow register

  #if ((MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V2) && \
       (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4))
    UINT32 mi_dma_y_pic_start_ad; // Y component image start address (rw) MRV_MI_BASE + 0x00c8
//    UINT32 mi_dma_y_pic_base_ad;                // 0x14C8 MI Y component image start address register
    UINT32 mi_dma_y_pic_width; // Y component image width (rw) MRV_MI_BASE + 0x00cc
    UINT32 mi_dma_y_llength; // Y component original line length (rw) MRV_MI_BASE + 0x00d0
    UINT32 mi_dma_y_pic_size; // Y component image size (rw) MRV_MI_BASE + 0x00d4
    UINT32 mi_dma_cb_pic_start_ad; // Cb component image start address (rw) MRV_MI_BASE + 0x00d8
//    UINT32 mi_dma_cb_pic_base_ad;               // 0x14D8 MI CB component image start address register

  #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    UINT32 notused_mi2[(0x14E8-0x14DC)/4];      // memory gap
    #else   // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)
    UINT32 mi_dma_cb_pic_width;                 // 0x14DC MI CB component image width register
    UINT32 mi_dma_cb_llength;                   // 0x14E0 MI CB component original line length register
    UINT32 mi_dma_cb_pic_size;                  // 0x14E4 MI CB component image size register
    #endif  // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)

    UINT32 mi_dma_cr_pic_start_ad; // Cr component image start address (rw) MRV_MI_BASE + 0x00e8
//    UINT32 mi_dma_cr_pic_base_ad;               // 0x14E8 MI CR component image start address register

  #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    UINT32 notused_mi3[(0x14F8-0x14EC)/4];      // memory gap
    #else   // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)
    UINT32 mi_dma_cr_pic_width;                 // 0x14EC MI CR component image width register
    UINT32 mi_dma_cr_llength;                   // 0x14F0 MI CR component original line length register
    UINT32 mi_dma_cr_pic_size;                  // 0x14F4 MI CR component image size register
    #endif  // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)

    UINT32 mi_imsc; // Interrupt Mask (‘1’: interrupt active, ‘0’: interrupt masked) (rw) MRV_MI_BASE + 0x00f8
    UINT32 mi_ris; // Raw Interrupt Status (r) MRV_MI_BASE + 0x00fc
    UINT32 mi_mis; // Masked Interrupt Status (r) MRV_MI_BASE + 0x0100
    UINT32 mi_icr; // Interrupt Clear Register (w) MRV_MI_BASE + 0x0104
    UINT32 mi_isr; // Interrupt Set Register (w) MRV_MI_BASE + 0x0108
  #else   // #if (MARVIN_FEATURE_MI >= MI_V2 && <= MI_V3)
    UINT32 mi_imsc;                             // 0x14C8 MI interrupt mask register
    UINT32 mi_ris;                              // 0x14CC MI raw interrupt status
    UINT32 mi_mis;                              // 0x14D0 MI masked interrupt status
    UINT32 mi_icr;                              // 0x14D4 MI interrupt clear register
    UINT32 mi_isr;                              // 0x14D8 MI interrupt set register
    UINT32 notused_mi4[(0x150C-0x14DC)/4];      // memory gap
  #endif  // #if (MARVIN_FEATURE_MI >= MI_V2 && <= MI_V3)

  #if (MARVIN_FEATURE_MI_STATUSFLAGS == MARVIN_FEATURE_EXIST)
    UINT32 mi_status; // MI Status Register (r) MRV_MI_BASE + 0x010c
    UINT32 mi_status_clr; // MI Status Clear Register (w) MRV_MI_BASE + 0x0110
  #else   // MARVIN_FEATURE_MI_STATUSFLAGS
    UINT32 notused_mi5[(0x1514-0x150C)/4];      // memory gap
  #endif  // MARVIN_FEATURE_MI_STATUSFLAGS

  #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    UINT32 mi_sp_y_pic_width; // Y component image width (rw) MRV_MI_BASE + 0x0114
    UINT32 mi_sp_y_pic_height; // Y component image height (rw) MRV_MI_BASE + 0x0118
    UINT32 mi_sp_y_pic_size; // Y component image size (rw) MRV_MI_BASE + 0x011c
    UINT32 mi_dma_ctrl; // DMA control register (rw) MRV_MI_BASE + 0x0120
    UINT32 mi_dma_start; // DMA start register (w) MRV_MI_BASE + 0x0124
    UINT32 mi_dma_status; // DMA status register (r) MRV_MI_BASE + 0x0128
  #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V4)
    UINT32 mi_pixel_cnt; // Counter value of JPEG or RAW data bytes (r) MRV_MI_BASE + 0x012C
    UINT32 notused_mi6[(0x1800-0x1530)/4];      // memory gap for MI V4
  #else   // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V4)
    UINT32 notused_mi6[(0x1800-0x152C)/4];      // memory gap for MI V3
  #endif
  #else   // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)
    UINT32 notused_mi6[(0x1800-0x1514)/4];      // memory gap for MI V1 and V2
  #endif  // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)
#endif  // MARVIN_FEATURE_SMALL_OUTUNIT

    UINT32 jpe_gen_header; // command to start stream header generation (w) MRV_JPE_BASE + 0x0000
    UINT32 jpe_encode; // Start command to start JFIF stream encoding (rw) MRV_JPE_BASE + 0x0004
#if ((MARVIN_FEATURE_JPE >= MARVIN_FEATURE_JPE_V2) && \
     (MARVIN_FEATURE_JPE <= MARVIN_FEATURE_JPE_V3))
    UINT32 jpe_init; // Automatic configuration update (INIT) (w) MRV_JPE_BASE + 0x0008
#else
    UINT32 notused_jpe1;                        // memory gap
#endif // MARVIN_FEATURE_JPE
    UINT32 jpe_y_scale_en; // Y value scaling control register (rw) MRV_JPE_BASE + 0x000c
    UINT32 jpe_cbcr_scale_en; // Cb/Cr value scaling control register (rw) MRV_JPE_BASE + 0x0010
    UINT32 jpe_table_flush; // header generation debug register (rw) MRV_JPE_BASE + 0x0014
//    UINT32 jpe_table_flush_en;                  // 0x1814 controls header data output
    UINT32 jpe_enc_hsize; // JPEG codec horizontal image size for encoding (rw) MRV_JPE_BASE + 0x0018
    UINT32 jpe_enc_vsize; // JPEG codec vertical image size for encoding (rw) MRV_JPE_BASE + 0x001c
    UINT32 jpe_pic_format; // JPEG picture encoding format (rw) MRV_JPE_BASE + 0x0020
    UINT32 jpe_restart_interval; // restart marker insertion register (rw) MRV_JPE_BASE + 0x0024
    UINT32 jpe_tq_y_select; // Q- table selector 0, quant. table for Y component (rw) MRV_JPE_BASE + 0x0028
    UINT32 jpe_tq_u_select; // Q- table selector 1, quant. table for U component (rw) MRV_JPE_BASE + 0x002c
    UINT32 jpe_tq_v_select; // Q- table selector 2, quant. table for V component (rw) MRV_JPE_BASE + 0x0030
    UINT32 jpe_dc_table_select; // Huffman table selector for DC values (rw) MRV_JPE_BASE + 0x0034
    UINT32 jpe_ac_table_select; // Huffman table selector for AC values (rw) MRV_JPE_BASE + 0x0038
    UINT32 jpe_table_data; // table programming register (w) MRV_JPE_BASE + 0x003c
    UINT32 jpe_table_id; // table programming select register (rw) MRV_JPE_BASE + 0x0040
    UINT32 jpe_tac0_len; // Huffman AC table 0 length (rw) MRV_JPE_BASE + 0x0044
    UINT32 jpe_tdc0_len; // Huffman DC table 0 length (rw) MRV_JPE_BASE + 0x0048
    UINT32 jpe_tac1_len; // Huffman AC table 1 length (rw) MRV_JPE_BASE + 0x004c
    UINT32 jpe_tdc1_len; // Huffman DC table 1 length (rw) MRV_JPE_BASE + 0x0050
    UINT32 notused_jpe2;                        // memory gap
    UINT32 jpe_encoder_busy; // encoder status flag (r) MRV_JPE_BASE + 0x0058
    UINT32 jpe_header_mode; // header mode definition (rw) MRV_JPE_BASE + 0x005c
    UINT32 jpe_encode_mode; // encode mode (r) MRV_JPE_BASE + 0x0060
    UINT32 jpe_debug; // debug information register (r) MRV_JPE_BASE + 0x0064
    UINT32 jpe_error_imr; // JPEG error interrupt mask register (rw) MRV_JPE_BASE + 0x0068
//    UINT32 jpe_error_imsc;                      // 0x1868 JPEG error interrupt mask register
    UINT32 jpe_error_ris; // JPEG error raw  interrupt status register (r) MRV_JPE_BASE + 0x006c
    UINT32 jpe_error_mis; // JPEG error masked interrupt status register (r) MRV_JPE_BASE + 0x0070
    UINT32 jpe_error_icr; // JPEG error interrupt set register (w) MRV_JPE_BASE + 0x0074
    UINT32 jpe_error_isr; // JPEG error interrupt clear register (w) MRV_JPE_BASE + 0x0078
    UINT32 jpe_status_imr; // JPEG status interrupt mask register (rw) MRV_JPE_BASE + 0x007c
//    UINT32 jpe_status_imsc;                     // 0x187C JPEG status interrupt mask register
    UINT32 jpe_status_ris; // JPEG status raw interrupt status register (r) MRV_JPE_BASE + 0x0080
    UINT32 jpe_status_mis; // JPEG status masked interrupt status register (r) MRV_JPE_BASE + 0x0084
    UINT32 jpe_status_icr; // JPEG status interrupt clear register (w) MRV_JPE_BASE + 0x0088
    UINT32 jpe_status_isr; // JPEG status interrupt set register (w) MRV_JPE_BASE + 0x008c
#if (MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V2)
    UINT32 jpe_config; // JPEG configuration register (w) MRV_JPE_BASE + 0x0090
    UINT32 notused_jpe3[(0x1A00-0x1894)/4];     // memory gap
#else // (MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V2)
    UINT32 notused_jpe3[(0x1A00-0x1890)/4];     // memory gap
#endif //(MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V2)
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || \
     (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP)      || \
     (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    UINT32 smia_ctrl; // global control register (rw) MRV_SMIA_BASE + 0x0000
    UINT32 smia_status; // global status register (r) MRV_SMIA_BASE + 0x0004
    UINT32 smia_imsc; // Interrupt mask (rw) MRV_SMIA_BASE + 0x0008
    UINT32 smia_ris; // Raw interrupt status (r) MRV_SMIA_BASE + 0x000c
    UINT32 smia_mis; // Masked interrupt status (r) MRV_SMIA_BASE + 0x0010
    UINT32 smia_icr; // Interrupt clear register (w) MRV_SMIA_BASE + 0x0014
    UINT32 smia_isr; // Interrupt set register (w) MRV_SMIA_BASE + 0x0018
    UINT32 smia_data_format_sel; // data format selector register (rw) MRV_SMIA_BASE + 0x001c
    UINT32 smia_sof_emb_data_lines; // start of frame embedded data lines register (rw) MRV_SMIA_BASE + 0x0020
  #if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || \
       (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP))
    UINT32 smia_emb_hstart; // embedded data hstart register (rw) MRV_SMIA_BASE + 0x0024
    UINT32 smia_emb_hsize; // embedded data hsize register (rw) MRV_SMIA_BASE + 0x0028
    UINT32 smia_emb_vstart; // embedded data vstart register (rw) MRV_SMIA_BASE + 0x002c
  #else  // #if ((MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP))
    UINT32 notused_smia1[(0x1A30-0x1A24)/4];    // memory gap
  #endif // #if ((MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP))
    UINT32 smia_num_lines; // image data lines register (rw) MRV_SMIA_BASE + 0x0030
    UINT32 smia_emb_data_fifo; // Embedded Data Fifo (r) MRV_SMIA_BASE + 0x0034
  #if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || \
       (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    UINT32 smia_fifo_fill_level; // Embedded Data FIFO Fill Level (rw) MRV_SMIA_BASE + 0x0038
    UINT32 notused_smia2[(0x1A40-0x1A3C)/4];     // memory gap
  #else  // #if ((MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_MIPI_EMP))
    UINT32 notused_smia2[(0x1A40-0x1A38)/4];     // memory gap
  #endif // #if ((MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_MIPI_EMP))
  #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    UINT32 smia_emb_hstart1; // embedded data horizontal start register (rw) MRV_SMIA_BASE + 0x0040
    UINT32 smia_emb_hsize1; // embedded data horizontal size register (rw) MRV_SMIA_BASE + 0x0044
    UINT32 smia_emb_vstart1; // embedded data vertical start register (rw) MRV_SMIA_BASE + 0x0048
    UINT32 smia_emb_vsize1; // embedded data vertical size register (rw) MRV_SMIA_BASE + 0x004c
    UINT32 smia_emb_hstart2; // embedded data horizontal start register (rw) MRV_SMIA_BASE + 0x0050
    UINT32 smia_emb_hsize2; // embedded data horizontal size register (rw) MRV_SMIA_BASE + 0x0054
    UINT32 smia_emb_vstart2; // embedded data vertical start register (rw) MRV_SMIA_BASE + 0x0058
    UINT32 smia_emb_vsize2; // embedded data vertical size register (rw) MRV_SMIA_BASE + 0x005c
  #else  // #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    UINT32 notused_smia3[(0x1A60-0x1A40)/4];     // memory gap
  #endif // #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    UINT32 notused_smia4[(0x1C00-0x1A60)/4];     // memory gap
#else  // #if ((MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP))
    UINT32 notused_smia[(0x1C00-0x1A00)/4];     // memory gap
#endif // #if ((MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP))

#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    UINT32 mipi_ctrl; // global control register (rw) MRV_MIPI_BASE + 0x0000
    UINT32 mipi_status; // global status register (r) MRV_MIPI_BASE + 0x0004
    UINT32 mipi_imsc; // Interrupt mask (rw) MRV_MIPI_BASE + 0x0008
    UINT32 mipi_ris; // Raw interrupt status (r) MRV_MIPI_BASE + 0x000c
    UINT32 mipi_mis; // Masked interrupt status (r) MRV_MIPI_BASE + 0x0010
    UINT32 mipi_icr; // Interrupt clear register (w) MRV_MIPI_BASE + 0x0014
    UINT32 mipi_isr; // Interrupt set register (w) MRV_MIPI_BASE + 0x0018
    UINT32 mipi_cur_data_id; // Current Data Identifier (r) MRV_MIPI_BASE + 0x001c
    UINT32 mipi_img_data_sel; // Image Data Selector (rw) MRV_MIPI_BASE + 0x0020
    UINT32 mipi_add_data_sel_1; // Additional Data Selector 1 (rw) MRV_MIPI_BASE + 0x0024
    UINT32 mipi_add_data_sel_2; // Additional Data Selector 2 (rw) MRV_MIPI_BASE + 0x0028
    UINT32 mipi_add_data_sel_3; // Additional Data Selector 3 (rw) MRV_MIPI_BASE + 0x002c
    UINT32 mipi_add_data_sel_4; // Additional Data Selector 4 (rw) MRV_MIPI_BASE + 0x0030
    UINT32 mipi_add_data_fifo; // Additional Data Fifo (r) MRV_MIPI_BASE + 0x0034
    UINT32 mipi_add_data_fill_level; // Additional Data FIFO Fill Level (rw) MRV_MIPI_BASE + 0x0038
  #if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    UINT32 mipi_compressed_mode; // controls processing of compressed raw data types (rw) MRV_MIPI_BASE + 0x003C
    UINT32 mipi_frame;           // provides information about frame number from short packets (r) MRV_MIPI_BASE + 0x0040
    UINT32 mipi_gen_short;       // provides information about generic short packets (r) MRV_MIPI_BASE + 0x0044
    UINT32 notused_mipi[(0x2000-0x1C48)/4];     // memory gap
  #else    
    UINT32 notused_mipi[(0x2000-0x1C3C)/4];     // memory gap
  #endif     
#else
    UINT32 notused_mipi[(0x2000-0x1C00)/4];     // memory gap
#endif

#if (MARVIN_FEATURE_AF_MEASURE == MARVIN_FEATURE_EXIST)
    UINT32 isp_afm_ctrl; // This is the control register for AF measurement unit (rw) MRV_AFM_BASE + 0x0000
    UINT32 isp_afm_lt_a; // Top Left corner of measure window A (rw) MRV_AFM_BASE + 0x0004
    UINT32 isp_afm_rb_a; // Bottom right corner of measure window A (rw) MRV_AFM_BASE + 0x0008
    UINT32 isp_afm_lt_b; // Top left corner of measure window B (rw) MRV_AFM_BASE + 0x000c
    UINT32 isp_afm_rb_b; // Bottom right corner of measure window B (rw) MRV_AFM_BASE + 0x0010
    UINT32 isp_afm_lt_c; // Top left corner of measure window C (rw) MRV_AFM_BASE + 0x0014
    UINT32 isp_afm_rb_c; // Bottom right corner of measure window C (rw) MRV_AFM_BASE + 0x0018
    UINT32 isp_afm_thres; // Threshold register (rw) MRV_AFM_BASE + 0x001c
    UINT32 isp_afm_var_shift; // Variable shift register (rw) MRV_AFM_BASE + 0x0020
    UINT32 isp_afm_sum_a; // Sharpness Value Status Register of Window A (r) MRV_AFM_BASE + 0x0024
    UINT32 isp_afm_sum_b; // Sharpness Value Status Register of Window B (r) MRV_AFM_BASE + 0x0028
    UINT32 isp_afm_sum_c; // Sharpness Value Status Register of Window C (r) MRV_AFM_BASE + 0x002c
    UINT32 isp_afm_lum_a; // Luminance Value Status Register of Window A (r) MRV_AFM_BASE + 0x0030
    UINT32 isp_afm_lum_b; // Luminance Value Status Register of Window B (r) MRV_AFM_BASE + 0x0034
    UINT32 isp_afm_lum_c; // Luminance Value Status Register of Window C (r) MRV_AFM_BASE + 0x0038
    UINT32 notused_ispafm[(0x2100-0x203C)/4];   // memory gap
#else   // MARVIN_FEATURE_AF_MEASURE
    UINT32 notused_ispafm[(0x2100-0x2000)/4];   // memory gap
#endif  // MARVIN_FEATURE_AF_MEASURE

#if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
    UINT32 isp_bp_ctrl; // Control register for bad pixel detection and correction (rw) MRV_BP_BASE + 0x0000
    UINT32 isp_bp_cfg1; // Thresholds for on-the-fly and table based detection (rw) MRV_BP_BASE + 0x0004
    UINT32 isp_bp_cfg2; // Deviation thresholds for on-the-fly detection / correction (rw) MRV_BP_BASE + 0x0008
    UINT32 isp_bp_number; // Number of entries for bad pixel table (table based correction) (rw) MRV_BP_BASE + 0x000c
    UINT32 isp_bp_table_addr; // TABLE Address for table-based correction algorithm (rwh) MRV_BP_BASE + 0x0010
    UINT32 isp_bp_table_data; // TABLE DATA register for read and write access of table RAM (rw) MRV_BP_BASE + 0x0014
    UINT32 isp_bp_new_number; // Number of entries in new bad pixel table (r) MRV_BP_BASE + 0x0018
    UINT32 isp_bp_new_table[MRV_ISP_BP_NEW_TABLE_ARR_SIZE]; // New bad pixel table of table-based detection algorithm (rw) 0x01C+n (n=0..7)
//    UINT32 isp_bp_new_table_0;                  // 0x211C
//    UINT32 isp_bp_new_table_1;                  // 0x2120
// ...
//    UINT32 isp_bp_new_table_7;                  // 0x2138
    UINT32 notused_ispbp[(0x2200-0x213C)/4];    // memory gap
#else // MARVIN_FEATURE_BAD_PIXEL
    UINT32 notused_ispbp[(0x2200-0x2100)/4];    // memory gap
#endif // MARVIN_FEATURE_BAD_PIXEL

#if (MARVIN_FEATURE_LENS_SHADING >= MARVIN_FEATURE_LSC_V1)
    UINT32 isp_lsc_ctrl; // Lens shade control (rw) MRV_LSC_BASE + 0x0000
    UINT32 isp_lsc_r_table_addr; // Table RAM Address for red component (rwh) MRV_LSC_BASE + 0x0004
    UINT32 isp_lsc_g_table_addr; // Table RAM Address for green component (rwh) MRV_LSC_BASE + 0x0008
    UINT32 isp_lsc_b_table_addr; // Table RAM Address for blue component (rwh) MRV_LSC_BASE + 0x000c
    UINT32 isp_lsc_r_table_data; // Sample table red (rw) MRV_LSC_BASE + 0x0010
    UINT32 isp_lsc_g_table_data; // Sample table green (rw) MRV_LSC_BASE + 0x0014
    UINT32 isp_lsc_b_table_data; // Sample table blue (rw) MRV_LSC_BASE + 0x0018
    UINT32 notused_isplsc1;                     // memory gap
    UINT32 isp_lsc_xgrad_01; // Gradient table x (rw) MRV_LSC_BASE + 0x0020
    UINT32 isp_lsc_xgrad_23; // Gradient table x (rw) MRV_LSC_BASE + 0x0024
    UINT32 isp_lsc_xgrad_45; // Gradient table x (rw) MRV_LSC_BASE + 0x0028
    UINT32 isp_lsc_xgrad_67; // Gradient table x (rw) MRV_LSC_BASE + 0x002c
    UINT32 isp_lsc_ygrad_01; // Gradient table y (rw) MRV_LSC_BASE + 0x0030
    UINT32 isp_lsc_ygrad_23; // Gradient table y (rw) MRV_LSC_BASE + 0x0034
    UINT32 isp_lsc_ygrad_45; // Gradient table y (rw) MRV_LSC_BASE + 0x0038
    UINT32 isp_lsc_ygrad_67; // Gradient table y (rw) MRV_LSC_BASE + 0x003c
    UINT32 isp_lsc_xsize_01; // Size table (rw) MRV_LSC_BASE + 0x0040
    UINT32 isp_lsc_xsize_23; // Size table (rw) MRV_LSC_BASE + 0x0044
    UINT32 isp_lsc_xsize_45; // Size table (rw) MRV_LSC_BASE + 0x0048
    UINT32 isp_lsc_xsize_67; // Size table (rw) MRV_LSC_BASE + 0x004c
    UINT32 isp_lsc_ysize_01; // Size table (rw) MRV_LSC_BASE + 0x0050
    UINT32 isp_lsc_ysize_23; // Size table (rw) MRV_LSC_BASE + 0x0054
    UINT32 isp_lsc_ysize_45; // Size table (rw) MRV_LSC_BASE + 0x0058
    UINT32 isp_lsc_ysize_67; // Size table (rw) MRV_LSC_BASE + 0x005c
    UINT32 notused_isplsc2[(0x2300-0x2260)/4];  // memory gap (with lens shading)
#else   // MARVIN_FEATURE_LENS_SHADING
    UINT32 notused_isplsc2[(0x2300-0x2200)/4];  // memory gap (without lens shading)
#endif  // MARVIN_FEATURE_LENS_SHADING

#if ((MARVIN_FEATURE_IMG_STABILIZATION >= MARVIN_FEATURE_IMG_STABILIZATION_V1) || \
     (MARVIN_FEATURE_IMG_STABILIZATION <= MARVIN_FEATURE_IMG_STABILIZATION_V2))
  #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)
    UINT32 isp_is_ctrl; // Image Stabilization Control Register (rw) MRV_IS_BASE + 0x0000
    UINT32 isp_is_recenter; // Recenter register (rw) MRV_IS_BASE + 0x0004
  #else   // #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)
    UINT32 notused_ispis1[(0x2308-0x2300)/4];   // memory gap (without image stabilization version 2)
  #endif  // #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)

    UINT32 isp_is_h_offs; // Horizontal offset of output window (rw) MRV_IS_BASE + 0x0008
    UINT32 isp_is_v_offs; // Vertical offset of output window (rw) MRV_IS_BASE + 0x000c
    UINT32 isp_is_h_size; // Output horizontal picture size (rw) MRV_IS_BASE + 0x0010
    UINT32 isp_is_v_size; // Output vertical picture size (rw) MRV_IS_BASE + 0x0014

  #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)
    UINT32 isp_is_max_dx; // Maximum Horizontal Displacement (rw) MRV_IS_BASE + 0x0018
    UINT32 isp_is_max_dy; // Maximum Vertical Displacement (rw) MRV_IS_BASE + 0x001c
    UINT32 isp_is_displace; // Camera displacement (rw) MRV_IS_BASE + 0x0020
  #else   // #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)
    UINT32 notused_ispis3[(0x2324-0x2318)/4];   // memory gap (without image stabilization version 2)
  #endif  // #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)

    UINT32 isp_is_h_offs_shd; // current horizontal offset of output window (shadow register) (r) MRV_IS_BASE + 0x0024
    UINT32 isp_is_v_offs_shd; // current vertical offset of output window (shadow register) (r) MRV_IS_BASE + 0x0028
    UINT32 isp_is_h_size_shd; // current output horizontal picture size (shadow register) (r) MRV_IS_BASE + 0x002c
    UINT32 isp_is_v_size_shd; // current output vertical picture size (shadow register) (r) MRV_IS_BASE + 0x0030
    UINT32 notused_ispis4[(0x2400-0x2334)/4];   // memory gap (with image stabilization)
#else   // MARVIN_FEATURE_IMG_STABILIZATION
    UINT32 notused_ispis4[(0x2400-0x2300)/4];   // memory gap (without image stabilization)
#endif  // MARVIN_FEATURE_IMG_STABILIZATION

#if ( (MARVIN_FEATURE_ISP_HISTOGRAM <= MARVIN_FEATURE_HIST_V3) && (MARVIN_FEATURE_ISP_HISTOGRAM >= MARVIN_FEATURE_HIST_V1))
    UINT32 isp_hist_prop; // Histogram properties (rw) MRV_HIST_BASE + 0x0000
    UINT32 isp_hist_h_offs; // Histogram window horizontal offset (rw) MRV_HIST_BASE + 0x0004
    UINT32 isp_hist_v_offs; // Histogram window vertical offset (rw) MRV_HIST_BASE + 0x0008
    UINT32 isp_hist_h_size; // histogram window horizontal size (rw) MRV_HIST_BASE + 0x000c
    UINT32 isp_hist_v_size; // histogram window vertical size (rw) MRV_HIST_BASE + 0x0010
    UINT32 isp_hist_bin[MRV_ISP_HIST_BIN_ARR_SIZE]; // histogram measurement result bin (rh) 0x014+n (n=0..15)
#if (MARVIN_FEATURE_ISP_HISTOGRAM == MARVIN_FEATURE_HIST_V3)
    UINT32 isp_hist_weight_00TO30; // histogram weight (rw) MRV_HIST_BASE + 0x0054
    UINT32 isp_hist_weight_40TO21; // histogram weight (rw) MRV_HIST_BASE + 0x0058
    UINT32 isp_hist_weight_31TO12; // histogram weight (rw) MRV_HIST_BASE + 0x005C
    UINT32 isp_hist_weight_22TO03; // histogram weight (rw) MRV_HIST_BASE + 0x0060
    UINT32 isp_hist_weight_13TO43; // histogram weight (rw) MRV_HIST_BASE + 0x0064
    UINT32 isp_hist_weight_04TO34; // histogram weight (rw) MRV_HIST_BASE + 0x0068
    UINT32 isp_hist_weight_44;     // histogram weight (rw) MRV_HIST_BASE + 0x006C
    UINT32 notused_isphist[(0x2500-0x2470)/4];  // memory gap (with histogram)
#else //(MARVIN_FEATURE_ISP_HISTOGRAM == MARVIN_FEATURE_HIST_V3)
    UINT32 notused_isphist[(0x2500-0x2454)/4];  // memory gap (with histogram)
#endif //(MARVIN_FEATURE_ISP_HISTOGRAM == MARVIN_FEATURE_HIST_V3)
#else   // MARVIN_FEATURE_ISP_HISTOGRAM
    UINT32 notused_isphist[(0x2500-0x2400)/4];  // memory gap (without histogram)
#endif  // MARVIN_FEATURE_ISP_HISTOGRAM

#if (MARVIN_FEATURE_ISP_FILTER == MARVIN_FEATURE_ISP_FILTER_V1)
    UINT32 isp_filt_mode; // Mode control for filter block (rw) MRV_FILT_BASE + 0x0000
    UINT32 isp_filt_mask_sharp_0; // Parameters for Sharp0 filter mask (rw) MRV_FILT_BASE + 0x0004
    UINT32 isp_filt_mask_sharp_1; // Parameters for sharp filter mask (rw) MRV_FILT_BASE + 0x0008
    UINT32 isp_filt_mask_sharp_diag; // Parameters for filter mask diagonal sharp mode (rw) MRV_FILT_BASE + 0x000c
    UINT32 isp_filt_mask_blur_max; // Parameters for maximum blur filter mask (rw) MRV_FILT_BASE + 0x0010
    UINT32 isp_filt_mask_blur; // Parameters for weak 2 dimensional blur filter mask (rw) MRV_FILT_BASE + 0x0014
    UINT32 isp_filt_mask_lin; // Parameters for one dimensional filter mask (rw) MRV_FILT_BASE + 0x0018
    UINT32 isp_filt_mask_orth; // Parameters for one dimensional orthogonal direction filter mask (rw) MRV_FILT_BASE + 0x001c
    UINT32 isp_filt_mask_diag; // Parameters for vertical filter mask in diagonal mode (rw) MRV_FILT_BASE + 0x0020
    UINT32 isp_filt_mask_h_diag; // Parameters for vertical filter mask in diagonal mode (rw) MRV_FILT_BASE + 0x0024
    UINT32 isp_filt_blur_th0; // Blurring threshold (rw) MRV_FILT_BASE + 0x0028
    UINT32 isp_filt_blur_th1; // Blurring threshold (rw) MRV_FILT_BASE + 0x002c
    UINT32 isp_filt_sharp0_th; // Sharpening threshold1 (rw) MRV_FILT_BASE + 0x0030
    UINT32 isp_filt_sharp1_th; // Sharpening threshold1 (rw) MRV_FILT_BASE + 0x0034
    UINT32 isp_filt_lum_weight; // Parameters for luminance weight function (rw) MRV_FILT_BASE + 0x0038
    UINT32 notused_ispfilt[(0x2600-0x253C)/4];  // memory gap (with filter)
#elif (MARVIN_FEATURE_ISP_FILTER == MARVIN_FEATURE_ISP_FILTER_V2)
    UINT32 isp_filt_mode; // mode control register for the filter block (rw) MRV_FILT_BASE + 0x0000
    UINT32 _notused_28 [(0x2528-0x2504)/4]; // gap in address space
//    UINT32 isp_filt_mask_sharp_0;               // 0x2504 reserved unused
//    UINT32 isp_filt_mask_sharp_1;               // 0x2508 reserved unused
//    UINT32 isp_filt_mask_sharp_diag;            // 0x250C reserved unused
//    UINT32 isp_filt_mask_blur_max;              // 0x2510 reserved unused
//    UINT32 isp_filt_mask_blur;                  // 0x2514 reserved unused
//    UINT32 isp_filt_mask_lin;                   // 0x2518 reserved unused
//    UINT32 isp_filt_mask_lin_orth;              // 0x251C reserved unused
//    UINT32 isp_filt_mask_diag;                  // 0x2520 reserved unused
//    UINT32 isp_filt_mask_h_diag;                // 0x2524 reserved unused
    UINT32 isp_filt_thresh_bl0; // Blurring threshold 0 (rw) MRV_FILT_BASE + 0x0028
//    UINT32 isp_filt_blur_th0;                   // 0x2528
    UINT32 isp_filt_thresh_bl1; // Blurring threshold 1 (rw) MRV_FILT_BASE + 0x002c
//    UINT32 isp_filt_blur_th1;                   // 0x252C
    UINT32 isp_filt_thresh_sh0; // Sharpening threshold 0 (rw) MRV_FILT_BASE + 0x0030
//    UINT32 isp_filt_sharp0_th;                  // 0x2530
    UINT32 isp_filt_thresh_sh1; // Sharpening threshold 1 (rw) MRV_FILT_BASE + 0x0034
//    UINT32 isp_filt_sharp1_th;                  // 0x2534
    UINT32 isp_filt_lum_weight; // Parameters for luminance weight function (rw) MRV_FILT_BASE + 0x0038
    UINT32 isp_filt_fac_sh1; // filter factor sharp1 (rw) MRV_FILT_BASE + 0x003c
//    UINT32 isp_filt_fac_sharp_1;                // 0x253C
    UINT32 isp_filt_fac_sh0; // filter factor sharp0 (rw) MRV_FILT_BASE + 0x0040
//    UINT32 isp_filt_fac_sharp_0;                // 0x2540
    UINT32 isp_filt_fac_mid; // filter factor midle (rw) MRV_FILT_BASE + 0x0044
    UINT32 isp_filt_fac_bl0; // Parameter for blur 0 filter (rw) MRV_FILT_BASE + 0x0048
//    UINT32 isp_filt_fac_blur;                   // 0x2548
    UINT32 isp_filt_fac_bl1; // Parameter for blur 1 filter (rw) MRV_FILT_BASE + 0x004c
//    UINT32 isp_filt_fac_blur_max;               // 0x254C
    UINT32 notused_ispfilt[(0x2580-0x2550)/4];  // memory gap (with filter)
#else   // MARVIN_FEATURE_ISP_FILTER
    UINT32 notused_ispfilt[(0x2580-0x2500)/4];  // memory gap (without filter)
#endif  // MARVIN_FEATURE_ISP_FILTER

#if (MARVIN_FEATURE_CHROM_ABERRATION == MARVIN_FEATURE_EXIST)
    UINT32 isp_cac_ctrl; // Control register for chromatic aberration correction (rw) MRV_CAC_BASE + 0x0000
    UINT32 isp_cac_count_start; // Preload values for CAC pixel and line counter (rw) MRV_CAC_BASE + 0x0004
    UINT32 isp_cac_a; // Linear Parameters for radial shift calculation (rw) MRV_CAC_BASE + 0x0008
    UINT32 isp_cac_b; // Square Parameters for radial shift calculation (rw) MRV_CAC_BASE + 0x000c
    UINT32 isp_cac_c; // Cubical Parameters for radial shift calculation (rw) MRV_CAC_BASE + 0x0010
    UINT32 isp_cac_x_norm; // Normalization parameters for calculation of image coordinate x_d relative to optical center (rw) MRV_CAC_BASE + 0x0014
    UINT32 isp_cac_y_norm; // Normalization parameters for calculation of image coordinate y_d relative to optical center (rw) MRV_CAC_BASE + 0x0018
    UINT32 notused_ispcac[(0x2600-0x259c)/4]; // gap in address space
#else // #if (MARVIN_FEATURE_CHROM_ABERRATION == MARVIN_FEATURE_EXIST)
    UINT32 notused_ispcac[(0x2600-0x2580)/4];  // memory gap (without filter)
#endif // #if (MARVIN_FEATURE_CHROM_ABERRATION == MARVIN_FEATURE_EXIST)

#if(   ( MARVIN_FEATURE_AUTO_EXPOSURE == MARVIN_FEATURE_AUTO_EXPOSURE_V2 )\
    || ( MARVIN_FEATURE_AUTO_EXPOSURE == MARVIN_FEATURE_AUTO_EXPOSURE_V4 ) )
    UINT32 isp_exp_ctrl; // Exposure control (rw) MRV_AE_BASE + 0x0000
    UINT32 isp_exp_h_offset; // Horizontal offset for first block (rw) MRV_AE_BASE + 0x0004
    UINT32 isp_exp_v_offset; // Vertical offset for first block (rw) MRV_AE_BASE + 0x0008
    UINT32 isp_exp_h_size; // Horizontal size of one block (rw) MRV_AE_BASE + 0x000c
    UINT32 isp_exp_v_size; // Vertical size of one block (rw) MRV_AE_BASE + 0x0010
    UINT32 isp_exp_mean_00; // Mean luminance value of block 00 (r) MRV_AE_BASE + 0x0014
    UINT32 isp_exp_mean_10; // Mean luminance value of block 10 (r) MRV_AE_BASE + 0x0018
    UINT32 isp_exp_mean_20; // Mean luminance value of block 20 (r) MRV_AE_BASE + 0x001c
    UINT32 isp_exp_mean_30; // Mean luminance value of block 30 (r) MRV_AE_BASE + 0x0020
    UINT32 isp_exp_mean_40; // Mean luminance value of block 40 (r) MRV_AE_BASE + 0x0024
    UINT32 isp_exp_mean_01; // Mean luminance value of block 01 (r) MRV_AE_BASE + 0x0028
    UINT32 isp_exp_mean_11; // Mean luminance value of block 11 (r) MRV_AE_BASE + 0x002c
    UINT32 isp_exp_mean_21; // Mean luminance value of block 21 (r) MRV_AE_BASE + 0x0030
    UINT32 isp_exp_mean_31; // Mean luminance value of block 31 (r) MRV_AE_BASE + 0x0034
    UINT32 isp_exp_mean_41; // Mean luminance value of block 41 (r) MRV_AE_BASE + 0x0038
    UINT32 isp_exp_mean_02; // Mean luminance value of block 02 (r) MRV_AE_BASE + 0x003c
    UINT32 isp_exp_mean_12; // Mean luminance value of block 12 (r) MRV_AE_BASE + 0x0040
    UINT32 isp_exp_mean_22; // Mean luminance value of block 22 (r) MRV_AE_BASE + 0x0044
    UINT32 isp_exp_mean_32; // Mean luminance value of block 32 (r) MRV_AE_BASE + 0x0048
    UINT32 isp_exp_mean_42; // Mean luminance value of block 42 (r) MRV_AE_BASE + 0x004c
    UINT32 isp_exp_mean_03; // Mean luminance value of block 03 (r) MRV_AE_BASE + 0x0050
    UINT32 isp_exp_mean_13; // Mean luminance value of block 13 (r) MRV_AE_BASE + 0x0054
    UINT32 isp_exp_mean_23; // Mean luminance value of block 23 (r) MRV_AE_BASE + 0x0058
    UINT32 isp_exp_mean_33; // Mean luminance value of block 33 (r) MRV_AE_BASE + 0x005c
    UINT32 isp_exp_mean_43; // Mean luminance value of block 43 (r) MRV_AE_BASE + 0x0060
    UINT32 isp_exp_mean_04; // Mean luminance value of block 04 (r) MRV_AE_BASE + 0x0064
    UINT32 isp_exp_mean_14; // Mean luminance value of block 14 (r) MRV_AE_BASE + 0x0068
    UINT32 isp_exp_mean_24; // Mean luminance value of block 24 (r) MRV_AE_BASE + 0x006c
    UINT32 isp_exp_mean_34; // Mean luminance value of block 34 (r) MRV_AE_BASE + 0x0070
    UINT32 isp_exp_mean_44; // Mean luminance value of block 44 (r) MRV_AE_BASE + 0x0074
    UINT32 notused_ispexp[(0x2700-0x2678)/4];   // memory gap (with filter)
#else   // MARVIN_FEATURE_AUTO_EXPOSURE: V2, V4
    UINT32 notused_ispexp[(0x2700-0x2600)/4];   // memory gap (without filter)
#endif  // MARVIN_FEATURE_AUTO_EXPOSURE: V2, V4

#if ((MARVIN_FEATURE_BLACK_LEVEL >= MARVIN_FEATURE_BLACK_LEVEL_V4) && \
     (MARVIN_FEATURE_BLACK_LEVEL <= MARVIN_FEATURE_BLACK_LEVEL_V5))
    UINT32 isp_bls_ctrl; // global control register (rw) MRV_BLS_BASE + 0x0000
    UINT32 isp_bls_samples; // samples register (rw) MRV_BLS_BASE + 0x0004
    UINT32 isp_bls_h1_start; // window 1 horizontal start (rw) MRV_BLS_BASE + 0x0008
    UINT32 isp_bls_h1_stop; // window 1 horizontal stop (rw) MRV_BLS_BASE + 0x000c
    UINT32 isp_bls_v1_start; // window 1 vertical start (rw) MRV_BLS_BASE + 0x0010
    UINT32 isp_bls_v1_stop; // window 1 vertical stop (rw) MRV_BLS_BASE + 0x0014
    UINT32 isp_bls_h2_start; // window 2 horizontal start (rw) MRV_BLS_BASE + 0x0018
    UINT32 isp_bls_h2_stop; // window 2 horizontal stop (rw) MRV_BLS_BASE + 0x001c
    UINT32 isp_bls_v2_start; // window 2 vertical start (rw) MRV_BLS_BASE + 0x0020
    UINT32 isp_bls_v2_stop; // window 2 vertical stop (rw) MRV_BLS_BASE + 0x0024
    UINT32 isp_bls_a_fixed; // fixed black level A (rw) MRV_BLS_BASE + 0x0028
    UINT32 isp_bls_b_fixed; // fixed black level B (rw) MRV_BLS_BASE + 0x002c
    UINT32 isp_bls_c_fixed; // fixed black level C (rw) MRV_BLS_BASE + 0x0030
    UINT32 isp_bls_d_fixed; // fixed black level D (rw) MRV_BLS_BASE + 0x0034
    UINT32 isp_bls_a_measured; // measured black level A (r) MRV_BLS_BASE + 0x0038
    UINT32 isp_bls_b_measured; // measured black level B (r) MRV_BLS_BASE + 0x003c
    UINT32 isp_bls_c_measured; // measured black level C (r) MRV_BLS_BASE + 0x0040
    UINT32 isp_bls_d_measured; // measured black level D (r) MRV_BLS_BASE + 0x0044
    UINT32 notused_ispbls2[(0x2800-0x2748)/4];  // memory gap (with BLS version 4 - 5)
#else   // MARVIN_FEATURE_BLACK_LEVEL
    UINT32 notused_ispbls2[(0x2800-0x2700)/4];  // memory gap (without BLS version 4 - 5)
#endif  // MARVIN_FEATURE_BLACK_LEVEL

/*---------------------------------------- D P F (begin) ----------------------------------------*/
#if( MARVIN_FEATURE_DPF == MARVIN_FEATURE_DPF_V1 )
    UINT32 isp_dpf_mode;             // Mode control for Denoising Pre-Filter block  (rw ) MRV_DPF_BASE + 0x0000
    UINT32 isp_dpf_strength_r;       // filter strength of the RED filter            (rw ) MRV_DPF_BASE + 0x0004
    UINT32 isp_dpf_strength_g;       // filter strength of the GREEN filter          (rw ) MRV_DPF_BASE + 0x0008
    UINT32 isp_dpf_strength_b;       // filter strength of the BLUE filter           (rw ) MRV_DPF_BASE + 0x000C
    UINT32 isp_dpf_s_weight_g_1_4;   // Spatial Weights green channel 1 2 3 4        (rw ) MRV_DPF_BASE + 0x0010
    UINT32 isp_dpf_s_weight_g_5_6;   // Spatial Weights green channel 5 6            (rw ) MRV_DPF_BASE + 0x0014
    UINT32 isp_dpf_s_weight_rb_1_4;  // Spatial Weights red/blue channels 1 2 3 4    (rw ) MRV_DPF_BASE + 0x0018
    UINT32 isp_dpf_s_weight_rb_5_6;  // Spatial Weights red/blue channels 5 6        (rw ) MRV_DPF_BASE + 0x001C
    UINT32 isp_dpf_nll_coeff_0;      // Noise Level Lookup Coefficient 0             (rw ) MRV_DPF_BASE + 0x0020
    UINT32 isp_dpf_nll_coeff_1;      // Noise Level Lookup Coefficient 1             (rw ) MRV_DPF_BASE + 0x0024
    UINT32 isp_dpf_nll_coeff_2;      // Noise Level Lookup Coefficient 2             (rw ) MRV_DPF_BASE + 0x0028
    UINT32 isp_dpf_nll_coeff_3;      // Noise Level Lookup Coefficient 3             (rw ) MRV_DPF_BASE + 0x002C
    UINT32 isp_dpf_nll_coeff_4;      // Noise Level Lookup Coefficient 4             (rw ) MRV_DPF_BASE + 0x0030
    UINT32 isp_dpf_nll_coeff_5;      // Noise Level Lookup Coefficient 5             (rw ) MRV_DPF_BASE + 0x0034
    UINT32 isp_dpf_nll_coeff_6;      // Noise Level Lookup Coefficient 6             (rw ) MRV_DPF_BASE + 0x0038
    UINT32 isp_dpf_nll_coeff_7;      // Noise Level Lookup Coefficient 7             (rw ) MRV_DPF_BASE + 0x003C
    UINT32 isp_dpf_nll_coeff_8;      // Noise Level Lookup Coefficient 8             (rw ) MRV_DPF_BASE + 0x0040
    UINT32 isp_dpf_nll_coeff_9;      // Noise Level Lookup Coefficient 9             (rw ) MRV_DPF_BASE + 0x0044
    UINT32 isp_dpf_nll_coeff_10;     // Noise Level Lookup Coefficient 10            (rw ) MRV_DPF_BASE + 0x0048
    UINT32 isp_dpf_nll_coeff_11;     // Noise Level Lookup Coefficient 11            (rw ) MRV_DPF_BASE + 0x004C
    UINT32 isp_dpf_nll_coeff_12;     // Noise Level Lookup Coefficient 12            (rw ) MRV_DPF_BASE + 0x0050
    UINT32 isp_dpf_nll_coeff_13;     // Noise Level Lookup Coefficient 13            (rw ) MRV_DPF_BASE + 0x0054
    UINT32 isp_dpf_nll_coeff_14;     // Noise Level Lookup Coefficient 14            (rw ) MRV_DPF_BASE + 0x0058
    UINT32 isp_dpf_nll_coeff_15;     // Noise Level Lookup Coefficient 15            (rw ) MRV_DPF_BASE + 0x005C
    UINT32 isp_dpf_nll_coeff_16;     // Noise Level Lookup Coefficient 16            (rw ) MRV_DPF_BASE + 0x0060
    UINT32 isp_dpf_nll_r_gain;       // Noise Level Red gain                         (rw ) MRV_DPF_BASE + 0x0064
    UINT32 isp_dpf_nll_gr_gain;      // Noise Level Green/Red gain                   (rw ) MRV_DPF_BASE + 0x0068
    UINT32 isp_dpf_nll_gb_gain;      // Noise Level Green/Blue gain                  (rw ) MRV_DPF_BASE + 0x006C
    UINT32 isp_dpf_nll_b_gain;       // Noise Level Blue gain                        (rw ) MRV_DPF_BASE + 0x0070
    UINT32 unused_ispdpf2[(0x2900-0x2874)/4]; // memory gap
#else
 #if (( MARVIN_FEATURE_DPCC == MARVIN_FEATURE_DPCC_V1 ) || ( MARVIN_FEATURE_WDR == MARVIN_FEATURE_WDR_V1 ) )
    UINT32 unused_ispdpf[(0x2900-0x2800)/4]; // memory gap
 #endif  
 #if ( MARVIN_FEATURE_DPF != MARVIN_FEATURE_EXIST_NOT )
 #error unknown value for MARVIN_FEATURE_DPF
 #endif  
#endif  // MARVIN_FEATURE_DPF
/*---------------------------------------- D P F (end) ----------------------------------------*/

/*---------------------------------------- D P C C (begin) ----------------------------------------*/
#if( MARVIN_FEATURE_DPCC == MARVIN_FEATURE_DPCC_V1 )
    UINT32 isp_dpcc_mode;           // Mode control for DPCC block                  (rw ) MRV_DPCC_BASE + 0x0000
    UINT32 isp_dpcc_output_mode;    // Output Interpolation mode                    (rw ) MRV_DPCC_BASE + 0x0004
    UINT32 isp_dpcc_set_use;        // DPCC parameter set use                       (rw ) MRV_DPCC_BASE + 0x0008
    UINT32 isp_dpcc_methods_set1;   // Methods enables for set 1                    (rw ) MRV_DPCC_BASE + 0x000C
    UINT32 isp_dpcc_methods_set2;   // Methods enables for set 2                    (rw ) MRV_DPCC_BASE + 0x0010
    UINT32 isp_dpcc_methods_set3;   // Methods enables for set 3                    (rw ) MRV_DPCC_BASE + 0x0014
    UINT32 isp_dpcc_line_thresh_1;  // Line threshold set 1                         (rw ) MRV_DPCC_BASE + 0x0018
    UINT32 isp_dpcc_line_mad_fac_1; // MAD factor for Line check set 1              (rw ) MRV_DPCC_BASE + 0x001C
    UINT32 isp_dpcc_pg_fac_1;       // Peak gradient factor for set 1               (rw ) MRV_DPCC_BASE + 0x0020
    UINT32 isp_dpcc_rnd_thresh_1;   // Rank Neighbor Difference threshold for set 1 (rw ) MRV_DPCC_BASE + 0x0024
    UINT32 isp_dpcc_rg_fac_1;       // Rank gradient factor for set 1               (rw ) MRV_DPCC_BASE + 0x0028
    UINT32 isp_dpcc_line_thresh_2;  // Line threshold set 2                         (rw ) MRV_DPCC_BASE + 0x002C
    UINT32 isp_dpcc_line_mad_fac_2; // MAD factor for Line check set 2              (rw ) MRV_DPCC_BASE + 0x0030
    UINT32 isp_dpcc_pg_fac_2;       // Peak gradient factor for set 2               (rw ) MRV_DPCC_BASE + 0x0034
    UINT32 isp_dpcc_rnd_thresh_2;   // Rank Neighbor Difference threshold for set 2 (rw ) MRV_DPCC_BASE + 0x0038
    UINT32 isp_dpcc_rg_fac_2;       // Rank gradient factor for set 2               (rw ) MRV_DPCC_BASE + 0x003C
    UINT32 isp_dpcc_line_thresh_3;  // Line threshold set 3                         (rw ) MRV_DPCC_BASE + 0x0040
    UINT32 isp_dpcc_line_mad_fac_3; // MAD factor for Line check set 3              (rw ) MRV_DPCC_BASE + 0x0044
    UINT32 isp_dpcc_pg_fac_3;       // Peak gradient factor for set 3               (rw ) MRV_DPCC_BASE + 0x0048
    UINT32 isp_dpcc_rnd_thresh_3;   // Rank Neighbor Difference threshold for set 3 (rw ) MRV_DPCC_BASE + 0x004C
    UINT32 isp_dpcc_rg_fac_3;       // Rank gradient factor for set 3               (rw ) MRV_DPCC_BASE + 0x0050
    UINT32 isp_dpcc_ro_limits;      // Rank Order Limits                            (rw ) MRV_DPCC_BASE + 0x0054
    UINT32 isp_dpcc_rnd_offs;       // Differential Rank Offsets                    (rw ) MRV_DPCC_BASE + 0x0058
    UINT32 isp_dpcc_bpt_ctrl;       // Bad Pixel Table Settings                     (rw ) MRV_DPCC_BASE + 0x005C
    UINT32 isp_dpcc_bpt_number;     // Number of entries for bad pixel table
                                    // (table based correction)                     (rw ) MRV_DPCC_BASE + 0x0060
    UINT32 isp_dpcc_bpt_addr;       // TABLE Start Address for table-based
                                    // correction algorithm                         (rwh) MRV_DPCC_BASE + 0x0064
    UINT32 isp_dpcc_bpt_data;       // TABLE DATA register for read and write
                                    // access of table RAM                          (rwh) MRV_DPCC_BASE + 0x0068
    UINT32 unused_ispdpcc2[(0x2A00-0x296C)/4]; // memory gap
#else
 #if ( MARVIN_FEATURE_WDR == MARVIN_FEATURE_WDR_V1 )
    UINT32 unused_ispdpcc[(0x2A00-0x2900)/4]; // memory gap
 #endif    
 #if ( MARVIN_FEATURE_DPCC != MARVIN_FEATURE_EXIST_NOT )
 #error unknown value for MARVIN_FEATURE_DPCC
 #endif  
#endif  // MARVIN_FEATURE_DPCC
/*---------------------------------------- D P C C (end) ----------------------------------------*/
/*---------------------------------------- W D R (begin) ----------------------------------------*/
  #if( MARVIN_FEATURE_WDR == MARVIN_FEATURE_WDR_V1 )   

    UINT32 isp_wdr_ctrl;                // Control Bits for Wide Dynamic Range Unit
    UINT32 isp_wdr_tonecurve_dyn_1;     // Tone Curve sample points dYn definition (part 4)
    UINT32 isp_wdr_tonecurve_dyn_2;     // Tone Curve sample points dYn definition (part 3)
    UINT32 isp_wdr_tonecurve_dyn_3;     // Tone Curve sample points dYn definition (part 2)
    UINT32 isp_wdr_tonecurve_dyn_4;     // Tone Curve sample points dYn definition (part 1)
    UINT32 isp_wdr_tonecurve_ym[33];    // Tonemapping curve coefficient n (n=0..32)  
    UINT32 isp_wdr_offset;              // Offset values for RGB path
    UINT32 isp_wdr_dmin_threshold;      // DeltaMin Threshold and Strength factor 
    UINT32 isp_wdr_tonecurve_dyn_1_shd; // Tone Curve sample points dYn definition shadow register(part 4)
    UINT32 isp_wdr_tonecurve_dyn_2_shd; // Tone Curve sample points dYn definition shadow register(part 3)
    UINT32 isp_wdr_tonecurve_dyn_3_shd; // Tone Curve sample points dYn definition shadow register(part 2)
    UINT32 isp_wdr_tonecurve_dyn_4_shd; // Tone Curve sample points dYn definition shadow register(part 1)
    UINT32 isp_wdr_tonecurve_ym_shd[33];// Tonemapping curve coefficient shadow register n (n=0..32)
    
    UINT32 unused_ispwdr2[(0x2C00-0x2B30)/4]; // memory gap
 #else //( MARVIN_FEATURE_WDR == MARVIN_FEATURE_WDR_V1 )  
 
  // if adjacent module !!!  UINT32 unused_ispwdr3[(0x2C00-0x2A00)/4]; // memory gap
  #if ( MARVIN_FEATURE_WDR != MARVIN_FEATURE_EXIST_NOT )
 #error unknown value for MARVIN_FEATURE_WDR
 #endif  

 #endif // ( MARVIN_FEATURE_WDR == MARVIN_FEATURE_WDR_V1 )  
/*---------------------------------------- W D R (end) ----------------------------------------*/
    
} tsMrvRegister;


// - MASK AND SHIFT MARCOS ----------------------------------------------------------

/********************************************************************/
/* Marvin Main Control Registers                                    */
/********************************************************************/

// Register: vi_ccl: Clock control register (0x0000)
//#define MRV_CCLFDIS              0x00000004 //[2] wr force disable module
// Slice: vi_cclfdis:
// Clock Control Logic disable
// 0: processing/cfg-clocks for all marvin sub modules enabled
// 1: processing/cfg-clocks for all marvin sub modules disabled w/o access to ID and VI_CCL register
#define MRV_VI_CCLFDIS
#define MRV_VI_CCLFDIS_MASK 0x00000004
#define MRV_VI_CCLFDIS_SHIFT 2
#define MRV_VI_CCLFDIS_ENABLE  0
#define MRV_VI_CCLFDIS_DISABLE 1
// Slice: vi_ccldiss:
// Status of vi_ccl[2] bit (copy of vi_ccl[2])
#define MRV_VI_CCLDISS
#define MRV_VI_CCLDISS_MASK 0x00000002
#define MRV_VI_CCLDISS_SHIFT 1

// Register: vi_id: Revision identification register (0x0008)
// Slice: rev_id:
// MARVIN5 revision IDs:
// M5_v1 id =  0x0015'3010
// M5_v2 id =  0x0025'3010
// M5_v3 id =  0x0035'3010
// M5_v5 id =  0x0015'3016
#define MRV_REV_ID
#define MRV_REV_ID_MASK 0xFFFFFFFF
#define MRV_REV_ID_SHIFT 0

// Register: vi_iccl: Internal clock  control register (0x0010)
//#define MRV_MI_CLK_EN            0x00000040 //[6] rw memory interface clock enable
//#define MRV_JPE_CLK_EN           0x00000020 //[5] rw JPEG encoder clock enable
//#define MRV_SRSZ_CLK_EN          0x00000010 //[4] rw self-picture resize clock enable
//#define MRV_MRSZ_CLK_EN          0x00000008 //[3] 0 main resize clock enable
//#define MRV_CP_CLK_EN            0x00000002 //[1] rw color processing clock enable
//#define MRV_ISP_CLK_EN           0x00000001 //[0] rw ISP clock enable
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    // Slice: vi_mipi_clk_enable:
    // MIPI interface clock enable
    // 1: processing mode
    // 0: power safe
    #define MRV_VI_MIPI_CLK_ENABLE
    #define MRV_VI_MIPI_CLK_ENABLE_MASK 0x00000800
    #define MRV_VI_MIPI_CLK_ENABLE_SHIFT 11
#else
    #define MRV_VI_MIPI_CLK_ENABLE_MASK 0
#endif
// Slice: vi_smia_clk_enable:
// SMIA interface clock enable
// 1: processing mode
// 0: power safe
#define MRV_VI_SMIA_CLK_ENABLE
#define MRV_VI_SMIA_CLK_ENABLE_MASK 0x00000400
#define MRV_VI_SMIA_CLK_ENABLE_SHIFT 10
#if (MARVIN_FEATURE_SUPERIMPOSE == MARVIN_FEATURE_EXIST)
    // Slice: vi_simp_clk_enable:
    // Superimpose clock enable
    // 1: processing mode
    // 0: power safe
    #define MRV_VI_SIMP_CLK_ENABLE
    #define MRV_VI_SIMP_CLK_ENABLE_MASK 0x00000200
    #define MRV_VI_SIMP_CLK_ENABLE_SHIFT 9
#else // MARVIN_FEATURE_SUPERIMPOSE
    #define MRV_VI_SIMP_CLK_ENABLE_MASK 0
#endif // MARVIN_FEATURE_SUPERIMPOSE
// Slice: vi_ie_clk_enable:
// Image effect clock enable
// 1: processing mode
// 0: power safe
#define MRV_VI_IE_CLK_ENABLE
#define MRV_VI_IE_CLK_ENABLE_MASK 0x00000100
#define MRV_VI_IE_CLK_ENABLE_SHIFT 8
#if (MARVIN_FEATURE_SMALL_OUTUNIT == MARVIN_FEATURE_EXIST)
    // Slice: vi_emp_clk_enable:
    // emp_out clock enable
    // 1: processing mode
    // 0: power safe
    #define MRV_VI_EMP_CLK_ENABLE
    #define MRV_VI_EMP_CLK_ENABLE_MASK 0x00000040
    #define MRV_VI_EMP_CLK_ENABLE_SHIFT 6
#else // MARVIN_FEATURE_SMALL_OUTUNIT
    #define MRV_VI_EMP_CLK_ENABLE_MASK 0
#endif // MARVIN_FEATURE_SMALL_OUTUNIT
#if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1)    // [hf]: ???
    // Slice: vi_mi_clk_enable:
    // memory interface clock enable
    // 1: processing mode
    // 0: power safe
    #define MRV_VI_MI_CLK_ENABLE
    #define MRV_VI_MI_CLK_ENABLE_MASK 0x00000040
    #define MRV_VI_MI_CLK_ENABLE_SHIFT 6
#else // MARVIN_FEATURE_MI
    #define MRV_VI_MI_CLK_ENABLE_MASK 0
#endif // MARVIN_FEATURE_MI
// Slice: vi_jpeg_clk_enable:
// JPEG encoder clock enable
// 1: processing mode
// 0: power safe
#define MRV_VI_JPEG_CLK_ENABLE
#define MRV_VI_JPEG_CLK_ENABLE_MASK 0x00000020
#define MRV_VI_JPEG_CLK_ENABLE_SHIFT 5
#if (MARVIN_FEATURE_SSCALE == MARVIN_FEATURE_EXIST)
    // Slice: vi_srsz_clk_enable:
    // main resize clock enable
    // 1: processing mode
    // 0: power safe
    #define MRV_VI_SRSZ_CLK_ENABLE
    #define MRV_VI_SRSZ_CLK_ENABLE_MASK 0x00000010
    #define MRV_VI_SRSZ_CLK_ENABLE_SHIFT 4
#else // MARVIN_FEATURE_SSCALE
    #define MRV_VI_SRSZ_CLK_ENABLE_MASK 0
#endif // MARVIN_FEATURE_SSCALE
// Slice: vi_mrsz_clk_enable:
// self-picture resize clock enable
// 1: processing mode
// 0: power safe
#define MRV_VI_MRSZ_CLK_ENABLE
#define MRV_VI_MRSZ_CLK_ENABLE_MASK 0x00000008
#define MRV_VI_MRSZ_CLK_ENABLE_SHIFT 3
// Slice: vi_cp_clk_enable:
// color processing clock enable
// 1: processing mode
// 0: power safe
#define MRV_VI_CP_CLK_ENABLE
#define MRV_VI_CP_CLK_ENABLE_MASK 0x00000002
#define MRV_VI_CP_CLK_ENABLE_SHIFT 1
// Slice: vi_isp_clk_enable:
// isp processing clock enable
// 1: processing mode
// 0: power safe
#define MRV_VI_ISP_CLK_ENABLE
#define MRV_VI_ISP_CLK_ENABLE_MASK 0x00000001
#define MRV_VI_ISP_CLK_ENABLE_SHIFT 0

// combination of all clock enable lines
#define MRV_VI_ALL_CLK_ENABLE
#define MRV_VI_ALL_CLK_ENABLE_MASK \
  (   0 \
    | MRV_VI_MIPI_CLK_ENABLE_MASK \
    | MRV_VI_SMIA_CLK_ENABLE_MASK \
    | MRV_VI_SIMP_CLK_ENABLE_MASK \
    | MRV_VI_IE_CLK_ENABLE_MASK \
    | MRV_VI_EMP_CLK_ENABLE_MASK \
    | MRV_VI_MI_CLK_ENABLE_MASK \
    | MRV_VI_JPEG_CLK_ENABLE_MASK \
    | MRV_VI_SRSZ_CLK_ENABLE_MASK \
    | MRV_VI_MRSZ_CLK_ENABLE_MASK \
    | MRV_VI_CP_CLK_ENABLE_MASK \
    | MRV_VI_ISP_CLK_ENABLE_MASK \
  )
#define MRV_VI_ALL_CLK_ENABLE_SHIFT 0

// Register: vi_ircl: Internal reset control register (0x0014)
//#define MRV_MIPI_SW_RST          0x00000800 //[11] rw MIPI interface software reset enable
//#define MRV_SMIA_SW_RST          0x00000400 //[10] rw SMIA interface software reset enable
//#define MRV_SI_SW_RST            0x00000200 //[9] rw super impose software reset enable
//#define MRV_IE_SW_RST            0x00000100 //[8] rw image effects software reset enable
//#define MRV_MRV_SW_RST           0x00000080 //[7] rw complete MARVIN reset enable (all modules are reset like HW reset)
//#define MRV_MI_SW_RST            0x00000040 //[6] rw memory interface software reset enable
//#define MRV_JPE_SW_RST           0x00000020 //[5] rw JPEG encoder software reset enable
//#define MRV_SRSZ_SW_RST          0x00000010 //[4] rw main resize software reset enable
//#define MRV_MRSZ_SW_RST          0x00000008 //[3] rw self-picture resize software reset enable
//#define MRV_YCS_SW_RST           0x00000004 //[2] rw yc-splitter software reset enable
//#define MRV_CP_SW_RST            0x00000002 //[1] rw color processing software reset enable
//#define MRV_ISP_SW_RST           0x00000001 //[0] rw ISP software reset enable
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    // Slice: vi_mipi_soft_rst:
    // MIPI Interface software reset
    // 0: processing mode
    // 1: reset state
    #define MRV_VI_MIPI_SOFT_RST
    #define MRV_VI_MIPI_SOFT_RST_MASK 0x00000800
    #define MRV_VI_MIPI_SOFT_RST_SHIFT 11
#else
    #define MRV_VI_MIPI_SOFT_RST_MASK 0
#endif
// Slice: vi_smia_soft_rst:
// SMIA Interface software reset
// 0: processing mode
// 1: reset state
#define MRV_VI_SMIA_SOFT_RST
#define MRV_VI_SMIA_SOFT_RST_MASK 0x00000400
#define MRV_VI_SMIA_SOFT_RST_SHIFT 10
#if (MARVIN_FEATURE_SUPERIMPOSE == MARVIN_FEATURE_EXIST)
    // Slice: vi_simp_soft_rst:
    // Superimpose software reset
    // 0: processing mode
    // 1: reset state
    #define MRV_VI_SIMP_SOFT_RST
    #define MRV_VI_SIMP_SOFT_RST_MASK 0x00000200
    #define MRV_VI_SIMP_SOFT_RST_SHIFT 9
#else // MARVIN_FEATURE_SUPERIMPOSE
    #define MRV_VI_SIMP_SOFT_RST_MASK 0
#endif // MARVIN_FEATURE_SUPERIMPOSE
// Slice: vi_ie_soft_rst:
// Image effect software reset
// 0: processing mode
// 1: reset state
#define MRV_VI_IE_SOFT_RST
#define MRV_VI_IE_SOFT_RST_MASK 0x00000100
#define MRV_VI_IE_SOFT_RST_SHIFT 8
// Slice: vi_marvin_rst:
// Soft reset of entire marvin
// 0: processing mode
// 1: reset state
#define MRV_VI_MARVIN_RST
#define MRV_VI_MARVIN_RST_MASK 0x00000080
#define MRV_VI_MARVIN_RST_SHIFT 7
#if (MARVIN_FEATURE_SMALL_OUTUNIT == MARVIN_FEATURE_EXIST)
    // Slice: vi_emp_soft_rst:
    // emp_out software reset
    // 0: processing mode
    // 1: reset state
    #define MRV_VI_EMP_SOFT_RST
    #define MRV_VI_EMP_SOFT_RST_MASK 0x00000040
    #define MRV_VI_EMP_SOFT_RST_SHIFT 6
#else // MARVIN_FEATURE_SMALL_OUTUNIT
    #define MRV_VI_EMP_SOFT_RST_MASK 0
#endif // MARVIN_FEATURE_SMALL_OUTUNIT
#if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1)    // [hf]: ???
    // Slice: vi_mi_soft_rst:
    // memory interface software reset
    // 0: processing mode
    // 1: reset state
    #define MRV_VI_MI_SOFT_RST
    #define MRV_VI_MI_SOFT_RST_MASK 0x00000040
    #define MRV_VI_MI_SOFT_RST_SHIFT 6
#else // MARVIN_FEATURE_MI
    #define MRV_VI_MI_SOFT_RST_MASK 0
#endif // MARVIN_FEATURE_MI
// Slice: vi_jpeg_soft_rst:
// JPEG encoder software reset
// 0: processing mode
// 1: reset state
#define MRV_VI_JPEG_SOFT_RST
#define MRV_VI_JPEG_SOFT_RST_MASK 0x00000020
#define MRV_VI_JPEG_SOFT_RST_SHIFT 5
#if (MARVIN_FEATURE_SSCALE == MARVIN_FEATURE_EXIST)
    // Slice: vi_srsz_soft_rst:
    // Self-picture resize software reset
    // 0: processing mode
    // 1: reset state
    #define MRV_VI_SRSZ_SOFT_RST
    #define MRV_VI_SRSZ_SOFT_RST_MASK 0x00000010
    #define MRV_VI_SRSZ_SOFT_RST_SHIFT 4
#else // MARVIN_FEATURE_SSCALE
    #define MRV_VI_SRSZ_SOFT_RST_MASK 0
#endif // MARVIN_FEATURE_SSCALE
// Slice: vi_mrsz_soft_rst:
// Main-picture resize software reset
// 0: processing mode
// 1: reset state
#define MRV_VI_MRSZ_SOFT_RST
#define MRV_VI_MRSZ_SOFT_RST_MASK 0x00000008
#define MRV_VI_MRSZ_SOFT_RST_SHIFT 3
// Slice: vi_ycs_soft_rst:
// y/c splitter software reset
// 0: processing mode
// 1: reset state
#define MRV_VI_YCS_SOFT_RST
#define MRV_VI_YCS_SOFT_RST_MASK 0x00000004
#define MRV_VI_YCS_SOFT_RST_SHIFT 2
// Slice: vi_cp_soft_rst:
// color processing software reset
// 0: processing mode
// 1: reset state
#define MRV_VI_CP_SOFT_RST
#define MRV_VI_CP_SOFT_RST_MASK 0x00000002
#define MRV_VI_CP_SOFT_RST_SHIFT 1
// Slice: vi_isp_soft_rst:
// isp software reset
// 0: processing mode
// 1: reset state
#define MRV_VI_ISP_SOFT_RST
#define MRV_VI_ISP_SOFT_RST_MASK 0x00000001
#define MRV_VI_ISP_SOFT_RST_SHIFT 0

// combination of all submodule soft reset lines
// (but not MRV_VI_MARVIN_RST)
#define MRV_VI_ALL_SOFT_RST
#define MRV_VI_ALL_SOFT_RST_MASK \
  (   0 \
    | MRV_VI_MIPI_SOFT_RST_MASK \
    | MRV_VI_SMIA_SOFT_RST_MASK \
    | MRV_VI_SIMP_SOFT_RST_MASK \
    | MRV_VI_IE_SOFT_RST_MASK \
    | MRV_VI_EMP_SOFT_RST_MASK \
    | MRV_VI_MI_SOFT_RST_MASK \
    | MRV_VI_JPEG_SOFT_RST_MASK \
    | MRV_VI_SRSZ_SOFT_RST_MASK \
    | MRV_VI_MRSZ_SOFT_RST_MASK \
    | MRV_VI_YCS_SOFT_RST_MASK \
    | MRV_VI_CP_SOFT_RST_MASK \
    | MRV_VI_ISP_SOFT_RST_MASK \
  )
#define MRV_VI_ALL_SOFT_RST_SHIFT 0

// Register: vi_dpcl: Data path control register (0x0018)
//#define MRV_YCS_CHN_MODE         0x0000000C //[3:2] rw Y/C splitter channel mode
//#define MRV_DP_SWITCH            0x00000003 //[1:0] rw data path selector
//#define MRV_DPCL_DMA_SPMUX    0x800 //[11]
//#define MRV_DPCL_DMA_IEMUX    0x400 //[10]
//#define MRV_DPCL_INIF__MASK   0x300 //[9:8] rw input interface selection
//#define MRV_DPCL_INIF_PAR     0x000 //0: parallel interface (legacy)
//#define MRV_DPCL_INIF_SMIA    0x100 //1: SMIA interface
//#define MRV_DPCL_INIF_MIPI    0x200 //2: MIPI interface
//#define MRV_DPCL_DMASW__MASK  0x70 //[6:4] rw DMA read path selector
//#define MRV_DPCL_DMASW_SELF   0x00 //0: direct path to self path mux
//#define MRV_DPCL_DMASW_SI     0x10 //1: path to superimpose block
//#define MRV_DPCL_DMASW_IE     0x20 //2: path to image effects block (i.e. replacement for data coming from the ISP)
//#define MRV_DPCL_DMASW_JPG    0x30 //3: direct path to JPEG encoder (R2B-buffer-less encodein mode)
//#define MRV_YCS__MASK      0x0C //[3:2] rw Y/C splitter channel mode
//#define MRV_YCS_OFF        0x00 //0: disabled
//#define MRV_YCS_Y          0xFF //not supported
//#define MRV_YCS_MV_RAW     0x04 //1: main path (and raw data) only mode
//#define MRV_YCS_MV         0x04 //
//#define MRV_YCS_SP         0x08 //2: self path only mode
//#define MRV_YCS_MV_SP      0x0C //3: main and self path mode
//#define MRV_DP__MASK       0x03 //[1:0] rw data path selector for main path
//#define MRV_DP_JPGDIRECT   0x00 //0: data from DMA-read port to the JPEG encoder
//#define MRV_DP_MV          0x01 //1: data from main resize to MI, uncompressed
//#define MRV_DP_RAW         0x01 //   (now also used for RAW data bypass)
//#define MRV_DP_JPEG        0x02 //2: data from main resize to JPEG encoder
// Slice: vi_dma_spmux:
// 0: data from camera interface to self resize
// 1: data from DMA read port to self resize
#define MRV_VI_DMA_SPMUX
#define MRV_VI_DMA_SPMUX_MASK 0x00000800
#define MRV_VI_DMA_SPMUX_SHIFT 11
#define MRV_VI_DMA_SPMUX_CAM    0 // 0: data from camera interface to self resize
#define MRV_VI_DMA_SPMUX_DMA    1 // 1: data from DMA read port to self resize
// Slice: vi_dma_iemux:
// 0: data from camera interface to image effects
// 1: data from DMA read port to image effects
#define MRV_VI_DMA_IEMUX
#define MRV_VI_DMA_IEMUX_MASK 0x00000400
#define MRV_VI_DMA_IEMUX_SHIFT 10
#define MRV_VI_DMA_IEMUX_CAM    0 // 0: data from camera interface to image effects
#define MRV_VI_DMA_IEMUX_DMA    1 // 1: data from DMA read port to image effects
// Slice: if_select:
// selects input interface
// 0: parallel interface
// 1: SMIA-interface
// 2: MIPI-interface
#define MRV_IF_SELECT
#define MRV_IF_SELECT_MASK 0x00000300
#define MRV_IF_SELECT_SHIFT 8
#define MRV_IF_SELECT_PAR   0 // 0: parallel interface
#define MRV_IF_SELECT_SMIA  1 // 1: SMIA-interface
#define MRV_IF_SELECT_MIPI  2 // 2: MIPI-interface
// Slice: vi_dma_switch:
// DMA read data path selector
// 0: path to SPMUX
// 1: path to Superimpose
// 2: path to Image Effects
// 3: reserved (for path to JPEG)
// 4..7: reserved
#define MRV_VI_DMA_SWITCH
#define MRV_VI_DMA_SWITCH_MASK 0x00000070
#define MRV_VI_DMA_SWITCH_SHIFT 4
#define MRV_VI_DMA_SWITCH_SELF  0 // 0: path to SPMUX
#define MRV_VI_DMA_SWITCH_SI    1 // 1: path to Superimpose
#define MRV_VI_DMA_SWITCH_IE    2 // 2: path to Image Effects
#define MRV_VI_DMA_SWITCH_JPG   3 // 3: reserved (for path to JPEG)
// Slice: vi_chan_mode:
// Y/C splitter channel mode
// 0: disabled
// 1: main path and raw data mode
// 2: self path mode
// 3: main and self path mode
#define MRV_VI_CHAN_MODE
#define MRV_VI_CHAN_MODE_MASK 0x0000000C
#define MRV_VI_CHAN_MODE_SHIFT 2
#if (MARVIN_FEATURE_DPMUX_YCSPLIT == MARVIN_FEATURE_YCS_V1)
    #define MRV_VI_CHAN_MODE_OFF     0xFF // not supported
    #define MRV_VI_CHAN_MODE_Y       0x02 // 2: 8bit data/Y only output
    #define MRV_VI_CHAN_MODE_MP_RAW  0x02 // 2: 8bit data/Y only output
    #define MRV_VI_CHAN_MODE_MP      0x00 // 0: separated 8bit Y, C main video output
    #define MRV_VI_CHAN_MODE_SP      0x01 // 1: separated 8bit Y, C main video and self-picture
    #define MRV_VI_CHAN_MODE_MP_SP   0x01 // 1: separated 8bit Y, C main video and self-picture
#elif (MARVIN_FEATURE_DPMUX_YCSPLIT == MARVIN_FEATURE_YCS_V2)
    #define MRV_VI_CHAN_MODE_OFF     0x00 // 0: disabled
    #define MRV_VI_CHAN_MODE_Y       0xFF // not supported
    #define MRV_VI_CHAN_MODE_MP_RAW  0x01 // 1: main path and raw data mode
    #define MRV_VI_CHAN_MODE_MP      0x01 // 1: main path and raw data mode
    #define MRV_VI_CHAN_MODE_SP      0x02 // 2: self path mode
    #define MRV_VI_CHAN_MODE_MP_SP   0x03 // 3: main and self path mode
#else
    #error "unknown setting for MARVIN_FEATURE_DPMUX_YCSPLIT"
#endif
// Slice: vi_mp_mux:
// data path selector for main path
// 00: reserved (future: data from DMA read port to JPEG encoder)
// 01: data from main resize to MI, uncompressed
// 10: data from main resize to JPEG encoder
// 11: reserved
#define MRV_VI_MP_MUX
#define MRV_VI_MP_MUX_MASK 0x00000003
#define MRV_VI_MP_MUX_SHIFT 0
#if (MARVIN_FEATURE_DPMUX_MAINPATH == MARVIN_FEATURE_DPMUX_MAIN_V1)
    #define MRV_VI_MP_MUX_JPGDIRECT  0xFF // not supported
    #define MRV_VI_MP_MUX_MP         0x01 // 01: main video path
    #define MRV_VI_MP_MUX_RAW        0x03 // 11: raw data bypass
    #define MRV_VI_MP_MUX_JPEG       0x02 // 10: JPEG encoding path
#elif (MARVIN_FEATURE_DPMUX_MAINPATH == MARVIN_FEATURE_DPMUX_MAIN_V2)
    #define MRV_VI_MP_MUX_JPGDIRECT  0x00 // 00: reserved (future: data from DMA read port to JPEG encoder)
    #define MRV_VI_MP_MUX_MP         0x01 // 01: data from main resize to MI, uncompressed
    #define MRV_VI_MP_MUX_RAW        0x01 //     (now also used for RAW data bypass)
    #define MRV_VI_MP_MUX_JPEG       0x02 // 10: data from main resize to JPEG encoder
#else
    #error "unknown setting for MARVIN_FEATURE_DPMUX_MAINPATH"
#endif

#if (MARVIN_FEATURE_CLOCK_DOMAINS == MARVIN_CLOCK_DOMAINS_2)
                                                //VI_TOC 32-bit
    #define MRV_TIME_OUT_STATUS      0x00000100 //[8] rh time-out status
    #define MRV_TIME_OUT_THRESH      0x000000FF //[7:0] rw time-out threshold value (determines maximum
#endif  // MARVIN_FEATURE_CLOCK_DOMAINS

#if ((MARVIN_FEATURE_DMA_READ == MARVIN_FEATURE_DMA_READ_V1) || \
     (MARVIN_FEATURE_DMA_READ == MARVIN_FEATURE_DMA_READ_V2))
                                                //VI_SPMCL 32-bit
    #define MRV_SP_DMA_SEL_SHD       0x00000100 //[8] rh shadow dma select status
    #define MRV_SPMCL_SOFT_UPD       0x00000002 //[1] rw soft update
    #define MRV_SP_DMA_SEL           0x00000001 //[0] rw time-out status
#endif  // MARVIN_FEATURE_DMA_READ


/********************************************************************/
/* Image Effects Registers                                          */
/********************************************************************/

// Register: img_eff_ctrl: Global control register (0x0000)
#if (MARVIN_FEATURE_IMAGE_EFFECTS >= MARVIN_FEATURE_IE_V2)
// Slice: full_range:
// write ‘0’: pixel value range according to BT.601
// write ‘1’: YCbCr full range 0...255
// read: always ‘0’
#define MRV_IMGEFF_FULL_RANGE
#define MRV_IMGEFF_FULL_RANGE_MASK 0x00000020
#define MRV_IMGEFF_FULL_RANGE_SHIFT 5
#define MRV_IMGEFF_FULL_RANGE_FULL    1 // 1: YCbCr full range 0...255
#define MRV_IMGEFF_FULL_RANGE_BT601   0 // 0: pixel value range according to BT.601
#endif // (MARVIN_FEATURE_IMAGE_EFFECTS >= MARVIN_FEATURE_IE_V2)
// Slice: cfg_upd:
// write ‘0’: nothing happens
// write ‘1’: update shadow registers
// read: always ‘0’
#define MRV_IMGEFF_CFG_UPD
#define MRV_IMGEFF_CFG_UPD_MASK 0x00000010
#define MRV_IMGEFF_CFG_UPD_SHIFT 4
// Slice: effect_mode:
// effect mode
// 000: black & white effect (grayscale)
// 001: negative effect
// 010: sepia effect
// 011: color selection effect
// 100: emboss effect
// 101: sketch effect
// 110…111: not used
#define MRV_IMGEFF_EFFECT_MODE
#define MRV_IMGEFF_EFFECT_MODE_MASK 0x0000000E
#define MRV_IMGEFF_EFFECT_MODE_SHIFT 1
#define MRV_IMGEFF_EFFECT_MODE_GRAY      0 // 000: black & white effect (grayscale)
#define MRV_IMGEFF_EFFECT_MODE_NEGATIVE  1 // 001: negative effect
#define MRV_IMGEFF_EFFECT_MODE_SEPIA     2 // 010: sepia effect
#define MRV_IMGEFF_EFFECT_MODE_COLOR_SEL 3 // 011: color selection effect
#define MRV_IMGEFF_EFFECT_MODE_EMBOSS    4 // 100: emboss effect
#define MRV_IMGEFF_EFFECT_MODE_SKETCH    5 // 101: sketch effect
#define MRV_IMGEFF_EFFECT_MODE_SHARPEN   6 // 110: sharpen effect
// Slice: bypass_mode:
// bypass mode
// 1: processing is activated
// 0: processing is deactivated, bypass mode is selected
#define MRV_IMGEFF_BYPASS_MODE
#define MRV_IMGEFF_BYPASS_MODE_MASK 0x00000001
#define MRV_IMGEFF_BYPASS_MODE_SHIFT 0
#define MRV_IMGEFF_BYPASS_MODE_PROCESS  1 // 1: processing is activated
#define MRV_IMGEFF_BYPASS_MODE_BYPASS   0 // 0: processing is deactivated, bypass mode is selected

// Register: img_eff_color_sel: Color selection register (for color selection effect) (0x0004)
// Slice: color_threshold:
// Threshold value of the RGB colors for the color selection effect.
#define MRV_IMGEFF_COLOR_THRESHOLD
#define MRV_IMGEFF_COLOR_THRESHOLD_MASK 0x0000FF00
#define MRV_IMGEFF_COLOR_THRESHOLD_SHIFT 8
// Slice: color_selection:
// Defining the maintained color:
// 000: red green and blue
// 001: blue
// 010: green
// 011: green and blue
// 100: red
// 101: red and blue
// 110: red and green
// 111: red green and blue
#define MRV_IMGEFF_COLOR_SELECTION
#define MRV_IMGEFF_COLOR_SELECTION_MASK 0x00000007
#define MRV_IMGEFF_COLOR_SELECTION_SHIFT 0
#define MRV_IMGEFF_COLOR_SELECTION_RGB  0 // 000: red green and blue
#define MRV_IMGEFF_COLOR_SELECTION_B    1 // 001: blue
#define MRV_IMGEFF_COLOR_SELECTION_G    2 // 010: green
#define MRV_IMGEFF_COLOR_SELECTION_BG   3 // 011: green and blue
#define MRV_IMGEFF_COLOR_SELECTION_R    4 // 100: red
#define MRV_IMGEFF_COLOR_SELECTION_BR   5 // 101: red and blue
#define MRV_IMGEFF_COLOR_SELECTION_GR   6 // 110: red and green
#define MRV_IMGEFF_COLOR_SELECTION_BGR  7 // 111: red green and blue

// Register: img_eff_mat_1: 3x3 matrix coefficients for emboss effect (1) (0x0008)
// Slice: emb_coef_21_en:
// 0: entry not used (coefficient is zero)
// 1: entry used
#define MRV_IMGEFF_EMB_COEF_21_EN
#define MRV_IMGEFF_EMB_COEF_21_EN_MASK 0x00008000
#define MRV_IMGEFF_EMB_COEF_21_EN_SHIFT 15
// Slice: emb_coef_21:
// second line, left entry of 3x3 emboss effect matrix, 2 bit for coefficient, one sign bit.
// coefficients:
// 000 (1), 001(2), 010(4), 011(8)
// 100 (-1), 101(-2), 110(-4), 111(-8)
#define MRV_IMGEFF_EMB_COEF_21
#define MRV_IMGEFF_EMB_COEF_21_MASK 0x00007000
#define MRV_IMGEFF_EMB_COEF_21_SHIFT 12
// combination of value and enable bit
#define MRV_IMGEFF_EMB_COEF_21_4
#define MRV_IMGEFF_EMB_COEF_21_4_MASK 0x0000F000
#define MRV_IMGEFF_EMB_COEF_21_4_SHIFT 12
// Slice: emb_coef_13_en:
// 0: entry not used (coefficient is zero)
// 1: entry used
#define MRV_IMGEFF_EMB_COEF_13_EN
#define MRV_IMGEFF_EMB_COEF_13_EN_MASK 0x00000800
#define MRV_IMGEFF_EMB_COEF_13_EN_SHIFT 11
// Slice: emb_coef_13:
// first line, right entry of 3x3 emboss effect matrix, 2 bit for coefficient, one sign bit.
// coefficients:
// 000 (1), 001(2), 010(4), 011(8)
// 100 (-1), 101(-2), 110(-4), 111(-8)
#define MRV_IMGEFF_EMB_COEF_13
#define MRV_IMGEFF_EMB_COEF_13_MASK 0x00000700
#define MRV_IMGEFF_EMB_COEF_13_SHIFT 8
// combination of value and enable bit
#define MRV_IMGEFF_EMB_COEF_13_4
#define MRV_IMGEFF_EMB_COEF_13_4_MASK 0x00000F00
#define MRV_IMGEFF_EMB_COEF_13_4_SHIFT 8
// Slice: emb_coef_12_en:
// 0: entry not used (coefficient is zero)
// 1: entry used
#define MRV_IMGEFF_EMB_COEF_12_EN
#define MRV_IMGEFF_EMB_COEF_12_EN_MASK 0x00000080
#define MRV_IMGEFF_EMB_COEF_12_EN_SHIFT 7
// Slice: emb_coef_12:
// first line, middle entry of 3x3 emboss effect matrix, 2 bit for coefficient, one sign bit.
// coefficients:
// 000 (1), 001(2), 010(4), 011(8)
// 100 (-1), 101(-2), 110(-4), 111(-8)
#define MRV_IMGEFF_EMB_COEF_12
#define MRV_IMGEFF_EMB_COEF_12_MASK 0x00000070
#define MRV_IMGEFF_EMB_COEF_12_SHIFT 4
// combination of value and enable bit
#define MRV_IMGEFF_EMB_COEF_12_4
#define MRV_IMGEFF_EMB_COEF_12_4_MASK 0x000000F0
#define MRV_IMGEFF_EMB_COEF_12_4_SHIFT 4
// Slice: emb_coef_11_en:
// 0: entry not used (coefficient is zero)
// 1: entry used
#define MRV_IMGEFF_EMB_COEF_11_EN
#define MRV_IMGEFF_EMB_COEF_11_EN_MASK 0x00000008
#define MRV_IMGEFF_EMB_COEF_11_EN_SHIFT 3
// Slice: emb_coef_11:
// first line, left entry of 3x3 emboss effect matrix, 2 bit for coefficient, one sign bit.
// coefficients:
// 000 (1), 001(2), 010(4), 011(8)
// 100 (-1), 101(-2), 110(-4), 111(-8)
#define MRV_IMGEFF_EMB_COEF_11
#define MRV_IMGEFF_EMB_COEF_11_MASK 0x00000007
#define MRV_IMGEFF_EMB_COEF_11_SHIFT 0
// combination of value and enable bit
#define MRV_IMGEFF_EMB_COEF_11_4
#define MRV_IMGEFF_EMB_COEF_11_4_MASK 0x0000000F
#define MRV_IMGEFF_EMB_COEF_11_4_SHIFT 0

// Register: img_eff_mat_2: 3x3 matrix coefficients for emboss effect (2) (0x000c)
// Slice: emb_coef_32_en:
// 0: entry not used (coefficient is zero)
// 1: entry used
#define MRV_IMGEFF_EMB_COEF_32_EN
#define MRV_IMGEFF_EMB_COEF_32_EN_MASK 0x00008000
#define MRV_IMGEFF_EMB_COEF_32_EN_SHIFT 15
// Slice: emb_coef_32:
// third line, middle entry of 3x3 emboss effect matrix, 2 bit for coefficient, one sign bit.
// coefficients:
// 000 (1), 001(2), 010(4), 011(8)
// 100 (-1), 101(-2), 110(-4), 111(-8)
#define MRV_IMGEFF_EMB_COEF_32
#define MRV_IMGEFF_EMB_COEF_32_MASK 0x00007000
#define MRV_IMGEFF_EMB_COEF_32_SHIFT 12
// combination of value and enable bit
#define MRV_IMGEFF_EMB_COEF_32_4
#define MRV_IMGEFF_EMB_COEF_32_4_MASK 0x0000F000
#define MRV_IMGEFF_EMB_COEF_32_4_SHIFT 12
// Slice: emb_coef_31_en:
// 0: entry not used (coefficient is zero)
// 1: entry used
#define MRV_IMGEFF_EMB_COEF_31_EN
#define MRV_IMGEFF_EMB_COEF_31_EN_MASK 0x00000800
#define MRV_IMGEFF_EMB_COEF_31_EN_SHIFT 11
// Slice: emb_coef_31:
// third line, left entry of 3x3 emboss effect matrix, 2 bit for coefficient, one sign bit.
// coefficients:
// 000 (1), 001(2), 010(4), 011(8)
// 100 (-1), 101(-2), 110(-4), 111(-8)
#define MRV_IMGEFF_EMB_COEF_31
#define MRV_IMGEFF_EMB_COEF_31_MASK 0x00000700
#define MRV_IMGEFF_EMB_COEF_31_SHIFT 8
// combination of value and enable bit
#define MRV_IMGEFF_EMB_COEF_31_4
#define MRV_IMGEFF_EMB_COEF_31_4_MASK 0x00000F00
#define MRV_IMGEFF_EMB_COEF_31_4_SHIFT 8
// Slice: emb_coef_23_en:
// 0: entry not used (coefficient is zero)
// 1: entry used
#define MRV_IMGEFF_EMB_COEF_23_EN
#define MRV_IMGEFF_EMB_COEF_23_EN_MASK 0x00000080
#define MRV_IMGEFF_EMB_COEF_23_EN_SHIFT 7
// Slice: emb_coef_23:
// second line, right entry of 3x3 emboss effect matrix, 2 bit for coefficient, one sign bit.
// coefficients:
// 000 (1), 001(2), 010(4), 011(8)
// 100 (-1), 101(-2), 110(-4), 111(-8)
#define MRV_IMGEFF_EMB_COEF_23
#define MRV_IMGEFF_EMB_COEF_23_MASK 0x00000070
#define MRV_IMGEFF_EMB_COEF_23_SHIFT 4
// combination of value and enable bit
#define MRV_IMGEFF_EMB_COEF_23_4
#define MRV_IMGEFF_EMB_COEF_23_4_MASK 0x000000F0
#define MRV_IMGEFF_EMB_COEF_23_4_SHIFT 4
// Slice: emb_coef_22_en:
// 0: entry not used (coefficient is zero)
// 1: entry used
#define MRV_IMGEFF_EMB_COEF_22_EN
#define MRV_IMGEFF_EMB_COEF_22_EN_MASK 0x00000008
#define MRV_IMGEFF_EMB_COEF_22_EN_SHIFT 3
// Slice: emb_coef_22:
// second line, middle entry of 3x3 emboss effect matrix, 2 bit for coefficient, one sign bit.
// coefficients:
// 000 (1), 001(2), 010(4), 011(8)
// 100 (-1), 101(-2), 110(-4), 111(-8)
#define MRV_IMGEFF_EMB_COEF_22
#define MRV_IMGEFF_EMB_COEF_22_MASK 0x00000007
#define MRV_IMGEFF_EMB_COEF_22_SHIFT 0
// combination of value and enable bit
#define MRV_IMGEFF_EMB_COEF_22_4
#define MRV_IMGEFF_EMB_COEF_22_4_MASK 0x0000000F
#define MRV_IMGEFF_EMB_COEF_22_4_SHIFT 0

// Register: img_eff_mat_3: 3x3 matrix coefficients for emboss(3) effect / sketch(1) effect (0x0010)
// Slice: sket_coef_13_en:
// 0: entry not used (coefficient is zero)
// 1: entry used
#define MRV_IMGEFF_SKET_COEF_13_EN
#define MRV_IMGEFF_SKET_COEF_13_EN_MASK 0x00008000
#define MRV_IMGEFF_SKET_COEF_13_EN_SHIFT 15
// Slice: sket_coef_13:
// first line, right entry of 3x3 sketch effect matrix, 2 bit for coefficient, one sign bit.
// coefficients:
// 000 (1), 001(2), 010(4), 011(8)
// 100 (-1), 101(-2), 110(-4), 111(-8)
#define MRV_IMGEFF_SKET_COEF_13
#define MRV_IMGEFF_SKET_COEF_13_MASK 0x00007000
#define MRV_IMGEFF_SKET_COEF_13_SHIFT 12
// combination of value and enable bit
#define MRV_IMGEFF_SKET_COEF_13_4
#define MRV_IMGEFF_SKET_COEF_13_4_MASK 0x0000F000
#define MRV_IMGEFF_SKET_COEF_13_4_SHIFT 12
// Slice: sket_coef_12_en:
// 0: entry not used (coefficient is zero)
// 1: entry used
#define MRV_IMGEFF_SKET_COEF_12_EN
#define MRV_IMGEFF_SKET_COEF_12_EN_MASK 0x00000800
#define MRV_IMGEFF_SKET_COEF_12_EN_SHIFT 11
// Slice: sket_coef_12:
// first line, middle entry of 3x3 sketch effect matrix, 2 bit for coefficient, one sign bit.
// coefficients:
// 000 (1), 001(2), 010(4), 011(8)
// 100 (-1), 101(-2), 110(-4), 111(-8)
#define MRV_IMGEFF_SKET_COEF_12
#define MRV_IMGEFF_SKET_COEF_12_MASK 0x00000700
#define MRV_IMGEFF_SKET_COEF_12_SHIFT 8
// combination of value and enable bit
#define MRV_IMGEFF_SKET_COEF_12_4
#define MRV_IMGEFF_SKET_COEF_12_4_MASK 0x00000F00
#define MRV_IMGEFF_SKET_COEF_12_4_SHIFT 8
// Slice: sket_coef_11_en:
// 0: entry not used (coefficient is zero)
// 1: entry used
#define MRV_IMGEFF_SKET_COEF_11_EN
#define MRV_IMGEFF_SKET_COEF_11_EN_MASK 0x00000080
#define MRV_IMGEFF_SKET_COEF_11_EN_SHIFT 7
// Slice: sket_coef_11:
// first line, left entry of 3x3 sketch effect matrix, 2 bit for coefficient, one sign bit.
// coefficients:
// 000 (1), 001(2), 010(4), 011(8)
// 100 (-1), 101(-2), 110(-4), 111(-8)
#define MRV_IMGEFF_SKET_COEF_11
#define MRV_IMGEFF_SKET_COEF_11_MASK 0x00000070
#define MRV_IMGEFF_SKET_COEF_11_SHIFT 4
// combination of value and enable bit
#define MRV_IMGEFF_SKET_COEF_11_4
#define MRV_IMGEFF_SKET_COEF_11_4_MASK 0x000000F0
#define MRV_IMGEFF_SKET_COEF_11_4_SHIFT 4
// Slice: emb_coef_33_en:
// 0: entry not used (coefficient is zero)
// 1: entry used
#define MRV_IMGEFF_EMB_COEF_33_EN
#define MRV_IMGEFF_EMB_COEF_33_EN_MASK 0x00000008
#define MRV_IMGEFF_EMB_COEF_33_EN_SHIFT 3
// Slice: emb_coef_33:
// third line, right entry of 3x3 emboss effect matrix, 2 bit for coefficient, one sign bit.
// coefficients:
// 000 (1), 001(2), 010(4), 011(8)
// 100 (-1), 101(-2), 110(-4), 111(-8)
#define MRV_IMGEFF_EMB_COEF_33
#define MRV_IMGEFF_EMB_COEF_33_MASK 0x00000007
#define MRV_IMGEFF_EMB_COEF_33_SHIFT 0
// combination of value and enable bit
#define MRV_IMGEFF_EMB_COEF_33_4
#define MRV_IMGEFF_EMB_COEF_33_4_MASK 0x0000000F
#define MRV_IMGEFF_EMB_COEF_33_4_SHIFT 0

// Register: img_eff_mat_4: 3x3 matrix coefficients for sketch effect (2) (0x0014)
// Slice: sket_coef_31_en:
// 0: entry not used (coefficient is zero)
// 1: entry used
#define MRV_IMGEFF_SKET_COEF_31_EN
#define MRV_IMGEFF_SKET_COEF_31_EN_MASK 0x00008000
#define MRV_IMGEFF_SKET_COEF_31_EN_SHIFT 15
// Slice: sket_coef_31:
// third line, left entry of 3x3 sketch effect matrix, 2 bit for coefficient, one sign bit.
// coefficients:
// 000 (1), 001(2), 010(4), 011(8)
// 100 (-1), 101(-2), 110(-4), 111(-8)
#define MRV_IMGEFF_SKET_COEF_31
#define MRV_IMGEFF_SKET_COEF_31_MASK 0x00007000
#define MRV_IMGEFF_SKET_COEF_31_SHIFT 12
// combination of value and enable bit
#define MRV_IMGEFF_SKET_COEF_31_4
#define MRV_IMGEFF_SKET_COEF_31_4_MASK 0x0000F000
#define MRV_IMGEFF_SKET_COEF_31_4_SHIFT 12
// Slice: sket_coef_23_en:
// 0: entry not used (coefficient is zero)
// 1: entry used
#define MRV_IMGEFF_SKET_COEF_23_EN
#define MRV_IMGEFF_SKET_COEF_23_EN_MASK 0x00000800
#define MRV_IMGEFF_SKET_COEF_23_EN_SHIFT 11
// Slice: sket_coef_23:
// second line, right entry of 3x3 sketch effect matrix, 2 bit for coefficient, one sign bit.
// coefficients:
// 000 (1), 001(2), 010(4), 011(8)
// 100 (-1), 101(-2), 110(-4), 111(-8)
#define MRV_IMGEFF_SKET_COEF_23
#define MRV_IMGEFF_SKET_COEF_23_MASK 0x00000700
#define MRV_IMGEFF_SKET_COEF_23_SHIFT 8
// combination of value and enable bit
#define MRV_IMGEFF_SKET_COEF_23_4
#define MRV_IMGEFF_SKET_COEF_23_4_MASK 0x00000F00
#define MRV_IMGEFF_SKET_COEF_23_4_SHIFT 8
// Slice: sket_coef_22_en:
// 0: entry not used (coefficient is zero)
// 1: entry used
#define MRV_IMGEFF_SKET_COEF_22_EN
#define MRV_IMGEFF_SKET_COEF_22_EN_MASK 0x00000080
#define MRV_IMGEFF_SKET_COEF_22_EN_SHIFT 7
// Slice: sket_coef_22:
// second line, middle entry of 3x3 sketch effect matrix, 2 bit for coefficient, one sign bit.
// coefficients:
// 000 (1), 001(2), 010(4), 011(8)
// 100 (-1), 101(-2), 110(-4), 111(-8)
#define MRV_IMGEFF_SKET_COEF_22
#define MRV_IMGEFF_SKET_COEF_22_MASK 0x00000070
#define MRV_IMGEFF_SKET_COEF_22_SHIFT 4
// combination of value and enable bit
#define MRV_IMGEFF_SKET_COEF_22_4
#define MRV_IMGEFF_SKET_COEF_22_4_MASK 0x000000F0
#define MRV_IMGEFF_SKET_COEF_22_4_SHIFT 4
// Slice: sket_coef_21_en:
// 0: entry not used (coefficient is zero)
// 1: entry used
#define MRV_IMGEFF_SKET_COEF_21_EN
#define MRV_IMGEFF_SKET_COEF_21_EN_MASK 0x00000008
#define MRV_IMGEFF_SKET_COEF_21_EN_SHIFT 3
// Slice: sket_coef_21:
// second line, left entry of 3x3 sketch effect matrix, 2 bit for coefficient, one sign bit.
// coefficients:
// 000 (1), 001(2), 010(4), 011(8)
// 100 (-1), 101(-2), 110(-4), 111(-8)
#define MRV_IMGEFF_SKET_COEF_21
#define MRV_IMGEFF_SKET_COEF_21_MASK 0x00000007
#define MRV_IMGEFF_SKET_COEF_21_SHIFT 0
// combination of value and enable bit
#define MRV_IMGEFF_SKET_COEF_21_4
#define MRV_IMGEFF_SKET_COEF_21_4_MASK 0x0000000F
#define MRV_IMGEFF_SKET_COEF_21_4_SHIFT 0

// Register: img_eff_mat_5: 3x3 matrix coefficients for sketch effect (3) (0x0018)
// Slice: sket_coef_33_en:
// 0: entry not used (coefficient is zero)
// 1: entry used
#define MRV_IMGEFF_SKET_COEF_33_EN
#define MRV_IMGEFF_SKET_COEF_33_EN_MASK 0x00000080
#define MRV_IMGEFF_SKET_COEF_33_EN_SHIFT 7
// Slice: sket_coef_33:
// third line, right entry of 3x3 sketch effect matrix, 2 bit for coefficient, one sign bit.
// coefficients:
// 000 (1), 001(2), 010(4), 011(8)
// 100 (-1), 101(-2), 110(-4), 111(-8)
#define MRV_IMGEFF_SKET_COEF_33
#define MRV_IMGEFF_SKET_COEF_33_MASK 0x00000070
#define MRV_IMGEFF_SKET_COEF_33_SHIFT 4
// combination of value and enable bit
#define MRV_IMGEFF_SKET_COEF_33_4
#define MRV_IMGEFF_SKET_COEF_33_4_MASK 0x000000F0
#define MRV_IMGEFF_SKET_COEF_33_4_SHIFT 4
// Slice: sket_coef_32_en:
// 0: entry not used (coefficient is zero)
// 1: entry used
#define MRV_IMGEFF_SKET_COEF_32_EN
#define MRV_IMGEFF_SKET_COEF_32_EN_MASK 0x00000008
#define MRV_IMGEFF_SKET_COEF_32_EN_SHIFT 3
// Slice: sket_coef_32:
// third line, middle entry of 3x3 sketch effect matrix, 2 bit for coefficient, one sign bit.
// coefficients:
// 000 (1), 001(2), 010(4), 011(8)
// 100 (-1), 101(-2), 110(-4), 111(-8)
#define MRV_IMGEFF_SKET_COEF_32
#define MRV_IMGEFF_SKET_COEF_32_MASK 0x00000007
#define MRV_IMGEFF_SKET_COEF_32_SHIFT 0
// combination of value and enable bit
#define MRV_IMGEFF_SKET_COEF_32_4
#define MRV_IMGEFF_SKET_COEF_32_4_MASK 0x0000000F
#define MRV_IMGEFF_SKET_COEF_32_4_SHIFT 0

// Register: img_eff_tint: Chrominance increment values of a tint (used for sepia effect) (0x001c)
// Slice: incr_cr:
// Cr increment value of a tint. 7 bits for value, 1 sign bit.
// Default tint is R=162 G=138 B=101, which is used for the sepia effect. See below for the calculation of the entries.
#define MRV_IMGEFF_INCR_CR
#define MRV_IMGEFF_INCR_CR_MASK 0x0000FF00
#define MRV_IMGEFF_INCR_CR_SHIFT 8
// Slice: incr_cb:
// Cb increment value of a tint. 7 bits for value, 1 sign bit.
// Default tint is R=162 G=138 B=101, which is used for the sepia effect. See below for the calculation of the entries.
#define MRV_IMGEFF_INCR_CB
#define MRV_IMGEFF_INCR_CB_MASK 0x000000FF
#define MRV_IMGEFF_INCR_CB_SHIFT 0

// Register: img_eff_ctrl_shd: Shadow register for control register (0x0020)
// Slice: effect_mode_shd:
// effect mode
// 000: black & white effect (grayscale)
// 001: negative effect
// 010: sepia effect
// 011: color selection effect
// 100: emboss effect
// 101: sketch effect
// 110…111: not used
#define MRV_IMGEFF_EFFECT_MODE_SHD
#define MRV_IMGEFF_EFFECT_MODE_SHD_MASK 0x0000000E
#define MRV_IMGEFF_EFFECT_MODE_SHD_SHIFT 1

#if (MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V3)
// Register: img_eff_sharpen: Factor and threshold for sharpen effect (0x0024)
// Slice: sharp_factor(11..8):
// Factor for sharpening function. Value range is from 0.0 (0000) to 1.5 (1111).
// High value means strong sharpening.

#define MRV_IMGEFF_SHARP_FACTOR
#define MRV_IMGEFF_SHARP_FACTOR_MASK 0x00000F00
#define MRV_IMGEFF_SHARP_FACTOR_SHIFT 8
// Slice: coring_thr(7..0):
// Threshold for coring function. This value is used to avoid amplifying noise too
// much by suppressing sharpening for small gradients.
// Higher value means less sharpening for smooth edges. Threshold zero means no coring,
// so all gradients are treated the same. Threshold 255 means nearly no sharpening.
#define MRV_IMGEFF_CORING_THR
#define MRV_IMGEFF_CORING_THR_MASK 0x000000FF
#define MRV_IMGEFF_CORING_THR_SHIFT 0
#endif // (MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V3)


/********************************************************************/
/* Super Impose Registers                                           */
/********************************************************************/

#if (MARVIN_FEATURE_SUPERIMPOSE == MARVIN_FEATURE_EXIST)
    // Register: super_imp_ctrl: Global control register (0x0000)
    //#define MRV_SI_CTRL_TRANS   (0x00000004)    // [2] transparency mode
    //#define MRV_SI_CTRL_REFIMG  (0x00000002)    // [1] reference image
    //#define MRV_SI_CTRL_EN      (0x00000001)    // [0] bypass mode
    // Slice: transparency_mode:
    // transparency mode
    // 1: transparency mode disabled
    // 0: transparency mode enabled
    #define MRV_SI_TRANSPARENCY_MODE
    #define MRV_SI_TRANSPARENCY_MODE_MASK 0x00000004
    #define MRV_SI_TRANSPARENCY_MODE_SHIFT 2
    #define MRV_SI_TRANSPARENCY_MODE_DISABLED 1 // 1: transparency mode disabled
    #define MRV_SI_TRANSPARENCY_MODE_ENABLED  0 // 0: transparency mode enabled
    // Slice: ref_image:
    // Define the reference image
    // 1: superimpose bitmap from main memory
    // 0: image from the Image Effect module
    // Note: the reference image defines the size of the output image
    #define MRV_SI_REF_IMAGE
    #define MRV_SI_REF_IMAGE_MASK 0x00000002
    #define MRV_SI_REF_IMAGE_SHIFT 1
    #define MRV_SI_REF_IMAGE_MEM   1 // 1: superimpose bitmap from main memory
    #define MRV_SI_REF_IMAGE_IE    0 // 0: image from the Image Effect module
    // Slice: bypass_mode:
    // Bypass mode
    // 0: bypass mode
    // 1: normal operation
    // Note: in the bypass mode the data stream from Image Effect is transmitted to MUX module without overlaying
    #define MRV_SI_BYPASS_MODE
    #define MRV_SI_BYPASS_MODE_MASK 0x00000001
    #define MRV_SI_BYPASS_MODE_SHIFT 0
    #define MRV_SI_BYPASS_MODE_BYPASS  0 // 0: bypass mode
    #define MRV_SI_BYPASS_MODE_PROCESS 1 // 1: normal operation

    // Register: super_imp_offset_x: Offset x register (0x0004)
    //#define MRV_SI_MAX_OFFS_X   (0x00001FFE)    // [11:1] offset x
    // Slice: offset_x:
    // Offset X
    // Note: the bit 0 is don’t care (write 1 doesn’t have any effect, the read access always gives ‘0’)
    // Note: the offset_x is positive and refers to the reference image
    #define MRV_SI_OFFSET_X
    #define MRV_SI_OFFSET_X_MASK 0x00001FFE
    #define MRV_SI_OFFSET_X_SHIFT 0
    #define MRV_SI_OFFSET_X_MAX  0x00001FFE

    // Register: super_imp_offset_y: Offset y register (0x0008)
    //#define MRV_SI_MAX_OFFS_Y   (0x00000FFF)    // [10:0] offset y
    // Slice: offset_y:
    // Offset Y
    // Note: the offset_y is positive and refers to the reference image
    #define MRV_SI_OFFSET_Y
    #define MRV_SI_OFFSET_Y_MASK 0x00000FFF
    #define MRV_SI_OFFSET_Y_SHIFT 0
    #define MRV_SI_OFFSET_Y_MAX  0x00000FFF

    // Register: super_imp_color_y: Y component of transparent key color (0x000c)
    // Slice: y_comp:
    // Y component of transparent key color
    #define MRV_SI_Y_COMP
    #define MRV_SI_Y_COMP_MASK 0x000000FF
    #define MRV_SI_Y_COMP_SHIFT 0

    // Register: super_imp_color_cb: Cb component of transparent key color (0x0010)
    // Slice: cb_comp:
    // Cb component of transparent key color
    #define MRV_SI_CB_COMP
    #define MRV_SI_CB_COMP_MASK 0x000000FF
    #define MRV_SI_CB_COMP_SHIFT 0

    // Register: super_imp_color_cr: Cr component of transparent key color (0x0014)
    // Slice: cr_comp:
    // Cr component of transparent key color
    #define MRV_SI_CR_COMP
    #define MRV_SI_CR_COMP_MASK 0x000000FF
    #define MRV_SI_CR_COMP_SHIFT 0
#endif // MARVIN_FEATURE_SUPERIMPOSE

/********************************************************************/
/* ISP Registers                                                    */
/********************************************************************/

// Register: isp_ctrl: global control register (0x0000)
//#define MRV_ISP_CSM_C_RANGE      0x00004000 //[14] Color Space Matrix chrominance clipping range for ISP output
//#define MRV_ISP_CSM_Y_RANGE      0x00002000 //[13] Color Space Matrix luminance clipping range for ISP output
//#define MRV_GAMMA2_EN            0x00000800 //[11] rw 1: gamma correction 2 (output) enabled
//#define MRV_GEN_CFG_UPD          0x00000400 //[10] w 1: generate configuration update signal for data path at frame end
//#define MRV_CFG_UPD              0x00000200 //[9] w 1: immedately update shadow registers
//#define MRV_AE_EN                0x00000100 //[8] rw 1: auto exposure support enabled
//#define MRV_AWB_EN               0x00000080 //[7] rw 1: auto white balance enabled
//#define MRV_GAMMA_EN             0x00000040 //[6] rw 1: gamma correction (input) enabled
//#define MRV_BLS_EN               0x00000020 //[5] rw 1: black level subtraction enabled
//#define MRV_INPUT_EN             0x00000010 //[4] rw 1: switches input interface on, frame synchronously
//#define MRV_MODE                 0x0000000E //[3:1] rw input data mode
//#define MRV_CTRL_RAW656    0x0C //"110": raw picture according to ITU-R BT.656   ***NEW***
//#define MRV_CTRL_RGB656    0x0A //"101": RGB Bayer according to ITU-R BT.656  ***NEW***
//#define MRV_CTRL_DATA      0x08 //"100": data mode
//#define MRV_CTRL_RGB       0x06 //"011": RGB Bayer according to ITU-R BT.601
//#define MRV_CTRL_601       0x04 //"010": ITU-R BT601 (YCbCr data)
//#define MRV_CTRL_656       0x02 //"001": ITU-R BT656 (YCbCr data)
//#define MRV_CTRL_RAW       0x00 //"000": raw picture according to ITU-R BT.601
//#define MRV_ISP_EN               0x00000001 //[0] rw 1: switches ISP (output interface) on, frame synchronously
#if (MARVIN_FEATURE_EXT_YCBCR_RANGE == MARVIN_FEATURE_EXIST)
    // Slice: ISP_CSM_C_RANGE:
    // Color Space Matrix chrominance clipping range for ISP output
    // 0: CbCr range 64..960 (16..240) according to ITU-R BT.601 standard
    // 1: full UV range 0..1023 ( 0..255)
    // Numbers in brackets are for 8 bit resolution.
    #define MRV_ISP_ISP_CSM_C_RANGE
    #define MRV_ISP_ISP_CSM_C_RANGE_MASK 0x00004000
    #define MRV_ISP_ISP_CSM_C_RANGE_SHIFT 14
    #define MRV_ISP_ISP_CSM_C_RANGE_BT601  0 // 0: CbCr range 64..960 (16..240) according to ITU-R BT.601 standard
    #define MRV_ISP_ISP_CSM_C_RANGE_FULL   1 // 1: full UV range 0..1023 ( 0..255)
    // Slice: ISP_CSM_Y_RANGE:
    // Color Space Matrix luminance clipping range for ISP output
    // 0: Y range 64..940 (16..235) according to ITU-R BT.601 standard
    // 1: full Y range 0..1023 ( 0..255)
    // Numbers in brackets are for 8 bit resolution.
    #define MRV_ISP_ISP_CSM_Y_RANGE
    #define MRV_ISP_ISP_CSM_Y_RANGE_MASK 0x00002000
    #define MRV_ISP_ISP_CSM_Y_RANGE_SHIFT 13
    #define MRV_ISP_ISP_CSM_Y_RANGE_BT601  0 // 0: Y range 64..940 (16..235) according to ITU-R BT.601 standard
    #define MRV_ISP_ISP_CSM_Y_RANGE_FULL   1 // 1: full Y range 0..1023 ( 0..255)
#endif // #if (MARVIN_FEATURE_EXT_YCBCR_RANGE == MARVIN_FEATURE_EXIST)
// Slice: ISP_FLASH_MODE:
// 0: sensor interface works independently from flash control unit
// 1: one frame is captured when signaled by flash control unit
#define MRV_ISP_ISP_FLASH_MODE
#define MRV_ISP_ISP_FLASH_MODE_MASK 0x00001000
#define MRV_ISP_ISP_FLASH_MODE_SHIFT 12
#define MRV_ISP_ISP_FLASH_MODE_INDEP  0 // 0: sensor interface works independently from flash control unit
#define MRV_ISP_ISP_FLASH_MODE_SYNC   1 // 1: one frame is captured when signaled by flash control unit
#if (MARVIN_FEATURE_GAMMAOUT == MARVIN_FEATURE_EXIST)
    // Slice: ISP_GAMMA_OUT_ENABLE:
    // gamma ON/OFF
    #define MRV_ISP_ISP_GAMMA_OUT_ENABLE
    #define MRV_ISP_ISP_GAMMA_OUT_ENABLE_MASK 0x00000800
    #define MRV_ISP_ISP_GAMMA_OUT_ENABLE_SHIFT 11
#endif // #if (MARVIN_FEATURE_GAMMAOUT == MARVIN_FEATURE_EXIST)
// Slice: ISP_GEN_CFG_UPD:
// generate vsynced config shadow signal at the output, write only
#define MRV_ISP_ISP_GEN_CFG_UPD
#define MRV_ISP_ISP_GEN_CFG_UPD_MASK 0x00000400
#define MRV_ISP_ISP_GEN_CFG_UPD_SHIFT 10
// Slice: ISP_CFG_UPD:
// immediate configure internal shadow registers, write only
#define MRV_ISP_ISP_CFG_UPD
#define MRV_ISP_ISP_CFG_UPD_MASK 0x00000200
#define MRV_ISP_ISP_CFG_UPD_SHIFT 9
#if( MARVIN_FEATURE_AUTO_EXPOSURE == MARVIN_FEATURE_AUTO_EXPOSURE_V1 )
    // Slice: ISP_AE_ENABLE:
    // auto exposure ON/OFF
    #define MRV_ISP_ISP_AE_ENABLE
    #define MRV_ISP_ISP_AE_ENABLE_MASK 0x00000100
    #define MRV_ISP_ISP_AE_ENABLE_SHIFT 8
#elif(   ( MARVIN_FEATURE_AUTO_EXPOSURE == MARVIN_FEATURE_AUTO_EXPOSURE_V2 )\
      || ( MARVIN_FEATURE_AUTO_EXPOSURE == MARVIN_FEATURE_AUTO_EXPOSURE_V4 ) )
    // Slice: reserved1:
    // unused (writing has no effect, reading returns zero)
#elif( MARVIN_FEATURE_AUTO_EXPOSURE == MARVIN_FEATURE_EXIST_NOT )
   //unused
#elif( MARVIN_FEATURE_AUTO_EXPOSURE == MARVIN_FEATURE_AUTO_EXPOSURE_V3 )
    #error "MARVIN_FEATURE_AUTO_EXPOSURE_V3 not supported"
#else
    #error "unknown setting for MARVIN_FEATURE_AUTO_EXPOSURE"
#endif
// Slice: ISP_AWB_ENABLE:
// auto white balance ON/OFF
#define MRV_ISP_ISP_AWB_ENABLE
#define MRV_ISP_ISP_AWB_ENABLE_MASK 0x00000080
#define MRV_ISP_ISP_AWB_ENABLE_SHIFT 7
// Slice: ISP_GAMMA_IN_ENABLE:
// Sensor De-gamma ON/OFF
#define MRV_ISP_ISP_GAMMA_IN_ENABLE
#define MRV_ISP_ISP_GAMMA_IN_ENABLE_MASK 0x00000040
#define MRV_ISP_ISP_GAMMA_IN_ENABLE_SHIFT 6
#if ((MARVIN_FEATURE_BLACK_LEVEL >= MARVIN_FEATURE_BLACK_LEVEL_V1) && \
     (MARVIN_FEATURE_BLACK_LEVEL <= MARVIN_FEATURE_BLACK_LEVEL_V3))
    // Slice: ISP_BLS_ENABLE:
    // black level subtraction ON/OFF
    #define MRV_ISP_ISP_BLS_ENABLE
    #define MRV_ISP_ISP_BLS_ENABLE_MASK 0x00000020
    #define MRV_ISP_ISP_BLS_ENABLE_SHIFT 5
#elif ((MARVIN_FEATURE_BLACK_LEVEL >= MARVIN_FEATURE_BLACK_LEVEL_V4) && \
       (MARVIN_FEATURE_BLACK_LEVEL <= MARVIN_FEATURE_BLACK_LEVEL_V5))
    // Slice: reserved2:
    // unused (writing has no effect, reading returns zero)
#elif (MARVIN_FEATURE_BLACK_LEVEL == MARVIN_FEATURE_EXIST_NOT)
    // unused:
#else
    #error "unknown setting for MARVIN_FEATURE_BLACK_LEVEL"
#endif
// Slice: ISP_INFORM_ENABLE:
// input block ON/OFF (frame-synced on)
#define MRV_ISP_ISP_INFORM_ENABLE
#define MRV_ISP_ISP_INFORM_ENABLE_MASK 0x00000010
#define MRV_ISP_ISP_INFORM_ENABLE_SHIFT 4
// Slice: ISP_MODE:
// 000 - RAW picture with BT.601 sync (ISP bypass)
// 001 - ITU-R BT.656 (YUV with embedded sync)
// 010 - ITU-R BT.601 (YUV input with H and Vsync signals)
// 011 - Bayer RGB processing with H and Vsync signals
// 100 - data mode (ISP bypass, sync signals interpreted as data enable)
// 101 - Bayer RGB processing with BT.656 synchronization
// 110 - RAW picture with ITU-R BT.656 synchronization (ISP bypass)
// 111 - reserved
//
// Side effect:
// If RAW, BT.601, BT.656, or data mode is selected, the clock of the ISP SRAMs (ISP line buffer, Lens Shading, Bad Pixel) is switched off. Only in Bayer RGB mode the clock to the SRAMs is enabled. This further reduces power consumption.
#define MRV_ISP_ISP_MODE
#define MRV_ISP_ISP_MODE_MASK 0x0000000E
#define MRV_ISP_ISP_MODE_SHIFT 1
#define MRV_ISP_ISP_MODE_RAW    0 // 000 - RAW picture with BT.601 sync (ISP bypass)
#define MRV_ISP_ISP_MODE_656    1 // 001 - ITU-R BT.656 (YUV with embedded sync)
#define MRV_ISP_ISP_MODE_601    2 // 010 - ITU-R BT.601 (YUV input with H and Vsync signals)
#define MRV_ISP_ISP_MODE_RGB    3 // 011 - Bayer RGB processing with H and Vsync signals
#define MRV_ISP_ISP_MODE_DATA   4 // 100 - data mode (ISP bypass, sync signals interpreted as data enable)
#define MRV_ISP_ISP_MODE_RGB656 5 // 101 - Bayer RGB processing with BT.656 synchronization
#define MRV_ISP_ISP_MODE_RAW656 6 // 110 - RAW picture with ITU-R BT.656 synchronization (ISP bypass)
// Slice: ISP_ENABLE:
// MANUAL ON/OFF (controls output formatter, framesynced)
#define MRV_ISP_ISP_ENABLE
#define MRV_ISP_ISP_ENABLE_MASK 0x00000001
#define MRV_ISP_ISP_ENABLE_SHIFT 0

// Register: isp_acq_prop: ISP acquisition properties (0x0004)
//  #define MRV_ACQ_INP_8MSB   2  //"10": 8Bit Interface, append 2 MSBs as LSBs
//  #define MRV_ACQ_INP_8ZERO  1  //"01": 8Bit Interface, append 2 zeroes as LSBs
//  #define MRV_ACQ_INP_10EXT  0  //"00": 10Bit external Interface
//  #define MRV_ACQ_INP_10MSB  2  //"010": 10Bit Interface, append 2 MSBs as LSBs
//  #define MRV_ACQ_INP_10ZERO 1  //"001": 10Bit Interface, append 2 zeroes as LSBs
//  #define MRV_ACQ_INP_12EXT  0  //"000": 12Bit external Interface
//#define MRV_FIELD_INV            0x00000800 //[11] rw 1: swap odd and even fields
//#define MRV_FIELD_SEL            0x00000600 //[10:9] rw "11": reserved
//#define MRV_ACQ_FIELD_ODD    2  //"10": sample odd fields only
//#define MRV_ACQ_FIELD_EVEN   1  //"01": sample even fields only
//#define MRV_ACQ_FIELD_ALL    0  //"00": sample all fields (don’t care about fields)
//#define MRV_YC_SEQ               0x00000180 //[8:7] rw "11": CrYCbY
//#define MRV_ACQ_YCSEQ_CRYCBY 3  //"11": CrYCbY
//#define MRV_ACQ_YCSEQ_CBYCRY 2  //"10": CbYCrY
//#define MRV_ACQ_YCSEQ_YCRYCB 1  //"01": YCrYCb
//#define MRV_ACQ_YCSEQ_YCBYCR 0  //"00": YCbYCr
//#define MRV_CONV_422             0x00000060 //[6:5] rw YCbCr 4:2:2 conversion method
//                                //"11": reserved
//#define MRV_ACQ_CONV_NONCO   2  //"10": non-cosited color subsampling Y0Cb(0+1)/2 – Y1Cr(0+1)/2
//#define MRV_ACQ_CONV_INTER   1  //"01": interleaved color subsampling Y0Cb0 – Y1Cr1 (not recommended)
//#define MRV_ACQ_CONV_CO      0  //"00": co-sited color subsampling //Y0Cb0Cr0 – Y1
//#define MRV_BAYER_PAT            0x00000018 //[4:3] rw RGB Bayer pattern layout
//#define MRV_ACQ_BAY_BG       3  //"11": BGBG  |  GRGR
//#define MRV_ACQ_BAY_GB       2  //"10": GBGB  |  RGRG
//#define MRV_ACQ_BAY_GR       1  //"01": GRGR  |  BGBG
//#define MRV_ACQ_BAY_RG       0  //"00": RGRG  |  GBGB
//#define MRV_VSYNC_POL            0x00000004 //[2] rw vertical synchronization signal polarity
//                                            //1: high active (trigger at positive edge)
//                                            //0: low active (trigger at negative edge)
//#define MRV_HSYNC_POL            0x00000002 //[1] rw horizontal synchronization signal polarity
//                                            //1: high active
//                                            //0: low active
//#define MRV_SAMPLE_EDGE          0x00000001 //[0] rw sensor clock sample edge
//                                            //1: use positive sensor clock edge
//                                            //0: use negative sensor clock edge
#if (MARVIN_FEATURE_CAMBUSWIDTH == MARVIN_FEATURE_8BITS)    //marvin1
    // no selection required
#elif (MARVIN_FEATURE_CAMBUSWIDTH == MARVIN_FEATURE_10BITS) //marvin3
    // Slice: INPUT_SELECTION:
    // 00- 10Bit external Interface
    // 01- 8Bit Interface, append 2 zeroes as LSBs
    // 10- 8Bit Interface, append 2 MSBs as LSBs
    // 11- reserved
    #define MRV_ISP_INPUT_SELECTION
    #define MRV_ISP_INPUT_SELECTION_MASK 0x00003000
    #define MRV_ISP_INPUT_SELECTION_SHIFT 12
    #define MRV_ISP_INPUT_SELECTION_10EXT  0 // 00- 10Bit external Interface
    #define MRV_ISP_INPUT_SELECTION_8ZERO  1 // 01- 8Bit Interface, append 2 zeroes as LSBs
    #define MRV_ISP_INPUT_SELECTION_8MSB   2 // 10- 8Bit Interface, append 2 MSBs as LSBs
#elif (MARVIN_FEATURE_CAMBUSWIDTH == MARVIN_FEATURE_12BITS) //marvin5
    // Slice: INPUT_SELECTION:
    // 000- 12Bit external Interface
    // 001- 10Bit Interface, append 2 zeroes as LSBs
    // 010- 10Bit Interface, append 2 MSBs as LSBs
    // 011- 8Bit Interface, append 4 zeroes as LSBs
    // 100- 8Bit Interface, append 4 MSBs as LSBs
    // 101...111 reserved
    #define MRV_ISP_INPUT_SELECTION
    #define MRV_ISP_INPUT_SELECTION_MASK 0x00007000
    #define MRV_ISP_INPUT_SELECTION_SHIFT 12
    #define MRV_ISP_INPUT_SELECTION_12EXT  0 // 000- 12Bit external Interface
    #define MRV_ISP_INPUT_SELECTION_10ZERO 1 // 001- 10Bit Interface, append 2 zeroes as LSBs
    #define MRV_ISP_INPUT_SELECTION_10MSB  2 // 010- 10Bit Interface, append 2 MSBs as LSBs
    #define MRV_ISP_INPUT_SELECTION_8ZERO  3 // 011- 8Bit Interface, append 4 zeroes as LSBs
    #define MRV_ISP_INPUT_SELECTION_8MSB   4 // 100- 8Bit Interface, append 4 MSBs as LSBs
#else // MARVIN_FEATURE_CAMBUSWIDTH
    #error "unknown setting for MARVIN_FEATURE_CAMBUSWIDTH"
#endif // MARVIN_FEATURE_CAMBUSWIDTH
#if (MARVIN_FEATURE_INPUT_AQUISITION == MARVIN_FEATURE_IAQU_V1)
    // Slice: FIELD_INV:
    // 1: swap odd and even fields
    // 0: do not swap fields
    #define MRV_ISP_FIELD_INV
    #define MRV_ISP_FIELD_INV_MASK 0x00000800
    #define MRV_ISP_FIELD_INV_SHIFT 11
#elif (MARVIN_FEATURE_INPUT_AQUISITION == MARVIN_FEATURE_IAQU_V2)
    // Slice: reserved
#else
    #error "unknown setting for MARVIN_FEATURE_INPUT_AQUISITION"
#endif
// Slice: FIELD_SELECTION:
// 00- sample all fields (don’t care about fields)
// 01- sample only even fields
// 10- sample only odd fields
// 11- reserved
#define MRV_ISP_FIELD_SELECTION
#define MRV_ISP_FIELD_SELECTION_MASK 0x00000600
#define MRV_ISP_FIELD_SELECTION_SHIFT 9
#define MRV_ISP_FIELD_SELECTION_BOTH  0 // 00- sample all fields (don’t care about fields)
#define MRV_ISP_FIELD_SELECTION_EVEN  1 // 01- sample only even fields
#define MRV_ISP_FIELD_SELECTION_ODD   2 // 10- sample only odd fields
// Slice: CCIR_SEQ:
// 00- YCbYCr
// 01- YCrYCb
// 10- CbYCrY
// 11- CrYCbY
#define MRV_ISP_CCIR_SEQ
#define MRV_ISP_CCIR_SEQ_MASK 0x00000180
#define MRV_ISP_CCIR_SEQ_SHIFT 7
#define MRV_ISP_CCIR_SEQ_YCBYCR 0 // 00- YCbYCr
#define MRV_ISP_CCIR_SEQ_YCRYCB 1 // 01- YCrYCb
#define MRV_ISP_CCIR_SEQ_CBYCRY 2 // 10- CbYCrY
#define MRV_ISP_CCIR_SEQ_CRYCBY 3 // 11- CrYCbY
// Slice: CONV_422:
// 00- co-sited color subsampling Y0Cb0Cr0 – Y1
// 01- interleaved color subsampling Y0Cb0 – Y1Cr1 (not recommended)
// 10- non-cosited color subsampling Y0Cb(0+1)/2 – Y1Cr(0+1)/2
// 11- reserved
#define MRV_ISP_CONV_422
#define MRV_ISP_CONV_422_MASK 0x00000060
#define MRV_ISP_CONV_422_SHIFT  5
#define MRV_ISP_CONV_422_CO     0 // 00- co-sited color subsampling Y0Cb0Cr0 – Y1
#define MRV_ISP_CONV_422_INTER  1 // 01- interleaved color subsampling Y0Cb0 – Y1Cr1 (not recommended)
#define MRV_ISP_CONV_422_NONCO  2 // 10- non-cosited color subsampling Y0Cb(0+1)/2 – Y1Cr(0+1)/2
// Slice: BAYER_PAT:
// color components from sensor, starting with top left position in sampled frame (reprogram with ISP_ACQ_H_OFFS, ISP_ACQ_V_OFFS)
// 00- first line: RGRG…, second line: GBGB…, etc.
// 01- first line: GRGR…, second line: BGBG…, etc.
// 10- first line: GBGB…, second line: RGRG…, etc.
// 11- first line: BGBG…, second line: GRGR…, etc.
// This configuration applies for the black level area after cropping by the input formatter.
#define MRV_ISP_BAYER_PAT
#define MRV_ISP_BAYER_PAT_MASK 0x00000018
#define MRV_ISP_BAYER_PAT_SHIFT 3
#define MRV_ISP_BAYER_PAT_RG    0 // 00- first line: RGRG…, second line: GBGB…, etc.
#define MRV_ISP_BAYER_PAT_GR    1 // 01- first line: GRGR…, second line: BGBG…, etc.
#define MRV_ISP_BAYER_PAT_GB    2 // 10- first line: GBGB…, second line: RGRG…, etc.
#define MRV_ISP_BAYER_PAT_BG    3 // 11- first line: BGBG…, second line: GRGR…, etc.
// Slice: VSYNC_POL:
// vertical sync polarity
// 0: high active
// 1: low active
#define MRV_ISP_VSYNC_POL
#define MRV_ISP_VSYNC_POL_MASK 0x00000004
#define MRV_ISP_VSYNC_POL_SHIFT 2
// Slice: HSYNC_POL:
// horizontal sync polarity
// 0: high active
// 1: low active
#define MRV_ISP_HSYNC_POL
#define MRV_ISP_HSYNC_POL_MASK 0x00000002
#define MRV_ISP_HSYNC_POL_SHIFT 1
// Slice: SAMPLE_EDGE:
// 0- negative edge sampling
// 1- positive edge sampling
#define MRV_ISP_SAMPLE_EDGE
#define MRV_ISP_SAMPLE_EDGE_MASK 0x00000001
#define MRV_ISP_SAMPLE_EDGE_SHIFT 0

// Register: isp_acq_h_offs: horizontal input offset (0x0008)
// Slice: ACQ_H_OFFS:
// horizontal sample offset in 8-bit samples (yuv: 4 samples=2pix)
#define MRV_ISP_ACQ_H_OFFS
#define MRV_ISP_ACQ_H_OFFS_MASK 0x00003FFF
#define MRV_ISP_ACQ_H_OFFS_SHIFT 0

// Register: isp_acq_v_offs: vertical input offset (0x000c)
// Slice: ACQ_V_OFFS:
// vertical sample offset in lines
#define MRV_ISP_ACQ_V_OFFS
#define MRV_ISP_ACQ_V_OFFS_MASK 0x00000FFF
#define MRV_ISP_ACQ_V_OFFS_SHIFT 0

// Register: isp_acq_h_size: horizontal input size (0x0010)
// Slice: ACQ_H_SIZE:
// horizontal sample size in 12-bit samples
// YUV input: 2 samples = 1 pixel, else 1 sample = 1 pixel; So in YUV mode ACQ_H_SIZE must be twice as large as horizontal image size
// horizontal image size must always be even exept in raw picture mode; if an odd size is programmed the value will be truncated to even size
#define MRV_ISP_ACQ_H_SIZE
#define MRV_ISP_ACQ_H_SIZE_MASK 0x00003FFF
#define MRV_ISP_ACQ_H_SIZE_SHIFT 0

// Register: isp_acq_v_size: vertical input size (0x0014)
// Slice: ACQ_V_SIZE:
// vertical sample size in lines
#define MRV_ISP_ACQ_V_SIZE
#define MRV_ISP_ACQ_V_SIZE_MASK 0x00000FFF
#define MRV_ISP_ACQ_V_SIZE_SHIFT 0

// Register: isp_acq_nr_frames: Number of frames to be captured (0x0018)
// Slice: ACQ_NR_FRAMES:
// number of input frames to be sampled ( 0 = continuous )
#define MRV_ISP_ACQ_NR_FRAMES
#define MRV_ISP_ACQ_NR_FRAMES_MASK 0x000003FF
#define MRV_ISP_ACQ_NR_FRAMES_SHIFT 0
#define MRV_ISP_ACQ_NR_FRAMES_MAX (MRV_ISP_ACQ_NR_FRAMES_MASK >> MRV_ISP_ACQ_NR_FRAMES_SHIFT)

// Register: isp_gamma_dx_lo: De-Gamma Curve definition lower x increments (sampling points) (0x001c)
//#define MRV_GAMMA_DX8            0x70000000 //[30:28] rw gamma curve interval 8 width (x-axis)
//#define MRV_GAMMA_DX7            0x07000000 //[26:24] rw gamma curve interval 7 width (x-axis)
//#define MRV_GAMMA_DX6            0x00700000 //[22:20] rw gamma curve interval 6 width (x-axis)
//#define MRV_GAMMA_DX5            0x00070000 //[18:16] rw gamma curve interval 5 width (x-axis)
//#define MRV_GAMMA_DX4            0x00007000 //[14:12] rw gamma curve interval 4 width (x-axis)
//#define MRV_GAMMA_DX3            0x00000700 //[10:8] rw gamma curve interval 3 width (x-axis)
//#define MRV_GAMMA_DX2            0x00000070 //[6:4] rw gamma curve interval 2 width (x-axis)
//#define MRV_GAMMA_DX1            0x00000007 //[2:0] rw gamma curve interval 1 width (x-axis)
// Slice: GAMMA_DX_8:
// gamma curve sample point definition x-axis (input)
#define MRV_ISP_GAMMA_DX_8
#define MRV_ISP_GAMMA_DX_8_MASK 0x70000000
#define MRV_ISP_GAMMA_DX_8_SHIFT 28
// Slice: GAMMA_DX_7:
// gamma curve sample point definition x-axis (input)
#define MRV_ISP_GAMMA_DX_7
#define MRV_ISP_GAMMA_DX_7_MASK 0x07000000
#define MRV_ISP_GAMMA_DX_7_SHIFT 24
// Slice: GAMMA_DX_6:
// gamma curve sample point definition x-axis (input)
#define MRV_ISP_GAMMA_DX_6
#define MRV_ISP_GAMMA_DX_6_MASK 0x00700000
#define MRV_ISP_GAMMA_DX_6_SHIFT 20
// Slice: GAMMA_DX_5:
// gamma curve sample point definition x-axis (input)
#define MRV_ISP_GAMMA_DX_5
#define MRV_ISP_GAMMA_DX_5_MASK 0x00070000
#define MRV_ISP_GAMMA_DX_5_SHIFT 16
// Slice: GAMMA_DX_4:
// gamma curve sample point definition x-axis (input)
#define MRV_ISP_GAMMA_DX_4
#define MRV_ISP_GAMMA_DX_4_MASK 0x00007000
#define MRV_ISP_GAMMA_DX_4_SHIFT 12
// Slice: GAMMA_DX_3:
// gamma curve sample point definition x-axis (input)
#define MRV_ISP_GAMMA_DX_3
#define MRV_ISP_GAMMA_DX_3_MASK 0x00000700
#define MRV_ISP_GAMMA_DX_3_SHIFT 8
// Slice: GAMMA_DX_2:
// gamma curve sample point definition x-axis (input)
#define MRV_ISP_GAMMA_DX_2
#define MRV_ISP_GAMMA_DX_2_MASK 0x00000070
#define MRV_ISP_GAMMA_DX_2_SHIFT 4
// Slice: GAMMA_DX_1:
// gamma curve sample point definition x-axis (input)
#define MRV_ISP_GAMMA_DX_1
#define MRV_ISP_GAMMA_DX_1_MASK 0x00000007
#define MRV_ISP_GAMMA_DX_1_SHIFT 0

// Register: isp_gamma_dx_hi: De-Gamma Curve definition higher x increments (sampling points) (0x0020)
//#define MRV_GAMMA_DX16           0x70000000 //[30:28] rw gamma curve interval 16 width (x-axis)
//#define MRV_GAMMA_DX15           0x07000000 //[26:24] rw gamma curve interval 15 width (x-axis)
//#define MRV_GAMMA_DX14           0x00700000 //[22:20] rw gamma curve interval 14 width (x-axis)
//#define MRV_GAMMA_DX13           0x00070000 //[18:16] rw gamma curve interval 13 width (x-axis)
//#define MRV_GAMMA_DX12           0x00007000 //[14:12] rw gamma curve interval 12 width (x-axis)
//#define MRV_GAMMA_DX11           0x00000700 //[10:8] rw gamma curve interval 11 width (x-axis)
//#define MRV_GAMMA_DX10           0x00000070 //[6:4] rw gamma curve interval 10 width (x-axis)
//#define MRV_GAMMA_DX9            0x00000007 //[2:0] rw gamma curve interval 9 width (x-axis)
// Slice: GAMMA_DX_16:
// gamma curve sample point definition x-axis (input)
#define MRV_ISP_GAMMA_DX_16
#define MRV_ISP_GAMMA_DX_16_MASK 0x70000000
#define MRV_ISP_GAMMA_DX_16_SHIFT 28
// Slice: GAMMA_DX_15:
// gamma curve sample point definition x-axis (input)
#define MRV_ISP_GAMMA_DX_15
#define MRV_ISP_GAMMA_DX_15_MASK 0x07000000
#define MRV_ISP_GAMMA_DX_15_SHIFT 24
// Slice: GAMMA_DX_14:
// gamma curve sample point definition x-axis (input)
#define MRV_ISP_GAMMA_DX_14
#define MRV_ISP_GAMMA_DX_14_MASK 0x00700000
#define MRV_ISP_GAMMA_DX_14_SHIFT 20
// Slice: GAMMA_DX_13:
// gamma curve sample point definition x-axis (input)
#define MRV_ISP_GAMMA_DX_13
#define MRV_ISP_GAMMA_DX_13_MASK 0x00070000
#define MRV_ISP_GAMMA_DX_13_SHIFT 16
// Slice: GAMMA_DX_12:
// gamma curve sample point definition x-axis (input)
#define MRV_ISP_GAMMA_DX_12
#define MRV_ISP_GAMMA_DX_12_MASK 0x00007000
#define MRV_ISP_GAMMA_DX_12_SHIFT 12
// Slice: GAMMA_DX_11:
// gamma curve sample point definition x-axis (input)
#define MRV_ISP_GAMMA_DX_11
#define MRV_ISP_GAMMA_DX_11_MASK 0x00000700
#define MRV_ISP_GAMMA_DX_11_SHIFT 8
// Slice: GAMMA_DX_10:
// gamma curve sample point definition x-axis (input)
#define MRV_ISP_GAMMA_DX_10
#define MRV_ISP_GAMMA_DX_10_MASK 0x00000070
#define MRV_ISP_GAMMA_DX_10_SHIFT 4
// Slice: GAMMA_DX_9:
// gamma curve sample point definition x-axis (input)
#define MRV_ISP_GAMMA_DX_9
#define MRV_ISP_GAMMA_DX_9_MASK 0x00000007
#define MRV_ISP_GAMMA_DX_9_SHIFT 0

// Register array: isp_gamma_?_y
// Slice: GAMMA_?_Y
//    #define MRV_WB_INPUT_GAMMA_MAX      (0x000000FF)
//    #define MRV_WB_INPUT_GAMMA_MAX      (0x000003FF)
//    #define MRV_WB_INPUT_GAMMA_MAX      (0x00000FFF)
#define MRV_ISP_GAMMA_Y
#if (MARVIN_FEATURE_CAMBUSWIDTH == MARVIN_FEATURE_8BITS)
    #define MRV_ISP_GAMMA_Y_MASK 0x000000FF
#elif (MARVIN_FEATURE_CAMBUSWIDTH == MARVIN_FEATURE_10BITS)
    #define MRV_ISP_GAMMA_Y_MASK 0x000003FF
#elif (MARVIN_FEATURE_CAMBUSWIDTH == MARVIN_FEATURE_12BITS)
    #define MRV_ISP_GAMMA_Y_MASK 0x00000FFF
#else   // MARVIN_FEATURE_CAMBUSWIDTH
    #error unknown MARVIN_FEATURE_CAMBUSWIDTH value
#endif  // MARVIN_FEATURE_CAMBUSWIDTH
#define MRV_ISP_GAMMA_Y_SHIFT 0
#define MRV_ISP_GAMMA_Y_MAX (MRV_ISP_GAMMA_Y_MASK >> MRV_ISP_GAMMA_Y_SHIFT)

// Register array: isp_gamma_r_y: De-Gamma Curve definition y red (0x0024+n (n=0..16))
// Slice: GAMMA_R_Y:
// gamma curve point definition y-axis (output) for red
// RESTRICTION: each Y must be in the +2047/-2048 range compared to its predecessor (so that dy is 12-bit signed !)
#define MRV_ISP_GAMMA_R_Y
#define MRV_ISP_GAMMA_R_Y_MASK  MRV_ISP_GAMMA_Y_MASK
#define MRV_ISP_GAMMA_R_Y_SHIFT MRV_ISP_GAMMA_Y_SHIFT

// Register array: isp_gamma_g_y: De-Gamma Curve definition y green (0x0068+n (n=0..16))
// Slice: GAMMA_G_Y:
// gamma curve point definition y-axis (output) for green
// RESTRICTION: each Y must be in the +2047/-2048 range compared to its predecessor (so that dy is 12-bit signed !)
#define MRV_ISP_GAMMA_G_Y
#define MRV_ISP_GAMMA_G_Y_MASK  MRV_ISP_GAMMA_Y_MASK
#define MRV_ISP_GAMMA_G_Y_SHIFT MRV_ISP_GAMMA_Y_SHIFT

// Register array: isp_gamma_b_y: De-Gamma Curve definition y blue (0x00AC+n (n=0..16))
// Slice: GAMMA_B_Y:
// gamma curve point definition y-axis (output) for blue
// RESTRICTION: each Y must be in the +2047/-2048 range compared to its predecessor (so that dy is 12-bit signed !)
#define MRV_ISP_GAMMA_B_Y
#define MRV_ISP_GAMMA_B_Y_MASK  MRV_ISP_GAMMA_Y_MASK
#define MRV_ISP_GAMMA_B_Y_SHIFT MRV_ISP_GAMMA_Y_SHIFT

// Register: isp_awb_prop: Auto white balance properties (0x0110)
//#define MRV_AWB_MEAS_MODE    0x80000000 // 1: RGB based measurement mode (M5_v6 only)
                                          // 0: near white discrimination mode
//#define MRV_AWB_Y_MAX_EN     4  // enable Y_MAX compare
//                                //MRV_AWB_MODE
//#define MRV_AWB_AUTO         3  //[1:0] rw "11": automatic white balance
//#define MRV_AWB_MAN_YCBCR    2  //"10": manual white balance, measure YCbCr means
//#define MRV_AWB_MAN_RGB      1  //"01":
//#define MRV_AWB_MAN_NO       0  //"00": manual white balance, no measurements
#if (MARVIN_FEATURE_WB == MARVIN_FEATURE_WB_V5)
    // Slice: AWB_MEAS_MODE:
    // 1: RGB based measurement mode  (M5_v6 only)
    // 0: near white discrimination mode
    #define MRV_ISP_AWB_MEAS_MODE
    #define MRV_ISP_AWB_MEAS_MODE_MASK 0x80000000
    #define MRV_ISP_AWB_MEAS_MODE_SHIFT 31
    #define MRV_ISP_AWB_MEAS_MODE_RGB 1
#endif // (MARVIN_FEATURE_WB == MARVIN_FEATURE_WB_V5)
    #define MRV_ISP_AWB_MEAS_MODE_YCBCR 0

// Slice: AWB_MAX_EN:
// 1: enable Y_MAX compare
// 0: disable Y_MAX compare
// ignored if RGB measurement mode is enabled
#define MRV_ISP_AWB_MAX_EN
#define MRV_ISP_AWB_MAX_EN_MASK 0x00000004
#define MRV_ISP_AWB_MAX_EN_SHIFT 2
// Slice: AWB_MODE:
// AWB_MODE(1:0):
// 11: reserved
// 10: white balance measurement of YCbCr means
// 01: reserved
// 00: manual white balance (gain adjust possible), no measurement
#define MRV_ISP_AWB_MODE
#define MRV_ISP_AWB_MODE_MASK 0x00000003
#define MRV_ISP_AWB_MODE_SHIFT  0
#define MRV_ISP_AWB_MODE_MEAS   2 // 10: white balance measurement of YCbCr or RGB means (dependent on MRV_ISP_AWB_MEAS_MODE)
#define MRV_ISP_AWB_MODE_NOMEAS 0 // 00: manual white balance (gain adjust possible), no measurement

// Register: isp_awb_h_offs: Auto white balance horizontal offset of measure window (0x0114)
// Slice: AWB_H_OFFS:
// horizontal window offset in pixel
#define MRV_ISP_AWB_H_OFFS
#define MRV_ISP_AWB_H_OFFS_MASK 0x00000FFF
#define MRV_ISP_AWB_H_OFFS_SHIFT 0
#define MRV_ISP_AWB_H_OFFS_MIN 0
#define MRV_ISP_AWB_H_OFFS_MAX (MRV_ISP_AWB_H_OFFS_MASK >> MRV_ISP_AWB_H_OFFS_SHIFT)

// Register: isp_awb_v_offs: Auto white balance vertical offset of measure window (0x0118)
// Slice: AWB_V_OFFS:
// vertical window offset in lines
#define MRV_ISP_AWB_V_OFFS
#define MRV_ISP_AWB_V_OFFS_MASK 0x00000FFF
#define MRV_ISP_AWB_V_OFFS_SHIFT 0
#define MRV_ISP_AWB_V_OFFS_MIN 0
#define MRV_ISP_AWB_V_OFFS_MAX (MRV_ISP_AWB_V_OFFS_MASK >> MRV_ISP_AWB_V_OFFS_SHIFT)

// Register: isp_awb_h_size: Auto white balance horizontal window size (0x011c)
// Slice: AWB_H_SIZE:
// horizontal measurement window size in pixel
#define MRV_ISP_AWB_H_SIZE
#if (MARVIN_FEATURE_FRAMESIZE == MARVIN_FEATURE_12M6)
#define MRV_ISP_AWB_H_SIZE_MASK 0x00001FFF
#else
#define MRV_ISP_AWB_H_SIZE_MASK 0x00000FFF
#endif
#define MRV_ISP_AWB_H_SIZE_SHIFT 0
#define MRV_ISP_AWB_H_SIZE_MIN 0
#define MRV_ISP_AWB_H_SIZE_MAX (MRV_ISP_AWB_H_SIZE_MASK >> MRV_ISP_AWB_H_SIZE_SHIFT)

// Register: isp_awb_v_size: Auto white balance vertical window size (0x0120)
// Slice: AWB_V_SIZE:
// vertical measurement window size in lines
#define MRV_ISP_AWB_V_SIZE
#define MRV_ISP_AWB_V_SIZE_MASK 0x00000FFF
#define MRV_ISP_AWB_V_SIZE_SHIFT 0
#define MRV_ISP_AWB_V_SIZE_MIN 0
#define MRV_ISP_AWB_V_SIZE_MAX (MRV_ISP_AWB_V_SIZE_MASK >> MRV_ISP_AWB_V_SIZE_SHIFT)

// Register: isp_awb_frames: Auto white balance mean value over multiple frames (0x0124)
//#define MRV_AWB_FRAMES           0x00000007 //[2:0] rw number of frames-1 used for mean value calculation
// Slice: AWB_FRAMES:
// number of frames-1 used for mean value calculation (value of 0 means 1 frame, value of 7 means 8 frames)
#define MRV_ISP_AWB_FRAMES
#define MRV_ISP_AWB_FRAMES_MASK 0x00000007
#define MRV_ISP_AWB_FRAMES_SHIFT 0

// Register: isp_awb_ref: Auto white balance reference Cb/Cr values (0x0128)
//                        or maximum red/blue values
// Slice: AWB_REF_CR__MAX_R:
// - reference Cr value for AWB regulation, target for AWB
// - maximum red value, if RGB measurement mode is selected
#define MRV_ISP_AWB_REF_CR__MAX_R
#define MRV_ISP_AWB_REF_CR__MAX_R_MASK 0x0000FF00
#define MRV_ISP_AWB_REF_CR__MAX_R_SHIFT 8
// Slice: AWB_REF_CB__MAX_B:
// - reference Cb value for AWB regulation, target for AWB
// - maximum blue value, if RGB measurement mode is selected
#define MRV_ISP_AWB_REF_CB__MAX_B
#define MRV_ISP_AWB_REF_CB__MAX_B_MASK 0x000000FF
#define MRV_ISP_AWB_REF_CB__MAX_B_SHIFT 0

// Register: isp_awb_thresh: Auto white balance threshold values (0x012c)
//#define MRV_ISP_AWB_MAX_Y        0xFF000000 //[31:24] rw luminance maximum value
//#define MRV_AWB_MIN_Y__MAX_G     0x00FF0000 //[23:16] rw luminance minimum value, maximum green value
//#define MRV_AWB_MAX_CSUM         0x0000FF00 //[15:8] rw chrominance sum maximum value
//#define MRV_AWB_MIN_C            0x000000FF //[7:0] rw chrominance minimum value
// Slice: AWB_MAX_Y:
// - Luminance maximum value, only consider pixels with luminance smaller than threshold for the WB measurement (must be enabled by AWB_MODE(2)),
// - ignored if RGB measurement mode is enabled
#define MRV_ISP_AWB_MAX_Y
#define MRV_ISP_AWB_MAX_Y_MASK 0xFF000000
#define MRV_ISP_AWB_MAX_Y_SHIFT 24

// Slice: AWB_MIN_Y__MAX_G:
// - Luminance minimum value, only consider pixels with luminance greater than threshold for the WB measurement
// - maximum green value, if RGB measurement mode is selected
#define MRV_ISP_AWB_MIN_Y__MAX_G
#define MRV_ISP_AWB_MIN_Y__MAX_G_MASK 0x00FF0000
#define MRV_ISP_AWB_MIN_Y__MAX_G_SHIFT 16

// Slice: AWB_MAX_CSUM:
// - Chrominance sum maximum value, only consider pixels with Cb+Cr smaller than threshold for WB measurements
// - ignored if RGB measurement mode is enabled
#define MRV_ISP_AWB_MAX_CSUM
#define MRV_ISP_AWB_MAX_CSUM_MASK 0x0000FF00
#define MRV_ISP_AWB_MAX_CSUM_SHIFT 8
// Slice: AWB_MIN_C:
// - Chrominance minimum value, only consider pixels with Cb/Cr each greater than threshold value for WB measurements
// - ignored if RGB measurement mode is enabled
#define MRV_ISP_AWB_MIN_C
#define MRV_ISP_AWB_MIN_C_MASK 0x000000FF
#define MRV_ISP_AWB_MIN_C_SHIFT 0

// Register: isp_awb_gain_g: Auto white balance gain green (0x0138)
// Slice: AWB_GAIN_GR:
// gain value for green component in red line 100h = 1
#define MRV_ISP_AWB_GAIN_GR
#define MRV_ISP_AWB_GAIN_GR_MASK 0x03FF0000
#define MRV_ISP_AWB_GAIN_GR_SHIFT 16
#define MRV_ISP_AWB_GAIN_GR_MAX  (MRV_ISP_AWB_GAIN_GR_MASK >> MRV_ISP_AWB_GAIN_GR_SHIFT)
// Slice: AWB_GAIN_GB:
// gain value for green component in blue line 100h = 1
#define MRV_ISP_AWB_GAIN_GB
#define MRV_ISP_AWB_GAIN_GB_MASK 0x000003FF
#define MRV_ISP_AWB_GAIN_GB_SHIFT 0
#define MRV_ISP_AWB_GAIN_GB_MAX  (MRV_ISP_AWB_GAIN_GB_MASK >> MRV_ISP_AWB_GAIN_GB_SHIFT)

// Register: isp_awb_gain_rb: Auto white balance gain red and blue (0x013c)
// Slice: AWB_GAIN_R:
// gain value for red component 100h = 1
#define MRV_ISP_AWB_GAIN_R
#define MRV_ISP_AWB_GAIN_R_MASK 0x03FF0000
#define MRV_ISP_AWB_GAIN_R_SHIFT 16
#define MRV_ISP_AWB_GAIN_R_MAX  (MRV_ISP_AWB_GAIN_R_MASK >> MRV_ISP_AWB_GAIN_R_SHIFT)
// Slice: AWB_GAIN_B:
// gain value for blue component 100h = 1
#define MRV_ISP_AWB_GAIN_B
#define MRV_ISP_AWB_GAIN_B_MASK 0x000003FF
#define MRV_ISP_AWB_GAIN_B_SHIFT 0
#define MRV_ISP_AWB_GAIN_B_MAX  (MRV_ISP_AWB_GAIN_B_MASK >> MRV_ISP_AWB_GAIN_B_SHIFT)

// Register: isp_awb_white_cnt: Auto white balance white pixel count (0x0140)
// Slice: AWB_WHITE_CNT:
// White pixel count, number of "white pixels" found during last measurement, i.e. pixels included in mean value calculation
#define MRV_ISP_AWB_WHITE_CNT
#define MRV_ISP_AWB_WHITE_CNT_MASK 0x03FFFFFF
#define MRV_ISP_AWB_WHITE_CNT_SHIFT 0

// Register: isp_awb_mean: Auto white balance measured mean value (0x0144)
//#define MRV_AWB_MEAN_Y_G          0x00FF0000 //[23:16] rh mean value of Y within window and frames
//#define MRV_AWB_MEAN_CB_B         0x0000FF00 //[15:8] rh mean value of Cb within window and frames
//#define MRV_AWB_MEAN_CR_R         0x000000FF //[7:0] rh mean value of Cr within window and frames
// Slice: AWB_MEAN_Y_G:
// - mean value of Y within window and frames
// - mean value of green within window and frames if RGB measurement is enabled
#define MRV_ISP_AWB_MEAN_Y__G
#define MRV_ISP_AWB_MEAN_Y__G_MASK 0x00FF0000
#define MRV_ISP_AWB_MEAN_Y__G_SHIFT 16
// Slice: AWB_MEAN_CB_B:
// - mean value of Cb within window and frames
// - mean value of blue within window and frames if RGB measurement is enabled
#define MRV_ISP_AWB_MEAN_CB__B
#define MRV_ISP_AWB_MEAN_CB__B_MASK 0x0000FF00
#define MRV_ISP_AWB_MEAN_CB__B_SHIFT 8
// Slice: AWB_MEAN_CR_R:
// - mean value of Cr within window and frames
// - mean value of red within window and frames if RGB measurement is enabled
#define MRV_ISP_AWB_MEAN_CR__R
#define MRV_ISP_AWB_MEAN_CR__R_MASK 0x000000FF
#define MRV_ISP_AWB_MEAN_CR__R_SHIFT 0

#if (MARVIN_FEATURE_AUTO_EXPOSURE == MARVIN_FEATURE_AUTO_EXPOSURE_V1)
    // Register: isp_ae_h_offs: 0x548 auto exposure measurement window horizontal offset
    // Slice: ae_h_offs: horizontal window offset in pixels
    #define MRV_ISP_AE_H_OFFS
    #define MRV_ISP_AE_H_OFFS_MASK  0x00000FFF
    #define MRV_ISP_AE_H_OFFS_SHIFT 0

    // Register: isp_ae_v_offs: 0x54C auto exposure measurement window vertical offset
    // Slice: ae_v_offs: vertical window offset in pixels
    #define MRV_ISP_AE_V_OFFS
    #define MRV_ISP_AE_V_OFFS_MASK  0x00000FFF
    #define MRV_ISP_AE_V_OFFS_SHIFT 0

    // Register: isp_ae_h_size: 0x550 auto exposure measurement window horizontal size
    // Slice: ae_h_size: horizontal measurement window size in 2^ae_h_size pixels
    #define MRV_ISP_AE_H_SIZE
    #define MRV_ISP_AE_H_SIZE_MASK  0x0000000F
    #define MRV_ISP_AE_H_SIZE_SHIFT 0

    // Register: isp_ae_v_size: 0x554 auto exposure measurement window vertical size
    // Slice: ae_v_size: vertical measurement window size in 2^ae_v_size lines
    #define MRV_ISP_AE_V_SIZE
    #define MRV_ISP_AE_V_SIZE_MASK  0x0000000F
    #define MRV_ISP_AE_V_SIZE_SHIFT 0

    // Register: isp_ae_frames: 0x558 number of frames for mean value calculation
    // Slice: ae_frames: number of frames used for mean value calculation in 2^ae_frames frames
    #define MRV_ISP_AE_FRAMES
    #define MRV_ISP_AE_FRAMES_MASK  0x00000003
    #define MRV_ISP_AE_FRAMES_SHIFT 0

    // Register: isp_ae_hist_range: 0x55C auto exposure luminance histogram range
    //                                            //ae luminance histogram range (SFR_BASE + 55CH)[Reset value: 0000’D030H]ae_hist_ub ae_hist_lb
    //#define MRV_AE_HIST_UB           0x0000FF00 //[15:8] rw upper boundary for occurrence counting
    //#define MRV_AE_HIST_LB           0x000000FF //[7:0] rw lower boundary for occurrence counting
    // Slice: ae_hist_ub: upper boundary for occurrence counting
    #define MRV_ISP_AE_HIST_UB
    #define MRV_ISP_AE_HIST_UB_MASK  0x0000FF00
    #define MRV_ISP_AE_HIST_UB_SHIFT 8
    // Slice: ae_hist_lb: lower boundary for occurrence counting
    #define MRV_ISP_AE_HIST_LB
    #define MRV_ISP_AE_HIST_LB_MASK  0x000000FF
    #define MRV_ISP_AE_HIST_LB_SHIFT 0

    // Register: isp_ae_occ_y: 0x560 auto exposure luminance histogram occurrence
    // Slice: ae_occ_y: ratio of luminance values in histogram range compared to all luminance values (occurrence): (Yhist / Y * 128)
    #define MRV_ISP_AE_OCC_Y
    #define MRV_ISP_AE_OCC_Y_MASK  0x0000000F
    #define MRV_ISP_AE_OCC_Y_SHIFT 0

    // Register: isp_ae_mean_y: 0x564 auto exposure mean luminance value
    // Slice: ae_mean_y: luminance mean value inside measurement window
    #define MRV_ISP_AE_MEAN_Y
    #define MRV_ISP_AE_MEAN_Y_MASK  0x0000000F
    #define MRV_ISP_AE_MEAN_Y_SHIFT 0

    // Register: isp_ae_max_y: 0x568 auto exposure maximum luminance value
    // Slice: ae_max_y: maximum luminance value inside measurement window
    #define MRV_ISP_AE_MAX_Y
    #define MRV_ISP_AE_MAX_Y_MASK  0x0000000F
    #define MRV_ISP_AE_MAX_Y_SHIFT 0

    // Register: isp_ae_min_y: 0x56C auto exposure minimum luminance value
    // Slice: ae_min_y: minimum luminance value inside measurement window
    #define MRV_ISP_AE_MIN_Y
    #define MRV_ISP_AE_MIN_Y_MASK  0x0000000F
    #define MRV_ISP_AE_MIN_Y_SHIFT 0
#endif  // MARVIN_FEATURE_AUTO_EXPOSURE_V1

// Register: isp_cc_coeff_0: Color conversion coefficient 0 (0x0170)
// Slice: cc_coeff_0:
// coefficient 0 for color space conversion
#define MRV_ISP_CC_COEFF_0
#define MRV_ISP_CC_COEFF_0_MASK 0x000001FF
#define MRV_ISP_CC_COEFF_0_SHIFT 0

// Register: isp_cc_coeff_1: Color conversion coefficient 1 (0x0174)
// Slice: cc_coeff_1:
// coefficient 1 for color space conversion
#define MRV_ISP_CC_COEFF_1
#define MRV_ISP_CC_COEFF_1_MASK 0x000001FF
#define MRV_ISP_CC_COEFF_1_SHIFT 0

// Register: isp_cc_coeff_2: Color conversion coefficient 2 (0x0178)
// Slice: cc_coeff_2:
// coefficient 2 for color space conversion
#define MRV_ISP_CC_COEFF_2
#define MRV_ISP_CC_COEFF_2_MASK 0x000001FF
#define MRV_ISP_CC_COEFF_2_SHIFT 0

// Register: isp_cc_coeff_3: Color conversion coefficient 3 (0x017c)
// Slice: cc_coeff_3:
// coefficient 3 for color space conversion
#define MRV_ISP_CC_COEFF_3
#define MRV_ISP_CC_COEFF_3_MASK 0x000001FF
#define MRV_ISP_CC_COEFF_3_SHIFT 0

// Register: isp_cc_coeff_4: Color conversion coefficient 4 (0x0180)
// Slice: cc_coeff_4:
// coefficient 4 for color space conversion
#define MRV_ISP_CC_COEFF_4
#define MRV_ISP_CC_COEFF_4_MASK 0x000001FF
#define MRV_ISP_CC_COEFF_4_SHIFT 0

// Register: isp_cc_coeff_5: Color conversion coefficient 5 (0x0184)
// Slice: cc_coeff_5:
// coefficient 5 for color space conversion
#define MRV_ISP_CC_COEFF_5
#define MRV_ISP_CC_COEFF_5_MASK 0x000001FF
#define MRV_ISP_CC_COEFF_5_SHIFT 0

// Register: isp_cc_coeff_6: Color conversion coefficient 6 (0x0188)
// Slice: cc_coeff_6:
// coefficient 6 for color space conversion
#define MRV_ISP_CC_COEFF_6
#define MRV_ISP_CC_COEFF_6_MASK 0x000001FF
#define MRV_ISP_CC_COEFF_6_SHIFT 0

// Register: isp_cc_coeff_7: Color conversion coefficient 7 (0x018c)
// Slice: cc_coeff_7:
// coefficient 7 for color space conversion
#define MRV_ISP_CC_COEFF_7
#define MRV_ISP_CC_COEFF_7_MASK 0x000001FF
#define MRV_ISP_CC_COEFF_7_SHIFT 0

// Register: isp_cc_coeff_8: Color conversion coefficient 8 (0x0190)
// Slice: cc_coeff_8:
// coefficient 8 for color space conversion
#define MRV_ISP_CC_COEFF_8
#define MRV_ISP_CC_COEFF_8_MASK 0x000001FF
#define MRV_ISP_CC_COEFF_8_SHIFT 0

#if ((MARVIN_FEATURE_OUTPUT_FORMATTER >= MARVIN_FEATURE_OUTPUT_FORMATTER_V1) && \
     (MARVIN_FEATURE_OUTPUT_FORMATTER <= MARVIN_FEATURE_OUTPUT_FORMATTER_V2))
    // Register: isp_out_h_offs: Horizontal offset of output window (0x0194)
    // Slice: ISP_OUT_H_OFFS:
    // vertical pic offset in lines
    #define MRV_ISP_ISP_OUT_H_OFFS
    #define MRV_ISP_ISP_OUT_H_OFFS_MASK 0x00000FFF
    #define MRV_ISP_ISP_OUT_H_OFFS_SHIFT 0

    // Register: isp_out_v_offs: Vertical offset of output window (0x0198)
    // Slice: ISP_OUT_V_OFFS:
    // vertical pic offset in lines
    #define MRV_ISP_ISP_OUT_V_OFFS
    #define MRV_ISP_ISP_OUT_V_OFFS_MASK 0x00000FFF
    #define MRV_ISP_ISP_OUT_V_OFFS_SHIFT 0

    // Register: isp_out_h_size: Output horizontal picture size (0x019c)
    // Slice: ISP_OUT_H_SIZE:
    // horizontal picture size in pixel
    // if ISP_MODE is set to
    // 001-(ITU-R BT.656 YUV),
    // 010-( ITU-R BT.601 YUV),
    // 011-( ITU-R BT.601 Bayer RGB),
    // 101-( ITU-R BT.656 Bayer RGB)
    // only even numbers are accepted, because complete quadruples of YUYV(YCbYCr) are needed for the 422 output. (if an odd size is programmed the value will be truncated to even size)
    #define MRV_ISP_ISP_OUT_H_SIZE
    #define MRV_ISP_ISP_OUT_H_SIZE_MASK 0x00003FFF
    #define MRV_ISP_ISP_OUT_H_SIZE_SHIFT 0

    // Register: isp_out_v_size: Output vertical picture size (0x01a0)
    // Slice: ISP_OUT_V_SIZE:
    // vertical pic size in lines
    #define MRV_ISP_ISP_OUT_V_SIZE
    #define MRV_ISP_ISP_OUT_V_SIZE_MASK 0x00000FFF
    #define MRV_ISP_ISP_OUT_V_SIZE_SHIFT 0
#endif  // MARVIN_FEATURE_OUTPUT_FORMATTER

// Register: isp_demosaic: Demosaic parameters (0x01a4)
//#define MRV_DEMOSAIC_MODE        0x00000300 //[9:8] rw Bayer demosaicing texture detection mode
//                                            //"11": reserved
//                                            //"10": reserved
//#define MRV_DEMO_ENH         1  //"01": enhanced mode
//#define MRV_DEMO_STD         0  //"00": standard
//#define MRV_DEMOSAIC_TH          0x000000FF //[7:0] rw threshold for Bayer demosaicing txture detection
//                                            //0: no texture detection
// Slice: DEMOSAIC_BYPASS:
// 0: normal operation for RGB Bayer Pattern input
// 1: demosaicing bypass for Black&White input data
#define MRV_ISP_DEMOSAIC_BYPASS
#define MRV_ISP_DEMOSAIC_BYPASS_MASK 0x00000400
#define MRV_ISP_DEMOSAIC_BYPASS_SHIFT 10
//#if (MARVIN_FEATURE_??? == MARVIN_FEATURE_???_V?)
    // Slice: DEMOSAIC_MODE:
    // Bayer demosaicing texture detection mode
    // "11": reserved
    // "10": reserved
    // "01": enhanced mode
    // "00": standard
    #define MRV_ISP_DEMOSAIC_MODE
    #define MRV_ISP_DEMOSAIC_MODE_MASK  0x00000300
    #define MRV_ISP_DEMOSAIC_MODE_SHIFT 8
    #define MRV_ISP_DEMOSAIC_MODE_STD   0 //"00": standard
    #define MRV_ISP_DEMOSAIC_MODE_ENH   1 //"01": enhanced mode
//#endif // MARVIN_FEATURE_???
// Slice: DEMOSAIC_TH:
// Threshold for Bayer demosaicing texture detection. This value shifted left 4bit is compared with the difference of the vertical and horizontal 12Bit wide texture indicators, to decide if the vertical or horizontal texture flag must be set.
// 0xFF: no texture detection
// 0x00: maximum edge sensitivity
#define MRV_ISP_DEMOSAIC_TH
#define MRV_ISP_DEMOSAIC_TH_MASK 0x000000FF
#define MRV_ISP_DEMOSAIC_TH_SHIFT 0

// Register: isp_flags_shd: Shadow flags (current status of certain signals) (0x01a8)
//    #define MRV_HSYNC_IN_PORT    0x80000000 //[31] rh hsync input port observation register
//    #define MRV_VSYNC_IN_PORT    0x40000000 //[30] rh vsync input port observation register
//    #define MRV_DATA_IN_PORT     0x0FFF0000 //[27:16] rh sensor data input port observation register
//#define MRV_IN_FIELD             0x00000004 //[2] rh current field information
//#define MRV_IN_ENABLE_SHD        0x00000002 //[1] rh input formatter enable shadow register
//#define MRV_ISP_ON_SHD           0x00000001 //[0] rh ISP enable shadow register
// Slice: S_HSYNC:
// state of ISP input port s_hsync, for test purposes
#define MRV_ISP_S_HSYNC
#if ((MARVIN_FEATURE_CAMBUSWIDTH == MARVIN_FEATURE_12BITS) || \
     (MARVIN_FEATURE_CAMBUSWIDTH == MARVIN_FEATURE_10BITS))
    #define MRV_ISP_S_HSYNC_MASK 0x80000000
    #define MRV_ISP_S_HSYNC_SHIFT 31
#elif (MARVIN_FEATURE_CAMBUSWIDTH == MARVIN_FEATURE_8BITS)
    #define MRV_ISP_S_HSYNC_MASK 0x02000000
    #define MRV_ISP_S_HSYNC_SHIFT 25
#else
    #error unknown MARVIN_FEATURE_CAMBUSWIDTH value
#endif // MARVIN_FEATURE_CAMBUSWIDTH
// Slice: S_VSYNC:
// state of ISP input port s_vsync, for test purposes
#define MRV_ISP_S_VSYNC
#if ((MARVIN_FEATURE_CAMBUSWIDTH == MARVIN_FEATURE_12BITS) || \
     (MARVIN_FEATURE_CAMBUSWIDTH == MARVIN_FEATURE_10BITS))
    #define MRV_ISP_S_VSYNC_MASK 0x40000000
    #define MRV_ISP_S_VSYNC_SHIFT 30
#elif (MARVIN_FEATURE_CAMBUSWIDTH == MARVIN_FEATURE_8BITS)
    #define MRV_ISP_S_VSYNC_MASK 0x01000000
    #define MRV_ISP_S_VSYNC_SHIFT 24
#else
    #error unknown MARVIN_FEATURE_CAMBUSWIDTH value
#endif // MARVIN_FEATURE_CAMBUSWIDTH
// Slice: S_DATA:
// state of ISP input port s_data, for test purposes
#define MRV_ISP_S_DATA
#if (MARVIN_FEATURE_CAMBUSWIDTH == MARVIN_FEATURE_12BITS)
    #define MRV_ISP_S_DATA_MASK 0x0FFF0000
#elif (MARVIN_FEATURE_CAMBUSWIDTH == MARVIN_FEATURE_10BITS)
    #define MRV_ISP_S_DATA_MASK 0x03FF0000
#elif (MARVIN_FEATURE_CAMBUSWIDTH == MARVIN_FEATURE_8BITS)
    #define MRV_ISP_S_DATA_MASK 0x00FF0000
#else
    #error unknown MARVIN_FEATURE_CAMBUSWIDTH value
#endif // MARVIN_FEATURE_CAMBUSWIDTH
#define MRV_ISP_S_DATA_SHIFT 16
// Slice: INFORM_FIELD:
// current field information (0=odd, 1=even)
#define MRV_ISP_INFORM_FIELD
#define MRV_ISP_INFORM_FIELD_MASK 0x00000004
#define MRV_ISP_INFORM_FIELD_SHIFT 2
#define MRV_ISP_INFORM_FIELD_ODD   0 // 0: odd field is sampled
#define MRV_ISP_INFORM_FIELD_EVEN  1 // 1: even field is sampled
// Slice: INFORM_EN_SHD:
// Input formatter enable shadow register
#define MRV_ISP_INFORM_EN_SHD
#define MRV_ISP_INFORM_EN_SHD_MASK 0x00000002
#define MRV_ISP_INFORM_EN_SHD_SHIFT 1
// Slice: ISP_ON_SHD:
// isp enable shadow register
// ISP currently outputs data (1) or not (0)
#define MRV_ISP_ISP_ON_SHD
#define MRV_ISP_ISP_ON_SHD_MASK 0x00000001
#define MRV_ISP_ISP_ON_SHD_SHIFT 0

#if ((MARVIN_FEATURE_OUTPUT_FORMATTER >= MARVIN_FEATURE_OUTPUT_FORMATTER_V1) && \
     (MARVIN_FEATURE_OUTPUT_FORMATTER <= MARVIN_FEATURE_OUTPUT_FORMATTER_V2))
    // Register: isp_out_h_offs_shd: current horizontal offset of output window (shadow register) (0x01ac)
    // Slice: ISP_OUT_H_OFFS_SHD:
    // current vertical pic offset in lines
    #define MRV_ISP_ISP_OUT_H_OFFS_SHD
    #define MRV_ISP_ISP_OUT_H_OFFS_SHD_MASK 0x00000FFF
    #define MRV_ISP_ISP_OUT_H_OFFS_SHD_SHIFT 0

    // Register: isp_out_v_offs_shd: current vertical offset of output window (shadow register) (0x01b0)
    // Slice: ISP_OUT_V_OFFS_SHD:
    // current vertical pic offset in lines
    #define MRV_ISP_ISP_OUT_V_OFFS_SHD
    #define MRV_ISP_ISP_OUT_V_OFFS_SHD_MASK 0x00000FFF
    #define MRV_ISP_ISP_OUT_V_OFFS_SHD_SHIFT 0

    // Register: isp_out_h_size_shd: current output horizontal picture size (shadow register) (0x01b4)
    // Slice: ISP_OUT_H_SIZE_SHD:
    // current horizontal pic size in pixel
    #define MRV_ISP_ISP_OUT_H_SIZE_SHD
    #define MRV_ISP_ISP_OUT_H_SIZE_SHD_MASK 0x00003FFF
    #define MRV_ISP_ISP_OUT_H_SIZE_SHD_SHIFT 0

    // Register: isp_out_v_size_shd: current output vertical picture size (shadow register) (0x01b8)
    // Slice: ISP_OUT_V_SIZE_SHD:
    // vertical pic size in lines
    #define MRV_ISP_ISP_OUT_V_SIZE_SHD
    #define MRV_ISP_ISP_OUT_V_SIZE_SHD_MASK 0x00000FFF
    #define MRV_ISP_ISP_OUT_V_SIZE_SHD_SHIFT 0
#endif  // MARVIN_FEATURE_OUTPUT_FORMATTER

                                            //ISP_IMSC 32-bit
                                            //interrupt mask register (SFR_BASE + 5BCH)[Reset value: 0000’0000H]
                                            //ISP_RIS 32-bit
                                            //raw interrupt status register (SFR_BASE + 5C0H)[Reset value: 0000’0000H]s_
                                            //ISP_MIS 32-bit
                                            //masked interrupt status register (SFR_BASE + 5C4H)[Reset value: 0000’0000H]s_
                                            //ISP_ICR 32-bit
                                            //interrupt clear register (SFR_BASE + 5C8H)[Reset value: 0000’0000H]r_
                                            //ISP_ISR 32-bit
                                            //interrupt set register (SFR_BASE + 5CCH)[Reset value: 0000’0000H]r_
// ISP_IMSC, ISP_RIS, ISP_MIS, ISP_ICR, ISP_ISR - all of these interrupt related registers use the same following bit configuration
//    #define MRV_ISPINT_FLASH_CAP        0x00020000 //[17] Signaling captured frame
//#define MRV_ISPINT_BP_DET           0x00010000 //[16] bad pixel was detected
//#define MRV_ISPINT_BP_NEW_TAB_FUL   0x00008000 //[15] new bad pixel table full
//#define MRV_ISPINT_AFM_FIN          0x00004000 //[14] AF measurement completely
//#define MRV_ISPINT_AFM_LUM_OF       0x00002000 //[13] overflow luminance
//#define MRV_ISPINT_AFM_SUM_OF       0x00001000 //[12] overflow summation of sharpness
//    #define MRV_ISPINT_SHUTTER_OFF      0x00000800 //[11] Mechanical shutter is switched off
//    #define MRV_ISPINT_SHUTTER_ON       0x00000400 //[10] Mechanical shutter is switched on
//    #define MRV_ISPINT_FLASH_OFF        0x00000200 //[ 9] Flash light is switched off
//    #define MRV_ISPINT_FLASH_ON         0x00000100 //[ 8] Flash light is switched on
//#define MRV_ISPINT_HSTART           0x00000080 //[7] hsync from sensor was detected
//#define MRV_ISPINT_VSTART           0x00000040 //[6] vsync from sensor was detected
//#define MRV_ISPINT_IN_FRAME_END     0x00000020 //[5] input image was sampled completely
//#define MRV_ISPINT_AWB_DONE         0x00000010 //[4] white balancing measurement cycle complete,results can be read out
//#define MRV_ISPINT_PIC_ERR          0x00000008 //[3] input picture size violation occurred,programming seems to be wrong
//#define MRV_ISPINT_DATA_LOSS        0x00000004 //[2] loss of data occurred within an active line,processing failure
//#define MRV_ISPINT_OUT_FRAME_END    0x00000002 //[1] current frame was completely put out
//#define MRV_ISPINT_ISP_OFF          0x00000001 //[0] ISP has switched off frame synchronized (was switched off manually or maximum frame count reached)
// Register: isp_imsc: Interrupt mask (0x01bc)
//#if (MARVIN_FEATURE_??? == MARVIN_FEATURE_???)  // [hf]: AUTO_EXPOSURE_V2 ?
    // Slice: IMSC_EXP_END:
    // enable interrupt (1) or mask out (0)
    #define MRV_ISP_IMSC_EXP_END
    #define MRV_ISP_IMSC_EXP_END_MASK 0x00040000
    #define MRV_ISP_IMSC_EXP_END_SHIFT 18
//#else // MARVIN_FEATURE_???
//    #define MRV_ISP_IMSC_EXP_END_MASK 0
//#endif // MARVIN_FEATURE_???
#if (MARVIN_FEATURE_FLASH_LIGHT == MARVIN_FEATURE_EXIST)
    // Slice: IMSC_FLASH_CAP:
    // enable interrupt (1) or mask out (0)
    #define MRV_ISP_IMSC_FLASH_CAP
    #define MRV_ISP_IMSC_FLASH_CAP_MASK 0x00020000
    #define MRV_ISP_IMSC_FLASH_CAP_SHIFT 17
#else // MARVIN_FEATURE_FLASH_LIGHT
    #define MRV_ISP_IMSC_FLASH_CAP_MASK 0
#endif // MARVIN_FEATURE_FLASH_LIGHT
// Slice: IMSC_BP_DET:
// enable interrupt (1) or mask out (0)
#define MRV_ISP_IMSC_BP_DET
#define MRV_ISP_IMSC_BP_DET_MASK 0x00010000
#define MRV_ISP_IMSC_BP_DET_SHIFT 16
// Slice: IMSC_BP_NEW_TAB_FUL:
// enable interrupt (1) or mask out (0)
#define MRV_ISP_IMSC_BP_NEW_TAB_FUL
#define MRV_ISP_IMSC_BP_NEW_TAB_FUL_MASK 0x00008000
#define MRV_ISP_IMSC_BP_NEW_TAB_FUL_SHIFT 15
// Slice: IMSC_AFM_FIN:
// enable interrupt (1) or mask out (0)
#define MRV_ISP_IMSC_AFM_FIN
#define MRV_ISP_IMSC_AFM_FIN_MASK 0x00004000
#define MRV_ISP_IMSC_AFM_FIN_SHIFT 14
// Slice: IMSC_AFM_LUM_OF:
// enable interrupt (1) or mask out (0)
#define MRV_ISP_IMSC_AFM_LUM_OF
#define MRV_ISP_IMSC_AFM_LUM_OF_MASK 0x00002000
#define MRV_ISP_IMSC_AFM_LUM_OF_SHIFT 13
// Slice: IMSC_AFM_SUM_OF:
// enable interrupt (1) or mask out (0)
#define MRV_ISP_IMSC_AFM_SUM_OF
#define MRV_ISP_IMSC_AFM_SUM_OF_MASK 0x00001000
#define MRV_ISP_IMSC_AFM_SUM_OF_SHIFT 12
#if (MARVIN_FEATURE_SHUTTER == MARVIN_FEATURE_EXIST)
    // Slice: IMSC_SHUTTER_OFF:
    // enable interrupt (1) or mask out (0)
    #define MRV_ISP_IMSC_SHUTTER_OFF
    #define MRV_ISP_IMSC_SHUTTER_OFF_MASK 0x00000800
    #define MRV_ISP_IMSC_SHUTTER_OFF_SHIFT 11
    // Slice: IMSC_SHUTTER_ON:
    // enable interrupt (1) or mask out (0)
    #define MRV_ISP_IMSC_SHUTTER_ON
    #define MRV_ISP_IMSC_SHUTTER_ON_MASK 0x00000400
    #define MRV_ISP_IMSC_SHUTTER_ON_SHIFT 10
#else // MARVIN_FEATURE_SHUTTER
    #define MRV_ISP_IMSC_SHUTTER_OFF_MASK 0
    #define MRV_ISP_IMSC_SHUTTER_ON_MASK 0
#endif // MARVIN_FEATURE_SHUTTER
#if (MARVIN_FEATURE_FLASH_LIGHT == MARVIN_FEATURE_EXIST)
    // Slice: IMSC_FLASH_OFF:
    // enable interrupt (1) or mask out (0)
    #define MRV_ISP_IMSC_FLASH_OFF
    #define MRV_ISP_IMSC_FLASH_OFF_MASK 0x00000200
    #define MRV_ISP_IMSC_FLASH_OFF_SHIFT 9
    // Slice: IMSC_FLASH_ON:
    // enable interrupt (1) or mask out (0)
    #define MRV_ISP_IMSC_FLASH_ON
    #define MRV_ISP_IMSC_FLASH_ON_MASK 0x00000100
    #define MRV_ISP_IMSC_FLASH_ON_SHIFT 8
#else // MARVIN_FEATURE_FLASH_LIGHT
    #define MRV_ISP_IMSC_FLASH_OFF_MASK 0
    #define MRV_ISP_IMSC_FLASH_ON_MASK 0
#endif // MARVIN_FEATURE_FLASH_LIGHT
// Slice: IMSC_H_START:
// enable interrupt (1) or mask out (0)
#define MRV_ISP_IMSC_H_START
#define MRV_ISP_IMSC_H_START_MASK 0x00000080
#define MRV_ISP_IMSC_H_START_SHIFT 7
// Slice: IMSC_V_START:
// enable interrupt (1) or mask out (0)
#define MRV_ISP_IMSC_V_START
#define MRV_ISP_IMSC_V_START_MASK 0x00000040
#define MRV_ISP_IMSC_V_START_SHIFT 6
// Slice: IMSC_FRAME_IN:
// enable interrupt (1) or mask out (0)
#define MRV_ISP_IMSC_FRAME_IN
#define MRV_ISP_IMSC_FRAME_IN_MASK 0x00000020
#define MRV_ISP_IMSC_FRAME_IN_SHIFT 5
// Slice: IMSC_AWB_DONE:
// enable interrupt (1) or mask out (0)
#define MRV_ISP_IMSC_AWB_DONE
#define MRV_ISP_IMSC_AWB_DONE_MASK 0x00000010
#define MRV_ISP_IMSC_AWB_DONE_SHIFT 4
// Slice: IMSC_PIC_SIZE_ERR:
// enable interrupt (1) or mask out (0)
#define MRV_ISP_IMSC_PIC_SIZE_ERR
#define MRV_ISP_IMSC_PIC_SIZE_ERR_MASK 0x00000008
#define MRV_ISP_IMSC_PIC_SIZE_ERR_SHIFT 3
// Slice: IMSC_DATA_LOSS:
// enable interrupt (1) or mask out (0)
#define MRV_ISP_IMSC_DATA_LOSS
#define MRV_ISP_IMSC_DATA_LOSS_MASK 0x00000004
#define MRV_ISP_IMSC_DATA_LOSS_SHIFT 2
// Slice: IMSC_FRAME:
// enable interrupt (1) or mask out (0)
#define MRV_ISP_IMSC_FRAME
#define MRV_ISP_IMSC_FRAME_MASK 0x00000002
#define MRV_ISP_IMSC_FRAME_SHIFT 1
// Slice: IMSC_ISP_OFF:
// enable interrupt (1) or mask out (0)
#define MRV_ISP_IMSC_ISP_OFF
#define MRV_ISP_IMSC_ISP_OFF_MASK 0x00000001
#define MRV_ISP_IMSC_ISP_OFF_SHIFT 0

// combination of all interrupt lines
#define MRV_ISP_IMSC_ALL
#define MRV_ISP_IMSC_ALL_MASK \
  (   0 \
    | MRV_ISP_IMSC_EXP_END_MASK \
    | MRV_ISP_IMSC_FLASH_CAP_MASK \
    | MRV_ISP_IMSC_BP_DET_MASK \
    | MRV_ISP_IMSC_BP_NEW_TAB_FUL_MASK \
    | MRV_ISP_IMSC_AFM_FIN_MASK \
    | MRV_ISP_IMSC_AFM_LUM_OF_MASK \
    | MRV_ISP_IMSC_AFM_SUM_OF_MASK \
    | MRV_ISP_IMSC_SHUTTER_OFF_MASK \
    | MRV_ISP_IMSC_SHUTTER_ON_MASK \
    | MRV_ISP_IMSC_FLASH_OFF_MASK \
    | MRV_ISP_IMSC_FLASH_ON_MASK \
    | MRV_ISP_IMSC_H_START_MASK \
    | MRV_ISP_IMSC_V_START_MASK \
    | MRV_ISP_IMSC_FRAME_IN_MASK \
    | MRV_ISP_IMSC_AWB_DONE_MASK \
    | MRV_ISP_IMSC_PIC_SIZE_ERR_MASK \
    | MRV_ISP_IMSC_DATA_LOSS_MASK \
    | MRV_ISP_IMSC_FRAME_MASK \
    | MRV_ISP_IMSC_ISP_OFF_MASK \
  )
#define MRV_ISP_IMSC_ALL_SHIFT 0

// Register: isp_ris: Raw interrupt status (0x01c0)
#if(   ( MARVIN_FEATURE_AUTO_EXPOSURE == MARVIN_FEATURE_AUTO_EXPOSURE_V2 )\
    || ( MARVIN_FEATURE_AUTO_EXPOSURE == MARVIN_FEATURE_AUTO_EXPOSURE_V4 ) )
    // Slice: RIS_EXP_END:
    // Exposure measurement complete
    #define MRV_ISP_RIS_EXP_END
    #define MRV_ISP_RIS_EXP_END_MASK 0x00040000
    #define MRV_ISP_RIS_EXP_END_SHIFT 18
#else // MARVIN_FEATURE_AUTO_EXPOSURE
    #define MRV_ISP_RIS_EXP_END_MASK 0
#endif // MARVIN_FEATURE_AUTO_EXPOSURE
#if (MARVIN_FEATURE_FLASH_LIGHT == MARVIN_FEATURE_EXIST)
    // Slice: RIS_FLASH_CAP:
    // Signaling captured frame
    #define MRV_ISP_RIS_FLASH_CAP
    #define MRV_ISP_RIS_FLASH_CAP_MASK 0x00020000
    #define MRV_ISP_RIS_FLASH_CAP_SHIFT 17
#else // MARVIN_FEATURE_FLASH_LIGHT
    #define MRV_ISP_RIS_FLASH_CAP_MASK 0
#endif // MARVIN_FEATURE_FLASH_LIGHT
// Slice: RIS_BP_DET:
// Bad pixel detected
#define MRV_ISP_RIS_BP_DET
#define MRV_ISP_RIS_BP_DET_MASK 0x00010000
#define MRV_ISP_RIS_BP_DET_SHIFT 16
// Slice: RIS_BP_NEW_TAB_FUL:
// New Bad Pixel table is full
#define MRV_ISP_RIS_BP_NEW_TAB_FUL
#define MRV_ISP_RIS_BP_NEW_TAB_FUL_MASK 0x00008000
#define MRV_ISP_RIS_BP_NEW_TAB_FUL_SHIFT 15
// Slice: RIS_AFM_FIN:
// AF measurement finished: this interrupt is set when the first complete frame is calculated after enabling the AF measurement
#define MRV_ISP_RIS_AFM_FIN
#define MRV_ISP_RIS_AFM_FIN_MASK 0x00004000
#define MRV_ISP_RIS_AFM_FIN_SHIFT 14
// Slice: RIS_AFM_LUM_OF:
// Auto focus luminance overflow
#define MRV_ISP_RIS_AFM_LUM_OF
#define MRV_ISP_RIS_AFM_LUM_OF_MASK 0x00002000
#define MRV_ISP_RIS_AFM_LUM_OF_SHIFT 13
// Slice: RIS_AFM_SUM_OF:
// Auto focus sum overflow
#define MRV_ISP_RIS_AFM_SUM_OF
#define MRV_ISP_RIS_AFM_SUM_OF_MASK 0x00001000
#define MRV_ISP_RIS_AFM_SUM_OF_SHIFT 12
#if (MARVIN_FEATURE_SHUTTER == MARVIN_FEATURE_EXIST)
    // Slice: RIS_SHUTTER_OFF:
    // Mechanical shutter is switched off
    #define MRV_ISP_RIS_SHUTTER_OFF
    #define MRV_ISP_RIS_SHUTTER_OFF_MASK 0x00000800
    #define MRV_ISP_RIS_SHUTTER_OFF_SHIFT 11
    // Slice: RIS_SHUTTER_ON:
    // Mechanical shutter is switched on
    #define MRV_ISP_RIS_SHUTTER_ON
    #define MRV_ISP_RIS_SHUTTER_ON_MASK 0x00000400
    #define MRV_ISP_RIS_SHUTTER_ON_SHIFT 10
#else // MARVIN_FEATURE_SHUTTER
    #define MRV_ISP_RIS_SHUTTER_OFF_MASK 0
    #define MRV_ISP_RIS_SHUTTER_ON_MASK 0
#endif // MARVIN_FEATURE_SHUTTER
#if (MARVIN_FEATURE_FLASH_LIGHT == MARVIN_FEATURE_EXIST)
    // Slice: RIS_FLASH_OFF:
    // Flash light is switched off
    #define MRV_ISP_RIS_FLASH_OFF
    #define MRV_ISP_RIS_FLASH_OFF_MASK 0x00000200
    #define MRV_ISP_RIS_FLASH_OFF_SHIFT 9
    // Slice: RIS_FLASH_ON:
    // Flash light is switched on
    #define MRV_ISP_RIS_FLASH_ON
    #define MRV_ISP_RIS_FLASH_ON_MASK 0x00000100
    #define MRV_ISP_RIS_FLASH_ON_SHIFT 8
#else // MARVIN_FEATURE_FLASH_LIGHT
    #define MRV_ISP_RIS_FLASH_OFF_MASK 0
    #define MRV_ISP_RIS_FLASH_ON_MASK 0
#endif // MARVIN_FEATURE_FLASH_LIGHT
// Slice: RIS_H_START:
// Start edge of h_sync
#define MRV_ISP_RIS_H_START
#define MRV_ISP_RIS_H_START_MASK 0x00000080
#define MRV_ISP_RIS_H_START_SHIFT 7
// Slice: RIS_V_START:
// Start edge of v_sync
#define MRV_ISP_RIS_V_START
#define MRV_ISP_RIS_V_START_MASK 0x00000040
#define MRV_ISP_RIS_V_START_SHIFT 6
// Slice: RIS_FRAME_IN:
// sampled input frame is complete
#define MRV_ISP_RIS_FRAME_IN
#define MRV_ISP_RIS_FRAME_IN_MASK 0x00000020
#define MRV_ISP_RIS_FRAME_IN_SHIFT 5
// Slice: RIS_AWB_DONE:
// White balancing measurement cycle is complete, results can be read out
#define MRV_ISP_RIS_AWB_DONE
#define MRV_ISP_RIS_AWB_DONE_MASK 0x00000010
#define MRV_ISP_RIS_AWB_DONE_SHIFT 4
// Slice: RIS_PIC_SIZE_ERR:
// pic size violation occurred, programming seems wrong
#define MRV_ISP_RIS_PIC_SIZE_ERR
#define MRV_ISP_RIS_PIC_SIZE_ERR_MASK 0x00000008
#define MRV_ISP_RIS_PIC_SIZE_ERR_SHIFT 3
// Slice: RIS_DATA_LOSS:
// loss of data occurred within a line, processing failure
#define MRV_ISP_RIS_DATA_LOSS
#define MRV_ISP_RIS_DATA_LOSS_MASK 0x00000004
#define MRV_ISP_RIS_DATA_LOSS_SHIFT 2
// Slice: RIS_FRAME:
// frame was completely put out
#define MRV_ISP_RIS_FRAME
#define MRV_ISP_RIS_FRAME_MASK 0x00000002
#define MRV_ISP_RIS_FRAME_SHIFT 1
// Slice: RIS_ISP_OFF:
// isp was turned off (vsynced) due to f_cnt reached or manual
#define MRV_ISP_RIS_ISP_OFF
#define MRV_ISP_RIS_ISP_OFF_MASK 0x00000001
#define MRV_ISP_RIS_ISP_OFF_SHIFT 0

// combination of all interrupt lines
#define MRV_ISP_RIS_ALL
#define MRV_ISP_RIS_ALL_MASK \
  (   0 \
    | MRV_ISP_RIS_EXP_END_MASK \
    | MRV_ISP_RIS_FLASH_CAP_MASK \
    | MRV_ISP_RIS_BP_DET_MASK \
    | MRV_ISP_RIS_BP_NEW_TAB_FUL_MASK \
    | MRV_ISP_RIS_AFM_FIN_MASK \
    | MRV_ISP_RIS_AFM_LUM_OF_MASK \
    | MRV_ISP_RIS_AFM_SUM_OF_MASK \
    | MRV_ISP_RIS_SHUTTER_OFF_MASK \
    | MRV_ISP_RIS_SHUTTER_ON_MASK \
    | MRV_ISP_RIS_FLASH_OFF_MASK \
    | MRV_ISP_RIS_FLASH_ON_MASK \
    | MRV_ISP_RIS_H_START_MASK \
    | MRV_ISP_RIS_V_START_MASK \
    | MRV_ISP_RIS_FRAME_IN_MASK \
    | MRV_ISP_RIS_AWB_DONE_MASK \
    | MRV_ISP_RIS_PIC_SIZE_ERR_MASK \
    | MRV_ISP_RIS_DATA_LOSS_MASK \
    | MRV_ISP_RIS_FRAME_MASK \
    | MRV_ISP_RIS_ISP_OFF_MASK \
  )
#define MRV_ISP_RIS_ALL_SHIFT 0

// Register: isp_mis: Masked interrupt status (0x01c4)
//#if (MARVIN_FEATURE_??? == MARVIN_FEATURE_???)
    // Slice: MIS_EXP_END:
    // Exposure measurement complete
    #define MRV_ISP_MIS_EXP_END
    #define MRV_ISP_MIS_EXP_END_MASK 0x00040000
    #define MRV_ISP_MIS_EXP_END_SHIFT 18
//#else // MARVIN_FEATURE_???
//    #define MRV_ISP_MIS_EXP_END_MASK 0
//#endif // MARVIN_FEATURE_???
#if (MARVIN_FEATURE_FLASH_LIGHT == MARVIN_FEATURE_EXIST)
    // Slice: MIS_FLASH_CAP:
    // Captured is frame is detected
    #define MRV_ISP_MIS_FLASH_CAP
    #define MRV_ISP_MIS_FLASH_CAP_MASK 0x00020000
    #define MRV_ISP_MIS_FLASH_CAP_SHIFT 17
#else // MARVIN_FEATURE_FLASH_LIGHT
    #define MRV_ISP_MIS_FLASH_CAP_MASK 0
#endif // MARVIN_FEATURE_FLASH_LIGHT
// Slice: MIS_BP_DET:
// Bad pixel detected
#define MRV_ISP_MIS_BP_DET
#define MRV_ISP_MIS_BP_DET_MASK 0x00010000
#define MRV_ISP_MIS_BP_DET_SHIFT 16
// Slice: MIS_BP_NEW_TAB_FUL:
// New Bad Pixel table is full
#define MRV_ISP_MIS_BP_NEW_TAB_FUL
#define MRV_ISP_MIS_BP_NEW_TAB_FUL_MASK 0x00008000
#define MRV_ISP_MIS_BP_NEW_TAB_FUL_SHIFT 15
// Slice: MIS_AFM_FIN:
// AF measurement finished: this interrupt is set when the first complete frame is calculated after enabling the AF measurement
#define MRV_ISP_MIS_AFM_FIN
#define MRV_ISP_MIS_AFM_FIN_MASK 0x00004000
#define MRV_ISP_MIS_AFM_FIN_SHIFT 14
// Slice: MIS_AFM_LUM_OF:
// Luminance overflow
#define MRV_ISP_MIS_AFM_LUM_OF
#define MRV_ISP_MIS_AFM_LUM_OF_MASK 0x00002000
#define MRV_ISP_MIS_AFM_LUM_OF_SHIFT 13
// Slice: MIS_AFM_SUM_OF:
// Sum overflow
#define MRV_ISP_MIS_AFM_SUM_OF
#define MRV_ISP_MIS_AFM_SUM_OF_MASK 0x00001000
#define MRV_ISP_MIS_AFM_SUM_OF_SHIFT 12
#if (MARVIN_FEATURE_SHUTTER == MARVIN_FEATURE_EXIST)
    // Slice: MIS_SHUTTER_OFF:
    // Mechanical shutter is switched off
    #define MRV_ISP_MIS_SHUTTER_OFF
    #define MRV_ISP_MIS_SHUTTER_OFF_MASK 0x00000800
    #define MRV_ISP_MIS_SHUTTER_OFF_SHIFT 11
    // Slice: MIS_SHUTTER_ON:
    // Mechanical shutter is switched on
    #define MRV_ISP_MIS_SHUTTER_ON
    #define MRV_ISP_MIS_SHUTTER_ON_MASK 0x00000400
    #define MRV_ISP_MIS_SHUTTER_ON_SHIFT 10
#else // MARVIN_FEATURE_SHUTTER
    #define MRV_ISP_MIS_SHUTTER_OFF_MASK 0
    #define MRV_ISP_MIS_SHUTTER_ON_MASK 0
#endif // MARVIN_FEATURE_SHUTTER
#if (MARVIN_FEATURE_FLASH_LIGHT == MARVIN_FEATURE_EXIST)
    // Slice: MIS_FLASH_OFF:
    // Flash light is switched off
    #define MRV_ISP_MIS_FLASH_OFF
    #define MRV_ISP_MIS_FLASH_OFF_MASK 0x00000200
    #define MRV_ISP_MIS_FLASH_OFF_SHIFT 9
    // Slice: MIS_FLASH_ON:
    // Flash light is switched on
    #define MRV_ISP_MIS_FLASH_ON
    #define MRV_ISP_MIS_FLASH_ON_MASK 0x00000100
    #define MRV_ISP_MIS_FLASH_ON_SHIFT 8
#else // MARVIN_FEATURE_FLASH_LIGHT
    #define MRV_ISP_MIS_FLASH_OFF_MASK 0
    #define MRV_ISP_MIS_FLASH_ON_MASK 0
#endif // MARVIN_FEATURE_FLASH_LIGHT
// Slice: MIS_H_START:
// Start edge of h_sync
#define MRV_ISP_MIS_H_START
#define MRV_ISP_MIS_H_START_MASK 0x00000080
#define MRV_ISP_MIS_H_START_SHIFT 7
// Slice: MIS_V_START:
// Start edge of v_sync
#define MRV_ISP_MIS_V_START
#define MRV_ISP_MIS_V_START_MASK 0x00000040
#define MRV_ISP_MIS_V_START_SHIFT 6
// Slice: MIS_FRAME_IN:
// sampled input frame is complete
#define MRV_ISP_MIS_FRAME_IN
#define MRV_ISP_MIS_FRAME_IN_MASK 0x00000020
#define MRV_ISP_MIS_FRAME_IN_SHIFT 5
// Slice: MIS_AWB_DONE:
// White balancing measurement cycle is complete, results can be read out
#define MRV_ISP_MIS_AWB_DONE
#define MRV_ISP_MIS_AWB_DONE_MASK 0x00000010
#define MRV_ISP_MIS_AWB_DONE_SHIFT 4
// Slice: MIS_PIC_SIZE_ERR:
// pic size violation occurred, programming seems wrong
#define MRV_ISP_MIS_PIC_SIZE_ERR
#define MRV_ISP_MIS_PIC_SIZE_ERR_MASK 0x00000008
#define MRV_ISP_MIS_PIC_SIZE_ERR_SHIFT 3
// Slice: MIS_DATA_LOSS:
// loss of data occurred within a line, processing failure
#define MRV_ISP_MIS_DATA_LOSS
#define MRV_ISP_MIS_DATA_LOSS_MASK 0x00000004
#define MRV_ISP_MIS_DATA_LOSS_SHIFT 2
// Slice: MIS_FRAME:
// frame was completely put out
#define MRV_ISP_MIS_FRAME
#define MRV_ISP_MIS_FRAME_MASK 0x00000002
#define MRV_ISP_MIS_FRAME_SHIFT 1
// Slice: MIS_ISP_OFF:
// isp was turned off (vsynced) due to f_cnt reached or manual
#define MRV_ISP_MIS_ISP_OFF
#define MRV_ISP_MIS_ISP_OFF_MASK 0x00000001
#define MRV_ISP_MIS_ISP_OFF_SHIFT 0

// combination of all interrupt lines
#define MRV_ISP_MIS_ALL
#define MRV_ISP_MIS_ALL_MASK \
  (   0 \
    | MRV_ISP_MIS_EXP_END_MASK \
    | MRV_ISP_MIS_FLASH_CAP_MASK \
    | MRV_ISP_MIS_BP_DET_MASK \
    | MRV_ISP_MIS_BP_NEW_TAB_FUL_MASK \
    | MRV_ISP_MIS_AFM_FIN_MASK \
    | MRV_ISP_MIS_AFM_LUM_OF_MASK \
    | MRV_ISP_MIS_AFM_SUM_OF_MASK \
    | MRV_ISP_MIS_SHUTTER_OFF_MASK \
    | MRV_ISP_MIS_SHUTTER_ON_MASK \
    | MRV_ISP_MIS_FLASH_OFF_MASK \
    | MRV_ISP_MIS_FLASH_ON_MASK \
    | MRV_ISP_MIS_H_START_MASK \
    | MRV_ISP_MIS_V_START_MASK \
    | MRV_ISP_MIS_FRAME_IN_MASK \
    | MRV_ISP_MIS_AWB_DONE_MASK \
    | MRV_ISP_MIS_PIC_SIZE_ERR_MASK \
    | MRV_ISP_MIS_DATA_LOSS_MASK \
    | MRV_ISP_MIS_FRAME_MASK \
    | MRV_ISP_MIS_ISP_OFF_MASK \
  )
#define MRV_ISP_MIS_ALL_SHIFT 0

// Register: isp_icr: Interrupt clear register (0x01c8)
//#if (MARVIN_FEATURE_??? == MARVIN_FEATURE_???)
    // Slice: ICR_EXP_END:
    // clear interrupt
    #define MRV_ISP_ICR_EXP_END
    #define MRV_ISP_ICR_EXP_END_MASK 0x00040000
    #define MRV_ISP_ICR_EXP_END_SHIFT 18
//#else // MARVIN_FEATURE_???
//    #define MRV_ISP_ICR_EXP_END_MASK 0
//#endif // MARVIN_FEATURE_???
#if (MARVIN_FEATURE_FLASH_LIGHT == MARVIN_FEATURE_EXIST)
    // Slice: ICR_FLASH_CAP:
    // clear interrupt
    #define MRV_ISP_ICR_FLASH_CAP
    #define MRV_ISP_ICR_FLASH_CAP_MASK 0x00020000
    #define MRV_ISP_ICR_FLASH_CAP_SHIFT 17
#else // MARVIN_FEATURE_FLASH_LIGHT
    #define MRV_ISP_ICR_FLASH_CAP_MASK 0
#endif // MARVIN_FEATURE_FLASH_LIGHT
// Slice: ICR_BP_DET:
// clear interrupt
#define MRV_ISP_ICR_BP_DET
#define MRV_ISP_ICR_BP_DET_MASK 0x00010000
#define MRV_ISP_ICR_BP_DET_SHIFT 16
// Slice: ICR_BP_NEW_TAB_FUL:
// clear interrupt
#define MRV_ISP_ICR_BP_NEW_TAB_FUL
#define MRV_ISP_ICR_BP_NEW_TAB_FUL_MASK 0x00008000
#define MRV_ISP_ICR_BP_NEW_TAB_FUL_SHIFT 15
// Slice: ICR_AFM_FIN:
// clear interrupt
#define MRV_ISP_ICR_AFM_FIN
#define MRV_ISP_ICR_AFM_FIN_MASK 0x00004000
#define MRV_ISP_ICR_AFM_FIN_SHIFT 14
// Slice: ICR_AFM_LUM_OF:
// clear interrupt
#define MRV_ISP_ICR_AFM_LUM_OF
#define MRV_ISP_ICR_AFM_LUM_OF_MASK 0x00002000
#define MRV_ISP_ICR_AFM_LUM_OF_SHIFT 13
// Slice: ICR_AFM_SUM_OF:
// clear interrupt
#define MRV_ISP_ICR_AFM_SUM_OF
#define MRV_ISP_ICR_AFM_SUM_OF_MASK 0x00001000
#define MRV_ISP_ICR_AFM_SUM_OF_SHIFT 12
#if (MARVIN_FEATURE_SHUTTER == MARVIN_FEATURE_EXIST)
    // Slice: ICR_SHUTTER_OFF:
    // clear interrupt
    #define MRV_ISP_ICR_SHUTTER_OFF
    #define MRV_ISP_ICR_SHUTTER_OFF_MASK 0x00000800
    #define MRV_ISP_ICR_SHUTTER_OFF_SHIFT 11
    // Slice: ICR_SHUTTER_ON:
    // clear interrupt
    #define MRV_ISP_ICR_SHUTTER_ON
    #define MRV_ISP_ICR_SHUTTER_ON_MASK 0x00000400
    #define MRV_ISP_ICR_SHUTTER_ON_SHIFT 10
#else // MARVIN_FEATURE_SHUTTER
    #define MRV_ISP_ICR_SHUTTER_OFF_MASK 0
    #define MRV_ISP_ICR_SHUTTER_ON_MASK 0
#endif // MARVIN_FEATURE_SHUTTER
#if (MARVIN_FEATURE_FLASH_LIGHT == MARVIN_FEATURE_EXIST)
    // Slice: ICR_FLASH_OFF:
    // clear interrupt
    #define MRV_ISP_ICR_FLASH_OFF
    #define MRV_ISP_ICR_FLASH_OFF_MASK 0x00000200
    #define MRV_ISP_ICR_FLASH_OFF_SHIFT 9
    // Slice: ICR_FLASH_ON:
    // clear interrupt
    #define MRV_ISP_ICR_FLASH_ON
    #define MRV_ISP_ICR_FLASH_ON_MASK 0x00000100
    #define MRV_ISP_ICR_FLASH_ON_SHIFT 8
#else // MARVIN_FEATURE_FLASH_LIGHT
    #define MRV_ISP_ICR_FLASH_OFF_MASK 0
    #define MRV_ISP_ICR_FLASH_ON_MASK 0
#endif // MARVIN_FEATURE_FLASH_LIGHT
// Slice: ICR_H_START:
// clear interrupt
#define MRV_ISP_ICR_H_START
#define MRV_ISP_ICR_H_START_MASK 0x00000080
#define MRV_ISP_ICR_H_START_SHIFT 7
// Slice: ICR_V_START:
// clear interrupt
#define MRV_ISP_ICR_V_START
#define MRV_ISP_ICR_V_START_MASK 0x00000040
#define MRV_ISP_ICR_V_START_SHIFT 6
// Slice: ICR_FRAME_IN:
// clear interrupt
#define MRV_ISP_ICR_FRAME_IN
#define MRV_ISP_ICR_FRAME_IN_MASK 0x00000020
#define MRV_ISP_ICR_FRAME_IN_SHIFT 5
// Slice: ICR_AWB_DONE:
// clear interrupt
#define MRV_ISP_ICR_AWB_DONE
#define MRV_ISP_ICR_AWB_DONE_MASK 0x00000010
#define MRV_ISP_ICR_AWB_DONE_SHIFT 4
// Slice: ICR_PIC_SIZE_ERR:
// clear interrupt
#define MRV_ISP_ICR_PIC_SIZE_ERR
#define MRV_ISP_ICR_PIC_SIZE_ERR_MASK 0x00000008
#define MRV_ISP_ICR_PIC_SIZE_ERR_SHIFT 3
// Slice: ICR_DATA_LOSS:
// clear interrupt
#define MRV_ISP_ICR_DATA_LOSS
#define MRV_ISP_ICR_DATA_LOSS_MASK 0x00000004
#define MRV_ISP_ICR_DATA_LOSS_SHIFT 2
// Slice: ICR_FRAME:
// clear interrupt
#define MRV_ISP_ICR_FRAME
#define MRV_ISP_ICR_FRAME_MASK 0x00000002
#define MRV_ISP_ICR_FRAME_SHIFT 1
// Slice: ICR_ISP_OFF:
// clear interrupt
#define MRV_ISP_ICR_ISP_OFF
#define MRV_ISP_ICR_ISP_OFF_MASK 0x00000001
#define MRV_ISP_ICR_ISP_OFF_SHIFT 0

// combination of all interrupt lines
#define MRV_ISP_ICR_ALL
#define MRV_ISP_ICR_ALL_MASK \
  (   0 \
    | MRV_ISP_ICR_EXP_END_MASK \
    | MRV_ISP_ICR_FLASH_CAP_MASK \
    | MRV_ISP_ICR_BP_DET_MASK \
    | MRV_ISP_ICR_BP_NEW_TAB_FUL_MASK \
    | MRV_ISP_ICR_AFM_FIN_MASK \
    | MRV_ISP_ICR_AFM_LUM_OF_MASK \
    | MRV_ISP_ICR_AFM_SUM_OF_MASK \
    | MRV_ISP_ICR_SHUTTER_OFF_MASK \
    | MRV_ISP_ICR_SHUTTER_ON_MASK \
    | MRV_ISP_ICR_FLASH_OFF_MASK \
    | MRV_ISP_ICR_FLASH_ON_MASK \
    | MRV_ISP_ICR_H_START_MASK \
    | MRV_ISP_ICR_V_START_MASK \
    | MRV_ISP_ICR_FRAME_IN_MASK \
    | MRV_ISP_ICR_AWB_DONE_MASK \
    | MRV_ISP_ICR_PIC_SIZE_ERR_MASK \
    | MRV_ISP_ICR_DATA_LOSS_MASK \
    | MRV_ISP_ICR_FRAME_MASK \
    | MRV_ISP_ICR_ISP_OFF_MASK \
  )
#define MRV_ISP_ICR_ALL_SHIFT 0

// Register: isp_isr: Interrupt set register (0x01cc)
//#if (MARVIN_FEATURE_??? == MARVIN_FEATURE_???)
    // Slice: ISR_EXP_END:
    // set interrupt
    #define MRV_ISP_ISR_EXP_END
    #define MRV_ISP_ISR_EXP_END_MASK 0x00040000
    #define MRV_ISP_ISR_EXP_END_SHIFT 18
//#else // MARVIN_FEATURE_???
//    #define MRV_ISP_ISR_EXP_END_MASK 0
//#endif // MARVIN_FEATURE_???
#if (MARVIN_FEATURE_FLASH_LIGHT == MARVIN_FEATURE_EXIST)
    // Slice: ISR_FLASH_CAP:
    // set interrupt
    #define MRV_ISP_ISR_FLASH_CAP
    #define MRV_ISP_ISR_FLASH_CAP_MASK 0x00020000
    #define MRV_ISP_ISR_FLASH_CAP_SHIFT 17
#else // MARVIN_FEATURE_FLASH_LIGHT
    #define MRV_ISP_ISR_FLASH_CAP_MASK 0
#endif // MARVIN_FEATURE_FLASH_LIGHT
// Slice: ISR_BP_DET:
// set interrupt
#define MRV_ISP_ISR_BP_DET
#define MRV_ISP_ISR_BP_DET_MASK 0x00010000
#define MRV_ISP_ISR_BP_DET_SHIFT 16
// Slice: ISR_BP_NEW_TAB_FUL:
// set interrupt
#define MRV_ISP_ISR_BP_NEW_TAB_FUL
#define MRV_ISP_ISR_BP_NEW_TAB_FUL_MASK 0x00008000
#define MRV_ISP_ISR_BP_NEW_TAB_FUL_SHIFT 15
// Slice: ISR_AFM_FIN:
// set interrupt
#define MRV_ISP_ISR_AFM_FIN
#define MRV_ISP_ISR_AFM_FIN_MASK 0x00004000
#define MRV_ISP_ISR_AFM_FIN_SHIFT 14
// Slice: ISR_AFM_LUM_OF:
// set interrupt
#define MRV_ISP_ISR_AFM_LUM_OF
#define MRV_ISP_ISR_AFM_LUM_OF_MASK 0x00002000
#define MRV_ISP_ISR_AFM_LUM_OF_SHIFT 13
// Slice: ISR_AFM_SUM_OF:
// set interrupt
#define MRV_ISP_ISR_AFM_SUM_OF
#define MRV_ISP_ISR_AFM_SUM_OF_MASK 0x00001000
#define MRV_ISP_ISR_AFM_SUM_OF_SHIFT 12
#if (MARVIN_FEATURE_SHUTTER == MARVIN_FEATURE_EXIST)
    // Slice: ISR_SHUTTER_OFF:
    // set interrupt
    #define MRV_ISP_ISR_SHUTTER_OFF
    #define MRV_ISP_ISR_SHUTTER_OFF_MASK 0x00000800
    #define MRV_ISP_ISR_SHUTTER_OFF_SHIFT 11
    // Slice: ISR_SHUTTER_ON:
    // set interrupt
    #define MRV_ISP_ISR_SHUTTER_ON
    #define MRV_ISP_ISR_SHUTTER_ON_MASK 0x00000400
    #define MRV_ISP_ISR_SHUTTER_ON_SHIFT 10
#else // MARVIN_FEATURE_SHUTTER
    #define MRV_ISP_ISR_SHUTTER_OFF_MASK 0
    #define MRV_ISP_ISR_SHUTTER_ON_MASK 0
#endif // MARVIN_FEATURE_SHUTTER
#if (MARVIN_FEATURE_FLASH_LIGHT == MARVIN_FEATURE_EXIST)
    // Slice: ISR_FLASH_OFF:
    // set interrupt
    #define MRV_ISP_ISR_FLASH_OFF
    #define MRV_ISP_ISR_FLASH_OFF_MASK 0x00000200
    #define MRV_ISP_ISR_FLASH_OFF_SHIFT 9
    // Slice: ISR_FLASH_ON:
    // set interrupt
    #define MRV_ISP_ISR_FLASH_ON
    #define MRV_ISP_ISR_FLASH_ON_MASK 0x00000100
    #define MRV_ISP_ISR_FLASH_ON_SHIFT 8
#else // MARVIN_FEATURE_FLASH_LIGHT
    #define MRV_ISP_ISR_FLASH_OFF_MASK 0
    #define MRV_ISP_ISR_FLASH_ON_MASK 0
#endif // MARVIN_FEATURE_FLASH_LIGHT
// Slice: ISR_H_START:
// set interrupt
#define MRV_ISP_ISR_H_START
#define MRV_ISP_ISR_H_START_MASK 0x00000080
#define MRV_ISP_ISR_H_START_SHIFT 7
// Slice: ISR_V_START:
// set interrupt
#define MRV_ISP_ISR_V_START
#define MRV_ISP_ISR_V_START_MASK 0x00000040
#define MRV_ISP_ISR_V_START_SHIFT 6
// Slice: ISR_FRAME_IN:
// set interrupt
#define MRV_ISP_ISR_FRAME_IN
#define MRV_ISP_ISR_FRAME_IN_MASK 0x00000020
#define MRV_ISP_ISR_FRAME_IN_SHIFT 5
// Slice: ISR_AWB_DONE:
// set interrupt
#define MRV_ISP_ISR_AWB_DONE
#define MRV_ISP_ISR_AWB_DONE_MASK 0x00000010
#define MRV_ISP_ISR_AWB_DONE_SHIFT 4
// Slice: ISR_PIC_SIZE_ERR:
// set interrupt
#define MRV_ISP_ISR_PIC_SIZE_ERR
#define MRV_ISP_ISR_PIC_SIZE_ERR_MASK 0x00000008
#define MRV_ISP_ISR_PIC_SIZE_ERR_SHIFT 3
// Slice: ISR_DATA_LOSS:
// set interrupt
#define MRV_ISP_ISR_DATA_LOSS
#define MRV_ISP_ISR_DATA_LOSS_MASK 0x00000004
#define MRV_ISP_ISR_DATA_LOSS_SHIFT 2
// Slice: ISR_FRAME:
// set interrupt
#define MRV_ISP_ISR_FRAME
#define MRV_ISP_ISR_FRAME_MASK 0x00000002
#define MRV_ISP_ISR_FRAME_SHIFT 1
// Slice: ISR_ISP_OFF:
// set interrupt
#define MRV_ISP_ISR_ISP_OFF
#define MRV_ISP_ISR_ISP_OFF_MASK 0x00000001
#define MRV_ISP_ISR_ISP_OFF_SHIFT 0

// combination of all interrupt lines
#define MRV_ISP_ISR_ALL
#define MRV_ISP_ISR_ALL_MASK \
  (   0 \
    | MRV_ISP_ISR_EXP_END_MASK \
    | MRV_ISP_ISR_FLASH_CAP_MASK \
    | MRV_ISP_ISR_BP_DET_MASK \
    | MRV_ISP_ISR_BP_NEW_TAB_FUL_MASK \
    | MRV_ISP_ISR_AFM_FIN_MASK \
    | MRV_ISP_ISR_AFM_LUM_OF_MASK \
    | MRV_ISP_ISR_AFM_SUM_OF_MASK \
    | MRV_ISP_ISR_SHUTTER_OFF_MASK \
    | MRV_ISP_ISR_SHUTTER_ON_MASK \
    | MRV_ISP_ISR_FLASH_OFF_MASK \
    | MRV_ISP_ISR_FLASH_ON_MASK \
    | MRV_ISP_ISR_H_START_MASK \
    | MRV_ISP_ISR_V_START_MASK \
    | MRV_ISP_ISR_FRAME_IN_MASK \
    | MRV_ISP_ISR_AWB_DONE_MASK \
    | MRV_ISP_ISR_PIC_SIZE_ERR_MASK \
    | MRV_ISP_ISR_DATA_LOSS_MASK \
    | MRV_ISP_ISR_FRAME_MASK \
    | MRV_ISP_ISR_ISP_OFF_MASK \
  )
#define MRV_ISP_ISR_ALL_SHIFT 0

#if ((MARVIN_FEATURE_XTALK >= MARVIN_FEATURE_XTALK_10BITS) && \
     (MARVIN_FEATURE_XTALK <= MARVIN_FEATURE_XTALK_11BITS))
    // Register array: isp_ct_coeff: cross-talk configuration register (color correction matrix) (0x01D0+n (n=0..8))
    // Slice: ct_coeff:
    // Coefficient n for cross talk matrix.
    //
    // Values are 11-bit signed fixed-point numbers with 4 bit integer and 7 bit fractional part,
    // ranging from -8 (0x400) to +7.992 (0x3FF). 0 is reprsented by 0x000 and a coefficient value of 1 as 0x080.
    #define MRV_ISP_CT_COEFF
  #if (MARVIN_FEATURE_XTALK == MARVIN_FEATURE_XTALK_10BITS)
    #define MRV_ISP_CT_COEFF_MASK 0x000003FF
  #elif (MARVIN_FEATURE_XTALK <= MARVIN_FEATURE_XTALK_11BITS)
    #define MRV_ISP_CT_COEFF_MASK 0x000007FF
  #endif // MARVIN_FEATURE_XTALK
    #define MRV_ISP_CT_COEFF_SHIFT 0
    #define MRV_ISP_CT_COEFF_MAX   (MRV_ISP_CT_COEFF_MASK >> MRV_ISP_CT_COEFF_SHIFT)
    #endif  // MARVIN_FEATURE_XTALK

#if (MARVIN_FEATURE_GAMMAOUT == MARVIN_FEATURE_EXIST)
    // Register: isp_gamma_out_mode: gamma segmentation mode register for output gamma (0x01f4)
    // Slice: equ_segm:
    // 0: logarithmic like segmentation of gamma curve (default after reset)
    //   segmentation from 0 to 4095: 64 64 64 64 128 128 128 128 256 256 256 512 512 512 512 512
    // 1: equidistant segmentation (all 16 segments are 256 )
    #define MRV_ISP_EQU_SEGM
    #define MRV_ISP_EQU_SEGM_MASK 0x00000001
    #define MRV_ISP_EQU_SEGM_SHIFT 0
    #define MRV_ISP_EQU_SEGM_LOG   0 // 0: logarithmic like segmentation of gamma curve (default after reset)
    #define MRV_ISP_EQU_SEGM_EQU   1 // 1: equidistant segmentation (all 16 segments are 256)

    // Register array: isp_gamma_out_y: Gamma Out Curve definition y (0x01F8+n (n=0..16))
    // Slice: isp_gamma_out_y:
    // Gamma_out curve point definition y-axis (output) for all color components (red,green,blue)
    // RESTRICTION: each Y_n must be in the +511/-512 range compared to its predecessor (so that dy is 10-bit signed !)
    #define MRV_ISP_ISP_GAMMA_OUT_Y
    #define MRV_ISP_ISP_GAMMA_OUT_Y_MASK 0x000003FF
    #define MRV_ISP_ISP_GAMMA_OUT_Y_SHIFT 0
#endif  // MARVIN_FEATURE_GAMMAOUT

#if (MARVIN_FEATURE_ISP_ERRORFLAGS == MARVIN_FEATURE_EXIST)
//    #define MRV_ERRFLAG_OUTFORM      0x00000004
//    #define MRV_ERRFLAG_IS           0x00000002
//    #define MRV_ERRFLAG_INFORM       0x00000001
    // Register: isp_err: ISP error register (0x023c)
    // Slice: outform_size_err:
    // size error is generated in outmux submodule
    #define MRV_ISP_OUTFORM_SIZE_ERR
    #define MRV_ISP_OUTFORM_SIZE_ERR_MASK 0x00000004
    #define MRV_ISP_OUTFORM_SIZE_ERR_SHIFT 2
    // Slice: is_size_err:
    // size error is generated in image stabilization submodule
    #define MRV_ISP_IS_SIZE_ERR
    #define MRV_ISP_IS_SIZE_ERR_MASK 0x00000002
    #define MRV_ISP_IS_SIZE_ERR_SHIFT 1
    // Slice: inform_size_err:
    // size error is generated in inform submodule
    #define MRV_ISP_INFORM_SIZE_ERR
    #define MRV_ISP_INFORM_SIZE_ERR_MASK 0x00000001
    #define MRV_ISP_INFORM_SIZE_ERR_SHIFT 0

    // combination of all isp error flags
    #define MRV_ISP_ALL_ERR
    #define MRV_ISP_ALL_ERR_MASK \
      (   0 \
        | MRV_ISP_OUTFORM_SIZE_ERR_MASK \
        | MRV_ISP_IS_SIZE_ERR_MASK      \
        | MRV_ISP_INFORM_SIZE_ERR_MASK  \
      )
    #define MRV_ISP_ALL_ERR_SHIFT 0

    // Register: isp_err_clr: ISP error clear register (0x0240)
    // Slice: outform_size_err_clr:
    // size error is cleared
    #define MRV_ISP_OUTFORM_SIZE_ERR_CLR
    #define MRV_ISP_OUTFORM_SIZE_ERR_CLR_MASK 0x00000004
    #define MRV_ISP_OUTFORM_SIZE_ERR_CLR_SHIFT 2
    // Slice: is_size_err_clr:
    // size error is cleared
    #define MRV_ISP_IS_SIZE_ERR_CLR
    #define MRV_ISP_IS_SIZE_ERR_CLR_MASK 0x00000002
    #define MRV_ISP_IS_SIZE_ERR_CLR_SHIFT 1
    // Slice: inform_size_err_clr:
    // size error is cleared
    #define MRV_ISP_INFORM_SIZE_ERR_CLR
    #define MRV_ISP_INFORM_SIZE_ERR_CLR_MASK 0x00000001
    #define MRV_ISP_INFORM_SIZE_ERR_CLR_SHIFT 0
#endif  // MARVIN_FEATURE_ISP_ERRORFLAGS

#if (MARVIN_FEATURE_FRAMECOUNTER == MARVIN_FEATURE_EXIST)
    // Register: isp_frame_count: Frame counter (0x0244)
    // Slice: frame_counter:
    // Current frame count of processing
    #define MRV_ISP_FRAME_COUNTER
    #define MRV_ISP_FRAME_COUNTER_MASK 0x000003FF
    #define MRV_ISP_FRAME_COUNTER_SHIFT 0
#endif  // MARVIN_FEATURE_FRAMECOUNTER

#if (MARVIN_FEATURE_XTALK_OFFSET == MARVIN_FEATURE_EXIST)
    // Register: isp_ct_offset_r: cross-talk offset red (0x0248)
    // Slice: ct_offset_r:
    // Offset red for cross talk matrix. Signed fixed-point number ranging from -2048 (0x800) to 2047 (0x7FF). 0 is represented as 0x000.
    #define MRV_ISP_CT_OFFSET_R
    #define MRV_ISP_CT_OFFSET_R_MASK 0x00000FFF
    #define MRV_ISP_CT_OFFSET_R_SHIFT 0

    // Register: isp_ct_offset_g: cross-talk offset green (0x024c)
    // Slice: ct_offset_g:
    // Offset green for cross talk matrix. Signed fixed-point number ranging from -2048 (0x800) to 2047 (0x7FF). 0 is represented as 0x000.
    #define MRV_ISP_CT_OFFSET_G
    #define MRV_ISP_CT_OFFSET_G_MASK 0x00000FFF
    #define MRV_ISP_CT_OFFSET_G_SHIFT 0

    // Register: isp_ct_offset_b: cross-talk offset blue (0x0250)
    // Slice: ct_offset_b:
    // Offset blue for cross talk matrix. Signed fixed-point number ranging from -2048 (0x800) to 2047 (0x7FF). 0 is represented as 0x000.
    #define MRV_ISP_CT_OFFSET_B
    #define MRV_ISP_CT_OFFSET_B_MASK 0x00000FFF
    #define MRV_ISP_CT_OFFSET_B_SHIFT 0
#endif  // MARVIN_FEATURE_XTALK_OFFSET

/********************************************************************/
/* ISP Flash Module Registers                                       */
/********************************************************************/

#if (MARVIN_FEATURE_FLASH_LIGHT == MARVIN_FEATURE_EXIST)
    // Register: isp_flash_cmd: Flash command (0x0000)
    // Slice: preflash_on:
    // preflash on
    // 0: no effect
    // 1: flash delay counter is started at next trigger event
    // No capture event is signaled to the sensor interface block.
    #define MRV_FLASH_PREFLASH_ON
    #define MRV_FLASH_PREFLASH_ON_MASK 0x00000004
    #define MRV_FLASH_PREFLASH_ON_SHIFT 2
    // Slice: flash_on:
    // flash on
    // 0: no effect
    // 1: flash delay counter is started at next trigger event
    // A capture event is signaled to the sensor interface block.
    #define MRV_FLASH_FLASH_ON
    #define MRV_FLASH_FLASH_ON_MASK 0x00000002
    #define MRV_FLASH_FLASH_ON_SHIFT 1
    // Slice: prelight_on:
    // prelight on
    // 0: prelight is switched off at next trigger event
    // 1: prelight is switched on at next trigger event
    #define MRV_FLASH_PRELIGHT_ON
    #define MRV_FLASH_PRELIGHT_ON_MASK 0x00000001
    #define MRV_FLASH_PRELIGHT_ON_SHIFT 0

    // Register: isp_flash_config: Flash config (0x0004)
    // Slice: fl_cap_del:
    // capture delay
    // frame number (0 to 15) to be captured after trigger event
    #define MRV_FLASH_FL_CAP_DEL
    #define MRV_FLASH_FL_CAP_DEL_MASK 0x000000F0
    #define MRV_FLASH_FL_CAP_DEL_SHIFT 4
    #define MRV_FLASH_FL_CAP_DEL_MAX (MRV_FLASH_FL_CAP_DEL_MASK >> MRV_FLASH_FL_CAP_DEL_SHIFT)
    // Slice: fl_trig_src:
    // trigger source for flash and prelight
    // 0: use “vds_vsync” for trigger event (with evaluation of vs_in_edge)
    // 1: use “fl_trig” for trigger event (positive edge)
    #define MRV_FLASH_FL_TRIG_SRC
    #define MRV_FLASH_FL_TRIG_SRC_MASK 0x00000008
    #define MRV_FLASH_FL_TRIG_SRC_SHIFT 3
    #define MRV_FLASH_FL_TRIG_SRC_VDS   0 // 0: use “vds_vsync” for trigger event (with evaluation of vs_in_edge)
    #define MRV_FLASH_FL_TRIG_SRC_FL    1 // 1: use “fl_trig” for trigger event (positive edge)
    // Slice: fl_pol:
    // polarity of flash related signals
    // 0: flash_trig, prelight_trig are high active
    // 1: flash_trig, prelight_trig are low active
    #define MRV_FLASH_FL_POL
    #define MRV_FLASH_FL_POL_MASK 0x00000004
    #define MRV_FLASH_FL_POL_SHIFT 2
    #define MRV_FLASH_FL_POL_HIGH  0 // 0: flash_trig, prelight_trig are high active
    #define MRV_FLASH_FL_POL_LOW   1 // 1: flash_trig, prelight_trig are low active
    // Slice: vs_in_edge:
    // VSYNC edge
    // 0: use negative edge of “vds_vsync” if generating a trigger event
    // 1: use positive edge of “vds_vsync” if generating a trigger event
    #define MRV_FLASH_VS_IN_EDGE
    #define MRV_FLASH_VS_IN_EDGE_MASK 0x00000002
    #define MRV_FLASH_VS_IN_EDGE_SHIFT 1
    #define MRV_FLASH_VS_IN_EDGE_NEG   0 // 0: use negative edge of “vds_vsync” if generating a trigger event
    #define MRV_FLASH_VS_IN_EDGE_POS   1 // 1: use positive edge of “vds_vsync” if generating a trigger event
    // Slice: prelight_mode:
    // prelight mode
    // 0: prelight is switched off at begin of flash
    // 1: prelight is switched off at end of flash
    #define MRV_FLASH_PRELIGHT_MODE
    #define MRV_FLASH_PRELIGHT_MODE_MASK 0x00000001
    #define MRV_FLASH_PRELIGHT_MODE_SHIFT 0
    #define MRV_FLASH_PRELIGHT_MODE_OASF  0 // 0: prelight is switched off at begin of flash
    #define MRV_FLASH_PRELIGHT_MODE_OAEF  1 // 1: prelight is switched off at end of flash

    // Register: isp_flash_prediv: Flash Counter Pre-Divider (0x0008)
    // Slice: fl_pre_div:
    // pre-divider for flush/preflash counter
    #define MRV_FLASH_FL_PRE_DIV
    #define MRV_FLASH_FL_PRE_DIV_MASK 0x000003FF
    #define MRV_FLASH_FL_PRE_DIV_SHIFT 0
    #define MRV_FLASH_FL_PRE_DIV_MAX (MRV_FLASH_FL_PRE_DIV_MASK >> MRV_FLASH_FL_PRE_DIV_SHIFT)

    // Register: isp_flash_delay: Flash Delay (0x000c)
    // Slice: fl_delay:
    // counter value for flash/preflash delay
    // open_delay =  (fl_delay + 1) * (fl_pre_div+1) / clk_isp
    // fl_delay =  (open_delay * clk_isp) / (fl_pre_div+1) - 1
    #define MRV_FLASH_FL_DELAY
    #define MRV_FLASH_FL_DELAY_MASK 0x0003FFFF
    #define MRV_FLASH_FL_DELAY_SHIFT 0
    #define MRV_FLASH_FL_DELAY_MAX (MRV_FLASH_FL_DELAY_MASK >> MRV_FLASH_FL_DELAY_SHIFT)

    // Register: isp_flash_time: Flash time (0x0010)
    // Slice: fl_time:
    // counter value for flash/preflash time
    // open_time =  (fl_time + 1) * (fl_pre_div+1) / clk_isp
    // fl_time =  (open_time * clk_isp) / (fl_pre_div+1) - 1
    #define MRV_FLASH_FL_TIME
    #define MRV_FLASH_FL_TIME_MASK 0x0003FFFF
    #define MRV_FLASH_FL_TIME_SHIFT 0
    #define MRV_FLASH_FL_TIME_MAX (MRV_FLASH_FL_TIME_MASK >> MRV_FLASH_FL_TIME_SHIFT)

    // Register: isp_flash_maxp: Maximum value for flash or preflash (0x0014)
    // Slice: fl_maxp:
    // maximum period value for flash or preflash
    // max. flash/preflash period = 214 * (fl_maxp + 1) / clk_isp
    // fl_maxp =  (max_period * clk_isp) / 214  - 1
    #define MRV_FLASH_FL_MAXP
    #define MRV_FLASH_FL_MAXP_MASK 0x0000FFFF
    #define MRV_FLASH_FL_MAXP_SHIFT 0
    #define MRV_FLASH_FL_MAXP_MAX (MRV_FLASH_FL_MAXP_MASK >> MRV_FLASH_FL_MAXP_SHIFT)
#endif  // MARVIN_FEATURE_FLASH_LIGHT

/********************************************************************/
/* ISP Shutter Module Registers                                     */
/********************************************************************/

#if (MARVIN_FEATURE_SHUTTER == MARVIN_FEATURE_EXIST)
    // Register: isp_sh_ctrl: mechanical shutter control (0x0000)
    // Slice: sh_open_pol:
    // shutter_open polarity
    // 0: shutter_open is high active
    // 1: shutter_open is low active
    #define MRV_SHUT_SH_OPEN_POL
    #define MRV_SHUT_SH_OPEN_POL_MASK 0x00000010
    #define MRV_SHUT_SH_OPEN_POL_SHIFT 4
    #define MRV_SHUT_SH_OPEN_POL_HIGH  0 // 0: shutter_open is high active
    #define MRV_SHUT_SH_OPEN_POL_LOW   1 // 1: shutter_open is low active
    // Slice: sh_trig_en:
    // mechanical shutter trigger edge
    // 0: use negative edge of trigger signal
    // 1: use positive edge of trigger signal
    #define MRV_SHUT_SH_TRIG_EN
    #define MRV_SHUT_SH_TRIG_EN_MASK 0x00000008
    #define MRV_SHUT_SH_TRIG_EN_SHIFT 3
    #define MRV_SHUT_SH_TRIG_EN_NEG   0 // 0: use negative edge of trigger signal
    #define MRV_SHUT_SH_TRIG_EN_POS   1 // 1: use positive edge of trigger signal
    // Slice: sh_trig_src:
    // mechanical shutter trigger source
    // 0: use “vds_vsync” for trigger event
    // 1: use “shutter_trig” for trigger event
    #define MRV_SHUT_SH_TRIG_SRC
    #define MRV_SHUT_SH_TRIG_SRC_MASK 0x00000004
    #define MRV_SHUT_SH_TRIG_SRC_SHIFT 2
    #define MRV_SHUT_SH_TRIG_SRC_VDS   0  // 0: use “vds_vsync” for trigger event
    #define MRV_SHUT_SH_TRIG_SRC_SHUT  1  // 1: use “shutter_trig” for trigger event
    // Slice: sh_rep_en:
    // mechanical shutter repetition enable
    // 0: shutter is opened only once
    // 1: shutter is opened with the repetition rate of the trigger signal
    #define MRV_SHUT_SH_REP_EN
    #define MRV_SHUT_SH_REP_EN_MASK 0x00000002
    #define MRV_SHUT_SH_REP_EN_SHIFT 1
    #define MRV_SHUT_SH_REP_EN_ONCE  0 // 0: shutter is opened only once
    #define MRV_SHUT_SH_REP_EN_REP   1 // 1: shutter is opened with the repetition rate of the trigger signal
    // Slice: sh_en:
    // mechanical shutter enable
    // 0: mechanical shutter function is disabled
    // 1: mechanical shutter function is enabled
    #define MRV_SHUT_SH_EN
    #define MRV_SHUT_SH_EN_MASK 0x00000001
    #define MRV_SHUT_SH_EN_SHIFT 0

    // Register: isp_sh_prediv: Mech. Shutter Counter Pre-Divider (0x0004)
    // Slice: sh_pre_div:
    // pre-divider for mechanical shutter open_delay and open_time counter
    #define MRV_SHUT_SH_PRE_DIV
    #define MRV_SHUT_SH_PRE_DIV_MASK 0x000003FF
    #define MRV_SHUT_SH_PRE_DIV_SHIFT 0
    #define MRV_SHUT_SH_PRE_DIV_MAX (MRV_SHUT_SH_PRE_DIV_MASK >> MRV_SHUT_SH_PRE_DIV_SHIFT)

    // Register: isp_sh_delay: Delay register (0x0008)
    // Slice: sh_delay:
    // counter value for delay
    // open_delay =  (sh_delay + 1) * (fl_pre_div+1) / clk_isp
    // sh_delay =  (open_delay * clk_isp) / (sh_pre_div+1) – 1
    #define MRV_SHUT_SH_DELAY
    #define MRV_SHUT_SH_DELAY_MASK 0x000FFFFF
    #define MRV_SHUT_SH_DELAY_SHIFT 0
    #define MRV_SHUT_SH_DELAY_MAX (MRV_SHUT_SH_DELAY_MASK >> MRV_SHUT_SH_DELAY_SHIFT)

    // Register: isp_sh_time: Time register (0x000c)
    // Slice: sh_time:
    // counter value for time
    // open_time =  (sh_time + 1) * (fl_pre_div+1) / clk_isp
    // sh_time =  (open_time * clk_isp) / (sh_pre_div+1) - 1
    #define MRV_SHUT_SH_TIME
    #define MRV_SHUT_SH_TIME_MASK 0x000FFFFF
    #define MRV_SHUT_SH_TIME_SHIFT 0
    #define MRV_SHUT_SH_TIME_MAX (MRV_SHUT_SH_TIME_MASK >> MRV_SHUT_SH_TIME_SHIFT)
#endif  // MARVIN_FEATURE_SHUTTER

/********************************************************************/
/* Color Processing Programming Registers                           */
/********************************************************************/

// Register: c_proc_ctrl: Global control register (0x0000)
// Slice: cproc_c_out_range:
// Color processing chrominance pixel clipping range at output
// 0: CbCr_out clipping range 16..240 according to ITU-R BT.601 standard
// 1: full UV_out clipping range 0..255
#define MRV_CPROC_CPROC_C_OUT_RANGE
#define MRV_CPROC_CPROC_C_OUT_RANGE_MASK 0x00000008
#define MRV_CPROC_CPROC_C_OUT_RANGE_SHIFT 3
#define MRV_CPROC_CPROC_C_OUT_RANGE_BT601 0 // 0: CbCr_out clipping range 16..240 according to ITU-R BT.601 standard
#define MRV_CPROC_CPROC_C_OUT_RANGE_FULL  1 // 1: full UV_out clipping range 0..255
// Slice: cproc_y_in_range:
// Color processing luminance input range (offset processing)
// 0: Y_in range 64..940 according to ITU-R BT.601 standard;
// offset of 64 will be subtracted from Y_in
// 1: Y_in full range 0..1023;  no offset will be subtracted from Y_in
#define MRV_CPROC_CPROC_Y_IN_RANGE
#define MRV_CPROC_CPROC_Y_IN_RANGE_MASK 0x00000004
#define MRV_CPROC_CPROC_Y_IN_RANGE_SHIFT 2
#define MRV_CPROC_CPROC_Y_IN_RANGE_BT601 0 // 0: Y_in range 64..940 according to ITU-R BT.601 standard
#define MRV_CPROC_CPROC_Y_IN_RANGE_FULL  1 // 1: Y_in full range 0..1023
// Slice: cproc_y_out_range:
// Color processing luminance output clipping range
// 0: Y_out clipping range 16..235; offset of 16 is added to Y_out according to ITU-R BT.601 standard
// 1: Y_out clipping range 0..255; no offset is added to Y_out
#define MRV_CPROC_CPROC_Y_OUT_RANGE
#define MRV_CPROC_CPROC_Y_OUT_RANGE_MASK 0x00000002
#define MRV_CPROC_CPROC_Y_OUT_RANGE_SHIFT 1
#define MRV_CPROC_CPROC_Y_OUT_RANGE_BT601 0 // 0: Y_out clipping range 16..235; offset of 16 is added to Y_out according to ITU-R BT.601 standard
#define MRV_CPROC_CPROC_Y_OUT_RANGE_FULL  1 // 1: Y_out clipping range 0..255; no offset is added to Y_out
// Slice: cproc_enable:
// color processing enable
// 0: color processing is bypassed
// 2 * 10 Bit input data are truncated to 2 * 8Bit output data
// 1: color processing is active
// output data are rounded to 2 * 8Bit and clipping is active
#define MRV_CPROC_CPROC_ENABLE
#define MRV_CPROC_CPROC_ENABLE_MASK 0x00000001
#define MRV_CPROC_CPROC_ENABLE_SHIFT 0

// Register: c_proc_contrast: Color Processing contrast register (0x0004)
// Slice: cproc_contrast:
// contrast adjustment value
// 00H equals x 0.0
// …
// 80H equals x 1.0
// …
// FFH equals x 1.992
#define MRV_CPROC_CPROC_CONTRAST
#define MRV_CPROC_CPROC_CONTRAST_MASK 0x000000FF
#define MRV_CPROC_CPROC_CONTRAST_SHIFT 0

// Register: c_proc_brightness: Color Processing brightness register (0x0008)
// Slice: cproc_brightness:
// brightness adjustment value
// 80H equals -128
// …
// 00H equals +0
// …
// 7FH equals +127
#define MRV_CPROC_CPROC_BRIGHTNESS
#define MRV_CPROC_CPROC_BRIGHTNESS_MASK 0x000000FF
#define MRV_CPROC_CPROC_BRIGHTNESS_SHIFT 0

// Register: c_proc_saturation: Color Processing saturation register (0x000c)
// Slice: cproc_saturation:
// saturation adjustment value
// 00H equals x 0.0
// …
// 80H equals x 1.0
// …
// FFH equals x 1.992
#define MRV_CPROC_CPROC_SATURATION
#define MRV_CPROC_CPROC_SATURATION_MASK 0x000000FF
#define MRV_CPROC_CPROC_SATURATION_SHIFT 0

// Register: c_proc_hue: Color Processing hue register (0x0010)
// Slice: cproc_hue:
// hue adjustment value
// 80H equals -90 deg
// …
// 00H equals 0 deg
// …
// 7FH equals +87.188 deg
#define MRV_CPROC_CPROC_HUE
#define MRV_CPROC_CPROC_HUE_MASK 0x000000FF
#define MRV_CPROC_CPROC_HUE_SHIFT 0

/********************************************************************/
/* Main and Self Resize Defines                                     */
/********************************************************************/

// bit mask for main and self resize scaler registers depending on scale factor width
#define MRV_RSZ_SCALE
#if (MARVIN_FEATURE_SCALE_FACTORWIDTH == MARVIN_FEATURE_12BITS)
    #define MRV_RSZ_SCALE_MASK 0x00000FFF
#elif (MARVIN_FEATURE_SCALE_FACTORWIDTH == MARVIN_FEATURE_14BITS)
    #define MRV_RSZ_SCALE_MASK 0x00003FFF
#elif (MARVIN_FEATURE_SCALE_FACTORWIDTH == MARVIN_FEATURE_16BITS)
    #define MRV_RSZ_SCALE_MASK 0x0000FFFF
#else
    #error unknown MARVIN_FEATURE_FACTORWIDTH value
#endif // MARVIN_FEATURE_SCALE_FACTORWIDTH
#define MRV_RSZ_SCALE_SHIFT 0
#define MRV_RSZ_SCALE_MAX (MRV_RSZ_SCALE_MASK >> MRV_RSZ_SCALE_SHIFT)

/********************************************************************/
/* Main Resize Programming Registers                                */
/********************************************************************/

// validate feature combination
#if (((MARVIN_FEATURE_MSCALE_UP         == MARVIN_FEATURE_EXIST) && \
      (MARVIN_FEATURE_MSCALE_FACTORCALC == MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV)) \
     || \
     ((MARVIN_FEATURE_MSCALE_UP         == MARVIN_FEATURE_EXIST_NOT) && \
      (MARVIN_FEATURE_MSCALE_FACTORCALC == MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV)))
    // this combinations are valid
#else
    #error invalid combination of MARVIN_FEATURE_MSCALE_UP and MARVIN_FEATURE_MSCALE_FACTORCALC values
#endif // MARVIN_FEATURE_SSCALE_UP <-> MARVIN_FEATURE_SSCALE_FACTORCALC

// Register: mrsz_ctrl: global control register (0x0000)
//#define MRV_CFG_UPD_C            0x00000020 //[5] w update chrominance scaler shadow registers
//#define MRV_CFG_UPD_Y            0x00000010 //[4] w update luminance scaler shadow registers
//#define MRV_SCALE_VC_EN          0x00000008 //[3] rw vertical chrominance scaling unit
//#define MRV_SCALE_VY_EN          0x00000004 //[2] rw vertical luminance scaling unit
//#define MRV_SCALE_HC_EN          0x00000002 //[1] rw horizontal chrominance scaling unit
//#define MRV_SCALE_HY_EN          0x00000001 //[0] rw horizontal luminance scaling unit
#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
    // Slice: cfg_upd:
    // write ‘0’: nothing happens
    // write ‘1’: update shadow registers
    // read: always ‘0’
    #define MRV_MRSZ_CFG_UPD
    #define MRV_MRSZ_CFG_UPD_MASK 0x00000100
    #define MRV_MRSZ_CFG_UPD_SHIFT 8
    // Slice: scale_vc_up:
    // 1: vertical chrominance upscaling selected
    // 0: vertical chrominance downscaling selected
    #define MRV_MRSZ_SCALE_VC_UP
    #define MRV_MRSZ_SCALE_VC_UP_MASK 0x00000080
    #define MRV_MRSZ_SCALE_VC_UP_SHIFT 7
    #define MRV_MRSZ_SCALE_VC_UP_UPSCALE   1 // 1: vertical chrominance upscaling selected
    #define MRV_MRSZ_SCALE_VC_UP_DOWNSCALE 0 // 0: vertical chrominance downscaling selected
    // Slice: scale_vy_up:
    // 1: vertical luminance upscaling selected
    // 0: vertical luminance downscaling selected
    #define MRV_MRSZ_SCALE_VY_UP
    #define MRV_MRSZ_SCALE_VY_UP_MASK 0x00000040
    #define MRV_MRSZ_SCALE_VY_UP_SHIFT 6
    #define MRV_MRSZ_SCALE_VY_UP_UPSCALE   1 // 1: vertical luminance upscaling selected
    #define MRV_MRSZ_SCALE_VY_UP_DOWNSCALE 0 // 0: vertical luminance downscaling selected
    // Slice: scale_hc_up:
    // 1: horizontal chrominance upscaling selected
    // 0: horizontal chrominance downscaling selected
    #define MRV_MRSZ_SCALE_HC_UP
    #define MRV_MRSZ_SCALE_HC_UP_MASK 0x00000020
    #define MRV_MRSZ_SCALE_HC_UP_SHIFT 5
    #define MRV_MRSZ_SCALE_HC_UP_UPSCALE   1 // 1: horizontal chrominance upscaling selected
    #define MRV_MRSZ_SCALE_HC_UP_DOWNSCALE 0 // 0: horizontal chrominance downscaling selected
    // Slice: scale_hy_up:
    // 1: horizontal luminance upscaling selected
    // 0: horizontal luminance downscaling selected
    #define MRV_MRSZ_SCALE_HY_UP
    #define MRV_MRSZ_SCALE_HY_UP_MASK 0x00000010
    #define MRV_MRSZ_SCALE_HY_UP_SHIFT 4
    #define MRV_MRSZ_SCALE_HY_UP_UPSCALE   1 // 1: horizontal luminance upscaling selected
    #define MRV_MRSZ_SCALE_HY_UP_DOWNSCALE 0 // 0: horizontal luminance downscaling selected
#else  // MARVIN_FEATURE_MSCALE_UP
    // Slice: cfg_upd_c:
    // write ‘0’: nothing happens
    // write ‘1’: update shadow registers of chrominance scaler
    // read: always ‘0’
    #define MRV_MRSZ_CFG_UPD_C
    #define MRV_MRSZ_CFG_UPD_C_MASK 0x00000020
    #define MRV_MRSZ_CFG_UPD_C_SHIFT 5
    // Slice: cfg_upd_y:
    // write ‘0’: nothing happens
    // write ‘1’: update shadow registers of luminance scaler
    // read: always ‘0’
    #define MRV_MRSZ_CFG_UPD_Y
    #define MRV_MRSZ_CFG_UPD_Y_MASK 0x00000010
    #define MRV_MRSZ_CFG_UPD_Y_SHIFT 4
#endif // MARVIN_FEATURE_MSCALE_UP
// Slice: scale_vc_enable:
// 0: bypass vertical chrominance scaling unit
// 1: enable vertical chrominance scaling unit
#define MRV_MRSZ_SCALE_VC_ENABLE
#define MRV_MRSZ_SCALE_VC_ENABLE_MASK 0x00000008
#define MRV_MRSZ_SCALE_VC_ENABLE_SHIFT 3
// Slice: scale_vy_enable:
// 0: bypass vertical luminance scaling unit
// 1: enable vertical luminance scaling unit
#define MRV_MRSZ_SCALE_VY_ENABLE
#define MRV_MRSZ_SCALE_VY_ENABLE_MASK 0x00000004
#define MRV_MRSZ_SCALE_VY_ENABLE_SHIFT 2
// Slice: scale_hc_enable:
// 0: bypass horizontal chrominance scaling unit
// 1: enable horizontal chrominance scaling unit
#define MRV_MRSZ_SCALE_HC_ENABLE
#define MRV_MRSZ_SCALE_HC_ENABLE_MASK 0x00000002
#define MRV_MRSZ_SCALE_HC_ENABLE_SHIFT 1
// Slice: scale_hy_enable:
// 0: bypass horizontal luminance scaling unit
// 1: enable horizontal luminance scaling unit
#define MRV_MRSZ_SCALE_HY_ENABLE
#define MRV_MRSZ_SCALE_HY_ENABLE_MASK 0x00000001
#define MRV_MRSZ_SCALE_HY_ENABLE_SHIFT 0

// Register: mrsz_scale_hy: horizontal luminance scale factor register (0x0004)
// Slice: scale_hy:
// This register is set to the horizontal luminance downscale factor or to the reciprocal of the horizontal luminance upscale factor
#define MRV_MRSZ_SCALE_HY
#define MRV_MRSZ_SCALE_HY_MASK  MRV_RSZ_SCALE_MASK
#define MRV_MRSZ_SCALE_HY_SHIFT MRV_RSZ_SCALE_SHIFT

#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
    // Register: mrsz_scale_hcb: horizontal Cb scale factor register (0x0008)
    // Slice: scale_hcb:
    // This register is set to the horizontal Cb downscale factor or to the reciprocal of the horizontal Cb upscale factor
    #define MRV_MRSZ_SCALE_HCB
    #define MRV_MRSZ_SCALE_HCB_MASK  MRV_RSZ_SCALE_MASK
    #define MRV_MRSZ_SCALE_HCB_SHIFT MRV_RSZ_SCALE_SHIFT

    // Register: mrsz_scale_hcr: horizontal Cr scale factor register (0x000c)
    // Slice: scale_hcr:
    // This register is set to the horizontal Cr downscale factor or to the reciprocal of the horizontal Cr upscale factor
    #define MRV_MRSZ_SCALE_HCR
    #define MRV_MRSZ_SCALE_HCR_MASK  MRV_RSZ_SCALE_MASK
    #define MRV_MRSZ_SCALE_HCR_SHIFT MRV_RSZ_SCALE_SHIFT
#else  // MARVIN_FEATURE_MSCALE_UP
    // Register: mrsz_scale_hc: horizontal chrominance scale factor register (0x0008)
    // Slice: scale_hc:
    // This register is set to the horizontal chrominance downscale factor
    #define MRV_MRSZ_SCALE_HC
    #define MRV_MRSZ_SCALE_HC_MASK  MRV_RSZ_SCALE_MASK
    #define MRV_MRSZ_SCALE_HC_SHIFT MRV_RSZ_SCALE_SHIFT
#endif  // MARVIN_FEATURE_MSCALE_UP

// Register: mrsz_scale_vy: vertical luminance scale factor register (0x0010)
// Slice: scale_vy:
// This register is set to the vertical luminance downscale factor or to the reciprocal of the vertical luminance upscale factor
#define MRV_MRSZ_SCALE_VY
#define MRV_MRSZ_SCALE_VY_MASK  MRV_RSZ_SCALE_MASK
#define MRV_MRSZ_SCALE_VY_SHIFT MRV_RSZ_SCALE_SHIFT

// Register: mrsz_scale_vc: vertical chrominance scale factor register (0x0014)
// Slice: scale_vc:
// This register is set to the vertical chrominance downscale factor or to the reciprocal of the vertical chrominance upscale factor
#define MRV_MRSZ_SCALE_VC
#define MRV_MRSZ_SCALE_VC_MASK  MRV_RSZ_SCALE_MASK
#define MRV_MRSZ_SCALE_VC_SHIFT MRV_RSZ_SCALE_SHIFT

// Register: mrsz_phase_hy: horizontal luminance phase register (0x0018)
// Slice: phase_hy:
// This register is set to the horizontal luminance phase offset
#define MRV_MRSZ_PHASE_HY
#define MRV_MRSZ_PHASE_HY_MASK  MRV_RSZ_SCALE_MASK
#define MRV_MRSZ_PHASE_HY_SHIFT MRV_RSZ_SCALE_SHIFT

// Register: mrsz_phase_hc: horizontal chrominance phase register (0x001c)
// Slice: phase_hc:
// This register is set to the horizontal chrominance phase offset
#define MRV_MRSZ_PHASE_HC
#define MRV_MRSZ_PHASE_HC_MASK  MRV_RSZ_SCALE_MASK
#define MRV_MRSZ_PHASE_HC_SHIFT MRV_RSZ_SCALE_SHIFT

// Register: mrsz_phase_vy: vertical luminance phase register (0x0020)
// Slice: phase_vy:
// This register is set to the vertical luminance phase offset
#define MRV_MRSZ_PHASE_VY
#define MRV_MRSZ_PHASE_VY_MASK  MRV_RSZ_SCALE_MASK
#define MRV_MRSZ_PHASE_VY_SHIFT MRV_RSZ_SCALE_SHIFT

// Register: mrsz_phase_vc: vertical chrominance phase register (0x0024)
// Slice: phase_vc:
// This register is set to the vertical chrominance phase offset
#define MRV_MRSZ_PHASE_VC
#define MRV_MRSZ_PHASE_VC_MASK  MRV_RSZ_SCALE_MASK
#define MRV_MRSZ_PHASE_VC_SHIFT MRV_RSZ_SCALE_SHIFT

#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
    // Register: mrsz_scale_lut_addr: Address pointer of up-scaling look up table (0x0028)
    // Slice: scale_lut_addr:
    // Pointer to entry of lookup table
    #define MRV_MRSZ_SCALE_LUT_ADDR
    #define MRV_MRSZ_SCALE_LUT_ADDR_MASK 0x0000003F
    #define MRV_MRSZ_SCALE_LUT_ADDR_SHIFT 0

    // Register: mrsz_scale_lut: Entry of up-scaling look up table (0x002c)
    // Slice: scale_lut:
    // Entry of lookup table at position scale_lut_addr. The lookup table must be filled with appropriate values before the up-scaling functionality can be used.
    #define MRV_MRSZ_SCALE_LUT
    #define MRV_MRSZ_SCALE_LUT_MASK 0x0000003F
    #define MRV_MRSZ_SCALE_LUT_SHIFT 0
#endif  // MARVIN_FEATURE_MSCALE_UP

// Register: mrsz_ctrl_shd: global control shadow register (0x0030)
#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
    // Slice: scale_vc_up_shd:
    // 1: vertical chrominance upscaling selected
    // 0: vertical chrominance downscaling selected
    #define MRV_MRSZ_SCALE_VC_UP_SHD
    #define MRV_MRSZ_SCALE_VC_UP_SHD_MASK 0x00000080
    #define MRV_MRSZ_SCALE_VC_UP_SHD_SHIFT 7
    #define MRV_MRSZ_SCALE_VC_UP_SHD_UPSCALE   1 // 1: vertical chrominance upscaling selected
    #define MRV_MRSZ_SCALE_VC_UP_SHD_DOWNSCALE 0 // 0: vertical chrominance downscaling selected
    // Slice: scale_vy_up_shd:
    // 1: vertical luminance upscaling selected
    // 0: vertical luminance downscaling selected
    #define MRV_MRSZ_SCALE_VY_UP_SHD
    #define MRV_MRSZ_SCALE_VY_UP_SHD_MASK 0x00000040
    #define MRV_MRSZ_SCALE_VY_UP_SHD_SHIFT 6
    #define MRV_MRSZ_SCALE_VY_UP_SHD_UPSCALE   1 // 1: vertical luminance upscaling selected
    #define MRV_MRSZ_SCALE_VY_UP_SHD_DOWNSCALE 0 // 0: vertical luminance downscaling selected
    // Slice: scale_hc_up_shd:
    // 1: horizontal chrominance upscaling selected
    // 0: horizontal chrominance downscaling selected
    #define MRV_MRSZ_SCALE_HC_UP_SHD
    #define MRV_MRSZ_SCALE_HC_UP_SHD_MASK 0x00000020
    #define MRV_MRSZ_SCALE_HC_UP_SHD_SHIFT 5
    #define MRV_MRSZ_SCALE_HC_UP_SHD_UPSCALE   1 // 1: horizontal chrominance upscaling selected
    #define MRV_MRSZ_SCALE_HC_UP_SHD_DOWNSCALE 0 // 0: horizontal chrominance downscaling selected
    // Slice: scale_hy_up_shd:
    // 1: horizontal luminance upscaling selected
    // 0: horizontal luminance downscaling selected
    #define MRV_MRSZ_SCALE_HY_UP_SHD
    #define MRV_MRSZ_SCALE_HY_UP_SHD_MASK 0x00000010
    #define MRV_MRSZ_SCALE_HY_UP_SHD_SHIFT 4
    #define MRV_MRSZ_SCALE_HY_UP_SHD_UPSCALE   1 // 1: horizontal luminance upscaling selected
    #define MRV_MRSZ_SCALE_HY_UP_SHD_DOWNSCALE 0 // 0: horizontal luminance downscaling selected
#else  // MARVIN_FEATURE_MSCALE_UP
#endif // MARVIN_FEATURE_MSCALE_UP
// Slice: scale_vc_enable_shd:
// 0: bypass vertical chrominance scaling unit
// 1: enable vertical chrominance scaling unit
#define MRV_MRSZ_SCALE_VC_ENABLE_SHD
#define MRV_MRSZ_SCALE_VC_ENABLE_SHD_MASK 0x00000008
#define MRV_MRSZ_SCALE_VC_ENABLE_SHD_SHIFT 3
// Slice: scale_vy_enable_shd:
// 0: bypass vertical luminance scaling unit
// 1: enable vertical luminance scaling unit
#define MRV_MRSZ_SCALE_VY_ENABLE_SHD
#define MRV_MRSZ_SCALE_VY_ENABLE_SHD_MASK 0x00000004
#define MRV_MRSZ_SCALE_VY_ENABLE_SHD_SHIFT 2
// Slice: scale_hc_enable_shd:
// 0: bypass horizontal chrominance scaling unit
// 1: enable horizontal chrominance scaling unit
#define MRV_MRSZ_SCALE_HC_ENABLE_SHD
#define MRV_MRSZ_SCALE_HC_ENABLE_SHD_MASK 0x00000002
#define MRV_MRSZ_SCALE_HC_ENABLE_SHD_SHIFT 1
// Slice: scale_hy_enable_shd:
// 0: bypass horizontal luminance scaling unit
// 1: enable horizontal luminance scaling unit
#define MRV_MRSZ_SCALE_HY_ENABLE_SHD
#define MRV_MRSZ_SCALE_HY_ENABLE_SHD_MASK 0x00000001
#define MRV_MRSZ_SCALE_HY_ENABLE_SHD_SHIFT 0

// Register: mrsz_scale_hy_shd: horizontal luminance scale factor shadow register (0x0034)
// Slice: scale_hy_shd:
// This register is set to the horizontal luminance downscale factor or to the reciprocal of the horizontal luminance upscale factor
#define MRV_MRSZ_SCALE_HY_SHD
#define MRV_MRSZ_SCALE_HY_SHD_MASK  MRV_RSZ_SCALE_MASK
#define MRV_MRSZ_SCALE_HY_SHD_SHIFT MRV_RSZ_SCALE_SHIFT

#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
    // Register: mrsz_scale_hcb_shd: horizontal Cb scale factor shadow register (0x0038)
    // Slice: scale_hcb_shd:
    // This register is set to the horizontal Cb downscale factor or to the reciprocal of the horizontal Cb upscale factor
    #define MRV_MRSZ_SCALE_HCB_SHD
    #define MRV_MRSZ_SCALE_HCB_SHD_MASK  MRV_RSZ_SCALE_MASK
    #define MRV_MRSZ_SCALE_HCB_SHD_SHIFT MRV_RSZ_SCALE_SHIFT

    // Register: mrsz_scale_hcr_shd: horizontal Cr scale factor shadow register (0x003c)
    // Slice: scale_hcr_shd:
    // This register is set to the horizontal Cr downscale factor or to the reciprocal of the horizontal Cr upscale factor
    #define MRV_MRSZ_SCALE_HCR_SHD
    #define MRV_MRSZ_SCALE_HCR_SHD_MASK  MRV_RSZ_SCALE_MASK
    #define MRV_MRSZ_SCALE_HCR_SHD_SHIFT MRV_RSZ_SCALE_SHIFT
#else  // MARVIN_FEATURE_MSCALE_UP
    // Register: mrsz_scale_hc_shd: horizontal chrominance scale factor shadow register (0x002c)
    // Slice: scale_hc_shd:
    // This register is set to the horizontal chrominance downscale factor
    #define MRV_MRSZ_SCALE_HC_SHD
    #define MRV_MRSZ_SCALE_HC_SHD_MASK  MRV_RSZ_SCALE_MASK
    #define MRV_MRSZ_SCALE_HC_SHD_SHIFT MRV_RSZ_SCALE_SHIFT
#endif  // MARVIN_FEATURE_MSCALE_UP

// Register: mrsz_scale_vy_shd: vertical luminance scale factor shadow register (0x0040)
// Slice: scale_vy_shd:
// This register is set to the vertical luminance downscale factor or to the reciprocal of the vertical luminance upscale factor
#define MRV_MRSZ_SCALE_VY_SHD
#define MRV_MRSZ_SCALE_VY_SHD_MASK  MRV_RSZ_SCALE_MASK
#define MRV_MRSZ_SCALE_VY_SHD_SHIFT MRV_RSZ_SCALE_SHIFT

// Register: mrsz_scale_vc_shd: vertical chrominance scale factor shadow register (0x0044)
// Slice: scale_vc_shd:
// This register is set to the vertical chrominance downscale factor or to the reciprocal of the vertical chrominance upscale factor
#define MRV_MRSZ_SCALE_VC_SHD
#define MRV_MRSZ_SCALE_VC_SHD_MASK  MRV_RSZ_SCALE_MASK
#define MRV_MRSZ_SCALE_VC_SHD_SHIFT MRV_RSZ_SCALE_SHIFT

// Register: mrsz_phase_hy_shd: horizontal luminance phase shadow register (0x0048)
// Slice: phase_hy_shd:
// This register is set to the horizontal luminance phase offset
#define MRV_MRSZ_PHASE_HY_SHD
#define MRV_MRSZ_PHASE_HY_SHD_MASK  MRV_RSZ_SCALE_MASK
#define MRV_MRSZ_PHASE_HY_SHD_SHIFT MRV_RSZ_SCALE_SHIFT

// Register: mrsz_phase_hc_shd: horizontal chrominance phase shadow register (0x004c)
// Slice: phase_hc_shd:
// This register is set to the horizontal chrominance phase offset
#define MRV_MRSZ_PHASE_HC_SHD
#define MRV_MRSZ_PHASE_HC_SHD_MASK  MRV_RSZ_SCALE_MASK
#define MRV_MRSZ_PHASE_HC_SHD_SHIFT MRV_RSZ_SCALE_SHIFT

// Register: mrsz_phase_vy_shd: vertical luminance phase shadow register (0x0050)
// Slice: phase_vy_shd:
// This register is set to the vertical luminance phase offset
#define MRV_MRSZ_PHASE_VY_SHD
#define MRV_MRSZ_PHASE_VY_SHD_MASK  MRV_RSZ_SCALE_MASK
#define MRV_MRSZ_PHASE_VY_SHD_SHIFT MRV_RSZ_SCALE_SHIFT

// Register: mrsz_phase_vc_shd: vertical chrominance phase shadow register (0x0054)
// Slice: phase_vc_shd:
// This register is set to the vertical chrominance phase offset
#define MRV_MRSZ_PHASE_VC_SHD
#define MRV_MRSZ_PHASE_VC_SHD_MASK  MRV_RSZ_SCALE_MASK
#define MRV_MRSZ_PHASE_VC_SHD_SHIFT MRV_RSZ_SCALE_SHIFT

/********************************************************************/
/* Self Resize Programming Registers                                */
/********************************************************************/

#if (MARVIN_FEATURE_SSCALE == MARVIN_FEATURE_EXIST)
  // validate feature combination
  #if (((MARVIN_FEATURE_SSCALE_UP         == MARVIN_FEATURE_EXIST) && \
        (MARVIN_FEATURE_SSCALE_FACTORCALC == MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV)) \
       || \
       ((MARVIN_FEATURE_SSCALE_UP         == MARVIN_FEATURE_EXIST_NOT) && \
        (MARVIN_FEATURE_SSCALE_FACTORCALC == MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV)))
    // this combinations are valid
  #else
    #error invalid combination of MARVIN_FEATURE_SSCALE_UP and MARVIN_FEATURE_SSCALE_FACTORCALC values
  #endif // MARVIN_FEATURE_SSCALE_UP <-> MARVIN_FEATURE_SSCALE_FACTORCALC

  // Register: srsz_ctrl: global control register (0x0000)
  //#define MRV_SP_CFG_UPD_Y            0x00000100 //[8] w update luminance scaler shadow registers
  //#define MRV_SP_SCALE_VC_UP          0x00000080 //[7] rw vertical chrominance scaling unit
  //#define MRV_SP_SCALE_VY_UP          0x00000040 //[6] rw vertical luminance scaling unit
  //#define MRV_SP_SCALE_HC_UP          0x00000020 //[5] rw horizontal chrominance scaling unit
  //#define MRV_SP_SCALE_HY_UP          0x00000010 //[4] rw horizontal luminance scaling unit
  //#define MRV_SP_SCALE_VC_EN          0x00000008 //[3] rw vertical chrominance scaling unit
  //#define MRV_SP_SCALE_VY_EN          0x00000004 //[2] rw vertical luminance scaling unit
  //#define MRV_SP_SCALE_HC_EN          0x00000002 //[1] rw horizontal chrominance scaling unit
  //#define MRV_SP_SCALE_HY_EN          0x00000001 //[0] rw horizontal luminance scaling unit
  #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
    // Slice: cfg_upd:
    // write ‘0’: nothing happens
    // write ‘1’: update shadow registers
    // read: always ‘0’
    #define MRV_SRSZ_CFG_UPD
    #define MRV_SRSZ_CFG_UPD_MASK 0x00000100
    #define MRV_SRSZ_CFG_UPD_SHIFT 8
    // Slice: scale_vc_up:
    // 1: vertical chrominance upscaling selected
    // 0: vertical chrominance downscaling selected
    #define MRV_SRSZ_SCALE_VC_UP
    #define MRV_SRSZ_SCALE_VC_UP_MASK 0x00000080
    #define MRV_SRSZ_SCALE_VC_UP_SHIFT 7
    #define MRV_SRSZ_SCALE_VC_UP_UPSCALE   1 // 1: vertical chrominance upscaling selected
    #define MRV_SRSZ_SCALE_VC_UP_DOWNSCALE 0 // 0: vertical chrominance downscaling selected
    // Slice: scale_vy_up:
    // 1: vertical luminance upscaling selected
    // 0: vertical luminance downscaling selected
    #define MRV_SRSZ_SCALE_VY_UP
    #define MRV_SRSZ_SCALE_VY_UP_MASK 0x00000040
    #define MRV_SRSZ_SCALE_VY_UP_SHIFT 6
    #define MRV_SRSZ_SCALE_VY_UP_UPSCALE   1 // 1: vertical luminance upscaling selected
    #define MRV_SRSZ_SCALE_VY_UP_DOWNSCALE 0// 0: vertical luminance downscaling selected
    // Slice: scale_hc_up:
    // 1: horizontal chrominance upscaling selected
    // 0: horizontal chrominance downscaling selected
    #define MRV_SRSZ_SCALE_HC_UP
    #define MRV_SRSZ_SCALE_HC_UP_MASK 0x00000020
    #define MRV_SRSZ_SCALE_HC_UP_SHIFT 5
    #define MRV_SRSZ_SCALE_HC_UP_UPSCALE   1 // 1: horizontal chrominance upscaling selected
    #define MRV_SRSZ_SCALE_HC_UP_DOWNSCALE 0// 0: horizontal chrominance downscaling selected
    // Slice: scale_hy_up:
    // 1: horizontal luminance upscaling selected
    // 0: horizontal luminance downscaling selected
    #define MRV_SRSZ_SCALE_HY_UP
    #define MRV_SRSZ_SCALE_HY_UP_MASK 0x00000010
    #define MRV_SRSZ_SCALE_HY_UP_SHIFT 4
    #define MRV_SRSZ_SCALE_HY_UP_UPSCALE   1 // 1: horizontal luminance upscaling selected
    #define MRV_SRSZ_SCALE_HY_UP_DOWNSCALE 0// 0: horizontal luminance downscaling selected
  #else   // MARVIN_FEATURE_SSCALE_UP
    // Slice: cfg_upd_c:
    // write ‘0’: nothing happens
    // write ‘1’: update shadow registers of chrominance scaler
    // read: always ‘0’
    #define MRV_SRSZ_CFG_UPD_C
    #define MRV_SRSZ_CFG_UPD_C_MASK 0x00000020
    #define MRV_SRSZ_CFG_UPD_C_SHIFT 5
    // Slice: cfg_upd_y:
    // write ‘0’: nothing happens
    // write ‘1’: update shadow registers of luminance scaler
    // read: always ‘0’
    #define MRV_SRSZ_CFG_UPD_Y
    #define MRV_SRSZ_CFG_UPD_Y_MASK 0x00000010
    #define MRV_SRSZ_CFG_UPD_Y_SHIFT 4
  #endif  // MARVIN_FEATURE_SSCALE_UP
  // Slice: scale_vc_enable:
  // 0: bypass vertical chrominance scaling unit
  // 1: enable vertical chrominance scaling unit
  #define MRV_SRSZ_SCALE_VC_ENABLE
  #define MRV_SRSZ_SCALE_VC_ENABLE_MASK 0x00000008
  #define MRV_SRSZ_SCALE_VC_ENABLE_SHIFT 3
  // Slice: scale_vy_enable:
  // 0: bypass vertical luminance scaling unit
  // 1: enable vertical luminance scaling unit
  #define MRV_SRSZ_SCALE_VY_ENABLE
  #define MRV_SRSZ_SCALE_VY_ENABLE_MASK 0x00000004
  #define MRV_SRSZ_SCALE_VY_ENABLE_SHIFT 2
  // Slice: scale_hc_enable:
  // 0: bypass horizontal chrominance scaling unit
  // 1: enable horizontal chrominance scaling unit
  #define MRV_SRSZ_SCALE_HC_ENABLE
  #define MRV_SRSZ_SCALE_HC_ENABLE_MASK 0x00000002
  #define MRV_SRSZ_SCALE_HC_ENABLE_SHIFT 1
  // Slice: scale_hy_enable:
  // 0: bypass horizontal luminance scaling unit
  // 1: enable horizontal luminance scaling unit
  #define MRV_SRSZ_SCALE_HY_ENABLE
  #define MRV_SRSZ_SCALE_HY_ENABLE_MASK 0x00000001
  #define MRV_SRSZ_SCALE_HY_ENABLE_SHIFT 0

  // Register: srsz_scale_hy: horizontal luminance scale factor register (0x0004)
  // Slice: scale_hy:
  // This register is set to the horizontal luminance downscale factor or to the reciprocal of the horizontal luminance upscale factor
  #define MRV_SRSZ_SCALE_HY
  #define MRV_SRSZ_SCALE_HY_MASK  MRV_RSZ_SCALE_MASK
  #define MRV_SRSZ_SCALE_HY_SHIFT MRV_RSZ_SCALE_SHIFT

  #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
    // Register: srsz_scale_hcb: horizontal chrominance scale factor register (0x0008)
    // Slice: scale_hcb:
    // This register is set to the horizontal Cb downscale factor or to the reciprocal of the horizontal Cb upscale factor
    #define MRV_SRSZ_SCALE_HCB
    #define MRV_SRSZ_SCALE_HCB_MASK  MRV_RSZ_SCALE_MASK
    #define MRV_SRSZ_SCALE_HCB_SHIFT MRV_RSZ_SCALE_SHIFT

    // Register: srsz_scale_hcr: horizontal chrominance scale factor register (0x000c)
    // Slice: scale_hcr:
    // This register is set to the horizontal Cr downscale factor or to the reciprocal of the horizontal Cr upscale factor
    #define MRV_SRSZ_SCALE_HCR
    #define MRV_SRSZ_SCALE_HCR_MASK  MRV_RSZ_SCALE_MASK
    #define MRV_SRSZ_SCALE_HCR_SHIFT MRV_RSZ_SCALE_SHIFT
  #else   // MARVIN_FEATURE_SSCALE_UP
    // Register: srsz_scale_hc: horizontal chrominance scale factor register (0x0008)
    // Slice: scale_hc:
    // This register is set to the horizontal chrominance scale factor
    #define MRV_SRSZ_SCALE_HC
    #define MRV_SRSZ_SCALE_HC_MASK  MRV_RSZ_SCALE_MASK
    #define MRV_SRSZ_SCALE_HC_SHIFT MRV_RSZ_SCALE_SHIFT
  #endif  // MARVIN_FEATURE_SSCALE_UP

  // Register: srsz_scale_vy: vertical luminance scale factor register (0x0010)
  // Slice: scale_vy:
  // This register is set to the vertical luminance downscale factor or to the reciprocal of the vertical luminance upscale factor
  #define MRV_SRSZ_SCALE_VY
  #define MRV_SRSZ_SCALE_VY_MASK  MRV_RSZ_SCALE_MASK
  #define MRV_SRSZ_SCALE_VY_SHIFT MRV_RSZ_SCALE_SHIFT

  // Register: srsz_scale_vc: vertical chrominance scale factor register (0x0014)
  // Slice: scale_vc:
  // This register is set to the vertical chrominance downscale factor or to the reciprocal of the vertical chrominance upscale factor
  #define MRV_SRSZ_SCALE_VC
  #define MRV_SRSZ_SCALE_VC_MASK  MRV_RSZ_SCALE_MASK
  #define MRV_SRSZ_SCALE_VC_SHIFT MRV_RSZ_SCALE_SHIFT

  // Register: srsz_phase_hy: horizontal luminance phase register (0x0018)
  // Slice: phase_hy:
  // This register is set to the horizontal luminance phase offset
  #define MRV_SRSZ_PHASE_HY
  #define MRV_SRSZ_PHASE_HY_MASK  MRV_RSZ_SCALE_MASK
  #define MRV_SRSZ_PHASE_HY_SHIFT MRV_RSZ_SCALE_SHIFT

  // Register: srsz_phase_hc: horizontal chrominance phase register (0x001c)
  // Slice: phase_hc:
  // This register is set to the horizontal chrominance phase offset
  #define MRV_SRSZ_PHASE_HC
  #define MRV_SRSZ_PHASE_HC_MASK  MRV_RSZ_SCALE_MASK
  #define MRV_SRSZ_PHASE_HC_SHIFT MRV_RSZ_SCALE_SHIFT

  // Register: srsz_phase_vy: vertical luminance phase register (0x0020)
  // Slice: phase_vy:
  // This register is set to the vertical luminance phase offset
  #define MRV_SRSZ_PHASE_VY
  #define MRV_SRSZ_PHASE_VY_MASK  MRV_RSZ_SCALE_MASK
  #define MRV_SRSZ_PHASE_VY_SHIFT MRV_RSZ_SCALE_SHIFT

  // Register: srsz_phase_vc: vertical chrominance phase register (0x0024)
  // Slice: phase_vc:
  // This register is set to the vertical chrominance phase offset
  #define MRV_SRSZ_PHASE_VC
  #define MRV_SRSZ_PHASE_VC_MASK  MRV_RSZ_SCALE_MASK
  #define MRV_SRSZ_PHASE_VC_SHIFT MRV_RSZ_SCALE_SHIFT

  #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
    // Register: srsz_scale_lut_addr: Address pointer of up-scaling look up table (0x0028)
    // Slice: scale_lut_addr:
    // Pointer to entry of lookup table
    #define MRV_SRSZ_SCALE_LUT_ADDR
    #define MRV_SRSZ_SCALE_LUT_ADDR_MASK 0x0000003F
    #define MRV_SRSZ_SCALE_LUT_ADDR_SHIFT 0

    // Register: srsz_scale_lut: Entry of up-scaling look up table (0x002c)
    // Slice: scale_lut:
    // Entry of lookup table at position scale_lut_addr. The lookup table must be filled with appropriate values before the up-scaling functionality can be used.
    #define MRV_SRSZ_SCALE_LUT
    #define MRV_SRSZ_SCALE_LUT_MASK 0x0000003F
    #define MRV_SRSZ_SCALE_LUT_SHIFT 0
  #endif  // MARVIN_FEATURE_SSCALE_UP

  // Register: srsz_ctrl_shd: global control shadow register (0x0030)
  #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
    // Slice: scale_vc_up_shd:
    // 1: vertical chrominance upscaling selected
    // 0: vertical chrominance downscaling selected
    #define MRV_SRSZ_SCALE_VC_UP_SHD
    #define MRV_SRSZ_SCALE_VC_UP_SHD_MASK 0x00000080
    #define MRV_SRSZ_SCALE_VC_UP_SHD_SHIFT 7
    #define MRV_SRSZ_SCALE_VC_UP_SHD_UPSCALE   1 // 1: vertical chrominance upscaling selected
    #define MRV_SRSZ_SCALE_VC_UP_SHD_DOWNSCALE 0 // 0: vertical chrominance downscaling selected
    // Slice: scale_vy_up_shd:
    // 1: vertical luminance upscaling selected
    // 0: vertical luminance downscaling selected
    #define MRV_SRSZ_SCALE_VY_UP_SHD
    #define MRV_SRSZ_SCALE_VY_UP_SHD_MASK 0x00000040
    #define MRV_SRSZ_SCALE_VY_UP_SHD_SHIFT 6
    #define MRV_SRSZ_SCALE_VY_UP_SHD_UPSCALE   1 // 1: vertical luminance upscaling selected
    #define MRV_SRSZ_SCALE_VY_UP_SHD_DOWNSCALE 0// 0: vertical luminance downscaling selected
    // Slice: scale_hc_up_shd:
    // 1: horizontal chrominance upscaling selected
    // 0: horizontal chrominance downscaling selected
    #define MRV_SRSZ_SCALE_HC_UP_SHD
    #define MRV_SRSZ_SCALE_HC_UP_SHD_MASK 0x00000020
    #define MRV_SRSZ_SCALE_HC_UP_SHD_SHIFT 5
    #define MRV_SRSZ_SCALE_HC_UP_SHD_UPSCALE   1 // 1: horizontal chrominance upscaling selected
    #define MRV_SRSZ_SCALE_HC_UP_SHD_DOWNSCALE 0// 0: horizontal chrominance downscaling selected
    // Slice: scale_hy_up_shd:
    // 1: horizontal luminance upscaling selected
    // 0: horizontal luminance downscaling selected
    #define MRV_SRSZ_SCALE_HY_UP_SHD
    #define MRV_SRSZ_SCALE_HY_UP_SHD_MASK 0x00000010
    #define MRV_SRSZ_SCALE_HY_UP_SHD_SHIFT 4
    #define MRV_SRSZ_SCALE_HY_UP_SHD_UPSCALE   1 // 1: horizontal luminance upscaling selected
    #define MRV_SRSZ_SCALE_HY_UP_SHD_DOWNSCALE 0// 0: horizontal luminance downscaling selected
  #endif  // MARVIN_FEATURE_SSCALE_UP
  // Slice: scale_vc_enable_shd:
  // 0: bypass vertical chrominance scaling unit
  // 1: enable vertical chrominance scaling unit
  #define MRV_SRSZ_SCALE_VC_ENABLE_SHD
  #define MRV_SRSZ_SCALE_VC_ENABLE_SHD_MASK 0x00000008
  #define MRV_SRSZ_SCALE_VC_ENABLE_SHD_SHIFT 3
  // Slice: scale_vy_enable_shd:
  // 0: bypass vertical luminance scaling unit
  // 1: enable vertical luminance scaling unit
  #define MRV_SRSZ_SCALE_VY_ENABLE_SHD
  #define MRV_SRSZ_SCALE_VY_ENABLE_SHD_MASK 0x00000004
  #define MRV_SRSZ_SCALE_VY_ENABLE_SHD_SHIFT 2
  // Slice: scale_hc_enable_shd:
  // 0: bypass horizontal chrominance scaling unit
  // 1: enable horizontal chrominance scaling unit
  #define MRV_SRSZ_SCALE_HC_ENABLE_SHD
  #define MRV_SRSZ_SCALE_HC_ENABLE_SHD_MASK 0x00000002
  #define MRV_SRSZ_SCALE_HC_ENABLE_SHD_SHIFT 1
  // Slice: scale_hy_enable_shd:
  // 0: bypass horizontal luminance scaling unit
  // 1: enable horizontal luminance scaling unit
  #define MRV_SRSZ_SCALE_HY_ENABLE_SHD
  #define MRV_SRSZ_SCALE_HY_ENABLE_SHD_MASK 0x00000001
  #define MRV_SRSZ_SCALE_HY_ENABLE_SHD_SHIFT 0

  // Register: srsz_scale_hy_shd: horizontal luminance scale factor shadow register (0x0034)
  // Slice: scale_hy_shd:
  // This register is set to the horizontal luminance downscale factor or to the reciprocal of the horizontal luminance upscale factor
  #define MRV_SRSZ_SCALE_HY_SHD
  #define MRV_SRSZ_SCALE_HY_SHD_MASK  MRV_RSZ_SCALE_MASK
  #define MRV_SRSZ_SCALE_HY_SHD_SHIFT MRV_RSZ_SCALE_SHIFT

  #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
    // Register: srsz_scale_hcb_shd: horizontal Cb scale factor shadow register (0x0038)
    // Slice: scale_hcb_shd:
    // This register is set to the horizontal Cb downscale factor or to the reciprocal of the horizontal Cb upscale factor
    #define MRV_SRSZ_SCALE_HCB_SHD
    #define MRV_SRSZ_SCALE_HCB_SHD_MASK  MRV_RSZ_SCALE_MASK
    #define MRV_SRSZ_SCALE_HCB_SHD_SHIFT MRV_RSZ_SCALE_SHIFT

    // Register: srsz_scale_hcr_shd: horizontal Cr scale factor shadow register (0x003c)
    // Slice: scale_hcr_shd:
    // This register is set to the horizontal r downscale factor or to the reciprocal of the horizontal r upscale factor
    #define MRV_SRSZ_SCALE_HCR_SHD
    #define MRV_SRSZ_SCALE_HCR_SHD_MASK  MRV_RSZ_SCALE_MASK
    #define MRV_SRSZ_SCALE_HCR_SHD_SHIFT MRV_RSZ_SCALE_SHIFT
  #else   // MARVIN_FEATURE_SSCALE_UP
    // Register: srsz_scale_hc_shd: horizontal chrominance scale factor shadow register (0x00??)
    // Slice: scale_hc_shd:
    // This register is set to the horizontal chrominance scale factor
    #define MRV_SRSZ_SCALE_HC_SHD
    #define MRV_SRSZ_SCALE_HC_SHD_MASK  MRV_RSZ_SCALE_MASK
    #define MRV_SRSZ_SCALE_HC_SHD_SHIFT MRV_RSZ_SCALE_SHIFT
  #endif  // MARVIN_FEATURE_SSCALE_UP

  // Register: srsz_scale_vy_shd: vertical luminance scale factor shadow register (0x0040)
  // Slice: scale_vy_shd:
  // This register is set to the vertical luminance downscale factor or to the reciprocal of the vertical luminance upscale factor
  #define MRV_SRSZ_SCALE_VY_SHD
  #define MRV_SRSZ_SCALE_VY_SHD_MASK  MRV_RSZ_SCALE_MASK
  #define MRV_SRSZ_SCALE_VY_SHD_SHIFT MRV_RSZ_SCALE_SHIFT

  // Register: srsz_scale_vc_shd: vertical chrominance scale factor shadow register (0x0044)
  // Slice: scale_vc_shd:
  // This register is set to the vertical chrominance downscale factor or to the reciprocal of the vertical chrominance upscale factor
  #define MRV_SRSZ_SCALE_VC_SHD
  #define MRV_SRSZ_SCALE_VC_SHD_MASK  MRV_RSZ_SCALE_MASK
  #define MRV_SRSZ_SCALE_VC_SHD_SHIFT MRV_RSZ_SCALE_SHIFT

  // Register: srsz_phase_hy_shd: horizontal luminance phase shadow register (0x0048)
  // Slice: phase_hy_shd:
  // This register is set to the horizontal luminance phase offset
  #define MRV_SRSZ_PHASE_HY_SHD
  #define MRV_SRSZ_PHASE_HY_SHD_MASK  MRV_RSZ_SCALE_MASK
  #define MRV_SRSZ_PHASE_HY_SHD_SHIFT MRV_RSZ_SCALE_SHIFT

  // Register: srsz_phase_hc_shd: horizontal chrominance phase shadow register (0x004c)
  // Slice: phase_hc_shd:
  // This register is set to the horizontal chrominance phase offset
  #define MRV_SRSZ_PHASE_HC_SHD
  #define MRV_SRSZ_PHASE_HC_SHD_MASK  MRV_RSZ_SCALE_MASK
  #define MRV_SRSZ_PHASE_HC_SHD_SHIFT MRV_RSZ_SCALE_SHIFT

  // Register: srsz_phase_vy_shd: vertical luminance phase shadow register (0x0050)
  // Slice: phase_vy_shd:
  // This register is set to the vertical luminance phase offset
  #define MRV_SRSZ_PHASE_VY_SHD
  #define MRV_SRSZ_PHASE_VY_SHD_MASK  MRV_RSZ_SCALE_MASK
  #define MRV_SRSZ_PHASE_VY_SHD_SHIFT MRV_RSZ_SCALE_SHIFT

  // Register: srsz_phase_vc_shd: vertical chrominance phase shadow register (0x0054)
  // Slice: phase_vc_shd:
  // This register is set to the vertical chrominance phase offset
  #define MRV_SRSZ_PHASE_VC_SHD
  #define MRV_SRSZ_PHASE_VC_SHD_MASK  MRV_RSZ_SCALE_MASK
  #define MRV_SRSZ_PHASE_VC_SHD_SHIFT MRV_RSZ_SCALE_SHIFT
#endif // MARVIN_FEATURE_SSCALE

/********************************************************************/
/* EMP Output Unit Programming Registers                           */
/********************************************************************/

#if (MARVIN_FEATURE_SMALL_OUTUNIT == MARVIN_FEATURE_EXIST)
    // Register: emp_out_ctrl: global control register (0x0000)
    // Slice: jpeg_enable:
    // Enables JPEG data
    //
    // Programmed value becomes effective after setting signal ‘soft_rst = 1’ or the two control bits ‘soft_upd_in = 1 and soft_upd_out = 1’.
    //
    // Affects MI_IN and MI_OUT module.
    #define MRV_EMPOUT_JPEG_ENABLE
    #define MRV_EMPOUT_JPEG_ENABLE_MASK 0x00000004
    #define MRV_EMPOUT_JPEG_ENABLE_SHIFT 2
    // Slice: mp_enable:
    // Enables main picture data
    //
    // Programmed value becomes effective after setting signal ‘soft_rst = 1’ or the two control bits ‘soft_upd_in = 1 and soft_upd_out = 1’ or input ‘main picture config update = 1’.
    //
    // Affects MI_IN and MI_OUT module.
    #define MRV_EMPOUT_MP_ENABLE
    #define MRV_EMPOUT_MP_ENABLE_MASK 0x00000001
    #define MRV_EMPOUT_MP_ENABLE_SHIFT 0

    // Register: emp_out_imsc: Interrupt Mask (0x00c8)
    // Slice: frame_end:
    // Mask bit for CIF end of frame interrupt
    #define MRV_EMPOUT_FRAME_END
    #define MRV_EMPOUT_FRAME_END_MASK 0x00000001
    #define MRV_EMPOUT_FRAME_END_SHIFT 0

    // Register: emp_out_ris: Raw Interrupt Status (0x00cc)
    /* same defines as for emp_out_imsc
    // Slice: frame_end:
    // Raw status of CIF end of frame interrupt
    #define MRV_EMPOUT_FRAME_END
    #define MRV_EMPOUT_FRAME_END_MASK 0x00000001
    #define MRV_EMPOUT_FRAME_END_SHIFT 0
    */

    // Register: emp_out_mis: Masked Interrupt Status (0x00d0)
    /* same defines as for emp_out_imsc
    // Slice: frame_end:
    // Masked status of CIF end of frame interrupt
    #define MRV_EMPOUT_FRAME_END
    #define MRV_EMPOUT_FRAME_END_MASK 0x00000001
    #define MRV_EMPOUT_FRAME_END_SHIFT 0
    */

    // Register: emp_out_icr: Interrupt Clear Register (0x00d4)
    /* same defines as for emp_out_imsc
    // Slice: frame_end:
    // Clear CIF end of frame interrupt
    #define MRV_EMPOUT_FRAME_END
    #define MRV_EMPOUT_FRAME_END_MASK 0x00000001
    #define MRV_EMPOUT_FRAME_END_SHIFT 0
    */

    // Register: emp_out_isr: Interrupt Set Register (0x00d8)
    /* same defines as for emp_out_imsc
    // Slice: frame_end:
    // Set CIF end of frame interrupt
    #define MRV_EMPOUT_FRAME_END
    #define MRV_EMPOUT_FRAME_END_MASK 0x00000001
    #define MRV_EMPOUT_FRAME_END_SHIFT 0
    */
#endif  // MARVIN_FEATURE_SMALL_OUTUNIT

/********************************************************************/
/* Memory Interface Programming Registers                           */
/********************************************************************/

// Register: mi_ctrl: Global control register (0x0000)
//#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)
//    #define MRV_SP_OUT_TYPE_MASK        0x70000000  //[30:28] rw set the output format of self picture
//    #define MRV_SP_OUT_RGB888_TYPE      0x60000000  //[30:28] rw RGB 888
//    #define MRV_SP_OUT_RGB666_TYPE      0x50000000  //[30:28] rw RGB 666
//    #define MRV_SP_OUT_RGB565_TYPE      0x40000000  //[30:28] rw RGB 565
//    #define MRV_SP_OUT_YUV444_TYPE      0x30000000  //[30:28] rw YCbCr 4:4:4
//    #define MRV_SP_OUT_YUV422_TYPE      0x20000000  //[30:28] rw YCbCr 4:2:2
//    #define MRV_SP_OUT_YUV420_TYPE      0x10000000  //[30:28] rw YCbCr 4:2:0
//    #define MRV_SP_OUT_YUV400_TYPE      0x00000000  //[30:28] rw YCbCr 4:0:0
//    #define MRV_SP_IN_TYPE_MASK         0x0C000000  //[27:26] rw set the input format of self picture
//    #define MRV_SP_IN_YUV444_TYPE       0x0C000000  //[27:26] rw YCbCr 4:4:4
//    #define MRV_SP_IN_YUV422_TYPE       0x08000000  //[27:26] rw YCbCr 4:2:2
//    #define MRV_SP_IN_YUV420_TYPE       0x04000000  //[27:26] rw YCbCr 4:2:0
//    #define MRV_SP_IN_YUV400_TYPE       0x00000000  //[27:26] rw YCbCr 4:0:0
//    #define MRV_SP_WRITE_FORMAT_MASK    0x03000000  //[25:24] rw defines how picture is written to memory
//    #define MRV_SP_WRITE_INTERLEAVED    0x02000000  //[25:24] rw interleaved (YCbCr 4:2:2 only)
//    #define MRV_SP_WRITE_SEMI_PLANAR    0x01000000  //[25:24] rw semi planar (YCbCr 4:2:x only)
//    #define MRV_SP_WRITE_PLANAR         0x00000000  //[25:24] rw planar      (all formats)
//    #define MRV_MP_WRITE_FORMAT_MASK    0x00C00000  //[23:22] rw defines how picture data is written to memory
//    #define MRV_MP_WRITE_INTERLEAVED    0x00800000  //[23:22] rw interleaved (YCbCr 4:2:2 only)
//    #define MRV_MP_WRITE_SEMI_PLANAR    0x00400000  //[23:22] rw semi planar (YCbCr 4:2:x only)
//    #define MRV_MP_WRITE_PLANAR         0x00000000  //[23:22] rw planar      (all formats)
//    #define MRV_INIT_OFFSET_EN          0x00200000  //[21] rw enables updating of the offset counters shadow registers
//    #define MRV_INIT_BASE_EN            0x00100000  //[20] rw enables updating of the base address and buffer size shadow
//    #define MRV_BURST_LEN_CHROM_8       0x00040000  //[18] rw 8-beat bursts
//    #define MRV_BURST_LEN_CHROM_4       0x00000000  //[18] rw 4-beat bursts
//    #define MRV_BURST_LEN_LUM_8         0x00010000  //[16] rw 8-beat bursts
//    #define MRV_BURST_LEN_LUM_4         0x00000000  //[16] rw 4-beat bursts
//    #define MRV_ROT_AND_FLIP_MASK       0x00000070  //[6:4] rw mask for rotation, horizontal and vertical flipping enable bits
//    #define MRV_ROTATION_CCW_EN         0x00000040  //[6] rw enable rotation 90 degree counter-clockwise of self-picture
//    #define MRV_SELF_V_MIRR_EN          0x00000020  //[5] rw enable vertical mirror for self-picture
//#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
//    #define MRV_DMA_PATH_SEL        0x00040000  //[18] rw Informs the MI which path(s) are used to output
//                                                //     the data coming from the DMA-read port.
//                                                //     0=self path only, 1=Main and/or self path through Image effects module
//    #define MRV_DMA_BYTE_SWAP       0x00020000  //[17] rw swap all data bytes for dma
//    #define MRV_DMA_SP_ENABLE       0x00010000  //[16] rw set the enable for dma
//    #define MRV_RGB_INPUT_TYPE      0x00004000  //[14] rw set the input format of rgb (1=ycbcr444,0=ycbcr422)
//    #define MRV_CFG_UPD_MI_OUT      0x00002000  //[13] w immediate configuration update for MI output part
//    #define MRV_CFG_UPD_MI_IN       0x00001000  //[12] w immediate configuration update for MI input part
//    #define MRV_SEMI_PLANAR_EN      0x00000800  //[11] rw set to semi planar mode for output interface
//    #define MRV_RGB_TYPE_MASK       0x00000600  //[10:9] rw set the input format of rgb (3=rgb888,2=rgb666,1=rgb565)
//    #define MRV_RGB888_TYPE         0x00000600  //[10:9] rw set the input format of rgb (3=rgb888)
//    #define MRV_RGB666_TYPE         0x00000400  //[10:9] rw set the input format of rgb (2=rgb666)
//    #define MRV_RGB565_TYPE         0x00000200  //[10:9] rw set the input format of rgb (1=rgb565)
//    #define MRV_RGB_ENABLE          0x00000100  //[8] rw set the enable for rgb
//#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
//    #define MRV_SEMI_PLANAR_EN      0x00000400  //[10] rw set to semi planar mode for output interface
//    #define MRV_RGB_TYPE_MASK       0x00000200  //[9] rw set the input format of rgb (1=rgb888,0=rgb565)
//    #define MRV_RGB888_TYPE         0x00000200  //[9] rw set the input format of rgb (1=rgb888,0=rgb565)
//    #define MRV_RGB565_TYPE         0x00000000  //[9] rw set the input format of rgb (1=rgb888,0=rgb565)
//    #define MRV_RGB_ENABLE          0x00000100  //[8] rw set the enable for rgb
//    #define MRV_CFG_UPD_MI_OUT      0x00000040  //[6] w immediate configuration update for MI output part
//    #define MRV_CFG_UPD_MI_IN       0x00000020  //[5] w immediate configuration update for MI input part
//#endif  // MARVIN_FEATURE_MI
//#define MRV_BYTE_SWAP_EN            0x00000080  //[7] rw swap all data bytes at the AHB output interface
//#define MRV_SELF_H_MIRR_EN          0x00000010  //[4] rw enable horizontal mirror for self-picture
//#define MRV_RAW_DATA_EN             0x00000008  //[3] rw enable raw data input
//#define MRV_JPEG_EN                 0x00000004  //[2] rw enable JPEG data input
//#define MRV_CIF_SP_ENABLE           0x00000002  //[1] rw enable self-picture data input
//#define MRV_CIF_MP_ENABLE           0x00000001  //[0] rw enable main picture data input
#if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    // Slice: sp_output_format:
    // Selects output format of self picture. For possible restrictions see sub-chapter "Picture Orientation" in chapter "Self Path Output Programming".
    //
    // 111: reserved
    // 110: RGB 888
    // 101: RGB 666
    // 100: RGB 565
    //
    // 011: YCbCr  4:4:4
    // 010: YCbCr  4:2:2
    // 001: YCbCr  4:2:0
    // 000: YCbCr  4:0:0
    //
    // Note: Programmed value becomes effective immediately. So write to the register only if no picture data is sent to the self path.
    #define MRV_MI_SP_OUTPUT_FORMAT
    #define MRV_MI_SP_OUTPUT_FORMAT_MASK 0x70000000
    #define MRV_MI_SP_OUTPUT_FORMAT_SHIFT 28
    #define MRV_MI_SP_OUTPUT_FORMAT_RGB888 6 // 110: RGB 888
    #define MRV_MI_SP_OUTPUT_FORMAT_RGB666 5 // 101: RGB 666
    #define MRV_MI_SP_OUTPUT_FORMAT_RGB565 4 // 100: RGB 565
    #define MRV_MI_SP_OUTPUT_FORMAT_YUV444 3 // 011: YCbCr  4:4:4
    #define MRV_MI_SP_OUTPUT_FORMAT_YUV422 2 // 010: YCbCr  4:2:2
    #define MRV_MI_SP_OUTPUT_FORMAT_YUV420 1 // 001: YCbCr  4:2:0
    #define MRV_MI_SP_OUTPUT_FORMAT_YUV400 0 // 000: YCbCr  4:0:0
    // Slice: sp_input_format:
    // Selects input format of self picture. For possible restrictions see sub-chapter "Picture Orientation" in chapter "Self Path Output Programming".
    //
    // 11: YCbCr  4:4:4
    // 10: YCbCr  4:2:2
    // 01: YCbCr  4:2:0
    // 00: YCbCr  4:0:0
    //
    // Note: Programmed value becomes effective immediately. So write to the register only if no picture data is sent to the self path.
    #define MRV_MI_SP_INPUT_FORMAT
    #define MRV_MI_SP_INPUT_FORMAT_MASK 0x0C000000
    #define MRV_MI_SP_INPUT_FORMAT_SHIFT 26
    #define MRV_MI_SP_INPUT_FORMAT_YUV444 3 // 11: YCbCr  4:4:4
    #define MRV_MI_SP_INPUT_FORMAT_YUV422 2 // 10: YCbCr  4:2:2
    #define MRV_MI_SP_INPUT_FORMAT_YUV420 1 // 01: YCbCr  4:2:0
    #define MRV_MI_SP_INPUT_FORMAT_YUV400 0 // 00: YCbCr  4:0:0
    // Slice: sp_write_format:
    // Defines how YCbCr self picture data is written to memory. Ignored if RGB conversion is chosen.
    //
    // 00:  planar
    // 01:  semi planar, for YCbCr 4:2:x
    // 10:  interleaved (combined), for YCbCr 4:2:2 only
    // 11:  reserved
    //
    // Note: Programmed value becomes effective immediately. So write to the register only if no picture data is sent to the self path.
    #define MRV_MI_SP_WRITE_FORMAT
    #define MRV_MI_SP_WRITE_FORMAT_MASK 0x03000000
    #define MRV_MI_SP_WRITE_FORMAT_SHIFT 24
    #define MRV_MI_SP_WRITE_FORMAT_PLANAR      0 // 00: planar
    #define MRV_MI_SP_WRITE_FORMAT_SEMIPLANAR  1 // 01: semi planar, for YCbCr 4:2:x
    #define MRV_MI_SP_WRITE_FORMAT_INTERLEAVED 2 // 10: interleaved (combined), for YCbCr 4:2:2 only
    // Slice: mp_write_format:
    // Defines how YCbCr main picture data is written to memory. Ignored if JPEG data is chosen.
    // In YCbCr mode the following meaning is applicable
    // 00:  planar
    // 01:  semi planar, for YCbCr 4:2:x
    // 10:  interleaved (combined), for YCbCr 4:2:2 only
    // 11:  reserved
    // In RAW data mode the following meaning is applicable
    // 0x:  RAW 8 bit
    // 1x:  RAW 12 bit
    // Note: Programmed value becomes effective immediately. So write to the register only if no picture data is sent to the main path.
    #define MRV_MI_MP_WRITE_FORMAT
    #define MRV_MI_MP_WRITE_FORMAT_MASK 0x00C00000
    #define MRV_MI_MP_WRITE_FORMAT_SHIFT 22
    #define MRV_MI_MP_WRITE_FORMAT_PLANAR      0 // 00: planar
    #define MRV_MI_MP_WRITE_FORMAT_SEMIPLANAR  1 // 01: semi planar, for YCbCr 4:2:x
    #define MRV_MI_MP_WRITE_FORMAT_INTERLEAVED 2 // 10: interleaved (combined), for YCbCr 4:2:2 only
    #define MRV_MI_MP_WRITE_FORMAT_RAW_8       0 // 0x: RAW 8 bit
    #define MRV_MI_MP_WRITE_FORMAT_RAW_12      2 // 1x: RAW 12 bit
    // Slice: init_offset_en:
    // Enables updating of the offset counters shadow registers for main and self picture to the programmed register init values.
    // MI_MP/SP_Y/CB/CR_OFFS_CNT_INIT
    // -> MI_MP/SP_Y/CB/CR_OFFS_CNT_SHD
    // The update will be executed either when a forced software update occurs (in register MI_INIT bit cfg_upd = 1) or when an automatic config update signal arrives at the MI input port. The latter is split into main and self picture. So only the corresponding main/self shadow registers are affected.
    // After a picture skip has been performed init_offset_en selects between skip restart and skip init mode (see bit skip in register MI_INIT).
    #define MRV_MI_INIT_OFFSET_EN
    #define MRV_MI_INIT_OFFSET_EN_MASK 0x00200000
    #define MRV_MI_INIT_OFFSET_EN_SHIFT 21
    // Slice: init_base_en:
    // Enables updating of the base address and buffer size shadow registers for main and self picture to the programmed register init values.
    // MI_MP/SP_Y/CB/CR_BASE_AD_INIT
    // -> MI_MP/SP_Y/CB/CR_BASE_AD_SHD
    // MI_MP/SP_Y/CB/CR_SIZE_INIT
    // -> MI_MP/SP_Y/CB/CR_SIZE_SHD
    // The update will be executed either when a forced software update occurs (in register MI_INIT bit cfg_upd = 1) or when an automatic config update signal arrives at the MI input port. The latter is split into main and self picture. So only the corresponding main/self shadow registers are affected.
    #define MRV_MI_INIT_BASE_EN
    #define MRV_MI_INIT_BASE_EN_MASK 0x00100000
    #define MRV_MI_INIT_BASE_EN_SHIFT 20
#if (MARVIN_FEATURE_MI_BURST_16 == MARVIN_FEATURE_EXIST)
    // Slice: burst_len_chrom:
    // Burst length for Cb or Cr data affecting write port.
    // 0: 4-beat bursts
    // 1: 8-beat bursts
    // 2: 16-beat bursts
    // Note: Programmed value becomes effective immediately. So write to the register only if no picture data is sent to the main and self path.
    #define MRV_MI_BURST_LEN_CHROM
    #define MRV_MI_BURST_LEN_CHROM_MASK 0x000C0000
    #define MRV_MI_BURST_LEN_CHROM_SHIFT 18
    #define MRV_MI_BURST_LEN_CHROM_4      0 // 0: 4-beat bursts
    #define MRV_MI_BURST_LEN_CHROM_8      1 // 1: 8-beat bursts
    #define MRV_MI_BURST_LEN_CHROM_16     2 // 2: 16-beat bursts
    // Slice: burst_len_lum:
    // Burst length for Y, JPEG, or raw data affecting write port.
    // 0: 4-beat bursts
    // 1: 8-beat bursts
    // 2: 16-beat bursts
    // Note: Programmed value becomes effective immediately. So write to the register only if no picture data is sent to the main and self path.
    #define MRV_MI_BURST_LEN_LUM
    #define MRV_MI_BURST_LEN_LUM_MASK 0x00030000
    #define MRV_MI_BURST_LEN_LUM_SHIFT 16
    #define MRV_MI_BURST_LEN_LUM_4      0 // 0: 4-beat bursts
    #define MRV_MI_BURST_LEN_LUM_8      1 // 1: 8-beat bursts
    #define MRV_MI_BURST_LEN_LUM_16     2 // 2: 16-beat bursts
#else //#if (MARVIN_FEATURE_MI_BURST_16 == MARVIN_FEATURE_EXIST)
    // Slice: burst_len_chrom:
    // Burst length for Cb or Cr data affecting write port.
    // 0: 4-beat bursts
    // 1: 8-beat bursts
    // Ignored if 8-beat bursts not supported.
    // Note: Programmed value becomes effective immediately. So write to the register only if no picture data is sent to the main and self path.
    #define MRV_MI_BURST_LEN_CHROM
    #define MRV_MI_BURST_LEN_CHROM_MASK 0x00040000
    #define MRV_MI_BURST_LEN_CHROM_SHIFT 18
    #define MRV_MI_BURST_LEN_CHROM_4      0 // 0: 4-beat bursts
    #define MRV_MI_BURST_LEN_CHROM_8      1 // 1: 8-beat bursts
    // Slice: burst_len_lum:
    // Burst length for Y, JPEG, or raw data affecting write port.
    // 0: 4-beat bursts
    // 1: 8-beat bursts
    // Ignored if 8-beat bursts not supported.
    // Note: Programmed value becomes effective immediately. So write to the register only if no picture data is sent to the main and self path.
    #define MRV_MI_BURST_LEN_LUM
    #define MRV_MI_BURST_LEN_LUM_MASK 0x00010000
    #define MRV_MI_BURST_LEN_LUM_SHIFT 16
    #define MRV_MI_BURST_LEN_LUM_4      0 // 0: 4-beat bursts
    #define MRV_MI_BURST_LEN_LUM_8      1 // 1: 8-beat bursts
#endif //#if (MARVIN_FEATURE_MI_BURST_16 == MARVIN_FEATURE_EXIST)
#if (MARVIN_FEATURE_MI_LAST_PIXEL == MARVIN_FEATURE_EXIST)
    // Slice: last_pixel_sig_en:
    // Enables the last pixel signalization
    // 1: byte order is mirrored but the bit order within one byte doesn’t change
    // 0: no byte mirroring
    // Note: Programmed value becomes effective immediately. So write to the register only if no picture data is sent to the main and self path.
    #define MRV_MI_LAST_PIXEL_SIG_EN
    #define MRV_MI_LAST_PIXEL_SIG_EN_MASK 0x00008000
    #define MRV_MI_LAST_PIXEL_SIG_EN_SHIFT 15
#endif //#if (MARVIN_FEATURE_MI_LAST_PIXEL == MARVIN_FEATURE_EXIST)
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V4)
#if (MARVIN_FEATURE_DPCC == MARVIN_FEATURE_DPCC_V1)
    // Slice: dpcc_enable:
    // Enables DPCC bad pixel data
    // Programmed value becomes effective (visible in control shadow register) after a soft reset or a forced software.
    #define MRV_MI_DPCC_ENABLE
    #define MRV_MI_DPCC_ENABLE_MASK 0x00000800
    #define MRV_MI_DPCC_ENABLE_SHIFT 11
#endif    
    // Slice: 422noncosited:
    // Enables self path YCbCr422non-co-sited -> YCbCr444 interpolation
    // (M5_v6, M5_v7 only)
    // 1: YCbCr422 data are non_co-sited (Cb and Cr samples are centered between Y samples) so modified interpolation is activated
    // 0: YCbCr422 data are co-sited (Y0 Cb0 and Cr0 are sampled at the same position)
    // Note: Programmed value becomes effective immediately. So write to the register only if no picture data is sent to the self path.
    #define MRV_MI_422NONCOSITED
    #define MRV_MI_422NONCOSITED_MASK 0x00000400
    #define MRV_MI_422NONCOSITED_SHIFT 10
    // Slice: cbcr_full_range:
    // Enables CbCr full range for self path YCbCr -> RGB conversion
    // (M5_v6, M5_v7 only)
    // 1: CbCr have full range (0..255)
    // 0: CbCr have compressed range range (16..240)
    // Note: Programmed value becomes effective immediately. So write to the register only if no picture data is sent to the self path.
    #define MRV_MI_CBCR_FULL_RANGE
    #define MRV_MI_CBCR_FULL_RANGE_MASK 0x00000200
    #define MRV_MI_CBCR_FULL_RANGE_SHIFT 9
    // Slice: y_full_range:
    // Enables Y full range for self path YCbCr -> RGB conversion
    // (M5_v6, M5_v7 only)
    // 1: Y has full range (0..255)
    // 0: Y has compressed range (16..235)
    // Note: Programmed value becomes effective immediately. So write to the register only if no picture data is sent to the self path.
    #define MRV_MI_Y_FULL_RANGE
    #define MRV_MI_Y_FULL_RANGE_MASK 0x00000100
    #define MRV_MI_Y_FULL_RANGE_SHIFT 8
#endif // (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V4)
#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    //#define MRV_DMA_PATH_SEL        0x00040000  //[18] rw Informs the MI which path(s) are used to output
    //                                            //     the data coming from the DMA-read port.
    //                                            //     0=self path only, 1=Main and/or self path through Image effects module
    #define MRV_MI_DMA_PATH
    #define MRV_MI_DMA_PATH_MASK 0x00040000
    #define MRV_MI_DMA_PATH_SHIFT 18
    #define MRV_MI_DMA_PATH_SP_ONLY 0
    #define MRV_MI_DMA_PATH_MP_SP   1
    //#define MRV_DMA_BYTE_SWAP       0x00020000  //[17] rw swap all data bytes for dma
    #define MRV_MI_DMA_BYTE_SWAP
    #define MRV_MI_DMA_BYTE_SWAP_MASK 0x00020000
    #define MRV_MI_DMA_BYTE_SWAP_SHIFT 17
    //#define MRV_DMA_SP_ENABLE       0x00010000  //[16] rw set the enable for dma
    #define MRV_MI_DMA_SP_EN
    #define MRV_MI_DMA_SP_EN_MASK 0x00010000
    #define MRV_MI_DMA_SP_EN_SHIFT 16
    //#define MRV_RGB_INPUT_TYPE      0x00004000  //[14] rw set the input format of rgb (1=ycbcr444,0=ycbcr422)
    #define MRV_MI_RGB_INPUT_FORMAT
    #define MRV_MI_RGB_INPUT_FORMAT_MASK 0x00004000
    #define MRV_MI_RGB_INPUT_FORMAT_SHIFT 14
    #define MRV_MI_RGB_INPUT_FORMAT_YUV444 1
    #define MRV_MI_RGB_INPUT_FORMAT_YUV422 0
    //#define MRV_CFG_UPD_MI_OUT      0x00002000  //[13] w immediate configuration update for MI output part
    #define MRV_MI_CFG_UPD_OUT
    #define MRV_MI_CFG_UPD_OUT_MASK 0x00002000
    #define MRV_MI_CFG_UPD_OUT_SHIFT 13
    //#define MRV_CFG_UPD_MI_IN       0x00001000  //[12] w immediate configuration update for MI input part
    #define MRV_MI_CFG_UPD_IN
    #define MRV_MI_CFG_UPD_IN_MASK 0x00001000
    #define MRV_MI_CFG_UPD_IN_SHIFT 12
    //#define MRV_SEMI_PLANAR_EN      0x00000800  //[11] rw set to semi planar mode for output interface
    #define MRV_MI_SEMI_PLANAR_EN
    #define MRV_MI_SEMI_PLANAR_EN_MASK 0x00000800
    #define MRV_MI_SEMI_PLANAR_EN_SHIFT 11
    //#define MRV_RGB_TYPE_MASK       0x00000600  //[10:9] rw set the input format of rgb (3=rgb888,2=rgb666,1=rgb565)
    //#define MRV_RGB888_TYPE         0x00000600  //[10:9] rw set the input format of rgb (3=rgb888)
    //#define MRV_RGB666_TYPE         0x00000400  //[10:9] rw set the input format of rgb (2=rgb666)
    //#define MRV_RGB565_TYPE         0x00000200  //[10:9] rw set the input format of rgb (1=rgb565)
    #define MRV_MI_RGB_FORMAT
    #define MRV_MI_RGB_FORMAT_MASK 0x00000600
    #define MRV_MI_RGB_FORMAT_SHIFT 9
    #define MRV_MI_RGB_FORMAT_888   3
    #define MRV_MI_RGB_FORMAT_666   2
    #define MRV_MI_RGB_FORMAT_565   1
    //#define MRV_RGB_ENABLE          0x00000100  //[8] rw set the enable for rgb
    #define MRV_MI_RGB_EN
    #define MRV_MI_RGB_EN_MASK 0x00000100
    #define MRV_MI_RGB_EN_SHIFT 8
#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    //#define MRV_SEMI_PLANAR_EN      0x00000400  //[10] rw set to semi planar mode for output interface
    #define MRV_MI_SEMI_PLANAR_EN
    #define MRV_MI_SEMI_PLANAR_EN_MASK 0x00000400
    #define MRV_MI_SEMI_PLANAR_EN_SHIFT 10
    //#define MRV_RGB_TYPE_MASK       0x00000200  //[9] rw set the input format of rgb (1=rgb888,0=rgb565)
    //#define MRV_RGB888_TYPE         0x00000200  //[9] rw set the input format of rgb (1=rgb888,0=rgb565)
    //#define MRV_RGB565_TYPE         0x00000000  //[9] rw set the input format of rgb (1=rgb888,0=rgb565)
    #define MRV_MI_RGB_FORMAT
    #define MRV_MI_RGB_FORMAT_MASK 0x00000200
    #define MRV_MI_RGB_FORMAT_SHIFT 9
    #define MRV_MI_RGB_FORMAT_888   1
    #define MRV_MI_RGB_FORMAT_565   0
    //#define MRV_RGB_ENABLE          0x00000100  //[8] rw set the enable for rgb
    #define MRV_MI_RGB_EN
    #define MRV_MI_RGB_EN_MASK 0x00000100
    #define MRV_MI_RGB_EN_SHIFT 8
#endif  // MARVIN_FEATURE_MI
// Slice: byte_swap:
// Enables change of byte order of the 32 bit output word at write port
// 1: byte order is mirrored but the bit order within one byte doesn’t change
// 0: no byte mirroring
// Note: Programmed value becomes effective immediately. So write to the register only if no picture data is sent to the main and self path.
#define MRV_MI_BYTE_SWAP
#define MRV_MI_BYTE_SWAP_MASK 0x00000080
#define MRV_MI_BYTE_SWAP_SHIFT 7
  #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    // Slice: rot:
    // Rotation 90 degree counter clockwise of self picture, only in RGB mode. For picture orientation and operation modes see sub-chapter "Picture Orientation" in chapter "Self Path Output Programming".
    // 1: enabled
    // 0: disabled
    // Note: Programmed value becomes effective immediately. So write to the register only if no picture data is sent to the self path.
    #define MRV_MI_ROT
    #define MRV_MI_ROT_MASK 0x00000040
    #define MRV_MI_ROT_SHIFT 6
    // Slice: v_flip:
    // Vertical flipping of self picture. For picture orientation and operation modes see sub-chapter "Picture Orientation" in chapter "Self Path Output Programming".
    // 1: enabled
    // 0: disabled
    // Note: Programmed value becomes effective immediately. So write to the register only if no picture data is sent to the self path.
    #define MRV_MI_V_FLIP
    #define MRV_MI_V_FLIP_MASK 0x00000020
    #define MRV_MI_V_FLIP_SHIFT 5
#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    // unused
#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    //#define MRV_CFG_UPD_MI_OUT      0x00000040  //[6] w immediate configuration update for MI output part
    #define MRV_MI_CFG_UPD_OUT
    #define MRV_MI_CFG_UPD_OUT_MASK 0x00000040
    #define MRV_MI_CFG_UPD_OUT_SHIFT 6
    //#define MRV_CFG_UPD_MI_IN       0x00000020  //[5] w immediate configuration update for MI input part
    #define MRV_MI_CFG_UPD_IN
    #define MRV_MI_CFG_UPD_IN_MASK 0x00000020
    #define MRV_MI_CFG_UPD_IN_SHIFT 5
#endif  // MARVIN_FEATURE_MI
// Slice: h_flip:
// Horizontal flipping of self picture. For picture orientation and operation modes see sub-chapter "Picture Orientation" in chapter "Self Path Output Programming".
// 1: enabled
// 0: disabled
// Note: Programmed value becomes effective immediately. So write to the register only if no picture data is sent to the self path.
#define MRV_MI_H_FLIP
#define MRV_MI_H_FLIP_MASK 0x00000010
#define MRV_MI_H_FLIP_SHIFT 4
// Slice: raw_enable:
// Enables raw data
// Programmed value becomes effective (visible in control shadow register) after a soft reset or a forced software update.
// Affects MI_IN and MI_OUT module.
#define MRV_MI_RAW_ENABLE
#define MRV_MI_RAW_ENABLE_MASK 0x00000008
#define MRV_MI_RAW_ENABLE_SHIFT 3
// Slice: jpeg_enable:
// Enables JPEG data
// Programmed value becomes effective (visible in control shadow register) after a soft reset or a forced software.
// Affects MI_IN and MI_OUT module.
#define MRV_MI_JPEG_ENABLE
#define MRV_MI_JPEG_ENABLE_MASK 0x00000004
#define MRV_MI_JPEG_ENABLE_SHIFT 2
// Slice: sp_enable:
// Enables self picture data path.
// Programmed value becomes effective (visible in control shadow register) after a soft reset, a forced software update or an automatic config update.
// Affects MI_IN and MI_OUT module.
#define MRV_MI_SP_ENABLE
#define MRV_MI_SP_ENABLE_MASK 0x00000002
#define MRV_MI_SP_ENABLE_SHIFT 1
// Slice: mp_enable:
// Enables main picture data path
// Programmed value becomes effective (visible in control shadow register) after a soft reset, a forced software update or an automatic config update.
// Affects MI_IN and MI_OUT module.
#define MRV_MI_MP_ENABLE
#define MRV_MI_MP_ENABLE_MASK 0x00000001
#define MRV_MI_MP_ENABLE_SHIFT 0
// Slice: path_enable:
// Enables picture data path
// Programmed value becomes effective (visible in control shadow register) after a soft reset, a forced software update or an automatic config update.
// Affects MI_IN and MI_OUT module.
#define MRV_MI_PATH_ENABLE
#define MRV_MI_PATH_ENABLE_MASK 0x0000000F
#define MRV_MI_PATH_ENABLE_SHIFT 0

// combination of rotation and flip bits
#define MRV_MI_ROT_AND_FLIP
  #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    #define MRV_MI_ROT_AND_FLIP_MASK   (MRV_MI_H_FLIP_MASK | MRV_MI_V_FLIP_MASK | MRV_MI_ROT_MASK)
    #define MRV_MI_ROT_AND_FLIP_SHIFT  (MRV_MI_H_FLIP_SHIFT)
    #define MRV_MI_ROT_AND_FLIP_H_FLIP (MRV_MI_H_FLIP_MASK >> MRV_MI_ROT_AND_FLIP_SHIFT)
    #define MRV_MI_ROT_AND_FLIP_V_FLIP (MRV_MI_V_FLIP_MASK >> MRV_MI_ROT_AND_FLIP_SHIFT)
    #define MRV_MI_ROT_AND_FLIP_ROTATE (MRV_MI_ROT_MASK    >> MRV_MI_ROT_AND_FLIP_SHIFT)
#else  // MARVIN_FEATURE_MI
    #define MRV_MI_ROT_AND_FLIP_MASK   (MRV_MI_H_FLIP_MASK)
    #define MRV_MI_ROT_AND_FLIP_SHIFT  (MRV_MI_H_FLIP_SHIFT)
    #define MRV_MI_ROT_AND_FLIP_H_FLIP (MRV_MI_H_FLIP_MASK >> MRV_MI_ROT_AND_FLIP_SHIFT)
#endif // MARVIN_FEATURE_MI

// Register: mi_init: Control register for address init and skip function (0x0004)
//#define MRV_SOFT_UPD                0x00000010  //[4] w forced software update leads to an immediate update of the shadow registers affecting the data processing
//#define MRV_SKIP                    0x00000004  //[2] w reset pointers to start of last frame
//#define MRV_INIT_OFFSET             0x00000002  //[1] w defines offset register updating for next configuration update
//#define MRV_INIT_BASE               0x00000001  //[0] w defines base register updating for next configuration update
  #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    // Slice: mi_cfg_upd:
    // Forced configuration update. Leads to an immediate update of the shadow registers.
    // Depending on the two init enable bits in the MI_CTRL register (init_offset_en and init_base_en) the offset counter, base address and buffer size shadow registers are also updated.
    #define MRV_MI_MI_CFG_UPD
    #define MRV_MI_MI_CFG_UPD_MASK 0x00000010
    #define MRV_MI_MI_CFG_UPD_SHIFT 4
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)
// Slice: mi_skip:
// Skip of current or next starting main picture:
// Aborts writing of main picture image data of the current frame to RAM (after the current burst transmission has been completed). Further main picture data up to the end of the current frame are discarded.
//
// No further makroblock line interrupt (mblk_line), no wrap around interrupt for main picture (wrap_mp_y/cb/cr) and no fill level interrupt (fill_mp_y) are generated.
//
// Skip does not affect the generation of the main path frame end interrupt (mp_frame_end).
// Skip does not affect the processing of self picture and its corresponding interrupts namely the self path frame end interrupt (sp_frame_end).
//
// The byte counter (register MI_BYTE_CNT) is not affected. It produces the correct number of JPEG or RAW data bytes at the end of the current (skipped) frame.
//
// After a skip has been performed the offset counter for the main picture at the start of the following frame are set depending on the bit init_offset_en in register MI_CTRL:
//
// - Skip restart mode (init_offset_en = 0)
// The offset counters of the main picture are restarted at the old start values of the previous skipped frame.
//
// - Skip init mode (init_offset_en = 1)
// The offset counters of the main picture are initialized with the register contents of the offset counter init registers without any additional forced software update or automatic config update.
#define MRV_MI_MI_SKIP
#define MRV_MI_MI_SKIP_MASK 0x00000004
#define MRV_MI_MI_SKIP_SHIFT 2
#if  ((MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1) || \
      (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2))
    #define MRV_MI_INIT_OFFSET
    #define MRV_MI_INIT_OFFSET_MASK 0x00000002
    #define MRV_MI_INIT_OFFSET_SHIFT 1
    #define MRV_MI_INIT_BASE
    #define MRV_MI_INIT_BASE_MASK 0x00000001
    #define MRV_MI_INIT_BASE_SHIFT 0
#endif // MARVIN_FEATURE_MI

                                                //MI_aa_b_SIZE_INIT 32-bit
//#define MRV_MIF_BUFFER_MAX_SIZE     0x00FFFFFC  //[23:2] maximum size of 'aa' picture 'b' component ring buffer

                                                //MI_aa_b_OFFS_CNT_INIT 32-bit
//#define MRV_MIF_BUFFER_MAX_OFFS     0x00FFFFFC  //[23:2] maximum offset counter init value of 'aa' picture 'b' component ring buffer

                                                //MI_MP_b_IRQ_OFFS_INIT 32-bit
//#define MRV_MIF_BUFFER_MAX_IRQOFFS  0x00FFFFFC  //[23:2] maximum offset counter init value of main picture 'b' component ring buffer before the fill_mp_y interrupt will be generated

                                                //MI_SP_b_LLENGTH 32-bit
//#define MRV_MIF_BUFFER_MAX_LLENGTH  0x00000FFE  //[11:1] maximum line length of self picture 'b' component also known as stride

// Register: mi_mp_y_base_ad_init: Base address for main picture Y component, JPEG or raw data (0x0008)
// Slice: mp_y_base_ad_init:
// Base address of main picture Y component ring buffer, JPEG ring buffer or raw data ring buffer.
// Programmed value becomes effective (visible in corresponding shadow register) after a soft reset, a forced software update or an automatic config update.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_MP_Y_BASE_AD_INIT
#define MRV_MI_MP_Y_BASE_AD_INIT_MASK 0xFFFFFFF8
#define MRV_MI_MP_Y_BASE_AD_INIT_SHIFT 0
#define MRV_MI_MP_Y_BASE_AD_INIT_VALID_MASK (MRV_MI_MP_Y_BASE_AD_INIT_MASK & ~0x00000007)

// Register: mi_mp_y_size_init: Size of main picture Y component, JPEG or raw data (0x000c)
// Slice: mp_y_size_init:
// Size of main picture Y component ring buffer, JPEG ring buffer or raw data ring buffer.
// Programmed value becomes effective (visible in corresponding shadow register) after a soft reset, a forced software update or an automatic config update.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_MP_Y_SIZE_INIT
#if(MARVIN_FEATURE_MI_FRAMESIZE == MARVIN_FEATURE_12M6)
  #define MRV_MI_MP_Y_SIZE_INIT_MASK 0x01FFFFF8
#else
  #define MRV_MI_MP_Y_SIZE_INIT_MASK 0x00FFFFF8
#endif
#define MRV_MI_MP_Y_SIZE_INIT_SHIFT 0
#define MRV_MI_MP_Y_SIZE_INIT_VALID_MASK (MRV_MI_MP_Y_SIZE_INIT_MASK & ~0x00000007)

// Register: mi_mp_y_offs_cnt_init: Offset counter init value for main picture Y, JPEG or raw data (0x0010)
// Slice: mp_y_offs_cnt_init:
// Offset counter init value of main picture Y component ring buffer, JPEG ring buffer or raw data ring buffer.
// Programmed value becomes effective (visible in corresponding shadow register) after a soft reset, a forced software update or an automatic config update.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_MP_Y_OFFS_CNT_INIT
#define MRV_MI_MP_Y_OFFS_CNT_INIT_MASK 0x01FFFFF8
#define MRV_MI_MP_Y_OFFS_CNT_INIT_SHIFT 0
#define MRV_MI_MP_Y_OFFS_CNT_INIT_VALID_MASK (MRV_MI_MP_Y_OFFS_CNT_INIT_MASK & ~0x00000007)

// Register: mi_mp_y_offs_cnt_start: Offset counter start value for main picture Y, JPEG or raw data (0x0014)
// Slice: mp_y_offs_cnt_start:
// Offset counter value which points to the start address of the previously processed picture (main picture Y component, JPEG or raw data). Updated at frame end.
// Note: A soft reset resets the contents to the reset value.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_MP_Y_OFFS_CNT_START
#define MRV_MI_MP_Y_OFFS_CNT_START_MASK 0x01FFFFF8
#define MRV_MI_MP_Y_OFFS_CNT_START_SHIFT 0
#define MRV_MI_MP_Y_OFFS_CNT_START_VALID_MASK (MRV_MI_MP_Y_OFFS_CNT_START_MASK & ~0x00000007)

// Register: mi_mp_y_irq_offs_init: Fill level interrupt offset value for main picture Y, JPEG or raw data (0x0018)
// Slice: mp_y_irq_offs_init:
// Reaching this programmed value by the current offset counter for addressing main picture Y component, JPEG or raw data leads to generation of fill level interrupt fill_mp_y.
// Programmed value becomes effective (visible in corresponding shadow register) after a soft reset, a forced software update or an automatic config update.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_MP_Y_IRQ_OFFS_INIT
#define MRV_MI_MP_Y_IRQ_OFFS_INIT_MASK 0x01FFFFF8
#define MRV_MI_MP_Y_IRQ_OFFS_INIT_SHIFT 0
#define MRV_MI_MP_Y_IRQ_OFFS_INIT_VALID_MASK (MRV_MI_MP_Y_IRQ_OFFS_INIT_MASK & ~0x00000007)

// Register: mi_mp_cb_base_ad_init: Base address for main picture Cb component ring buffer (0x001c)
// Slice: mp_cb_base_ad_init:
// Base address of main picture Cb component ring buffer.
// Programmed value becomes effective (visible in corresponding shadow register) after a soft reset, a forced software update or an automatic config update.
// Note: Set control bit init_base_en before updating so that a forced or automatic update can take effect.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_MP_CB_BASE_AD_INIT
#define MRV_MI_MP_CB_BASE_AD_INIT_MASK 0xFFFFFFF8
#define MRV_MI_MP_CB_BASE_AD_INIT_SHIFT 0
#define MRV_MI_MP_CB_BASE_AD_INIT_VALID_MASK (MRV_MI_MP_CB_BASE_AD_INIT_MASK & ~0x00000007)

// Register: mi_mp_cb_size_init: Size of main picture Cb component ring buffer (0x0020)
// Slice: mp_cb_size_init:
// Size of main picture Cb component ring buffer.
// Programmed value becomes effective (visible in corresponding shadow register) after a soft reset, a forced software update or an automatic config update.
// Note: Set control bit init_base_en before updating so that a forced or automatic update can take effect.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_MP_CB_SIZE_INIT
#define MRV_MI_MP_CB_SIZE_INIT_MASK 0x00FFFFF8
#define MRV_MI_MP_CB_SIZE_INIT_SHIFT 0
#define MRV_MI_MP_CB_SIZE_INIT_VALID_MASK (MRV_MI_MP_CB_SIZE_INIT_MASK & ~0x00000007)

// Register: mi_mp_cb_offs_cnt_init: Offset counter init value for main picture Cb component ring buffer (0x0024)
// Slice: mp_cb_offs_cnt_init:
// Offset counter init value of main picture Cb component ring buffer.
// Programmed value becomes effective (visible in corresponding shadow register) after a soft reset, a forced software update or an automatic config update.
// Note: Set control bit init_base_en before updating so that a forced or automatic update can take effect. Check exceptional handling in skip modes.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_MP_CB_OFFS_CNT_INIT
#define MRV_MI_MP_CB_OFFS_CNT_INIT_MASK 0x00FFFFF8
#define MRV_MI_MP_CB_OFFS_CNT_INIT_SHIFT 0
#define MRV_MI_MP_CB_OFFS_CNT_INIT_VALID_MASK (MRV_MI_MP_CB_OFFS_CNT_INIT_MASK & ~0x00000007)

// Register: mi_mp_cb_offs_cnt_start: Offset counter start value for main picture Cb component ring buffer (0x0028)
// Slice: mp_cb_offs_cnt_start:
// Offset counter value which points to the start address of the previously processed picture (main picture Cb component). Updated at frame end.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_MP_CB_OFFS_CNT_START
#define MRV_MI_MP_CB_OFFS_CNT_START_MASK 0x00FFFFF8
#define MRV_MI_MP_CB_OFFS_CNT_START_SHIFT 0
#define MRV_MI_MP_CB_OFFS_CNT_START_VALID_MASK (MRV_MI_MP_CB_OFFS_CNT_START_MASK & ~0x00000007)

// Register: mi_mp_cr_base_ad_init: Base address for main picture Cr component ring buffer (0x002c)
// Slice: mp_cr_base_ad_init:
// Base address of main picture Cr component ring buffer.
// Programmed value becomes effective (visible in corresponding shadow register) after a soft reset, a forced software update or an automatic config update.
// Note: Set control bit init_base_en before updating so that a forced or automatic update can take effect.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_MP_CR_BASE_AD_INIT
#define MRV_MI_MP_CR_BASE_AD_INIT_MASK 0xFFFFFFF8
#define MRV_MI_MP_CR_BASE_AD_INIT_SHIFT 0
#define MRV_MI_MP_CR_BASE_AD_INIT_VALID_MASK (MRV_MI_MP_CR_BASE_AD_INIT_MASK & ~0x00000007)

// Register: mi_mp_cr_size_init: Size of main picture Cr component ring buffer (0x0030)
// Slice: mp_cr_size_init:
// Size of main picture Cr component ring buffer.
// Programmed value becomes effective (visible in corresponding shadow register) after a soft reset, a forced software update or an automatic config update.
// Note: Set control bit init_base_en before updating so that a forced or automatic update can take effect.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_MP_CR_SIZE_INIT
#define MRV_MI_MP_CR_SIZE_INIT_MASK 0x00FFFFF8
#define MRV_MI_MP_CR_SIZE_INIT_SHIFT 0
#define MRV_MI_MP_CR_SIZE_INIT_VALID_MASK (MRV_MI_MP_CR_SIZE_INIT_MASK & ~0x00000007)

// Register: mi_mp_cr_offs_cnt_init: Offset counter init value for main picture Cr component ring buffer (0x0034)
// Slice: mp_cr_offs_cnt_init:
// Offset counter init value of main picture Cr component ring buffer.
// Programmed value becomes effective (visible in corresponding shadow register) after a soft reset, a forced software update or an automatic config update.
// Note: Set control bit init_base_en before updating so that a forced or automatic update can take effect. Check exceptional handling in skip modes.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_MP_CR_OFFS_CNT_INIT
#define MRV_MI_MP_CR_OFFS_CNT_INIT_MASK 0x00FFFFF8
#define MRV_MI_MP_CR_OFFS_CNT_INIT_SHIFT 0
#define MRV_MI_MP_CR_OFFS_CNT_INIT_VALID_MASK (MRV_MI_MP_CR_OFFS_CNT_INIT_MASK & ~0x00000007)

// Register: mi_mp_cr_offs_cnt_start: Offset counter start value for main picture Cr component ring buffer (0x0038)
// Slice: mp_cr_offs_cnt_start:
// Offset counter value which points to the start address of the previously processed picture (main picture Cr component). Updated at frame end.
// Note: Soft reset will reset the contents to reset value.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_MP_CR_OFFS_CNT_START
#define MRV_MI_MP_CR_OFFS_CNT_START_MASK 0x00FFFFF8
#define MRV_MI_MP_CR_OFFS_CNT_START_SHIFT 0
#define MRV_MI_MP_CR_OFFS_CNT_START_VALID_MASK (MRV_MI_MP_CR_OFFS_CNT_START_MASK & ~0x00000007)

// Register: mi_sp_y_base_ad_init: Base address for self picture Y component ring buffer (0x003c)
// Slice: sp_y_base_ad_init:
// Base address of self picture Y component ring buffer.
// Programmed value becomes effective (visible in corresponding shadow register) after a soft reset, a forced software update or an automatic config update.
// Note: Set control bit init_base_en before updating so that a forced or automatic update can take effect.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_SP_Y_BASE_AD_INIT
#define MRV_MI_SP_Y_BASE_AD_INIT_MASK 0xFFFFFFF8
#define MRV_MI_SP_Y_BASE_AD_INIT_SHIFT 0
#define MRV_MI_SP_Y_BASE_AD_INIT_VALID_MASK (MRV_MI_SP_Y_BASE_AD_INIT_MASK & ~0x00000007)

// Register: mi_sp_y_size_init: Size of self picture Y component ring buffer (0x0040)
// Slice: sp_y_size_init:
// Size of self picture Y component ring buffer.
// Programmed value becomes effective (visible in corresponding shadow register) after a soft reset, a forced software update or an automatic config update.
// Note: Set control bit init_base_en before updating so that a forced or automatic update can take effect.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_SP_Y_SIZE_INIT
#define MRV_MI_SP_Y_SIZE_INIT_MASK 0x01FFFFF8
#define MRV_MI_SP_Y_SIZE_INIT_SHIFT 0
#define MRV_MI_SP_Y_SIZE_INIT_VALID_MASK (MRV_MI_SP_Y_SIZE_INIT_MASK & ~0x00000007)

// Register: mi_sp_y_offs_cnt_init: Offset counter init value for self picture Y component ring buffer (0x0044)
// Slice: sp_y_offs_cnt_init:
// Offset counter init value of self picture Y component ring buffer.
// Programmed value becomes effective (visible in corresponding shadow register) after a soft reset, a forced software update or an automatic config update.
// Note: Set control bit init_base_en before updating so that a forced or automatic update can take effect.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_SP_Y_OFFS_CNT_INIT
#define MRV_MI_SP_Y_OFFS_CNT_INIT_MASK 0x01FFFFF8
#define MRV_MI_SP_Y_OFFS_CNT_INIT_SHIFT 0
#define MRV_MI_SP_Y_OFFS_CNT_INIT_VALID_MASK (MRV_MI_SP_Y_OFFS_CNT_INIT_MASK & ~0x00000007)

// Register: mi_sp_y_offs_cnt_start: Offset counter start value for self picture Y component ring buffer (0x0048)
// Slice: sp_y_offs_cnt_start:
// Offset counter value which points to the start address of the previously processed picture (self picture Y component). Updated at frame end.
// Note: Soft reset will reset the contents to reset value.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_SP_Y_OFFS_CNT_START
#define MRV_MI_SP_Y_OFFS_CNT_START_MASK 0x01FFFFF8
#define MRV_MI_SP_Y_OFFS_CNT_START_SHIFT 0
#define MRV_MI_SP_Y_OFFS_CNT_START_VALID_MASK (MRV_MI_SP_Y_OFFS_CNT_START_MASK & ~0x00000007)

// Register: mi_sp_y_llength: Line length of self picture Y component (0x004c)
// Slice: sp_y_llength:
// Line length of self picture Y component or RGB picture in pixel, also known as line stride.
// If no line stride is used, line length must match image width.
// For Y component the line length in 4:2:x planar mode must be a multiple of 8, for all other component modes a multiple of 4 and for RGB 565 a multiple of 2. There are no restrictions for RGB 888/666.
// In planar mode the line length of the Cb and Cr component is assumed according to the YCbCr format, i.e. half for 4:2:x and the same size for 4:4:4. In semi planar 4:2:x mode the line length of the Cb and Cr component is assumed the same size.
// Note: Line length always refers to the line length of the output image. This is particularly important when rotating.
// Note: Programmed value becomes effective immediately. So write to the register only if no picture data is sent to the self path.
#define MRV_MI_SP_Y_LLENGTH
#define MRV_MI_SP_Y_LLENGTH_MASK 0x00001FFF
#define MRV_MI_SP_Y_LLENGTH_SHIFT 0
#define MRV_MI_SP_Y_LLENGTH_VALID_MASK (MRV_MI_SP_Y_LLENGTH_MASK & ~0x00000000)

// Register: mi_sp_cb_base_ad_init: Base address for self picture Cb component ring buffer (0x0050)
// Slice: sp_cb_base_ad_init:
// Base address of self picture Cb component ring buffer.
// Programmed value becomes effective (visible in corresponding shadow register) after a soft reset, a forced software update or an automatic config update.
// Note: Set control bit init_base_en before updating so that a forced or automatic update can take effect.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_SP_CB_BASE_AD_INIT
#define MRV_MI_SP_CB_BASE_AD_INIT_MASK 0xFFFFFFF8
#define MRV_MI_SP_CB_BASE_AD_INIT_SHIFT 0
#define MRV_MI_SP_CB_BASE_AD_INIT_VALID_MASK (MRV_MI_SP_CB_BASE_AD_INIT_MASK & ~0x00000007)

// Register: mi_sp_cb_size_init: Size of self picture Cb component ring buffer (0x0054)
// Slice: sp_cb_size_init:
// Size of self picture Cb component ring buffer.
// Programmed value becomes effective (visible in corresponding shadow register) after a soft reset, a forced software update or an automatic config update.
// Note: Set control bit init_base_en before updating so that a forced or automatic update can take effect.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_SP_CB_SIZE_INIT
#define MRV_MI_SP_CB_SIZE_INIT_MASK 0x00FFFFF8
#define MRV_MI_SP_CB_SIZE_INIT_SHIFT 0
#define MRV_MI_SP_CB_SIZE_INIT_VALID_MASK (MRV_MI_SP_CB_SIZE_INIT_MASK & ~0x00000007)

// Register: mi_sp_cb_offs_cnt_init: Offset counter init value for self picture Cb component ring buffer (0x0058)
// Slice: sp_cb_offs_cnt_init:
// Offset counter init value of self picture Cb component ring buffer.
// Programmed value becomes effective (visible in corresponding shadow register) after a soft reset, a forced software update or an automatic config update.
// Note: Set control bit init_base_en before updating so that a forced or automatic update can take effect.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_SP_CB_OFFS_CNT_INIT
#define MRV_MI_SP_CB_OFFS_CNT_INIT_MASK 0x00FFFFF8
#define MRV_MI_SP_CB_OFFS_CNT_INIT_SHIFT 0
#define MRV_MI_SP_CB_OFFS_CNT_INIT_VALID_MASK (MRV_MI_SP_CB_OFFS_CNT_INIT_MASK & ~0x00000007)

// Register: mi_sp_cb_offs_cnt_start: Offset counter start value for self picture Cb component ring buffer (0x005c)
// Slice: sp_cb_offs_cnt_start:
// Offset counter value which points to the start address of the previously processed picture (self picture Cb component). Updated at frame end.
// Note: Soft reset will reset the contents to reset value.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_SP_CB_OFFS_CNT_START
#define MRV_MI_SP_CB_OFFS_CNT_START_MASK 0x00FFFFF8
#define MRV_MI_SP_CB_OFFS_CNT_START_SHIFT 0
#define MRV_MI_SP_CB_OFFS_CNT_START_VALID_MASK (MRV_MI_SP_CB_OFFS_CNT_START_MASK & ~0x00000007)

// Register: mi_sp_cr_base_ad_init: Base address for self picture Cr component ring buffer (0x0060)
// Slice: sp_cr_base_ad_init:
// Base address of self picture Cr component ring buffer.
// Programmed value becomes effective (visible in corresponding shadow register) after a soft reset, a forced software update or an automatic config update.
// Note: Set control bit init_base_en before updating so that a forced or automatic update can take effect.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_SP_CR_BASE_AD_INIT
#define MRV_MI_SP_CR_BASE_AD_INIT_MASK 0xFFFFFFF8
#define MRV_MI_SP_CR_BASE_AD_INIT_SHIFT 0
#define MRV_MI_SP_CR_BASE_AD_INIT_VALID_MASK (MRV_MI_SP_CR_BASE_AD_INIT_MASK & ~0x00000007)

// Register: mi_sp_cr_size_init: Size of self picture Cr component ring buffer (0x0064)
// Slice: sp_cr_size_init:
// Size of self picture Cr component ring buffer.
// Programmed value becomes effective (visible in corresponding shadow register) after a soft reset, a forced software update or an automatic config update.
// Note: Set control bit init_base_en before updating so that a forced or automatic update can take effect.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_SP_CR_SIZE_INIT
#define MRV_MI_SP_CR_SIZE_INIT_MASK 0x00FFFFF8
#define MRV_MI_SP_CR_SIZE_INIT_SHIFT 0
#define MRV_MI_SP_CR_SIZE_INIT_VALID_MASK (MRV_MI_SP_CR_SIZE_INIT_MASK & ~0x00000007)

// Register: mi_sp_cr_offs_cnt_init: Offset counter init value for self picture Cr component ring buffer (0x0068)
// Slice: sp_cr_offs_cnt_init:
// Offset counter init value of self picture Cr component ring buffer.
// Programmed value becomes effective (visible in corresponding shadow register) after a soft reset, a forced software update or an automatic config update.
// Note: Set control bit init_base_en before updating so that a forced or automatic update can take effect.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_SP_CR_OFFS_CNT_INIT
#define MRV_MI_SP_CR_OFFS_CNT_INIT_MASK 0x00FFFFF8
#define MRV_MI_SP_CR_OFFS_CNT_INIT_SHIFT 0
#define MRV_MI_SP_CR_OFFS_CNT_INIT_VALID_MASK (MRV_MI_SP_CR_OFFS_CNT_INIT_MASK & ~0x00000007)

// Register: mi_sp_cr_offs_cnt_start: Offset counter start value for self picture Cr component ring buffer (0x006c)
// Slice: sp_cr_offs_cnt_start:
// Offset counter value which points to the start address of the previously processed picture (self picture Cr component). Updated at frame end.
// Must be 64-bit aligned (bit 0, bit 1 and bit 2 are hard wired to "000")
#define MRV_MI_SP_CR_OFFS_CNT_START
#define MRV_MI_SP_CR_OFFS_CNT_START_MASK 0x00FFFFF8
#define MRV_MI_SP_CR_OFFS_CNT_START_SHIFT 0
#define MRV_MI_SP_CR_OFFS_CNT_START_VALID_MASK (MRV_MI_SP_CR_OFFS_CNT_START_MASK & ~0x00000007)

// Register: mi_byte_cnt: Counter value of JPEG or RAW data bytes (0x0070)
// Slice: byte_cnt:
// Counter value specifies the number of JPEG or RAW data bytes of the last transmitted frame. Updated at frame end.
// A soft reset will set the byte counter to zero.
#define MRV_MI_BYTE_CNT
#define MRV_MI_BYTE_CNT_MASK 0x01FFFFFF
#define MRV_MI_BYTE_CNT_SHIFT 0

// Register: mi_ctrl_shd: global control internal shadow register (0x0074)
// Slice: raw_enable_out:
// raw data used in module MI_OUT
#define MRV_MI_RAW_ENABLE_OUT
#define MRV_MI_RAW_ENABLE_OUT_MASK 0x00080000
#define MRV_MI_RAW_ENABLE_OUT_SHIFT 19
// Slice: jpeg_enable_out:
// JPEG data used in module MI_OUT
#define MRV_MI_JPEG_ENABLE_OUT
#define MRV_MI_JPEG_ENABLE_OUT_MASK 0x00040000
#define MRV_MI_JPEG_ENABLE_OUT_SHIFT 18
// Slice: sp_enable_out:
// self picture used in module MI_OUT
#define MRV_MI_SP_ENABLE_OUT
#define MRV_MI_SP_ENABLE_OUT_MASK 0x00020000
#define MRV_MI_SP_ENABLE_OUT_SHIFT 17
// Slice: mp_enable_out:
// main picture used in module MI_OUT
#define MRV_MI_MP_ENABLE_OUT
#define MRV_MI_MP_ENABLE_OUT_MASK 0x00010000
#define MRV_MI_MP_ENABLE_OUT_SHIFT 16
// Slice: raw_enable_in:
// raw data used in module MI_IN
#define MRV_MI_RAW_ENABLE_IN
#define MRV_MI_RAW_ENABLE_IN_MASK 0x00000020
#define MRV_MI_RAW_ENABLE_IN_SHIFT 5
// Slice: jpeg_enable_in:
// JPEG data used in module MI_IN
#define MRV_MI_JPEG_ENABLE_IN
#define MRV_MI_JPEG_ENABLE_IN_MASK 0x00000010
#define MRV_MI_JPEG_ENABLE_IN_SHIFT 4
// Slice: sp_enable_in:
// self picture data used in module MI_IN
#define MRV_MI_SP_ENABLE_IN
#define MRV_MI_SP_ENABLE_IN_MASK 0x00000004
#define MRV_MI_SP_ENABLE_IN_SHIFT 2
// Slice: mp_enable_in:
// main picture data used in module MI_IN
#define MRV_MI_MP_ENABLE_IN
#define MRV_MI_MP_ENABLE_IN_MASK 0x00000001
#define MRV_MI_MP_ENABLE_IN_SHIFT 0

// Register: mi_mp_y_base_ad_shd: Base address shadow register for main picture Y component, JPEG or raw data ring buffer (0x0078)
// Slice: mp_y_base_ad:
// Base address of main picture Y component ring buffer, JPEG ring buffer or raw data ring buffer.
// Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
#define MRV_MI_MP_Y_BASE_AD
#define MRV_MI_MP_Y_BASE_AD_MASK 0xFFFFFFFC
#define MRV_MI_MP_Y_BASE_AD_SHIFT 0
#define MRV_MI_MP_Y_BASE_AD_VALID_MASK (MRV_MI_MP_Y_BASE_AD_MASK & ~0x00000003)

// Register: mi_mp_y_size_shd: Size shadow register of main picture Y component, JPEG or raw data (0x007c)
// Slice: mp_y_size:
// Size of main picture Y component ring buffer, JPEG ring buffer or raw data ring buffer.
// Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
#define MRV_MI_MP_Y_SIZE
#define MRV_MI_MP_Y_SIZE_MASK 0x01FFFFFC
#define MRV_MI_MP_Y_SIZE_SHIFT 0
#define MRV_MI_MP_Y_SIZE_VALID_MASK (MRV_MI_MP_Y_SIZE_MASK & ~0x00000003)

// Register: mi_mp_y_offs_cnt_shd: Current offset counter of main picture Y component, JPEG or raw data ring buffer (0x0080)
// Slice: mp_y_offs_cnt:
// Current offset counter of main picture Y component, JPEG or raw data ring buffer for address generation
// Note: Soft reset will reset the contents to reset value.
// Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
#define MRV_MI_MP_Y_OFFS_CNT
#define MRV_MI_MP_Y_OFFS_CNT_MASK 0x01FFFFFC
#define MRV_MI_MP_Y_OFFS_CNT_SHIFT 0
#define MRV_MI_MP_Y_OFFS_CNT_VALID_MASK (MRV_MI_MP_Y_OFFS_CNT_MASK & ~0x00000003)

// Register: mi_mp_y_irq_offs_shd: Shadow register of fill level interrupt offset value for main picture Y component, JPEG or raw data (0x0084)
// Slice: mp_y_irq_offs:
// Reaching this offset value by the current offset counter for addressing main picture Y component, JPEG or raw data leads to generation of fill level interrupt fill_mp_y.
// Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
#define MRV_MI_MP_Y_IRQ_OFFS
#define MRV_MI_MP_Y_IRQ_OFFS_MASK 0x01FFFFFC
#define MRV_MI_MP_Y_IRQ_OFFS_SHIFT 0
#define MRV_MI_MP_Y_IRQ_OFFS_VALID_MASK (MRV_MI_MP_Y_IRQ_OFFS_MASK & ~0x00000003)

// Register: mi_mp_cb_base_ad_shd: Base address shadow register for main picture Cb component ring buffer (0x0088)
// Slice: mp_cb_base_ad:
// Base address of main picture Cb component ring buffer.
// Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
#define MRV_MI_MP_CB_BASE_AD
#define MRV_MI_MP_CB_BASE_AD_MASK 0xFFFFFFFF
#define MRV_MI_MP_CB_BASE_AD_SHIFT 0
#define MRV_MI_MP_CB_BASE_AD_VALID_MASK (MRV_MI_MP_CB_BASE_AD_MASK & ~0x00000003)

// Register: mi_mp_cb_size_shd: Size shadow register of main picture Cb component ring buffer (0x008c)
// Slice: mp_cb_size:
// Size of main picture Cb component ring buffer.
// Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
#define MRV_MI_MP_CB_SIZE
#define MRV_MI_MP_CB_SIZE_MASK 0x00FFFFFF
#define MRV_MI_MP_CB_SIZE_SHIFT 0
#define MRV_MI_MP_CB_SIZE_VALID_MASK (MRV_MI_MP_CB_SIZE_MASK & ~0x00000003)

// Register: mi_mp_cb_offs_cnt_shd: Current offset counter of main picture Cb component ring buffer (0x0090)
// Slice: mp_cb_offs_cnt:
// Current offset counter of main picture Cb component ring buffer for address generation
// Note: Soft reset will reset the contents to reset value.
// Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
#define MRV_MI_MP_CB_OFFS_CNT
#define MRV_MI_MP_CB_OFFS_CNT_MASK 0x00FFFFFF
#define MRV_MI_MP_CB_OFFS_CNT_SHIFT 0
#define MRV_MI_MP_CB_OFFS_CNT_VALID_MASK (MRV_MI_MP_CB_OFFS_CNT_MASK & ~0x00000003)

// Register: mi_mp_cr_base_ad_shd: Base address shadow register for main picture Cr component ring buffer (0x0094)
// Slice: mp_cr_base_ad:
// Base address of main picture Cr component ring buffer.
// Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
#define MRV_MI_MP_CR_BASE_AD
#define MRV_MI_MP_CR_BASE_AD_MASK 0xFFFFFFFF
#define MRV_MI_MP_CR_BASE_AD_SHIFT 0
#define MRV_MI_MP_CR_BASE_AD_VALID_MASK (MRV_MI_MP_CR_BASE_AD_MASK & ~0x00000003)

// Register: mi_mp_cr_size_shd: Size shadow register of main picture Cr component ring buffer (0x0098)
// Slice: mp_cr_size:
// Size of main picture Cr component ring buffer.
// Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
#define MRV_MI_MP_CR_SIZE
#define MRV_MI_MP_CR_SIZE_MASK 0x00FFFFFF
#define MRV_MI_MP_CR_SIZE_SHIFT 0
#define MRV_MI_MP_CR_SIZE_VALID_MASK (MRV_MI_MP_CR_SIZE_MASK & ~0x00000003)

// Register: mi_mp_cr_offs_cnt_shd: Current offset counter of main picture Cr component ring buffer (0x009c)
// Slice: mp_cr_offs_cnt:
// Current offset counter of main picture Cr component ring buffer for address generation
// Note: Soft reset will reset the contents to reset value.
// Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
#define MRV_MI_MP_CR_OFFS_CNT
#define MRV_MI_MP_CR_OFFS_CNT_MASK 0x00FFFFFF
#define MRV_MI_MP_CR_OFFS_CNT_SHIFT 0
#define MRV_MI_MP_CR_OFFS_CNT_VALID_MASK (MRV_MI_MP_CR_OFFS_CNT_MASK & ~0x00000003)

// Register: mi_sp_y_base_ad_shd: Base address shadow register for self picture Y component ring buffer (0x00a0)
// Slice: sp_y_base_ad:
// Base address of self picture Y component ring buffer.
// Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
#define MRV_MI_SP_Y_BASE_AD
#define MRV_MI_SP_Y_BASE_AD_MASK 0xFFFFFFFF
#define MRV_MI_SP_Y_BASE_AD_SHIFT 0
#define MRV_MI_SP_Y_BASE_AD_VALID_MASK (MRV_MI_SP_Y_BASE_AD_MASK & ~0x00000003)

// Register: mi_sp_y_size_shd: Size shadow register of self picture Y component ring buffer (0x00a4)
// Slice: sp_y_size:
// Size of self picture Y component ring buffer.
// Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
#define MRV_MI_SP_Y_SIZE
#define MRV_MI_SP_Y_SIZE_MASK 0x01FFFFFC
#define MRV_MI_SP_Y_SIZE_SHIFT 0
#define MRV_MI_SP_Y_SIZE_VALID_MASK (MRV_MI_SP_Y_SIZE_MASK & ~0x00000003)

// Register: mi_sp_y_offs_cnt_shd: Current offset counter of self picture Y component ring buffer (0x00a8)
// Slice: sp_y_offs_cnt:
// Current offset counter of self picture Y component ring buffer for address generation
// Note: Soft reset will reset the contents to reset value.
// Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
#define MRV_MI_SP_Y_OFFS_CNT
#define MRV_MI_SP_Y_OFFS_CNT_MASK 0x01FFFFFC
#define MRV_MI_SP_Y_OFFS_CNT_SHIFT 0
#define MRV_MI_SP_Y_OFFS_CNT_VALID_MASK (MRV_MI_SP_Y_OFFS_CNT_MASK & ~0x00000003)

// Register: mi_sp_cb_base_ad_shd: Base address shadow register for self picture Cb component ring buffer (0x00b0)
// Slice: sp_cb_base_ad:
// Base address of self picture Cb component ring buffer.
// Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
#define MRV_MI_SP_CB_BASE_AD
#define MRV_MI_SP_CB_BASE_AD_MASK 0xFFFFFFFF
#define MRV_MI_SP_CB_BASE_AD_SHIFT 0
#define MRV_MI_SP_CB_BASE_AD_VALID_MASK (MRV_MI_SP_CB_BASE_AD_MASK & ~0x00000003)

// Register: mi_sp_cb_size_shd: Size shadow register of self picture Cb component ring buffer (0x00b4)
// Slice: sp_cb_size:
// Size of self picture Cb component ring buffer.
// Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
#define MRV_MI_SP_CB_SIZE
#define MRV_MI_SP_CB_SIZE_MASK 0x00FFFFFF
#define MRV_MI_SP_CB_SIZE_SHIFT 0
#define MRV_MI_SP_CB_SIZE_VALID_MASK (MRV_MI_SP_CB_SIZE_MASK & ~0x00000003)

// Register: mi_sp_cb_offs_cnt_shd: Current offset counter of self picture Cb component ring buffer (0x00b8)
// Slice: sp_cb_offs_cnt:
// Current offset counter of self picture Cb component ring buffer for address generation
// Note: Soft reset will reset the contents to reset value.
// Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
#define MRV_MI_SP_CB_OFFS_CNT
#define MRV_MI_SP_CB_OFFS_CNT_MASK 0x00FFFFFF
#define MRV_MI_SP_CB_OFFS_CNT_SHIFT 0
#define MRV_MI_SP_CB_OFFS_CNT_VALID_MASK (MRV_MI_SP_CB_OFFS_CNT_MASK & ~0x00000003)

// Register: mi_sp_cr_base_ad_shd: Base address shadow register for self picture Cr component ring buffer (0x00bc)
// Slice: sp_cr_base_ad:
// Base address of self picture Cr component ring buffer.
// Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
#define MRV_MI_SP_CR_BASE_AD
#define MRV_MI_SP_CR_BASE_AD_MASK 0xFFFFFFFF
#define MRV_MI_SP_CR_BASE_AD_SHIFT 0
#define MRV_MI_SP_CR_BASE_AD_VALID_MASK (MRV_MI_SP_CR_BASE_AD_MASK & ~0x00000003)

// Register: mi_sp_cr_size_shd: Size shadow register of self picture Cr component ring buffer (0x00c0)
// Slice: sp_cr_size:
// Size of self picture Cr component ring buffer.
// Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
#define MRV_MI_SP_CR_SIZE
#define MRV_MI_SP_CR_SIZE_MASK 0x00FFFFFF
#define MRV_MI_SP_CR_SIZE_SHIFT 0
#define MRV_MI_SP_CR_SIZE_VALID_MASK (MRV_MI_SP_CR_SIZE_MASK & ~0x00000003)

// Register: mi_sp_cr_offs_cnt_shd: Current offset counter of self picture Cr component ring buffer (0x00c4)
// Slice: sp_cr_offs_cnt:
// Current offset counter of self picture Cr component ring buffer for address generation
// Note: Soft reset will reset the contents to reset value.
// Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
#define MRV_MI_SP_CR_OFFS_CNT
#define MRV_MI_SP_CR_OFFS_CNT_MASK 0x00FFFFFF
#define MRV_MI_SP_CR_OFFS_CNT_SHIFT 0
#define MRV_MI_SP_CR_OFFS_CNT_VALID_MASK (MRV_MI_SP_CR_OFFS_CNT_MASK & ~0x00000003)

#if ((MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V2) && \
     (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4))
    // Register: mi_dma_y_pic_start_ad: Y component image start address (0x00c8)
    // Slice: dma_y_pic_start_ad:
    // Image start address of the y component
    // Note: Must be multiple of 4 in interleaved mode.
    #define MRV_MI_DMA_Y_PIC_START_AD
    #define MRV_MI_DMA_Y_PIC_START_AD_MASK 0xFFFFFFFF
    #define MRV_MI_DMA_Y_PIC_START_AD_SHIFT 0

    // Register: mi_dma_y_pic_width: Y component image width (0x00cc)
    // Slice: dma_y_pic_width:
    // Image width of the Y component in pixel.
    // For YCbCr 4:2:x the image width must be a multiple of 2.
    // In planar mode the image width of the Cb and Cr component is assumed according to the YCbCr format, i.e. half for 4:2:x and the same size for 4:4:4. In semi planar 4:2:x mode the image width of the Cb component (which includes Cr) is assumed the same size. In interleave mode no Cb/Cr image width is used.
    #define MRV_MI_DMA_Y_PIC_WIDTH
    #define MRV_MI_DMA_Y_PIC_WIDTH_MASK 0x00001FFF
    #define MRV_MI_DMA_Y_PIC_WIDTH_SHIFT 0

    // Register: mi_dma_y_llength: Y component original line length (0x00d0)
    // Slice: dma_y_llength:
    // Line length of the Y component of the original image in memory
    // For an uncropped image, where lines follow each other without offset (no line stride), line length must match image width.
    // For Y component the line length in 4:2:x planar mode must be a multiple of 8, for all other component modes a multiple of 4.
    // In planar mode the line length of the Cb and Cr component is assumed according to the YCbCr format, i.e. half for 4:2:x and the same size for 4:4:4. In semi planar 4:2:x mode the line length of the Cb component (which includes Cr) is assumed the same size. In interleave mode no Cb/Cr line length is used.
    #define MRV_MI_DMA_Y_LLENGTH
    #define MRV_MI_DMA_Y_LLENGTH_MASK 0x00001FFF
    #define MRV_MI_DMA_Y_LLENGTH_SHIFT 0

    // Register: mi_dma_y_pic_size: Y component image size (0x00d4)
    // Slice: dma_y_pic_size:
    // Image size of the Y component in pixel which has to be the Y line length multiplied by the Y image height (dma_y_llength * dma_y_pic_height).
    // In planar mode the image size of the Cb and Cr component is assumed according to the YCbCr format, i.e. a quarter for 4:2:0, half for 4:2:2 and the same for 4:4:4. In semi planar mode the image size of the Cb component (which includes Cr) is assumed half for 4:2:0 and the same size for 4:2:2. In interleave mode no Cb/Cr image size is used.
    #define MRV_MI_DMA_Y_PIC_SIZE
    #define MRV_MI_DMA_Y_PIC_SIZE_MASK 0x00FFFFFF
    #define MRV_MI_DMA_Y_PIC_SIZE_SHIFT 0

    // Register: mi_dma_cb_pic_start_ad: Cb component image start address (0x00d8)
    // Slice: dma_cb_pic_start_ad:
    // Image start address of the Cb component
    // Note: Must be multiple of 2 in semi-planar mode.
    #define MRV_MI_DMA_CB_PIC_START_AD
    #define MRV_MI_DMA_CB_PIC_START_AD_MASK 0xFFFFFFFF
    #define MRV_MI_DMA_CB_PIC_START_AD_SHIFT 0

  #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    // Register: mi_dma_cb_pic_width:
    #define MRV_MI_DMA_CB_PIC_WIDTH
    #define MRV_MI_DMA_CB_PIC_WIDTH_MASK 0x00000FFF
    #define MRV_MI_DMA_CB_PIC_WIDTH_SHIFT 0

    // Register: mi_dma_cb_llength:
    #define MRV_MI_DMA_CB_LLENGTH
    #define MRV_MI_DMA_CB_LLENGTH_MASK 0x00000FFF
    #define MRV_MI_DMA_CB_LLENGTH_SHIFT 0

    // Register: mi_dma_cb_pic_size:
    #define MRV_MI_DMA_CB_PIC_SIZE
    #define MRV_MI_DMA_CB_PIC_SIZE_MASK 0x00FFFFFF
    #define MRV_MI_DMA_CB_PIC_SIZE_SHIFT 0
  #endif  // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)

    // Register: mi_dma_cr_pic_start_ad: Cr component image start address (0x00e8)
    // Slice: dma_cr_pic_start_ad:
    // Image start address of the Cr component
    #define MRV_MI_DMA_CR_PIC_START_AD
    #define MRV_MI_DMA_CR_PIC_START_AD_MASK 0xFFFFFFFF
    #define MRV_MI_DMA_CR_PIC_START_AD_SHIFT 0

  #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    // Register: mi_dma_cr_pic_width:
    #define MRV_MI_DMA_CR_PIC_WIDTH
    #define MRV_MI_DMA_CR_PIC_WIDTH_MASK 0x00000FFF
    #define MRV_MI_DMA_CR_PIC_WIDTH_SHIFT 0

    // Register: mi_dma_cr_llength:
    #define MRV_MI_DMA_CR_LLENGTH
    #define MRV_MI_DMA_CR_LLENGTH_MASK 0x00000FFF
    #define MRV_MI_DMA_CR_LLENGTH_SHIFT 0

    // Register: mi_dma_cr_pic_size:
    #define MRV_MI_DMA_CR_PIC_SIZE
    #define MRV_MI_DMA_CR_PIC_SIZE_MASK 0x00FFFFFF
    #define MRV_MI_DMA_CR_PIC_SIZE_SHIFT 0
  #endif  // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
#endif  // #if (MARVIN_FEATURE_MI >= MI_V2 && <= MI_V3)

                                                //MI_IMSC 32-bit
                                                //MI_RIS 32-bit
                                                //MI_MIS 32-bit
                                                //MI_ICR 32-bit
                                                //MI_ISR 32-bit
// all of these interrupt related registers use the same following bit configuration
//#if ((MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V2) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V3))
//    #define MRV_MIINT_DMA_READY         0x00000800  //[11] dma read ready masked interrupt
//    #define MRV_MIINT_WRITE_ERR         0x00000400  //[10] AHB write error interrupt
//    #define MRV_MIINT_WRP_S_CR          0x00000200  //[9] self-picture Cr buffer wrap-around interrupt
//    #define MRV_MIINT_WRP_S_CB          0x00000100  //[8] self-picture Cb buffer wrap-around interrupt
//    #define MRV_MIINT_WRP_S_Y           0x00000080  //[7] self-picture Y buffer wrap-around interrupt
//    #define MRV_MIINT_WRP_M_CR          0x00000040  //[6] main picture Cr buffer wrap-around interrupt
//    #define MRV_MIINT_WRP_M_CB          0x00000020  //[5] main picture Cb buffer wrap-around interrupt
//    #define MRV_MIINT_WRP_M_Y           0x00000010  //[4] main picture Y buffer wrap-around interrupt
//    #define MRV_MIINT_FILL_M_Y          0x00000008  //[3] main picture Y or JPEG/raw data buffer fill level interrupt
//    #define MRV_MIINT_MBLK_LINE         0x00000004  //[2] main picture macro block line interrupt
//    #define MRV_MIINT_2ND_FRAME_END     0x00000002  //[1] for MI_V3: self path frame end interrupt
//                                                    //    for MI_V2: DMA frame end interrupt
//#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
//    #define MRV_MIINT_WRITE_ERR         0x00000200  //[9] AHB write error interrupt
//    #define MRV_MIINT_WRP_S_CR          0x00000100  //[8] self-picture Cr buffer wrap-around interrupt
//    #define MRV_MIINT_WRP_S_CB          0x00000080  //[7] self-picture Cb buffer wrap-around interrupt
//    #define MRV_MIINT_WRP_S_Y           0x00000040  //[6] self-picture Y buffer wrap-around interrupt
//    #define MRV_MIINT_WRP_M_CR          0x00000020  //[5] main picture Cr buffer wrap-around interrupt
//    #define MRV_MIINT_WRP_M_CB          0x00000010  //[4] main picture Cb buffer wrap-around interrupt
//    #define MRV_MIINT_WRP_M_Y           0x00000008  //[3] main picture Y buffer wrap-around interrupt
//    #define MRV_MIINT_FILL_M_Y          0x00000004  //[2] main picture Y or JPEG/raw data buffer fill level interrupt
//    #define MRV_MIINT_MBLK_LINE         0x00000002  //[1] main picture macro block line interrupt
//#endif  // MARVIN_FEATURE_MI
//#define MRV_MIINT_1ST_FRAME_END     0x00000001  //[0] main picture frame or JPEG/raw data end interrupt
// Register: mi_imsc: Interrupt Mask (‘1’: interrupt active, ‘0’: interrupt masked) (0x00f8)
#if ((MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V2) && \
     (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4))
    // Slice: dma_ready:
    // Mask bit for dma ready interrupt
    #define MRV_MI_DMA_READY
    #define MRV_MI_DMA_READY_MASK 0x00000800
    #define MRV_MI_DMA_READY_SHIFT 11
#else // #if (MARVIN_FEATURE_MI)
    #define MRV_MI_DMA_READY_MASK 0
#endif // #if (MARVIN_FEATURE_MI)
// Slice: ahb_error:
// Mask bit for AHB write or read error interrupt (from BVCI target interfaces)
#define MRV_MI_AHB_ERROR
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_AHB_ERROR_MASK 0x00000200
    #define MRV_MI_AHB_ERROR_SHIFT 9
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_AHB_ERROR_MASK 0x00000400
    #define MRV_MI_AHB_ERROR_SHIFT 10
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_sp_cr:
// Mask bit for self picture Cr address wrap interrupt
#define MRV_MI_WRAP_SP_CR
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_CR_MASK 0x00000100
    #define MRV_MI_WRAP_SP_CR_SHIFT 8
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_CR_MASK 0x00000200
    #define MRV_MI_WRAP_SP_CR_SHIFT 9
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_sp_cb:
// Mask bit for self picture Cb address wrap interrupt
#define MRV_MI_WRAP_SP_CB
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_CB_MASK 0x00000080
    #define MRV_MI_WRAP_SP_CB_SHIFT 7
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_CB_MASK 0x00000100
    #define MRV_MI_WRAP_SP_CB_SHIFT 8
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_sp_y:
// Mask bit for self picture Y address wrap interrupt
#define MRV_MI_WRAP_SP_Y
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_Y_MASK 0x00000040
    #define MRV_MI_WRAP_SP_Y_SHIFT 6
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_Y_MASK 0x00000080
    #define MRV_MI_WRAP_SP_Y_SHIFT 7
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_mp_cr:
// Mask bit for main picture Cr address wrap interrupt
#define MRV_MI_WRAP_MP_CR
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_CR_MASK 0x00000020
    #define MRV_MI_WRAP_MP_CR_SHIFT 5
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_CR_MASK 0x00000040
    #define MRV_MI_WRAP_MP_CR_SHIFT 6
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_mp_cb:
// Mask bit for main picture Cb address wrap interrupt
#define MRV_MI_WRAP_MP_CB
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_CB_MASK 0x00000010
    #define MRV_MI_WRAP_MP_CB_SHIFT 4
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_CB_MASK 0x00000020
    #define MRV_MI_WRAP_MP_CB_SHIFT 5
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_mp_y:
// Mask bit for main picture Y address wrap interrupt
#define MRV_MI_WRAP_MP_Y
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_Y_MASK 0x00000008
    #define MRV_MI_WRAP_MP_Y_SHIFT 3
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_Y_MASK 0x00000010
    #define MRV_MI_WRAP_MP_Y_SHIFT 4
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: fill_mp_y:
// Mask bit for fill level interrupt of main picture Y, JPEG or raw data
#define MRV_MI_FILL_MP_Y
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_FILL_MP_Y_MASK 0x00000004
    #define MRV_MI_FILL_MP_Y_SHIFT 2
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_FILL_MP_Y_MASK 0x00000008
    #define MRV_MI_FILL_MP_Y_SHIFT 3
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: mblk_line:
// Mask bit for makroblock line interrupt of main picture (16 lines of Y, 8 lines of Cb and 8 lines of Cr are written into RAM)
#define MRV_MI_MBLK_LINE
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_MBLK_LINE_MASK 0x00000002
    #define MRV_MI_MBLK_LINE_SHIFT 1
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_MBLK_LINE_MASK 0x00000004
    #define MRV_MI_MBLK_LINE_SHIFT 2
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
  #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    // Slice: sp_frame_end:
    // Mask self picture end of frame interrupt
    #define MRV_MI_SP_FRAME_END
    #define MRV_MI_SP_FRAME_END_MASK 0x00000002
    #define MRV_MI_SP_FRAME_END_SHIFT 1
#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    // Slice: dma_frame_end:
    // Mask DMA end of frame interrupt
    #define MRV_MI_DMA_FRAME_END
    #define MRV_MI_DMA_FRAME_END_MASK 0x00000002
    #define MRV_MI_DMA_FRAME_END_SHIFT 1
#endif // MARVIN_FEATURE_MI
// Slice: mp_frame_end:
// Mask main picture end of frame interrupt
#define MRV_MI_MP_FRAME_END
#define MRV_MI_MP_FRAME_END_MASK 0x00000001
#define MRV_MI_MP_FRAME_END_SHIFT 0

#ifndef MRV_MI_SP_FRAME_END
    #define MRV_MI_SP_FRAME_END_MASK 0
#endif
#ifndef MRV_MI_DMA_FRAME_END
    #define MRV_MI_DMA_FRAME_END_MASK 0
#endif

// combination of all MI IRQ lines
#define MRV_MI_ALLIRQS
#define MRV_MI_ALLIRQS_MASK \
  (   0 \
    | MRV_MI_DMA_READY_MASK \
    | MRV_MI_AHB_ERROR_MASK \
    | MRV_MI_WRAP_SP_CR_MASK \
    | MRV_MI_WRAP_SP_CB_MASK \
    | MRV_MI_WRAP_SP_Y_MASK \
    | MRV_MI_WRAP_MP_CR_MASK \
    | MRV_MI_WRAP_MP_CB_MASK \
    | MRV_MI_WRAP_MP_Y_MASK \
    | MRV_MI_FILL_MP_Y_MASK \
    | MRV_MI_MBLK_LINE_MASK \
    | MRV_MI_SP_FRAME_END_MASK \
    | MRV_MI_DMA_FRAME_END_MASK \
    | MRV_MI_MP_FRAME_END_MASK \
  )
#define MRV_MI_ALLIRQS_SHIFT 0

// Register: mi_ris: Raw Interrupt Status (0x00fc)
/* same defines as for mi_imsc
  #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    // Slice: dma_ready:
    // Mask bit for dma ready interrupt
    #define MRV_MI_DMA_READY
    #define MRV_MI_DMA_READY_MASK 0x00000800
    #define MRV_MI_DMA_READY_SHIFT 11
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)
// Slice: ahb_error:
// Mask bit for AHB write or read error interrupt (from BVCI target interfaces)
#define MRV_MI_AHB_ERROR
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_AHB_ERROR_MASK 0x00000200
    #define MRV_MI_AHB_ERROR_SHIFT 9
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_AHB_ERROR_MASK 0x00000400
    #define MRV_MI_AHB_ERROR_SHIFT 10
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_sp_cr:
// Mask bit for self picture Cr address wrap interrupt
#define MRV_MI_WRAP_SP_CR
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_CR_MASK 0x00000100
    #define MRV_MI_WRAP_SP_CR_SHIFT 8
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_CR_MASK 0x00000200
    #define MRV_MI_WRAP_SP_CR_SHIFT 9
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_sp_cb:
// Mask bit for self picture Cb address wrap interrupt
#define MRV_MI_WRAP_SP_CB
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_CB_MASK 0x00000080
    #define MRV_MI_WRAP_SP_CB_SHIFT 7
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_CB_MASK 0x00000100
    #define MRV_MI_WRAP_SP_CB_SHIFT 8
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_sp_y:
// Mask bit for self picture Y address wrap interrupt
#define MRV_MI_WRAP_SP_Y
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_Y_MASK 0x00000040
    #define MRV_MI_WRAP_SP_Y_SHIFT 6
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_Y_MASK 0x00000080
    #define MRV_MI_WRAP_SP_Y_SHIFT 7
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_mp_cr:
// Mask bit for main picture Cr address wrap interrupt
#define MRV_MI_WRAP_MP_CR
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_CR_MASK 0x00000020
    #define MRV_MI_WRAP_MP_CR_SHIFT 5
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_CR_MASK 0x00000040
    #define MRV_MI_WRAP_MP_CR_SHIFT 6
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_mp_cb:
// Mask bit for main picture Cb address wrap interrupt
#define MRV_MI_WRAP_MP_CB
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_CB_MASK 0x00000010
    #define MRV_MI_WRAP_MP_CB_SHIFT 4
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_CB_MASK 0x00000020
    #define MRV_MI_WRAP_MP_CB_SHIFT 5
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_mp_y:
// Mask bit for main picture Y address wrap interrupt
#define MRV_MI_WRAP_MP_Y
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_Y_MASK 0x00000008
    #define MRV_MI_WRAP_MP_Y_SHIFT 3
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_Y_MASK 0x00000010
    #define MRV_MI_WRAP_MP_Y_SHIFT 4
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: fill_mp_y:
// Mask bit for fill level interrupt of main picture Y, JPEG or raw data
#define MRV_MI_FILL_MP_Y
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_FILL_MP_Y_MASK 0x00000004
    #define MRV_MI_FILL_MP_Y_SHIFT 2
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_FILL_MP_Y_MASK 0x00000008
    #define MRV_MI_FILL_MP_Y_SHIFT 3
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: mblk_line:
// Mask bit for makroblock line interrupt of main picture (16 lines of Y, 8 lines of Cb and 8 lines of Cr are written into RAM)
#define MRV_MI_MBLK_LINE
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_MBLK_LINE_MASK 0x00000002
    #define MRV_MI_MBLK_LINE_SHIFT 1
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_MBLK_LINE_MASK 0x00000004
    #define MRV_MI_MBLK_LINE_SHIFT 2
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
  #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    // Slice: sp_frame_end:
    // Mask self picture end of frame interrupt
    #define MRV_MI_SP_FRAME_END
    #define MRV_MI_SP_FRAME_END_MASK 0x00000002
    #define MRV_MI_SP_FRAME_END_SHIFT 1
#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    // Slice: dma_frame_end:
    // Mask DMA end of frame interrupt
    #define MRV_MI_DMA_FRAME_END
    #define MRV_MI_DMA_FRAME_END_MASK 0x00000002
    #define MRV_MI_DMA_FRAME_END_SHIFT 1
#endif // MARVIN_FEATURE_MI
// Slice: mp_frame_end:
// Mask main picture end of frame interrupt
#define MRV_MI_MP_FRAME_END
#define MRV_MI_MP_FRAME_END_MASK 0x00000001
#define MRV_MI_MP_FRAME_END_SHIFT 0
*/

// Register: mi_mis: Masked Interrupt Status (0x0100)
/* same defines as for mi_imsc
  #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    // Slice: dma_ready:
    // Mask bit for dma ready interrupt
    #define MRV_MI_DMA_READY
    #define MRV_MI_DMA_READY_MASK 0x00000800
    #define MRV_MI_DMA_READY_SHIFT 11
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)
// Slice: ahb_error:
// Mask bit for AHB write or read error interrupt (from BVCI target interfaces)
#define MRV_MI_AHB_ERROR
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_AHB_ERROR_MASK 0x00000200
    #define MRV_MI_AHB_ERROR_SHIFT 9
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_AHB_ERROR_MASK 0x00000400
    #define MRV_MI_AHB_ERROR_SHIFT 10
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_sp_cr:
// Mask bit for self picture Cr address wrap interrupt
#define MRV_MI_WRAP_SP_CR
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_CR_MASK 0x00000100
    #define MRV_MI_WRAP_SP_CR_SHIFT 8
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_CR_MASK 0x00000200
    #define MRV_MI_WRAP_SP_CR_SHIFT 9
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_sp_cb:
// Mask bit for self picture Cb address wrap interrupt
#define MRV_MI_WRAP_SP_CB
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_CB_MASK 0x00000080
    #define MRV_MI_WRAP_SP_CB_SHIFT 7
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_CB_MASK 0x00000100
    #define MRV_MI_WRAP_SP_CB_SHIFT 8
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_sp_y:
// Mask bit for self picture Y address wrap interrupt
#define MRV_MI_WRAP_SP_Y
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_Y_MASK 0x00000040
    #define MRV_MI_WRAP_SP_Y_SHIFT 6
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_Y_MASK 0x00000080
    #define MRV_MI_WRAP_SP_Y_SHIFT 7
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_mp_cr:
// Mask bit for main picture Cr address wrap interrupt
#define MRV_MI_WRAP_MP_CR
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_CR_MASK 0x00000020
    #define MRV_MI_WRAP_MP_CR_SHIFT 5
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_CR_MASK 0x00000040
    #define MRV_MI_WRAP_MP_CR_SHIFT 6
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_mp_cb:
// Mask bit for main picture Cb address wrap interrupt
#define MRV_MI_WRAP_MP_CB
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_CB_MASK 0x00000010
    #define MRV_MI_WRAP_MP_CB_SHIFT 4
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_CB_MASK 0x00000020
    #define MRV_MI_WRAP_MP_CB_SHIFT 5
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_mp_y:
// Mask bit for main picture Y address wrap interrupt
#define MRV_MI_WRAP_MP_Y
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_Y_MASK 0x00000008
    #define MRV_MI_WRAP_MP_Y_SHIFT 3
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_Y_MASK 0x00000010
    #define MRV_MI_WRAP_MP_Y_SHIFT 4
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: fill_mp_y:
// Mask bit for fill level interrupt of main picture Y, JPEG or raw data
#define MRV_MI_FILL_MP_Y
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_FILL_MP_Y_MASK 0x00000004
    #define MRV_MI_FILL_MP_Y_SHIFT 2
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_FILL_MP_Y_MASK 0x00000008
    #define MRV_MI_FILL_MP_Y_SHIFT 3
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: mblk_line:
// Mask bit for makroblock line interrupt of main picture (16 lines of Y, 8 lines of Cb and 8 lines of Cr are written into RAM)
#define MRV_MI_MBLK_LINE
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_MBLK_LINE_MASK 0x00000002
    #define MRV_MI_MBLK_LINE_SHIFT 1
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_MBLK_LINE_MASK 0x00000004
    #define MRV_MI_MBLK_LINE_SHIFT 2
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
  #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    // Slice: sp_frame_end:
    // Mask self picture end of frame interrupt
    #define MRV_MI_SP_FRAME_END
    #define MRV_MI_SP_FRAME_END_MASK 0x00000002
    #define MRV_MI_SP_FRAME_END_SHIFT 1
#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    // Slice: dma_frame_end:
    // Mask DMA end of frame interrupt
    #define MRV_MI_DMA_FRAME_END
    #define MRV_MI_DMA_FRAME_END_MASK 0x00000002
    #define MRV_MI_DMA_FRAME_END_SHIFT 1
#endif // MARVIN_FEATURE_MI
// Slice: mp_frame_end:
// Mask main picture end of frame interrupt
#define MRV_MI_MP_FRAME_END
#define MRV_MI_MP_FRAME_END_MASK 0x00000001
#define MRV_MI_MP_FRAME_END_SHIFT 0
*/

// Register: mi_icr: Interrupt Clear Register (0x0104)
/* same defines as for mi_imsc
  #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    // Slice: dma_ready:
    // Mask bit for dma ready interrupt
    #define MRV_MI_DMA_READY
    #define MRV_MI_DMA_READY_MASK 0x00000800
    #define MRV_MI_DMA_READY_SHIFT 11
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)
// Slice: ahb_error:
// Mask bit for AHB write or read error interrupt (from BVCI target interfaces)
#define MRV_MI_AHB_ERROR
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_AHB_ERROR_MASK 0x00000200
    #define MRV_MI_AHB_ERROR_SHIFT 9
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_AHB_ERROR_MASK 0x00000400
    #define MRV_MI_AHB_ERROR_SHIFT 10
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_sp_cr:
// Mask bit for self picture Cr address wrap interrupt
#define MRV_MI_WRAP_SP_CR
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_CR_MASK 0x00000100
    #define MRV_MI_WRAP_SP_CR_SHIFT 8
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_CR_MASK 0x00000200
    #define MRV_MI_WRAP_SP_CR_SHIFT 9
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_sp_cb:
// Mask bit for self picture Cb address wrap interrupt
#define MRV_MI_WRAP_SP_CB
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_CB_MASK 0x00000080
    #define MRV_MI_WRAP_SP_CB_SHIFT 7
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_CB_MASK 0x00000100
    #define MRV_MI_WRAP_SP_CB_SHIFT 8
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_sp_y:
// Mask bit for self picture Y address wrap interrupt
#define MRV_MI_WRAP_SP_Y
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_Y_MASK 0x00000040
    #define MRV_MI_WRAP_SP_Y_SHIFT 6
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_Y_MASK 0x00000080
    #define MRV_MI_WRAP_SP_Y_SHIFT 7
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_mp_cr:
// Mask bit for main picture Cr address wrap interrupt
#define MRV_MI_WRAP_MP_CR
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_CR_MASK 0x00000020
    #define MRV_MI_WRAP_MP_CR_SHIFT 5
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_CR_MASK 0x00000040
    #define MRV_MI_WRAP_MP_CR_SHIFT 6
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_mp_cb:
// Mask bit for main picture Cb address wrap interrupt
#define MRV_MI_WRAP_MP_CB
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_CB_MASK 0x00000010
    #define MRV_MI_WRAP_MP_CB_SHIFT 4
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_CB_MASK 0x00000020
    #define MRV_MI_WRAP_MP_CB_SHIFT 5
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_mp_y:
// Mask bit for main picture Y address wrap interrupt
#define MRV_MI_WRAP_MP_Y
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_Y_MASK 0x00000008
    #define MRV_MI_WRAP_MP_Y_SHIFT 3
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_Y_MASK 0x00000010
    #define MRV_MI_WRAP_MP_Y_SHIFT 4
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: fill_mp_y:
// Mask bit for fill level interrupt of main picture Y, JPEG or raw data
#define MRV_MI_FILL_MP_Y
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_FILL_MP_Y_MASK 0x00000004
    #define MRV_MI_FILL_MP_Y_SHIFT 2
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_FILL_MP_Y_MASK 0x00000008
    #define MRV_MI_FILL_MP_Y_SHIFT 3
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: mblk_line:
// Mask bit for makroblock line interrupt of main picture (16 lines of Y, 8 lines of Cb and 8 lines of Cr are written into RAM)
#define MRV_MI_MBLK_LINE
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_MBLK_LINE_MASK 0x00000002
    #define MRV_MI_MBLK_LINE_SHIFT 1
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_MBLK_LINE_MASK 0x00000004
    #define MRV_MI_MBLK_LINE_SHIFT 2
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
  #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    // Slice: sp_frame_end:
    // Mask self picture end of frame interrupt
    #define MRV_MI_SP_FRAME_END
    #define MRV_MI_SP_FRAME_END_MASK 0x00000002
    #define MRV_MI_SP_FRAME_END_SHIFT 1
#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    // Slice: dma_frame_end:
    // Mask DMA end of frame interrupt
    #define MRV_MI_DMA_FRAME_END
    #define MRV_MI_DMA_FRAME_END_MASK 0x00000002
    #define MRV_MI_DMA_FRAME_END_SHIFT 1
#endif // MARVIN_FEATURE_MI
// Slice: mp_frame_end:
// Mask main picture end of frame interrupt
#define MRV_MI_MP_FRAME_END
#define MRV_MI_MP_FRAME_END_MASK 0x00000001
#define MRV_MI_MP_FRAME_END_SHIFT 0
*/

// Register: mi_isr: Interrupt Set Register (0x0108)
/* same defines as for mi_imsc
  #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    // Slice: dma_ready:
    // Mask bit for dma ready interrupt
    #define MRV_MI_DMA_READY
    #define MRV_MI_DMA_READY_MASK 0x00000800
    #define MRV_MI_DMA_READY_SHIFT 11
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)
// Slice: ahb_error:
// Mask bit for AHB write or read error interrupt (from BVCI target interfaces)
#define MRV_MI_AHB_ERROR
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_AHB_ERROR_MASK 0x00000200
    #define MRV_MI_AHB_ERROR_SHIFT 9
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_AHB_ERROR_MASK 0x00000400
    #define MRV_MI_AHB_ERROR_SHIFT 10
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_sp_cr:
// Mask bit for self picture Cr address wrap interrupt
#define MRV_MI_WRAP_SP_CR
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_CR_MASK 0x00000100
    #define MRV_MI_WRAP_SP_CR_SHIFT 8
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_CR_MASK 0x00000200
    #define MRV_MI_WRAP_SP_CR_SHIFT 9
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_sp_cb:
// Mask bit for self picture Cb address wrap interrupt
#define MRV_MI_WRAP_SP_CB
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_CB_MASK 0x00000080
    #define MRV_MI_WRAP_SP_CB_SHIFT 7
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_CB_MASK 0x00000100
    #define MRV_MI_WRAP_SP_CB_SHIFT 8
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_sp_y:
// Mask bit for self picture Y address wrap interrupt
#define MRV_MI_WRAP_SP_Y
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_Y_MASK 0x00000040
    #define MRV_MI_WRAP_SP_Y_SHIFT 6
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_SP_Y_MASK 0x00000080
    #define MRV_MI_WRAP_SP_Y_SHIFT 7
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_mp_cr:
// Mask bit for main picture Cr address wrap interrupt
#define MRV_MI_WRAP_MP_CR
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_CR_MASK 0x00000020
    #define MRV_MI_WRAP_MP_CR_SHIFT 5
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_CR_MASK 0x00000040
    #define MRV_MI_WRAP_MP_CR_SHIFT 6
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_mp_cb:
// Mask bit for main picture Cb address wrap interrupt
#define MRV_MI_WRAP_MP_CB
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_CB_MASK 0x00000010
    #define MRV_MI_WRAP_MP_CB_SHIFT 4
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_CB_MASK 0x00000020
    #define MRV_MI_WRAP_MP_CB_SHIFT 5
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: wrap_mp_y:
// Mask bit for main picture Y address wrap interrupt
#define MRV_MI_WRAP_MP_Y
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_Y_MASK 0x00000008
    #define MRV_MI_WRAP_MP_Y_SHIFT 3
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_WRAP_MP_Y_MASK 0x00000010
    #define MRV_MI_WRAP_MP_Y_SHIFT 4
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: fill_mp_y:
// Mask bit for fill level interrupt of main picture Y, JPEG or raw data
#define MRV_MI_FILL_MP_Y
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_FILL_MP_Y_MASK 0x00000004
    #define MRV_MI_FILL_MP_Y_SHIFT 2
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_FILL_MP_Y_MASK 0x00000008
    #define MRV_MI_FILL_MP_Y_SHIFT 3
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
// Slice: mblk_line:
// Mask bit for makroblock line interrupt of main picture (16 lines of Y, 8 lines of Cb and 8 lines of Cr are written into RAM)
#define MRV_MI_MBLK_LINE
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_MBLK_LINE_MASK 0x00000002
    #define MRV_MI_MBLK_LINE_SHIFT 1
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    #define MRV_MI_MBLK_LINE_MASK 0x00000004
    #define MRV_MI_MBLK_LINE_SHIFT 2
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
  #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    // Slice: sp_frame_end:
    // Mask self picture end of frame interrupt
    #define MRV_MI_SP_FRAME_END
    #define MRV_MI_SP_FRAME_END_MASK 0x00000002
    #define MRV_MI_SP_FRAME_END_SHIFT 1
#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    // Slice: dma_frame_end:
    // Mask DMA end of frame interrupt
    #define MRV_MI_DMA_FRAME_END
    #define MRV_MI_DMA_FRAME_END_MASK 0x00000002
    #define MRV_MI_DMA_FRAME_END_SHIFT 1
#endif // MARVIN_FEATURE_MI
// Slice: mp_frame_end:
// Mask main picture end of frame interrupt
#define MRV_MI_MP_FRAME_END
#define MRV_MI_MP_FRAME_END_MASK 0x00000001
#define MRV_MI_MP_FRAME_END_SHIFT 0
*/

#if (MARVIN_FEATURE_MI_STATUSFLAGS == MARVIN_FEATURE_EXIST)
    // Register: mi_status: MI Status Register (0x010c)
    // Slice: ahb_read_error:
    // An AHB error occurred (ahb_error interrupt raised) while reading from the AHB bus (DMA access) since last clear.
    #define MRV_MI_AHB_READ_ERROR
    #define MRV_MI_AHB_READ_ERROR_MASK 0x00000200
    #define MRV_MI_AHB_READ_ERROR_SHIFT 9
    // Slice: ahb_write_error:
    // An AHB error occurred (ahb_error interrupt raised) while writing to the AHB bus (main/self path) since last clear.
    #define MRV_MI_AHB_WRITE_ERROR
    #define MRV_MI_AHB_WRITE_ERROR_MASK 0x00000100
    #define MRV_MI_AHB_WRITE_ERROR_SHIFT 8
    // Slice: sp_cr_fifo_full:
    // FIFO full flag of Cr FIFO in self path asserted since last clear
    #define MRV_MI_SP_CR_FIFO_FULL
    #define MRV_MI_SP_CR_FIFO_FULL_MASK 0x00000040
    #define MRV_MI_SP_CR_FIFO_FULL_SHIFT 6
    // Slice: sp_cb_fifo_full:
    // FIFO full flag of Cb FIFO in self path asserted since last clear
    #define MRV_MI_SP_CB_FIFO_FULL
    #define MRV_MI_SP_CB_FIFO_FULL_MASK 0x00000020
    #define MRV_MI_SP_CB_FIFO_FULL_SHIFT 5
    // Slice: sp_y_fifo_full:
    // FIFO full flag of Y FIFO in self path asserted since last clear
    #define MRV_MI_SP_Y_FIFO_FULL
    #define MRV_MI_SP_Y_FIFO_FULL_MASK 0x00000010
    #define MRV_MI_SP_Y_FIFO_FULL_SHIFT 4
    // Slice: mp_cr_fifo_full:
    // FIFO full flag of Cr FIFO in main path asserted since last clear
    #define MRV_MI_MP_CR_FIFO_FULL
    #define MRV_MI_MP_CR_FIFO_FULL_MASK 0x00000004
    #define MRV_MI_MP_CR_FIFO_FULL_SHIFT 2
    // Slice: mp_cb_fifo_full:
    // FIFO full flag of Cb FIFO in main path asserted since last clear
    #define MRV_MI_MP_CB_FIFO_FULL
    #define MRV_MI_MP_CB_FIFO_FULL_MASK 0x00000002
    #define MRV_MI_MP_CB_FIFO_FULL_SHIFT 1
    // Slice: mp_y_fifo_full:
    // FIFO full flag of Y FIFO in main path asserted since last clear
    #define MRV_MI_MP_Y_FIFO_FULL
    #define MRV_MI_MP_Y_FIFO_FULL_MASK 0x00000001
    #define MRV_MI_MP_Y_FIFO_FULL_SHIFT 0

    // combination of all MI status lines
    #define MRV_MI_ALL_STAT
    #define MRV_MI_ALL_STAT_MASK \
      (   0 \
        | MRV_MI_AHB_READ_ERROR_MASK  \
        | MRV_MI_AHB_WRITE_ERROR_MASK \
        | MRV_MI_SP_CR_FIFO_FULL_MASK \
        | MRV_MI_SP_CB_FIFO_FULL_MASK \
        | MRV_MI_SP_Y_FIFO_FULL_MASK  \
        | MRV_MI_MP_CR_FIFO_FULL_MASK \
        | MRV_MI_MP_CB_FIFO_FULL_MASK \
        | MRV_MI_MP_Y_FIFO_FULL_MASK  \
      )
    #define MRV_MI_ALL_STAT_SHIFT 0

    // Register: mi_status_clr: MI Status Clear Register (0x0110)
    /* same defines as for mi_status
    // Slice: ahb_read_error:
    // Clear status of AHB read error flag
    #define MRV_MI_AHB_READ_ERROR
    #define MRV_MI_AHB_READ_ERROR_MASK 0x00000200
    #define MRV_MI_AHB_READ_ERROR_SHIFT 9
    // Slice: ahb_write_error:
    // Clear status of AHB write error flag
    #define MRV_MI_AHB_WRITE_ERROR
    #define MRV_MI_AHB_WRITE_ERROR_MASK 0x00000100
    #define MRV_MI_AHB_WRITE_ERROR_SHIFT 8
    // Slice: sp_cr_fifo_full:
    // Clear status of Cr FIFO full flag in self path
    #define MRV_MI_SP_CR_FIFO_FULL
    #define MRV_MI_SP_CR_FIFO_FULL_MASK 0x00000040
    #define MRV_MI_SP_CR_FIFO_FULL_SHIFT 6
    // Slice: sp_cb_fifo_full:
    // Clear status of Cb FIFO full flag in self path
    #define MRV_MI_SP_CB_FIFO_FULL
    #define MRV_MI_SP_CB_FIFO_FULL_MASK 0x00000020
    #define MRV_MI_SP_CB_FIFO_FULL_SHIFT 5
    // Slice: sp_y_fifo_full:
    // Clear status of Y FIFO full flag in self path
    #define MRV_MI_SP_Y_FIFO_FULL
    #define MRV_MI_SP_Y_FIFO_FULL_MASK 0x00000010
    #define MRV_MI_SP_Y_FIFO_FULL_SHIFT 4
    // Slice: mp_cr_fifo_full:
    // Clear status of Cr FIFO full flag in main path
    #define MRV_MI_MP_CR_FIFO_FULL
    #define MRV_MI_MP_CR_FIFO_FULL_MASK 0x00000004
    #define MRV_MI_MP_CR_FIFO_FULL_SHIFT 2
    // Slice: mp_cb_fifo_full:
    // Clear status of Cb FIFO full flag in main path
    #define MRV_MI_MP_CB_FIFO_FULL
    #define MRV_MI_MP_CB_FIFO_FULL_MASK 0x00000002
    #define MRV_MI_MP_CB_FIFO_FULL_SHIFT 1
    // Slice: mp_y_fifo_full:
    // Clear status of Y FIFO full flag in main path
    #define MRV_MI_MP_Y_FIFO_FULL
    #define MRV_MI_MP_Y_FIFO_FULL_MASK 0x00000001
    #define MRV_MI_MP_Y_FIFO_FULL_SHIFT 0
    */
#endif  // MARVIN_FEATURE_MI_STATUSFLAGS

  #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    // Register: mi_sp_y_pic_width: Y component image width (0x0114)
    // Slice: sp_y_pic_width:
    // Image width of the self picture Y component or RGB picture in pixel.
    // For YCbCr 4:2:x and RGB 565 the image width must be a multiple of 2. If no line stride is used but flipping required, the image width must be a multiple of 8 for 4:2:x planar or 4 for 4:4:4 planar/4:2:x semi planar. There are no restrictions for RGB 888/666.
    // In planar mode the image width of the Cb and Cr component is assumed according to the YCbCr format, i.e. half for 4:2:x and the same size for 4:4:4. In semi planar 4:2:x mode the image width of the Cb component (which includes Cr) is assumed the same size. In interleave mode no Cb/Cr image width is used.
    // Note: Image width always refers to the picture width of the output image. This is particularly important when rotating.
    // Note: Programmed value becomes effective immediately. So write to the register only if no picture data is sent to the self path.
    #define MRV_MI_SP_Y_PIC_WIDTH
    #define MRV_MI_SP_Y_PIC_WIDTH_MASK 0x00000FFF
    #define MRV_MI_SP_Y_PIC_WIDTH_SHIFT 0

    // Register: mi_sp_y_pic_height: Y component image height (0x0118)
    // Slice: sp_y_pic_height:
    // Image height of the y component or RGB picture in pixel.
    // In planar and semi planar mode the image width of the cb and cr component is assumed according to the YCbCr format, i.e. half for 4:2:0 and the same for 4:2:2 and 4:4:4.
    // Note: Image height always refers to the picture height of the output image. This is particularly important when rotating.
    // Note: Programmed value becomes effective immediately. So write to the register only if no picture data is sent to the self path.
    #define MRV_MI_SP_Y_PIC_HEIGHT
    #define MRV_MI_SP_Y_PIC_HEIGHT_MASK 0x00000FFF
    #define MRV_MI_SP_Y_PIC_HEIGHT_SHIFT 0

    // Register: mi_sp_y_pic_size: Y component image size (0x011c)
    // Slice: sp_y_pic_size:
    // Image size of the Y component or RGB picture in pixel which has to be the Y line length multiplied by the Y image height (sp_y_llength * sp_y_pic_height).
    // In planar mode the image size of the Cb and Cr component is assumed according to the YCbCr format, i.e. a quarter for 4:2:0, half for 4:2:2 and the same for 4:4:4. In semi planar mode the image size of the Cb and Cr component is assumed half for 4:2:0 and the same size for 4:2:2.
    // Note: Programmed value becomes effective immediately. So write to the register only if no picture data is sent to the self path.
    #define MRV_MI_SP_Y_PIC_SIZE
    #define MRV_MI_SP_Y_PIC_SIZE_MASK 0x01FFFFFF
    #define MRV_MI_SP_Y_PIC_SIZE_SHIFT 0

    // Register: mi_dma_ctrl: DMA control register (0x0120)
    //#define MRV_DMA_FRAME_END_DIS       0x00000400  //[10] rw suppresses v_end_so that no frame end can be detected by following instances
    //#define MRV_DMA_CONTINUOUS_EN       0x00000200  //[9] rw Enables continuous mode. If set the same frame is read back
    //                                                //      over and over. A start pulse on dma_start is need only for the
    //                                                //      first time. To stop continuous mode reset this bit (takes
    //                                                //      effect after the next frame end) or execute a soft reset.
    //#define MRV_DMA_BYTE_SWAP           0x00000100  //[8] rw enables change of DMA byte order of the 32 bit input word at read port
    //#define MRV_DMA_IN_TYPE_MASK        0x000000C0  //[7:6] rw set the input format of DMA
    //#define MRV_DMA_IN_YUV444_TYPE      0x000000C0  //[7:6] rw YCbCr 4:4:4
    //#define MRV_DMA_IN_YUV422_TYPE      0x00000080  //[7:6] rw YCbCr 4:2:2
    //#define MRV_DMA_IN_YUV420_TYPE      0x00000040  //[7:6] rw YCbCr 4:2:0
    //#define MRV_DMA_IN_YUV400_TYPE      0x00000000  //[7:6] rw YCbCr 4:0:0
    //#define MRV_DMA_READ_FORMAT_MASK    0x00000030  //[5:4] rw defines how picture data is read from memory
    //#define MRV_DMA_READ_INTERLEAVED    0x00000020  //[5:4] rw interleaved
    //#define MRV_DMA_READ_SEMI_PLANAR    0x00000010  //[5:4] rw semi-planar
    //#define MRV_DMA_READ_PLANAR         0x00000000  //[5:4] rw planar
    //#define MRV_DMA_BURST_LEN_CHROM_8   0x00000004  //[2] rw 8-beat bursts
    //#define MRV_DMA_BURST_LEN_CHROM_4   0x00000000  //[2] rw 4-beat bursts
    //#define MRV_DMA_BURST_LEN_LUM_8     0x00000001  //[0] rw 8-beat bursts
    //#define MRV_DMA_BURST_LEN_LUM_4     0x00000000  //[0] rw 4-beat bursts
    // Slice: dma_frame_end_disable:
    // Suppresses v_end so that no frame end can be detected by following instances. Note: The dma_ready interrupt is raised as usual, but the dma_frame_end interrupt will not be generated until v_end has been enabled again.
    #define MRV_MI_DMA_FRAME_END_DISABLE
    #define MRV_MI_DMA_FRAME_END_DISABLE_MASK 0x00000400
    #define MRV_MI_DMA_FRAME_END_DISABLE_SHIFT 10
    // Slice: dma_continuous_en:
    // Enables continuous mode. If set the same frame is read back over and over. A start pulse on dma_start is needed only for the first time. To stop continuous mode reset this bit (takes effect after the next frame end) or execute a soft reset. This bit is intended to be used in conjunction with the Superimpose feature.
    #define MRV_MI_DMA_CONTINUOUS_EN
    #define MRV_MI_DMA_CONTINUOUS_EN_MASK 0x00000200
    #define MRV_MI_DMA_CONTINUOUS_EN_SHIFT 9
    // Slice: dma_byte_swap:
    // Enables change of DMA byte order of the 32 bit input word at read port
    // 1: byte order is mirrored but the bit order within one byte doesn’t change
    // 0: no byte mirroring
    #define MRV_MI_DMA_BYTE_SWAP
    #define MRV_MI_DMA_BYTE_SWAP_MASK 0x00000100
    #define MRV_MI_DMA_BYTE_SWAP_SHIFT 8
    // Slice: dma_inout_format:
    // Selects input/output format of DMA picture.
    // 11: YCbCr  4:4:4
    // 10: YCbCr  4:2:2
    // 01: YCbCr  4:2:0
    // 00: YCbCr  4:0:0
    #define MRV_MI_DMA_INOUT_FORMAT
    #define MRV_MI_DMA_INOUT_FORMAT_MASK 0x000000C0
    #define MRV_MI_DMA_INOUT_FORMAT_SHIFT 6
    #define MRV_MI_DMA_INOUT_FORMAT_YUV444 3 // 11: YCbCr  4:4:4
    #define MRV_MI_DMA_INOUT_FORMAT_YUV422 2 // 10: YCbCr  4:2:2
    #define MRV_MI_DMA_INOUT_FORMAT_YUV420 1 // 01: YCbCr  4:2:0
    #define MRV_MI_DMA_INOUT_FORMAT_YUV400 0 // 00: YCbCr  4:0:0
    // Slice: dma_read_format:
    // Defines how YCbCr picture data is read from memory.
    // 00: planar
    // 01: semi planar, for YCbCr 4:2:x
    // 10: interleaved (combined), for YCbCr 4:2:2 only
    // 11: reserved
    #define MRV_MI_DMA_READ_FORMAT
    #define MRV_MI_DMA_READ_FORMAT_MASK 0x00000030
    #define MRV_MI_DMA_READ_FORMAT_SHIFT 4
    #define MRV_MI_DMA_READ_FORMAT_PLANAR      0 // 00: planar
    #define MRV_MI_DMA_READ_FORMAT_SEMIPLANAR  1 // 01: semi planar, for YCbCr 4:2:x
    #define MRV_MI_DMA_READ_FORMAT_INTERLEAVED 2 // 10: interleaved (combined), for YCbCr 4:2:2 only
#if (MARVIN_FEATURE_MI_BURST_16 == MARVIN_FEATURE_EXIST)
    // Slice: dma_burst_len_chrom:
    // Burst length for Cb or Cr data affecting DMA read port.
    // 0: 4-beat bursts
    // 1: 8-beat bursts
    // 2: 16-beat bursts
    #define MRV_MI_DMA_BURST_LEN_CHROM
    #define MRV_MI_DMA_BURST_LEN_CHROM_MASK 0x0000000C
    #define MRV_MI_DMA_BURST_LEN_CHROM_SHIFT 2
    #define MRV_MI_DMA_BURST_LEN_CHROM_4  0 // 0: 4-beat bursts
    #define MRV_MI_DMA_BURST_LEN_CHROM_8  1 // 1: 8-beat bursts
    #define MRV_MI_DMA_BURST_LEN_CHROM_16 2 // 2: 16-beat bursts
    // Slice: dma_burst_len_lum:
    // Burst length for Y data affecting DMA read port.
    // 0: 4-beat bursts
    // 1: 8-beat bursts
    // 2: 16-beat bursts
    #define MRV_MI_DMA_BURST_LEN_LUM
    #define MRV_MI_DMA_BURST_LEN_LUM_MASK 0x00000003
    #define MRV_MI_DMA_BURST_LEN_LUM_SHIFT 0
    #define MRV_MI_DMA_BURST_LEN_LUM_4  0 // 0: 4-beat bursts
    #define MRV_MI_DMA_BURST_LEN_LUM_8  1 // 1: 8-beat bursts
    #define MRV_MI_DMA_BURST_LEN_LUM_16 2 // 2: 16-beat bursts
#else //#if (MARVIN_FEATURE_MI_BURST_16 == MARVIN_FEATURE_EXIST)
    // Slice: dma_burst_len_chrom:
    // Burst length for Cb or Cr data affecting DMA read port.
    // 0: 4-beat bursts
    // 1: 8-beat bursts
    // Ignored if 8-beat bursts not supported.
    #define MRV_MI_DMA_BURST_LEN_CHROM
    #define MRV_MI_DMA_BURST_LEN_CHROM_MASK 0x00000004
    #define MRV_MI_DMA_BURST_LEN_CHROM_SHIFT 2
    #define MRV_MI_DMA_BURST_LEN_CHROM_4 0 // 0: 4-beat bursts
    #define MRV_MI_DMA_BURST_LEN_CHROM_8 1 // 1: 8-beat bursts
    // Slice: dma_burst_len_lum:
    // Burst length for Y data affecting DMA read port.
    // 0: 4-beat bursts
    // 1: 8-beat bursts
    // Ignored if 8-beat bursts not supported.
    #define MRV_MI_DMA_BURST_LEN_LUM
    #define MRV_MI_DMA_BURST_LEN_LUM_MASK 0x00000001
    #define MRV_MI_DMA_BURST_LEN_LUM_SHIFT 0
    #define MRV_MI_DMA_BURST_LEN_LUM_4 0 // 0: 4-beat bursts
    #define MRV_MI_DMA_BURST_LEN_LUM_8 1 // 1: 8-beat bursts
#endif //#if (MARVIN_FEATURE_MI_BURST_16 == MARVIN_FEATURE_EXIST)

    // Register: mi_dma_start: DMA start register (0x0124)
    //#define MRV_DMA_SP_ENABLE           0x00000001  //[1] enables DMA access including self path
    // Slice: dma_start:
    // Enables DMA access. Additionally main or self path has to be enabled separately.
    #define MRV_MI_DMA_START
    #define MRV_MI_DMA_START_MASK 0x00000001
    #define MRV_MI_DMA_START_SHIFT 0

    // Register: mi_dma_status: DMA status register (0x0128)
    // Slice: dma_active:
    // If set DMA access is active.
    #define MRV_MI_DMA_ACTIVE
    #define MRV_MI_DMA_ACTIVE_MASK 0x00000001
    #define MRV_MI_DMA_ACTIVE_SHIFT 0
#endif  // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)

#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V4)
// Register: mi_pixel_cnt: Counter value of DPCC data bytes (0x012c)
// Slice: pixel_cnt:
// Counter value specifies the number of dpcc data bytes of the last transmitted frame. Updated at frame end.
// A soft reset will set the pixel counter to zero.
#define MRV_MI_PIXEL_CNT
#define MRV_MI_PIXEL_CNT_MASK 0x01FFFFFF
#define MRV_MI_PIXEL_CNT_SHIFT 0
#endif //#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V4)

/********************************************************************/
/* JPEG Encoder Programming Registers                               */
/********************************************************************/

// Register: jpe_gen_header: command to start stream header generation (0x0000)
//#define MRV_GEN_HEADER           0x00000001 //[0] w start flag for JFIF stream header generation
// Slice: gen_header:
// "1" = Start command to generate stream header ;
// auto reset to zero after one clock cycle
#define MRV_JPE_GEN_HEADER
#define MRV_JPE_GEN_HEADER_MASK 0x00000001
#define MRV_JPE_GEN_HEADER_SHIFT 0

// Register: jpe_encode: Start command to start JFIF stream encoding (0x0004)
//#define MRV_ENCODE               0x00000001 //[0] w start flag for JPEG encoding
#if ((MARVIN_FEATURE_JPE >= MARVIN_FEATURE_JPE_V3) && \
     (MARVIN_FEATURE_JPE <= MARVIN_FEATURE_JPE_V3) && \
     (MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V1))

    // Slice: cont_mode:
    // Encoder continous mode
    // "00": encoder stops at frame end (corresponds to former behavior)
    // "01": encoder starts automatically to encode the next frame
    // "10": unused
    // "11": encoder first generates next header and then encodes automatically the next frame
    // These settings are checked after encoding one frame. They are not auto-reset by hardware.
    #define MRV_JPE_CONT_MODE
    #define MRV_JPE_CONT_MODE_MASK 0x00000030
    #define MRV_JPE_CONT_MODE_SHIFT 4
    #define MRV_JPE_CONT_MODE_STOP   0 // "00": encoder stops at frame end (corresponds to former behavior)
    #define MRV_JPE_CONT_MODE_NEXT   1 // "01": encoder starts automatically to encode the next frame
    #define MRV_JPE_CONT_MODE_HEADER 3 // "11": encoder first generates next header and then encodes automatically the next frame
#endif // MARVIN_FEATURE_JPE
// Slice: encode:
// "1" = Start command to start JFIF stream encoding;
// auto reset to zero after one clock cycle.
// This bit is write-only: reading result is always zero!
#define MRV_JPE_ENCODE
#define MRV_JPE_ENCODE_MASK 0x00000001
#define MRV_JPE_ENCODE_SHIFT 0

#if ((MARVIN_FEATURE_JPE >= MARVIN_FEATURE_JPE_V2) && \
     (MARVIN_FEATURE_JPE <= MARVIN_FEATURE_JPE_V3))
    // Register: jpe_init: Automatic configuration update (INIT) (0x0008)
    // Slice: JP_INIT:
    // "1" = Immediate start of JPEG encoder.
    // This bit has to be set after "Encode" to start the JPEG encoder. The "Encode" command becomes active either with JP_INIT or with the input signal "CFG_UPD".
    // auto reset to zero after one clock cycle !!!
    #define MRV_JPE_JP_INIT
    #define MRV_JPE_JP_INIT_MASK 0x00000001
    #define MRV_JPE_JP_INIT_SHIFT 0
#endif // MARVIN_FEATURE_JPE

// Register: jpe_y_scale_en: Y value scaling control register (0x000c)
//#define MRV_Y_SCALE_EN           0x00000001 //[0] rw Y scale flag
// Slice: y_scale_en:
// Y scale flag
// 1: scaling Y input from [16..235] to [0..255]
// 0: no Y input scaling
#define MRV_JPE_Y_SCALE_EN
#define MRV_JPE_Y_SCALE_EN_MASK 0x00000001
#define MRV_JPE_Y_SCALE_EN_SHIFT 0

// Register: jpe_cbcr_scale_en: Cb/Cr value scaling control register (0x0010)
//#define MRV_CBCR_SCALE_EN        0x00000001 //[0] rw Cb/Cr scale flag
// Slice: cbcr_scale_en:
// Cb/Cr scale flag
// 1: scaling Cb/Cr input from [16..240] to [0..255]
// 0: no Cb/Cr input scaling
#define MRV_JPE_CBCR_SCALE_EN
#define MRV_JPE_CBCR_SCALE_EN_MASK 0x00000001
#define MRV_JPE_CBCR_SCALE_EN_SHIFT 0

// Register: jpe_table_flush: header generation debug register (0x0014)
//#define MRV_TABLE_FLUSH_EN       0x00000001 //[0] rw header generation debug control flag
// Slice: table_flush:
// header generation debug control flag
// (controls transmission of last header bytes if the 64 bit output buffer is not completely filled)
// 1: immediately transmit last header bytes
// 0: wait for encoded image data to fill output buffer
#define MRV_JPE_TABLE_FLUSH
#define MRV_JPE_TABLE_FLUSH_MASK 0x00000001
#define MRV_JPE_TABLE_FLUSH_SHIFT 0

// Register: jpe_enc_hsize: JPEG codec horizontal image size for encoding (0x0018)
//#define MRV_IMG_HSIZE            0x000007FF //[10:0] rw horizontal size of image to encode in pixels
// Slice: enc_hsize:
// JPEG codec horizontal image size for R2B and SGEN blocks
#define MRV_JPE_ENC_HSIZE
#if (MARVIN_FEATURE_JPE_SIZE == MARVIN_FEATURE_JPE_SIZE_11BITS)
    #define MRV_JPE_ENC_HSIZE_MASK 0x000007FF
#elif (MARVIN_FEATURE_JPE_SIZE == MARVIN_FEATURE_JPE_SIZE_12BITS)
    #define MRV_JPE_ENC_HSIZE_MASK 0x00000FFF
#elif (MARVIN_FEATURE_JPE_SIZE == MARVIN_FEATURE_JPE_SIZE_13BITS)
    #define MRV_JPE_ENC_HSIZE_MASK 0x00001FFF
#else // MARVIN_FEATURE_JPE_SIZE
    #error "unknown value for MARVIN_FEATURE_JPE_SIZE"
#endif // MARVIN_FEATURE_JPE_SIZE
#define MRV_JPE_ENC_HSIZE_SHIFT 0

// Register: jpe_enc_vsize: JPEG codec vertical image size for encoding (0x001c)
//#define MRV_IMG_VSIZE            0x000007FF //[10:0] rw vertical size of image to encode in lines
// Slice: enc_vsize:
// JPEG codec vertical image size for R2B and SGEN blocks
#define MRV_JPE_ENC_VSIZE
#if (MARVIN_FEATURE_JPE_SIZE == MARVIN_FEATURE_JPE_SIZE_11BITS)
    #define MRV_JPE_ENC_VSIZE_MASK 0x000007FF
#elif (MARVIN_FEATURE_JPE_SIZE == MARVIN_FEATURE_JPE_SIZE_12BITS)
    #define MRV_JPE_ENC_VSIZE_MASK 0x00000FFF
#elif (MARVIN_FEATURE_JPE_SIZE == MARVIN_FEATURE_JPE_SIZE_13BITS) // VSIZE 12 bit!
    #define MRV_JPE_ENC_VSIZE_MASK 0x00000FFF
#else // MARVIN_FEATURE_JPE_SIZE
    #error "unknown value for MARVIN_FEATURE_JPE_SIZE"
#endif // MARVIN_FEATURE_JPE_SIZE
#define MRV_JPE_ENC_VSIZE_SHIFT 0

// Register: jpe_pic_format: JPEG picture encoding format (0x0020)
//#define MRV_PIC_ENC_FORMAT        0x00000007 //[2:0] rw picture format for encoding
//#define MRV_PIC_ENC_FORMAT_YUV422 0x00000001 //"001": YUV 4:2:2 format
// Slice: enc_pic_format:
// "0:0:1"  = 4:2:2 format
// "1:x:x"  = 4:0:0 format
#define MRV_JPE_ENC_PIC_FORMAT
#define MRV_JPE_ENC_PIC_FORMAT_MASK 0x00000007
#define MRV_JPE_ENC_PIC_FORMAT_SHIFT 0
#define MRV_JPE_ENC_PIC_FORMAT_422 1 // "0:0:1"  = 4:2:2 format
#define MRV_JPE_ENC_PIC_FORMAT_400 4 // "1:x:x"  = 4:0:0 format

// Register: jpe_restart_interval: restart marker insertion register (0x0024)
// Slice: restart_interval:
// No of MCU in  reset interval via host
#define MRV_JPE_RESTART_INTERVAL
#define MRV_JPE_RESTART_INTERVAL_MASK 0x0000FFFF
#define MRV_JPE_RESTART_INTERVAL_SHIFT 0

// Register: jpe_tq_y_select: Q- table selector 0, quant. table for Y component (0x0028)
//#define MRV_TQ_Y_SELECT          0x00000003 //[1:0] rw selects quatization table for Y component
// Slice: tq0_select:
// "00" = qtable 0
// "01" = qtable 1
// "10" = qtable 2
// "11" = qtable 3
#define MRV_JPE_TQ0_SELECT
#define MRV_JPE_TQ0_SELECT_MASK 0x00000003
#define MRV_JPE_TQ0_SELECT_SHIFT 0

// Register: jpe_tq_u_select: Q- table selector 1, quant. table for U component (0x002c)
//#define MRV_TQ_U_SELECT          0x00000003 //[1:0] rw selects quatization table for U component
// Slice: tq1_select:
// "00" = qtable 0
// "01" = qtable 1
// "10" = qtable 2
// "11" = qtable 3
#define MRV_JPE_TQ1_SELECT
#define MRV_JPE_TQ1_SELECT_MASK 0x00000003
#define MRV_JPE_TQ1_SELECT_SHIFT 0

// Register: jpe_tq_v_select: Q- table selector 2, quant. table for V component (0x0030)
//#define MRV_TQ_V_SELECT          0x00000003 //[1:0] rw selects quatization table for V component
// Slice: tq2_select:
// "00" = qtable 0
// "01" = qtable 1
// "10" = qtable 2
// "11" = qtable 3
#define MRV_JPE_TQ2_SELECT
#define MRV_JPE_TQ2_SELECT_MASK 0x00000003
#define MRV_JPE_TQ2_SELECT_SHIFT 0

//#define MRV_TQ_TAB3          3  //"11": quantization table 3
//#define MRV_TQ_TAB2          2  //"10": quantization table 2
//#define MRV_TQ_TAB1          1  //"01": quantization table 1
//#define MRV_TQ_TAB0          0  //"00": quantization table 0
#define MRV_JPE_TQ_SELECT_TAB3 3  // "11": qtable 3
#define MRV_JPE_TQ_SELECT_TAB2 2  // "10": qtable 2
#define MRV_JPE_TQ_SELECT_TAB1 1  // "01": qtable 1
#define MRV_JPE_TQ_SELECT_TAB0 0  // "00": qtable 0

// Register: jpe_dc_table_select: Huffman table selector for DC values (0x0034)
//#define MRV_DC_V_TABLE           0x00000004 //[2] rw DC table selection for V component
//#define MRV_DC_U_TABLE           0x00000002 //[1] DC table selection for U component
//#define MRV_DC_Y_TABLE           0x00000001 //[0] DC table selection for Y component
// Slice: dc_table_select_y:
// "0" = dc table 0; color component 0 (Y)
// "1" = dc table 1; color component 0 (Y)
#define MRV_JPE_DC_TABLE_SELECT_Y
#define MRV_JPE_DC_TABLE_SELECT_Y_MASK 0x00000001
#define MRV_JPE_DC_TABLE_SELECT_Y_SHIFT 0
// Slice: dc_table_select_u:
// "0" = dc table 0; color component 1 (U)
// "1" = dc table 1; color component 1 (U)
#define MRV_JPE_DC_TABLE_SELECT_U
#define MRV_JPE_DC_TABLE_SELECT_U_MASK 0x00000002
#define MRV_JPE_DC_TABLE_SELECT_U_SHIFT 1
// Slice: dc_table_select_v:
// "0" = dc table 0; color component 2 (V)
// "1" = dc table 1; color component 2 (V)
#define MRV_JPE_DC_TABLE_SELECT_V
#define MRV_JPE_DC_TABLE_SELECT_V_MASK 0x00000004
#define MRV_JPE_DC_TABLE_SELECT_V_SHIFT 2

// Register: jpe_ac_table_select: Huffman table selector for AC values (0x0038)
//#define MRV_AC_V_TABLE           0x00000004 //[2] rw AC table selection for V component
//#define MRV_AC_U_TABLE           0x00000002 //[1] AC table selection for U component
//#define MRV_AC_Y_TABLE           0x00000001 //[0] AC table selection for Y component
// Slice: ac_table_select_y:
// "0" = ac table 0; component 0 (Y)
// "1" = ac table 1; component 0 (Y)
#define MRV_JPE_AC_TABLE_SELECT_Y
#define MRV_JPE_AC_TABLE_SELECT_Y_MASK 0x00000001
#define MRV_JPE_AC_TABLE_SELECT_Y_SHIFT 0
// Slice: ac_table_select_u:
// "0" = ac table 0; component 1 (U)
// "1" = ac table 1; component 1 (U)
#define MRV_JPE_AC_TABLE_SELECT_U
#define MRV_JPE_AC_TABLE_SELECT_U_MASK 0x00000002
#define MRV_JPE_AC_TABLE_SELECT_U_SHIFT 1
// Slice: ac_table_select_v:
// "0" = ac table 0; component 2 (V)
// "1" = ac table 1; component 2 (V)
#define MRV_JPE_AC_TABLE_SELECT_V
#define MRV_JPE_AC_TABLE_SELECT_V_MASK 0x00000004
#define MRV_JPE_AC_TABLE_SELECT_V_SHIFT 2

// Register: jpe_table_data: table programming register (0x003c)
//#define MRV_TABLE_DATA_HIGH      0x0000FF00 //[15:8] w table data upper byte
//#define MRV_TABLE_DATA_LOW       0x000000FF //[7:0] w table data lower byte
// Slice: table_wdata_h:
// Table data MSB
#define MRV_JPE_TABLE_WDATA_H
#define MRV_JPE_TABLE_WDATA_H_MASK 0x0000FF00
#define MRV_JPE_TABLE_WDATA_H_SHIFT 8
// Slice: table_wdata_l:
// Table data LSB
#define MRV_JPE_TABLE_WDATA_L
#define MRV_JPE_TABLE_WDATA_L_MASK 0x000000FF
#define MRV_JPE_TABLE_WDATA_L_SHIFT 0

// Register: jpe_table_id: table programming select register (0x0040)
//#define MRV_TABLE_ID             0x00000007 //[2:0] w table ID for write access
//#define MRV_TAB_ID_HUFFAC1   7  //"111": Huffman AC table 1
//#define MRV_TAB_ID_HUFFDC1   6  //"110": Huffman DC table 1
//#define MRV_TAB_ID_HUFFAC0   5  //"101": Huffman AC table 0
//#define MRV_TAB_ID_HUFFDC0   4  //"100": Huffman DC table 0
//#define MRV_TAB_ID_QUANT3    3  //"011": quantization table 3
//#define MRV_TAB_ID_QUANT2    2  //"010": quantization table 2
//#define MRV_TAB_ID_QUANT1    1  //"001": quantization table 1
//#define MRV_TAB_ID_QUANT0    0  //"000": quantization table 0
//                                //(prior to Huffman table writes table length must be
//                                //programmed!, default are the table lengths defined in Annex K)
// Slice: table_id:
// select table
// "0000" : Q-table 0
// "0001" : Q-table 1
// "0010" : Q-table 2
// "0011" : Q-table 3
// "0100" : VLC DC-table 0
// "0101" : VLC AC-table 0
// "0110" : VLC DC-table 1
// "0111" : VLC AC-table 1
// "1xxx" : reserved for debug
#define MRV_JPE_TABLE_ID
#define MRV_JPE_TABLE_ID_MASK 0x0000000F
#define MRV_JPE_TABLE_ID_SHIFT 0
#define MRV_JPE_TABLE_ID_QUANT0  0 // "0000" : Q-table 0
#define MRV_JPE_TABLE_ID_QUANT1  1 // "0001" : Q-table 1
#define MRV_JPE_TABLE_ID_QUANT2  2 // "0010" : Q-table 2
#define MRV_JPE_TABLE_ID_QUANT3  3 // "0011" : Q-table 3
#define MRV_JPE_TABLE_ID_VLC_DC0 4 // "0100" : VLC DC-table 0
#define MRV_JPE_TABLE_ID_VLC_AC0 5 // "0101" : VLC AC-table 0
#define MRV_JPE_TABLE_ID_VLC_DC1 6 // "0110" : VLC DC-table 1
#define MRV_JPE_TABLE_ID_VLC_AC1 7 // "0111" : VLC AC-table 1

// Register: jpe_tac0_len: Huffman AC table 0 length (0x0044)
// Slice: tac0_len:
// Huffman table length for ac0 table
#define MRV_JPE_TAC0_LEN
#define MRV_JPE_TAC0_LEN_MASK 0x000000FF
#define MRV_JPE_TAC0_LEN_SHIFT 0

// Register: jpe_tdc0_len: Huffman DC table 0 length (0x0048)
// Slice: tdc0_len:
// Huffman table length for dc0 table
#define MRV_JPE_TDC0_LEN
#define MRV_JPE_TDC0_LEN_MASK 0x000000FF
#define MRV_JPE_TDC0_LEN_SHIFT 0

// Register: jpe_tac1_len: Huffman AC table 1 length (0x004c)
// Slice: tac1_len:
// Huffman table length for ac1 table
#define MRV_JPE_TAC1_LEN
#define MRV_JPE_TAC1_LEN_MASK 0x000000FF
#define MRV_JPE_TAC1_LEN_SHIFT 0

// Register: jpe_tdc1_len: Huffman DC table 1 length (0x0050)
// Slice: tdc1_len:
// Huffman table length for dc1 table
#define MRV_JPE_TDC1_LEN
#define MRV_JPE_TDC1_LEN_MASK 0x000000FF
#define MRV_JPE_TDC1_LEN_SHIFT 0

// Register: jpe_encoder_busy: encoder status flag (0x0058)
//#define MRV_BUSY                 0x00000001 //[0] rh JPEG encoder status
// Slice: codec_busy:
// Bit 0 = "1" : JPEG codec in process
#define MRV_JPE_CODEC_BUSY
#define MRV_JPE_CODEC_BUSY_MASK 0x00000001
#define MRV_JPE_CODEC_BUSY_SHIFT 0

// Register: jpe_header_mode: header mode definition (0x005c)
//#define MRV_HEADER_MODE          0x00000003 //[1:0] rw stream header generation mode
//#define MRV_HEADER_JFIF      2  //"10": JFIF header
//#define MRV_HEADER_NO        0  //"00": no APP header
// Slice: header_mode:
// "00" = no APPn header
// "01" = no APPn header
// "10" = JFIF header
// "11" = JFIF header
#define MRV_JPE_HEADER_MODE
#define MRV_JPE_HEADER_MODE_MASK 0x00000003
#define MRV_JPE_HEADER_MODE_SHIFT 0
#define MRV_JPE_HEADER_MODE_NO    0 // "00" = no APPn header
#define MRV_JPE_HEADER_MODE_JFIF  2 // "10" = JFIF header

// Register: jpe_encode_mode: encode mode (0x0060)
// Slice: encode_mode:
// Always  "1", because this is the encoder only edition
#define MRV_JPE_ENCODE_MODE
#define MRV_JPE_ENCODE_MODE_MASK 0x00000001
#define MRV_JPE_ENCODE_MODE_SHIFT 0

// Register: jpe_debug: debug information register (0x0064)
//#define MRV_VLC_TABLE_BUSY       0x00000020 //[5] rh VLC table programming debug signal
//                                            //(JPEG encoder will initialize Huffman tables after
//                                            //reset, busy for ~400 clock cycles after reset)
//#define MRV_R2B_MEM_FULL         0x00000010 //[4] rh line memory status of the raster-to-block converter
//                                            //(signals that the memory has run full)
//#define MRV_VLC_ENC_BUSY         0x00000008 //[3] rh VLC encode process active
//#define MRV_Q_TABLE_ACCESS       0x00000004 //[2] rh quantization table access
// Slice: deb_bad_table_access:
// Debug signal only (set if an access to the TABLE_DATA or to the TABLE_ID register is performed, when the JPEG_ENCODER is busy. In this case a default PVCI Acknowledge is generated. Thus the configuration bus is not blocked)
#define MRV_JPE_DEB_BAD_TABLE_ACCESS
#define MRV_JPE_DEB_BAD_TABLE_ACCESS_MASK 0x00000100
#define MRV_JPE_DEB_BAD_TABLE_ACCESS_SHIFT 8
// Slice: deb_vlc_table_busy:
// Debug signal only (vlc access to huff-tables)
#define MRV_JPE_DEB_VLC_TABLE_BUSY
#define MRV_JPE_DEB_VLC_TABLE_BUSY_MASK 0x00000020
#define MRV_JPE_DEB_VLC_TABLE_BUSY_SHIFT 5
// Slice: deb_r2b_memory_full:
// Debug signal only (line memory status of r2b)
#define MRV_JPE_DEB_R2B_MEMORY_FULL
#define MRV_JPE_DEB_R2B_MEMORY_FULL_MASK 0x00000010
#define MRV_JPE_DEB_R2B_MEMORY_FULL_SHIFT 4
// Slice: deb_vlc_encode_busy:
// Debug signal only (vlc encode processing active)
#define MRV_JPE_DEB_VLC_ENCODE_BUSY
#define MRV_JPE_DEB_VLC_ENCODE_BUSY_MASK 0x00000008
#define MRV_JPE_DEB_VLC_ENCODE_BUSY_SHIFT 3
// Slice: deb_qiq_table_acc:
// Debug signal only (QIQ table access)
#define MRV_JPE_DEB_QIQ_TABLE_ACC
#define MRV_JPE_DEB_QIQ_TABLE_ACC_MASK 0x00000004
#define MRV_JPE_DEB_QIQ_TABLE_ACC_SHIFT 2

//#define MRV_JPGINT_VLC_TABLE_ERR    0x00000400 //[10] illegal table interrupt
//#define MRV_JPGINT_IMG_SIZE_ERR     0x00000200 //[9] programmed image size does not match incoming image interrupt
//#define MRV_JPGINT_DCT_ERR          0x00000080 //[7] block start mismatch interrupt
//#define MRV_JPGINT_VLC_SYM_ERR      0x00000010 //[4] illegal Huffman symbol interrupt

// Register: jpe_error_imr: JPEG error interrupt mask register (0x0068)
// Slice: vlc_table_err:
// "1" = interrupt is  activated (masked in)
#define MRV_JPE_VLC_TABLE_ERR
#define MRV_JPE_VLC_TABLE_ERR_MASK 0x00000400
#define MRV_JPE_VLC_TABLE_ERR_SHIFT 10
// Slice: r2b_IMG_size_err:
// "1" = interrupt is  activated (masked in)
#define MRV_JPE_R2B_IMG_SIZE_ERR
#define MRV_JPE_R2B_IMG_SIZE_ERR_MASK 0x00000200
#define MRV_JPE_R2B_IMG_SIZE_ERR_SHIFT 9
// Slice: DCT_ERR:
// "1" = interrupt is  activated (masked in)
#define MRV_JPE_DCT_ERR
#define MRV_JPE_DCT_ERR_MASK 0x00000080
#define MRV_JPE_DCT_ERR_SHIFT 7
// Slice: vlc_symbol_err:
// "1" = interrupt is  activated (masked in)
#define MRV_JPE_VLC_SYMBOL_ERR
#define MRV_JPE_VLC_SYMBOL_ERR_MASK 0x00000010
#define MRV_JPE_VLC_SYMBOL_ERR_SHIFT 4

// combination of all JPE error lines
#define MRV_JPE_ALL_ERR
#define MRV_JPE_ALL_ERR_MASK \
  (   0 \
    | MRV_JPE_VLC_TABLE_ERR_MASK \
    | MRV_JPE_R2B_IMG_SIZE_ERR_MASK \
    | MRV_JPE_DCT_ERR_MASK \
    | MRV_JPE_VLC_SYMBOL_ERR_MASK \
  )
#define MRV_JPE_ALL_ERR_SHIFT 0

// Register: jpe_error_ris: JPEG error raw  interrupt status register (0x006c)
/* same defines as for jpe_error_imr
// Slice: vlc_table_err:
// "1" = illegal table detected
#define MRV_JPE_VLC_TABLE_ERR
#define MRV_JPE_VLC_TABLE_ERR_MASK 0x00000400
#define MRV_JPE_VLC_TABLE_ERR_SHIFT 10
// Slice: r2b_IMG_size_err:
// "1" = mismatch of predefined h_size and v_size values with calculated values (encode mode)
#define MRV_JPE_R2B_IMG_SIZE_ERR
#define MRV_JPE_R2B_IMG_SIZE_ERR_MASK 0x00000200
#define MRV_JPE_R2B_IMG_SIZE_ERR_SHIFT 9
// Slice: DCT_ERR:
// "1" =  block start mismatch
#define MRV_JPE_DCT_ERR
#define MRV_JPE_DCT_ERR_MASK 0x00000080
#define MRV_JPE_DCT_ERR_SHIFT 7
// Slice: vlc_symbol_err:
// "1" = illegal symbol detected (encoding)
#define MRV_JPE_VLC_SYMBOL_ERR
#define MRV_JPE_VLC_SYMBOL_ERR_MASK 0x00000010
#define MRV_JPE_VLC_SYMBOL_ERR_SHIFT 4
*/

// Register: jpe_error_mis: JPEG error masked interrupt status register (0x0070)
/* same defines as for jpe_error_imr
// Slice: vlc_table_err:
// "1" = illegal table detected
#define MRV_JPE_VLC_TABLE_ERR
#define MRV_JPE_VLC_TABLE_ERR_MASK 0x00000400
#define MRV_JPE_VLC_TABLE_ERR_SHIFT 10
// Slice: r2b_IMG_size_err:
// "1" = mismatch of predefined h_size and v_size values with calculated values (encode mode)
#define MRV_JPE_R2B_IMG_SIZE_ERR
#define MRV_JPE_R2B_IMG_SIZE_ERR_MASK 0x00000200
#define MRV_JPE_R2B_IMG_SIZE_ERR_SHIFT 9
// Slice: DCT_ERR:
// "1" =  block start mismatch
#define MRV_JPE_DCT_ERR
#define MRV_JPE_DCT_ERR_MASK 0x00000080
#define MRV_JPE_DCT_ERR_SHIFT 7
// Slice: vlc_symbol_err:
// "1" = illegal symbol detected (encoding)
#define MRV_JPE_VLC_SYMBOL_ERR
#define MRV_JPE_VLC_SYMBOL_ERR_MASK 0x00000010
#define MRV_JPE_VLC_SYMBOL_ERR_SHIFT 4
*/

// Register: jpe_error_icr: JPEG error interrupt set register (0x0074)
/* same defines as for jpe_error_imr
// Slice: vlc_table_err:
// "1" = set error bit, bit is reset to zero after 1 clk
#define MRV_JPE_VLC_TABLE_ERR
#define MRV_JPE_VLC_TABLE_ERR_MASK 0x00000400
#define MRV_JPE_VLC_TABLE_ERR_SHIFT 10
// Slice: r2b_IMG_size_err:
// "1" = set error bit, bit is reset to zero after 1 clk
#define MRV_JPE_R2B_IMG_SIZE_ERR
#define MRV_JPE_R2B_IMG_SIZE_ERR_MASK 0x00000200
#define MRV_JPE_R2B_IMG_SIZE_ERR_SHIFT 9
// Slice: DCT_ERR:
// "1" = set error bit, bit is reset to zero after 1 clk
#define MRV_JPE_DCT_ERR
#define MRV_JPE_DCT_ERR_MASK 0x00000080
#define MRV_JPE_DCT_ERR_SHIFT 7
// Slice: vlc_symbol_err:
// "1" = set error bit, bit is reset to zero after 1 clk
#define MRV_JPE_VLC_SYMBOL_ERR
#define MRV_JPE_VLC_SYMBOL_ERR_MASK 0x00000010
#define MRV_JPE_VLC_SYMBOL_ERR_SHIFT 4
*/

// Register: jpe_error_isr: JPEG error interrupt clear register (0x0078)
/* same defines as for jpe_error_imr
// Slice: vlc_table_err:
// "1" = clear status bit, bit is reset to zero after 1 clk
#define MRV_JPE_VLC_TABLE_ERR
#define MRV_JPE_VLC_TABLE_ERR_MASK 0x00000400
#define MRV_JPE_VLC_TABLE_ERR_SHIFT 10
// Slice: r2b_IMG_size_err:
// "1" = clear status bit, bit is reset to zero after 1 clk
#define MRV_JPE_R2B_IMG_SIZE_ERR
#define MRV_JPE_R2B_IMG_SIZE_ERR_MASK 0x00000200
#define MRV_JPE_R2B_IMG_SIZE_ERR_SHIFT 9
// Slice: dct_err:
// "1" = clear status bit, bit is reset to zero after 1 clk
#define MRV_JPE_DCT_ERR
#define MRV_JPE_DCT_ERR_MASK 0x00000080
#define MRV_JPE_DCT_ERR_SHIFT 7
// Slice: vlc_symbol_err:
// "1" = clear status bit, bit is reset to zero after 1 clk
#define MRV_JPE_VLC_SYMBOL_ERR
#define MRV_JPE_VLC_SYMBOL_ERR_MASK 0x00000010
#define MRV_JPE_VLC_SYMBOL_ERR_SHIFT 4
*/

//#define MRV_JPGINT_GEN_HEADER_DONE  0x00000020 //[5] header generation done interrupt
//#define MRV_JPGINT_ENCODE_DONE      0x00000010 //[4] JPEG encode done interrupt

// Register: jpe_status_imr: JPEG status interrupt mask register (0x007c)
// Slice: gen_header_done:
// "1" = interrupt is activated (masked in)
#define MRV_JPE_GEN_HEADER_DONE
#define MRV_JPE_GEN_HEADER_DONE_MASK 0x00000020
#define MRV_JPE_GEN_HEADER_DONE_SHIFT 5
// Slice: encode_done:
// "1" = interrupt is activated (masked in)
#define MRV_JPE_ENCODE_DONE
#define MRV_JPE_ENCODE_DONE_MASK 0x00000010
#define MRV_JPE_ENCODE_DONE_SHIFT 4

// combination of all JPG status lines
#define MRV_JPE_ALL_STAT
#define MRV_JPE_ALL_STAT_MASK \
  (   0 \
    | MRV_JPE_GEN_HEADER_DONE_MASK \
    | MRV_JPE_ENCODE_DONE_MASK \
  )
#define MRV_JPE_ALL_STAT_SHIFT 0

// Register: jpe_status_ris: JPEG status raw interrupt status register (0x0080)
/* same defines as for jpe_status_imr
// Slice: gen_header_done:
// "1" = Stream header generation finished
#define MRV_JPE_GEN_HEADER_DONE
#define MRV_JPE_GEN_HEADER_DONE_MASK 0x00000020
#define MRV_JPE_GEN_HEADER_DONE_SHIFT 5
// Slice: encode_done:
// "1" = Encode processing finished
#define MRV_JPE_ENCODE_DONE
#define MRV_JPE_ENCODE_DONE_MASK 0x00000010
#define MRV_JPE_ENCODE_DONE_SHIFT 4
*/

// Register: jpe_status_mis: JPEG status masked interrupt status register (0x0084)
/* same defines as for jpe_status_imr
// Slice: gen_header_done:
// "1" = Stream header generation finished
#define MRV_JPE_GEN_HEADER_DONE
#define MRV_JPE_GEN_HEADER_DONE_MASK 0x00000020
#define MRV_JPE_GEN_HEADER_DONE_SHIFT 5
// Slice: encode_done:
// "1" = Encode processing finished
#define MRV_JPE_ENCODE_DONE
#define MRV_JPE_ENCODE_DONE_MASK 0x00000010
#define MRV_JPE_ENCODE_DONE_SHIFT 4
*/

// Register: jpe_status_icr: JPEG status interrupt clear register (0x0088)
/* same defines as for jpe_status_imr
// Slice: gen_header_done:
// "1" = clear status bit, bit is reset to zero after 1 clk
#define MRV_JPE_GEN_HEADER_DONE
#define MRV_JPE_GEN_HEADER_DONE_MASK 0x00000020
#define MRV_JPE_GEN_HEADER_DONE_SHIFT 5
// Slice: encode_done:
// "1" = clear status bit, bit is reset to zero after 1 clk
#define MRV_JPE_ENCODE_DONE
#define MRV_JPE_ENCODE_DONE_MASK 0x00000010
#define MRV_JPE_ENCODE_DONE_SHIFT 4
*/

// Register: jpe_status_isr: JPEG status interrupt set register (0x008c)
/* same defines as for jpe_status_imr
// Slice: gen_header_done:
// "1" = set error bit, bit is reset to zero after 1 clk
#define MRV_JPE_GEN_HEADER_DONE
#define MRV_JPE_GEN_HEADER_DONE_MASK 0x00000020
#define MRV_JPE_GEN_HEADER_DONE_SHIFT 5
// Slice: encode_done:
// "1" = set error bit, bit is reset to zero after 1 clk
#define MRV_JPE_ENCODE_DONE
#define MRV_JPE_ENCODE_DONE_MASK 0x00000010
#define MRV_JPE_ENCODE_DONE_SHIFT 4
*/

// Register: jpe_config: enables or disables the Speedview (Scalado) feature (0x0090)
#if ((MARVIN_FEATURE_JPE >= MARVIN_FEATURE_JPE_V3) && \
     (MARVIN_FEATURE_JPE <= MARVIN_FEATURE_JPE_V3) && \
     (MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V2))
    // Slice: speedview_en
    // enables or disables the Speedview (Scalado) feature, only available for M5_v52
    // "1": SpeedView JPEG Stream Encoding enabled
    // "0": JPEG Stream Encoding according to JPEG standard
    #define MRV_JPE_SPEEDVIEW
    #define MRV_JPE_SPEEDVIEW_MASK 0x00000010
    #define MRV_JPE_SPEEDVIEW_SHIFT 4
    #define MRV_JPE_SPEEDVIEW_DIS   0  // "0": JPEG Stream Encoding according to JPEG standard
    #define MRV_JPE_SPEEDVIEW_ENA   1  // "1": SpeedView JPEG Stream Encoding enabled


    // Slice: cont_mode:
    // Encoder continous mode
    // "00": encoder stops at frame end (corresponds to former behavior)
    // "01": encoder starts automatically to encode the next frame
    // "10": unused
    // "11": encoder first generates next header and then encodes automatically the next frame
    // These settings are checked after encoding one frame. They are not auto-reset by hardware.
    #define MRV_JPE_CONT_MODE
    #define MRV_JPE_CONT_MODE_MASK 0x00000003
    #define MRV_JPE_CONT_MODE_SHIFT 0
    #define MRV_JPE_CONT_MODE_STOP   0 // "00": encoder stops at frame end (corresponds to former behavior)
    #define MRV_JPE_CONT_MODE_NEXT   1 // "01": encoder starts automatically to encode the next frame
    #define MRV_JPE_CONT_MODE_HEADER 3 // "11": encoder first generates next header and then encodes automatically the next frame
#endif // MARVIN_FEATURE_JPE

/********************************************************************/
/* SMIA Module Programming Registers                                */
/********************************************************************/
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || \
     (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP)      || \
     (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))

    // Register: smia_ctrl: global control register (0x0000)
  #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)
    // Slice: DMA_CHANNEL_SEL:
    // DMA channel selector for image data output
    #define MRV_SMIA_DMA_CHANNEL_SEL
    #define MRV_SMIA_DMA_CHANNEL_SEL_MASK 0x00000700
    #define MRV_SMIA_DMA_CHANNEL_SEL_SHIFT 8
    // Slice: SHUTDOWN_LANE:
    // Shutdown Lane Module. Content of this register is directly connected to the output signal shutdown
    #define MRV_SMIA_SHUTDOWN_LANE
    #define MRV_SMIA_SHUTDOWN_LANE_MASK 0x00000008
    #define MRV_SMIA_SHUTDOWN_LANE_SHIFT 3
  #endif // MARVIN_FEATURE_SMIA_COMPLETE
// slice removed
//    // Slice: CFG_UPD_ENA:
//    // enables generation of cfg_upd signal at frame end
//    #define MRV_SMIA_CFG_UPD_ENA
//    #define MRV_SMIA_CFG_UPD_ENA_MASK 0x00000004
//    #define MRV_SMIA_CFG_UPD_ENA_SHIFT 2
    // Slice: FLUSH_FIFO:
    // writing '1' resets the write- and read pointers of the embedded data fifo.
    #define MRV_SMIA_FLUSH_FIFO
    #define MRV_SMIA_FLUSH_FIFO_MASK 0x00000002
    #define MRV_SMIA_FLUSH_FIFO_SHIFT 1
    // Slice: OUTPUT_ENA:
    // 1: transmission to alomics output interface is enabled
    // 0: transmission is disabled
    #define MRV_SMIA_OUTPUT_ENA
    #define MRV_SMIA_OUTPUT_ENA_MASK 0x00000001
    #define MRV_SMIA_OUTPUT_ENA_SHIFT 0

    // Register: smia_status: global status register (0x0004)
  #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)
    // Slice: DMA_CHANNEL:
    // DMA channel of currently received packet
    #define MRV_SMIA_DMA_CHANNEL
    #define MRV_SMIA_DMA_CHANNEL_MASK 0x00000700
    #define MRV_SMIA_DMA_CHANNEL_SHIFT 8
  #endif // MARVIN_FEATURE_SMIA_COMPLETE
    // Slice: EMB_DATA_AVAIL:
    // 1: embedded data fifo not empty
    #define MRV_SMIA_EMB_DATA_AVAIL
    #define MRV_SMIA_EMB_DATA_AVAIL_MASK 0x00000001
    #define MRV_SMIA_EMB_DATA_AVAIL_SHIFT 0

    // Register: smia_imsc: Interrupt mask (0x0008)
  #if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || \
       (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    // Slice: IMSC_FIFO_FILL_LEVEL:
    // enable interrupt (1) or mask out (0)
    #define MRV_SMIA_IMSC_FIFO_FILL_LEVEL
    #define MRV_SMIA_IMSC_FIFO_FILL_LEVEL_MASK 0x00000020
    #define MRV_SMIA_IMSC_FIFO_FILL_LEVEL_SHIFT 5
  #else // MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_MIPI_EMP
    #define MRV_SMIA_IMSC_FIFO_FILL_LEVEL_MASK 0
  #endif // MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_MIPI_EMP
  #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)
    // Slice: IMSC_SYNC_FIFO_OVFLW:
    // enable interrupt (1) or mask out (0)
    #define MRV_SMIA_IMSC_SYNC_FIFO_OVFLW
    #define MRV_SMIA_IMSC_SYNC_FIFO_OVFLW_MASK 0x00000010
    #define MRV_SMIA_IMSC_SYNC_FIFO_OVFLW_SHIFT 4
    // Slice: IMSC_ERR_CS:
    // enable interrupt (1) or mask out (0)
    #define MRV_SMIA_IMSC_ERR_CS
    #define MRV_SMIA_IMSC_ERR_CS_MASK 0x00000008
    #define MRV_SMIA_IMSC_ERR_CS_SHIFT 3
    // Slice: IMSC_ERR_PROTOCOL:
    // enable interrupt (1) or mask out (0)
    #define MRV_SMIA_IMSC_ERR_PROTOCOL
    #define MRV_SMIA_IMSC_ERR_PROTOCOL_MASK 0x00000004
    #define MRV_SMIA_IMSC_ERR_PROTOCOL_SHIFT 2
  #else // MARVIN_FEATURE_SMIA_COMPLETE
    #define MRV_SMIA_IMSC_SYNC_FIFO_OVFLW_MASK 0
    #define MRV_SMIA_IMSC_ERR_CS_MASK 0
    #define MRV_SMIA_IMSC_ERR_PROTOCOL_MASK 0
  #endif // MARVIN_FEATURE_SMIA_COMPLETE
    // Slice: IMSC_EMB_DATA_OVFLW:
    // enable interrupt (1) or mask out (0)
    #define MRV_SMIA_IMSC_EMB_DATA_OVFLW
    #define MRV_SMIA_IMSC_EMB_DATA_OVFLW_MASK 0x00000002
    #define MRV_SMIA_IMSC_EMB_DATA_OVFLW_SHIFT 1
    // Slice: IMSC_FRAME_END:
    // enable interrupt (1) or mask out (0)
    #define MRV_SMIA_IMSC_FRAME_END
    #define MRV_SMIA_IMSC_FRAME_END_MASK 0x00000001
    #define MRV_SMIA_IMSC_FRAME_END_SHIFT 0
    // combination of all interrupt lines
    #define MRV_SMIA_IMSC_ALL_IRQS
    #define MRV_SMIA_IMSC_ALL_IRQS_MASK \
      (   0 \
        | MRV_SMIA_IMSC_FIFO_FILL_LEVEL_MASK \
        | MRV_SMIA_IMSC_SYNC_FIFO_OVFLW_MASK \
        | MRV_SMIA_IMSC_ERR_CS_MASK \
        | MRV_SMIA_IMSC_ERR_PROTOCOL_MASK \
        | MRV_SMIA_IMSC_EMB_DATA_OVFLW_MASK \
        | MRV_SMIA_IMSC_FRAME_END_MASK \
      )
    #define MRV_SMIA_IMSC_ALL_IRQS_SHIFT 0

    // Register: smia_ris: Raw interrupt status (0x000c)
    // Slice: RIS_FIFO_FILL_LEVEL:
    // Programmed fill level was reached; will be raised as long as the fill level is greater or equal the programmed value
    #define MRV_SMIA_RIS_FIFO_FILL_LEVEL
    #define MRV_SMIA_RIS_FIFO_FILL_LEVEL_MASK 0x00000020
    #define MRV_SMIA_RIS_FIFO_FILL_LEVEL_SHIFT 5
  #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)
    // Slice: RIS_SYNC_FIFO_OVFLW:
    // Sync fifo overflow error
    #define MRV_SMIA_RIS_SYNC_FIFO_OVFLW
    #define MRV_SMIA_RIS_SYNC_FIFO_OVFLW_MASK 0x00000010
    #define MRV_SMIA_RIS_SYNC_FIFO_OVFLW_SHIFT 4
    // Slice: RIS_ERR_CS:
    // Checksum error
    #define MRV_SMIA_RIS_ERR_CS
    #define MRV_SMIA_RIS_ERR_CS_MASK 0x00000008
    #define MRV_SMIA_RIS_ERR_CS_SHIFT 3
    // Slice: RIS_ERR_PROTOCOL:
    // Protocol error
    #define MRV_SMIA_RIS_ERR_PROTOCOL
    #define MRV_SMIA_RIS_ERR_PROTOCOL_MASK 0x00000004
    #define MRV_SMIA_RIS_ERR_PROTOCOL_SHIFT 2
  #else // MARVIN_FEATURE_SMIA_COMPLETE
    #define MRV_SMIA_RIS_SYNC_FIFO_OVFLW_MASK 0
    #define MRV_SMIA_RIS_ERR_CS_MASK 0
    #define MRV_SMIA_RIS_ERR_PROTOCOL_MASK 0
  #endif // MARVIN_FEATURE_SMIA_COMPLETE
    // Slice: RIS_EMB_DATA_OVFLW:
    // Embedded data fifo overflow error
    #define MRV_SMIA_RIS_EMB_DATA_OVFLW
    #define MRV_SMIA_RIS_EMB_DATA_OVFLW_MASK 0x00000002
    #define MRV_SMIA_RIS_EMB_DATA_OVFLW_SHIFT 1
    // Slice: RIS_FRAME_END:
    // Frame end reached
    #define MRV_SMIA_RIS_FRAME_END
    #define MRV_SMIA_RIS_FRAME_END_MASK 0x00000001
    #define MRV_SMIA_RIS_FRAME_END_SHIFT 0
    // combination of all interrupt lines
    #define MRV_SMIA_RIS_ALL_IRQS
    #define MRV_SMIA_RIS_ALL_IRQS_MASK \
      (   0 \
        | MRV_SMIA_RIS_FIFO_FILL_LEVEL_MASK \
        | MRV_SMIA_RIS_SYNC_FIFO_OVFLW_MASK \
        | MRV_SMIA_RIS_ERR_CS_MASK \
        | MRV_SMIA_RIS_ERR_PROTOCOL_MASK \
        | MRV_SMIA_RIS_EMB_DATA_OVFLW_MASK \
        | MRV_SMIA_RIS_FRAME_END_MASK \
      )
    #define MRV_SMIA_RIS_ALL_IRQS_SHIFT 0

    // Register: smia_mis: Masked interrupt status (0x0010)
    // Slice: MIS_FIFO_FILL_LEVEL:
    // Programmed fill level was reached; will be raised as long as the fill level is greater or equal the programmed value
    #define MRV_SMIA_MIS_FIFO_FILL_LEVEL
    #define MRV_SMIA_MIS_FIFO_FILL_LEVEL_MASK 0x00000020
    #define MRV_SMIA_MIS_FIFO_FILL_LEVEL_SHIFT 5
  #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)
    // Slice: MIS_SYNC_FIFO_OVFLW:
    // Sync fifo overflow error
    #define MRV_SMIA_MIS_SYNC_FIFO_OVFLW
    #define MRV_SMIA_MIS_SYNC_FIFO_OVFLW_MASK 0x00000010
    #define MRV_SMIA_MIS_SYNC_FIFO_OVFLW_SHIFT 4
    // Slice: MIS_ERR_CS:
    // Checksum error
    #define MRV_SMIA_MIS_ERR_CS
    #define MRV_SMIA_MIS_ERR_CS_MASK 0x00000008
    #define MRV_SMIA_MIS_ERR_CS_SHIFT 3
    // Slice: MIS_ERR_PROTOCOL:
    // Protocol error
    #define MRV_SMIA_MIS_ERR_PROTOCOL
    #define MRV_SMIA_MIS_ERR_PROTOCOL_MASK 0x00000004
    #define MRV_SMIA_MIS_ERR_PROTOCOL_SHIFT 2
  #else // MARVIN_FEATURE_SMIA_COMPLETE
    #define MRV_SMIA_MIS_SYNC_FIFO_OVFLW_MASK 0
    #define MRV_SMIA_MIS_ERR_CS_MASK 0
    #define MRV_SMIA_MIS_ERR_PROTOCOL_MASK 0
  #endif // MARVIN_FEATURE_SMIA_COMPLETE
    // Slice: MIS_EMB_DATA_OVFLW:
    // Embedded data fifo overflow error
    #define MRV_SMIA_MIS_EMB_DATA_OVFLW
    #define MRV_SMIA_MIS_EMB_DATA_OVFLW_MASK 0x00000002
    #define MRV_SMIA_MIS_EMB_DATA_OVFLW_SHIFT 1
    // Slice: MIS_FRAME_END:
    // Frame end reached
    #define MRV_SMIA_MIS_FRAME_END
    #define MRV_SMIA_MIS_FRAME_END_MASK 0x00000001
    #define MRV_SMIA_MIS_FRAME_END_SHIFT 0
    // combination of all interrupt lines
    #define MRV_SMIA_MIS_ALL_IRQS
    #define MRV_SMIA_MIS_ALL_IRQS_MASK \
      (   0 \
        | MRV_SMIA_MIS_FIFO_FILL_LEVEL_MASK \
        | MRV_SMIA_MIS_SYNC_FIFO_OVFLW_MASK \
        | MRV_SMIA_MIS_ERR_CS_MASK \
        | MRV_SMIA_MIS_ERR_PROTOCOL_MASK \
        | MRV_SMIA_MIS_EMB_DATA_OVFLW_MASK \
        | MRV_SMIA_MIS_FRAME_END_MASK \
      )
    #define MRV_SMIA_MIS_ALL_IRQS_SHIFT 0

    // Register: smia_icr: Interrupt clear register (0x0014)
    // Slice: ICR_FIFO_FILL_LEVEL:
    // Write '1': clear interrupt; Write '0': no effect
    #define MRV_SMIA_ICR_FIFO_FILL_LEVEL
    #define MRV_SMIA_ICR_FIFO_FILL_LEVEL_MASK 0x00000020
    #define MRV_SMIA_ICR_FIFO_FILL_LEVEL_SHIFT 5
  #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)
    // Slice: ICR_SYNC_FIFO_OVFLW:
    // Write '1': clear interrupt; Write '0': no effect
    #define MRV_SMIA_ICR_SYNC_FIFO_OVFLW
    #define MRV_SMIA_ICR_SYNC_FIFO_OVFLW_MASK 0x00000010
    #define MRV_SMIA_ICR_SYNC_FIFO_OVFLW_SHIFT 4
    // Slice: ICR_ERR_CS:
    // Write '1': clear interrupt; Write '0': no effect
    #define MRV_SMIA_ICR_ERR_CS
    #define MRV_SMIA_ICR_ERR_CS_MASK 0x00000008
    #define MRV_SMIA_ICR_ERR_CS_SHIFT 3
    // Slice: ICR_ERR_PROTOCOL:
    // Write '1': clear interrupt; Write '0': no effect
    #define MRV_SMIA_ICR_ERR_PROTOCOL
    #define MRV_SMIA_ICR_ERR_PROTOCOL_MASK 0x00000004
    #define MRV_SMIA_ICR_ERR_PROTOCOL_SHIFT 2
  #else // MARVIN_FEATURE_SMIA_COMPLETE
    #define MRV_SMIA_ICR_SYNC_FIFO_OVFLW_MASK 0
    #define MRV_SMIA_ICR_ERR_CS_MASK 0
    #define MRV_SMIA_ICR_ERR_PROTOCOL_MASK 0
  #endif // MARVIN_FEATURE_SMIA_COMPLETE
    // Slice: ICR_EMB_DATA_OVFLW:
    // Write '1': clear interrupt; Write '0': no effect
    #define MRV_SMIA_ICR_EMB_DATA_OVFLW
    #define MRV_SMIA_ICR_EMB_DATA_OVFLW_MASK 0x00000002
    #define MRV_SMIA_ICR_EMB_DATA_OVFLW_SHIFT 1
    // Slice: ICR_FRAME_END:
    // Write '1': clear interrupt; Write '0': no effect
    #define MRV_SMIA_ICR_FRAME_END
    #define MRV_SMIA_ICR_FRAME_END_MASK 0x00000001
    #define MRV_SMIA_ICR_FRAME_END_SHIFT 0
    // combination of all interrupt lines
    #define MRV_SMIA_ICR_ALL_IRQS
    #define MRV_SMIA_ICR_ALL_IRQS_MASK \
      (   0 \
        | MRV_SMIA_ICR_FIFO_FILL_LEVEL_MASK \
        | MRV_SMIA_ICR_SYNC_FIFO_OVFLW_MASK \
        | MRV_SMIA_ICR_ERR_CS_MASK \
        | MRV_SMIA_ICR_ERR_PROTOCOL_MASK \
        | MRV_SMIA_ICR_EMB_DATA_OVFLW_MASK \
        | MRV_SMIA_ICR_FRAME_END_MASK \
      )
    #define MRV_SMIA_ICR_ALL_IRQS_SHIFT 0

    // Register: smia_isr: Interrupt set register (0x0018)
    // Slice: ISR_FIFO_FILL_LEVEL:
    // Write '1': set interrupt; Write '0': no effect
    #define MRV_SMIA_ISR_FIFO_FILL_LEVEL
    #define MRV_SMIA_ISR_FIFO_FILL_LEVEL_MASK 0x00000020
    #define MRV_SMIA_ISR_FIFO_FILL_LEVEL_SHIFT 5
  #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)
    // Slice: ISR_SYNC_FIFO_OVFLW:
    // Write '1': set interrupt; Write '0': no effect
    #define MRV_SMIA_ISR_SYNC_FIFO_OVFLW
    #define MRV_SMIA_ISR_SYNC_FIFO_OVFLW_MASK 0x00000010
    #define MRV_SMIA_ISR_SYNC_FIFO_OVFLW_SHIFT 4
    // Slice: ISR_ERR_CS:
    // Write '1': set interrupt; Write '0': no effect
    #define MRV_SMIA_ISR_ERR_CS
    #define MRV_SMIA_ISR_ERR_CS_MASK 0x00000008
    #define MRV_SMIA_ISR_ERR_CS_SHIFT 3
    // Slice: ISR_ERR_PROTOCOL:
    // Write '1': set interrupt; Write '0': no effect
    #define MRV_SMIA_ISR_ERR_PROTOCOL
    #define MRV_SMIA_ISR_ERR_PROTOCOL_MASK 0x00000004
    #define MRV_SMIA_ISR_ERR_PROTOCOL_SHIFT 2
  #else // MARVIN_FEATURE_SMIA_COMPLETE
    #define MRV_SMIA_ISR_SYNC_FIFO_OVFLW_MASK 0
    #define MRV_SMIA_ISR_ERR_CS_MASK 0
    #define MRV_SMIA_ISR_ERR_PROTOCOL_MASK 0
  #endif // MARVIN_FEATURE_SMIA_COMPLETE
    // Slice: ISR_EMB_DATA_OVFLW:
    // Write '1': set interrupt; Write '0': no effect
    #define MRV_SMIA_ISR_EMB_DATA_OVFLW
    #define MRV_SMIA_ISR_EMB_DATA_OVFLW_MASK 0x00000002
    #define MRV_SMIA_ISR_EMB_DATA_OVFLW_SHIFT 1
    // Slice: ISR_FRAME_END:
    // Write '1': set interrupt; Write '0': no effect
    #define MRV_SMIA_ISR_FRAME_END
    #define MRV_SMIA_ISR_FRAME_END_MASK 0x00000001
    #define MRV_SMIA_ISR_FRAME_END_SHIFT 0
    // combination of all interrupt lines
    #define MRV_SMIA_ISR_ALL_IRQS
    #define MRV_SMIA_ISR_ALL_IRQS_MASK \
      (   0 \
        | MRV_SMIA_ISR_FIFO_FILL_LEVEL_MASK \
        | MRV_SMIA_ISR_SYNC_FIFO_OVFLW_MASK \
        | MRV_SMIA_ISR_ERR_CS_MASK \
        | MRV_SMIA_ISR_ERR_PROTOCOL_MASK \
        | MRV_SMIA_ISR_EMB_DATA_OVFLW_MASK \
        | MRV_SMIA_ISR_FRAME_END_MASK \
      )
    #define MRV_SMIA_ISR_ALL_IRQS_SHIFT 0

    // Register: smia_data_format_sel: data format selector register (0x001c)
    // Slice: DATA_FORMAT_SEL:
    // data format selector:
    // 0x0: YUV 422
    // 0x1: YUV 420
    // 0x4: RGB 444
    // 0x5: RGB 565
    // 0x6: RGB 888
    // 0x8: RAW 6
    // 0x9: RAW 7
    // 0xA: RAW 8
    // 0xB: RAW 10
    // 0xC: RAW12
    // 0xD: RAW 8-bit to 10-bit decompression
    // 0xF: compressed
    // 0x2, 0x3, 0x7, 0xE: reserved, no output
    #define MRV_SMIA_DATA_FORMAT_SEL
    #define MRV_SMIA_DATA_FORMAT_SEL_MASK 0x0000000F
    #define MRV_SMIA_DATA_FORMAT_SEL_SHIFT 0
  #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)
    #define MRV_SMIA_DATA_FORMAT_SEL_YUV422      0 // 0x0: YUV 422
    #define MRV_SMIA_DATA_FORMAT_SEL_YUV420      1 // 0x1: YUV 420
    #define MRV_SMIA_DATA_FORMAT_SEL_RGB444      4 // 0x4: RGB 444
    #define MRV_SMIA_DATA_FORMAT_SEL_RGB565      5 // 0x5: RGB 565
    #define MRV_SMIA_DATA_FORMAT_SEL_RGB888      6 // 0x6: RGB 888
    #define MRV_SMIA_DATA_FORMAT_SEL_RAW6        8 // 0x8: RAW 6
    #define MRV_SMIA_DATA_FORMAT_SEL_RAW7        9 // 0x9: RAW 7
  #endif // MARVIN_FEATURE_SMIA_COMPLETE
    #define MRV_SMIA_DATA_FORMAT_SEL_RAW8       10 // 0xA: RAW 8 / 8-bit data bypass
    #define MRV_SMIA_DATA_FORMAT_SEL_RAW10      11 // 0xB: RAW 10
    #define MRV_SMIA_DATA_FORMAT_SEL_RAW12      12 // 0xC: RAW12
    #define MRV_SMIA_DATA_FORMAT_SEL_RAW8TO10   13 // 0xD: RAW 8-bit to 10-bit decompression
  #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)
    #define MRV_SMIA_DATA_FORMAT_SEL_COMPRESSED 15 // 0xF: compressed
  #endif // MARVIN_FEATURE_SMIA_COMPLETE

    // Register: smia_sof_emb_data_lines: start of frame embedded data lines register (0x0020)
    // Slice: SOF_EMB_DATA_LINES:
    // number of embedded data lines at frame start
    #define MRV_SMIA_SOF_EMB_DATA_LINES
    #define MRV_SMIA_SOF_EMB_DATA_LINES_MASK 0x00000007
    #define MRV_SMIA_SOF_EMB_DATA_LINES_SHIFT 0
    #define MRV_SMIA_SOF_EMB_DATA_LINES_MIN  0
    #define MRV_SMIA_SOF_EMB_DATA_LINES_MAX (MRV_SMIA_SOF_EMB_DATA_LINES_MASK >> MRV_SMIA_SOF_EMB_DATA_LINES_SHIFT)

  #if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || \
       (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP))
    // Register: smia_emb_hstart: embedded data hstart register (0x0024)
    // Slice: EMB_HSTART:
    // horizontal start position of captured embedded data.
    // Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
    #define MRV_SMIA_EMB_HSTART
    #define MRV_SMIA_EMB_HSTART_MASK 0x00003FFF
    #define MRV_SMIA_EMB_HSTART_SHIFT 0
    #define MRV_SMIA_EMB_HSTART_VALID_MASK (MRV_SMIA_EMB_HSTART_MASK & ~0x00000003)

    // Register: smia_emb_hsize: embedded data hsize register (0x0028)
    // Slice: EMB_HSIZE:
    // number of captured embedded data dwords per line
    // '0' means no capturing of embedded data.
    // Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
    #define MRV_SMIA_EMB_HSIZE
    #define MRV_SMIA_EMB_HSIZE_MASK 0x00003FFF
    #define MRV_SMIA_EMB_HSIZE_SHIFT 0
    #define MRV_SMIA_EMB_HSIZE_VALID_MASK (MRV_SMIA_EMB_HSIZE_MASK & ~0x00000003)

    // Register: smia_emb_vstart: embedded data vstart register (0x002c)
    // Slice: EMB_VSTART:
    // start line of embedded data extraction.
    // '0' means no capturing of embedded data
    #define MRV_SMIA_EMB_VSTART
    #define MRV_SMIA_EMB_VSTART_MASK 0x00000FFF
    #define MRV_SMIA_EMB_VSTART_SHIFT 0
  #endif // #if ((MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP))

    // Register: smia_num_lines: image data lines register (0x0030)
    // Slice: NUM_LINES:
    // number of image data lines per frame
    #define MRV_SMIA_NUM_LINES
  #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)
    #define MRV_SMIA_NUM_LINES_MASK 0x00000FFF
  #else // MARVIN_FEATURE_SMIA_COMPLETE
    #define MRV_SMIA_NUM_LINES_MASK 0x000007FF
  #endif // MARVIN_FEATURE_SMIA_COMPLETE
    #define MRV_SMIA_NUM_LINES_SHIFT 0
    #define MRV_SMIA_NUM_LINES_MIN  1
    #define MRV_SMIA_NUM_LINES_MAX (MRV_SMIA_NUM_LINES_MASK >> MRV_SMIA_NUM_LINES_SHIFT)

    // Register: smia_emb_data_fifo: Embedded Data Fifo (0x0034)
    // Slice: EMB_DATA_FIFO:
    // lowest 4 bytes in embedded data fifo;
    // reading increments fifo read pointer.
    // First embedded data byte will be written to bits 7:0 of 32-bit data word, second data byte written to 15:8 etc.
    #define MRV_SMIA_EMB_DATA_FIFO
    #define MRV_SMIA_EMB_DATA_FIFO_MASK 0xFFFFFFFF
    #define MRV_SMIA_EMB_DATA_FIFO_SHIFT 0

  #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)
    // Register: smia_fifo_fill_level: Embedded Data FIFO Fill Level (0x0038)
    // Slice: FIFO_FILL_LEVEL:
    // FIFO level in dwords for triggering the fill level interrupt.
    // Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
    #define MRV_SMIA_FIFO_FILL_LEVEL
    #define MRV_SMIA_FIFO_FILL_LEVEL_MASK 0x000003FF
    #define MRV_SMIA_FIFO_FILL_LEVEL_SHIFT 0
    #define MRV_SMIA_FIFO_FILL_LEVEL_VALID_MASK (MRV_SMIA_FIFO_FILL_LEVEL_MASK & ~0x00000003)
  #elif (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    // Register: smia_fifo_fill_level: Embedded Data FIFO Fill Level (0x0038)
    // Slice: FIFO_FILL_LEVEL:
    // number of dwords in embedded data fifo to generate a fifo fill level interrupt, fifo depth of 2048 dwords max. in this case; '0' generates irq as soon as fifo not empty. The MSB of the address is configurable. It depends on how much data have to be read into the embedded data RAM.
    // Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
    #define MRV_SMIA_FIFO_FILL_LEVEL
    #define MRV_SMIA_FIFO_FILL_LEVEL_MASK 0x00001FFF
    #define MRV_SMIA_FIFO_FILL_LEVEL_SHIFT 0
    #define MRV_SMIA_FIFO_FILL_LEVEL_VALID_MASK (MRV_SMIA_FIFO_FILL_LEVEL_MASK & ~0x00000003)
  #endif // #if ((MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_MIPI_EMP))

  #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    // Register: smia_emb_hstart1: embedded data horizontal start register (0x0040)
    // Slice: EMB_HSTART1:
    // horizontal start position of embedded data extraction area 1.
    // Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
    #define MRV_SMIA_EMB_HSTART1
    #define MRV_SMIA_EMB_HSTART1_MASK 0x00003FFF
    #define MRV_SMIA_EMB_HSTART1_SHIFT 0
    #define MRV_SMIA_EMB_HSTART1_VALID_MASK (MRV_SMIA_EMB_HSTART1_MASK & ~0x00000003)

    // Register: smia_emb_hsize1: embedded data horizontal size register (0x0044)
    // Slice: EMB_HSIZE1:
    // number of dwords per line in embedded data extraction area 1
    // '0' means no capturing of embedded data.
    // Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
    #define MRV_SMIA_EMB_HSIZE1
    #define MRV_SMIA_EMB_HSIZE1_MASK 0x00003FFF
    #define MRV_SMIA_EMB_HSIZE1_SHIFT 0
    #define MRV_SMIA_EMB_HSIZE1_VALID_MASK (MRV_SMIA_EMB_HSIZE1_MASK & ~0x00000003)

    // Register: smia_emb_vstart1: embedded data vertical start register (0x0048)
    // Slice: EMB_VSTART1:
    // start line of embedded data extraction area 1
    // '0' means no capturing of embedded data
    #define MRV_SMIA_EMB_VSTART1
    #define MRV_SMIA_EMB_VSTART1_MASK 0x00000FFF
    #define MRV_SMIA_EMB_VSTART1_SHIFT 0

    // Register: smia_emb_vsize1: embedded data vertical size register (0x004c)
    // Slice: EMB_VSIZE1:
    // number of lines of embedded data extraction area 1
    // '0' means no capturing of embedded data
    #define MRV_SMIA_EMB_VSIZE1
    #define MRV_SMIA_EMB_VSIZE1_MASK 0x00000FFF
    #define MRV_SMIA_EMB_VSIZE1_SHIFT 0

    // Register: smia_emb_hstart2: embedded data horizontal start register (0x0050)
    // Slice: EMB_HSTART2:
    // horizontal start position of embedded data extraction area 2.
    // Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
    #define MRV_SMIA_EMB_HSTART2
    #define MRV_SMIA_EMB_HSTART2_MASK 0x00003FFF
    #define MRV_SMIA_EMB_HSTART2_SHIFT 0
    #define MRV_SMIA_EMB_HSTART2_VALID_MASK (MRV_SMIA_EMB_HSTART2_MASK & ~0x00000003)

    // Register: smia_emb_hsize2: embedded data horizontal size register (0x0054)
    // Slice: EMB_HSIZE2:
    // number of dwords per line in embedded data extraction area 2
    // '0' means no capturing of embedded data.
    // Must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
    #define MRV_SMIA_EMB_HSIZE2
    #define MRV_SMIA_EMB_HSIZE2_MASK 0x00003FFF
    #define MRV_SMIA_EMB_HSIZE2_SHIFT 0
    #define MRV_SMIA_EMB_HSIZE2_VALID_MASK (MRV_SMIA_EMB_HSIZE2_MASK & ~0x00000003)

    // Register: smia_emb_vstart2: embedded data vertical start register (0x0058)
    // Slice: EMB_VSTART2:
    // start line of embedded data extraction area 2
    // '0' means no capturing of embedded data
    #define MRV_SMIA_EMB_VSTART2
    #define MRV_SMIA_EMB_VSTART2_MASK 0x00000FFF
    #define MRV_SMIA_EMB_VSTART2_SHIFT 0

    // Register: smia_emb_vsize2: embedded data vertical size register (0x005c)
    // Slice: EMB_VSIZE2:
    // number of lines of embedded data extraction area 2
    // '0' means no capturing of embedded data
    #define MRV_SMIA_EMB_VSIZE2
    #define MRV_SMIA_EMB_VSIZE2_MASK 0x00000FFF
    #define MRV_SMIA_EMB_VSIZE2_SHIFT 0
  #endif // #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
#endif  // #if ((MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP))

/********************************************************************/
/* MIPI Module Programming Registers                                */
/********************************************************************/
// Register: mipi_ctrl: global control register (0x0000)
// Slice: ERR_SOT_SYNC_HS_SKIP:
// 1: data within the current transmission is skipped if ErrSotSyncHS is detected (default)
// 0: ErrSotSyncHS does not affect transmission
#define MRV_MIPI_ERR_SOT_SYNC_HS_SKIP
#define MRV_MIPI_ERR_SOT_SYNC_HS_SKIP_MASK 0x00020000
#define MRV_MIPI_ERR_SOT_SYNC_HS_SKIP_SHIFT 17
// Slice: ERR_SOT_HS_SKIP:
// 1: data within the current transmission is skipped if ErrSotHS is detected
// 0: ErrSotHS does not affect transmission (default)
#define MRV_MIPI_ERR_SOT_HS_SKIP
#define MRV_MIPI_ERR_SOT_HS_SKIP_MASK 0x00010000
#define MRV_MIPI_ERR_SOT_HS_SKIP_SHIFT 16
// Slice: NUM_LANES:
// 00: Lane 1 is used; 01: Lanes 1 and 2 are used;
// 10: Lanes 1...3 are used; 11: Lanes 1...4 are used
#define MRV_MIPI_NUM_LANES
#define MRV_MIPI_NUM_LANES_MASK 0x00003000
#define MRV_MIPI_NUM_LANES_SHIFT 12
// Slice: SHUTDOWN_LANE:
// Shutdown Lane Module. Content of this register is directly connected to the output signal shutdown[n-1:0]
#define MRV_MIPI_SHUTDOWN_LANE
#define MRV_MIPI_SHUTDOWN_LANE_MASK 0x00000F00
#define MRV_MIPI_SHUTDOWN_LANE_SHIFT 8
// Slice: FLUSH_FIFO:
// writing '1' resets the write- and read pointers of the additional data fifo.
#define MRV_MIPI_FLUSH_FIFO
#define MRV_MIPI_FLUSH_FIFO_MASK 0x00000002
#define MRV_MIPI_FLUSH_FIFO_SHIFT 1
// Slice: OUTPUT_ENA:
// 1: output to add data fifo and to output interface is enabled
#define MRV_MIPI_OUTPUT_ENA
#define MRV_MIPI_OUTPUT_ENA_MASK 0x00000001
#define MRV_MIPI_OUTPUT_ENA_SHIFT 0

// Register: mipi_status: global status register (0x0004)
// Slice: STOPSTATE:
// Lane is in stopstate. This register is directly connected to the synchronized input signal stopstate[n-1:0]
#define MRV_MIPI_STOPSTATE
#define MRV_MIPI_STOPSTATE_MASK 0x00000F00
#define MRV_MIPI_STOPSTATE_SHIFT 8
// Slice: ADD_DATA_AVAIL:
// 1: additional data fifo not empty
#define MRV_MIPI_ADD_DATA_AVAIL
#define MRV_MIPI_ADD_DATA_AVAIL_MASK 0x00000001
#define MRV_MIPI_ADD_DATA_AVAIL_SHIFT 0

// Register: mipi_imsc: Interrupt mask (0x0008)
// Slice: IMSC_ADD_DATA_FILL_LEVEL:
// enable interrupt (1) or mask out (0)
#define MRV_MIPI_IMSC_ADD_DATA_FILL_LEVEL
#define MRV_MIPI_IMSC_ADD_DATA_FILL_LEVEL_MASK 0x04000000
#define MRV_MIPI_IMSC_ADD_DATA_FILL_LEVEL_SHIFT 26
// Slice: IMSC_ADD_DATA_OVFLW:
// enable interrupt (1) or mask out (0)
#define MRV_MIPI_IMSC_ADD_DATA_OVFLW
#define MRV_MIPI_IMSC_ADD_DATA_OVFLW_MASK 0x02000000
#define MRV_MIPI_IMSC_ADD_DATA_OVFLW_SHIFT 25
// Slice: IMSC_FRAME_END:
// enable interrupt (1) or mask out (0)
#define MRV_MIPI_IMSC_FRAME_END
#define MRV_MIPI_IMSC_FRAME_END_MASK 0x01000000
#define MRV_MIPI_IMSC_FRAME_END_SHIFT 24
// Slice: IMSC_ERR_CS:
// enable interrupt (1) or mask out (0)
#define MRV_MIPI_IMSC_ERR_CS
#define MRV_MIPI_IMSC_ERR_CS_MASK 0x00800000
#define MRV_MIPI_IMSC_ERR_CS_SHIFT 23
// Slice: IMSC_ERR_ECC1:
// enable interrupt (1) or mask out (0)
#define MRV_MIPI_IMSC_ERR_ECC1
#define MRV_MIPI_IMSC_ERR_ECC1_MASK 0x00400000
#define MRV_MIPI_IMSC_ERR_ECC1_SHIFT 22
// Slice: IMSC_ERR_ECC2:
// enable interrupt (1) or mask out (0)
#define MRV_MIPI_IMSC_ERR_ECC2
#define MRV_MIPI_IMSC_ERR_ECC2_MASK 0x00200000
#define MRV_MIPI_IMSC_ERR_ECC2_SHIFT 21
// Slice: IMSC_ERR_PROTOCOL:
// enable interrupt (1) or mask out (0)
#define MRV_MIPI_IMSC_ERR_PROTOCOL
#define MRV_MIPI_IMSC_ERR_PROTOCOL_MASK 0x00100000
#define MRV_MIPI_IMSC_ERR_PROTOCOL_SHIFT 20
// Slice: IMSC_ERR_CONTROL:
// enable interrupt (1) or mask out (0)
#define MRV_MIPI_IMSC_ERR_CONTROL
#define MRV_MIPI_IMSC_ERR_CONTROL_MASK 0x000F0000
#define MRV_MIPI_IMSC_ERR_CONTROL_SHIFT 16
// Slice: IMSC_ERR_EOT_SYNC:
// enable interrupt (1) or mask out (0)
#define MRV_MIPI_IMSC_ERR_EOT_SYNC
#define MRV_MIPI_IMSC_ERR_EOT_SYNC_MASK 0x0000F000
#define MRV_MIPI_IMSC_ERR_EOT_SYNC_SHIFT 12
// Slice: IMSC_ERR_SOT_SYNC:
// enable interrupt (1) or mask out (0)
#define MRV_MIPI_IMSC_ERR_SOT_SYNC
#define MRV_MIPI_IMSC_ERR_SOT_SYNC_MASK 0x00000F00
#define MRV_MIPI_IMSC_ERR_SOT_SYNC_SHIFT 8
// Slice: IMSC_ERR_SOT:
// enable interrupt (1) or mask out (0)
#define MRV_MIPI_IMSC_ERR_SOT
#define MRV_MIPI_IMSC_ERR_SOT_MASK 0x000000F0
#define MRV_MIPI_IMSC_ERR_SOT_SHIFT 4
// Slice: IMSC_SYNC_FIFO_OVFLW:
// enable interrupt (1) or mask out (0)
#define MRV_MIPI_IMSC_SYNC_FIFO_OVFLW
#define MRV_MIPI_IMSC_SYNC_FIFO_OVFLW_MASK 0x0000000F
#define MRV_MIPI_IMSC_SYNC_FIFO_OVFLW_SHIFT 0
// combination of all interrupt lines
#define MRV_MIPI_IMSC_ALL_IRQS
#define MRV_MIPI_IMSC_ALL_IRQS_MASK \
  (   0 \
    | MRV_MIPI_IMSC_ADD_DATA_FILL_LEVEL_MASK \
    | MRV_MIPI_IMSC_ADD_DATA_OVFLW_MASK \
    | MRV_MIPI_IMSC_FRAME_END_MASK \
    | MRV_MIPI_IMSC_ERR_CS_MASK \
    | MRV_MIPI_IMSC_ERR_ECC1_MASK \
    | MRV_MIPI_IMSC_ERR_ECC2_MASK \
    | MRV_MIPI_IMSC_ERR_PROTOCOL_MASK \
    | MRV_MIPI_IMSC_ERR_CONTROL_MASK \
    | MRV_MIPI_IMSC_ERR_EOT_SYNC_MASK \
    | MRV_MIPI_IMSC_ERR_SOT_SYNC_MASK \
    | MRV_MIPI_IMSC_ERR_SOT_MASK \
    | MRV_MIPI_IMSC_SYNC_FIFO_OVFLW_MASK \
  )
#define MRV_MIPI_IMSC_ALL_IRQS_SHIFT 0

// Register: mipi_ris: Raw interrupt status (0x000c)
// Slice: RIS_ADD_DATA_FILL_LEVEL:
// Programmed fill level was reached; will be raised as long as the fill level is greater or equal the programmed value
#define MRV_MIPI_RIS_ADD_DATA_FILL_LEVEL
#define MRV_MIPI_RIS_ADD_DATA_FILL_LEVEL_MASK 0x04000000
#define MRV_MIPI_RIS_ADD_DATA_FILL_LEVEL_SHIFT 26
// Slice: RIS_ADD_DATA_OVFLW:
// additional data fifo overflow
#define MRV_MIPI_RIS_ADD_DATA_OVFLW
#define MRV_MIPI_RIS_ADD_DATA_OVFLW_MASK 0x02000000
#define MRV_MIPI_RIS_ADD_DATA_OVFLW_SHIFT 25
// Slice: RIS_FRAME_END:
// frame end send to output interface
#define MRV_MIPI_RIS_FRAME_END
#define MRV_MIPI_RIS_FRAME_END_MASK 0x01000000
#define MRV_MIPI_RIS_FRAME_END_SHIFT 24
// Slice: RIS_ERR_CS:
// checksum error occurred
#define MRV_MIPI_RIS_ERR_CS
#define MRV_MIPI_RIS_ERR_CS_MASK 0x00800000
#define MRV_MIPI_RIS_ERR_CS_SHIFT 23
// Slice: RIS_ERR_ECC1:
// 1-bit ecc error occurred
#define MRV_MIPI_RIS_ERR_ECC1
#define MRV_MIPI_RIS_ERR_ECC1_MASK 0x00400000
#define MRV_MIPI_RIS_ERR_ECC1_SHIFT 22
// Slice: RIS_ERR_ECC2:
// 2-bit ecc error occurred
#define MRV_MIPI_RIS_ERR_ECC2
#define MRV_MIPI_RIS_ERR_ECC2_MASK 0x00200000
#define MRV_MIPI_RIS_ERR_ECC2_SHIFT 21
// Slice: RIS_ERR_PROTOCOL:
// packet start detected within current packet
#define MRV_MIPI_RIS_ERR_PROTOCOL
#define MRV_MIPI_RIS_ERR_PROTOCOL_MASK 0x00100000
#define MRV_MIPI_RIS_ERR_PROTOCOL_SHIFT 20
// Slice: RIS_ERR_CONTROL:
// PPI interface control error occured
#define MRV_MIPI_RIS_ERR_CONTROL
#define MRV_MIPI_RIS_ERR_CONTROL_MASK 0x000F0000
#define MRV_MIPI_RIS_ERR_CONTROL_SHIFT 16
// Slice: RIS_ERR_EOT_SYNC:
// PPI interface eot sync error occured
#define MRV_MIPI_RIS_ERR_EOT_SYNC
#define MRV_MIPI_RIS_ERR_EOT_SYNC_MASK 0x0000F000
#define MRV_MIPI_RIS_ERR_EOT_SYNC_SHIFT 12
// Slice: RIS_ERR_SOT_SYNC:
// PPI interface sot sync error occured
#define MRV_MIPI_RIS_ERR_SOT_SYNC
#define MRV_MIPI_RIS_ERR_SOT_SYNC_MASK 0x00000F00
#define MRV_MIPI_RIS_ERR_SOT_SYNC_SHIFT 8
// Slice: RIS_ERR_SOT:
// PPI interface sot error occured
#define MRV_MIPI_RIS_ERR_SOT
#define MRV_MIPI_RIS_ERR_SOT_MASK 0x000000F0
#define MRV_MIPI_RIS_ERR_SOT_SHIFT 4
// Slice: RIS_SYNC_FIFO_OVFLW:
// synchronization fifo overflow occurred
#define MRV_MIPI_RIS_SYNC_FIFO_OVFLW
#define MRV_MIPI_RIS_SYNC_FIFO_OVFLW_MASK 0x0000000F
#define MRV_MIPI_RIS_SYNC_FIFO_OVFLW_SHIFT 0
// combination of all interrupt lines
#define MRV_MIPI_RIS_ALL_IRQS
#define MRV_MIPI_RIS_ALL_IRQS_MASK \
  (   0 \
    | MRV_MIPI_RIS_ADD_DATA_FILL_LEVEL_MASK \
    | MRV_MIPI_RIS_ADD_DATA_OVFLW_MASK \
    | MRV_MIPI_RIS_FRAME_END_MASK \
    | MRV_MIPI_RIS_ERR_CS_MASK \
    | MRV_MIPI_RIS_ERR_ECC1_MASK \
    | MRV_MIPI_RIS_ERR_ECC2_MASK \
    | MRV_MIPI_RIS_ERR_PROTOCOL_MASK \
    | MRV_MIPI_RIS_ERR_CONTROL_MASK \
    | MRV_MIPI_RIS_ERR_EOT_SYNC_MASK \
    | MRV_MIPI_RIS_ERR_SOT_SYNC_MASK \
    | MRV_MIPI_RIS_ERR_SOT_MASK \
    | MRV_MIPI_RIS_SYNC_FIFO_OVFLW_MASK \
  )
#define MRV_MIPI_RIS_ALL_IRQS_SHIFT 0

// Register: mipi_mis: Masked interrupt status (0x0010)
// Slice: MIS_ADD_DATA_FILL_LEVEL:
// Programmed fill level was reached; will be raised as long as the fill level is greater or equal the programmed value
#define MRV_MIPI_MIS_ADD_DATA_FILL_LEVEL
#define MRV_MIPI_MIS_ADD_DATA_FILL_LEVEL_MASK 0x04000000
#define MRV_MIPI_MIS_ADD_DATA_FILL_LEVEL_SHIFT 26
// Slice: MIS_ADD_DATA_OVFLW:
// additional data fifo overflow
#define MRV_MIPI_MIS_ADD_DATA_OVFLW
#define MRV_MIPI_MIS_ADD_DATA_OVFLW_MASK 0x02000000
#define MRV_MIPI_MIS_ADD_DATA_OVFLW_SHIFT 25
// Slice: MIS_FRAME_END:
// frame end send to output interface
#define MRV_MIPI_MIS_FRAME_END
#define MRV_MIPI_MIS_FRAME_END_MASK 0x01000000
#define MRV_MIPI_MIS_FRAME_END_SHIFT 24
// Slice: MIS_ERR_CS:
// checksum error occurred
#define MRV_MIPI_MIS_ERR_CS
#define MRV_MIPI_MIS_ERR_CS_MASK 0x00800000
#define MRV_MIPI_MIS_ERR_CS_SHIFT 23
// Slice: MIS_ERR_ECC1:
// 1-bit ecc error occurred
#define MRV_MIPI_MIS_ERR_ECC1
#define MRV_MIPI_MIS_ERR_ECC1_MASK 0x00400000
#define MRV_MIPI_MIS_ERR_ECC1_SHIFT 22
// Slice: MIS_ERR_ECC2:
// 2-bit ecc error occurred
#define MRV_MIPI_MIS_ERR_ECC2
#define MRV_MIPI_MIS_ERR_ECC2_MASK 0x00200000
#define MRV_MIPI_MIS_ERR_ECC2_SHIFT 21
// Slice: MIS_ERR_PROTOCOL:
// packet start detected within current packet
#define MRV_MIPI_MIS_ERR_PROTOCOL
#define MRV_MIPI_MIS_ERR_PROTOCOL_MASK 0x00100000
#define MRV_MIPI_MIS_ERR_PROTOCOL_SHIFT 20
// Slice: MIS_ERR_CONTROL:
// PPI interface control error occured
#define MRV_MIPI_MIS_ERR_CONTROL
#define MRV_MIPI_MIS_ERR_CONTROL_MASK 0x000F0000
#define MRV_MIPI_MIS_ERR_CONTROL_SHIFT 16
// Slice: MIS_ERR_EOT_SYNC:
// PPI interface eot sync error occured
#define MRV_MIPI_MIS_ERR_EOT_SYNC
#define MRV_MIPI_MIS_ERR_EOT_SYNC_MASK 0x0000F000
#define MRV_MIPI_MIS_ERR_EOT_SYNC_SHIFT 12
// Slice: MIS_ERR_SOT_SYNC:
// PPI interface sot sync error occured
#define MRV_MIPI_MIS_ERR_SOT_SYNC
#define MRV_MIPI_MIS_ERR_SOT_SYNC_MASK 0x00000F00
#define MRV_MIPI_MIS_ERR_SOT_SYNC_SHIFT 8
// Slice: MIS_ERR_SOT:
// PPI interface sot error occured
#define MRV_MIPI_MIS_ERR_SOT
#define MRV_MIPI_MIS_ERR_SOT_MASK 0x000000F0
#define MRV_MIPI_MIS_ERR_SOT_SHIFT 4
// Slice: MIS_SYNC_FIFO_OVFLW:
// synchronization fifo overflow occurred
#define MRV_MIPI_MIS_SYNC_FIFO_OVFLW
#define MRV_MIPI_MIS_SYNC_FIFO_OVFLW_MASK 0x0000000F
#define MRV_MIPI_MIS_SYNC_FIFO_OVFLW_SHIFT 0
// combination of all interrupt lines
#define MRV_MIPI_MIS_ALL_IRQS
#define MRV_MIPI_MIS_ALL_IRQS_MASK \
  (   0 \
    | MRV_MIPI_MIS_ADD_DATA_FILL_LEVEL_MASK \
    | MRV_MIPI_MIS_ADD_DATA_OVFLW_MASK \
    | MRV_MIPI_MIS_FRAME_END_MASK \
    | MRV_MIPI_MIS_ERR_CS_MASK \
    | MRV_MIPI_MIS_ERR_ECC1_MASK \
    | MRV_MIPI_MIS_ERR_ECC2_MASK \
    | MRV_MIPI_MIS_ERR_PROTOCOL_MASK \
    | MRV_MIPI_MIS_ERR_CONTROL_MASK \
    | MRV_MIPI_MIS_ERR_EOT_SYNC_MASK \
    | MRV_MIPI_MIS_ERR_SOT_SYNC_MASK \
    | MRV_MIPI_MIS_ERR_SOT_MASK \
    | MRV_MIPI_MIS_SYNC_FIFO_OVFLW_MASK \
  )
#define MRV_MIPI_MIS_ALL_IRQS_SHIFT 0

// Register: mipi_icr: Interrupt clear register (0x0014)
// Slice: ICR_ADD_DATA_FILL_LEVEL:
// 1: clear register; 0: nothing happens
#define MRV_MIPI_ICR_ADD_DATA_FILL_LEVEL
#define MRV_MIPI_ICR_ADD_DATA_FILL_LEVEL_MASK 0x04000000
#define MRV_MIPI_ICR_ADD_DATA_FILL_LEVEL_SHIFT 26
// Slice: ICR_ADD_DATA_OVFLW:
// 1: clear register; 0: nothing happens
#define MRV_MIPI_ICR_ADD_DATA_OVFLW
#define MRV_MIPI_ICR_ADD_DATA_OVFLW_MASK 0x02000000
#define MRV_MIPI_ICR_ADD_DATA_OVFLW_SHIFT 25
// Slice: ICR_FRAME_END:
// 1: clear register; 0: nothing happens
#define MRV_MIPI_ICR_FRAME_END
#define MRV_MIPI_ICR_FRAME_END_MASK 0x01000000
#define MRV_MIPI_ICR_FRAME_END_SHIFT 24
// Slice: ICR_ERR_CS:
// 1: clear register; 0: nothing happens
#define MRV_MIPI_ICR_ERR_CS
#define MRV_MIPI_ICR_ERR_CS_MASK 0x00800000
#define MRV_MIPI_ICR_ERR_CS_SHIFT 23
// Slice: ICR_ERR_ECC1:
// 1: clear register; 0: nothing happens
#define MRV_MIPI_ICR_ERR_ECC1
#define MRV_MIPI_ICR_ERR_ECC1_MASK 0x00400000
#define MRV_MIPI_ICR_ERR_ECC1_SHIFT 22
// Slice: ICR_ERR_ECC2:
// 1: clear register; 0: nothing happens
#define MRV_MIPI_ICR_ERR_ECC2
#define MRV_MIPI_ICR_ERR_ECC2_MASK 0x00200000
#define MRV_MIPI_ICR_ERR_ECC2_SHIFT 21
// Slice: ICR_ERR_PROTOCOL:
// 1: clear register; 0: nothing happens
#define MRV_MIPI_ICR_ERR_PROTOCOL
#define MRV_MIPI_ICR_ERR_PROTOCOL_MASK 0x00100000
#define MRV_MIPI_ICR_ERR_PROTOCOL_SHIFT 20
// Slice: ICR_ERR_CONTROL:
// 1: clear register; 0: nothing happens
#define MRV_MIPI_ICR_ERR_CONTROL
#define MRV_MIPI_ICR_ERR_CONTROL_MASK 0x000F0000
#define MRV_MIPI_ICR_ERR_CONTROL_SHIFT 16
// Slice: ICR_ERR_EOT_SYNC:
// 1: clear register; 0: nothing happens
#define MRV_MIPI_ICR_ERR_EOT_SYNC
#define MRV_MIPI_ICR_ERR_EOT_SYNC_MASK 0x0000F000
#define MRV_MIPI_ICR_ERR_EOT_SYNC_SHIFT 12
// Slice: ICR_ERR_SOT_SYNC:
// 1: clear register; 0: nothing happens
#define MRV_MIPI_ICR_ERR_SOT_SYNC
#define MRV_MIPI_ICR_ERR_SOT_SYNC_MASK 0x00000F00
#define MRV_MIPI_ICR_ERR_SOT_SYNC_SHIFT 8
// Slice: ICR_ERR_SOT:
// 1: clear register; 0: nothing happens
#define MRV_MIPI_ICR_ERR_SOT
#define MRV_MIPI_ICR_ERR_SOT_MASK 0x000000F0
#define MRV_MIPI_ICR_ERR_SOT_SHIFT 4
// Slice: ICR_SYNC_FIFO_OVFLW:
// 1: clear register; 0: nothing happens
#define MRV_MIPI_ICR_SYNC_FIFO_OVFLW
#define MRV_MIPI_ICR_SYNC_FIFO_OVFLW_MASK 0x0000000F
#define MRV_MIPI_ICR_SYNC_FIFO_OVFLW_SHIFT 0
// combination of all interrupt lines
#define MRV_MIPI_ICR_ALL_IRQS
#define MRV_MIPI_ICR_ALL_IRQS_MASK \
  (   0 \
    | MRV_MIPI_ICR_ADD_DATA_FILL_LEVEL_MASK \
    | MRV_MIPI_ICR_ADD_DATA_OVFLW_MASK \
    | MRV_MIPI_ICR_FRAME_END_MASK \
    | MRV_MIPI_ICR_ERR_CS_MASK \
    | MRV_MIPI_ICR_ERR_ECC1_MASK \
    | MRV_MIPI_ICR_ERR_ECC2_MASK \
    | MRV_MIPI_ICR_ERR_PROTOCOL_MASK \
    | MRV_MIPI_ICR_ERR_CONTROL_MASK \
    | MRV_MIPI_ICR_ERR_EOT_SYNC_MASK \
    | MRV_MIPI_ICR_ERR_SOT_SYNC_MASK \
    | MRV_MIPI_ICR_ERR_SOT_MASK \
    | MRV_MIPI_ICR_SYNC_FIFO_OVFLW_MASK \
  )
#define MRV_MIPI_ICR_ALL_IRQS_SHIFT 0

// Register: mipi_isr: Interrupt set register (0x0018)
// Slice: ISR_ADD_DATA_FILL_LEVEL:
// 1: set register; 0: nothing happens
#define MRV_MIPI_ISR_ADD_DATA_FILL_LEVEL
#define MRV_MIPI_ISR_ADD_DATA_FILL_LEVEL_MASK 0x04000000
#define MRV_MIPI_ISR_ADD_DATA_FILL_LEVEL_SHIFT 26
// Slice: ISR_ADD_DATA_OVFLW:
// 1: set register; 0: nothing happens
#define MRV_MIPI_ISR_ADD_DATA_OVFLW
#define MRV_MIPI_ISR_ADD_DATA_OVFLW_MASK 0x02000000
#define MRV_MIPI_ISR_ADD_DATA_OVFLW_SHIFT 25
// Slice: ISR_FRAME_END:
// 1: set register; 0: nothing happens
#define MRV_MIPI_ISR_FRAME_END
#define MRV_MIPI_ISR_FRAME_END_MASK 0x01000000
#define MRV_MIPI_ISR_FRAME_END_SHIFT 24
// Slice: ISR_ERR_CS:
// 1: set register; 0: nothing happens
#define MRV_MIPI_ISR_ERR_CS
#define MRV_MIPI_ISR_ERR_CS_MASK 0x00800000
#define MRV_MIPI_ISR_ERR_CS_SHIFT 23
// Slice: ISR_ERR_ECC1:
// 1: set register; 0: nothing happens
#define MRV_MIPI_ISR_ERR_ECC1
#define MRV_MIPI_ISR_ERR_ECC1_MASK 0x00400000
#define MRV_MIPI_ISR_ERR_ECC1_SHIFT 22
// Slice: ISR_ERR_ECC2:
// 1: set register; 0: nothing happens
#define MRV_MIPI_ISR_ERR_ECC2
#define MRV_MIPI_ISR_ERR_ECC2_MASK 0x00200000
#define MRV_MIPI_ISR_ERR_ECC2_SHIFT 21
// Slice: ISR_ERR_PROTOCOL:
// 1: set register; 0: nothing happens
#define MRV_MIPI_ISR_ERR_PROTOCOL
#define MRV_MIPI_ISR_ERR_PROTOCOL_MASK 0x00100000
#define MRV_MIPI_ISR_ERR_PROTOCOL_SHIFT 20
// Slice: ISR_ERR_CONTROL:
// 1: set register; 0: nothing happens
#define MRV_MIPI_ISR_ERR_CONTROL
#define MRV_MIPI_ISR_ERR_CONTROL_MASK 0x000F0000
#define MRV_MIPI_ISR_ERR_CONTROL_SHIFT 16
// Slice: ISR_ERR_EOT_SYNC:
// 1: set register; 0: nothing happens
#define MRV_MIPI_ISR_ERR_EOT_SYNC
#define MRV_MIPI_ISR_ERR_EOT_SYNC_MASK 0x0000F000
#define MRV_MIPI_ISR_ERR_EOT_SYNC_SHIFT 12
// Slice: ISR_ERR_SOT_SYNC:
// 1: set register; 0: nothing happens
#define MRV_MIPI_ISR_ERR_SOT_SYNC
#define MRV_MIPI_ISR_ERR_SOT_SYNC_MASK 0x00000F00
#define MRV_MIPI_ISR_ERR_SOT_SYNC_SHIFT 8
// Slice: ISR_ERR_SOT:
// 1: set register; 0: nothing happens
#define MRV_MIPI_ISR_ERR_SOT
#define MRV_MIPI_ISR_ERR_SOT_MASK 0x000000F0
#define MRV_MIPI_ISR_ERR_SOT_SHIFT 4
// Slice: ISR_SYNC_FIFO_OVFLW:
// 1: set register; 0: nothing happens
#define MRV_MIPI_ISR_SYNC_FIFO_OVFLW
#define MRV_MIPI_ISR_SYNC_FIFO_OVFLW_MASK 0x0000000F
#define MRV_MIPI_ISR_SYNC_FIFO_OVFLW_SHIFT 0
// combination of all interrupt lines
#define MRV_MIPI_ISR_ALL_IRQS
#define MRV_MIPI_ISR_ALL_IRQS_MASK \
  (   0 \
    | MRV_MIPI_ISR_ADD_DATA_FILL_LEVEL_MASK \
    | MRV_MIPI_ISR_ADD_DATA_OVFLW_MASK \
    | MRV_MIPI_ISR_FRAME_END_MASK \
    | MRV_MIPI_ISR_ERR_CS_MASK \
    | MRV_MIPI_ISR_ERR_ECC1_MASK \
    | MRV_MIPI_ISR_ERR_ECC2_MASK \
    | MRV_MIPI_ISR_ERR_PROTOCOL_MASK \
    | MRV_MIPI_ISR_ERR_CONTROL_MASK \
    | MRV_MIPI_ISR_ERR_EOT_SYNC_MASK \
    | MRV_MIPI_ISR_ERR_SOT_SYNC_MASK \
    | MRV_MIPI_ISR_ERR_SOT_MASK \
    | MRV_MIPI_ISR_SYNC_FIFO_OVFLW_MASK \
  )
#define MRV_MIPI_ISR_ALL_IRQS_SHIFT 0

// Register: mipi_cur_data_id: Current Data Identifier (0x001c)
// Slice: VIRTUAL_CHANNEL:
// virtual channel of currently received packet
#define MRV_MIPI_VIRTUAL_CHANNEL
#define MRV_MIPI_VIRTUAL_CHANNEL_MASK 0x000000C0
#define MRV_MIPI_VIRTUAL_CHANNEL_SHIFT 6
// max value also valid for all data_sel registers
#define MRV_MIPI_VIRTUAL_CHANNEL_MAX (MRV_MIPI_VIRTUAL_CHANNEL_MASK >> MRV_MIPI_VIRTUAL_CHANNEL_SHIFT)
// Slice: DATA_TYPE:
// data type of currently received packet
#define MRV_MIPI_DATA_TYPE
#define MRV_MIPI_DATA_TYPE_MASK 0x0000003F
#define MRV_MIPI_DATA_TYPE_SHIFT 0
// max value also valid for all data_sel registers
#define MRV_MIPI_DATA_TYPE_MAX (MRV_MIPI_DATA_TYPE_MASK >> MRV_MIPI_DATA_TYPE_SHIFT)

// Register: mipi_img_data_sel: Image Data Selector (0x0020)
// Slice: VIRTUAL_CHANNEL_SEL:
// virtual channel selector for image data output
#define MRV_MIPI_VIRTUAL_CHANNEL_SEL
#define MRV_MIPI_VIRTUAL_CHANNEL_SEL_MASK 0x000000C0
#define MRV_MIPI_VIRTUAL_CHANNEL_SEL_SHIFT 6
// Slice: DATA_TYPE_SEL:
// data type selector for image data output:
// 0x18 YUV 420 8-bit
// 0x19 YUV 420 10-bit
// 0x1A Legacy YUV 420 8-bit
// 0x1C YUV 420 8-bit (CSPS)
// 0x1D YUV 420 10-bit (CSPS)
// 0x1E YUV 422 8-bit
// 0x1F YUV 422 10-bit
// 0x20 RGB 444
// 0x21 RGB 555
// 0x22 RGB 565
// 0x23 RGB 666
// 0x24 RGB 888
// 0x28 RAW 6
// 0x29 RAW 7
// 0x2A RAW 8
// 0x2B RAW 10
// 0x2C RAW 12
// 0x30...0x33  User Defined Byte-based data
#define MRV_MIPI_DATA_TYPE_SEL
#define MRV_MIPI_DATA_TYPE_SEL_MASK 0x0000003F
#define MRV_MIPI_DATA_TYPE_SEL_SHIFT 0
#define MRV_MIPI_DATA_TYPE_SEL_YUV420_8BIT        24 // 0x18    YUV 420 8-bit
#define MRV_MIPI_DATA_TYPE_SEL_YUV420_10BIT       25 // 0x19    YUV 420 10-bit
#define MRV_MIPI_DATA_TYPE_SEL_YUV420_8BIT_LEGACY 26 // 0x1A    Legacy YUV 420 8-bit
#define MRV_MIPI_DATA_TYPE_SEL_YUV420_8BIT_CSPS   28 // 0x1C    YUV 420 8-bit (CSPS)
#define MRV_MIPI_DATA_TYPE_SEL_YUV420_10BIT_CSPS  29 // 0x1D    YUV 420 10-bit (CSPS)
#define MRV_MIPI_DATA_TYPE_SEL_YUV422_8BIT        30 // 0x1E    YUV 422 8-bit
#define MRV_MIPI_DATA_TYPE_SEL_YUV422_10BIT       31 // 0x1F    YUV 422 10-bit
#define MRV_MIPI_DATA_TYPE_SEL_RGB444             32 // 0x20    RGB 444
#define MRV_MIPI_DATA_TYPE_SEL_RGB555             33 // 0x21    RGB 555
#define MRV_MIPI_DATA_TYPE_SEL_RGB565             34 // 0x22    RGB 565
#define MRV_MIPI_DATA_TYPE_SEL_RGB666             35 // 0x23    RGB 666
#define MRV_MIPI_DATA_TYPE_SEL_RGB888             36 // 0x24    RGB 888
#define MRV_MIPI_DATA_TYPE_SEL_RAW6               40 // 0x28    RAW 6
#define MRV_MIPI_DATA_TYPE_SEL_RAW7               41 // 0x29    RAW 7
#define MRV_MIPI_DATA_TYPE_SEL_RAW8               42 // 0x2A    RAW 8
#define MRV_MIPI_DATA_TYPE_SEL_RAW10              43 // 0x2B    RAW 10
#define MRV_MIPI_DATA_TYPE_SEL_RAW12              44 // 0x2C    RAW 12
#define MRV_MIPI_DATA_TYPE_SEL_USER1              48 // 0x30...0x37 User Defined Byte-based data
#define MRV_MIPI_DATA_TYPE_SEL_USER2              49 // 0x30...0x37 User Defined Byte-based data
#define MRV_MIPI_DATA_TYPE_SEL_USER3              50 // 0x30...0x37 User Defined Byte-based data
#define MRV_MIPI_DATA_TYPE_SEL_USER4              51 // 0x30...0x37 User Defined Byte-based data
#define MRV_MIPI_DATA_TYPE_SEL_USER5              52 // 0x30...0x37 User Defined Byte-based data
#define MRV_MIPI_DATA_TYPE_SEL_USER6              53 // 0x30...0x37 User Defined Byte-based data
#define MRV_MIPI_DATA_TYPE_SEL_USER7              54 // 0x30...0x37 User Defined Byte-based data
#define MRV_MIPI_DATA_TYPE_SEL_USER8              55 // 0x30...0x37 User Defined Byte-based data

// Register: mipi_add_data_sel_1: Additional Data Selector 1 (0x0024)
// Slice: ADD_DATA_VC_1:
// virtual channel selector for additional data output
#define MRV_MIPI_ADD_DATA_VC_1
#define MRV_MIPI_ADD_DATA_VC_1_MASK 0x000000C0
#define MRV_MIPI_ADD_DATA_VC_1_SHIFT 6
// Slice: ADD_DATA_TYPE_1:
// data type selector for additional data output
#define MRV_MIPI_ADD_DATA_TYPE_1
#define MRV_MIPI_ADD_DATA_TYPE_1_MASK 0x0000003F
#define MRV_MIPI_ADD_DATA_TYPE_1_SHIFT 0

// Register: mipi_add_data_sel_2: Additional Data Selector 2 (0x0028)
// Slice: ADD_DATA_VC_2:
// virtual channel selector for additional data output
#define MRV_MIPI_ADD_DATA_VC_2
#define MRV_MIPI_ADD_DATA_VC_2_MASK 0x000000C0
#define MRV_MIPI_ADD_DATA_VC_2_SHIFT 6
// Slice: ADD_DATA_TYPE_2:
// data type selector for additional data output
#define MRV_MIPI_ADD_DATA_TYPE_2
#define MRV_MIPI_ADD_DATA_TYPE_2_MASK 0x0000003F
#define MRV_MIPI_ADD_DATA_TYPE_2_SHIFT 0

// Register: mipi_add_data_sel_3: Additional Data Selector 3 (0x002c)
// Slice: ADD_DATA_VC_3:
// virtual channel selector for additional data output
#define MRV_MIPI_ADD_DATA_VC_3
#define MRV_MIPI_ADD_DATA_VC_3_MASK 0x000000C0
#define MRV_MIPI_ADD_DATA_VC_3_SHIFT 6
// Slice: ADD_DATA_TYPE_3:
// data type selector for additional data output
#define MRV_MIPI_ADD_DATA_TYPE_3
#define MRV_MIPI_ADD_DATA_TYPE_3_MASK 0x0000003F
#define MRV_MIPI_ADD_DATA_TYPE_3_SHIFT 0

// Register: mipi_add_data_sel_4: Additional Data Selector 4 (0x0030)
// Slice: ADD_DATA_VC_4:
// virtual channel selector for additional data output
#define MRV_MIPI_ADD_DATA_VC_4
#define MRV_MIPI_ADD_DATA_VC_4_MASK 0x000000C0
#define MRV_MIPI_ADD_DATA_VC_4_SHIFT 6
// Slice: ADD_DATA_TYPE_4:
// data type selector for additional data output
#define MRV_MIPI_ADD_DATA_TYPE_4
#define MRV_MIPI_ADD_DATA_TYPE_4_MASK 0x0000003F
#define MRV_MIPI_ADD_DATA_TYPE_4_SHIFT 0

// Register: mipi_add_data_fifo: Additional Data Fifo (0x0034)
// Slice: ADD_DATA_FIFO:
// lowest 4 bytes in additional data fifo;
// reading increments fifo read pointer.
// First embedded data byte will be written to bits 7:0 of 32-bit data word, second data byte written to 15:8 etc.
#define MRV_MIPI_ADD_DATA_FIFO
#define MRV_MIPI_ADD_DATA_FIFO_MASK 0xFFFFFFFF
#define MRV_MIPI_ADD_DATA_FIFO_SHIFT 0

// Register: mipi_add_data_fill_level: Additional Data FIFO Fill Level (0x0038)
// Slice: ADD_DATA_FILL_LEVEL:
// FIFO level in dwords for triggering the FILL_LEVEL interrupt,
// must be 32-bit aligned (bit 0 and bit 1 are hard wired to "00")
#define MRV_MIPI_ADD_DATA_FILL_LEVEL
#define MRV_MIPI_ADD_DATA_FILL_LEVEL_MASK 0x00001FFC
#define MRV_MIPI_ADD_DATA_FILL_LEVEL_SHIFT 0
#define MRV_MIPI_ADD_DATA_FILL_LEVEL_MAX  0x00001FFC

#if (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4)
// Register: mipi_compressed_mode: Compressed raw data types (0x003C)
// Slice: predictor_sel:
// MIPI compress enable
// 0: predictor 1
// 1: predictor 2
#define MRV_MIPI_PREDICTOR_SEL
#define MRV_MIPI_PREDICTOR_SEL_MASK 0x00000100
#define MRV_MIPI_PREDICTOR_SEL_SHIFT 8
#define MRV_MIPI_PREDICTOR_SEL_SHIFT 8
#define MRV_MIPI_PREDICTOR_SEL_SHIFT 8
// Slice: COMPRESSED_SEL:
// Compressed raw data types
// 0x00: 12-8-12 
// 0x01: 12-7-12 
// 0x02: 12-6-12 
// 0x03: 10-8-10 
// 0x04: 10-7-10 
// 0x05: 10-6-10 
#define MRV_MIPI_COMPRESSED_SEL
#define MRV_MIPI_COMPRESSED_SEL_MASK 0x00000070
#define MRV_MIPI_COMPRESSED_SEL_SHIFT 4
#define MRV_MIPI_COMPRESSED_SEL_12_8_12        0 // 0x00    12-8-12
#define MRV_MIPI_COMPRESSED_SEL_12_7_12        1 // 0x01    12-8-12
#define MRV_MIPI_COMPRESSED_SEL_12_6_12        2 // 0x02    12-8-12
#define MRV_MIPI_COMPRESSED_SEL_10_8_10        3 // 0x03    10-8-10
#define MRV_MIPI_COMPRESSED_SEL_10_7_10        4 // 0x04    10-7-10
#define MRV_MIPI_COMPRESSED_SEL_10_6_10        5 // 0x05    10-6-10
#define MRV_MIPI_COMPRESSED_SEL_OFF          255 // 0xFF    off
// max value also valid for all compressed_sel register
#define MRV_MIPI_COMP_MODE_MAX  5
// Slice: compress_en:
// MIPI compress enable
// 0: disable compressed mode
// 1: enable compressed mode processing
#define MRV_MIPI_COMPRESS_EN
#define MRV_MIPI_COMPRESS_EN_MASK 0x00000001
#define MRV_MIPI_COMPRESS_EN_SHIFT 0
#endif    
/********************************************************************/
/* ISP Auto Focus Measurement Registers                             */
/********************************************************************/

#if (MARVIN_FEATURE_AF_MEASURE == MARVIN_FEATURE_EXIST)
    // Register: isp_afm_ctrl: This is the control register for AF measurement unit (0x0000)
    //#define MRV_AFMCTRL_EN           0x00000001 //[0] AF measurement enable
    // Slice: afm_en:
    // AF measurement enable
    // 0: AF measurement is disabled
    // 1: AF measurement is enabled
    // Writing a 1 to this register starts a new measurement and resets the afm_fin (measurement finished) interrupt to 0.
    // As long as the afm_en is 1, the AFM unit calculates new sharpness values for each frame.
    #define MRV_AFM_AFM_EN
    #define MRV_AFM_AFM_EN_MASK 0x00000001
    #define MRV_AFM_AFM_EN_SHIFT 0

    // Register: isp_afm_lt_a: Top Left corner of measure window A (0x0004)
    // Slice: a_h_l:
    // first pixel of window A (horizontal left row), value must be greater or equal 5
    #define MRV_AFM_A_H_L
    #define MRV_AFM_A_H_L_MASK 0x0FFF0000
    #define MRV_AFM_A_H_L_SHIFT 16
    #define MRV_AFM_A_H_L_MIN  5
    #define MRV_AFM_A_H_L_MAX  (MRV_AFM_A_H_L_MASK >> MRV_AFM_A_H_L_SHIFT)
    // Slice: a_v_t:
    // first line of window A (vertical top line), value must be greater or equal 2
    #define MRV_AFM_A_V_T
    #define MRV_AFM_A_V_T_MASK 0x00000FFF
    #define MRV_AFM_A_V_T_SHIFT 0
    #define MRV_AFM_A_V_T_MIN  2
    #define MRV_AFM_A_V_T_MAX  (MRV_AFM_A_V_T_MASK >> MRV_AFM_A_V_T_SHIFT)

    // Register: isp_afm_rb_a: Bottom right corner of measure window A (0x0008)
    // Slice: a_h_r:
    // last pixel of window A (horizontal right row)
    #define MRV_AFM_A_H_R
    #define MRV_AFM_A_H_R_MASK 0x0FFF0000
    #define MRV_AFM_A_H_R_SHIFT 16
    #define MRV_AFM_A_H_R_MIN  5
    #define MRV_AFM_A_H_R_MAX  (MRV_AFM_A_H_R_MASK >> MRV_AFM_A_H_R_SHIFT)
    // Slice: a_v_b:
    // last line of window A (vertical bottom line), value must be lower than (number of lines – 2)
    #define MRV_AFM_A_V_B
    #define MRV_AFM_A_V_B_MASK 0x00000FFF
    #define MRV_AFM_A_V_B_SHIFT 0
    #define MRV_AFM_A_V_B_MIN  2
    #define MRV_AFM_A_V_B_MAX  (MRV_AFM_A_V_B_MASK >> MRV_AFM_A_V_B_SHIFT)

    // Register: isp_afm_lt_b: Top left corner of measure window B (0x000c)
    // Slice: b_h_l:
    // first pixel of window B (horizontal left row), value must be greater or equal 5
    #define MRV_AFM_B_H_L
    #define MRV_AFM_B_H_L_MASK 0x0FFF0000
    #define MRV_AFM_B_H_L_SHIFT 16
    #define MRV_AFM_B_H_L_MIN  5
    #define MRV_AFM_B_H_L_MAX  (MRV_AFM_B_H_L_MASK >> MRV_AFM_B_H_L_SHIFT)
    // Slice: b_v_t:
    // first line of window B (vertical top line), value must be greater or equal 2
    #define MRV_AFM_B_V_T
    #define MRV_AFM_B_V_T_MASK 0x00000FFF
    #define MRV_AFM_B_V_T_SHIFT 0
    #define MRV_AFM_B_V_T_MIN  2
    #define MRV_AFM_B_V_T_MAX  (MRV_AFM_B_V_T_MASK >> MRV_AFM_B_V_T_SHIFT)

    // Register: isp_afm_rb_b: Bottom right corner of measure window B (0x0010)
    // Slice: b_h_r:
    // last pixel of window b (horizontal right row)
    #define MRV_AFM_B_H_R
    #define MRV_AFM_B_H_R_MASK 0x0FFF0000
    #define MRV_AFM_B_H_R_SHIFT 16
    #define MRV_AFM_B_H_R_MIN  5
    #define MRV_AFM_B_H_R_MAX  (MRV_AFM_B_H_R_MASK >> MRV_AFM_B_H_R_SHIFT)
    // Slice: b_v_b:
    // last line of window b (vertical bottom line), value must be lower than (number of lines – 2)
    #define MRV_AFM_B_V_B
    #define MRV_AFM_B_V_B_MASK 0x00000FFF
    #define MRV_AFM_B_V_B_SHIFT 0
    #define MRV_AFM_B_V_B_MIN  2
    #define MRV_AFM_B_V_B_MAX  (MRV_AFM_B_V_B_MASK >> MRV_AFM_B_V_B_SHIFT)

    // Register: isp_afm_lt_c: Top left corner of measure window C (0x0014)
    // Slice: c_h_l:
    // first pixel of window C (horizontal left row), value must be greater or equal 5
    #define MRV_AFM_C_H_L
    #define MRV_AFM_C_H_L_MASK 0x0FFF0000
    #define MRV_AFM_C_H_L_SHIFT 16
    #define MRV_AFM_C_H_L_MIN  5
    #define MRV_AFM_C_H_L_MAX  (MRV_AFM_C_H_L_MASK >> MRV_AFM_C_H_L_SHIFT)
    // Slice: c_v_t:
    // first line of window C (vertical top line), value must be greater or equal 2
    #define MRV_AFM_C_V_T
    #define MRV_AFM_C_V_T_MASK 0x00000FFF
    #define MRV_AFM_C_V_T_SHIFT 0
    #define MRV_AFM_C_V_T_MIN  2
    #define MRV_AFM_C_V_T_MAX  (MRV_AFM_C_V_T_MASK >> MRV_AFM_C_V_T_SHIFT)

    // Register: isp_afm_rb_c: Bottom right corner of measure window C (0x0018)
    // Slice: c_h_r:
    // last pixel of window c (horizontal right row)
    #define MRV_AFM_C_H_R
    #define MRV_AFM_C_H_R_MASK 0x0FFF0000
    #define MRV_AFM_C_H_R_SHIFT 16
    #define MRV_AFM_C_H_R_MIN  5
    #define MRV_AFM_C_H_R_MAX  (MRV_AFM_C_H_R_MASK >> MRV_AFM_C_H_R_SHIFT)
    // Slice: c_v_b:
    // last line of window c (vertical bottom line), value must be lower than (number of lines – 2)
    #define MRV_AFM_C_V_B
    #define MRV_AFM_C_V_B_MASK 0x00000FFF
    #define MRV_AFM_C_V_B_SHIFT 0
    #define MRV_AFM_C_V_B_MIN  2
    #define MRV_AFM_C_V_B_MAX  (MRV_AFM_C_V_B_MASK >> MRV_AFM_C_V_B_SHIFT)

    // Register: isp_afm_thres: Threshold register (0x001c)
    // Slice: afm_thres:
    // AF measurement threshold
    // This register defines a threshold which can be used for minimizing the influence of noise in the measurement result.
    #define MRV_AFM_AFM_THRES
    #define MRV_AFM_AFM_THRES_MASK 0x0000FFFF
    #define MRV_AFM_AFM_THRES_SHIFT 0

    // Register: isp_afm_var_shift: Variable shift register (0x0020)
    // Slice: lum_var_shift:
    // variable shift for luminance summation
    // The lum_var_shift defines the number of bits for the shift operation of the value of the current pixel before summation. The shift operation is used to avoid a luminance sum overflow.
    #define MRV_AFM_LUM_VAR_SHIFT
    #define MRV_AFM_LUM_VAR_SHIFT_MASK 0x00070000
    #define MRV_AFM_LUM_VAR_SHIFT_SHIFT 16
    // Slice: afm_var_shift:
    // variable shift for AF measurement
    // The afm_var_shift defines the number of bits for the shift operation at the end of the calculation chain. The shift operation is used to avoid an AF measurement sum overflow.
    #define MRV_AFM_AFM_VAR_SHIFT
    #define MRV_AFM_AFM_VAR_SHIFT_MASK 0x00000007
    #define MRV_AFM_AFM_VAR_SHIFT_SHIFT 0

    // Register: isp_afm_sum_a: Sharpness Value Status Register of Window A (0x0024)
    // Slice: afm_sum_a:
    // sharpness value of window A
    #define MRV_AFM_AFM_SUM_A
    #define MRV_AFM_AFM_SUM_A_MASK 0xFFFFFFFF
    #define MRV_AFM_AFM_SUM_A_SHIFT 0

    // Register: isp_afm_sum_b: Sharpness Value Status Register of Window B (0x0028)
    // Slice: afm_sum_b:
    // sharpness value of window B
    #define MRV_AFM_AFM_SUM_B
    #define MRV_AFM_AFM_SUM_B_MASK 0xFFFFFFFF
    #define MRV_AFM_AFM_SUM_B_SHIFT 0

    // Register: isp_afm_sum_c: Sharpness Value Status Register of Window C (0x002c)
    // Slice: afm_sum_c:
    // sharpness value of window C
    #define MRV_AFM_AFM_SUM_C
    #define MRV_AFM_AFM_SUM_C_MASK 0xFFFFFFFF
    #define MRV_AFM_AFM_SUM_C_SHIFT 0

    // Register: isp_afm_lum_a: Luminance Value Status Register of Window A (0x0030)
    // Slice: afm_lum_a:
    // luminance value of window A
    #define MRV_AFM_AFM_LUM_A
    #define MRV_AFM_AFM_LUM_A_MASK 0x00FFFFFF
    #define MRV_AFM_AFM_LUM_A_SHIFT 0

    // Register: isp_afm_lum_b: Luminance Value Status Register of Window B (0x0034)
    // Slice: afm_lum_b:
    // luminance value of window B
    #define MRV_AFM_AFM_LUM_B
    #define MRV_AFM_AFM_LUM_B_MASK 0x00FFFFFF
    #define MRV_AFM_AFM_LUM_B_SHIFT 0

    // Register: isp_afm_lum_c: Luminance Value Status Register of Window C (0x0038)
    // Slice: afm_lum_c:
    // luminance value of window C
    #define MRV_AFM_AFM_LUM_C
    #define MRV_AFM_AFM_LUM_C_MASK 0x00FFFFFF
    #define MRV_AFM_AFM_LUM_C_SHIFT 0
#endif  // MARVIN_FEATURE_AF_MEASURE

/********************************************************************/
/* ISP Bad Pixel Correction Registers                               */
/********************************************************************/

#if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
    // Register: isp_bp_ctrl: Control register for bad pixel detection and correction (0x0000)
    //#define MRV_BP_CORR_TYP         0x00000010  //[4]cor_type Correction type
    //#define MRV_BP_REP_APPR         0x00000008  //[3]rep_appr Pixel replace approach
    //#define MRV_BP_D_COR_EN         0x00000004  //[2]dead_cor_en  dead pixel correction enable
    //#define MRV_BP_H_COR_EN         0x00000002  //[1]hot_cor_en   Hot pixel correction enable
    //#define MRV_BP_DET_EN           0x00000001  //[0]bp_det_en    Bad pixel detection enable
    // Slice: cor_type:
    // Correction type
    // 0: correction of bad pixel from the table
    // 1: direct detection and correction
    #define MRV_BP_COR_TYPE
    #define MRV_BP_COR_TYPE_MASK 0x00000010
    #define MRV_BP_COR_TYPE_SHIFT 4
    #define MRV_BP_COR_TYPE_TABLE  0 // 0: correction of bad pixel from the table
    #define MRV_BP_COR_TYPE_DIRECT 1 // 1: direct detection and correction
    // Slice: rep_appr:
    // Pixel replace approach
    // 0: nearest neighbour approach
    // 1: simple bilinear interpolation approach
    #define MRV_BP_REP_APPR
    #define MRV_BP_REP_APPR_MASK 0x00000008
    #define MRV_BP_REP_APPR_SHIFT 3
    #define MRV_BP_REP_APPR_NEAREST  0 // 0: nearest neighbour approach
    #define MRV_BP_REP_APPR_INTERPOL 1 // 1: simple bilinear interpolation approach
    // Slice: dead_cor_en:
    // dead pixel correction enable
    // 0: dead pixel correction is disabled
    // 1: dead pixel correction is enabled
    #define MRV_BP_DEAD_COR_EN
    #define MRV_BP_DEAD_COR_EN_MASK 0x00000004
    #define MRV_BP_DEAD_COR_EN_SHIFT 2
    // Slice: hot_cor_en:
    // Hot pixel correction enable
    // 0: hot pixel correction is disabled
    // 1: hot pixel correction is enabled
    #define MRV_BP_HOT_COR_EN
    #define MRV_BP_HOT_COR_EN_MASK 0x00000002
    #define MRV_BP_HOT_COR_EN_SHIFT 1
    // Slice: bp_det_en:
    // Bad pixel detection enable
    // 0: bad pixel detection is disabled
    // 1: bad pixel detection is enabled
    #define MRV_BP_BP_DET_EN
    #define MRV_BP_BP_DET_EN_MASK 0x00000001
    #define MRV_BP_BP_DET_EN_SHIFT 0

    // Register: isp_bp_cfg1: Thresholds for on-the-fly and table based detection (0x0004)
    //#define MRV_BP_H_THRES          0x0FFF0000  //[25:16]abs_hot_thres    Absolute hot pixel threshold
    //#define MRV_BP_H_THRES_SHIFT            16
    //#define MRV_BP_D_THRES          0x00000FFF  //[9:0]abs_dead_thres Absolute dead pixel threshold
    // Slice: hot_thres:
    // Hot Pixel Threshold
    // Defines the absolute threshold value for hot pixels of on-the-fly detection. If a pixel value is greater than hot_thres, then it is a potential candidate for a "hot" bad pixel depending on the deviation threshold.
    #define MRV_BP_HOT_THRES
    #define MRV_BP_HOT_THRES_MASK 0x0FFF0000
    #define MRV_BP_HOT_THRES_SHIFT 16
    // Slice: dead_thres:
    // Dead Pixel Threshold
    // The meaning of this threshold depends on the selected correction type.
    //
    // If on-the-fly correction is selected (cor_type = "1"), then it is the absolute threshold value for dead pixels. This means, if the pixel value is less than dead_thres, then it is a potential candidate for a “dead” bad pixel depending on the deviation threshold.
    //
    // If the table based correction is selected (cor_type = "0"), then dead_thres defines the "good" condition of evaluated pixels. If the difference between the max and the min table of a pixel is greater than dead_thres, then this pixel is a good pixel.
    #define MRV_BP_DEAD_THRES
    #define MRV_BP_DEAD_THRES_MASK 0x00000FFF
    #define MRV_BP_DEAD_THRES_SHIFT 0

    // Register: isp_bp_cfg2: Deviation thresholds for on-the-fly detection / correction (0x0008)
    //#define MRV_BP_H_DEV            0x0FFF0000  //[25:16]dev_hot_thres    Hot Pixel deviation Threshold
    //#define MRV_BP_H_DEV_SHIFT              16
    //#define MRV_BP_D_DEV            0x00000FFF  //[9:0]dev_dead_thres Dead Pixel deviation Threshold
    // Slice: dev_hot_thres:
    // Hot Pixel deviation Threshold
    #define MRV_BP_DEV_HOT_THRES
    #define MRV_BP_DEV_HOT_THRES_MASK 0x0FFF0000
    #define MRV_BP_DEV_HOT_THRES_SHIFT 16
    // Slice: dev_dead_thres:
    // Dead Pixel deviation Threshold
    #define MRV_BP_DEV_DEAD_THRES
    #define MRV_BP_DEV_DEAD_THRES_MASK 0x00000FFF
    #define MRV_BP_DEV_DEAD_THRES_SHIFT 0

    // Register: isp_bp_number: Number of entries for bad pixel table (table based correction) (0x000c)
    //#define MRV_BP_NUMBER           0x000000FF  //[7:0]bp_number  Number of bad pixel Bad Pixel entries in ISP_BP_TABLE
    // Slice: bp_number:
    // Number of Bad Pixel entries in ISP_BP_TABLE
    #define MRV_BP_BP_NUMBER
#if( MARVIN_FEATURE_BAD_PIXEL_RAM == MARVIN_FEATURE_7BITS )
    #define MRV_BP_BP_NUMBER_MASK 0x000000FF
#elif( MARVIN_FEATURE_BAD_PIXEL_RAM == MARVIN_FEATURE_9BITS )
    #define MRV_BP_BP_NUMBER_MASK 0x000003FF
#elif( MARVIN_FEATURE_BAD_PIXEL_RAM == MARVIN_FEATURE_11BITS )
    #define MRV_BP_BP_NUMBER_MASK 0x00000FFF
#else //#if( MARVIN_FEATURE_BAD_PIXEL_RAM == MARVIN_FEATURE_7BITS )
    return 0;
#endif
    #define MRV_BP_BP_NUMBER_SHIFT 0

    // Register: isp_bp_table_addr: TABLE Address for table-based correction algorithm (0x0010)
    //#define MRV_BP_TBL_ADDR         0x0000007F  //[6:0]bp_table_addr  Setting of RAM start address for initializing
    // Slice: bp_table_addr:
    // Setting of RAM start address for initializing
    #define MRV_BP_BP_TABLE_ADDR
#if( MARVIN_FEATURE_BAD_PIXEL_RAM == MARVIN_FEATURE_7BITS )
    #define MRV_BP_BP_TABLE_ADDR_MASK 0x0000007F
#elif( MARVIN_FEATURE_BAD_PIXEL_RAM == MARVIN_FEATURE_9BITS )
    #define MRV_BP_BP_TABLE_ADDR_MASK 0x000001FF
#elif( MARVIN_FEATURE_BAD_PIXEL_RAM == MARVIN_FEATURE_11BITS )
    #define MRV_BP_BP_TABLE_ADDR_MASK 0x000007FF
#else //#if( MARVIN_FEATURE_BAD_PIXEL_RAM == MARVIN_FEATURE_7BITS )
    return 0;
#endif
    #define MRV_BP_BP_TABLE_ADDR_SHIFT 0
    #define MRV_BP_BP_TABLE_ADDR_MAX MRV_BP_BP_TABLE_ADDR_MASK

    // Register: isp_bp_table_data: TABLE DATA register for read and write access of table RAM (0x0014)
    //#define MRV_BP_PIX_TYP          0x80000000  //[31]pix_type    Bad pixel type
    //#define MRV_BP_V_ADDR           0x07FF0000  //[26:16]v_addr   Bad Pixel vertical address
    //#define MRV_BP_V_ADDR_SHIFT             16
    //#define MRV_BP_H_ADDR           0x000007FF  //[10:0]h_addr    Bad Pixel horizontal address
    //#define MRV_BP_MAX_TBL_NUM             128  //number of elements in the bad pixel table
    // Slice: pix_type:
    // Bad pixel type
    // 0: “Dead” Pixel
    // 1: “Hot” Pixel
    #define MRV_BP_PIX_TYPE
    #define MRV_BP_PIX_TYPE_MASK 0x80000000
    #define MRV_BP_PIX_TYPE_SHIFT 31
    #define MRV_BP_PIX_TYPE_DEAD   0u // 0: "Dead" Pixel
    #define MRV_BP_PIX_TYPE_HOT    1u // 1: "Hot" Pixel
    // Slice: v_addr:
    // Bad Pixel vertical address
    #define MRV_BP_V_ADDR
#if( MARVIN_FEATURE_BAD_PIXEL_RAM == MARVIN_FEATURE_7BITS )
    #define MRV_BP_V_ADDR_MASK 0x07FF0000
#elif( MARVIN_FEATURE_BAD_PIXEL_RAM == MARVIN_FEATURE_9BITS )
    #define MRV_BP_V_ADDR_MASK 0x07FF0000
#elif( MARVIN_FEATURE_BAD_PIXEL_RAM == MARVIN_FEATURE_11BITS )
    #define MRV_BP_V_ADDR_MASK 0x0FFF0000
#else //#if( MARVIN_FEATURE_BAD_PIXEL_RAM == MARVIN_FEATURE_7BITS )
    return 0;
#endif
    #define MRV_BP_V_ADDR_SHIFT 16
    // Slice: h_addr:
    // Bad Pixel horizontal address
    #define MRV_BP_H_ADDR
    #define MRV_BP_H_ADDR_MASK 0x00000FFF
    #define MRV_BP_H_ADDR_SHIFT 0

    // Register: isp_bp_new_number: Number of entries in new bad pixel table (0x0018)
    //#define MRV_BP_NEW_NUMBER       0x0000000F  //[3:0]bp_new_number  Number of bad pixel Bad Pixel entries in ISP_BP_NEW_TABLE
    // Slice: bp_new_number:
    // Number of Bad Pixel entries in ISP_BP_NEW_TABLE
    // A write access to ISP_BP_TABLE_DATA resets the following registers:
    // ISP_BP_NEW_NUMBER
    // ISP_BP_NEW_TABLE (0…7)
    // and the following interrupt bits:
    // BP_DET
    // BP_NEW_TAB_FUL
    #define MRV_BP_BP_NEW_NUMBER
    #define MRV_BP_BP_NEW_NUMBER_MASK 0x0000000F
    #define MRV_BP_BP_NEW_NUMBER_SHIFT 0

    // Register array: isp_bp_new_table: New bad pixel table of table-based detection algorithm (0x01C+n (n=0..7))
    //#define MRV_BP_NEW_VALUE       0xF8000000   //[31:27]new_value    Value of detected bad pixel (MSBs)
    //#define MRV_BP_NEW_VALUE_SHIFT         27
    //#define MRV_BP_NEW_V_ADDR      0x07FF0000  //[26:16]new_v_addr    Bad Pixel vertical address
    //#define MRV_BP_NEW_V_ADDR_SHIFT        16
    //#define MRV_BP_NEW_H_ADDR      0x000007FF  //[10:0]new_h_addr Bad Pixel horizontal address
    // Slice: new_value:
    // Value of detected bad pixel (MSBs)
    #define MRV_BP_NEW_VALUE
#if (MARVIN_FEATURE_FRAMESIZE == MARVIN_FEATURE_12M6)
    #define MRV_BP_NEW_VALUE_MASK 0xF0000000
    #define MRV_BP_NEW_VALUE_SHIFT 28
#else
    #define MRV_BP_NEW_VALUE_MASK 0xF8000000
    #define MRV_BP_NEW_VALUE_SHIFT 27
#endif
    // Slice: new_v_addr:
    // Bad Pixel vertical address
    #define MRV_BP_NEW_V_ADDR
#if (MARVIN_FEATURE_FRAMESIZE == MARVIN_FEATURE_12M6)
    #define MRV_BP_NEW_V_ADDR_MASK 0x0FFF0000
#else
    #define MRV_BP_NEW_V_ADDR_MASK 0x07FF0000
#endif
    #define MRV_BP_NEW_V_ADDR_SHIFT 16
    // Slice: new_h_addr:
    // Bad Pixel horizontal address
    #define MRV_BP_NEW_H_ADDR
    #define MRV_BP_NEW_H_ADDR_MASK 0x00000FFF
    #define MRV_BP_NEW_H_ADDR_SHIFT 0
#endif  // MARVIN_FEATURE_BAD_PIXEL

/********************************************************************/
/* ISP Lens Shade Correction Registers                              */
/********************************************************************/

#if (MARVIN_FEATURE_LENS_SHADING >= MARVIN_FEATURE_LSC_V1)
    // Register: isp_lsc_ctrl: Lens shade control (0x0000)
    //#define MRV_LSC_EN                  0x00000001 //[0] lens shading correction on
    // Slice: lsc_en:
    // 0: lens shading correction off
    // 1: lens shading correction on
    #define MRV_LSC_LSC_EN
    #define MRV_LSC_LSC_EN_MASK 0x00000001
    #define MRV_LSC_LSC_EN_SHIFT 0

    // Register: isp_lsc_r_table_addr: Table RAM Address for red component (0x0004)
    // Slice: r_ram_addr:
    // table address in RAM for samples of the R color component.
    // Will be automatically incremented by each read or write access to the table.
    // Valid addresses are in the range 0 to 152.
    #define MRV_LSC_R_RAM_ADDR
    #define MRV_LSC_R_RAM_ADDR_MASK 0x000000FF
    #define MRV_LSC_R_RAM_ADDR_SHIFT 0
    #define MRV_LSC_R_RAM_ADDR_MIN  0x00000000
    #define MRV_LSC_R_RAM_ADDR_MAX  0x00000098

    // Register: isp_lsc_g_table_addr: Table RAM Address for green component (0x0008)
    // Slice: g_ram_addr:
    // table address in RAM for samples of the G color component.
    // Will be automatically incremented by each read or write access to the table.
    #define MRV_LSC_G_RAM_ADDR
    #define MRV_LSC_G_RAM_ADDR_MASK 0x000000FF
    #define MRV_LSC_G_RAM_ADDR_SHIFT 0
    #define MRV_LSC_G_RAM_ADDR_MIN  0x00000000
    #define MRV_LSC_G_RAM_ADDR_MAX  0x00000098

    // Register: isp_lsc_b_table_addr: Table RAM Address for blue component (0x000c)
    // Slice: b_ram_addr:
    // table address in RAM for samples of the B color component.
    // Will be automatically incremented by each read or write access to the table.
    #define MRV_LSC_B_RAM_ADDR
    #define MRV_LSC_B_RAM_ADDR_MASK 0x000000FF
    #define MRV_LSC_B_RAM_ADDR_SHIFT 0
    #define MRV_LSC_B_RAM_ADDR_MIN  0x00000000
    #define MRV_LSC_B_RAM_ADDR_MAX  0x00000098

    // Register: isp_lsc_r_table_data: Sample table red (0x0010)
    // Slice: r_sample_1:
    // correction factor at sample point (fixed point number: 2 bits integer with 10-bit fractional part, range 1..3.999)
    #define MRV_LSC_R_SAMPLE_1
    #define MRV_LSC_R_SAMPLE_1_MASK 0x00FFF000
    #define MRV_LSC_R_SAMPLE_1_SHIFT 12
    // Slice: r_sample_0:
    // correction factor at sample point (fixed point number: 2 bits integer with 10-bit fractional part, range 1..3.999)
    #define MRV_LSC_R_SAMPLE_0
    #define MRV_LSC_R_SAMPLE_0_MASK 0x00000FFF
    #define MRV_LSC_R_SAMPLE_0_SHIFT 0

    // Register: isp_lsc_g_table_data: Sample table green (0x0014)
    // Slice: g_sample_1:
    // correction factor at sample point (fixed point number: 2 bits integer with 10-bit fractional part, range 1..3.999)
    #define MRV_LSC_G_SAMPLE_1
    #define MRV_LSC_G_SAMPLE_1_MASK 0x00FFF000
    #define MRV_LSC_G_SAMPLE_1_SHIFT 12
    // Slice: g_sample_0:
    // correction factor at sample point (fixed point number: 2 bits integer with 10-bit fractional part, range 1..3.999)
    #define MRV_LSC_G_SAMPLE_0
    #define MRV_LSC_G_SAMPLE_0_MASK 0x00000FFF
    #define MRV_LSC_G_SAMPLE_0_SHIFT 0

    // Register: isp_lsc_b_table_data: Sample table blue (0x0018)
    // Slice: b_sample_1:
    // correction factor at sample point (fixed point number: 2 bits integer with 10-bit fractional part, range 1..3.999)
    #define MRV_LSC_B_SAMPLE_1
    #define MRV_LSC_B_SAMPLE_1_MASK 0x00FFF000
    #define MRV_LSC_B_SAMPLE_1_SHIFT 12
    // Slice: b_sample_0:
    // correction factor at sample point (fixed point number: 2 bits integer with 10-bit fractional part, range 1..3.999)
    #define MRV_LSC_B_SAMPLE_0
    #define MRV_LSC_B_SAMPLE_0_MASK 0x00000FFF
    #define MRV_LSC_B_SAMPLE_0_SHIFT 0

    // Register: isp_lsc_xgrad_01: Gradient table x (0x0020)
    // Slice: xgrad_1:
    // factor for x-gradient calculation of sector 1
    #define MRV_LSC_XGRAD_1
    #define MRV_LSC_XGRAD_1_MASK 0x0FFF0000
    #define MRV_LSC_XGRAD_1_SHIFT 16
    // Slice: xgrad_0:
    // factor for x-gradient calculation of sector 0
    #define MRV_LSC_XGRAD_0
    #define MRV_LSC_XGRAD_0_MASK 0x00000FFF
    #define MRV_LSC_XGRAD_0_SHIFT 0

    // Register: isp_lsc_xgrad_23: Gradient table x (0x0024)
    // Slice: xgrad_3:
    // factor for x-gradient calculation of sector 3
    #define MRV_LSC_XGRAD_3
    #define MRV_LSC_XGRAD_3_MASK 0x0FFF0000
    #define MRV_LSC_XGRAD_3_SHIFT 16
    // Slice: xgrad_2:
    // factor for x-gradient calculation of sector 2
    #define MRV_LSC_XGRAD_2
    #define MRV_LSC_XGRAD_2_MASK 0x00000FFF
    #define MRV_LSC_XGRAD_2_SHIFT 0

    // Register: isp_lsc_xgrad_45: Gradient table x (0x0028)
    // Slice: xgrad_5:
    // factor for x-gradient calculation of sector 5
    #define MRV_LSC_XGRAD_5
    #define MRV_LSC_XGRAD_5_MASK 0x0FFF0000
    #define MRV_LSC_XGRAD_5_SHIFT 16
    // Slice: xgrad_4:
    // factor for x-gradient calculation of sector 4
    #define MRV_LSC_XGRAD_4
    #define MRV_LSC_XGRAD_4_MASK 0x00000FFF
    #define MRV_LSC_XGRAD_4_SHIFT 0

    // Register: isp_lsc_xgrad_67: Gradient table x (0x002c)
    // Slice: xgrad_7:
    // factor for x-gradient calculation of sector 7
    #define MRV_LSC_XGRAD_7
    #define MRV_LSC_XGRAD_7_MASK 0x0FFF0000
    #define MRV_LSC_XGRAD_7_SHIFT 16
    // Slice: xgrad_6:
    // factor for x-gradient calculation of sector 6
    #define MRV_LSC_XGRAD_6
    #define MRV_LSC_XGRAD_6_MASK 0x00000FFF
    #define MRV_LSC_XGRAD_6_SHIFT 0

    // Register: isp_lsc_ygrad_01: Gradient table y (0x0030)
    // Slice: ygrad_1:
    // factor for y-gradient calculation of sector 1
    #define MRV_LSC_YGRAD_1
    #define MRV_LSC_YGRAD_1_MASK 0x0FFF0000
    #define MRV_LSC_YGRAD_1_SHIFT 16
    // Slice: ygrad_0:
    // factor for y-gradient calculation of sector 0
    #define MRV_LSC_YGRAD_0
    #define MRV_LSC_YGRAD_0_MASK 0x00000FFF
    #define MRV_LSC_YGRAD_0_SHIFT 0

    // Register: isp_lsc_ygrad_23: Gradient table y (0x0034)
    // Slice: ygrad_3:
    // factor for y-gradient calculation of sector 3
    #define MRV_LSC_YGRAD_3
    #define MRV_LSC_YGRAD_3_MASK 0x0FFF0000
    #define MRV_LSC_YGRAD_3_SHIFT 16
    // Slice: ygrad_2:
    // factor for y-gradient calculation of sector 2
    #define MRV_LSC_YGRAD_2
    #define MRV_LSC_YGRAD_2_MASK 0x00000FFF
    #define MRV_LSC_YGRAD_2_SHIFT 0

    // Register: isp_lsc_ygrad_45: Gradient table y (0x0038)
    // Slice: ygrad_5:
    // factor for y-gradient calculation of sector 5
    #define MRV_LSC_YGRAD_5
    #define MRV_LSC_YGRAD_5_MASK 0x0FFF0000
    #define MRV_LSC_YGRAD_5_SHIFT 16
    // Slice: ygrad_4:
    // factor for y-gradient calculation of sector 4
    #define MRV_LSC_YGRAD_4
    #define MRV_LSC_YGRAD_4_MASK 0x00000FFF
    #define MRV_LSC_YGRAD_4_SHIFT 0

    // Register: isp_lsc_ygrad_67: Gradient table y (0x003c)
    // Slice: ygrad_7:
    // factor for y-gradient calculation of sector 7
    #define MRV_LSC_YGRAD_7
    #define MRV_LSC_YGRAD_7_MASK 0x0FFF0000
    #define MRV_LSC_YGRAD_7_SHIFT 16
    // Slice: ygrad_6:
    // factor for y-gradient calculation of sector 6
    #define MRV_LSC_YGRAD_6
    #define MRV_LSC_YGRAD_6_MASK 0x00000FFF
    #define MRV_LSC_YGRAD_6_SHIFT 0

    // Register: isp_lsc_xsize_01: Size table (0x0040)
    // Slice: x_sect_size_1:
    // sector size 1 in x-direction
    #define MRV_LSC_X_SECT_SIZE_1
    #define MRV_LSC_X_SECT_SIZE_1_MASK 0x03FF0000
    #define MRV_LSC_X_SECT_SIZE_1_SHIFT 16
    // Slice: x_sect_size_0:
    // sector size 0 in x-direction
    #define MRV_LSC_X_SECT_SIZE_0
    #define MRV_LSC_X_SECT_SIZE_0_MASK 0x000003FF
    #define MRV_LSC_X_SECT_SIZE_0_SHIFT 0

    // Register: isp_lsc_xsize_23: Size table (0x0044)
    // Slice: x_sect_size_3:
    // sector size 3 in x-direction
    #define MRV_LSC_X_SECT_SIZE_3
    #define MRV_LSC_X_SECT_SIZE_3_MASK 0x03FF0000
    #define MRV_LSC_X_SECT_SIZE_3_SHIFT 16
    // Slice: x_sect_size_2:
    // sector size 2 in x-direction
    #define MRV_LSC_X_SECT_SIZE_2
    #define MRV_LSC_X_SECT_SIZE_2_MASK 0x000003FF
    #define MRV_LSC_X_SECT_SIZE_2_SHIFT 0

    // Register: isp_lsc_xsize_45: Size table (0x0048)
    // Slice: x_sect_size_5:
    // sector size 5 in x-direction
    #define MRV_LSC_X_SECT_SIZE_5
    #define MRV_LSC_X_SECT_SIZE_5_MASK 0x03FF0000
    #define MRV_LSC_X_SECT_SIZE_5_SHIFT 16
    // Slice: x_sect_size_4:
    // sector size 4in x-direction
    #define MRV_LSC_X_SECT_SIZE_4
    #define MRV_LSC_X_SECT_SIZE_4_MASK 0x000003FF
    #define MRV_LSC_X_SECT_SIZE_4_SHIFT 0

    // Register: isp_lsc_xsize_67: Size table (0x004c)
    // Slice: x_sect_size_7:
    // sector size 7 in x-direction
    #define MRV_LSC_X_SECT_SIZE_7
    #define MRV_LSC_X_SECT_SIZE_7_MASK 0x03FF0000
    #define MRV_LSC_X_SECT_SIZE_7_SHIFT 16
    // Slice: x_sect_size_6:
    // sector size 6 in x-direction
    #define MRV_LSC_X_SECT_SIZE_6
    #define MRV_LSC_X_SECT_SIZE_6_MASK 0x000003FF
    #define MRV_LSC_X_SECT_SIZE_6_SHIFT 0

    // Register: isp_lsc_ysize_01: Size table (0x0050)
    // Slice: y_sect_size_1:
    // sector size 1 in y-direction
    #define MRV_LSC_Y_SECT_SIZE_1
    #define MRV_LSC_Y_SECT_SIZE_1_MASK 0x03FF0000
    #define MRV_LSC_Y_SECT_SIZE_1_SHIFT 16
    // Slice: y_sect_size_0:
    // sector size 0 in y-direction
    #define MRV_LSC_Y_SECT_SIZE_0
    #define MRV_LSC_Y_SECT_SIZE_0_MASK 0x000003FF
    #define MRV_LSC_Y_SECT_SIZE_0_SHIFT 0

    // Register: isp_lsc_ysize_23: Size table (0x0054)
    // Slice: y_sect_size_3:
    // sector size 3 in y-direction
    #define MRV_LSC_Y_SECT_SIZE_3
    #define MRV_LSC_Y_SECT_SIZE_3_MASK 0x03FF0000
    #define MRV_LSC_Y_SECT_SIZE_3_SHIFT 16
    // Slice: y_sect_size_2:
    // sector size 2 in y-direction
    #define MRV_LSC_Y_SECT_SIZE_2
    #define MRV_LSC_Y_SECT_SIZE_2_MASK 0x000003FF
    #define MRV_LSC_Y_SECT_SIZE_2_SHIFT 0

    // Register: isp_lsc_ysize_45: Size table (0x0058)
    // Slice: y_sect_size_5:
    // sector size 5 in y-direction
    #define MRV_LSC_Y_SECT_SIZE_5
    #define MRV_LSC_Y_SECT_SIZE_5_MASK 0x03FF0000
    #define MRV_LSC_Y_SECT_SIZE_5_SHIFT 16
    // Slice: y_sect_size_4:
    // sector size 4 in y-direction
    #define MRV_LSC_Y_SECT_SIZE_4
    #define MRV_LSC_Y_SECT_SIZE_4_MASK 0x000003FF
    #define MRV_LSC_Y_SECT_SIZE_4_SHIFT 0

    // Register: isp_lsc_ysize_67: Size table (0x005c)
    // Slice: y_sect_size_7:
    // sector size 7 in y-direction
    #define MRV_LSC_Y_SECT_SIZE_7
    #define MRV_LSC_Y_SECT_SIZE_7_MASK 0x03FF0000
    #define MRV_LSC_Y_SECT_SIZE_7_SHIFT 16
    // Slice: y_sect_size_6:
    // sector size 6 in y-direction
    #define MRV_LSC_Y_SECT_SIZE_6
    #define MRV_LSC_Y_SECT_SIZE_6_MASK 0x000003FF
    #define MRV_LSC_Y_SECT_SIZE_6_SHIFT 0
#endif  // MARVIN_FEATURE_LENS_SHADING

/********************************************************************/
/* ISP Image Stabilization Registers                                */
/********************************************************************/

#if ((MARVIN_FEATURE_IMG_STABILIZATION >= MARVIN_FEATURE_IMG_STABILIZATION_V1) || \
     (MARVIN_FEATURE_IMG_STABILIZATION <= MARVIN_FEATURE_IMG_STABILIZATION_V2))
  #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)
    // Register: isp_is_ctrl: Image Stabilization Control Register (0x0000)
    //#define MRV_IS_CTRL_EN      (0x00000001)    // [0] bypass mode
    // Slice: is_en:
    // 1: image stabilization switched on
    // 0: image stabilization switched off
    #define MRV_IS_IS_EN
    #define MRV_IS_IS_EN_MASK 0x00000001
    #define MRV_IS_IS_EN_SHIFT 0

    // Register: isp_is_recenter: Recenter register (0x0004)
    //#define MRV_IS_RECENTER     (0x00000007)    // [2:0] per frame is
    // Slice: is_recenter:
    // 000: recenter feature switched off
    // 1..7: recentering by (cur_h/v_offs-H/V_OFFS)/2^RECENTER
    #define MRV_IS_IS_RECENTER
    #define MRV_IS_IS_RECENTER_MASK 0x00000007
    #define MRV_IS_IS_RECENTER_SHIFT 0
    #define MRV_IS_IS_RECENTER_MAX (MRV_IS_IS_RECENTER_MASK >> MRV_IS_IS_RECENTER_SHIFT)
  #endif  // #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)

    // Register: isp_is_h_offs: Horizontal offset of output window (0x0008)
    // Slice: is_h_offs:
    // horizontal picture offset in pixel
    #define MRV_IS_IS_H_OFFS
    #define MRV_IS_IS_H_OFFS_MASK 0x00001FFF
    #define MRV_IS_IS_H_OFFS_SHIFT 0

    // Register: isp_is_v_offs: Vertical offset of output window (0x000c)
    // Slice: is_v_offs:
    // vertical picture offset in lines
    #define MRV_IS_IS_V_OFFS
    #define MRV_IS_IS_V_OFFS_MASK 0x00000FFF
    #define MRV_IS_IS_V_OFFS_SHIFT 0

    // Register: isp_is_h_size: Output horizontal picture size (0x0010)
    // Slice: is_h_size:
    // horizontal picture size in pixel
    // if ISP_MODE is set to
    // 001-(ITU-R BT.656 YUV),
    // 010-( ITU-R BT.601 YUV),
    // 011-( ITU-R BT.601 Bayer RGB),
    // 101-( ITU-R BT.656 Bayer RGB)
    // only even numbers are accepted, because complete quadruples of YUYV(YCbYCr) are needed for the following modules. If an odd size is programmed the value will be truncated to even size.
    #define MRV_IS_IS_H_SIZE
    #define MRV_IS_IS_H_SIZE_MASK 0x00003FFF
    #define MRV_IS_IS_H_SIZE_SHIFT 0

    // Register: isp_is_v_size: Output vertical picture size (0x0014)
    // Slice: is_v_size:
    // vertical picture size in lines
    #define MRV_IS_IS_V_SIZE
    #define MRV_IS_IS_V_SIZE_MASK 0x00000FFF
    #define MRV_IS_IS_V_SIZE_SHIFT 0

  #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)
    // Register: isp_is_max_dx: Maximum Horizontal Displacement (0x0018)
    //#define MRV_IS_MAX_DX       (0x00000FFF)    // [11:0] offset x
    // Slice: is_max_dx:
    // maximum allowed accumulated horizontal displacement in pixels
    #define MRV_IS_IS_MAX_DX
    #define MRV_IS_IS_MAX_DX_MASK 0x00000FFF
    #define MRV_IS_IS_MAX_DX_SHIFT 0
    #define MRV_IS_IS_MAX_DX_MAX (MRV_IS_IS_MAX_DX_MASK >> MRV_IS_IS_MAX_DX_SHIFT)

    // Register: isp_is_max_dy: Maximum Vertical Displacement (0x001c)
    //#define MRV_IS_MAX_DY       (0x00000FFF)    // [11:0] offset y
    // Slice: is_max_dy:
    // maximum allowed accumulated vertical displacement in lines
    #define MRV_IS_IS_MAX_DY
    #define MRV_IS_IS_MAX_DY_MASK 0x00000FFF
    #define MRV_IS_IS_MAX_DY_SHIFT 0
    #define MRV_IS_IS_MAX_DY_MAX (MRV_IS_IS_MAX_DY_MASK >> MRV_IS_IS_MAX_DY_SHIFT)

    // Register: isp_is_displace: Camera displacement (0x0020)
    //#define MRV_IS_DISPLACE     (0x0FFF0FFF)    // [27:16,11:0] dy,dx
    // Slice: dy:
    // ISP_IS will compensate for vertical camera displacement of DY lines in the next frame
    // Note: signed values from -2048 (0xF800) to 2047 (0x07FF)
    #define MRV_IS_DY
    #define MRV_IS_DY_MASK 0x0FFF0000
    #define MRV_IS_DY_SHIFT 16
    #define MRV_IS_DY_MAX 0x000007FF
    #define MRV_IS_DY_MIN (~ MRV_IS_DY_MAX)
    // Slice: dx:
    // ISP_IS will compensate for horizontal camera displacement of DX pixels in the next frame
    // Note: signed values from -2048 (0xF800) to 2047 (0x07FF)
    #define MRV_IS_DX
    #define MRV_IS_DX_MASK 0x00000FFF
    #define MRV_IS_DX_SHIFT 0
    #define MRV_IS_DX_MAX 0x000007FF
    #define MRV_IS_DX_MIN (~ MRV_IS_DX_MAX)
  #endif  // #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)

    // Register: isp_is_h_offs_shd: current horizontal offset of output window (shadow register) (0x0024)
    // Slice: is_h_offs_shd:
    // current horizonatl picture offset in lines
    #define MRV_IS_IS_H_OFFS_SHD
    #define MRV_IS_IS_H_OFFS_SHD_MASK 0x00001FFF
    #define MRV_IS_IS_H_OFFS_SHD_SHIFT 0

    // Register: isp_is_v_offs_shd: current vertical offset of output window (shadow register) (0x0028)
    // Slice: is_v_offs_shd:
    // current vertical picture offset in lines
    #define MRV_IS_IS_V_OFFS_SHD
    #define MRV_IS_IS_V_OFFS_SHD_MASK 0x00000FFF
    #define MRV_IS_IS_V_OFFS_SHD_SHIFT 0

    // Register: isp_is_h_size_shd: current output horizontal picture size (shadow register) (0x002c)
    // Slice: isp_h_size_shd:
    // current horizontal picture size in pixel
    #define MRV_IS_ISP_H_SIZE_SHD
    #define MRV_IS_ISP_H_SIZE_SHD_MASK 0x00001FFF
    #define MRV_IS_ISP_H_SIZE_SHD_SHIFT 0

    // Register: isp_is_v_size_shd: current output vertical picture size (shadow register) (0x0030)
    // Slice: isp_v_size_shd:
    // vertical picture size in lines
    #define MRV_IS_ISP_V_SIZE_SHD
    #define MRV_IS_ISP_V_SIZE_SHD_MASK 0x00000FFF
    #define MRV_IS_ISP_V_SIZE_SHD_SHIFT 0
#endif  // MARVIN_FEATURE_IMG_STABILIZATION

/********************************************************************/
/* ISP Histogram Module Registers                                   */
/********************************************************************/

#if (MARVIN_FEATURE_ISP_HISTOGRAM >= MARVIN_FEATURE_HIST_V1)
    // Register: isp_hist_prop: Histogram properties (0x0000)
    // Slice: hist_pdiv:
    // histogram predivider, process every (hist_pdiv+1)th pixel, all other pixels are skipped
    // 0,1,2: not allowed
    // 3: process every fourth input pixel
    // 4: process every fifth input pixel
    // ...
    // 7FH: process every 128th pixel
    #define MRV_HIST_HIST_PDIV
    #define MRV_HIST_HIST_PDIV_MASK 0x000007F8
    #define MRV_HIST_HIST_PDIV_SHIFT 3
#if (MARVIN_FEATURE_ISP_HISTOGRAM == MARVIN_FEATURE_HIST_V1)
    #define MRV_HIST_HIST_PDIV_MIN  0x00000003
#else // (MARVIN_FEATURE_ISP_HISTOGRAM == MARVIN_FEATURE_HIST_V1)
    #define MRV_HIST_HIST_PDIV_MIN  0x00000003
#endif // (MARVIN_FEATURE_ISP_HISTOGRAM == MARVIN_FEATURE_HIST_V1)
    #define MRV_HIST_HIST_PDIV_MAX  0x000000FF
    // Slice: hist_mode:
    // histogram mode, luminance is taken at ISP output before output formatter, RGB is taken at xtalk output
    // 7, 6: must not be used
    // 5: Y (luminance) histogram
    // 4: B histogram
    // 3: G histogram
    // 2: R histogram
    // 1: RGB combined histogram
    // 0: disable, no measurements
    #define MRV_HIST_HIST_MODE
    #define MRV_HIST_HIST_MODE_MASK 0x00000007
    #define MRV_HIST_HIST_MODE_SHIFT 0
    #define MRV_HIST_HIST_MODE_MAX  5 // because 6 and 7 are reserved
    #define MRV_HIST_HIST_MODE_LUM  5 // 5: Y (luminance) histogram
    #define MRV_HIST_HIST_MODE_B    4 // 4: B histogram
    #define MRV_HIST_HIST_MODE_G    3 // 3: G histogram
    #define MRV_HIST_HIST_MODE_R    2 // 2: R histogram
    #define MRV_HIST_HIST_MODE_RGB  1 // 1: RGB combined histogram
    #define MRV_HIST_HIST_MODE_NONE 0 // 0: disable, no measurements

    // Register: isp_hist_h_offs: Histogram window horizontal offset (0x0004)
    // Slice: hist_h_offs:
    // horizontal offset of programmable histogram window,
    // only pixels inside this window are processed
    #define MRV_HIST_HIST_H_OFFS
    #if (MARVIN_FEATURE_ISP_HISTOGRAM >= MARVIN_FEATURE_HIST_V3)
      #define MRV_HIST_HIST_H_OFFS_MASK 0x00001FFF
    #else
      #define MRV_HIST_HIST_H_OFFS_MASK 0x00000FFF
    #endif
    #define MRV_HIST_HIST_H_OFFS_SHIFT 0
    #define MRV_HIST_HIST_H_OFFS_MAX (MRV_HIST_HIST_H_OFFS_MASK >> MRV_HIST_HIST_H_OFFS_SHIFT)

    // Register: isp_hist_v_offs: Histogram window vertical offset (0x0008)
    // Slice: hist_v_offs:
    // vertical offset of programmable histogram window,
    // only pixels inside this window are processed
    #define MRV_HIST_HIST_V_OFFS
    #define MRV_HIST_HIST_V_OFFS_MASK 0x00000FFF
    #define MRV_HIST_HIST_V_OFFS_SHIFT 0
    #define MRV_HIST_HIST_V_OFFS_MAX (MRV_HIST_HIST_V_OFFS_MASK >> MRV_HIST_HIST_V_OFFS_SHIFT)

    // Register: isp_hist_h_size: histogram window horizontal size (0x000c)
    // Slice: hist_h_size:
    // horizontal size of programmable histogram window,
    // only pixels inside this window are processed
    #define MRV_HIST_HIST_H_SIZE
#if (MARVIN_FEATURE_ISP_HISTOGRAM == MARVIN_FEATURE_HIST_V1)
    #define MRV_HIST_HIST_H_SIZE_MASK 0x00000FFF
#elif (MARVIN_FEATURE_ISP_HISTOGRAM == MARVIN_FEATURE_HIST_V3)
    #define MRV_HIST_HIST_H_SIZE_MASK 0x000003FF
#else // MARVIN_FEATURE_ISP_HISTOGRAM == MARVIN_FEATURE_HIST_V1
    #define MRV_HIST_HIST_H_SIZE_MASK 0x00001FFF
#endif // MARVIN_FEATURE_ISP_HISTOGRAM == MARVIN_FEATURE_HIST_V1
    #define MRV_HIST_HIST_H_SIZE_SHIFT 0
    #define MRV_HIST_HIST_H_SIZE_MAX (MRV_HIST_HIST_H_SIZE_MASK >> MRV_HIST_HIST_H_SIZE_SHIFT)

    // Register: isp_hist_v_size: histogram window vertical size (0x0010)
    // Slice: hist_v_size:
    // vertical size of programmable histogram window,
    // only pixels inside this window are processed
    #define MRV_HIST_HIST_V_SIZE
#if (MARVIN_FEATURE_ISP_HISTOGRAM == MARVIN_FEATURE_HIST_V3)
    #define MRV_HIST_HIST_V_SIZE_MASK 0x000003FF
#else // MARVIN_FEATURE_ISP_HISTOGRAM == MARVIN_FEATURE_HIST_V3
    #define MRV_HIST_HIST_V_SIZE_MASK 0x00000FFF
#endif // MARVIN_FEATURE_ISP_HISTOGRAM == MARVIN_FEATURE_HIST_V3
    #define MRV_HIST_HIST_V_SIZE_SHIFT 0
    #define MRV_HIST_HIST_V_SIZE_MAX (MRV_HIST_HIST_V_SIZE_MASK >> MRV_HIST_HIST_V_SIZE_SHIFT)

    // Register array: isp_hist_bin: histogram measurement result bin (0x014+n (n=0..15))
    // Slice: hist_bin_n:
    // measured bin count
    #define MRV_HIST_HIST_BIN_N
#if (MARVIN_FEATURE_ISP_HISTOGRAM == MARVIN_FEATURE_HIST_V1)
    #define MRV_HIST_HIST_BIN_N_MASK 0x000000FF
#elif (MARVIN_FEATURE_ISP_HISTOGRAM == MARVIN_FEATURE_HIST_V3)
    #define MRV_HIST_HIST_BIN_N_MASK 0x000FFFFF
#else // MARVIN_FEATURE_ISP_HISTOGRAM == MARVIN_FEATURE_HIST_V1
    #define MRV_HIST_HIST_BIN_N_MASK 0x0000FFFF
#endif // MARVIN_FEATURE_ISP_HISTOGRAM == MARVIN_FEATURE_HIST_V1
    #define MRV_HIST_HIST_BIN_N_SHIFT 0
    #define MRV_HIST_HIST_BIN_N_MAX (MRV_HIST_HIST_BIN_N_MASK >> MRV_HIST_HIST_BIN_N_SHIFT)
#endif  // MARVIN_FEATURE_ISP_HISTOGRAM >= MARVIN_FEATURE_HIST_V1


#if (MARVIN_FEATURE_ISP_HISTOGRAM == MARVIN_FEATURE_HIST_V3)
    // Matrix 5x5 weight
    //possible values are 1=0x10,15/16=0x0F,14/16,...,1/16,0
    #define MRV_HIST_WEIGHT_MAX 0x10
    // Register: isp_hist_weight_00TO30: Parameters for weight mask (0x0054)
    // Slice: weight_00:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_00
    #define MRV_HIST_WEIGHT_00_MASK 0x0000001F
    #define MRV_HIST_WEIGHT_00_SHIFT 0
    // Slice: weight_10:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_10
    #define MRV_HIST_WEIGHT_10_MASK 0x00001F00
    #define MRV_HIST_WEIGHT_10_SHIFT 8
    // Slice: weight_20:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_20
    #define MRV_HIST_WEIGHT_20_MASK 0x001F0000
    #define MRV_HIST_WEIGHT_20_SHIFT 16
    // Slice: weight_30:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_30
    #define MRV_HIST_WEIGHT_30_MASK 0x1F000000
    #define MRV_HIST_WEIGHT_30_SHIFT 24

    // Register: isp_hist_weight_40TO21: Parameters for weight mask (0x0058)
    // Slice: weight_40:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_40
    #define MRV_HIST_WEIGHT_40_MASK 0x0000001F
    #define MRV_HIST_WEIGHT_40_SHIFT 0
    // Slice: WEIGHT_01:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_01
    #define MRV_HIST_WEIGHT_01_MASK 0x00001F00
    #define MRV_HIST_WEIGHT_01_SHIFT 8
    // Slice: WEIGHT_11:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_11
    #define MRV_HIST_WEIGHT_11_MASK 0x001F0000
    #define MRV_HIST_WEIGHT_11_SHIFT 16
    // Slice: WEIGHT_21:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_21
    #define MRV_HIST_WEIGHT_21_MASK 0x1F000000
    #define MRV_HIST_WEIGHT_21_SHIFT 24

    // Register: isp_hist_weight_31TO12: Parameters for weight mask (0x005C)
    // Slice: WEIGHT_31:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_31
    #define MRV_HIST_WEIGHT_31_MASK 0x0000001F
    #define MRV_HIST_WEIGHT_31_SHIFT 0
    // Slice: WEIGHT_41:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_41
    #define MRV_HIST_WEIGHT_41_MASK 0x00001F00
    #define MRV_HIST_WEIGHT_41_SHIFT 8
    // Slice: WEIGHT_02:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_02
    #define MRV_HIST_WEIGHT_02_MASK 0x001F0000
    #define MRV_HIST_WEIGHT_02_SHIFT 16
    // Slice: WEIGHT_12:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_12
    #define MRV_HIST_WEIGHT_12_MASK 0x1F000000
    #define MRV_HIST_WEIGHT_12_SHIFT 24

    // Register: isp_hist_weight_22TO03: Parameters for weight mask (0x0060)
    // Slice: WEIGHT_22:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_22
    #define MRV_HIST_WEIGHT_22_MASK 0x0000001F
    #define MRV_HIST_WEIGHT_22_SHIFT 0
    // Slice: WEIGHT_32:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_32
    #define MRV_HIST_WEIGHT_32_MASK 0x00001F00
    #define MRV_HIST_WEIGHT_32_SHIFT 8
    // Slice: WEIGHT_42:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_42
    #define MRV_HIST_WEIGHT_42_MASK 0x001F0000
    #define MRV_HIST_WEIGHT_42_SHIFT 16
    // Slice: WEIGHT_03:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_03
    #define MRV_HIST_WEIGHT_03_MASK 0x1F000000
    #define MRV_HIST_WEIGHT_03_SHIFT 24

    // Register: isp_hist_weight_13TO43: Parameters for weight mask (0x0064)
    // Slice: WEIGHT_13:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_13
    #define MRV_HIST_WEIGHT_13_MASK 0x0000001F
    #define MRV_HIST_WEIGHT_13_SHIFT 0
    // Slice: WEIGHT_23:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_23
    #define MRV_HIST_WEIGHT_23_MASK 0x00001F00
    #define MRV_HIST_WEIGHT_23_SHIFT 8
    // Slice: WEIGHT_33:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_33
    #define MRV_HIST_WEIGHT_33_MASK 0x001F0000
    #define MRV_HIST_WEIGHT_33_SHIFT 16
    // Slice: WEIGHT_43:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_43
    #define MRV_HIST_WEIGHT_43_MASK 0x1F000000
    #define MRV_HIST_WEIGHT_43_SHIFT 24

    // Register: isp_hist_weight_04TO34: Parameters for weight mask (0x0068)
    // Slice: WEIGHT_04:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_04
    #define MRV_HIST_WEIGHT_04_MASK 0x0000001F
    #define MRV_HIST_WEIGHT_04_SHIFT 0
    // Slice: WEIGHT_14:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_14
    #define MRV_HIST_WEIGHT_14_MASK 0x00001F00
    #define MRV_HIST_WEIGHT_14_SHIFT 8
    // Slice: WEIGHT_24:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_24
    #define MRV_HIST_WEIGHT_24_MASK 0x001F0000
    #define MRV_HIST_WEIGHT_24_SHIFT 16
    // Slice: WEIGHT_34:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_34
    #define MRV_HIST_WEIGHT_34_MASK 0x1F000000
    #define MRV_HIST_WEIGHT_34_SHIFT 24

    // Register: isp_hist_weight_44: Parameters for weight mask (0x006C)
    // Slice: weight_44:
    // weight signed value range 16..0
    #define MRV_HIST_WEIGHT_44
    #define MRV_HIST_WEIGHT_44_MASK 0x0000001F
    #define MRV_HIST_WEIGHT_44_SHIFT 0
#endif // (MARVIN_FEATURE_ISP_HISTOGRAM == MARVIN_FEATURE_HIST_V3)

/********************************************************************/
/* ISP Filter2 Module Registers                                     */
/********************************************************************/

#if (MARVIN_FEATURE_ISP_FILTER == MARVIN_FEATURE_ISP_FILTER_V1)
    // Register: isp_filt_mode: Mode control for filter block (0x0000)
    // Slice: Chrominance_h_mode:
    // Chroma filter horizontal mode
    // 00: horizontal chroma filter bypass
    // 01: horizontal chroma filter 1 static [10 12 10]
    // 10: horizontal chroma filter 2 (dynamic blur1) *Default*
    // 11: horizontal chroma filter 3 (dynamic blur2)
    #define MRV_FILT_CHROMINANCE_H_MODE
    #define MRV_FILT_CHROMINANCE_H_MODE_MASK 0x000000C0
    #define MRV_FILT_CHROMINANCE_H_MODE_SHIFT 6
    #define MRV_FILT_CHROMINANCE_H_MODE_BYPASS 0 // 00 horizontal chroma filter bypass
    #define MRV_FILT_CHROMINANCE_H_MODE_STATIC 1 // 01 horizontal chroma filter 1 static [10 12 10]
    #define MRV_FILT_CHROMINANCE_H_MODE_DYN_1  2 // 10 horizontal chroma filter 2 (dynamic blur1) *Default*
    #define MRV_FILT_CHROMINANCE_H_MODE_DYN_2  3 // 11 horizontal chroma filter 3 (dynamic blur2)
    // Slice: Chrominance_v_mode:
    // Chroma filter vertical mode
    // 00: vertical chroma filter bypass
    // 01: vertical chroma filter 1 static [8 16 8] *Default*
    // 10: vertical chroma filter 2 static [10 12 10]
    // 11: vertical chroma filter 3 static [12 8 12]
    #define MRV_FILT_CHROMINANCE_V_MODE
    #define MRV_FILT_CHROMINANCE_V_MODE_MASK 0x00000030
    #define MRV_FILT_CHROMINANCE_V_MODE_SHIFT 4
    #define MRV_FILT_CHROMINANCE_V_MODE_BYPASS   0 // 00 vertical chroma filter bypass
    #define MRV_FILT_CHROMINANCE_V_MODE_STATIC8  1 // 01 vertical chroma filter 1 static [8 16 8] *Default*
    #define MRV_FILT_CHROMINANCE_V_MODE_STATIC10 2 // 10 vertical chroma filter 2 static [10 12 10]
    #define MRV_FILT_CHROMINANCE_V_MODE_STATIC12 3 // 11 vertical chroma filter 3 static [12 8 12]
    // Slice: diag_sharp_mode:
    // 1: diagonal sharpening mode for detail level 2, ISP_FILT_MASK_DIAG = sharp mask
    // 0: diagonal blurring mode for detail level 2, ISP_FILT_MASK_DIAG = blur mask   *Default*
    #define MRV_FILT_DIAG_SHARP_MODE
    #define MRV_FILT_DIAG_SHARP_MODE_MASK 0x00000008
    #define MRV_FILT_DIAG_SHARP_MODE_SHIFT 3
    #define MRV_FILT_DIAG_SHARP_MODE_BLUR  0 // 0: diagonal blurring mode for detail level 2, ISP_FILT_MASK_DIAG = blur mask   *Default*
    #define MRV_FILT_DIAG_SHARP_MODE_SHARP 1 // 1: diagonal sharpening mode for detail level 2, ISP_FILT_MASK_DIAG = sharp mask
    // Slice: filt_mode:
    // 00: static mode, uses
    // isp_filt_mask_lin for vertical green filter
    // isp_filt_mask_lin_orth for horizontal green filter
    //
    // 01: dynamic noise reduction/sharpen  *Default*
    // 10: reserved
    // 11: dynamic noise reduction/sharpen  +
    // diagonal sharpening mode for detail level 0 at diagonal edges:
    // ISP_FILT_MASK_SHARP_DIAG will be used instead of ISP_FILT_MASK_SHARP1
    #define MRV_FILT_FILT_MODE
    #define MRV_FILT_FILT_MODE_MASK 0x00000006
    #define MRV_FILT_FILT_MODE_SHIFT 1
    #define MRV_FILT_FILT_MODE_STATIC       0 // 00: static mode
    #define MRV_FILT_FILT_MODE_DYNAMIC      1 // 01: dynamic noise reduction/sharpen  *Default*
    #define MRV_FILT_FILT_MODE_DYNAMIC_DIAG 3 // 11: dynamic noise reduction/sharpen + diagonal sharpening mode
    // Slice: filt_enable:
    // 1 enable filter
    // 0 bypass filter  *Default*
    #define MRV_FILT_FILT_ENABLE
    #define MRV_FILT_FILT_ENABLE_MASK 0x00000001
    #define MRV_FILT_FILT_ENABLE_SHIFT 0

    // Register: isp_filt_mask_sharp_0: Parameters for Sharp0 filter mask (0x0004)
    // Slice: out_coeff:
    // outer coefficients signed value range -64..63
    #define MRV_FILT_OUT_COEFF
    #define MRV_FILT_OUT_COEFF_MASK 0x007F0000
    #define MRV_FILT_OUT_COEFF_SHIFT 16
    // Slice: mid_coeff:
    // mid coefficients signed value range -64..63
    #define MRV_FILT_MID_COEFF
    #define MRV_FILT_MID_COEFF_MASK 0x00007F00
    #define MRV_FILT_MID_COEFF_SHIFT 8
    // Slice: cen_coeff:
    // center coefficient signed value range -64..63
    #define MRV_FILT_CEN_COEFF
    #define MRV_FILT_CEN_COEFF_MASK 0x0000007F
    #define MRV_FILT_CEN_COEFF_SHIFT 0
    // Slice: coeff_all:
    // combination of all coefficients
    #define MRV_FILT_COEFF_ALL
    #define MRV_FILT_COEFF_ALL_MASK \
      (  0 \
       | MRV_FILT_OUT_COEFF_MASK \
       | MRV_FILT_MID_COEFF_MASK \
       | MRV_FILT_CEN_COEFF_MASK \
      )
    #define MRV_FILT_COEFF_ALL_SHIFT 0

    // Register: isp_filt_mask_sharp_1: Parameters for sharp filter mask (0x0008)
    /* same defines as for isp_filt_mask_sharp_0
    // Slice: out_coeff:
    // outer coefficients signed value range -64..63
    #define MRV_FILT_OUT_COEFF
    #define MRV_FILT_OUT_COEFF_MASK 0x007F0000
    #define MRV_FILT_OUT_COEFF_SHIFT 16
    // Slice: mid_coeff:
    // mid coefficients signed value range -64..63
    #define MRV_FILT_MID_COEFF
    #define MRV_FILT_MID_COEFF_MASK 0x00007F00
    #define MRV_FILT_MID_COEFF_SHIFT 8
    // Slice: cen_coeff:
    // center coefficient signed value range -64..63
    #define MRV_FILT_CEN_COEFF
    #define MRV_FILT_CEN_COEFF_MASK 0x0000007F
    #define MRV_FILT_CEN_COEFF_SHIFT 0
    */

    // Register: isp_filt_mask_sharp_diag: Parameters for filter mask diagonal sharp mode (0x000c)
    /* same defines as for isp_filt_mask_sharp_0
    // Slice: out_coeff:
    // outer coefficients signed value range -64..63
    #define MRV_FILT_OUT_COEFF
    #define MRV_FILT_OUT_COEFF_MASK 0x007F0000
    #define MRV_FILT_OUT_COEFF_SHIFT 16
    // Slice: mid_coeff:
    // mid coefficients signed value range -64..63
    #define MRV_FILT_MID_COEFF
    #define MRV_FILT_MID_COEFF_MASK 0x00007F00
    #define MRV_FILT_MID_COEFF_SHIFT 8
    // Slice: cen_coeff:
    // center coefficient signed value range -64..63
    #define MRV_FILT_CEN_COEFF
    #define MRV_FILT_CEN_COEFF_MASK 0x0000007F
    #define MRV_FILT_CEN_COEFF_SHIFT 0
    */

    // Register: isp_filt_mask_blur_max: Parameters for maximum blur filter mask (0x0010)
    /* same defines as for isp_filt_mask_sharp_0
    // Slice: out_coeff:
    // outer coefficients signed value range -64..63
    #define MRV_FILT_OUT_COEFF
    #define MRV_FILT_OUT_COEFF_MASK 0x007F0000
    #define MRV_FILT_OUT_COEFF_SHIFT 16
    // Slice: mid_coeff:
    // mid coefficients signed value range -64..63
    #define MRV_FILT_MID_COEFF
    #define MRV_FILT_MID_COEFF_MASK 0x00007F00
    #define MRV_FILT_MID_COEFF_SHIFT 8
    // Slice: cen_coeff:
    // center coefficient signed value range -64..63
    #define MRV_FILT_CEN_COEFF
    #define MRV_FILT_CEN_COEFF_MASK 0x0000007F
    #define MRV_FILT_CEN_COEFF_SHIFT 0
    */

    // Register: isp_filt_mask_blur: Parameters for weak 2 dimensional blur filter mask (0x0014)
    /* same defines as for isp_filt_mask_sharp_0
    // Slice: out_coeff:
    // outer coefficients signed value range -64..63
    #define MRV_FILT_OUT_COEFF
    #define MRV_FILT_OUT_COEFF_MASK 0x007F0000
    #define MRV_FILT_OUT_COEFF_SHIFT 16
    // Slice: mid_coeff:
    // mid coefficients signed value range -64..63
    #define MRV_FILT_MID_COEFF
    #define MRV_FILT_MID_COEFF_MASK 0x00007F00
    #define MRV_FILT_MID_COEFF_SHIFT 8
    // Slice: cen_coeff:
    // center coefficient signed value range -64..63
    #define MRV_FILT_CEN_COEFF
    #define MRV_FILT_CEN_COEFF_MASK 0x0000007F
    #define MRV_FILT_CEN_COEFF_SHIFT 0
    */

    // Register: isp_filt_mask_lin: Parameters for one dimensional filter mask (0x0018)
    /* same defines as for isp_filt_mask_sharp_0
    // Slice: out_coeff:
    // outer coefficients signed value range -64..63
    #define MRV_FILT_OUT_COEFF
    #define MRV_FILT_OUT_COEFF_MASK 0x007F0000
    #define MRV_FILT_OUT_COEFF_SHIFT 16
    // Slice: mid_coeff:
    // mid coefficients signed value range -64..63
    #define MRV_FILT_MID_COEFF
    #define MRV_FILT_MID_COEFF_MASK 0x00007F00
    #define MRV_FILT_MID_COEFF_SHIFT 8
    // Slice: cen_coeff:
    // center coefficient signed value range -64..63
    #define MRV_FILT_CEN_COEFF
    #define MRV_FILT_CEN_COEFF_MASK 0x0000007F
    #define MRV_FILT_CEN_COEFF_SHIFT 0
    */

    // Register: isp_filt_mask_orth: Parameters for one dimensional orthogonal direction filter mask (0x001c)
    /* same defines as for isp_filt_mask_sharp_0
    // Slice: out_coeff:
    // outer coefficients signed value range -64..63
    #define MRV_FILT_OUT_COEFF
    #define MRV_FILT_OUT_COEFF_MASK 0x007F0000
    #define MRV_FILT_OUT_COEFF_SHIFT 16
    // Slice: mid_coeff:
    // mid coefficients signed value range -64..63
    #define MRV_FILT_MID_COEFF
    #define MRV_FILT_MID_COEFF_MASK 0x00007F00
    #define MRV_FILT_MID_COEFF_SHIFT 8
    // Slice: cen_coeff:
    // center coefficient signed value range -64..63
    #define MRV_FILT_CEN_COEFF
    #define MRV_FILT_CEN_COEFF_MASK 0x0000007F
    #define MRV_FILT_CEN_COEFF_SHIFT 0
    */

    // Register: isp_filt_mask_diag: Parameters for vertical filter mask in diagonal mode (0x0020)
    /* same defines as for isp_filt_mask_sharp_0
    // Slice: out_coeff:
    // outer coefficients signed value range -64..63
    #define MRV_FILT_OUT_COEFF
    #define MRV_FILT_OUT_COEFF_MASK 0x007F0000
    #define MRV_FILT_OUT_COEFF_SHIFT 16
    // Slice: mid_coeff:
    // mid coefficients signed value range -64..63
    #define MRV_FILT_MID_COEFF
    #define MRV_FILT_MID_COEFF_MASK 0x00007F00
    #define MRV_FILT_MID_COEFF_SHIFT 8
    // Slice: cen_coeff:
    // center coefficient signed value range -64..63
    #define MRV_FILT_CEN_COEFF
    #define MRV_FILT_CEN_COEFF_MASK 0x0000007F
    #define MRV_FILT_CEN_COEFF_SHIFT 0
    */

    // Register: isp_filt_mask_h_diag: Parameters for vertical filter mask in diagonal mode (0x0024)
    /* same defines as for isp_filt_mask_sharp_0
    // Slice: out_coeff:
    // outer coefficients signed value range -64..63
    #define MRV_FILT_OUT_COEFF
    #define MRV_FILT_OUT_COEFF_MASK 0x007F0000
    #define MRV_FILT_OUT_COEFF_SHIFT 16
    // Slice: mid_coeff:
    // mid coefficients signed value range -64..63
    #define MRV_FILT_MID_COEFF
    #define MRV_FILT_MID_COEFF_MASK 0x00007F00
    #define MRV_FILT_MID_COEFF_SHIFT 8
    // Slice: cen_coeff:
    // center coefficient signed value range -64..63
    #define MRV_FILT_CEN_COEFF
    #define MRV_FILT_CEN_COEFF_MASK 0x0000007F
    #define MRV_FILT_CEN_COEFF_SHIFT 0
    */

    // Register: isp_filt_blur_th0: Blurring threshold (0x0028)
    // Slice: blur_th0:
    // Threshold for max_gradient below detail level 3 is selected
    #define MRV_FILT_BLUR_TH0
    #define MRV_FILT_BLUR_TH0_MASK 0x000003FF
    #define MRV_FILT_BLUR_TH0_SHIFT 0

    // Register: isp_filt_blur_th1: Blurring threshold (0x002c)
    // Slice: blur_th1:
    // Threshold for max_gradient below detail level 4 is selected
    #define MRV_FILT_BLUR_TH1
    #define MRV_FILT_BLUR_TH1_MASK 0x000003FF
    #define MRV_FILT_BLUR_TH1_SHIFT 0

    // Register: isp_filt_sharp0_th: Sharpening threshold1 (0x0030)
    // Slice: sharp0_th:
    // Threshold for max_gradient above detail level 1 is selected
    #define MRV_FILT_SHARP0_TH
    #define MRV_FILT_SHARP0_TH_MASK 0x000003FF
    #define MRV_FILT_SHARP0_TH_SHIFT 0

    // Register: isp_filt_sharp1_th: Sharpening threshold1 (0x0034)
    // Slice: sharp1_th:
    // Threshold for max_gradient above detail level 0 is selected
    #define MRV_FILT_SHARP1_TH
    #define MRV_FILT_SHARP1_TH_MASK 0x000003FF
    #define MRV_FILT_SHARP1_TH_SHIFT 0

    // Register: isp_filt_lum_weight: Parameters for luminance weight function (0x0038)
    // Slice: LUM_WEIGHT_GAIN:
    // gain of luminance weight function
    #define MRV_FILT_LUM_WEIGHT_GAIN
    #define MRV_FILT_LUM_WEIGHT_GAIN_MASK 0x00070000
    #define MRV_FILT_LUM_WEIGHT_GAIN_SHIFT 16
    // Slice: LUM_WEIGHT_MAX:
    // maximum of luminance weight function
    #define MRV_FILT_LUM_WEIGHT_MAX
    #define MRV_FILT_LUM_WEIGHT_MAX_MASK 0x0000FF00
    #define MRV_FILT_LUM_WEIGHT_MAX_SHIFT 8
    // Slice: LUM_WEIGHT_MIN:
    // minimum of luminance weight function
    #define MRV_FILT_LUM_WEIGHT_MIN
    #define MRV_FILT_LUM_WEIGHT_MIN_MASK 0x000000FF
    #define MRV_FILT_LUM_WEIGHT_MIN_SHIFT 0
#elif (MARVIN_FEATURE_ISP_FILTER == MARVIN_FEATURE_ISP_FILTER_V2)
    // Register: isp_filt_mode: mode control register for the filter block (0x0000)
    // Slice: stage1_select:
    // Green filter stage 1 select (range 0x0...0x8)
    // 0x0 maximum blurring
    // 0x4 Default
    // 0x7 minimum blurring
    // 0x8 filter stage1 bypass
    // For a detailed description refer to chapter "ISP Filter Programming" of user manual
    #define MRV_FILT_STAGE1_SELECT
    #define MRV_FILT_STAGE1_SELECT_MASK 0x00000F00
    #define MRV_FILT_STAGE1_SELECT_SHIFT 8
    // Note: all values between 0 and 8 are possible, but only some of them defined here:
    #define MRV_FILT_STAGE1_SELECT_MAX_BLUR 0 // 0x0 maximum blurring
    #define MRV_FILT_STAGE1_SELECT_DEFAULT  4 // 0x4 Default
    #define MRV_FILT_STAGE1_SELECT_MIN_BLUR 7 // 0x7 minimum blurring
    #define MRV_FILT_STAGE1_SELECT_BYPASS   8 // 0x8 filter stage1 bypass
    // Slice: filt_chr_h_mode:
    // Chroma filter horizontal mode
    // 00 horizontal chroma filter bypass
    // 01 horizontal chroma filter 1 static mask = [10 12 10]
    // 10 horizontal chroma filter 2 (dynamic blur1)
    // 11 horizontal chroma filter 3 (dynamic blur2) Default
    #define MRV_FILT_FILT_CHR_H_MODE
    #define MRV_FILT_FILT_CHR_H_MODE_MASK 0x000000C0
    #define MRV_FILT_FILT_CHR_H_MODE_SHIFT 6
    #define MRV_FILT_FILT_CHR_H_MODE_BYPASS 0 // 00 horizontal chroma filter bypass
    #define MRV_FILT_FILT_CHR_H_MODE_STATIC 1 // 01 horizontal chroma filter 1 static mask = [10 12 10]
    #define MRV_FILT_FILT_CHR_H_MODE_DYN_1  2 // 10 horizontal chroma filter 2 (dynamic blur1)
    #define MRV_FILT_FILT_CHR_H_MODE_DYN_2  3 // 11 horizontal chroma filter 3 (dynamic blur2) Default
    // Slice: filt_chr_v_mode:
    // Chroma filter vertical mode
    // 00 vertical chroma filter bypass
    // 01 vertical chroma filter 1 static [8 16 8]
    // 10 vertical chroma filter 2 static [10 12 10]
    // 11 vertical chroma filter 3 static [12 8 12] Default
    #define MRV_FILT_FILT_CHR_V_MODE
    #define MRV_FILT_FILT_CHR_V_MODE_MASK 0x00000030
    #define MRV_FILT_FILT_CHR_V_MODE_SHIFT 4
    #define MRV_FILT_FILT_CHR_V_MODE_BYPASS   0 // 00 vertical chroma filter bypass
    #define MRV_FILT_FILT_CHR_V_MODE_STATIC8  1 // 01 vertical chroma filter 1 static [8 16 8]
    #define MRV_FILT_FILT_CHR_V_MODE_STATIC10 2 // 10 vertical chroma filter 2 static [10 12 10]
    #define MRV_FILT_FILT_CHR_V_MODE_STATIC12 3 // 11 vertical chroma filter 3 static [12 8 12] Default
    // Slice: filt_mode:
    // 0 green filter static mode (active filter factor = FILT_FAC_MID)
    // 1 dynamic noise reduction/sharpen Default
    #define MRV_FILT_FILT_MODE
    #define MRV_FILT_FILT_MODE_MASK 0x00000002
    #define MRV_FILT_FILT_MODE_SHIFT 1
    #define MRV_FILT_FILT_MODE_STATIC  0 // 0 green filter static mode (active filter factor = FILT_FAC_MID)
    #define MRV_FILT_FILT_MODE_DYNAMIC 1 // 1 dynamic noise reduction/sharpen Default
    // Slice: filt_enable:
    // 1 enable filter
    // 0 bypass filter Default
    #define MRV_FILT_FILT_ENABLE
    #define MRV_FILT_FILT_ENABLE_MASK 0x00000001
    #define MRV_FILT_FILT_ENABLE_SHIFT 0

    // Register: isp_filt_thresh_bl0: Blurring threshold 0 (0x0028)
    // Slice: filt_thresh_bl0:
    // If filt_thresh_bl1 < sum_grad < filt_thresh_bl0 then filt_fac_bl0 is selected
    #define MRV_FILT_FILT_THRESH_BL0
    #define MRV_FILT_FILT_THRESH_BL0_MASK 0x000003FF
    #define MRV_FILT_FILT_THRESH_BL0_SHIFT 0

    // Register: isp_filt_thresh_bl1: Blurring threshold 1 (0x002c)
    // Slice: filt_thresh_bl1:
    // If sum_grad < filt_thresh_bl1 then filt_fac_bl1 is selected
    #define MRV_FILT_FILT_THRESH_BL1
    #define MRV_FILT_FILT_THRESH_BL1_MASK 0x000003FF
    #define MRV_FILT_FILT_THRESH_BL1_SHIFT 0

    // Register: isp_filt_thresh_sh0: Sharpening threshold 0 (0x0030)
    // Slice: filt_thresh_sh0:
    // If filt_thresh_sh0 < sum_grad < filt_thresh_sh1 then filt_thresh_sh0 is selected
    #define MRV_FILT_FILT_THRESH_SH0
    #define MRV_FILT_FILT_THRESH_SH0_MASK 0x000003FF
    #define MRV_FILT_FILT_THRESH_SH0_SHIFT 0

    // Register: isp_filt_thresh_sh1: Sharpening threshold 1 (0x0034)
    // Slice: filt_thresh_sh1:
    // If filt_thresh_sh1 < sum_grad then filt_thresh_sh1 is selected
    #define MRV_FILT_FILT_THRESH_SH1
    #define MRV_FILT_FILT_THRESH_SH1_MASK 0x000003FF
    #define MRV_FILT_FILT_THRESH_SH1_SHIFT 0

    // Register: isp_filt_lum_weight: Parameters for luminance weight function (0x0038)
    // Slice: lum_weight_gain:
    // Gain select of luminance weight function
    #define MRV_FILT_LUM_WEIGHT_GAIN
    #define MRV_FILT_LUM_WEIGHT_GAIN_MASK 0x00070000
    #define MRV_FILT_LUM_WEIGHT_GAIN_SHIFT 16
    // Slice: lum_weight_kink:
    // Kink position of luminance weight function
    #define MRV_FILT_LUM_WEIGHT_KINK
    #define MRV_FILT_LUM_WEIGHT_KINK_MASK 0x0000FF00
    #define MRV_FILT_LUM_WEIGHT_KINK_SHIFT 8
    // Slice: lum_weight_min:
    // Minimum value of luminance weight function
    #define MRV_FILT_LUM_WEIGHT_MIN
    #define MRV_FILT_LUM_WEIGHT_MIN_MASK 0x000000FF
    #define MRV_FILT_LUM_WEIGHT_MIN_SHIFT 0

    // Register: isp_filt_fac_sh1: filter factor sharp1 (0x003c)
    // Slice: filt_fac_sh1:
    // Filter factor for sharp1 level
    #define MRV_FILT_FILT_FAC_SH1
    #define MRV_FILT_FILT_FAC_SH1_MASK 0x0000003F
    #define MRV_FILT_FILT_FAC_SH1_SHIFT 0

    // Register: isp_filt_fac_sh0: filter factor sharp0 (0x0040)
    // Slice: filt_fac_sh0:
    // Filter factor for sharp0 level
    #define MRV_FILT_FILT_FAC_SH0
    #define MRV_FILT_FILT_FAC_SH0_MASK 0x0000003F
    #define MRV_FILT_FILT_FAC_SH0_SHIFT 0

    // Register: isp_filt_fac_mid: filter factor midle (0x0044)
    // Slice: filt_fac_mid:
    // Filter factor for mid level and for static filter mode
    #define MRV_FILT_FILT_FAC_MID
    #define MRV_FILT_FILT_FAC_MID_MASK 0x0000003F
    #define MRV_FILT_FILT_FAC_MID_SHIFT 0

    // Register: isp_filt_fac_bl0: Parameter for blur 0 filter (0x0048)
    // Slice: filt_fac_bl0:
    // Filter factor for blur 0 level
    #define MRV_FILT_FILT_FAC_BL0
    #define MRV_FILT_FILT_FAC_BL0_MASK 0x0000003F
    #define MRV_FILT_FILT_FAC_BL0_SHIFT 0

    // Register: isp_filt_fac_bl1: Parameter for blur 1 filter (0x004c)
    // Slice: filt_fac_bl1:
    // Filter factor for blur 1 level (max blur)
    #define MRV_FILT_FILT_FAC_BL1
    #define MRV_FILT_FILT_FAC_BL1_MASK 0x0000003F
    #define MRV_FILT_FILT_FAC_BL1_SHIFT 0
#endif  // MARVIN_FEATURE_ISP_FILTER

/********************************************************************/
/* ISP Chromatic Aberration Correction Registers                    */
/********************************************************************/

#if (MARVIN_FEATURE_CHROM_ABERRATION == MARVIN_FEATURE_EXIST)
    // Register: isp_cac_ctrl: Control register for chromatic aberration correction (0x0000)
    // Slice: h_clip_mode:
    // Defines the maximum red/blue pixel shift in horizontal direction At pixel positions, that require a larger displacement, the maximum shift value is used instead (vector clipping)
    // 0: Set horizontal vector clipping to +/-4 pixel displacement (Default)
    // 1: Set horizontal vector clipping to +/-4 or +/-5 pixel displacement depending on pixel position inside the Bayer raster (dynamic switching between +/-4 and +/-5)
    #define MRV_CAC_H_CLIP_MODE
    #define MRV_CAC_H_CLIP_MODE_MASK 0x00000008
    #define MRV_CAC_H_CLIP_MODE_SHIFT 3
    #define MRV_CAC_H_CLIP_MODE_FIX4 // 0: Set horizontal vector clipping to +/-4 pixel displacement (Default)
    #define MRV_CAC_H_CLIP_MODE_DYN5 // 1: Set horizontal vector clipping to +/-4 or +/-5 pixel displacement depending on pixel position inside the Bayer raster (dynamic switching between +/-4 and +/-5)
    // Slice: v_clip_mode:
    // Defines the maximum red/blue pixel shift in vertical direction
    // 00: Set vertical vector clipping to +/-2 pixel ; fix filter_enable (Default)
    // 01: Set vertical vector clipping to +/-3 pixel; dynamic filter_enable for chroma low pass filter
    // 10: Set vertical vector clipping +/-3 or +/-4 pixel displacement depending on pixel position inside the Bayer raster (dynamic switching between +/-3 and +/-4)
    // 11: reserved
    #define MRV_CAC_V_CLIP_MODE
    #define MRV_CAC_V_CLIP_MODE_MASK 0x00000006
    #define MRV_CAC_V_CLIP_MODE_SHIFT 1
    #define MRV_CAC_V_CLIP_MODE_FIX2 // 00: Set vertical vector clipping to +/-2 pixel ; fix filter_enable (Default)
    #define MRV_CAC_V_CLIP_MODE_FIX3 // 01: Set vertical vector clipping to +/-3 pixel; dynamic filter_enable for chroma low pass filter
    #define MRV_CAC_V_CLIP_MODE_DYN4 // 10: Set vertical vector clipping +/-3 or +/-4 pixel displacement depending on pixel position inside the Bayer raster (dynamic switching between +/-3 and +/-4)
    // Slice: cac_en:
    // 0: chromatic aberration correction off
    // 1: chromatic aberration correction on
    #define MRV_CAC_CAC_EN
    #define MRV_CAC_CAC_EN_MASK 0x00000001
    #define MRV_CAC_CAC_EN_SHIFT 0

    // Register: isp_cac_count_start: Preload values for CAC pixel and line counter (0x0004)
    // Slice: v_count_start:
    // 12 bit v_count preload value (range 4095 ... 1) of the vertical CAC line counter. Before frame start v_count has to be preloaded with (v_size/2 + v_center_offset), with
    // v_size the image height and
    // v_center_offset the vertical distance between image center and optical center.
    //
    // After frame start the v_count decrements with every line until a value of zero is reached for the line in the optical center. Than the v_sign bit toggles and the v_counter decrements with every line until end of frame.
    #define MRV_CAC_V_COUNT_START
    #define MRV_CAC_V_COUNT_START_MASK 0x0FFF0000
    #define MRV_CAC_V_COUNT_START_SHIFT 16
    #define MRV_CAC_V_COUNT_START_MIN (1)
    #define MRV_CAC_V_COUNT_START_MAX (MRV_CAC_V_COUNT_START_MASK >> MRV_CAC_V_COUNT_START_SHIFT)
    // Slice: h_count_start:
    // 12 bit h_count preload value (range 4095 .. 1) of the horizontal CAC pixel counter. Before line start h_count has to be preloaded with (h_size/2 + h_center_offset), with
    // h_size the image width and
    // h_center_offset the horizontal distance between image center and optical center.
    //
    // After line start the h_count decrements with every pixel until a value of zero is reached for the column in the optical center. Than the h_sign bit toggles and the h_counter increments with every pixel until end of line.
    #define MRV_CAC_H_COUNT_START
    #define MRV_CAC_H_COUNT_START_MASK 0x00000FFF
    #define MRV_CAC_H_COUNT_START_SHIFT 0
    #define MRV_CAC_H_COUNT_START_MIN (1)
    #define MRV_CAC_H_COUNT_START_MAX (MRV_CAC_H_COUNT_START_MASK >> MRV_CAC_H_COUNT_START_SHIFT)

    // Register: isp_cac_a: Linear Parameters for radial shift calculation (0x0008)
    // Slice: A_Blue:
    // Parameter A_Blue for radial blue shift calculation, according to
    //(A_Blue * r  + B_Blue * r^2 + C_Blue * r^3).
    // It is a 9 bit twos complement integer with 4 fractional digits value and value range from -16 up to 15.9375.
    #define MRV_CAC_A_BLUE
    #define MRV_CAC_A_BLUE_MASK 0x01FF0000
    #define MRV_CAC_A_BLUE_SHIFT 16
    #define MRV_CAC_A_BLUE_MAX (MRV_CAC_A_BLUE_MASK >> (MRV_CAC_A_BLUE_SHIFT + 1))
    #define MRV_CAC_A_BLUE_MIN (~ MRV_CAC_A_BLUE_MAX)
    // Slice: A_Red:
    // Parameter A_Red for radial red shift calculation, according to
    //(A_Red * r + B_Red * r^2 + C_Red * r^3).
    // It is a 9 bit twos complement integer with 4 fractional digits value and value range from -16 up to 15.9375.
    #define MRV_CAC_A_RED
    #define MRV_CAC_A_RED_MASK 0x000001FF
    #define MRV_CAC_A_RED_SHIFT 0
    #define MRV_CAC_A_RED_MAX (MRV_CAC_A_RED_MASK >> (MRV_CAC_A_RED_SHIFT + 1))
    #define MRV_CAC_A_RED_MIN (~ MRV_CAC_A_RED_MAX)

    // Register: isp_cac_b: Square Parameters for radial shift calculation (0x000c)
    // Slice: B_Blue:
    // Parameter B_Blue for radial blue shift calculation, according to
    //(A_Blue * r  + B_Blue * r^2 + C_Blue * r^3).
    // It is a 9 bit twos complement integer with 4 fractional digits value and value range from -16 up to 15.9375.
    #define MRV_CAC_B_BLUE
    #define MRV_CAC_B_BLUE_MASK 0x01FF0000
    #define MRV_CAC_B_BLUE_SHIFT 16
    #define MRV_CAC_B_BLUE_MAX (MRV_CAC_B_BLUE_MASK >> (MRV_CAC_B_BLUE_SHIFT + 1))
    #define MRV_CAC_B_BLUE_MIN (~ MRV_CAC_B_BLUE_MAX)
    // Slice: B_Red:
    // Parameter B_Red for radial red shift calculation, according to
    //(A_Red * r + B_Red * r^2 + C_Red * r^3).
    // It is a 9 bit twos complement integer with 4 fractional digits value and value range from -16 up to 15.9375.
    #define MRV_CAC_B_RED
    #define MRV_CAC_B_RED_MASK 0x000001FF
    #define MRV_CAC_B_RED_SHIFT 0
    #define MRV_CAC_B_RED_MAX (MRV_CAC_B_RED_MASK >> (MRV_CAC_B_RED_SHIFT + 1))
    #define MRV_CAC_B_RED_MIN (~ MRV_CAC_B_RED_MAX)

    // Register: isp_cac_c: Cubical Parameters for radial shift calculation (0x0010)
    // Slice: C_Blue:
    // Parameter C_Blue for radial blue shift calculation, according to
    //(A_Blue * r  + B_Blue * r^2 + C_Blue * r^3).
    // It is a 9 bit twos complement integer with 4 fractional digits value and value range from -16 up to 15.9375.
    #define MRV_CAC_C_BLUE
    #define MRV_CAC_C_BLUE_MASK 0x01FF0000
    #define MRV_CAC_C_BLUE_SHIFT 16
    #define MRV_CAC_C_BLUE_MAX (MRV_CAC_C_BLUE_MASK >> (MRV_CAC_C_BLUE_SHIFT + 1))
    #define MRV_CAC_C_BLUE_MIN (~ MRV_CAC_C_BLUE_MAX)
    // Slice: C_Red:
    // Parameter C_Red for radial red shift calculation, according to
    //(A_Red * r + B_Red * r^2 + C_Red * r^3).
    // It is a 9 bit twos complement integer with 4 fractional digits value and value range from -16 up to 15.9375.
    #define MRV_CAC_C_RED
    #define MRV_CAC_C_RED_MASK 0x000001FF
    #define MRV_CAC_C_RED_SHIFT 0
    #define MRV_CAC_C_RED_MAX (MRV_CAC_C_RED_MASK >> (MRV_CAC_C_RED_SHIFT + 1))
    #define MRV_CAC_C_RED_MIN (~ MRV_CAC_C_RED_MAX)

    // Register: isp_cac_x_norm: Normalization parameters for calculation of image coordinate x_d relative to optical center (0x0014)
    // Slice: x_ns:
    // Horizontal normalization shift parameter x_ns (3 bit unsigned integer) in equation
    // x_d[7:0] = (((h_count <% 4) %> x_ns) * x_normfactor) >> 5
    #define MRV_CAC_X_NS
    #define MRV_CAC_X_NS_MASK 0x00070000
    #define MRV_CAC_X_NS_SHIFT 16
    #define MRV_CAC_X_NS_MIN (0)
    #define MRV_CAC_X_NS_MAX (MRV_CAC_X_NS_MASK >> MRV_CAC_X_NS_SHIFT)
    // Slice: x_normfactor:
    // Horizontal scaling factor x_normfactor (5 bit unsigned integer) range 16 .. 31 in equation
    // x_d[7:0] = (((h_count <% 4) %> x_ns) * x_normfactor) >> 5
    #define MRV_CAC_X_NORMFACTOR
    #define MRV_CAC_X_NORMFACTOR_MASK 0x0000001F
    #define MRV_CAC_X_NORMFACTOR_SHIFT 0
    #define MRV_CAC_X_NORMFACTOR_MIN (0)
    #define MRV_CAC_X_NORMFACTOR_MAX (31)

    // Register: isp_cac_y_norm: Normalization parameters for calculation of image coordinate y_d relative to optical center (0x0018)
    // Slice: y_ns:
    // Vertical normalization shift parameter y_ns (3 bit unsigned integer) in equation
    // y_d[7:0] = (((v_count <% 4) %> y_ns) * y_normfactor) >> 5
    #define MRV_CAC_Y_NS
    #define MRV_CAC_Y_NS_MASK 0x00070000
    #define MRV_CAC_Y_NS_SHIFT 16
    #define MRV_CAC_Y_NS_MIN (0)
    #define MRV_CAC_Y_NS_MAX (MRV_CAC_Y_NS_MASK >> MRV_CAC_Y_NS_SHIFT)
    // Slice: y_normfactor:
    // Vertical scaling factor y_normfactor (5 bit unsigned integer) range 16 .. 31 in equation
    // y_d[7:0] = (((v_count <% 4) %> y_ns) * y_normfactor) >> 5
    #define MRV_CAC_Y_NORMFACTOR
    #define MRV_CAC_Y_NORMFACTOR_MASK 0x0000001F
    #define MRV_CAC_Y_NORMFACTOR_SHIFT 0
    #define MRV_CAC_Y_NORMFACTOR_MIN (0)
    #define MRV_CAC_Y_NORMFACTOR_MAX (31)
#endif // #if (MARVIN_FEATURE_CHROM_ABERRATION == MARVIN_FEATURE_EXIST)

/********************************************************************/
/* ISP Exposure Measurement Registers                               */
/********************************************************************/

#if(   ( MARVIN_FEATURE_AUTO_EXPOSURE == MARVIN_FEATURE_AUTO_EXPOSURE_V2 )\
    || ( MARVIN_FEATURE_AUTO_EXPOSURE == MARVIN_FEATURE_AUTO_EXPOSURE_V4 ) )
    // Register: isp_exp_ctrl: Exposure control (0x0000)
    //#define MRV_AE_EXP_MEAS_MODE        (0x80000000)    // [31] exposure measurement mode
    //#define MRV_ISP_EXP_AUTO_STOP       (0x00000002)    // [ 1] exposure measurement automatic stop
    //#define MRV_ISP_EXP_START           (0x00000001)    // [ 0] exposure measurement start

#if( MARVIN_FEATURE_AUTO_EXPOSURE == MARVIN_FEATURE_AUTO_EXPOSURE_V4 )
    // Slice: exp_meas_mode:
    // '1' luminance calculation according to
    // Y=(R+G+B) x 0.332 (85/256)    (M5_v6 only)
    // '0' luminance calculation according to Y=16+0.25R+0.5G+0.1094B
    #define MRV_AE_EXP_MEAS_MODE
    #define MRV_AE_EXP_MEAS_MODE_MASK 0x80000000
    #define MRV_AE_EXP_MEAS_MODE_SHIFT 31
#endif

    // Slice: autostop:
    // ‘1’ stop measuring after a complete frame
    // ‘0’ continous measurement
    #define MRV_AE_AUTOSTOP
    #define MRV_AE_AUTOSTOP_MASK 0x00000002
    #define MRV_AE_AUTOSTOP_SHIFT 1
    // Slice: exp_start:
    // ‘1’ start measuring a frame. The exp block will reset this bit and halt after completing one frame, if bit “autostop” is set to ‘1’.
    #define MRV_AE_EXP_START
    #define MRV_AE_EXP_START_MASK 0x00000001
    #define MRV_AE_EXP_START_SHIFT 0

    // Register: isp_exp_h_offset: Horizontal offset for first block (0x0004)
    //#define MRV_ISP_EXP_MAX_HOFFSET     (0x00000FFF)    // mask for used bits
    // Slice: isp_exp_h_offset:
    // Horizontal offset of first block in pixels.
    // 0 <= value <= 3920
    #define MRV_AE_ISP_EXP_H_OFFSET
    #define MRV_AE_ISP_EXP_H_OFFSET_MASK 0x00000FFF
    #define MRV_AE_ISP_EXP_H_OFFSET_SHIFT 0
    #define MRV_AE_ISP_EXP_H_OFFSET_MIN  0x00000000
    #define MRV_AE_ISP_EXP_H_OFFSET_MAX  0x00000F50 //4096-176

    // Register: isp_exp_v_offset: Vertical offset for first block (0x0008)
    //#define MRV_ISP_EXP_MAX_VOFFSET     (0x00000FFF)    // mask for used bits
    // Slice: isp_exp_v_offset:
    // Vertical offset of first block in pixels.
    // 0 <= value <= 1806
    #define MRV_AE_ISP_EXP_V_OFFSET
    #define MRV_AE_ISP_EXP_V_OFFSET_MASK 0x00000FFF
    #define MRV_AE_ISP_EXP_V_OFFSET_SHIFT 0
    #define MRV_AE_ISP_EXP_V_OFFSET_MIN  0x00000000
    #define MRV_AE_ISP_EXP_V_OFFSET_MAX  0x00000B74 //3072-140

    // Register: isp_exp_h_size: Horizontal size of one block (0x000c)
    //#define MRV_ISP_EXP_WIDTH_MASK      (0x000003FF)    // mask for used bits
    // Slice: isp_exp_h_size:
    // Horizontal size in pixels of one block.
    // 35 <= value <= 819
    #define MRV_AE_ISP_EXP_H_SIZE
    #define MRV_AE_ISP_EXP_H_SIZE_MASK 0x000003FF
    #define MRV_AE_ISP_EXP_H_SIZE_SHIFT 0
    #define MRV_AE_ISP_EXP_H_SIZE_MIN  0x00000023
    #define MRV_AE_ISP_EXP_H_SIZE_MAX  0x00000333

    // Register: isp_exp_v_size: Vertical size of one block (0x0010)
    //#define MRV_ISP_EXP_HEIGHT_MASK     (0x000003FE)    // mask for used bits
    // Slice: isp_exp_v_size:
    // Vertical size in pixels of one block.
    // 28 <= value <= 614
    // Only even values are allowed. Vertical size must be a multiple of 2.
    #define MRV_AE_ISP_EXP_V_SIZE
    #define MRV_AE_ISP_EXP_V_SIZE_MASK 0x000003FE
    #define MRV_AE_ISP_EXP_V_SIZE_SHIFT 0
    #define MRV_AE_ISP_EXP_V_SIZE_VALID_MASK (MRV_AE_ISP_EXP_V_SIZE_MASK & ~0x00000001)
    #define MRV_AE_ISP_EXP_V_SIZE_MIN  0x0000001C
    #define MRV_AE_ISP_EXP_V_SIZE_MAX  0x00000266

    //#define MRV_ISP_EXP_BITMASK_LUMA    (0x000000FF)    // mask for used bits
    // Register: isp_exp_mean_XY: Mean luminance value of block XY (0x0014..0x0074)
    // Array definition:
    //   X: SIZE1, OFS1
    //   Y: SIZE2, OFS2
    #define MRV_AE_ISP_EXP_MEAN_ARR_SIZE1 5
    #define MRV_AE_ISP_EXP_MEAN_ARR_SIZE2 5
    #define MRV_AE_ISP_EXP_MEAN_ARR_OFS1  1
    #define MRV_AE_ISP_EXP_MEAN_ARR_OFS2  MRV_AE_ISP_EXP_MEAN_ARR_SIZE1
    // Slice: isp_exp_mean:
    // Mean luminance value of one block
    #define MRV_AE_ISP_EXP_MEAN
    #define MRV_AE_ISP_EXP_MEAN_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_SHIFT 0

    // Register: isp_exp_mean_00: Mean luminance value of block 00 (0x0014)
    // Slice: isp_exp_mean_00:
    // Mean luminance value of block 00 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_00
    #define MRV_AE_ISP_EXP_MEAN_00_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_00_SHIFT 0

    // Register: isp_exp_mean_10: Mean luminance value of block 10 (0x0018)
    // Slice: isp_exp_mean_10:
    // Mean luminance value of block 10 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_10
    #define MRV_AE_ISP_EXP_MEAN_10_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_10_SHIFT 0

    // Register: isp_exp_mean_20: Mean luminance value of block 20 (0x001c)
    // Slice: isp_exp_mean_20:
    // Mean luminance value of block 20 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_20
    #define MRV_AE_ISP_EXP_MEAN_20_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_20_SHIFT 0

    // Register: isp_exp_mean_30: Mean luminance value of block 30 (0x0020)
    // Slice: isp_exp_mean_30:
    // Mean luminance value of block 30 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_30
    #define MRV_AE_ISP_EXP_MEAN_30_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_30_SHIFT 0

    // Register: isp_exp_mean_40: Mean luminance value of block 40 (0x0024)
    // Slice: isp_exp_mean_40:
    // Mean luminance value of block 40 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_40
    #define MRV_AE_ISP_EXP_MEAN_40_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_40_SHIFT 0

    // Register: isp_exp_mean_01: Mean luminance value of block 01 (0x0028)
    // Slice: isp_exp_mean_01:
    // Mean luminance value of block 01 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_01
    #define MRV_AE_ISP_EXP_MEAN_01_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_01_SHIFT 0

    // Register: isp_exp_mean_11: Mean luminance value of block 11 (0x002c)
    // Slice: isp_exp_mean_11:
    // Mean luminance value of block 11 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_11
    #define MRV_AE_ISP_EXP_MEAN_11_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_11_SHIFT 0

    // Register: isp_exp_mean_21: Mean luminance value of block 21 (0x0030)
    // Slice: isp_exp_mean_21:
    // Mean luminance value of block 21 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_21
    #define MRV_AE_ISP_EXP_MEAN_21_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_21_SHIFT 0

    // Register: isp_exp_mean_31: Mean luminance value of block 31 (0x0034)
    // Slice: isp_exp_mean_31:
    // Mean luminance value of block 31 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_31
    #define MRV_AE_ISP_EXP_MEAN_31_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_31_SHIFT 0

    // Register: isp_exp_mean_41: Mean luminance value of block 41 (0x0038)
    // Slice: isp_exp_mean_41:
    // Mean luminance value of block 41 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_41
    #define MRV_AE_ISP_EXP_MEAN_41_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_41_SHIFT 0

    // Register: isp_exp_mean_02: Mean luminance value of block 02 (0x003c)
    // Slice: isp_exp_mean_02:
    // Mean luminance value of block 02 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_02
    #define MRV_AE_ISP_EXP_MEAN_02_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_02_SHIFT 0

    // Register: isp_exp_mean_12: Mean luminance value of block 12 (0x0040)
    // Slice: isp_exp_mean_12:
    // Mean luminance value of block 12 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_12
    #define MRV_AE_ISP_EXP_MEAN_12_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_12_SHIFT 0

    // Register: isp_exp_mean_22: Mean luminance value of block 22 (0x0044)
    // Slice: isp_exp_mean_22:
    // Mean luminance value of block 22 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_22
    #define MRV_AE_ISP_EXP_MEAN_22_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_22_SHIFT 0

    // Register: isp_exp_mean_32: Mean luminance value of block 32 (0x0048)
    // Slice: isp_exp_mean_32:
    // Mean luminance value of block 32 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_32
    #define MRV_AE_ISP_EXP_MEAN_32_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_32_SHIFT 0

    // Register: isp_exp_mean_42: Mean luminance value of block 42 (0x004c)
    // Slice: isp_exp_mean_42:
    // Mean luminance value of block 42 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_42
    #define MRV_AE_ISP_EXP_MEAN_42_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_42_SHIFT 0

    // Register: isp_exp_mean_03: Mean luminance value of block 03 (0x0050)
    // Slice: isp_exp_mean_03:
    // Mean luminance value of block 03 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_03
    #define MRV_AE_ISP_EXP_MEAN_03_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_03_SHIFT 0

    // Register: isp_exp_mean_13: Mean luminance value of block 13 (0x0054)
    // Slice: isp_exp_mean_13:
    // Mean luminance value of block 13 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_13
    #define MRV_AE_ISP_EXP_MEAN_13_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_13_SHIFT 0

    // Register: isp_exp_mean_23: Mean luminance value of block 23 (0x0058)
    // Slice: isp_exp_mean_23:
    // Mean luminance value of block 23 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_23
    #define MRV_AE_ISP_EXP_MEAN_23_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_23_SHIFT 0

    // Register: isp_exp_mean_33: Mean luminance value of block 33 (0x005c)
    // Slice: isp_exp_mean_33:
    // Mean luminance value of block 33 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_33
    #define MRV_AE_ISP_EXP_MEAN_33_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_33_SHIFT 0

    // Register: isp_exp_mean_43: Mean luminance value of block 43 (0x0060)
    // Slice: isp_exp_mean_43:
    // Mean luminance value of block 43 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_43
    #define MRV_AE_ISP_EXP_MEAN_43_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_43_SHIFT 0

    // Register: isp_exp_mean_04: Mean luminance value of block 04 (0x0064)
    // Slice: isp_exp_mean_04:
    // Mean luminance value of block 04 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_04
    #define MRV_AE_ISP_EXP_MEAN_04_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_04_SHIFT 0

    // Register: isp_exp_mean_14: Mean luminance value of block 14 (0x0068)
    // Slice: isp_exp_mean_14:
    // Mean luminance value of block 14 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_14
    #define MRV_AE_ISP_EXP_MEAN_14_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_14_SHIFT 0

    // Register: isp_exp_mean_24: Mean luminance value of block 24 (0x006c)
    // Slice: isp_exp_mean_24:
    // Mean luminance value of block 24 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_24
    #define MRV_AE_ISP_EXP_MEAN_24_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_24_SHIFT 0

    // Register: isp_exp_mean_34: Mean luminance value of block 34 (0x0070)
    // Slice: isp_exp_mean_34:
    // Mean luminance value of block 34 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_34
    #define MRV_AE_ISP_EXP_MEAN_34_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_34_SHIFT 0

    // Register: isp_exp_mean_44: Mean luminance value of block 44 (0x0074)
    // Slice: isp_exp_mean_44:
    // Mean luminance value of block 44 (x,y)
    #define MRV_AE_ISP_EXP_MEAN_44
    #define MRV_AE_ISP_EXP_MEAN_44_MASK 0x000000FF
    #define MRV_AE_ISP_EXP_MEAN_44_SHIFT 0
#endif  //MARVIN_FEATURE_AUTO_EXPOSURE

/********************************************************************/
/* ISP Black Level Subtraction Registers                            */
/********************************************************************/

#if ((MARVIN_FEATURE_BLACK_LEVEL >= MARVIN_FEATURE_BLACK_LEVEL_V1) && \
     (MARVIN_FEATURE_BLACK_LEVEL <= MARVIN_FEATURE_BLACK_LEVEL_V3))
    // Register: isp_bls_prop: black level subtraction properties (FR_BASE + 0x04F0)
    //#define MRV_BLS_AUTO             0x00000008 //[3] rw 1: use measured black value for subtraction
    //#define MRV_BLS_V_ENABLE         0x00000002 //[1] rw enable vertical accumulation for mean black value
    //#define MRV_BLS_H_ENABLE         0x00000001 //[0] rw enable horizontal accumulation for mean black value
    // Slice: BLS_MODE:
    // 1: subtract measured values
    // 0: subtract fixed values
    #define MRV_BLS_BLS_MODE
    #define MRV_BLS_BLS_MODE_MASK 0x00000008
    #define MRV_BLS_BLS_MODE_SHIFT 3
    #define MRV_BLS_BLS_MODE_MEAS  1 // 1: subtract measured values
    #define MRV_BLS_BLS_MODE_FIX   0 // 0: subtract fixed values
    // Slice: BLS_V_ENABLE:
    // 1: enable vertical accumulation for mean black value
    // 0: bypass the black level processing
    #define MRV_BLS_BLS_V_ENABLE
    #define MRV_BLS_BLS_V_ENABLE_MASK 0x00000002
    #define MRV_BLS_BLS_V_ENABLE_SHIFT 1
    // Slice: BLS_H_ENABLE:
    // 1: enable horicontal accumulation for mean black value
    // 0: bypass the black level processing
    #define MRV_BLS_BLS_H_ENABLE
    #define MRV_BLS_BLS_H_ENABLE_MASK 0x00000001
    #define MRV_BLS_BLS_H_ENABLE_SHIFT 0
#elif ((MARVIN_FEATURE_BLACK_LEVEL >= MARVIN_FEATURE_BLACK_LEVEL_V4) && \
       (MARVIN_FEATURE_BLACK_LEVEL <= MARVIN_FEATURE_BLACK_LEVEL_V5))
    // Register: isp_bls_ctrl: global control register (0x0000)
    //#define MRV_ISP_BLS_WIN_MASK        (0x0000000C)    // [3:2] rw window mask
    //#define MRV_ISP_BLS_WIN_2_ENABLE    (0x00000008)    //     "10": only window 2 is measured
    //#define MRV_ISP_BLS_WIN_1_ENABLE    (0x00000004)    //     "01": only window 1 is measured
    //#define MRV_ISP_BLS_MODE            (0x00000002)    // [1] rw subtraction mode
    //#define MRV_ISP_BLS_EN              (0x00000001)    // [0] rw en-/disable black level processing
    // Slice: WINDOW_ENABLE:
    // 0: no measuring is performed
    // 1: only window 1 is measured
    // 2: only window 2 is measured
    // 3: both windows are measured
    #define MRV_BLS_WINDOW_ENABLE
    #define MRV_BLS_WINDOW_ENABLE_MASK 0x0000000C
    #define MRV_BLS_WINDOW_ENABLE_SHIFT 2
    #define MRV_BLS_WINDOW_ENABLE_NONE  0 // 0: no measuring is performed
    #define MRV_BLS_WINDOW_ENABLE_WND1  1 // 1: only window 1 is measured
    #define MRV_BLS_WINDOW_ENABLE_WND2  2 // 2: only window 2 is measured
    #define MRV_BLS_WINDOW_ENABLE_BOTH  3 // 3: both windows are measured
    // Slice: BLS_MODE:
    // 1: subtract measured values
    // 0: subtract fixed values
    #define MRV_BLS_BLS_MODE
    #define MRV_BLS_BLS_MODE_MASK 0x00000002
    #define MRV_BLS_BLS_MODE_SHIFT 1
    #define MRV_BLS_BLS_MODE_MEAS  1 // 1: subtract measured values
    #define MRV_BLS_BLS_MODE_FIX   0 // 0: subtract fixed values
    // Slice: BLS_ENABLE:
    // 1: black level subtraction is enabled
    // 0: bypass the black level processing
    #define MRV_BLS_BLS_ENABLE
    #define MRV_BLS_BLS_ENABLE_MASK 0x00000001
    #define MRV_BLS_BLS_ENABLE_SHIFT 0
#endif  // MARVIN_FEATURE_BLACK_LEVEL

// Register: isp_bls_samples: samples register (0x0004)
// Slice: BLS_SAMPLES:
// This number to the power of two gives the number of measure samples for each Bayer position. Range 0x00: 2^0=1  to  0x12: 2^18=262144. This number is also the divider for the accumulator for each Bayer position.
// The accumulation will be stopped, if the number of measured pixels for the current Bayer position is equal to the number of samples.
// The measure windows must be positioned that way that the number of included pixels of each Bayer position included by both windows is equal or greater than the number of measure samples calculated by 2^BLS_SAMPLES !
// NOTE: The number of pixels of one Bayer position is 1/4 of the number of all Pixels included by the measure windows.
#define MRV_BLS_BLS_SAMPLES
#define MRV_BLS_BLS_SAMPLES_MASK 0x0000001F
#define MRV_BLS_BLS_SAMPLES_SHIFT 0
#if (MARVIN_FEATURE_BLACK_LEVEL == MARVIN_FEATURE_BLACK_LEVEL_V1)              // 4-bit
    #define MRV_BLS_BLS_SAMPLES_MAX     (0x0000000F)    // maximum is 2^14 = 16384
#elif (MARVIN_FEATURE_BLACK_LEVEL == MARVIN_FEATURE_BLACK_LEVEL_V2)            // 4-bit
    #define MRV_BLS_BLS_SAMPLES_MAX     (0x0000000F)    // maximum is 2^14 = 16384
#elif (MARVIN_FEATURE_BLACK_LEVEL == MARVIN_FEATURE_BLACK_LEVEL_V3)            // 4-bit
    #define MRV_BLS_BLS_SAMPLES_MAX     (0x0000000F)    // maximum is 2^14 = 16384
#elif (MARVIN_FEATURE_BLACK_LEVEL == MARVIN_FEATURE_BLACK_LEVEL_V4)            // 5-bit
    #define MRV_BLS_BLS_SAMPLES_MAX     (0x00000014)    // maximum is 2^20 = 1048576
#elif (MARVIN_FEATURE_BLACK_LEVEL == MARVIN_FEATURE_BLACK_LEVEL_V5)            // 5-bit
    #define MRV_BLS_BLS_SAMPLES_MAX     (0x00000014)    // maximum is 2^20 = 1048576
#endif  // MARVIN_FEATURE_BLACK_LEVEL

#if ((MARVIN_FEATURE_BLACK_LEVEL >= MARVIN_FEATURE_BLACK_LEVEL_V1) && \
     (MARVIN_FEATURE_BLACK_LEVEL <= MARVIN_FEATURE_BLACK_LEVEL_V3))
    // Register: isp_bls_h_window: bls horizontal window (SFR_BASE + 4F4H)[Reset value: 0000’0000H]
    //#define MRV_BLS_H_START          0x00000FFF //[11:0] rw horizontal black window start position
    //#define MRV_BLS_H_STOP           0x0FFF0000 //[27:16] rw horizontal black window stop position
    // Slice: BLS_H_START:
    // Black pixel window horizontal start position
    #define MRV_BLS_BLS_H_START
    #define MRV_BLS_BLS_H_START_MASK 0x00000FFF
    #define MRV_BLS_BLS_H_START_SHIFT 0
    #define MRV_BLS_BLS_H_START_MAX (MRV_BLS_BLS_H_START_MASK >> MRV_BLS_BLS_H_START_SHIFT)
    // Slice: BLS_H_STOP:
    // Black pixel window horizontal stop position
    #define MRV_BLS_BLS_H_STOP
    #define MRV_BLS_BLS_H_STOP_MASK 0x0FFF0000
    #define MRV_BLS_BLS_H_STOP_SHIFT 16
    #define MRV_BLS_BLS_H_STOP_MAX (MRV_BLS_BLS_H_STOP_MASK >> MRV_BLS_BLS_H_STOP_SHIFT)

    // Register: isp_bls_v_window: bls vertical window (SFR_BASE + 4F8H)[Reset value: 0000’0000H]
    //#define MRV_BLS_V_START          0x00000FFF //[11:0] rw vertical black window start position
    //#define MRV_BLS_V_STOP           0x0FFF0000 //[27:16] rw vertical black window stop position
    // Slice: BLS_V_START:
    // Black pixel window vertical start position
    #define MRV_BLS_BLS_V_START
    #define MRV_BLS_BLS_V_START_MASK 0x00000FFF
    #define MRV_BLS_BLS_V_START_SHIFT 0
    #define MRV_BLS_BLS_V_START_MAX (MRV_BLS_BLS_V_START_MASK >> MRV_BLS_BLS_V_START_SHIFT)
    // Slice: BLS_V_STOP:
    // Black pixel window vertical stop position
    #define MRV_BLS_BLS_V_STOP
    #define MRV_BLS_BLS_V_STOP_MASK 0x0FFF0000
    #define MRV_BLS_BLS_V_STOP_SHIFT 16
    #define MRV_BLS_BLS_V_STOP_MAX (MRV_BLS_BLS_V_STOP_MASK >> MRV_BLS_BLS_V_STOP_SHIFT)
#elif ((MARVIN_FEATURE_BLACK_LEVEL >= MARVIN_FEATURE_BLACK_LEVEL_V4) && \
       (MARVIN_FEATURE_BLACK_LEVEL <= MARVIN_FEATURE_BLACK_LEVEL_V5))
    //#define MRV_ISP_BLS_START_H_MAX     (0x00000FFF)    // maximum value for horizontal start address
    //#define MRV_ISP_BLS_STOP_H_MAX      (0x00000FFF)    // maximum value for horizontal stop address
    //#define MRV_ISP_BLS_START_V_MAX     (0x00000FFF)    // maximum value for vertical start address
    //#define MRV_ISP_BLS_STOP_V_MAX      (0x00000FFF)    // maximum value for vertical stop address

    // Register: isp_bls_h1_start: window 1 horizontal start (0x0008)
    // Slice: BLS_H1_START:
    // Black pixel window 1 horizontal start position
    #define MRV_BLS_BLS_H1_START
    #define MRV_BLS_BLS_H1_START_MASK 0x00000FFF
    #define MRV_BLS_BLS_H1_START_SHIFT 0
    #define MRV_BLS_BLS_H1_START_MAX (MRV_BLS_BLS_H1_START_MASK >> MRV_BLS_BLS_H1_START_SHIFT)

    // Register: isp_bls_h1_stop: window 1 horizontal stop (0x000c)
    // Slice: BLS_H1_STOP:
    // Black pixel window 1 horizontal stop position
    #define MRV_BLS_BLS_H1_STOP
    #define MRV_BLS_BLS_H1_STOP_MASK 0x00001FFF
    #define MRV_BLS_BLS_H1_STOP_SHIFT 0
    #define MRV_BLS_BLS_H1_STOP_MAX (MRV_BLS_BLS_H1_STOP_MASK >> MRV_BLS_BLS_H1_STOP_SHIFT)

    // Register: isp_bls_v1_start: window 1 vertical start (0x0010)
    // Slice: BLS_V1_START:
    // Black pixel window 1 vertical start position
    #define MRV_BLS_BLS_V1_START
    #define MRV_BLS_BLS_V1_START_MASK 0x00001FFF
    #define MRV_BLS_BLS_V1_START_SHIFT 0
    #define MRV_BLS_BLS_V1_START_MAX (MRV_BLS_BLS_V1_START_MASK >> MRV_BLS_BLS_V1_START_SHIFT)

    // Register: isp_bls_v1_stop: window 1 vertical stop (0x0014)
    // Slice: BLS_V1_STOP:
    // Black pixel window 1 vertical stop position
    #define MRV_BLS_BLS_V1_STOP
    #define MRV_BLS_BLS_V1_STOP_MASK 0x00001FFF
    #define MRV_BLS_BLS_V1_STOP_SHIFT 0
    #define MRV_BLS_BLS_V1_STOP_MAX (MRV_BLS_BLS_V1_STOP_MASK >> MRV_BLS_BLS_V1_STOP_SHIFT)

    // Register: isp_bls_h2_start: window 2 horizontal start (0x0018)
    // Slice: BLS_H2_START:
    // Black pixel window 2 horizontal start position
    #define MRV_BLS_BLS_H2_START
    #define MRV_BLS_BLS_H2_START_MASK 0x00001FFF
    #define MRV_BLS_BLS_H2_START_SHIFT 0
    #define MRV_BLS_BLS_H2_START_MAX (MRV_BLS_BLS_H2_START_MASK >> MRV_BLS_BLS_H2_START_SHIFT)

    // Register: isp_bls_h2_stop: window 2 horizontal stop (0x001c)
    // Slice: BLS_H2_STOP:
    // Black pixel window 2 horizontal stop position
    #define MRV_BLS_BLS_H2_STOP
    #define MRV_BLS_BLS_H2_STOP_MASK 0x00001FFF
    #define MRV_BLS_BLS_H2_STOP_SHIFT 0
    #define MRV_BLS_BLS_H2_STOP_MAX (MRV_BLS_BLS_H2_STOP_MASK >> MRV_BLS_BLS_H2_STOP_SHIFT)

    // Register: isp_bls_v2_start: window 2 vertical start (0x0020)
    // Slice: BLS_V2_START:
    // Black pixel window 2 vertical start position
    #define MRV_BLS_BLS_V2_START
    #define MRV_BLS_BLS_V2_START_MASK 0x00001FFF
    #define MRV_BLS_BLS_V2_START_SHIFT 0
    #define MRV_BLS_BLS_V2_START_MAX (MRV_BLS_BLS_V2_START_MASK >> MRV_BLS_BLS_V2_START_SHIFT)

    // Register: isp_bls_v2_stop: window 2 vertical stop (0x0024)
    // Slice: BLS_V2_STOP:
    // Black pixel window 2 vertical stop position
    #define MRV_BLS_BLS_V2_STOP
    #define MRV_BLS_BLS_V2_STOP_MASK 0x00001FFF
    #define MRV_BLS_BLS_V2_STOP_SHIFT 0
    #define MRV_BLS_BLS_V2_STOP_MAX (MRV_BLS_BLS_V2_STOP_MASK >> MRV_BLS_BLS_V2_STOP_SHIFT)
#endif  // MARVIN_FEATURE_BLACK_LEVEL

// isp_bls_?_fixed, isp_bls_ref_value_ab, isp_bls_ref_value_cd
// isp_bls_?_measured,  isp_bls_mean_value_ab, isp_bls_mean_value_cd
//#define MRV_ISP_BLS_MEAN_MASK_A     (0x000000FF)    //  8 bit range
//#define MRV_ISP_BLS_MEAN_MASK_B     (0x00FF0000)    //  8 bit range
//#define MRV_ISP_BLS_MEAN_MASK_C     (0x000000FF)    //  8 bit range
//#define MRV_ISP_BLS_MEAN_MASK_D     (0x00FF0000)    //  8 bit range
//#define MRV_BLACK_VALUE_A        0x000000FF //[7:0] rw manual black value to be subtracted for sample at position A
//#define MRV_BLACK_VALUE_B        0x00FF0000 //[23:16] rw manual black value to be subtracted for sample at position B
//#define MRV_BLACK_VALUE_C        0x000000FF //[7:0] rw manual black value to be subtracted for sample at position C
//#define MRV_BLACK_VALUE_D        0x00FF0000 //[23:16] rw manual black value to be subtracted for sample at position D
//#define MRV_BLACK_MEAN_VALUE_A   0x000000FF //[7:0] rh measured black value at sample at position A
//#define MRV_BLACK_MEAN_VALUE_B   0x00FF0000 //[23:16] rh measured black value at sample at position B
//#define MRV_BLACK_MEAN_VALUE_C   0x000000FF //[7:0] rh measured black value at sample at position C
//#define MRV_BLACK_MEAN_VALUE_D   0x00FF0000 //[23:16] rh measured black value at sample at position D
#if (MARVIN_FEATURE_BLACK_LEVEL == MARVIN_FEATURE_BLACK_LEVEL_V1)              //  8-bit
    #define MRV_ISP_BLS_FIX_SUB_MIN     (0x00000000)    // minimum value for fixed black level
    #define MRV_ISP_BLS_FIX_SUB_MAX     (0x000000FF)    // maximum value for fixed black level
    #define MRV_ISP_BLS_FIX_MASK        (0x000000FF)    //  8 bit range
    #define MRV_ISP_BLS_FIX_SHIFT_A              (0)    // no shift necessary
    #define MRV_ISP_BLS_FIX_SHIFT_B             (16)    // 16 bit shift necessary
    #define MRV_ISP_BLS_FIX_SHIFT_C              (0)    // no shift necessary
    #define MRV_ISP_BLS_FIX_SHIFT_D             (16)    // 16 bit shift necessary
    #define MRV_ISP_BLS_MEAN_MASK       (0x000000FF)    //  8 bit range
    #define MRV_ISP_BLS_MEAN_SHIFT_A             (0)    // no shift necessary
    #define MRV_ISP_BLS_MEAN_SHIFT_B            (16)    // 16 bit shift necessary
    #define MRV_ISP_BLS_MEAN_SHIFT_C             (0)    // no shift necessary
    #define MRV_ISP_BLS_MEAN_SHIFT_D            (16)    // 16 bit shift necessary
#elif (MARVIN_FEATURE_BLACK_LEVEL == MARVIN_FEATURE_BLACK_LEVEL_V2)            // 10-bit
    #define MRV_ISP_BLS_FIX_SUB_MIN     (0x00000000)    // minimum value for fixed black level
    #define MRV_ISP_BLS_FIX_SUB_MAX     (0x000003FF)    // maximum value for fixed black level
    #define MRV_ISP_BLS_FIX_MASK        (0x000003FF)    // 10 bit range
    #define MRV_ISP_BLS_FIX_SHIFT_A              (0)    // no shift necessary
    #define MRV_ISP_BLS_FIX_SHIFT_B             (16)    // 16 bit shift necessary
    #define MRV_ISP_BLS_FIX_SHIFT_C              (0)    // no shift necessary
    #define MRV_ISP_BLS_FIX_SHIFT_D             (16)    // 16 bit shift necessary
    #define MRV_ISP_BLS_MEAN_MASK       (0x000003FF)    // 10 bit range
    #define MRV_ISP_BLS_MEAN_SHIFT_A             (0)    // no shift necessary
    #define MRV_ISP_BLS_MEAN_SHIFT_B            (16)    // 16 bit shift necessary
    #define MRV_ISP_BLS_MEAN_SHIFT_C             (0)    // no shift necessary
    #define MRV_ISP_BLS_MEAN_SHIFT_D            (16)    // 16 bit shift necessary
#elif (MARVIN_FEATURE_BLACK_LEVEL == MARVIN_FEATURE_BLACK_LEVEL_V3)            // 12-bit
    #define MRV_ISP_BLS_FIX_SUB_MIN     (0x00000000)    // minimum value for fixed black level
    #define MRV_ISP_BLS_FIX_SUB_MAX     (0x00000FFF)    // maximum value for fixed black level
    #define MRV_ISP_BLS_FIX_MASK        (0x00000FFF)    // 12 bit range
    #define MRV_ISP_BLS_FIX_SHIFT_A              (0)    // no shift necessary
    #define MRV_ISP_BLS_FIX_SHIFT_B             (16)    // 16 bit shift necessary
    #define MRV_ISP_BLS_FIX_SHIFT_C              (0)    // no shift necessary
    #define MRV_ISP_BLS_FIX_SHIFT_D             (16)    // 16 bit shift necessary
    #define MRV_ISP_BLS_MEAN_MASK       (0x00000FFF)    // 12 bit range
    #define MRV_ISP_BLS_MEAN_SHIFT_A             (0)    // no shift necessary
    #define MRV_ISP_BLS_MEAN_SHIFT_B            (16)    // 16 bit shift necessary
    #define MRV_ISP_BLS_MEAN_SHIFT_C             (0)    // no shift necessary
    #define MRV_ISP_BLS_MEAN_SHIFT_D            (16)    // 16 bit shift necessary
#elif (MARVIN_FEATURE_BLACK_LEVEL == MARVIN_FEATURE_BLACK_LEVEL_V4)            // 11-bit
    #define MRV_ISP_BLS_FIX_SUB_MIN     (0xFFFFFC01)    // minimum value for fixed black level
    #define MRV_ISP_BLS_FIX_SUB_MAX     (0x000003FF)    // maximum value for fixed black level
    #define MRV_ISP_BLS_FIX_MASK        (0x000007FF)    // 11 bit range (signed)
    #define MRV_ISP_BLS_FIX_SHIFT_A              (0)    // no shift necessary
    #define MRV_ISP_BLS_FIX_SHIFT_B              (0)    // no shift necessary
    #define MRV_ISP_BLS_FIX_SHIFT_C              (0)    // no shift necessary
    #define MRV_ISP_BLS_FIX_SHIFT_D              (0)    // no shift necessary
    #define MRV_ISP_BLS_MEAN_MASK       (0x000003FF)    // 10 bit range
    #define MRV_ISP_BLS_MEAN_SHIFT_A             (0)    // no shift necessary
    #define MRV_ISP_BLS_MEAN_SHIFT_B             (0)    // no shift necessary
    #define MRV_ISP_BLS_MEAN_SHIFT_C             (0)    // no shift necessary
    #define MRV_ISP_BLS_MEAN_SHIFT_D             (0)    // no shift necessary
#elif (MARVIN_FEATURE_BLACK_LEVEL == MARVIN_FEATURE_BLACK_LEVEL_V5)            // 13-bit
    #define MRV_ISP_BLS_FIX_SUB_MIN     (0xFFFFF001)    // minimum value for fixed black level
    #define MRV_ISP_BLS_FIX_SUB_MAX     (0x00000FFF)    // maximum value for fixed black level
    #define MRV_ISP_BLS_FIX_MASK        (0x00001FFF)    // 13 bit range (signed)
    #define MRV_ISP_BLS_FIX_SHIFT_A              (0)    // no shift necessary
    #define MRV_ISP_BLS_FIX_SHIFT_B              (0)    // no shift necessary
    #define MRV_ISP_BLS_FIX_SHIFT_C              (0)    // no shift necessary
    #define MRV_ISP_BLS_FIX_SHIFT_D              (0)    // no shift necessary
    #define MRV_ISP_BLS_MEAN_MASK       (0x00000FFF)    // 12 bit range
    #define MRV_ISP_BLS_MEAN_SHIFT_A             (0)    // no shift necessary
    #define MRV_ISP_BLS_MEAN_SHIFT_B             (0)    // no shift necessary
    #define MRV_ISP_BLS_MEAN_SHIFT_C             (0)    // no shift necessary
    #define MRV_ISP_BLS_MEAN_SHIFT_D             (0)    // no shift necessary
#endif  // MARVIN_FEATURE_BLACK_LEVEL

// Register: isp_bls_a_fixed: fixed black level A (0x0028)
// Slice: BLS_A_FIXED:
// Fixed black level for A pixels – signed
// two's complement, value range from -4096 to +4095,
// a positive value will be subtracted from the pixel values
#define MRV_BLS_BLS_A_FIXED
#define MRV_BLS_BLS_A_FIXED_MASK (MRV_ISP_BLS_FIX_MASK << MRV_ISP_BLS_FIX_SHIFT_A)
#define MRV_BLS_BLS_A_FIXED_SHIFT MRV_ISP_BLS_FIX_SHIFT_A

// Register: isp_bls_b_fixed: fixed black level B (0x002c)
// Slice: BLS_B_FIXED:
// Fixed black level for B pixels – signed
// two's complement, value range from -4096 to +4095
#define MRV_BLS_BLS_B_FIXED
#define MRV_BLS_BLS_B_FIXED_MASK (MRV_ISP_BLS_FIX_MASK << MRV_ISP_BLS_FIX_SHIFT_B)
#define MRV_BLS_BLS_B_FIXED_SHIFT MRV_ISP_BLS_FIX_SHIFT_B

// Register: isp_bls_c_fixed: fixed black level C (0x0030)
// Slice: BLS_C_FIXED:
// Fixed black level for C pixels – signed
// two's complement, value range from -4096 to +4095
#define MRV_BLS_BLS_C_FIXED
#define MRV_BLS_BLS_C_FIXED_MASK (MRV_ISP_BLS_FIX_MASK << MRV_ISP_BLS_FIX_SHIFT_C)
#define MRV_BLS_BLS_C_FIXED_SHIFT MRV_ISP_BLS_FIX_SHIFT_C

// Register: isp_bls_d_fixed: fixed black level D (0x0034)
// Slice: BLS_D_FIXED:
// Fixed black level for D pixels  - signed
// two's complement, value range from -4096 to +4095
#define MRV_BLS_BLS_D_FIXED
#define MRV_BLS_BLS_D_FIXED_MASK (MRV_ISP_BLS_FIX_MASK << MRV_ISP_BLS_FIX_SHIFT_D)
#define MRV_BLS_BLS_D_FIXED_SHIFT MRV_ISP_BLS_FIX_SHIFT_D

// Register: isp_bls_a_measured: measured black level A (0x0038)
// Slice: BLS_A_MEASURED:
// Measured black level for A pixels
#define MRV_BLS_BLS_A_MEASURED
#define MRV_BLS_BLS_A_MEASURED_MASK (MRV_ISP_BLS_MEAN_MASK << MRV_ISP_BLS_MEAN_SHIFT_A)
#define MRV_BLS_BLS_A_MEASURED_SHIFT MRV_ISP_BLS_MEAN_SHIFT_A

// Register: isp_bls_b_measured: measured black level B (0x003c)
// Slice: BLS_B_MEASURED:
// Measured black level for B pixels
#define MRV_BLS_BLS_B_MEASURED
#define MRV_BLS_BLS_B_MEASURED_MASK (MRV_ISP_BLS_MEAN_MASK << MRV_ISP_BLS_MEAN_SHIFT_B)
#define MRV_BLS_BLS_B_MEASURED_SHIFT MRV_ISP_BLS_MEAN_SHIFT_B

// Register: isp_bls_c_measured: measured black level C (0x0040)
// Slice: BLS_C_MEASURED:
// Measured black level for C pixels
#define MRV_BLS_BLS_C_MEASURED
#define MRV_BLS_BLS_C_MEASURED_MASK (MRV_ISP_BLS_MEAN_MASK << MRV_ISP_BLS_MEAN_SHIFT_C)
#define MRV_BLS_BLS_C_MEASURED_SHIFT MRV_ISP_BLS_MEAN_SHIFT_C

// Register: isp_bls_d_measured: measured black level D (0x0044)
// Slice: BLS_D_MEASURED:
// Measured black level for D pixels
#define MRV_BLS_BLS_D_MEASURED
#define MRV_BLS_BLS_D_MEASURED_MASK (MRV_ISP_BLS_MEAN_MASK << MRV_ISP_BLS_MEAN_SHIFT_D)
#define MRV_BLS_BLS_D_MEASURED_SHIFT MRV_ISP_BLS_MEAN_SHIFT_D

/*---------------------------------------- D P C C (begin) ----------------------------------------*/
#if( MARVIN_FEATURE_DPCC == MARVIN_FEATURE_DPCC_V1 )
// Register: isp_dpcc_mode: Mode control for DPCC block (rw) MRV_DPCC_BASE + 0x0000
// Slice: STAGE1_ENABLE
// 1: enable stage1 *Default*
// 0: bypass stage1
#define MRV_DPCC_STAGE1_ENABLE
#define MRV_DPCC_STAGE1_ENABLE_MASK (0x00000004)
#define MRV_DPCC_STAGE1_ENABLE_SHIFT (2)
// Slice: GRAYSCALE_MODE
// 1: gray scale data input from black and white sensors
// 0: BAYER DATA INPUT *Default*
#define MRV_DPCC_GRAYSCALE_MODE
#define MRV_DPCC_GRAYSCALE_MODE_MASK (0x00000002)
#define MRV_DPCC_GRAYSCALE_MODE_SHIFT (1)
// Slice: DPCC_ENABLE
// 1: enable DPCC
// 0: bypass DPCC *Default*
#define MRV_DPCC_ENABLE
#define MRV_DPCC_ENABLE_MASK (0x00000001)
#define MRV_DPCC_ENABLE_SHIFT (0)

// Register: isp_dpcc_output_mode: Output Interpolation mode (rw) MRV_DPCC_BASE + 0x0004
// Slice: STAGE1_RB_3x3
// 1: stage1 red/blue 9 pixel (3x3) output median
// 0: stage1 red/blue 4 or 5 pixel output median *Default*
#define MRV_DPCC_STAGE1_RB_3x3
#define MRV_DPCC_STAGE1_RB_3x3_MASK (0x00000008)
#define MRV_DPCC_STAGE1_RB_3x3_SHIFT (3)
// Slice: STAGE1_G_3x3
// 1: stage2 green 9 pixel (3x3) output median
// 0: stage2 green 4 or 5 pixel output median *Default*
#define MRV_DPCC_STAGE1_G_3x3
#define MRV_DPCC_STAGE1_G_3x3_MASK (0x00000004)
#define MRV_DPCC_STAGE1_G_3x3_SHIFT (2)
// Slice: STAGE1_INCL_RB_CENTER
// 1: stage1 include center pixel for red/blue output median 2x2+1 *Default*
// 0: stage1 do not include center pixel for red/blue output median 2x2
#define MRV_DPCC_STAGE1_INCL_RB_CENTER
#define MRV_DPCC_STAGE1_INCL_RB_CENTER_MASK (0x00000002)
#define MRV_DPCC_STAGE1_INCL_RB_CENTER_SHIFT (1)
// Slice: STAGE1_INCL_GREEN_CENTER
// 1: stage1 include center pixel for green output median 2x2+1 *Default*
// 0: stage1 do not include center pixel for green output median 2x2
#define MRV_DPCC_STAGE1_INCL_GREEN_CENTER
#define MRV_DPCC_STAGE1_INCL_GREEN_CENTER_MASK (0x00000001)
#define MRV_DPCC_STAGE1_INCL_GREEN_CENTER_SHIFT (0)

// Register: isp_dpcc_set_use: DPCC parameter set use (rw) MRV_DPCC_BASE + 0x0008
// Slice: STAGE1_USE_FIX_SET
// 1: stage1 use hard coded parameter set *Default*
// 0: stage1 do not use hard coded parameter set
#define MRV_DPCC_STAGE1_USE_FIX_SET
#define MRV_DPCC_STAGE1_USE_FIX_SET_MASK (0x00000008)
#define MRV_DPCC_STAGE1_USE_FIX_SET_SHIFT (3)
// Slice: STAGE1_USE_SET_3
// 1: stage1 use parameter set 3
// 0: stage1 do not use parameter set 3 *Default*
#define MRV_DPCC_STAGE1_USE_SET_3
#define MRV_DPCC_STAGE1_USE_SET_3_MASK (0x00000004)
#define MRV_DPCC_STAGE1_USE_SET_3_SHIFT (2)
// Slice: STAGE1_USE_SET_2
// 1: stage1 use parameter set 2
// 0: stage1 do not use parameter set 2 *Default*
#define MRV_DPCC_STAGE1_USE_SET_2
#define MRV_DPCC_STAGE1_USE_SET_2_MASK (0x00000002)
#define MRV_DPCC_STAGE1_USE_SET_2_SHIFT (1)
// Slice: STAGE1_USE_SET_1
// 1: stage1 use parameter set 1 *Default*
// 0: stage1 do not use parameter set 1
#define MRV_DPCC_STAGE1_USE_SET_1
#define MRV_DPCC_STAGE1_USE_SET_1_MASK (0x00000001)
#define MRV_DPCC_STAGE1_USE_SET_1_SHIFT (0)

// Register: isp_dpcc_methods_set1: Methods enables for set 1 (rw) MRV_DPCC_BASE + 0x000C
// Slice: RG_RED_BLUE1_ENABLE
// 1: enable Rank Gradient check for red_blue *Default*
// 0: bypass Rank Gradient check for red_blue
#define MRV_DPCC_RG_RED_BLUE1_ENABLE
#define MRV_DPCC_RG_RED_BLUE1_ENABLE_MASK (0x00001000)
#define MRV_DPCC_RG_RED_BLUE1_ENABLE_SHIFT (12)
// Slice: RND_RED_BLUE1_ENABLE
// 1: enable Rank Neighbor Difference check for red_blue *Default*
// 0: bypass Rank Neighbor Difference check for red_blue
#define MRV_DPCC_RND_RED_BLUE1_ENABLE
#define MRV_DPCC_RND_RED_BLUE1_ENABLE_MASK (0x00000800)
#define MRV_DPCC_RND_RED_BLUE1_ENABLE_SHIFT (11)
// Slice: RO_RED_BLUE1_ENABLE
// 1: enable Rank Order check for red_blue *Default*
// 0: bypass Rank Order check for red_blue
#define MRV_DPCC_RO_RED_BLUE1_ENABLE
#define MRV_DPCC_RO_RED_BLUE1_ENABLE_MASK (0x00000400)
#define MRV_DPCC_RO_RED_BLUE1_ENABLE_SHIFT (10)
// Slice: LC_RED_BLUE1_ENABLE
// 1: enable Line check for red_blue
// 0: bypass Line check for red_blue *Default*
#define MRV_DPCC_LC_RED_BLUE1_ENABLE
#define MRV_DPCC_LC_RED_BLUE1_ENABLE_MASK (0x00000200)
#define MRV_DPCC_LC_RED_BLUE1_ENABLE_SHIFT (9)
// Slice: PG_RED_BLUE1_ENABLE
// 1: enable Peak Gradient check for red_blue *Default*
// 0: bypass Peak Gradient check for red_blue
#define MRV_DPCC_PG_RED_BLUE1_ENABLE
#define MRV_DPCC_PG_RED_BLUE1_ENABLE_MASK (0x00000100)
#define MRV_DPCC_PG_RED_BLUE1_ENABLE_SHIFT (8)
// Slice: RG_GREEN1_ENABLE
// 1: enable Rank Gradient check for green *Default*
// 0: bypass Rank Gradient check for green
#define MRV_DPCC_RG_GREEN1_ENABLE
#define MRV_DPCC_RG_GREEN1_ENABLE_MASK (0x00000010)
#define MRV_DPCC_RG_GREEN1_ENABLE_SHIFT (4)
// Slice: RND_GREEN1_ENABLE
// 1: enable Rank Neighbor Difference check for green *Default*
// 0: bypass Rank Neighbor Difference check for green
#define MRV_DPCC_RND_GREEN1_ENABLE
#define MRV_DPCC_RND_GREEN1_ENABLE_MASK (0x00000008)
#define MRV_DPCC_RND_GREEN1_ENABLE_SHIFT (3)
// Slice: RO_GREEN1_ENABLE
// 1: enable Rank Order check for green *Default*
// 0: bypass Rank Order check for green
#define MRV_DPCC_RO_GREEN1_ENABLE
#define MRV_DPCC_RO_GREEN1_ENABLE_MASK (0x00000004)
#define MRV_DPCC_RO_GREEN1_ENABLE_SHIFT (2)
// Slice: LC_GREEN1_ENABLE
// 1: enable Line check for green
// 0: bypass Line check for green *Default*
#define MRV_DPCC_LC_GREEN1_ENABLE
#define MRV_DPCC_LC_GREEN1_ENABLE_MASK (0x00000002)
#define MRV_DPCC_LC_GREEN1_ENABLE_SHIFT (1)
// Slice: PG_GREEN1_ENABLE
// 1: enable Peak Gradient check for green *Default*
// 0: bypass Peak Gradient check for green
#define MRV_DPCC_PG_GREEN1_ENABLE
#define MRV_DPCC_PG_GREEN1_ENABLE_MASK (0x00000001)
#define MRV_DPCC_PG_GREEN1_ENABLE_SHIFT (0)

// Register: isp_dpcc_methods_set2: Methods enables for set 2 (rw) MRV_DPCC_BASE + 0x0010
// Slice: RG_RED_BLUE2_ENABLE
// 1: enable Rank Gradient check for red_blue *Default*
// 0: bypass Rank Gradient check for red_blue
#define MRV_DPCC_RG_RED_BLUE2_ENABLE
#define MRV_DPCC_RG_RED_BLUE2_ENABLE_MASK (0x00001000)
#define MRV_DPCC_RG_RED_BLUE2_ENABLE_SHIFT (12)
// Slice: RND_RED_BLUE2_ENABLE
// 1: enable Rank Neighbor Difference check for red_blue *Default*
// 0: bypass Rank Neighbor Difference check for red_blue
#define MRV_DPCC_RND_RED_BLUE2_ENABLE
#define MRV_DPCC_RND_RED_BLUE2_ENABLE_MASK (0x00000800)
#define MRV_DPCC_RND_RED_BLUE2_ENABLE_SHIFT (11)
// Slice: RO_RED_BLUE2_ENABLE
// 1: enable Rank Order check for red_blue *Default*
// 0: bypass Rank Order check for red_blue
#define MRV_DPCC_RO_RED_BLUE2_ENABLE
#define MRV_DPCC_RO_RED_BLUE2_ENABLE_MASK (0x00000400)
#define MRV_DPCC_RO_RED_BLUE2_ENABLE_SHIFT (10)
// Slice: LC_RED_BLUE2_ENABLE
// 1: enable Line check for red_blue
// 0: bypass Line check for red_blue *Default*
#define MRV_DPCC_LC_RED_BLUE2_ENABLE
#define MRV_DPCC_LC_RED_BLUE2_ENABLE_MASK (0x00000200)
#define MRV_DPCC_LC_RED_BLUE2_ENABLE_SHIFT (9)
// Slice: PG_RED_BLUE2_ENABLE
// 1: enable Peak Gradient check for red_blue *Default*
// 0: bypass Peak Gradient check for red_blue
#define MRV_DPCC_PG_RED_BLUE2_ENABLE
#define MRV_DPCC_PG_RED_BLUE2_ENABLE_MASK (0x00000100)
#define MRV_DPCC_PG_RED_BLUE2_ENABLE_SHIFT (8)
// Slice: RG_GREEN2_ENABLE
// 1: enable Rank Gradient check for green *Default*
// 0: bypass Rank Gradient check for green
#define MRV_DPCC_RG_GREEN2_ENABLE
#define MRV_DPCC_RG_GREEN2_ENABLE_MASK (0x00000010)
#define MRV_DPCC_RG_GREEN2_ENABLE_SHIFT (4)
// Slice: RND_GREEN2_ENABLE
// 1: enable Rank Neighbor Difference check for green *Default*
// 0: bypass Rank Neighbor Difference check for green
#define MRV_DPCC_RND_GREEN2_ENABLE
#define MRV_DPCC_RND_GREEN2_ENABLE_MASK (0x00000008)
#define MRV_DPCC_RND_GREEN2_ENABLE_SHIFT (3)
// Slice: RO_GREEN2_ENABLE
// 1: enable Rank Order check for green *Default*
// 0: bypass Rank Order check for green
#define MRV_DPCC_RO_GREEN2_ENABLE
#define MRV_DPCC_RO_GREEN2_ENABLE_MASK (0x00000004)
#define MRV_DPCC_RO_GREEN2_ENABLE_SHIFT (2)
// Slice: LC_GREEN2_ENABLE
// 1: enable Line check for green
// 0: bypass Line check for green *Default*
#define MRV_DPCC_LC_GREEN2_ENABLE
#define MRV_DPCC_LC_GREEN2_ENABLE_MASK (0x00000002)
#define MRV_DPCC_LC_GREEN2_ENABLE_SHIFT (1)
// Slice: PG_GREEN2_ENABLE
// 1: enable Peak Gradient check for green *Default*
// 0: bypass Peak Gradient check for green
#define MRV_DPCC_PG_GREEN2_ENABLE
#define MRV_DPCC_PG_GREEN2_ENABLE_MASK (0x00000001)
#define MRV_DPCC_PG_GREEN2_ENABLE_SHIFT (0)

// Register: isp_dpcc_methods_set3: Methods enables for set 3 (rw) MRV_DPCC_BASE + 0x0014
// Slice: RG_RED_BLUE3_ENABLE
// 1: enable Rank Gradient check for red_blue *Default*
// 0: bypass Rank Gradient check for red_blue
#define MRV_DPCC_RG_RED_BLUE3_ENABLE
#define MRV_DPCC_RG_RED_BLUE3_ENABLE_MASK (0x00001000)
#define MRV_DPCC_RG_RED_BLUE3_ENABLE_SHIFT (12)
// Slice: RND_RED_BLUE3_ENABLE
// 1: enable Rank Neighbor Difference check for red_blue *Default*
// 0: bypass Rank Neighbor Difference check for red_blue
#define MRV_DPCC_RND_RED_BLUE3_ENABLE
#define MRV_DPCC_RND_RED_BLUE3_ENABLE_MASK (0x00000800)
#define MRV_DPCC_RND_RED_BLUE3_ENABLE_SHIFT (11)
// Slice: RO_RED_BLUE3_ENABLE
// 1: enable Rank Order check for red_blue *Default*
// 0: bypass Rank Order check for red_blue
#define MRV_DPCC_RO_RED_BLUE3_ENABLE
#define MRV_DPCC_RO_RED_BLUE3_ENABLE_MASK (0x00000400)
#define MRV_DPCC_RO_RED_BLUE3_ENABLE_SHIFT (10)
// Slice: LC_RED_BLUE3_ENABLE
// 1: enable Line check for red_blue
// 0: bypass Line check for red_blue *Default*
#define MRV_DPCC_LC_RED_BLUE3_ENABLE
#define MRV_DPCC_LC_RED_BLUE3_ENABLE_MASK (0x00000200)
#define MRV_DPCC_LC_RED_BLUE3_ENABLE_SHIFT (9)
// Slice: PG_RED_BLUE3_ENABLE
// 1: enable Peak Gradient check for red_blue *Default*
// 0: bypass Peak Gradient check for red_blue
#define MRV_DPCC_PG_RED_BLUE3_ENABLE
#define MRV_DPCC_PG_RED_BLUE3_ENABLE_MASK (0x00000100)
#define MRV_DPCC_PG_RED_BLUE3_ENABLE_SHIFT (8)
// Slice: RG_GREEN3_ENABLE
// 1: enable Rank Gradient check for green *Default*
// 0: bypass Rank Gradient check for green
#define MRV_DPCC_RG_GREEN3_ENABLE
#define MRV_DPCC_RG_GREEN3_ENABLE_MASK (0x00000010)
#define MRV_DPCC_RG_GREEN3_ENABLE_SHIFT (4)
// Slice: RND_GREEN3_ENABLE
// 1: enable Rank Neighbor Difference check for green *Default*
// 0: bypass Rank Neighbor Difference check for green
#define MRV_DPCC_RND_GREEN3_ENABLE
#define MRV_DPCC_RND_GREEN3_ENABLE_MASK (0x00000008)
#define MRV_DPCC_RND_GREEN3_ENABLE_SHIFT (3)
// Slice: RO_GREEN3_ENABLE
// 1: enable Rank Order check for green *Default*
// 0: bypass Rank Order check for green
#define MRV_DPCC_RO_GREEN3_ENABLE
#define MRV_DPCC_RO_GREEN3_ENABLE_MASK (0x00000004)
#define MRV_DPCC_RO_GREEN3_ENABLE_SHIFT (2)
// Slice: LC_GREEN3_ENABLE
// 1: enable Line check for green
// 0: bypass Line check for green *Default*
#define MRV_DPCC_LC_GREEN3_ENABLE
#define MRV_DPCC_LC_GREEN3_ENABLE_MASK (0x00000002)
#define MRV_DPCC_LC_GREEN3_ENABLE_SHIFT (1)
// Slice: PG_GREEN3_ENABLE
// 1: enable Peak Gradient check for green *Default*
// 0: bypass Peak Gradient check for green
#define MRV_DPCC_PG_GREEN3_ENABLE
#define MRV_DPCC_PG_GREEN3_ENABLE_MASK (0x00000001)
#define MRV_DPCC_PG_GREEN3_ENABLE_SHIFT (0)

// Register: isp_dpcc_line_thresh_1:  Line threshold set 1 (rw) MRV_DPCC_BASE + 0x0018
// Slice: LINE_THR_1_RB
// line threshold for set 1 red/blue
#define MRV_DPCC_LINE_THR_1_RB
#define MRV_DPCC_LINE_THR_1_RB_MASK (0x0000FF00)
#define MRV_DPCC_LINE_THR_1_RB_SHIFT (8)
// Slice: LINE_THR_1_G
// line threshold for set 1 green
#define MRV_DPCC_LINE_THR_1_G
#define MRV_DPCC_LINE_THR_1_G_MASK (0x000000FF)
#define MRV_DPCC_LINE_THR_1_G_SHIFT (0)

// Register: isp_dpcc_line_mad_fac_1: MAD factor for Line check set 1 (rw) MRV_DPCC_BASE + 0x001C
// Slice: LINE_MAD_FAC_1_RB
// line MAD factor for set 1 red/blue
#define MRV_DPCC_LINE_MAD_FAC_1_RB
#define MRV_DPCC_LINE_MAD_FAC_1_RB_MASK (0x00003F00)
#define MRV_DPCC_LINE_MAD_FAC_1_RB_SHIFT (8)
// Slice: LINE_MAD_FAC_1_G
// line MAD factor for set 1 green
#define MRV_DPCC_LINE_MAD_FAC_1_G
#define MRV_DPCC_LINE_MAD_FAC_1_G_MASK (0x0000003F)
#define MRV_DPCC_LINE_MAD_FAC_1_G_SHIFT (0)

// Register: isp_dpcc_pg_fac_1: Peak gradient factor for set 1 (rw) MRV_DPCC_BASE + 0x0020
// Slice: PG_FAC_1_RB
// Peak gradient factor for set 1 red/blue
#define MRV_DPCC_PG_FAC_1_RB
#define MRV_DPCC_PG_FAC_1_RB_MASK (0x00003F00)
#define MRV_DPCC_PG_FAC_1_RB_SHIFT (8)
// Slice: PG_FAC_1_G
// Peak gradient factor for set 1 green
#define MRV_DPCC_PG_FAC_1_G
#define MRV_DPCC_PG_FAC_1_G_MASK (0x0000003F)
#define MRV_DPCC_PG_FAC_1_G_SHIFT (0)

// Register: isp_dpcc_rnd_thresh_1: Rank Neighbor Difference threshold for set 1 (rw) MRV_DPCC_BASE + 0x0024
// Slice: RND_THR_1_RB
// Rank Neighbor Difference threshold for set 1 red/blue
#define MRV_DPCC_RND_THR_1_RB
#define MRV_DPCC_RND_THR_1_RB_MASK (0x0000FF00)
#define MRV_DPCC_RND_THR_1_RB_SHIFT (8)
// Slice: RND_THR_1_G
// Rank Neighbor Difference threshold for set 1 green
#define MRV_DPCC_RND_THR_1_G
#define MRV_DPCC_RND_THR_1_G_MASK (0x000000FF)
#define MRV_DPCC_RND_THR_1_G_SHIFT (0)

// Register: isp_dpcc_rg_fac_1: Rank gradient factor for set 1 (rw) MRV_DPCC_BASE + 0x0028
// Slice: RG_FAC_1_RB
// Rank gradient factor for set 1 red/blue
#define MRV_DPCC_RG_FAC_1_RB
#define MRV_DPCC_RG_FAC_1_RB_MASK (0x00003F00)
#define MRV_DPCC_RG_FAC_1_RB_SHIFT (8)
//Slice: RG_FAC_1_G
// Rank gradient factor for set 1 green
#define MRV_DPCC_RG_FAC_1_G
#define MRV_DPCC_RG_FAC_1_G_MASK (0x0000003F)
#define MRV_DPCC_RG_FAC_1_G_SHIFT (0)

// Register: isp_dpcc_line_thresh_2: Line threshold set 2 (rw) MRV_DPCC_BASE + 0x002C
// Slice: LINE_THR_2_RB
// line threshold for set 2 red/blue
#define MRV_DPCC_LINE_THR_2_RB
#define MRV_DPCC_LINE_THR_2_RB_MASK (0x0000FF00)
#define MRV_DPCC_LINE_THR_2_RB_SHIFT (8)
// Slice: LINE_THR_2_G
// line threshold for set 2 green
#define MRV_DPCC_LINE_THR_2_G
#define MRV_DPCC_LINE_THR_2_G_MASK (0x000000FF)
#define MRV_DPCC_LINE_THR_2_G_SHIFT (0)

// Register: isp_dpcc_line_mad_fac_2: MAD factor for Line check set 2 (rw) MRV_DPCC_BASE + 0x0030
// Slice: LINE_MAD_FAC_2_RB
// line MAD factor for set 2 red/blue
#define MRV_DPCC_LINE_MAD_FAC_2_RB
#define MRV_DPCC_LINE_MAD_FAC_2_RB_MASK (0x00003F00)
#define MRV_DPCC_LINE_MAD_FAC_2_RB_SHIFT (8)
// Slice: LINE_MAD_FAC_2_G
// line MAD factor for set 2 green
#define MRV_DPCC_LINE_MAD_FAC_2_G
#define MRV_DPCC_LINE_MAD_FAC_2_G_MASK (0x0000003F)
#define MRV_DPCC_LINE_MAD_FAC_2_G_SHIFT (0)

// Register: isp_dpcc_pg_fac_2: Peak gradient factor for set 2 (rw) MRV_DPCC_BASE + 0x0034
// Slice: PG_FAC_2_RB
// Peak gradient factor for set 2 red/blue
#define MRV_DPCC_PG_FAC_2_RB
#define MRV_DPCC_PG_FAC_2_RB_MASK (0x00003F00)
#define MRV_DPCC_PG_FAC_2_RB_SHIFT (8)
// Slice: PG_FAC_2_G
// Peak gradient factor for set 2 green
#define MRV_DPCC_PG_FAC_2_G
#define MRV_DPCC_PG_FAC_2_G_MASK (0x0000003F)
#define MRV_DPCC_PG_FAC_2_G_SHIFT (0)

// Register: isp_dpcc_rnd_thresh_2: Rank Neighbor Difference threshold for set 2 (rw) MRV_DPCC_BASE + 0x0038
// Slice: RND_THR_2_RB
// Rank Neighbor Difference threshold for set 2 red/blue
#define MRV_DPCC_RND_THR_2_RB
#define MRV_DPCC_RND_THR_2_RB_MASK (0x0000FF00)
#define MRV_DPCC_RND_THR_2_RB_SHIFT (8)
// Slice: RND_THR_2_G
// Rank Neighbor Difference threshold for set 2 green
#define MRV_DPCC_RND_THR_2_G
#define MRV_DPCC_RND_THR_2_G_MASK (0x000000FF)
#define MRV_DPCC_RND_THR_2_G_SHIFT (0)

// Register: isp_dpcc_rg_fac_2: Rank gradient factor for set 2 (rw) MRV_DPCC_BASE + 0x003C
// Slice: RG_FAC_2_RB
// Rank gradient factor for set 2 red/blue
#define MRV_DPCC_RG_FAC_2_RB
#define MRV_DPCC_RG_FAC_2_RB_MASK (0x00003F00)
#define MRV_DPCC_RG_FAC_2_RB_SHIFT (8)
//Slice: RG_FAC_2_G
// Rank gradient factor for set 2 green
#define MRV_DPCC_RG_FAC_2_G
#define MRV_DPCC_RG_FAC_2_G_MASK (0x0000003F)
#define MRV_DPCC_RG_FAC_2_G_SHIFT (0)

// Register: isp_dpcc_line_thresh_3: Line threshold set 3 (rw) MRV_DPCC_BASE + 0x0040
// Slice: LINE_THR_3_RB
// line threshold for set 3 red/blue
#define MRV_DPCC_LINE_THR_3_RB
#define MRV_DPCC_LINE_THR_3_RB_MASK (0x0000FF00)
#define MRV_DPCC_LINE_THR_3_RB_SHIFT (8)
// Slice: LINE_THR_3_G
// line threshold for set 3 green
#define MRV_DPCC_LINE_THR_3_G
#define MRV_DPCC_LINE_THR_3_G_MASK (0x000000FF)
#define MRV_DPCC_LINE_THR_3_G_SHIFT (0)

// Register: isp_dpcc_line_mad_fac_3: MAD factor for Line check set 3 (rw) MRV_DPCC_BASE + 0x0044
// Slice: LINE_MAD_FAC_3_RB
// line MAD factor for set 3 red/blue
#define MRV_DPCC_LINE_MAD_FAC_3_RB
#define MRV_DPCC_LINE_MAD_FAC_3_RB_MASK (0x00003F00)
#define MRV_DPCC_LINE_MAD_FAC_3_RB_SHIFT (8)
// Slice: LINE_MAD_FAC_3_G
// line MAD factor for set 3 green
#define MRV_DPCC_LINE_MAD_FAC_3_G
#define MRV_DPCC_LINE_MAD_FAC_3_G_MASK (0x0000003F)
#define MRV_DPCC_LINE_MAD_FAC_3_G_SHIFT (0)

// Register: isp_dpcc_pg_fac_3: Peak gradient factor for set 3 (rw) MRV_DPCC_BASE + 0x0048
// Slice: PG_FAC_3_RB
// Peak gradient factor for set 3 red/blue
#define MRV_DPCC_PG_FAC_3_RB
#define MRV_DPCC_PG_FAC_3_RB_MASK (0x00003F00)
#define MRV_DPCC_PG_FAC_3_RB_SHIFT (8)
// Slice: PG_FAC_3_G
// Peak gradient factor for set 3 green
#define MRV_DPCC_PG_FAC_3_G
#define MRV_DPCC_PG_FAC_3_G_MASK (0x0000003F)
#define MRV_DPCC_PG_FAC_3_G_SHIFT (0)

// Register: isp_dpcc_rnd_thresh_3: Rank Neighbor Difference threshold for set 3 (rw) MRV_DPCC_BASE + 0x004C
// Slice: RND_THR_3_RB
// Rank Neighbor Difference threshold for set 3 red/blue
#define MRV_DPCC_RND_THR_3_RB
#define MRV_DPCC_RND_THR_3_RB_MASK (0x0000FF00)
#define MRV_DPCC_RND_THR_3_RB_SHIFT (8)
// Slice: RND_THR_3_G
// Rank Neighbor Difference threshold for set 3 green
#define MRV_DPCC_RND_THR_3_G
#define MRV_DPCC_RND_THR_3_G_MASK (0x000000FF)
#define MRV_DPCC_RND_THR_3_G_SHIFT (0)

// Register: isp_dpcc_rg_fac_3: Rank gradient factor for set 3 (rw) MRV_DPCC_BASE + 0x0050
// Slice: RG_FAC_3_RB
// Rank gradient factor for set 3 red/blue
#define MRV_DPCC_RG_FAC_3_RB
#define MRV_DPCC_RG_FAC_3_RB_MASK (0x00003F00)
#define MRV_DPCC_RG_FAC_3_RB_SHIFT (8)
//Slice: RG_FAC_3_G
// Rank gradient factor for set 3 green
#define MRV_DPCC_RG_FAC_3_G
#define MRV_DPCC_RG_FAC_3_G_MASK (0x0000003F)
#define MRV_DPCC_RG_FAC_3_G_SHIFT (0)

// Register: isp_dpcc_ro_limits: Rank Order Limits (rw) MRV_DPCC_BASE + 0x0054
// Slice: RO_LIM_3_RB
// Rank order limit for set 3 red/blue
#define MRV_DPCC_RO_LIM_3_RB
#define MRV_DPCC_RO_LIM_3_RB_MASK (0x00000C00)
#define MRV_DPCC_RO_LIM_3_RB_SHIFT (10)
// Slice: RO_LIM_3_G
// Rank order limit for set 3 green
#define MRV_DPCC_RO_LIM_3_G
#define MRV_DPCC_RO_LIM_3_G_MASK (0x00000300)
#define MRV_DPCC_RO_LIM_3_G_SHIFT (8)
// Slice: RO_LIM_2_RB
// Rank order limit for set 2 red/blue
#define MRV_DPCC_RO_LIM_2_RB
#define MRV_DPCC_RO_LIM_2_RB_MASK (0x000000C0)
#define MRV_DPCC_RO_LIM_2_RB_SHIFT (6)
// Slice: RO_LIM_2_G
// Rank order limit for set 2 green
#define MRV_DPCC_RO_LIM_2_G
#define MRV_DPCC_RO_LIM_2_G_MASK (0x00000030)
#define MRV_DPCC_RO_LIM_2_G_SHIFT (4)
// Slice: RO_LIM_1_RB
// Rank order limit for set 1 red/blue
#define MRV_DPCC_RO_LIM_1_RB
#define MRV_DPCC_RO_LIM_1_RB_MASK (0x0000000C)
#define MRV_DPCC_RO_LIM_1_RB_SHIFT (2)
// Slice: RO_LIM_1_G
// Rank order limit for set 1 green
#define MRV_DPCC_RO_LIM_1_G
#define MRV_DPCC_RO_LIM_1_G_MASK (0x00000003)
#define MRV_DPCC_RO_LIM_1_G_SHIFT (0)

// Register: isp_dpcc_rnd_offs: Differential Rank Offsets (rw) MRV_DPCC_BASE + 0x0058
// Slice: RND_OFFS_3_RB
// Rank Offset to Neighbor for set 3 red/blue
#define MRV_DPCC_RND_OFFS_3_RB
#define MRV_DPCC_RND_OFFS_3_RB_MASK (0x00000C00)
#define MRV_DPCC_RND_OFFS_3_RB_SHIFT (10)
// Slice: RND_OFFS_3_G
// Rank Offset to Neighbor for set 3 green
#define MRV_DPCC_RND_OFFS_3_G
#define MRV_DPCC_RND_OFFS_3_G_MASK (0x00000300)
#define MRV_DPCC_RND_OFFS_3_G_SHIFT (8)
// Slice: RND_OFFS_2_RB
// Rank Offset to Neighbor for set 2 red/blue
#define MRV_DPCC_RND_OFFS_2_RB
#define MRV_DPCC_RND_OFFS_2_RB_MASK (0x000000C0)
#define MRV_DPCC_RND_OFFS_2_RB_SHIFT (6)
// Slice: RND_OFFS_2_G
// Rank Offset to Neighbor for set 2 green
#define MRV_DPCC_RND_OFFS_2_G
#define MRV_DPCC_RND_OFFS_2_G_MASK (0x00000030)
#define MRV_DPCC_RND_OFFS_2_G_SHIFT (4)
// Slice: RND_OFFS_1_RB
// Rank Offset to Neighbor for set 1 red/blue
#define MRV_DPCC_RND_OFFS_1_RB
#define MRV_DPCC_RND_OFFS_1_RB_MASK (0x0000000C)
#define MRV_DPCC_RND_OFFS_1_RB_SHIFT (2)
// Slice: RND_OFFS_1_G
// Rank Offset to Neighbor for set 1 green
#define MRV_DPCC_RND_OFFS_1_G
#define MRV_DPCC_RND_OFFS_1_G_MASK (0x00000003)
#define MRV_DPCC_RND_OFFS_1_G_SHIFT (0)

// Register: isp_dpcc_bpt_ctrl: Bad Pixel Table Settings (rw) MRV_DPCC_BASE + 0x005C
// Slice: BPT_RB_3x3
// 1: if BPT active red/blue 9 pixel (3x3) output median
// 0: if BPT active red/blue 4 or 5 pixel output median *Default*
#define MRV_DPCC_BPT_RB_3X3
#define MRV_DPCC_BPT_RB_3X3_MASK (0x00000800)
#define MRV_DPCC_BPT_RB_3X3_SHIFT (11)
// Slice: BPT_G_3x3
// 1: if BPT active green 9 pixel (3x3) output median
// 0: if BPT active green 4 or 5 pixel output median *Default*
#define MRV_DPCC_BPT_G_3X3
#define MRV_DPCC_BPT_G_3X3_MASK (0x00000400)
#define MRV_DPCC_BPT_G_3X3_SHIFT (10)
// Slice: BPT_INCL_RB_CENTER
// 1: if BPT active include center pixel for red/blue output median 2x2+1
// 0: if BPT active do not include center pixel for red/blue output median 2x2 *Default*
#define MRV_DPCC_BPT_INCL_RB_CENTER
#define MRV_DPCC_BPT_INCL_RB_CENTER_MASK (0x00000200)
#define MRV_DPCC_BPT_INCL_RB_CENTER_SHIFT (9)
// Slice: BPT_INCL_GREEN_CENTER
// 1: if BPT active include center pixel for green output median 2x2+1
// 0: if BPT active do not include center pixel for green output median 2x2 *Default*
#define MRV_DPCC_BPT_INCL_GREEN_CENTER
#define MRV_DPCC_BPT_INCL_GREEN_CENTER_MASK (0x00000100)
#define MRV_DPCC_BPT_INCL_GREEN_CENTER_SHIFT (8)
// Slice: BPT_USE_FIX_SET
// 1: for BPT write use hard coded parameter set
// 0: for BPT write do not use hard coded parameter set *Default*
#define MRV_DPCC_BPT_USE_FIX_SET
#define MRV_DPCC_BPT_USE_FIX_SET_MASK (0x00000080)
#define MRV_DPCC_BPT_USE_FIX_SET_SHIFT (7)
// Slice: BPT_USE_SET_3
// 1: for BPT write use parameter set 3
// 0: for BPT write do not use parameter set 3 *Default*
#define MRV_DPCC_BPT_USE_SET_3
#define MRV_DPCC_BPT_USE_SET_3_MASK (0x00000040)
#define MRV_DPCC_BPT_USE_SET_3_SHIFT (6)
// Slice: BPT_USE_SET_2
// 1: for BPT write use parameter set 2
// 0: for BPT write do not use parameter set 2 *Default*
#define MRV_DPCC_BPT_USE_SET_2
#define MRV_DPCC_BPT_USE_SET_2_MASK (0x00000020)
#define MRV_DPCC_BPT_USE_SET_2_SHIFT (5)
// Slice: BPT_USE_SET_1
// 1: for BPT write use parameter set 1
// 0: for BPT write do not use parameter set 1 *Default*
#define MRV_DPCC_BPT_USE_SET_1
#define MRV_DPCC_BPT_USE_SET_1_MASK (0x00000010)
#define MRV_DPCC_BPT_USE_SET_1_SHIFT (4)
// Slice: bpt_cor_en
// table based correction enable
// 0: table based correction is disabled
// 1: table based correction is enabled
#define MRV_DPCC_BPT_COR_EN
#define MRV_DPCC_BPT_COR_EN_MASK (0x00000002)
#define MRV_DPCC_BPT_COR_EN_SHIFT (1)
// Slice: bpt_det_en
// Bad pixel detection write enable
// 0: bad pixel detection write to memory is disabled
// 1: bad pixel detection write to memory is enabled
#define MRV_DPCC_BPT_DET_EN
#define MRV_DPCC_BPT_DET_EN_MASK (0x00000001)
#define MRV_DPCC_BPT_DET_EN_SHIFT (0)

// Register: isp_dpcc_bpt_number: Number of entries for bad pixel table (table based correction) (rw) MRV_BP_BASE + 0x000c
// Slice: bp_number
// Number of bad pixel entries in ISP_BP_TABLE
#define MRV_DPCC_BP_NUMBER
#define MRV_DPCC_BP_NUMBER_MASK (0x00000FFF)
#define MRV_DPCC_BP_NUMBER_SHIFT (0)

// Register: isp_dpcc_bpt_addr: TABLE Address for table-based correction algorithm (rwh) MRV_BP_BASE + 0x0010
// Slice: bp_table_addr
// Setting of RAM start address for initializing
#define MRV_DPCC_BP_TABLE_ADDR
#define MRV_DPCC_BP_TABLE_ADDR_MASK (0x000007FF)
#define MRV_DPCC_BP_TABLE_ADDR_SHIFT (0)

// Register: isp_dpcc_bpt_data: TABLE DATA register for read and write access of table RAM (rw) MRV_BP_BASE + 0x0014
// Slice: v_addr
// Bad pixel vertical address
#define MRV_DPCC_V_ADDR
#define MRV_DPCC_V_ADDR_MASK (0x0FFF0000)
#define MRV_DPCC_V_ADDR_SHIFT (16)
// Slice: h_addr
// Bad pixel horizontal address
#define MRV_DPCC_H_ADDR
#define MRV_DPCC_H_ADDR_MASK (0x00001FFF)
#define MRV_DPCC_H_ADDR_SHIFT (0)

#elif ( MARVIN_FEATURE_DPCC != MARVIN_FEATURE_EXIST_NOT )
#error unknown value for MARVIN_FEATURE_DPCC
#endif  // MARVIN_FEATURE_DPCC
/*---------------------------------------- D P C C (end) ----------------------------------------*/
/*---------------------------------------- D P F (begin) ----------------------------------------*/
#if( MARVIN_FEATURE_DPF == MARVIN_FEATURE_DPF_V1 )
// Register: isp_dpf_mode: Mode control for Denoising Pre-Filter block (rw) MRV_DPF_BASE + 0x0000
// Slice: MRV_DPF_NL_GAIN_ENABLE
// 1: noise level gain will be processed
// 0: AWB gain instead noise level gain will be processed  *Default*
#define MRV_DPF_NL_GAIN_ENABLE
#define MRV_DPF_NL_GAIN_ENABLE_MASK (0x00000200)
#define MRV_DPF_NL_GAIN_ENABLE_SHIFT (9)
// Slice: LSC_GAIN_COMP
// 1: LSC gain will be processed
// 0: LSC gain will not be processed  *Default*
#define MRV_DPF_LSC_GAIN_COMP
#define MRV_DPF_LSC_GAIN_COMP_MASK (0x00000100)
#define MRV_DPF_LSC_GAIN_COMP_SHIFT (8)
// Slice: AWB_GAIN_COMP
// 1: AWB gain will be processed
// 0: AWB gain will not be processed  *Default*
#define MRV_DPF_AWB_GAIN_COMP
#define MRV_DPF_AWB_GAIN_COMP_MASK (0x00000080)
#define MRV_DPF_AWB_GAIN_COMP_SHIFT (7)
// Slice: NLL_SEGMENTATION
// 1: optimized logarithmic like segmentation for Noise Level Lookup (NLL)
// 0: equidistant segmentation for NLL  *Default*
#define MRV_DPF_NLL_SEGMENTATION
#define MRV_DPF_NLL_SEGMENTATION_MASK (0x00000040)
#define MRV_DPF_NLL_SEGMENTATION_SHIFT (6)
// Slice: RB_FILTER_SIZE
// 1: Red and Blue filter kernel size of 9x9 (5x5 active) pixels
// 0: Wide Red and Blue filter kernel size of 13x9 (7x5 active) pixels  *Default*
#define MRV_DPF_RB_FILTER_SIZE
#define MRV_DPF_RB_FILTER_SIZE_MASK (0x00000020)
#define MRV_DPF_RB_FILTER_SIZE_SHIFT (5)
// Slice: R_FILTER_OFF
// 1: disable filter processing for red pixels (R)
// 0: filter R pixels  *Default*
#define MRV_DPF_R_FILTER_OFF
#define MRV_DPF_R_FILTER_OFF_MASK (0x00000010)
#define MRV_DPF_R_FILTER_OFF_SHIFT (4)
// Slice: GR_FILTER_OFF
// 1: disable filter processing for green pixels in green/red lines (GR)
// 0: filter GR pixels  *Default*
#define MRV_DPF_GR_FILTER_OFF
#define MRV_DPF_GR_FILTER_OFF_MASK (0x00000008)
#define MRV_DPF_GR_FILTER_OFF_SHIFT (3)
// Slice: GB_FILTER_OFF
// 1: disable filter processing for green pixels in green/blue lines
// 0: filter GB pixels  *Default*
#define MRV_DPF_GB_FILTER_OFF
#define MRV_DPF_GB_FILTER_OFF_MASK (0x00000004)
#define MRV_DPF_GB_FILTER_OFF_SHIFT (2)
// Slice: B_FILTER_OFF
// 1: disable filter processing for blue pixels (B)
// 0: filter B pixels  *Default*
#define MRV_DPF_B_FILTER_OFF
#define MRV_DPF_B_FILTER_OFF_MASK (0x00000002)
#define MRV_DPF_B_FILTER_OFF_SHIFT (1)
// Slice: DPF_ENABLE
// 1: enable dpf
// 0: bypass dpf  *Default*
#define MRV_DPF_DPF_ENABLE
#define MRV_DPF_DPF_ENABLE_MASK (0x00000001)
#define MRV_DPF_DPF_ENABLE_SHIFT (0)

// Register: isp_dpf_strength_r: filter strength of the RED filter  (rw) MRV_DPF_BASE + 0x0004
// Slice: INV_WEIGHT_R
//   Default is a weight of 1. A higher weight increases the filter strength. 
//   In this register the unsigned 8 bit value 64/weight is stored.
//   The following values show examples: 
//   weight=0.251 -> 255, weight=0.5 -> 128,  
//   weight=1 -> 64 *default*   
//   weight=1.25  -> 51, weight=1.5 -> 42,
//   weight=1.75 -> 37, weight=2 -> 32
#define MRV_DPF_INV_WEIGHT_R
#define MRV_DPF_INV_WEIGHT_R_MASK (0x000000FF)
#define MRV_DPF_INV_WEIGHT_R_SHIFT (0)

// Register: isp_dpf_strength_g: filter strength of the GREEN filter  (rw) MRV_DPF_BASE + 0x0008
// Slice: INV_WEIGHT_G
//   Default is a weight of 1. A higher weight increases the filter strength. 
//   In this register the unsigned 8 bit value 64/weight is stored.
//   The following values show examples: 
//   weight=0.251 -> 255, weight=0.5 -> 128,  
//   weight=1 -> 64 *default*   
//   weight=1.25  -> 51, weight=1.5 -> 42,
//   weight=1.75 -> 37, weight=2 -> 32
#define MRV_DPF_INV_WEIGHT_G
#define MRV_DPF_INV_WEIGHT_G_MASK (0x000000FF)
#define MRV_DPF_INV_WEIGHT_G_SHIFT (0)

// Register: isp_dpf_strength_b: filter strength of the BLUE filter  (rw) MRV_DPF_BASE + 0x000C
// Slice: INV_WEIGHT_B
//   Default is a weight of 1. A higher weight increases the filter strength. 
//   In this register the unsigned 8 bit value 64/weight is stored.
//   The following values show examples: 
//   weight=0.251 -> 255, weight=0.5 -> 128,  
//   weight=1 -> 64 *default*   
//   weight=1.25  -> 51, weight=1.5 -> 42,
//   weight=1.75 -> 37, weight=2 -> 32
#define MRV_DPF_INV_WEIGHT_B
#define MRV_DPF_INV_WEIGHT_B_MASK (0x000000FF)
#define MRV_DPF_INV_WEIGHT_B_SHIFT (0)

// Register: isp_dpf_s_weight_g_1_4: Spatial Filter Coefficients green channel  (rw) MRV_DPF_BASE + 0x0010
// Slice: S_WEIGHT_G4
//    Filter Coefficient green channel s_WEIGHT_G4 
//    5 bit unsigned, value range 1/16 to 16/16 
//    Default value is 16/16 (*Default*)
#define MRV_DPF_S_WEIGHT_G4
#define MRV_DPF_S_WEIGHT_G4_MASK (0x1F000000)
#define MRV_DPF_S_WEIGHT_G4_SHIFT (24)
// Slice: S_WEIGHT_G3
//    Filter Coefficient green channel s_WEIGHT_G3 
//    5 bit unsigned, value range 1/16 to 16/16 
//    Default value is 16/16 (*Default*)
#define MRV_DPF_S_WEIGHT_G3
#define MRV_DPF_S_WEIGHT_G3_MASK (0x001F0000)
#define MRV_DPF_S_WEIGHT_G3_SHIFT (16)
// Slice: S_WEIGHT_G2
//    Filter Coefficient green channel s_WEIGHT_G2 
//    5 bit unsigned, value range 1/16 to 16/16 
//    Default value is 16/16 (*Default*)
#define MRV_DPF_S_WEIGHT_G2
#define MRV_DPF_S_WEIGHT_G2_MASK (0x00001F00)
#define MRV_DPF_S_WEIGHT_G2_SHIFT (8)
// Slice: S_WEIGHT_G1
//    Filter Coefficient green channel s_WEIGHT_G1 
//    5 bit unsigned, value range 1/16 to 16/16 
//    Default value is 16/16 (*Default*)
#define MRV_DPF_S_WEIGHT_G1
#define MRV_DPF_S_WEIGHT_G1_MASK (0x0000001F)
#define MRV_DPF_S_WEIGHT_G1_SHIFT (0)

// Register: isp_dpf_s_weight_g_5_6: Spatial Filter Coefficients green channel  (rw) MRV_DPF_BASE + 0x0014
// Slice: S_WEIGHT_G6
//    Filter Coefficient green channel s_WEIGHT_G6 
//    5 bit unsigned, value range 1/16 to 16/16 
//    Default value is 16/16 (*Default*)
#define MRV_DPF_S_WEIGHT_G6
#define MRV_DPF_S_WEIGHT_G6_MASK (0x00001F00)
#define MRV_DPF_S_WEIGHT_G6_SHIFT (8)
// Slice: S_WEIGHT_G5
//    Filter Coefficient green channel s_WEIGHT_G5 
//    5 bit unsigned, value range 1/16 to 16/16 
//    Default value is 16/16 (*Default*)
#define MRV_DPF_S_WEIGHT_G5
#define MRV_DPF_S_WEIGHT_G5_MASK (0x0000001F)
#define MRV_DPF_S_WEIGHT_G5_SHIFT (0)

// Register: isp_dpf_s_weight_rb_1_4: Spatial Filter Coefficients red/blue channel  (rw) MRV_DPF_BASE + 0x0018
// Slice: S_WEIGHT_RB4
//    Filter Coefficient red/blue channel s_WEIGHT_RB4 
//    5 bit unsigned, value range 1/16 to 16/16 
//    Default value is 16/16 (*Default*)
#define MRV_DPF_S_WEIGHT_RB4
#define MRV_DPF_S_WEIGHT_RB4_MASK (0x1F000000)
#define MRV_DPF_S_WEIGHT_RB4_SHIFT (24)
// Slice: S_WEIGHT_RB3
//    Filter Coefficient red/blue channel s_WEIGHT_RB3 
//    5 bit unsigned, value range 1/16 to 16/16 
//    Default value is 16/16 (*Default*)
#define MRV_DPF_S_WEIGHT_RB3
#define MRV_DPF_S_WEIGHT_RB3_MASK (0x001F0000)
#define MRV_DPF_S_WEIGHT_RB3_SHIFT (16)
// Slice: S_WEIGHT_RB2
//    Filter Coefficient red/blue channel s_WEIGHT_RB2 
//    5 bit unsigned, value range 1/16 to 16/16 
//    Default value is 16/16 (*Default*)
#define MRV_DPF_S_WEIGHT_RB2
#define MRV_DPF_S_WEIGHT_RB2_MASK (0x00001F00)
#define MRV_DPF_S_WEIGHT_RB2_SHIFT (8)
// Slice: S_WEIGHT_RB1
//    Filter Coefficient red/blue channel s_WEIGHT_RB1 
//    5 bit unsigned, value range 1/16 to 16/16 
//    Default value is 16/16 (*Default*)
#define MRV_DPF_S_WEIGHT_RB1
#define MRV_DPF_S_WEIGHT_RB1_MASK (0x0000001F)
#define MRV_DPF_S_WEIGHT_RB1_SHIFT (0)

// Register: isp_dpf_s_weight_rb_5_6: Spatial Filter Coefficients red/blue channel  (rw) MRV_DPF_BASE + 0x001C
// Slice: S_WEIGHT_RB6
//    Filter Coefficient red/blue channel s_WEIGHT_RB6 
//    5 bit unsigned, value range 1/16 to 16/16 
//    Default value is 16/16 (*Default*)
#define MRV_DPF_S_WEIGHT_RB6
#define MRV_DPF_S_WEIGHT_RB6_MASK (0x00001F00)
#define MRV_DPF_S_WEIGHT_RB6_SHIFT (8)
// Slice: S_WEIGHT_RB5
//    Filter Coefficient red/blue channel s_WEIGHT_RB5 
//    5 bit unsigned, value range 1/16 to 16/16 
//    Default value is 16/16 (*Default*)
#define MRV_DPF_S_WEIGHT_RB5
#define MRV_DPF_S_WEIGHT_RB5_MASK (0x0000001F)
#define MRV_DPF_S_WEIGHT_RB5_SHIFT (0)

// Register: isp_dpf_nll_coeff_0: Noise Level Lookup Coefficient  (rw) MRV_DPF_BASE + 0x0020
// Slice: NLL_COEFF_0
//    Noise Level Lookup Table Coefficient nll_coeff_0 
//    8 bit unsigned, value range 1/1024 to 1023/1024 (*Default*)
#define MRV_DPF_NLL_COEFF_0
#define MRV_DPF_NLL_COEFF_0_MASK (0x000003FF)
#define MRV_DPF_NLL_COEFF_0_SHIFT (0)

// Register: isp_dpf_nll_coeff_1: Noise Level Lookup Coefficient  (rw) MRV_DPF_BASE + 0x0024
// Slice: NLL_COEFF_1
//    Noise Level Lookup Table Coefficient nll_coeff_1 
//    8 bit unsigned, value range 1/1024 to 1023/1024 (*Default*)
#define MRV_DPF_NLL_COEFF_1
#define MRV_DPF_NLL_COEFF_1_MASK (0x000003FF)
#define MRV_DPF_NLL_COEFF_1_SHIFT (0)

// Register: isp_dpf_nll_coeff_2: Noise Level Lookup Coefficient  (rw) MRV_DPF_BASE + 0x0028
// Slice: NLL_COEFF_2
//    Noise Level Lookup Table Coefficient nll_coeff_2 
//    8 bit unsigned, value range 1/1024 to 1023/1024 (*Default*)
#define MRV_DPF_NLL_COEFF_2
#define MRV_DPF_NLL_COEFF_2_MASK (0x000003FF)
#define MRV_DPF_NLL_COEFF_2_SHIFT (0)

// Register: isp_dpf_nll_coeff_3: Noise Level Lookup Coefficient  (rw) MRV_DPF_BASE + 0x002C
// Slice: NLL_COEFF_3
//    Noise Level Lookup Table Coefficient nll_coeff_3 
//    8 bit unsigned, value range 1/1024 to 1023/1024 (*Default*)
#define MRV_DPF_NLL_COEFF_3
#define MRV_DPF_NLL_COEFF_3_MASK (0x000003FF)
#define MRV_DPF_NLL_COEFF_3_SHIFT (0)

// Register: isp_dpf_nll_coeff_4: Noise Level Lookup Coefficient  (rw) MRV_DPF_BASE + 0x0030
// Slice: NLL_COEFF_4
//    Noise Level Lookup Table Coefficient nll_coeff_4 
//    8 bit unsigned, value range 1/1024 to 1023/1024 (*Default*)
#define MRV_DPF_NLL_COEFF_4
#define MRV_DPF_NLL_COEFF_4_MASK (0x000003FF)
#define MRV_DPF_NLL_COEFF_4_SHIFT (0)

// Register: isp_dpf_nll_coeff_5: Noise Level Lookup Coefficient  (rw) MRV_DPF_BASE + 0x0034
// Slice: NLL_COEFF_5
//    Noise Level Lookup Table Coefficient nll_coeff_5 
//    8 bit unsigned, value range 1/1024 to 1023/1024 (*Default*)
#define MRV_DPF_NLL_COEFF_5
#define MRV_DPF_NLL_COEFF_5_MASK (0x000003FF)
#define MRV_DPF_NLL_COEFF_5_SHIFT (0)

// Register: isp_dpf_nll_coeff_6: Noise Level Lookup Coefficient  (rw) MRV_DPF_BASE + 0x0038
// Slice: NLL_COEFF_6
//    Noise Level Lookup Table Coefficient nll_coeff_6 
//    8 bit unsigned, value range 1/1024 to 1023/1024 (*Default*)
#define MRV_DPF_NLL_COEFF_6
#define MRV_DPF_NLL_COEFF_6_MASK (0x000003FF)
#define MRV_DPF_NLL_COEFF_6_SHIFT (0)

// Register: isp_dpf_nll_coeff_7: Noise Level Lookup Coefficient  (rw) MRV_DPF_BASE + 0x003C
// Slice: NLL_COEFF_7
//    Noise Level Lookup Table Coefficient nll_coeff_7 
//    8 bit unsigned, value range 1/1024 to 1023/1024 (*Default*)
#define MRV_DPF_NLL_COEFF_7
#define MRV_DPF_NLL_COEFF_7_MASK (0x000003FF)
#define MRV_DPF_NLL_COEFF_7_SHIFT (0)

// Register: isp_dpf_nll_coeff_8: Noise Level Lookup Coefficient  (rw) MRV_DPF_BASE + 0x0040
// Slice: NLL_COEFF_8
//    Noise Level Lookup Table Coefficient nll_coeff_8 
//    8 bit unsigned, value range 1/1024 to 1023/1024 (*Default*)
#define MRV_DPF_NLL_COEFF_8
#define MRV_DPF_NLL_COEFF_8_MASK (0x000003FF)
#define MRV_DPF_NLL_COEFF_8_SHIFT (0)

// Register: isp_dpf_nll_coeff_9: Noise Level Lookup Coefficient  (rw) MRV_DPF_BASE + 0x0044
// Slice: NLL_COEFF_9
//    Noise Level Lookup Table Coefficient nll_coeff_9 
//    8 bit unsigned, value range 1/1024 to 1023/1024 (*Default*)
#define MRV_DPF_NLL_COEFF_9
#define MRV_DPF_NLL_COEFF_9_MASK (0x000003FF)
#define MRV_DPF_NLL_COEFF_9_SHIFT (0)

// Register: isp_dpf_nll_coeff_10: Noise Level Lookup Coefficient  (rw) MRV_DPF_BASE + 0x0048
// Slice: NLL_COEFF_10
//    Noise Level Lookup Table Coefficient nll_coeff_10 
//    8 bit unsigned, value range 1/1024 to 1023/1024 (*Default*)
#define MRV_DPF_NLL_COEFF_10
#define MRV_DPF_NLL_COEFF_10_MASK (0x000003FF)
#define MRV_DPF_NLL_COEFF_10_SHIFT (0)

// Register: isp_dpf_nll_coeff_11: Noise Level Lookup Coefficient  (rw) MRV_DPF_BASE + 0x004C
// Slice: NLL_COEFF_11
//    Noise Level Lookup Table Coefficient nll_coeff_11 
//    8 bit unsigned, value range 1/1024 to 1023/1024 (*Default*)
#define MRV_DPF_NLL_COEFF_11
#define MRV_DPF_NLL_COEFF_11_MASK (0x000003FF)
#define MRV_DPF_NLL_COEFF_11_SHIFT (0)

// Register: isp_dpf_nll_coeff_12: Noise Level Lookup Coefficient  (rw) MRV_DPF_BASE + 0x0050
// Slice: NLL_COEFF_12
//    Noise Level Lookup Table Coefficient nll_coeff_12 
//    8 bit unsigned, value range 1/1024 to 1023/1024 (*Default*)
#define MRV_DPF_NLL_COEFF_12
#define MRV_DPF_NLL_COEFF_12_MASK (0x000003FF)
#define MRV_DPF_NLL_COEFF_12_SHIFT (0)

// Register: isp_dpf_nll_coeff_13: Noise Level Lookup Coefficient  (rw) MRV_DPF_BASE + 0x0054
// Slice: NLL_COEFF_13
//    Noise Level Lookup Table Coefficient nll_coeff_13 
//    8 bit unsigned, value range 1/1024 to 1023/1024 (*Default*)
#define MRV_DPF_NLL_COEFF_13
#define MRV_DPF_NLL_COEFF_13_MASK (0x000003FF)
#define MRV_DPF_NLL_COEFF_13_SHIFT (0)

// Register: isp_dpf_nll_coeff_14: Noise Level Lookup Coefficient  (rw) MRV_DPF_BASE + 0x0058
// Slice: NLL_COEFF_14
//    Noise Level Lookup Table Coefficient nll_coeff_14 
//    8 bit unsigned, value range 1/1024 to 1023/1024 (*Default*)
#define MRV_DPF_NLL_COEFF_14
#define MRV_DPF_NLL_COEFF_14_MASK (0x000003FF)
#define MRV_DPF_NLL_COEFF_14_SHIFT (0)

// Register: isp_dpf_nll_coeff_15: Noise Level Lookup Coefficient  (rw) MRV_DPF_BASE + 0x005C
// Slice: NLL_COEFF_15
//    Noise Level Lookup Table Coefficient nll_coeff_15 
//    8 bit unsigned, value range 1/1024 to 1023/1024 (*Default*)
#define MRV_DPF_NLL_COEFF_15
#define MRV_DPF_NLL_COEFF_15_MASK (0x000003FF)
#define MRV_DPF_NLL_COEFF_15_SHIFT (0)

// Register: isp_dpf_nll_coeff_16: Noise Level Lookup Coefficient  (rw) MRV_DPF_BASE + 0x0060
// Slice: NLL_COEFF_16
//    Noise Level Lookup Table Coefficient nll_coeff_16 
//    8 bit unsigned, value range 1/1024 to 1023/1024 (*Default*)
#define MRV_DPF_NLL_COEFF_16
#define MRV_DPF_NLL_COEFF_16_MASK (0x000003FF)
#define MRV_DPF_NLL_COEFF_16_SHIFT (0)

// Register: isp_dpf_nl_r_gain: Noise Level Gain  (rw) MRV_DPF_BASE + 0x0064
// Slice: MRV_DPF_NL_R_GAIN
//    Noise Level Gain  
//    12 bit unsigned, value range 1/256 to 15 + 255/256 
#define MRV_DPF_NL_R_GAIN
#define MRV_DPF_NL_R_GAIN_MASK (0x00000FFF)
#define MRV_DPF_NL_R_GAIN_SHIFT (0)

// Register: isp_dpf_nl_gr_gain: Noise Level Gain  (rw) MRV_DPF_BASE + 0x0064
// Slice: MRV_DPF_NL_GR_GAIN
//    Noise Level Gain  
//    12 bit unsigned, value range 1/256 to 15 + 255/256 
#define MRV_DPF_NL_GR_GAIN
#define MRV_DPF_NL_GR_GAIN_MASK (0x00000FFF)
#define MRV_DPF_NL_GR_GAIN_SHIFT (0)

// Register: isp_dpf_nl_gb_gain: Noise Level Gain  (rw) MRV_DPF_BASE + 0x0064
// Slice: MRV_DPF_NL_GB_GAIN
//    Noise Level Gain  
//    12 bit unsigned, value range 1/256 to 15 + 255/256 
#define MRV_DPF_NL_GB_GAIN
#define MRV_DPF_NL_GB_GAIN_MASK (0x00000FFF)
#define MRV_DPF_NL_GB_GAIN_SHIFT (0)

// Register: isp_dpf_nl_b_gain: Noise Level Gain  (rw) MRV_DPF_BASE + 0x0064
// Slice: MRV_DPF_NL_B_GAIN
//    Noise Level Gain  
//    12 bit unsigned, value range 1/256 to 15 + 255/256 
#define MRV_DPF_NL_B_GAIN
#define MRV_DPF_NL_B_GAIN_MASK (0x00000FFF)
#define MRV_DPF_NL_B_GAIN_SHIFT (0)

#elif ( MARVIN_FEATURE_DPF != MARVIN_FEATURE_EXIST_NOT )
#error unknown value for MARVIN_FEATURE_DPF
#endif  // MARVIN_FEATURE_DPF
/*---------------------------------------- D P F (end) ----------------------------------------*/

/********************************************************************/
/* ISP WIDE DYNAMIC RANGE Registers                                 */
/********************************************************************/

    // Register: isp_wdr_ctrl: Control Bits for Wide Dynamic Range Unit
    // Slice: WDR_enable
    // 1: enable WDR
    // 0: bypass WDR
    #define MRV_WDR_ENABLE
    #define MRV_WDR_ENABLE_MASK 0x00000001
    #define MRV_WDR_ENABLE_SHIFT 0
    #define MRV_WDR_MASK        0x0000003F
    // Slice: WDR_color_space_select
    // 1: R, G, B color space
    // 0: Luminance/ Chrominance color space
    #define MRV_WDR_COL_SPACE_SEL
    #define MRV_WDR_COL_SPACE_SEL_MASK 0x00000002
    #define MRV_WDR_COL_SPACE_SEL_SHIFT 1
    // Slice: WDR_cr_mapping_enable
    // 1: bypass Crominance Mapping
    // 0: enable Crominance Mapping
    #define MRV_WDR_CHR_BYPASS
    #define MRV_WDR_CHR_BYPASS_MASK 0x00000004
    #define MRV_WDR_CHR_BYPASS_SHIFT 2
    // Slice: WDR_use_IRef
    // 1: use Iref instaed of Y for ToneMapping and Gain calculation
    // 0: use Y for ToneMapping and Gain calculation  *Default* 
    #define MRV_WDR_USE_IREF
    #define MRV_WDR_USE_IREF_MASK 0x00000008
    #define MRV_WDR_USE_IREF_SHIFT 3
    // Slice: WDR_use_Y9_8
    // 1: use R, G, B and Y*9/8 for maximum value calculation
    // 0: only use R, G, B for maximum value calculation  *Default* 
    // requires that Iref has been selected 
    #define MRV_WDR_USE_Y9_8
    #define MRV_WDR_USE_Y9_8_MASK 0x00000010
    #define MRV_WDR_USE_Y9_8_SHIFT 4 
    // Slice: WDR_use_RGB7_8
    // 1: decrease RGB_max by 7/8 (for noise reduction)
    // 0: do not modify RGB_max *Default* 
    // requires that Iref has been selected 
    #define MRV_WDR_USE_RGB7_8
    #define MRV_WDR_USE_RGB7_8_MASK 0x00000020
    #define MRV_WDR_USE_RGB7_8_SHIFT 5   
    // Slice: WDR_disable_transient
    // 1: disable transient between Y and Iref
    // 0: calculate transient between Y and Iref *Default*
    // requires that Iref has been selected 
    #define MRV_WDR_DISABLE_TRANSIENT
    #define MRV_WDR_DISABLE_TRANSIENT_MASK 0x00000040
    #define MRV_WDR_DISABLE_TRANSIENT_SHIFT 6   
    // Slice: WDR_CONTROL
    // defines all control bits 
    #define MRV_WDR_CONTROL
    #define MRV_WDR_CONTROL_MASK 0x0000007F
    #define MRV_WDR_CONTROL_SHIFT 0   
    // Slice: WDR_rgb_factor
    // defines Iref = (WDR_RGB_FACTOR * RGBmax + (8-WDR_RGB_FACTOR) * Y)/8
    // requires that Iref has been selected 
    #define MRV_WDR_RGB_FACTOR
    #define MRV_WDR_RGB_FACTOR_MASK 0x00000F00
    #define MRV_WDR_RGB_FACTOR_SHIFT 8         
    #define MRV_WDR_RGB_FACTOR_INV_MASK 0xFFFFF0FF           
    // Register: isp_wdr_tonecurve_dyn_n: Tone curve sample points
    // Slice: WDR_dyn
    // Ton curve sample point dy2 on the horizontal axis
    #define MRV_WDR_DYN
    #define MRV_WDR_DYN_MASK 0x77777777
    #define MRV_WDR_DYN_SHIFT 0
    #define MRV_WDR_DYN_INV_MASK 0x88888888
    // Register: isp_wdr_tonecurve_ym: Tone curve sample points    
    // Slice: tonecurve_ym
    // Ton curve value y axis
    #define MRV_WDR_YM
    #define MRV_WDR_YM_MASK 0x00001FFF
    #define MRV_WDR_YM_SHIFT 0
    #define MRV_WDR_YM_INV_MASK 0xFFFFE000
    // Register: isp_wdr_offset:    
    // Slice: lum_offset
    // Luminance Offset (a) for RGB operation mode 
    #define MRV_WDR_LUM_OFFSET
    #define MRV_WDR_LUM_OFFSET_MASK 0x0FFF0000
    #define MRV_WDR_LUM_OFFSET_SHIFT 16
    // Slice: RGB_offset
    // RGB Offset (b) for RGB operation mode 
    #define MRV_WDR_RGB_OFFSET
    #define MRV_WDR_RGB_OFFSET_MASK 0x00000FFF
    #define MRV_WDR_RGB_OFFSET_SHIFT 0
    #define MRV_WDR_RGB_OFFSET_INV_MASK 0xFFFFF000
    // Register: isp_wdr_dmin_threshold:
    // Slice: Dmin_strength
    // strenght factor for DMIN
    #define MRV_WDR_DMIN_STRENGTH
    #define MRV_WDR_DMIN_STRENGTH_MASK 0x001F0000
    #define MRV_WDR_DMIN_STRENGTH_SHIFT 16
    #define MRV_WDR_DMIN_STRENGTH_INV_MASK 0xFFE0FFFF
    // Slice: Dmin_threshold
    // Lower threshold for deltaMin value 
    #define MRV_WDR_DMIN_THRES
    #define MRV_WDR_DMIN_THRES_MASK 0x00000FFF
    #define MRV_WDR_DMIN_THRES_SHIFT 0
    #define MRV_WDR_DMIN_THRES_INV_MASK 0xFFFFF000
/********************************************************************/
/* Private prototypes                                               */
/********************************************************************/
RESULT MrvWaitForVSyncHelper (void);
void   MrvJpeSetTables( UINT8 ucCompressionRatio );
void   MrvJpeSelectTables( void );
void   MrvJpeSetConfig( UINT16 usHSize, UINT16 usVSize, BOOL bJpeScale );
RESULT MrvJpeGenerateHeader( UINT8 ucHeaderMode );
void MrvJpePrepEnc( teMrvJpeEncMode tMrvJpeEncMode );
RESULT MrvJpeWaitForHeaderGenDone( void );
RESULT MrvJpeWaitForEncodeDone( void );


#endif //_MRV_PRIV_H
