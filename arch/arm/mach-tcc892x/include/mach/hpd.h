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
 * @file  hpd.h
 * @brief This file defines interface of HPD driver.
 *
 * @author   Digital IP Development Team (mrkim@samsung.com) \n
 *           SystemLSI, Samsung Electronics
 * @version  1.0
 *
 * @remarks  09-30-2008 1.0 Initial version
 */

#ifndef _LINUX_HPD_H_
#define _LINUX_HPD_H_

#define HPD_IOC_MAGIC        'p'

/**
 * HPD device request code to set logical address.
 */
#define HPD_IOC_START	 _IO(HPD_IOC_MAGIC, 0)
#define HPD_IOC_STOP	 _IO(HPD_IOC_MAGIC, 1)

#endif /* _LINUX_HPD_H_ */
