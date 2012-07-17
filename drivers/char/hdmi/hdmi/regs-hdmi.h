//-------------------------------------------------------------------
// Copyright (c) Telechips, Inc.
// All right reserved.
//
//-------------------------------------------------------------------
/**
 * @file  regs-hdmi.h
 * @brief This file defines addresses of HDMI registers and corresponding bit values.
 *
 */
#ifndef __ASM_ARCH_REGS_HDMI_H
#define __ASM_ARCH_REGS_HDMI_H


#include <mach/bsp.h>
#include <mach/hardware.h>

#define TELECHIPS

#ifndef IRQ_HDMI
#define IRQ_HDMI	INT_HDMI
#endif//

static inline int machine_is_hdmidp(void)
{
	return 1;
}



#if defined (CONFIG_ARCH_TCC88XX )  || defined( CONFIG_ARCH_TCC92XX)
#define PMU_REG(x)					(0xF0404000 + (x))
#elif defined (CONFIG_ARCH_TCC93XX)
#define PMU_REG(x)					(0xF0503000 + (x))
#elif defined(CONFIG_ARCH_TCC892X )
#define PMU_REG(x)                                (tcc_p2v(0x74400000) + (x))
#endif
//@{
/**
 * @name PMU PHY PWROFF register
 */
#if defined (CONFIG_ARCH_TCC88XX )  || defined( CONFIG_ARCH_TCC92XX) || defined (CONFIG_ARCH_TCC93XX)
#define PMU_PWROFF					PMU_REG(0x18)
#define PWROFF_HDMIPHY			(1<<1)
#elif defined(CONFIG_ARCH_TCC892X )
#define PMU_PWROFF					PMU_REG(0x5C)
#define PWROFF_HDMIPHY			(1<<1)
#endif//


#if defined (CONFIG_ARCH_TCC88XX )  || defined( CONFIG_ARCH_TCC92XX)
#define DDICFG_REG(x)				(0xF0251000 + (x))
#elif defined (CONFIG_ARCH_TCC93XX)
#define DDICFG_REG(x)				(0xF0A51000 + (x))
#elif defined(CONFIG_ARCH_TCC892X )
#define DDICFG_REG(x)				(tcc_p2v(0x72380000) + (x))
#endif

//@{
/**
 * @name DDI_BUS HDMI Control register
 */
#if  defined (CONFIG_ARCH_TCC88XX )  || defined( CONFIG_ARCH_TCC92XX) || defined (CONFIG_ARCH_TCC93XX)
#define DDICFG_HDMICTRL				DDICFG_REG(0x2C)
#define HDMICTRL_RESET_HDMI			(1<<0)
#define HDMICTRL_RESET_SPDIF			(1<<1)
#define HDMICTRL_RESET_TMDS			(1<<2)
#define HDMICTRL_HDMI_ENABLE			(1<<14)		//!! CLK EN ??
#define HDMICTRL_PATH_LCDC0			(0<<15)
#define HDMICTRL_PATH_LCDC1			(1<<15)
#elif defined(CONFIG_ARCH_TCC892X )
#define DDICFG_HDMICTRL				DDICFG_REG(0x10)
#define HDMICTRL_RESET_HDMI			(1<<0)
#define HDMICTRL_RESET_SPDIF			(1<<1)
#define HDMICTRL_RESET_TMDS			(1<<2)
#define HDMICTRL_HDMI_ENABLE			(1<<15)		//!! CLK EN ??
#endif//

//@{
/**
 * @name DDI_BUS PWDN register
 */
 #if  defined (CONFIG_ARCH_TCC88XX )  || defined( CONFIG_ARCH_TCC92XX) || defined (CONFIG_ARCH_TCC93XX)
#define DDICFG_PWDN					DDICFG_REG(0x30)
#define PWDN_HDMI						(1<<8)
#elif defined(CONFIG_ARCH_TCC892X )
#define DDICFG_PWDN					DDICFG_REG(0x00)
#define PWDN_HDMI						(1<<2)
#endif//
//@{
/**
 * @name DDI_BUS SWRESET register
 */
 #if  defined (CONFIG_ARCH_TCC88XX )  || defined( CONFIG_ARCH_TCC92XX) || defined (CONFIG_ARCH_TCC93XX)
#define DDICFG_SWRESET					DDICFG_REG(0x34)
#define SWRESET_HDMI					(1<<8)		//!!
#elif defined(CONFIG_ARCH_TCC892X )
#define DDICFG_SWRESET					DDICFG_REG(0x04)
#define SWRESET_HDMI					(1<<2)
#endif//





#if defined(CONFIG_ARCH_TCC892X)
//@{
/**
 * @name DDI_BUS HDMI_AES register
 */
#define DDICFG_HDMI_AES				DDICFG_REG(0x14)

//@{
/**
 * @name DDI_BUS HDMI_AES_DATA register
 */
#define DDICFG_HDMI_AES_DATA0		DDICFG_REG(0x18)
#define DDICFG_HDMI_AES_DATA1		DDICFG_REG(0x1C)

//@{
/**
 * @name DDI_BUS HDMI_AES_HW_x register
 */
#define DDICFG_HDMI_AES_HW0			DDICFG_REG(0x20)
#define DDICFG_HDMI_AES_HW1			DDICFG_REG(0x24)
#define DDICFG_HDMI_AES_HW2			DDICFG_REG(0x28)
#elif (CONFIG_ARCH_TCC88XX ) || defined(CONFIG_MACH_TCC8900 ) || defined( CONFIG_ARCH_TCC92XX)
//@{
/**
 * @name DDI_BUS HDMI_AES register
 */
#define DDICFG_HDMI_AES				DDICFG_REG(0x44)

//@{
/**
 * @name DDI_BUS HDMI_AES_DATA register
 */
#define DDICFG_HDMI_AES_DATA0		DDICFG_REG(0x48)
#define DDICFG_HDMI_AES_DATA1		DDICFG_REG(0x4C)

//@{
/**
 * @name DDI_BUS HDMI_AES_HW_x register
 */
