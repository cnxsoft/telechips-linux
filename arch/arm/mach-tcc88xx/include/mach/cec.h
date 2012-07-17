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
 * @file  cec.h
 * @brief This file defines interface of CEC driver.
 *
 * @author   Digital IP Development Team (mrkim@samsung.com) \n
 *           SystemLSI, Samsung Electronics
 * @version  1.0
 *
 * @remarks  09-30-2008 1.0 Initial version
 */

#ifndef _LINUX_CEC_H_
#define _LINUX_CEC_H_

#define CEC_IOC_MAGIC        'c'

/**
 * CEC device request code to set logical address.
 */
#define CEC_IOC_SETLADDR     _IOW(CEC_IOC_MAGIC, 0, unsigned int)
#define CEC_IOC_SENDDATA	 _IOW(CEC_IOC_MAGIC, 1, unsigned int)
#define CEC_IOC_RECVDATA	 _IOW(CEC_IOC_MAGIC, 2, unsigned int)

#define CEC_IOC_START	 _IO(CEC_IOC_MAGIC, 3)
#define CEC_IOC_STOP	 _IO(CEC_IOC_MAGIC, 4)

#endif /* _LINUX_CEC_H_ */
