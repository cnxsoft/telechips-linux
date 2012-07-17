/*
 * Copyright (C) 2007 Texas Instruments Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*ths8200_encoder.h*/

#ifndef THS8200_ENCODER_H
#define THS8200_ENCODER_H


struct ths8200_std_info {
	int reg;
	u32 val;
};

#define THS8200_I2C_ADDR			0x40
#define THS8200_I2C_DEVICE			(THS8200_I2C_ADDR>>1)

extern int ths8200_i2c_write(unsigned char reg, unsigned char val);
extern int ths8200_i2c_read(unsigned char reg, unsigned char *val);
extern void ths8200_reset(void);
extern int ths8200_set_ready(void);
extern void ths8200_set_end(void);
extern void ths8200_set_mode(int type);
extern void ths8200_enable(int type);

/* Register Macros */
#define THS8200_VERSION             0x02
#define THS8200_CHIP_CTL            0x03
#define THS8200_CSC_R11             0x04
#define THS8200_CSC_R12             0x05
#define THS8200_CSC_R21             0x06
#define THS8200_CSC_R22             0x07
#define THS8200_CSC_R31             0x08
#define THS8200_CSC_R32             0x09
#define THS8200_CSC_G11             0x0a
#define THS8200_CSC_G12             0x0b
#define THS8200_CSC_G21             0x0c
#define THS8200_CSC_G22             0x0d
#define THS8200_CSC_G31             0x0e
#define THS8200_CSC_G32             0x0f
#define THS8200_CSC_B11             0x10
#define THS8200_CSC_B12             0x11
#define THS8200_CSC_B21             0x12
#define THS8200_CSC_B22             0x13
#define THS8200_CSC_B31             0x14
#define THS8200_CSC_B32             0x15
#define THS8200_CSC_OFFS1           0x16
#define THS8200_CSC_OFFS12          0x17
#define THS8200_CSC_OFFS23          0x18
#define THS8200_CSC_OFFS3           0x19
#define THS8200_TST_CNTL1           0x1a
#define THS8200_TST_CNTL2           0x1b
#define THS8200_DATA_CNTL           0x1c
#define THS8200_DTG1_Y_SYNC1_LSB    0x1d
#define THS8200_DTG1_Y_SYNC2_LSB    0x1e
#define THS8200_DTG1_Y_SYNC3_LSB    0x1f
#define THS8200_DTG1_CBCR_SYNC1_LSB 0x20
#define THS8200_DTG1_CBCR_SYNC2_LSB 0x21
#define THS8200_DTG1_CBCR_SYNC3_LSB 0x22
#define THS8200_DTG1_Y_SYNC_MSB     0x23
#define THS8200_DTG1_CBCR_SYNC_MSB  0x24
#define THS8200_DTG1_SPEC_A         0x25
#define THS8200_DTG1_SPEC_B         0x26
#define THS8200_DTG1_SPEC_C         0x27
#define THS8200_DTG1_SPEC_D_LSB     0x28
#define THS8200_DTG1_SPEC_D1        0x29
#define THS8200_DTG1_SPEC_E_LSB     0x2a
#define THS8200_DTG1_SPEC_DEH_MSB   0x2b
#define THS8200_DTG1_SPEC_H_LSB     0x2c
#define THS8200_DTG1_SPEC_I_MSB     0x2d
#define THS8200_DTG1_SPEC_I_LSB     0x2e
#define THS8200_DTG1_SPEC_K_LSB     0x2f
#define THS8200_DTG1_SPEC_K_MSB     0x30
#define THS8200_DTG1_SPEC_K1        0x31
#define THS8200_DTG1_SPEC_G_LSB     0x32
#define THS8200_DTG1_SPEC_G_MSB     0x33
#define THS8200_DTG1_TOT_PIXELS_MSB 0x34
#define THS8200_DTG1_TOT_PIXELS_LSB 0x35
#define THS8200_DTG1_FLD_FLIP_LINECNT_MSB 0x36
#define THS8200_DTG1_LINECNT_LSB    0x37
#define THS8200_DTG1_MODE           0x38
#define THS8200_DTG1_FRAME_FIELD_SZ_MSB   0x39
#define THS8200_DTG1_FRAME_SZ_LSB   0x3a
#define THS8200_DTG1_FIELD_SZ_LSB   0x3b
#define THS8200_DTG1_VESA_CBAR_SIZE 0x3c
#define THS8200_DAC_CNTL_MSB        0x3d
#define THS8200_DAC1_CNTL_LSB       0x3e
#define THS8200_DAC2_CNTL_LSB       0x3f
#define THS8200_DAC3_CNTL_LSB       0x40
#define THS8200_CSM_CLIP_GY_LOW     0x41
#define THS8200_CSM_CLIP_BCB_LOW    0x42
#define THS8200_CSM_CLIP_RCR_LOW    0x43
#define THS8200_CSM_CLIP_GY_HIGH    0x44
#define THS8200_CSM_CLIP_BCB_HIGH   0x45
#define THS8200_CSM_CLIP_RCR_HIGH   0x46
#define THS8200_CSM_SHIFT_GY        0x47
#define THS8200_CSM_SHIFT_BCB       0x48
#define THS8200_CSM_SHIFT_RCR       0x49
#define THS8200_CSM_GY_CNTL_MULT_MSB     0x4a
#define THS8200_CSM_MULT_BCB_RCR_MSB     0x4b
#define THS8200_CSM_MULT_GY_LSB     0x4c
#define THS8200_CSM_MULT_BCB_LSB    0x4d
#define THS8200_CSM_MULT_RCR_LSB    0x4e
#define THS8200_CSM_MULT_RCR_BCB_CNTL    0x4f
#define THS8200_CSM_MULT_RCR_LSB    0x4e
#define THS8200_DTG2_BP1_2_MSB      0x50
#define THS8200_DTG2_BP3_4_MSB      0x51
#define THS8200_DTG2_BP5_6_MSB      0x52
#define THS8200_DTG2_BP7_8_MSB      0x53
#define THS8200_DTG2_BP9_10_MSB     0x54
#define THS8200_DTG2_BP11_12_MSB    0x55
#define THS8200_DTG2_BP13_14_MSB    0x56
#define THS8200_DTG2_BP15_16_MSB    0x57
#define THS8200_DTG2_BP1_LSB        0x58
#define THS8200_DTG2_BP2_LSB        0x59
#define THS8200_DTG2_BP3_LSB        0x5a
#define THS8200_DTG2_BP4_LSB        0x5b
#define THS8200_DTG2_BP5_LSB        0x5c
#define THS8200_DTG2_BP6_LSB        0x5d
#define THS8200_DTG2_BP7_LSB        0x5e
#define THS8200_DTG2_BP8_LSB        0x5f
#define THS8200_DTG2_BP9_LSB        0x60
#define THS8200_DTG2_BP10_LSB       0x61
#define THS8200_DTG2_BP11_LSB       0x62
#define THS8200_DTG2_BP12_LSB       0x63
#define THS8200_DTG2_BP13_LSB       0x64
#define THS8200_DTG2_BP14_LSB       0x65
#define THS8200_DTG2_BP15_LSB       0x66
#define THS8200_DTG2_BP16_LSB       0x67
#define THS8200_DTG2_LINETYPE1      0x68
#define THS8200_DTG2_LINETYPE2      0x69
#define THS8200_DTG2_LINETYPE3      0x6a
#define THS8200_DTG2_LINETYPE4      0x6b
#define THS8200_DTG2_LINETYPE5      0x6c
#define THS8200_DTG2_LINETYPE6      0x6d
#define THS8200_DTG2_LINETYPE7      0x6e
#define THS8200_DTG2_LINETYPE8      0x6f
#define THS8200_DTG2_HLENGTH_LSB    0x70
#define THS8200_DTG2_HLENGTH_LSB_HDLY_MSB    0x71
#define THS8200_DTG2_HLENGTH_HDLY_LSB        0x72
#define THS8200_DTG2_VLENGTH1_LSB   0x73
#define THS8200_DTG2_VLENGTH1_MSB_VDLY1_MSB  0x74
#define THS8200_DTG2_VDLY1_LSB      0x75
#define THS8200_DTG2_VLENGTH2_LSB   0x76
#define THS8200_DTG2_VLENGTH2_MSB_VDLY2_MSB  0x77
#define THS8200_DTG2_VDLY2_LSB      0x78
#define THS8200_DTG2_HS_IN_DLY_MSB  0x79
#define THS8200_DTG2_HS_IN_DLY_LSB  0x7a
#define THS8200_DTG2_VS_IN_DLY_MSB  0x7b
#define THS8200_DTG2_VS_IN_DLY_LSB  0x7c
#define THS8200_DTG2_PIXEL_CNT_MSB  0x7d
#define THS8200_DTG2_PIXEL_CNT_LSB  0x7e
#define THS8200_DTG2_LINE_CNT_MSB   0x7f
#define THS8200_DTG2_LINE_CNT_LSB   0x80
#define THS8200_DTG2_CNTL           0x82
#define THS8200_CGMS_CNTL_HEADER    0x83
#define THS8200_CGMS_PAYLOAD_MSB    0x84
#define THS8200_CGMS_PAYLOAD_LSB    0x85
#define THS8200_MISC_PPL_LSB        0x86
#define THS8200_MISC_PPL_MSB        0x87
#define THS8200_MISC_LPF_MSB        0x88
#define THS8200_MISC_LPF_LSB        0x89