#define DDICFG_HDMI_AES_HW0			DDICFG_REG(0x50)
#define DDICFG_HDMI_AES_HW1			DDICFG_REG(0x54)
#define DDICFG_HDMI_AES_HW2			DDICFG_REG(0x58)
#else

#endif /* CONFIG_ARCH_TCC892X */

//@{
/**
 * @name HDMI IRQ bit values
 */
#define HDMI_IRQ_PIN_POLAR_CTL       (7)
#define HDMI_IRQ_GLOBAL                	(6)
#define HDMI_IRQ_I2S                    		(5)
#define HDMI_IRQ_CEC                    	(4)
#define HDMI_IRQ_HPD_PLUG               	(3)
#define HDMI_IRQ_HPD_UNPLUG  		(2)
#define HDMI_IRQ_SPDIF                  	(1)
#define HDMI_IRQ_HDCP                   	(0)
//@}
#if defined (CONFIG_ARCH_TCC88XX ) || defined(CONFIG_MACH_TCC8900 ) || defined( CONFIG_ARCH_TCC92XX)
#define HDMIDP_HDMI_SSREG(x)        (0xF0254000 + (x))
#elif defined (CONFIG_ARCH_TCC93XX)
#define HDMIDP_HDMI_SSREG(x)        (0xF0A54000 + (x))
#elif defined(CONFIG_ARCH_TCC892X)
#define HDMIDP_HDMI_SSREG(x)        (tcc_p2v(0x72300000) + (x))
#endif

//@{
/**
 * @name HDMI Subsystem registers
 */
#define HDMI_SS_INTC_CON            	HDMIDP_HDMI_SSREG(0x00)
#define HDMI_SS_INTC_FLAG          	HDMIDP_HDMI_SSREG(0x04)
#if defined (CONFIG_ARCH_TCC93XX)
#define HDMI_SS_AESKEY_VALID        	HDMIDP_HDMI_SSREG(0x08)
#endif
#define HDMI_SS_HPD                 		HDMIDP_HDMI_SSREG(0x0C)
//@}

#if defined (CONFIG_ARCH_TCC88XX ) || defined(CONFIG_MACH_TCC8900 ) || defined( CONFIG_ARCH_TCC92XX)
#define HDMIDP_HDMIREG(x)           	(0xF0255000 + (x))
#elif defined (CONFIG_ARCH_TCC93XX)
#define HDMIDP_HDMIREG(x)           	(0xF0A55000 + (x))
#elif defined(CONFIG_ARCH_TCC892X)
#define HDMIDP_HDMIREG(x)        		(tcc_p2v(0x72310000) + (x))
#endif
//@{
/**
 * @name HDMI control registers
 */
#define HDMI_CON_0                  	HDMIDP_HDMIREG(0x0000)
#define HDMI_CON_1                  	HDMIDP_HDMIREG(0x0004)
#define HDMI_CON_2                  	HDMIDP_HDMIREG(0x0008)
#define HDMI_STATUS                 	HDMIDP_HDMIREG(0x0010)
#define HDMI_PHY_STATUS             HDMIDP_HDMIREG(0x0014)
#define HDMI_STATUS_EN              	HDMIDP_HDMIREG(0x0020)
#define HDMI_HPD                    	HDMIDP_HDMIREG(0x0030)
#define HDMI_MODE_SEL               	HDMIDP_HDMIREG(0x0040)
#define HDCP_ENC_EN                 	HDMIDP_HDMIREG(0x0044)
#define HDMI_HPD_GEN                	HDMIDP_HDMIREG(0x05c8)
//@}

//@{
/**
 * HDMI audio sample packet register
 */
#define HDMI_ASP_CON                HDMIDP_HDMIREG(0x0160)
//@}

//@{
/**
 * @name Audio Clock Recovery packet registers
 */
#define HDMI_ACR_CON                HDMIDP_HDMIREG(0x0180)
#define HDMI_ACR_N0                 HDMIDP_HDMIREG(0x01a0)
#define HDMI_ACR_N1                 HDMIDP_HDMIREG(0x01a4)
#define HDMI_ACR_N2                 HDMIDP_HDMIREG(0x01a8)
//@}

//@{
/**
 * @name GCP registers
 */
#define HDMI_GCP_CON                HDMIDP_HDMIREG(0x01c0)
#define HDMI_GCP_BYTE1              HDMIDP_HDMIREG(0x01d0)
#define HDMI_GCP_BYTE2              HDMIDP_HDMIREG(0x01d4)
#define HDMI_GCP_BYTE3              HDMIDP_HDMIREG(0x01d8)
//@}

//@{
/**
 * @name AVI packet registers
 */
#define HDMI_AVI_CON                HDMIDP_HDMIREG(0x0300)
#define HDMI_AVI_CHECK_SUM          HDMIDP_HDMIREG(0x0310)
#define HDMI_AVI_BYTE1              HDMIDP_HDMIREG(0x0320)
#define HDMI_AVI_BYTE2              HDMIDP_HDMIREG(0x0324)
#define HDMI_AVI_BYTE3              HDMIDP_HDMIREG(0x0328)
#define HDMI_AVI_BYTE4              HDMIDP_HDMIREG(0x032c)
#define HDMI_AVI_BYTE5              HDMIDP_HDMIREG(0x0330)
#define HDMI_AVI_BYTE6              HDMIDP_HDMIREG(0x0334)
#define HDMI_AVI_BYTE7              HDMIDP_HDMIREG(0x0338)
#define HDMI_AVI_BYTE8              HDMIDP_HDMIREG(0x033c)
#define HDMI_AVI_BYTE9              HDMIDP_HDMIREG(0x0340)
#define HDMI_AVI_BYTE10             HDMIDP_HDMIREG(0x0344)
#define HDMI_AVI_BYTE11             HDMIDP_HDMIREG(0x0348)
#define HDMI_AVI_BYTE12             HDMIDP_HDMIREG(0x034c)
#define HDMI_AVI_BYTE13             HDMIDP_HDMIREG(0x0350)
//@}

//@{
/**
 * @name AUI packet registers
 */
