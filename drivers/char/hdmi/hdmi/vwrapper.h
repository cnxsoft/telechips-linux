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
 * @file  vwrapper.h
 * @brief This file defines interface of Video Wrapper driver.\n
 *        ONLY FOR SAMSUNG FPGA TEST 
 *
 * @author   Digital IP Development Team (mrkim@samsung.com) \n
 *           SystemLSI, Samsung Electronics
 * @version  1.0
 *
 * @remarks  02-02-2009 1.0 Initial version
 */

#ifndef _LINUX_VIDEO_WRAPPER_H_
#define _LINUX_VIDEO_WRAPPER_H_

#include <mach/hdmi.h>

void video_wrapper_set_mode(struct device_video_params param);
void video_wrapper_enable(unsigned char enable);

#endif /* _LINUX_CEC_H_ */
