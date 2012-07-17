/*****************************************************************************/
/*!
 *  \file        debug_filter.h \n
 *  \version     1.0 \n
 *  \author      Vogt \n
 *  \brief       Public header for debug output filtering. \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: debug_filter.h 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#ifndef _DBG_FILTER_H
#define _DBG_FILTER_H


/*********************************************************************/
/*!
 * \name DBG_CAT_FILTER, 
 *       debug output categories, values must be unique and power
 *       of 2. Upper 3 bytes must be 0. Debug output is only shown for
 *       the categories specified in DBG_CAT_FILTER
 */
/*********************************************************************/
//@{
#define DBG_INFO        0x00000001      //!< information mask
#define DBG_WARN        0x00000002      //!< warning mask
#define DBG_ERR         0x00000004      //!< error mask
#define DBG_ASSERT      0x00000008      //!< assert mask
#define DBG_CAT__ALL    0x000000FF      //!< mask to get all categories

#define DBG_CAT_FILTER  (DBG_CAT__ALL)  //!< currently used category mask
//@}


/*********************************************************************/
/*!
 * \name DBG_CAT_FILTER, 
 *       debug output modules, values must be unique and power
 *       of 2. Lower byte must be 0. Debug output is only shown for
 *       the modules specified in DBG_MODULE_FILTER
 */
/*********************************************************************/
//@{
#define DBG_I2C             0x00000100          //!< I2C driver mask
#define DBG_IRQ             0x00000200          //!< interrupt driver mask
#define DBG_ISI             0x00000400          //!< ISI driver mask
#define DBG_LCD             0x00000800          //!< LCD driver mask
#define DBG_MRV             0x00001000          //!< MARVIN driver mask
#define DBG_APP             0x00002000          //!< application mask
#define DBG_KLED            0x00004000          //!< key and LED driver mask
#define DBG_MOTO            0x00008000          //!< motor driver mask
#define DBG_UI              0x00010000          //!< user interface mask
#define DBG_NOSE            0x00020000          //!< non semi-hosting file access mask
#define DBG_MIPI            0x00040000          //!< mipi driver mask
#define DBG_REG             0x10000000          //!< register access functions/macros (very often!)
#define DBG_MODULE__ALL     0x0FFFFF00          //!< mask to get all modules

#define DBG_MODULE_FILTER   (DBG_MODULE__ALL)   //!< currently used module mask
//@}



#endif //_DBG_FILTER_H