#define HDMI_AUI_CON                HDMIDP_HDMIREG(0x0360)
#define HDMI_AUI_CHECK_SUM          HDMIDP_HDMIREG(0x0370)
#define HDMI_AUI_BYTE1              HDMIDP_HDMIREG(0x0380)
#define HDMI_AUI_BYTE2              HDMIDP_HDMIREG(0x0384)
#define HDMI_AUI_BYTE3              HDMIDP_HDMIREG(0x0388)
#define HDMI_AUI_BYTE4              HDMIDP_HDMIREG(0x038c)
#define HDMI_AUI_BYTE5              HDMIDP_HDMIREG(0x0390)
//@}

//@{
/**
 * @name SPD packet registers
 */
#define HDMI_SPD_CON                HDMIDP_HDMIREG(0x0400)
#define HDMI_SPD_HEADER0            HDMIDP_HDMIREG(0x0410)
#define HDMI_SPD_HEADER1            HDMIDP_HDMIREG(0x0414)
#define HDMI_SPD_HEADER2            HDMIDP_HDMIREG(0x0418)
#define HDMI_SPD_CHECK_SUM          HDMIDP_HDMIREG(0x0420) //HDMI_SPD_BYTE0
#define HDMI_SPD_DATA1              HDMIDP_HDMIREG(0x0424)
#define HDMI_SPD_DATA2              HDMIDP_HDMIREG(0x0428)
#define HDMI_SPD_DATA3              HDMIDP_HDMIREG(0x042C)
#define HDMI_SPD_DATA4              HDMIDP_HDMIREG(0x0430)
#define HDMI_SPD_DATA5              HDMIDP_HDMIREG(0x0434)
#define HDMI_SPD_DATA6              HDMIDP_HDMIREG(0x0438)
#define HDMI_SPD_DATA7              HDMIDP_HDMIREG(0x043C)

//@}

//@{
/**
 * @name HDMI video config registers
 */
#define HDMI_VACT_END_MG            HDMIDP_HDMIREG(0x008c)

#define HDMI_H_BLANK_0              HDMIDP_HDMIREG(0x00a0)
#define HDMI_H_BLANK_1              HDMIDP_HDMIREG(0x00a4)

#define HDMI_V_BLANK_0              HDMIDP_HDMIREG(0x00b0)
#define HDMI_V_BLANK_1              HDMIDP_HDMIREG(0x00b4)
#define HDMI_V_BLANK_2              HDMIDP_HDMIREG(0x00b8)

#define HDMI_H_V_LINE_0             HDMIDP_HDMIREG(0x00c0)
#define HDMI_H_V_LINE_1             HDMIDP_HDMIREG(0x00c4)
#define HDMI_H_V_LINE_2             HDMIDP_HDMIREG(0x00c8)

#define HDMI_VSYNC_POL              HDMIDP_HDMIREG(0x00e4)

#define HDMI_INT_PRO_MODE           HDMIDP_HDMIREG(0x00e8)

#define HDMI_V_BLANK_F_0            HDMIDP_HDMIREG(0x0110)
#define HDMI_V_BLANK_F_1            HDMIDP_HDMIREG(0x0114)
#define HDMI_V_BLANK_F_2            HDMIDP_HDMIREG(0x0118)

#define HDMI_H_SYNC_GEN_0           HDMIDP_HDMIREG(0x0120)
#define HDMI_H_SYNC_GEN_1           HDMIDP_HDMIREG(0x0124)
#define HDMI_H_SYNC_GEN_2           HDMIDP_HDMIREG(0x0128)

#define HDMI_V_SYNC_GEN1_0          HDMIDP_HDMIREG(0x0130)
#define HDMI_V_SYNC_GEN1_1          HDMIDP_HDMIREG(0x0134)
#define HDMI_V_SYNC_GEN1_2          HDMIDP_HDMIREG(0x0138)

#define HDMI_V_SYNC_GEN2_0          HDMIDP_HDMIREG(0x0140)
#define HDMI_V_SYNC_GEN2_1          HDMIDP_HDMIREG(0x0144)
#define HDMI_V_SYNC_GEN2_2          HDMIDP_HDMIREG(0x0148)

#define HDMI_V_SYNC_GEN3_0          HDMIDP_HDMIREG(0x0150)
#define HDMI_V_SYNC_GEN3_1          HDMIDP_HDMIREG(0x0154)
#define HDMI_V_SYNC_GEN3_2          HDMIDP_HDMIREG(0x0158)

#define HDMI_DC_CONTROL             HDMIDP_HDMIREG(0x05c0)
#define HDMI_VIDEO_PATTERN_GEN      HDMIDP_HDMIREG(0x05c4)
//@}

//@{
/**
 * @name HDCP config registers
 */
#define HDCP_SHA1_00                HDMIDP_HDMIREG(0x0600)
#define HDCP_SHA1_01                HDMIDP_HDMIREG(0x0604)
#define HDCP_SHA1_02                HDMIDP_HDMIREG(0x0608)
#define HDCP_SHA1_03                HDMIDP_HDMIREG(0x060c)
#define HDCP_SHA1_04                HDMIDP_HDMIREG(0x0610)
#define HDCP_SHA1_05                HDMIDP_HDMIREG(0x0614)
#define HDCP_SHA1_06                HDMIDP_HDMIREG(0x0618)
#define HDCP_SHA1_07                HDMIDP_HDMIREG(0x061c)
#define HDCP_SHA1_08                HDMIDP_HDMIREG(0x0620)
#define HDCP_SHA1_09                HDMIDP_HDMIREG(0x0624)
#define HDCP_SHA1_10                HDMIDP_HDMIREG(0x0628)
#define HDCP_SHA1_11                HDMIDP_HDMIREG(0x062c)
#define HDCP_SHA1_12                HDMIDP_HDMIREG(0x0630)
#define HDCP_SHA1_13                HDMIDP_HDMIREG(0x0634)
#define HDCP_SHA1_14                HDMIDP_HDMIREG(0x0638)
#define HDCP_SHA1_15                HDMIDP_HDMIREG(0x063c)
#define HDCP_SHA1_16                HDMIDP_HDMIREG(0x0640)
#define HDCP_SHA1_17                HDMIDP_HDMIREG(0x0644)
#define HDCP_SHA1_18                HDMIDP_HDMIREG(0x0648)
#define HDCP_SHA1_19                HDMIDP_HDMIREG(0x064c)