/* Default values for the registers */
#define THS8200_DTG1_Y_SYNC1_LSB_DEFAULT    0xff
#define THS8200_DTG1_Y_SYNC2_LSB_DEFAULT    0x49
#define THS8200_DTG1_Y_SYNC3_LSB_DEFAULT    0xb6
#define THS8200_DTG1_CBCR_SYNC1_LSB_DEFAULT 0xff
#define THS8200_DTG1_CBCR_SYNC2_LSB_DEFAULT 0xff
#define THS8200_DTG1_CBCR_SYNC3_LSB_DEFAULT 0xff
#define THS8200_DTG1_Y_SYNC_MSB_DEFAULT     0x13
#define THS8200_DTG1_CBCR_SYNC_MSB_DEFAULT  0x15
#define THS8200_DTG1_SPEC_H_LSB_DEFAULT     0x0
#define THS8200_DTG1_SPEC_K_MSB_DEFAULT     0x0
#define THS8200_DTG1_FLD_FLIP_LINECNT_MSB_DEFAULT 0x0
#define THS8200_DTG1_LINECNT_LSB_DEFAULT 0x1
#define THS8200_DTG2_HS_IN_DLY_MSB_DEFAULT 0x0

/* defaults for 720P HD */
#define THS8200_DTG2_CNTL_720P_DEFAULT    0x1b
#define THS8200_DTG1_SPEC_A_720P_DEFAULT  0x28
/* distance from end of active video to start of sync */
#define THS8200_DTG1_SPEC_B_720P_DEFAULT  0x6e//0x46
#define THS8200_DTG1_SPEC_C_720P_DEFAULT  0x28
#define THS8200_DTG1_SPEC_D_LSB_720P_DEFAULT 0x04//0x2c
/* distance from Hsync to start of active video */
#define THS8200_DTG1_SPEC_E_LSB_720P_DEFAULT 0x04//0x2c
#define THS8200_DTG1_SPEC_DEH_MSB_720P_DEFAULT 0xc0
#define THS8200_DTG1_SPEC_K_LSB_720P_DEFAULT 0x6e//0x46
#define THS8200_DTG1_TOT_PIXELS_MSB_720P_DEFAULT 0x06
#define THS8200_DTG1_TOT_PIXELS_LSB_720P_DEFAULT 0x72
#define THS8200_DTG1_MODE_720P_DEFAULT 0x82
#define THS8200_DTG1_FRAME_FIELD_SZ_MSB_720P_DEFAULT 0x27
#define THS8200_DTG1_FRAME_SZ_LSB_720P_DEFAULT 0xee
#define THS8200_DTG1_FIELD_SZ_LSB_720P_DEFAULT 0xff
#define THS8200_DTG2_HS_IN_DLY_LSB_720P_DEFAULT 0x60
#define THS8200_DTG2_VS_IN_DLY_MSB_720P_DEFAULT 0x00//0x08
#define THS8200_DTG2_VS_IN_DLY_LSB_720P_DEFAULT 0x07//0x06

