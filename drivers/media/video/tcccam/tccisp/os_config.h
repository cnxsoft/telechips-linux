/*****************************************************************************/
/*!
 *  \file        os_config.h \n
 *  \version     1.0 \n
 *  \author      Vahlsing \n
 *  \brief       Per project configuration header for OS module.
 *               Included by \ref os.h and the target OS implementations. \n
 *
 *  \revision    $Revision: 714 $ \n
 *               $Author: vahlsing $ \n
 *               $Date: 2009-11-28 00:32:26 +0100 (Sa, 28 Nov 2009) $ \n
 *               $Id: os_config.h 714 2009-11-27 23:32:26Z vahlsing $ \n
 */
/*  Copyright(c) Silicon Image GmbH, 2009.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted without any limitations.
 *
 *  Parts of this software are provided under individual licenses with more stringend
 *  terms and conditions. Those licenses include but are not limited to:
 *  - the GNU General Public License (multiple versions) 
 *  - the GNU General Public License (version 2) modified by the FreeRTOS exception
 *  - the BSD License or derived licenses.
 *  Detailed copyright and licensing information of those software parts is keept
 *  along with those software parts as required by their respective licenses.
 *
 *  This software is provided by the copyright holders and contributors "as is" and
 *  any express or implied warranties, including, but not limited to, the implied
 *  warranties of merchantability and fitness for a particular purpose are disclaimed.
 *  In no event shall the copyright holder or contributors be liable for any direct,
 *  indirect, incidental, special, exemplary, or consequential damages (including,
 *  but not limited to, procurement of substitute goods or services; loss of use,
 *  data, or profits; or business interruption) however caused and on any theory of
 *  liability, whether in contract, strict liability, or tort (including negligence
 *  or otherwise) arising in any way out of the use of this software, even if advised
 *  of the possibility of such damage.
 */
/*****************************************************************************/

#ifndef _OS_CONFIG_H
#define _OS_CONFIG_H

///////////////////////////////////////////////////////////////////////////////
///  \def OS_EVENT
///  \brief  Include event handling in OS Abstraction Layer library.
#define OS_EVENT

///////////////////////////////////////////////////////////////////////////////
///  \def    OS_MUTEX
///  \brief  Include mutex handling in OS Abstraction Layer library.
#define OS_MUTEX

///////////////////////////////////////////////////////////////////////////////
///  \def    OS_SEMAPHORE
///  \brief  Include semaphore handling in OS Abstraction Layer library.
#define OS_SEMAPHORE

///////////////////////////////////////////////////////////////////////////////
///  \def    OS_QUEUE
///  \brief  Include queue handling in OS Abstraction Layer library.
#define OS_QUEUE

///////////////////////////////////////////////////////////////////////////////
///  \def    OS_THREAD
///  \brief  Include thread handling in OS Abstraction Layer library.
#define OS_THREAD

///////////////////////////////////////////////////////////////////////////////
///  \def    OS_TIMER
///  \brief  Include timer support in OS Abstraction Layer library.
#define OS_TIMER
#undef OS_TIMER // we want this to be documented by doxygen but not compiled in for now

///////////////////////////////////////////////////////////////////////////////
///  \def    OS_FILE
///  \brief  Include file handling in OS Abstraction Layer library.
#define OS_FILE

///////////////////////////////////////////////////////////////////////////////
///  \def    OS_FILE_WANT_ARMSEMI
///  \brief  Include ARM's semihosting in OS Abstraction Layer library.
#define OS_FILE_WANT_ARMSEMI
////#undef OS_FILE_WANT_ARMSEMI // we want this to be documented by doxygen but not compiled in for now

///////////////////////////////////////////////////////////////////////////////
///  \def    OS_FILE_WANT_TRIXI
///  \brief  Include TRIXI remote I/O in OS Abstraction Layer library.
#define OS_FILE_WANT_TRIXI
#undef OS_FILE_WANT_TRIXI // we want this to be documented by doxygen but not compiled in for now

///////////////////////////////////////////////////////////////////////////////
///  \def    OS_MISC
///  \brief  Include misc functionality in OS Abstraction Layer library.
#define OS_MISC

///////////////////////////////////////////////////////////////////////////////
///  \def    OS_DEBUG
///  \brief  Enable debug functionality in OS Abstraction Layer library.
#define OS_DEBUG

///////////////////////////////////////////////////////////////////////////////
///  \def    OS_HWDEP
///  \brief  Include Trillian hardware dependent functionality in OS Abstraction Layer library.
#define OS_HWDEP


#endif // _OS_CONFIG_H