#define HDCP_KSV_LIST_0             HDMIDP_HDMIREG(0x0650)
#define HDCP_KSV_LIST_1             HDMIDP_HDMIREG(0x0654)
#define HDCP_KSV_LIST_2             HDMIDP_HDMIREG(0x0658)
#define HDCP_KSV_LIST_3             HDMIDP_HDMIREG(0x065c)
#define HDCP_KSV_LIST_4             HDMIDP_HDMIREG(0x0660)
#define HDCP_KSV_LIST_CON           HDMIDP_HDMIREG(0x0664)

#define HDCP_SHA_RESULT             HDMIDP_HDMIREG(0x0670)

#define HDCP_CTRL1                  HDMIDP_HDMIREG(0x0680)
#define HDCP_CTRL2                  HDMIDP_HDMIREG(0x0684)

#define HDCP_CHECK_RESULT           HDMIDP_HDMIREG(0x0690)

#define HDCP_BKSV_0                 HDMIDP_HDMIREG(0x06a0)
#define HDCP_BKSV_1                 HDMIDP_HDMIREG(0x06a4)
#define HDCP_BKSV_2                 HDMIDP_HDMIREG(0x06a8)
#define HDCP_BKSV_3                 HDMIDP_HDMIREG(0x06ac)
#define HDCP_BKSV_4                 HDMIDP_HDMIREG(0x06b0)

#define HDCP_AKSV_0                 HDMIDP_HDMIREG(0x06c0)
#define HDCP_AKSV_1                 HDMIDP_HDMIREG(0x06c4)
#define HDCP_AKSV_2                 HDMIDP_HDMIREG(0x06c8)
#define HDCP_AKSV_3                 HDMIDP_HDMIREG(0x06cc)
#define HDCP_AKSV_4                 HDMIDP_HDMIREG(0x06d0)

#define HDCP_AN_0                   HDMIDP_HDMIREG(0x06e0)
#define HDCP_AN_1                   HDMIDP_HDMIREG(0x06e4)
#define HDCP_AN_2                   HDMIDP_HDMIREG(0x06e8)
#define HDCP_AN_3                   HDMIDP_HDMIREG(0x06ec)
#define HDCP_AN_4                   HDMIDP_HDMIREG(0x06f0)
#define HDCP_AN_5                   HDMIDP_HDMIREG(0x06f4)
#define HDCP_AN_6                   HDMIDP_HDMIREG(0x06f8)
#define HDCP_AN_7                   HDMIDP_HDMIREG(0x06fc)

#define HDCP_BCAPS                  HDMIDP_HDMIREG(0x0700)

#define HDCP_BSTATUS_0              HDMIDP_HDMIREG(0x0710)
#define HDCP_BSTATUS_1              HDMIDP_HDMIREG(0x0714)

#define HDCP_RI_0                   HDMIDP_HDMIREG(0x0740)
#define HDCP_RI_1                   HDMIDP_HDMIREG(0x0744)

#define HDCP_PJ                     HDMIDP_HDMIREG(0x0754)

#define HDCP_I2C_INT                HDMIDP_HDMIREG(0x0780)
#define HDCP_AN_INT                 HDMIDP_HDMIREG(0x0790)
#define HDCP_WDT_INT                HDMIDP_HDMIREG(0x07a0)
#define HDCP_RI_INT                 HDMIDP_HDMIREG(0x07b0)
#define HDCP_PJ_INT                 HDMIDP_HDMIREG(0x07c0)

#define HDCP_RI_COMPARE_0           HDMIDP_HDMIREG(0x07d0)
#define HDCP_RI_COMPARE_1           HDMIDP_HDMIREG(0x07d4)
#define HDCP_FRAME_COUNT            HDMIDP_HDMIREG(0x07e0)

#define HDCP_SHA1_TX_00             HDMIDP_HDMIREG(0x0800)
#define HDCP_SHA1_TX_01             HDMIDP_HDMIREG(0x0804)
#define HDCP_SHA1_TX_02             HDMIDP_HDMIREG(0x0808)
#define HDCP_SHA1_TX_03             HDMIDP_HDMIREG(0x080c)
#define HDCP_SHA1_TX_04             HDMIDP_HDMIREG(0x0810)
#define HDCP_SHA1_TX_05             HDMIDP_HDMIREG(0x0814)
#define HDCP_SHA1_TX_06             HDMIDP_HDMIREG(0x0818)
#define HDCP_SHA1_TX_07             HDMIDP_HDMIREG(0x081c)
#define HDCP_SHA1_TX_08             HDMIDP_HDMIREG(0x0820)
#define HDCP_SHA1_TX_09             HDMIDP_HDMIREG(0x0824)
#define HDCP_SHA1_TX_10             HDMIDP_HDMIREG(0x0828)
#define HDCP_SHA1_TX_11             HDMIDP_HDMIREG(0x082c)
#define HDCP_SHA1_TX_12             HDMIDP_HDMIREG(0x0830)
#define HDCP_SHA1_TX_13             HDMIDP_HDMIREG(0x0834)
#define HDCP_SHA1_TX_14             HDMIDP_HDMIREG(0x0838)
#define HDCP_SHA1_TX_15             HDMIDP_HDMIREG(0x083c)
#define HDCP_SHA1_TX_16             HDMIDP_HDMIREG(0x0840)
#define HDCP_SHA1_TX_17             HDMIDP_HDMIREG(0x0844)
#define HDCP_SHA1_TX_18             HDMIDP_HDMIREG(0x0848)
#define HDCP_SHA1_TX_19             HDMIDP_HDMIREG(0x084c)

#define HDCP_M0_TX_00               HDMIDP_HDMIREG(0x0880)
#define HDCP_M0_TX_01               HDMIDP_HDMIREG(0x0884)
#define HDCP_M0_TX_02               HDMIDP_HDMIREG(0x0888)
#define HDCP_M0_TX_03               HDMIDP_HDMIREG(0x088c)
#define HDCP_M0_TX_04               HDMIDP_HDMIREG(0x0890)
#define HDCP_M0_TX_05               HDMIDP_HDMIREG(0x0894)
#define HDCP_M0_TX_06               HDMIDP_HDMIREG(0x0898)
#define HDCP_M0_TX_07               HDMIDP_HDMIREG(0x089c)
//@}

