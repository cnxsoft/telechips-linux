//-------------------------------------------------------------------
// Copyright SAMSUNG Electronics Co., Ltd
// All right reserved.
//
// This software is the confidential and proprietary information
// of Samsung Electronics, Inc. ("Confidential Information").  You
// shall not disclose such Confidential Information and shall use
// it only in accordance with the terms of the license agreement
// you entered into with Samsung Electronics.
//-------------------------------------------------------------------
/**
 * @file  regs-vwrapper.h
 * @brief This file defines video wrapper.\n
 *        ONLY FOR SAMSUNG FPGA TEST \n  
 *
 * @author   Digital IP Development Team (mrkim@samsung.com) \n
 *           SystemLSI, Samsung Electronics
 * @version  1.0
 *
 * @remarks  02-02-2009 1.0 Initial version
 */

#ifndef __REGS_VIDEO_WRAPPER_H
#define __REGS_VIDEO_WRAPPER_H

#define HDMIDP_VIDEO_WRAPPER_REG(x)    (0x00A50000 + x)

//@{
/**
 * @name Video Wrapper config registers
 */
#define VIDEO_WRAPPER_SYNC_MODE         HDMIDP_VIDEO_WRAPPER_REG(0x0000)
#define VIDEO_WRAPPER_HV_LINE_0         HDMIDP_VIDEO_WRAPPER_REG(0x0004)
#define VIDEO_WRAPPER_HV_LINE_1         HDMIDP_VIDEO_WRAPPER_REG(0x0008)
#define VIDEO_WRAPPER_VSYNC_GEN         HDMIDP_VIDEO_WRAPPER_REG(0x000C)
#define VIDEO_WRAPPER_HSYNC_GEN         HDMIDP_VIDEO_WRAPPER_REG(0x0010)
#define VIDEO_WRAPPER_H_BLANK           HDMIDP_VIDEO_WRAPPER_REG(0x0014)
#define VIDEO_WRAPPER_V_PATTERN         HDMIDP_VIDEO_WRAPPER_REG(0x0018)
#define VIDEO_WRAPPER_SYS_EN            HDMIDP_VIDEO_WRAPPER_REG(0x001C)
//@}

#endif /* __REGS_VIDEO_WRAPPER_H */