/* defaults for 1080i HD */
#define THS8200_TST_CNTL1_1080I_DEFAULT             0x0
#define THS8200_TST_CNTL2_1080I_DEFAULT             0x0
#define THS8200_CSM_GY_CNTL_MULT_MSB_1080I_DEFAULT  0x0
#define THS8200_DTG2_CNTL_1080I_DEFAULT 0x9b//0x1b//0x1f
#define THS8200_DTG1_SPEC_A_1080I_DEFAULT 0x2c
/* distance from end of active video to start of sync */
#define THS8200_DTG1_SPEC_B_1080I_DEFAULT 0x58
#define THS8200_DTG1_SPEC_C_1080I_DEFAULT 0x2c
#define THS8200_DTG1_SPEC_D1_1080I_DEFAULT 0x0
#define THS8200_DTG1_SPEC_D_LSB_1080I_DEFAULT 0x84
/* distance from Hsync to start of active video */
#define THS8200_DTG1_SPEC_E_LSB_1080I_DEFAULT 0xc0
#define THS8200_DTG1_SPEC_DEH_MSB_1080I_DEFAULT 0x0
#define THS8200_DTG1_SPEC_K_LSB_1080I_DEFAULT 0x58
#define THS8200_DTG1_SPEC_G_LSB_1080I_DEFAULT 0x58
#define THS8200_DTG1_SPEC_G_MSB_1080I_DEFAULT 0x0
#define THS8200_DTG1_TOT_PIXELS_MSB_1080I_DEFAULT 0x08
/* Total pixels per line */
#define THS8200_DTG1_TOT_PIXELS_LSB_1080I_DEFAULT 0x98
#define THS8200_DTG1_MODE_1080I_DEFAULT 0x81
#define THS8200_DTG1_FRAME_FIELD_SZ_MSB_1080I_DEFAULT 0x42
#define THS8200_DTG1_FRAME_SZ_LSB_1080I_DEFAULT 0x65
#define THS8200_DTG1_FIELD_SZ_LSB_1080I_DEFAULT 0x33
#define THS8200_DTG2_HLENGTH_LSB_1080I_DEFAULT 0x58
#define THS8200_DTG2_HLENGTH_LSB_HDLY_MSB_1080I_DEFAULT 0x0
#define THS8200_DTG2_HLENGTH_HDLY_LSB_1080I_DEFAULT 0x0
#define THS8200_DTG2_VLENGTH1_LSB_1080I_DEFAULT 0x05
#define THS8200_DTG2_VLENGTH1_MSB_VDLY1_MSB_1080I_DEFAULT 0x0
#define THS8200_DTG2_VDLY1_LSB_1080I_DEFAULT 0x0
#define THS8200_DTG2_VLENGTH2_LSB_1080I_DEFAULT 0x5
#define THS8200_DTG2_VDLY2_LSB_1080I_DEFAULT 0x0
#define THS8200_DTG2_VLENGTH2_MSB_VDLY2_MSB_1080I_DEFAULT 0x0
#define THS8200_DTG2_VDLY1_LSB_1080I_DEFAULT 0x0
#define THS8200_DTG2_HS_IN_DLY_LSB_1080I_DEFAULT 0x44
#define THS8200_DTG2_VS_IN_DLY_MSB_1080I_DEFAULT 0x0
#define THS8200_DTG2_VS_IN_DLY_LSB_1080I_DEFAULT 0x1