#if defined (CONFIG_ARCH_TCC88XX ) || defined(CONFIG_MACH_TCC8900 ) || defined( CONFIG_ARCH_TCC92XX)
#define HDMIDP_AESREG(x)            (0xF0256000 + (x))
#elif defined (CONFIG_ARCH_TCC93XX)
#define HDMIDP_AESREG(x)            (0xF0A56000 + (x))
#elif defined(CONFIG_ARCH_TCC892X)
#define HDMIDP_AESREG(x)        	(tcc_p2v(0x72320000) + (x))
#endif
//@{
/**
 * @name AES config registers
 */
#define AES_START                   HDMIDP_AESREG(0x00)
#define AES_DATA                    HDMIDP_AESREG(0x40)
//@}

//@{
/**
 * @name HDCP IRQ bit values
 */
#define HDCP_I2C_INT_NUM            (0)
#define HDCP_WATCHDOG_INT_NUM       (1)
#define HDCP_AN_WRITE_INT_NUM       (2)
#define HDCP_UPDATE_PJ_INT_NUM      (3)
#define HDCP_UPDATE_RI_INT_NUM      (4)
#define HDCP_AUD_FIFO_OVF_EN_NUM    (5)
#define HDCP_AUTHEN_ACK_NUM         (7)
//@}

//@{
/**
 * @name HDCP bit values
 */
#define HDCP_ENABLE                 (1<<1)
#define HDCP_TIMEOUT                (1<<2)
#define HDCP_REVOCATION_SET         (1<<0)

#define HDCP_RI_NOT_MATCH           (1<<1)
#define HDCP_RI_MATCH               (1<<0|1<<1)

#define HDCP_SHA1_VALID_READY       (1<<1)
#define HDCP_SHA1_VALID             (1<<0)

#define HDCP_KSV_LIST_READ_DONE     (1<<0)
#define HDCP_KSV_LIST_END           (1<<1)
#define HDCP_KSV_LIST_EMPTY         (1<<2)
#define HDCP_KSV_WRITE_DONE         (1<<3)

#define HDCP_COMPARE_FRAME_COUNT0_ENABLE    (1<<7 | 0)
#define HDCP_COMPARE_FRAME_COUNT1_ENABLE    (1<<7 | 0x7F)
//@}

//@{
/**
 * @name HDMI bit values
 */
#define HPD_SW_ENABLE               (1<<0)
#define HPD_SW_DISABLE              (0)
#define HPD_ON                      (1<<1)
#define HPD_OFF                     (0)

#define HDMI_EXTERNAL_VIDEO         (1<<1)

#define DO_NOT_TRANSMIT             (0)
#define TRANSMIT_ONCE               (1)
#define TRANSMIT_EVERY_VSYNC        (1<<1)
#define GCP_TRANSMIT_EVERY_VSYNC    (1<<3|1<<2|1<<1)

#define HDMI_SYS_ENABLE             (1<<0)
#define HDMI_PWDN_ENABLE            (1<<1)
#define HDMI_ASP_ENABLE             (1<<2)
#define HDMI_YCBCR422_ENABLE        (1<<3)
#define HDMI_ENCODING_OPTION_ENABLE (1<<4)
#define HDMI_BLUE_SCR_ENABLE        (1<<5)

#define HDMI_MODE_SEL_DVI           (1<<0)
#define HDMI_MODE_SEL_HDMI          (1<<1)

#define HDMICON1_DOUBLE_PIXEL_REPETITION    (1<<0)

#define HDMICON1_LIMIT_MASK         (1<<5|1<<6)
#define HDMICON1_RGB_LIMIT          (1<<5)
#define HDMICON1_YCBCR_LIMIT        (1<<6)

#define HDMICON2_DVI                (1<<5|1<<1)
#define HDMICON2_HDMI               (0)

#define ACR_MEASURED_CTS_MODE       (1<<2)

#define AVI_HEADER                  (0x82 + 0x02 + 0x0D)
#define AVI_PACKET_BYTE_LENGTH      (13)
#define AVI_CS_RGB                  (0)
#define AVI_CS_Y422                 (1<<5)
#define AVI_CS_Y444                 (1<<6)
#define AVI_QUANTIZATION_MASK       (1<<2|1<<3)
#define AVI_QUANTIZATION_DEFAULT    (0)
#define AVI_QUANTIZATION_LIMITED    (1<<2)
#define AVI_QUANTIZATION_FULL       (1<<3)
#define AVI_PIXEL_REPETITION_DOUBLE (1<<0)
#define AVI_FORMAT_MASK             (0x0F)
#define AVI_FORMAT_ASPECT_AS_PICTURE    (1<<3)
#define AVI_PICTURE_ASPECT_RATIO_4_3    (1<<4)
#define AVI_PICTURE_ASPECT_RATIO_16_9   (1<<5)

#define AUI_HEADER                  (0x84 + 0x01 + 0x0A)
#define AUI_PACKET_BYTE_LENGTH      (10)
#define AUI_CC_2CH                  (1)
#define AUI_CC_3CH                  (2)
#define AUI_CC_4CH                  (3)
#define AUI_CC_5CH                  (4)
#define AUI_CC_6CH                  (5)
#define AUI_CC_7CH                  (6)
#define AUI_CC_8CH                  (7)
#define HDMI_AUI_SF_MASK            (1<<4|1<<3|1<<2)
#define HDMI_AUI_SF_SF_32KHZ        (1<<2)
#define HDMI_AUI_SF_SF_44KHZ        (1<<3)
#define HDMI_AUI_SF_SF_88KHZ        (1<<4)
#define HDMI_AUI_SF_SF_176KHZ       (1<<4|1<<3)
#define HDMI_AUI_SF_SF_48KHZ        (1<<3|1<<2)
#define HDMI_AUI_SF_SF_96KHZ        (1<<4|1<<2)
#define HDMI_AUI_SF_SF_192KHZ       (1<<4|1<<3|1<<2)

