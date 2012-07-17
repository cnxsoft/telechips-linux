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
 * @file  vwrapper.c
 * @brief This file contains an implementation of Video Wrapper.\n
 *        ONLY FOR SAMSUNG FPGA TEST 
 *
 * @author  Digital IP Development Team (mrkim@samsung.com) \n
 *          SystemLSI, Samsung Electronics
 * @version 1.0
 *
 * @remarks 02-02-2009 1.0 Initial version
 */

#include <asm/io.h>

#include "regs-vwrapper.h"
#include "vwrapper.h"

void video_wrapper_enable(unsigned char enable)
{
    if (enable)
        writeb(0x01,VIDEO_WRAPPER_SYS_EN);
    else
        writeb(0x00,VIDEO_WRAPPER_SYS_EN);
}

void video_wrapper_set_mode(struct device_video_params param)
{
    writel(param.HBlank, VIDEO_WRAPPER_H_BLANK);

    writel((param.HVLine&0x00000FFF), VIDEO_WRAPPER_HV_LINE_0);
    writel((param.HVLine&0x00FFF000)>>12, VIDEO_WRAPPER_HV_LINE_1);

    writel((param.HSYNCGEN&0x000001FF), VIDEO_WRAPPER_HSYNC_GEN);
    writel((param.HSYNCGEN>>20), VIDEO_WRAPPER_SYNC_MODE);

    writel((param.VSYNCGEN&0x00FFF000)>>12, VIDEO_WRAPPER_VSYNC_GEN);

    writel(0x1,VIDEO_WRAPPER_V_PATTERN);
}