/* MASKS */

/* MODE VALUES */
#define THS8200_ATSCHD_MODE_1080P    0x0
#define THS8200_ATSCHD_MODE_1080I    0x1
#define THS8200_ATSCHD_MODE_720P     0x2
#define THS8200_HDMODE_GENERIC       0x3
#define THS8200_ATSCSD_MODE_480I     0x4
#define THS8200_ATSCSD_MODE_480P     0x5
#define THS8200_VESA_MASTER          0x6
#define THS8200_VESA_SLAVE           0x7
#define THS8200_SD625I               0x8
#define THS8200_MODE_MAX             0x8
#define THS8200_DTG1_MODE_MASK       0xf


/* Reset */
#define THS8200_CHIP_CNTL_RESET            0x10
#define THS8200_CHIP_CNTL_RESET_NORMAL     0x11


/* CSC bypass */
#define THS8200_CSC_BYPASS_SHIFT           0x1
#define THS8200_CSC_BYPASS                 0x1
#define THS8200_CSC_UOF_CNTL               0x1


/* data manager control input mode */
#define THS8200_DATA_CNTL_MODE_30BIT_YCBCR 0x30	/* 30 bit YCbCr/RGB 4:4:4 */
#define THS8200_DATA_CNTL_MODE_20BIT_YCBCR 0x03	/* 20 bit YCbCr 4:2:2 */



#endif				/* End of #ifndef THS8200_ENCODER_H */