#define SPD_PACKET_TYPE					(0x81)
#define SPD_PACKET_VERSION				(0x01)
#define SPD_PACKET_BYTE_LENGTH			(0x07)
#define SPD_HEADER                  	(SPD_PACKET_TYPE + SPD_PACKET_VERSION + SPD_PACKET_BYTE_LENGTH)
#define SPD_PACKET_ID0					(0x03)
#define SPD_PACKET_ID1					(0x0c)
#define SPD_PACKET_ID2					(0x00)
#define SPD_PACKET_ID_LENGTH			(0x03)
#define SPD_HDMI_VIDEO_FORMAT_NONE		(0x00)
#define SPD_HDMI_VIDEO_FORMAT_VIC		(0x01)
#define SPD_HDMI_VIDEO_FORMAT_3D		(0x02)

#define SPD_3D_STRUCT_FRAME_PACKING		(0x00)
#define SPD_3D_STRUCT_TOP_AND_BOTTOM	(0x06)
#define SPD_3D_STRUCT_SIDE_BY_SIDE		(0x08)

#define GCP_AVMUTE_ON               (1<<0)
#define GCP_AVMUTE_OFF              (1<<4)
#define GCP_CD_NOT_INDICATED        (0)
#define GCP_CD_24BPP                (1<<2)
#define GCP_CD_30BPP                (1<<0|1<<2)
#define GCP_CD_36BPP                (1<<1|1<<2)

#define HDMI_DC_CTL_8               (0)
#define HDMI_DC_CTL_10              (1<<0)
#define HDMI_DC_CTL_12              (1<<1)

#define ASP_TYPE_MASK               (1<<5|1<<6)
#define ASP_LPCM_TYPE               (0)
#define ASP_DSD_TYPE               (1<<5)
#define ASP_HBR_TYPE                (1<<6)
#define ASP_DST_TYPE                (1<<5|1<<6)
#define ASP_MODE_MASK               (1<<4)
#define ASP_LAYOUT_0                (0)
#define ASP_LAYOUT_1                (1<<4)
#define ASP_SP_MASK                 (0x0F)
#define ASP_SP_0                    (1<<0)
#define ASP_SP_1                    (1<<1)
#define ASP_SP_2                    (1<<2)
#define ASP_SP_3                    (1<<3)

#define HDMI_MODE_SEL_HDMI          (1<<1)
//@}
#if defined (CONFIG_ARCH_TCC88XX ) || defined(CONFIG_MACH_TCC8900 ) || defined( CONFIG_ARCH_TCC92XX)
#define HDMIDP_I2SREG(x)            (0xF0258000 + (x))
#elif defined (CONFIG_ARCH_TCC93XX)
#define HDMIDP_I2SREG(x)            (0xF0A58000 + (x))
#elif defined(CONFIG_ARCH_TCC892X)
#define HDMIDP_I2SREG(x)        	(tcc_p2v(0x72340000) + (x))
#endif
//@{
/**
 * @name I2S registers
 */
#define HDMI_SS_I2S_CLK_CON         HDMIDP_I2SREG(0x000)
#define HDMI_SS_I2S_CON_1           HDMIDP_I2SREG(0x004)
#define HDMI_SS_I2S_CON_2           HDMIDP_I2SREG(0x008)
#define HDMI_SS_I2S_PIN_SEL_0       HDMIDP_I2SREG(0x00C)
#define HDMI_SS_I2S_PIN_SEL_1       HDMIDP_I2SREG(0x010)
#define HDMI_SS_I2S_PIN_SEL_2       HDMIDP_I2SREG(0x014)
#define HDMI_SS_I2S_PIN_SEL_3       HDMIDP_I2SREG(0x018)
#define HDMI_SS_I2S_DSD_CON         HDMIDP_I2SREG(0x01C)

#define HDMI_SS_I2S_IN_MUX_CON      HDMIDP_I2SREG(0x020)
#define HDMI_SS_I2S_CH_ST_CON       HDMIDP_I2SREG(0x024)
#define HDMI_SS_I2S_CH_ST_0         HDMIDP_I2SREG(0x028)
#define HDMI_SS_I2S_CH_ST_1         HDMIDP_I2SREG(0x02C)
#define HDMI_SS_I2S_CH_ST_2         HDMIDP_I2SREG(0x030)
#define HDMI_SS_I2S_CH_ST_3         HDMIDP_I2SREG(0x034)
#define HDMI_SS_I2S_CH_ST_4         HDMIDP_I2SREG(0x038)
#define HDMI_SS_I2S_CH_ST_SH_0      HDMIDP_I2SREG(0x03C)
#define HDMI_SS_I2S_CH_ST_SH_1      HDMIDP_I2SREG(0x040)
#define HDMI_SS_I2S_CH_ST_SH_2      HDMIDP_I2SREG(0x044)
#define HDMI_SS_I2S_CH_ST_SH_3      HDMIDP_I2SREG(0x048)
#define HDMI_SS_I2S_CH_ST_SH_4      HDMIDP_I2SREG(0x04C)
#define HDMI_SS_I2S_MUX_CH          HDMIDP_I2SREG(0x054)
#define HDMI_SS_I2S_MUX_CUV         HDMIDP_I2SREG(0x058)
//@}

#if defined (CONFIG_ARCH_TCC88XX ) || defined(CONFIG_MACH_TCC8900 ) || defined( CONFIG_ARCH_TCC92XX)
#define HDMIDP_SPDIFREG(x)          (0xF0257000 + (x))
#elif defined (CONFIG_ARCH_TCC93XX)
#define HDMIDP_SPDIFREG(x)          (0xF0A57000 + (x))
#elif defined(CONFIG_ARCH_TCC892X)
#define HDMIDP_SPDIFREG(x)        	(tcc_p2v(0x72330000) + (x))
#endif
//@{
/**
 * @name SPDIF registers
 */
#define HDMI_SS_SPDIF_CLK_CTRL          HDMIDP_SPDIFREG(0x00)
#define HDMI_SS_SPDIF_OP_CTRL           HDMIDP_SPDIFREG(0x04)
#define HDMI_SS_SPDIF_IRQ_MASK          HDMIDP_SPDIFREG(0x08)
#define HDMI_SS_SPDIF_IRQ_STATUS        HDMIDP_SPDIFREG(0x0C)
#define HDMI_SS_SPDIF_CONFIG_1          HDMIDP_SPDIFREG(0x10)
#define HDMI_SS_SPDIF_CONFIG_2          HDMIDP_SPDIFREG(0x14)

#define HDMI_SS_SPDIF_USER_VALUE_1      HDMIDP_SPDIFREG(0x20)
#define HDMI_SS_SPDIF_USER_VALUE_2      HDMIDP_SPDIFREG(0x24)
#define HDMI_SS_SPDIF_USER_VALUE_3      HDMIDP_SPDIFREG(0x28)
#define HDMI_SS_SPDIF_USER_VALUE_4      HDMIDP_SPDIFREG(0x2C)
#define HDMI_SS_SPDIF_CH_STATUS_0_1     HDMIDP_SPDIFREG(0x30)
#define HDMI_SS_SPDIF_CH_STATUS_0_2     HDMIDP_SPDIFREG(0x34)
#define HDMI_SS_SPDIF_CH_STATUS_0_3     HDMIDP_SPDIFREG(0x38)
#define HDMI_SS_SPDIF_CH_STATUS_0_4     HDMIDP_SPDIFREG(0x3C)
#define HDMI_SS_SPDIF_CH_STATUS_1       HDMIDP_SPDIFREG(0x40)

#define HDMI_SS_SPDIF_FRAME_PERIOD_1    HDMIDP_SPDIFREG(0x48)
#define HDMI_SS_SPDIF_FRAME_PERIOD_2    HDMIDP_SPDIFREG(0x4C)
#define HDMI_SS_SPDIF_PC_INFO_1         HDMIDP_SPDIFREG(0x50)
#define HDMI_SS_SPDIF_PC_INFO_2         HDMIDP_SPDIFREG(0x54)
#define HDMI_SS_SPDIF_PD_INFO_1         HDMIDP_SPDIFREG(0x58)
#define HDMI_SS_SPDIF_PD_INFO_2         HDMIDP_SPDIFREG(0x5C)
#define HDMI_SS_SPDIF_DATA_BUF_0_1      HDMIDP_SPDIFREG(0x60)
#define HDMI_SS_SPDIF_DATA_BUF_0_2      HDMIDP_SPDIFREG(0x64)
#define HDMI_SS_SPDIF_DATA_BUF_0_3      HDMIDP_SPDIFREG(0x68)
#define HDMI_SS_SPDIF_USER_BUF_0        HDMIDP_SPDIFREG(0x6C)
#define HDMI_SS_SPDIF_DATA_BUF_1_1      HDMIDP_SPDIFREG(0x70)
#define HDMI_SS_SPDIF_DATA_BUF_1_2      HDMIDP_SPDIFREG(0x74)
#define HDMI_SS_SPDIF_DATA_BUF_1_3      HDMIDP_SPDIFREG(0x78)
#define HDMI_SS_SPDIF_USER_BUF_1        HDMIDP_SPDIFREG(0x7C)
//@}

//@{
/**
 * @name I2S bit values
 */
#define I2S_PIN_SEL_AUDIO_0             (0)
#define I2S_PIN_SEL_AUDIO_1             (1)
#define I2S_PIN_SEL_AUDIO_2             (2)
#define I2S_PIN_SEL_AUDIO_3             (3)
#define I2S_PIN_SEL_AUDIO_4             (4)
#define I2S_PIN_SEL_AUDIO_5             (5)
#define I2S_PIN_SEL_AUDIO_6             (6)

#define I2S_CLK_CON_ENABLE              (1)
#define I2S_CLK_CON_DISABLE             (0)

#define I2S_CON_SC_POL_FALLING          (0<<1)
#define I2S_CON_SC_POL_RISING           (1<<1)

#define I2S_CON_CH_POL_LOW              (0)
#define I2S_CON_CH_POL_HIGH             (1)

#define I2S_CON_MSB                     (0<<6)
#define I2S_CON_LSB                     (1<<6)

#define I2S_CON_BIT_CH_32               (0<<4)
#define I2S_CON_BIT_CH_48               (1<<4)
#define I2S_CON_BIT_CH_64               (2<<4)

#define I2S_CON_DATA_NUM_MASK           (3<<2)
#define I2S_CON_DATA_NUM_16             (1<<2)
#define I2S_CON_DATA_NUM_20             (2<<2)
#define I2S_CON_DATA_NUM_24             (3<<2)

#define I2S_CON_I2S_MODE_BASIC              (0)
#define I2S_CON_I2S_MODE_LEFT_JUSTIFIED     (2)
#define I2S_CON_I2S_MODE_RIGHT_JUSTIFIED    (3)

#define I2S_DSD_CON_POL_RISING          (1<<1)
#define I2S_DSD_CON_POL_FALLING         (0<<1)

#define I2S_DSD_CON_ENABLE              (1)
#define I2S_DSD_CON_DISABLE             (0)

#define I2S_IN_MUX_IN_ENABLE            (1<<4)
#define I2S_IN_MUX_SELECT_DSD           (2<<2)
#define I2S_IN_MUX_SELECT_I2S           (1<<2)
#define I2S_IN_MUX_SELECT_SPDIF         (0)
#define I2S_IN_MUX_CUV_ENABLE           (1<<1)
#define I2S_IN_MUX_ENABLE               (1)

#define I2S_MUX_CH_0_LEFT_ENABLE        (1<<0)
#define I2S_MUX_CH_0_RIGHT_ENABLE       (1<<1)
#define I2S_MUX_CH_1_LEFT_ENABLE        (1<<2)
#define I2S_MUX_CH_1_RIGHT_ENABLE       (1<<3)
#define I2S_MUX_CH_2_LEFT_ENABLE        (1<<4)
#define I2S_MUX_CH_2_RIGHT_ENABLE       (1<<5)
#define I2S_MUX_CH_3_LEFT_ENABLE        (1<<6)
#define I2S_MUX_CH_3_RIGHT_ENABLE       (1<<7)
#define I2S_MUX_CH_ALL_ENABLE           (0xFF)

#define I2S_MUX_CUV_LEFT_ENABLE         (1<<0)
#define I2S_MUX_CUV_RIGHT_ENABLE        (1<<1)

#define I2S_CH_ST_0_TYPE_MASK           (1<<1)
#define I2S_CH_ST_0_TYPE_LPCM           (0)
#define I2S_CH_ST_0_TYPE_NLPCM          (1<<1)

#define I2S_CH_ST_0_COPY_MASK           (1<<2)
#define I2S_CH_ST_0_NOT_COPYRIGHTED     (1<<2)

#define I2S_CH_ST_2_CHANNEL_MASK        (0xF0)
#define I2S_CH_ST_2_CH_UNDEFINED        (0)
#define I2S_CH_ST_2_CH_01               (0x1<<4)
#define I2S_CH_ST_2_CH_02               (0x2<<4)
#define I2S_CH_ST_2_CH_03               (0x3<<4)
#define I2S_CH_ST_2_CH_04               (0x4<<4)
#define I2S_CH_ST_2_CH_05               (0x5<<4)
#define I2S_CH_ST_2_CH_06               (0x6<<4)
#define I2S_CH_ST_2_CH_07               (0x7<<4)
#define I2S_CH_ST_2_CH_08               (0x8<<4)
#define I2S_CH_ST_2_CH_09               (0x9<<4)
#define I2S_CH_ST_2_CH_10               (0xA<<4)
#define I2S_CH_ST_2_CH_11               (0xB<<4)
#define I2S_CH_ST_2_CH_12               (0xC<<4)
#define I2S_CH_ST_2_CH_13               (0xD<<4)
#define I2S_CH_ST_2_CH_14               (0xE<<4)
#define I2S_CH_ST_2_CH_15               (0xF<<4)

#define I2S_CH_ST_3_SF_MASK             (0x0F)
#define I2S_CH_ST_3_SF_44KHZ            (0)
#define I2S_CH_ST_3_SF_88KHZ            (1<<3)
#define I2S_CH_ST_3_SF_176KHZ           (1<<3|1<<2)
#define I2S_CH_ST_3_SF_48KHZ            (1<<1)
#define I2S_CH_ST_3_SF_96KHZ            (1<<3|1<<1)
#define I2S_CH_ST_3_SF_192KHZ           (1<<3|1<<2|1<<1)
#define I2S_CH_ST_3_SF_32KHZ            (1<<1|1<<0)
#define I2S_CH_ST_3_SF_768KHZ           (1<<3|1<<0)


#define I2S_CH_ST_4_WL_MASK             (0x0F)
#define I2S_CH_ST_4_WL_20_NOT_DEFINED   (0)
#define I2S_CH_ST_4_WL_20_16            (1<<1)
#define I2S_CH_ST_4_WL_20_18            (1<<2)
#define I2S_CH_ST_4_WL_20_19            (1<<3)
#define I2S_CH_ST_4_WL_20_20            (1<<3|1<<1)
#define I2S_CH_ST_4_WL_20_17            (1<<3|1<<2)
#define I2S_CH_ST_4_WL_24_NOT_DEFINED   (1<<0)
#define I2S_CH_ST_4_WL_24_20            (1<<1|1<<0)
#define I2S_CH_ST_4_WL_24_22            (1<<2|1<<0)
#define I2S_CH_ST_4_WL_24_23            (1<<3|1<<0)
#define I2S_CH_ST_4_WL_24_24            (1<<3|1<<1|1<<0)
#define I2S_CH_ST_4_WL_24_21            (1<<3|1<<2|1<<0)
//@}

//@{
/**
 * @name SPDIF bit values
 */
#define SPDIF_CLK_CTRL_ENABLE           (1)
#define SPDIF_CLK_CTRL_DISABLE          (0)

#define SPDIF_IRQ_ENABLE_ALL            (0xFF)
#define SPDIF_IRQ_DISABLE_ALL           (0)

#define SPDIF_CH_STATUS_0_1_LPCM        (0)
#define SPDIF_CH_STATUS_0_1_NPCM        (1<<1)

#define SPDIF_CONFIG_1_NOISE_FILTER_2_SAMPLES   (1<<6)
#define SPDIF_CONFIG_1_NOISE_FILTER_3_SAMPLES   (0)
#define SPDIF_CONFIG_1_LPCM                     (0)
#define SPDIF_CONFIG_1_NPCM                     (1<<5)
#define SPDIF_CONFIG_1_PCPD_MANUAL              (1<<4)
#define SPDIF_CONFIG_1_WORD_LENGTH_MANUAL       (1<<3)
#define SPDIF_CONFIG_1_UVCP_ENABLE              (1<<2)
#define SPDIF_CONFIG_1_HDMI_1_BURST             (0)
#define SPDIF_CONFIG_1_HDMI_2_BURST             (1<<1)
#define SPDIF_CONFIG_1_ALIGN_16BIT              (0)
#define SPDIF_CONFIG_1_ALIGN_32BIT              (1<<0)

#define SPDIF_CLK_RECOVERY_FAIL_MASK            (1<<0)
#define SPDIF_STATUS_RECOVERED_MASK             (1<<1)
#define SPDIF_PREAMBLE_DETECTED_MASK            (1<<2)
#define SPDIF_HEADER_NOT_DETECTED_MASK          (1<<3)
#define SPDIF_HEADER_DETECTED_MASK              (1<<4)
#define SPDIF_PAPD_NOT_DETECTED_MASK            (1<<5)
#define SPDIF_ABNORMAL_PD_MASK                  (1<<6)
#define SPDIF_BUFFER_OVERFLOW_MASK              (1<<7)

#define SPDIF_SIGNAL_RESET              (0)
#define SPDIF_SIGNAL_DETECT             (1<<0)
#define SPDIF_RUNNING                   (1<<1 | 1<<0)
//@}

#endif /* __ASM_ARCH_REGS_HDMI_H */
