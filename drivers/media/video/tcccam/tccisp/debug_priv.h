/*****************************************************************************/
/*!
 *  <!-- DOXYGEN prepared -->
 *  Copyright(c)2004-2006 sci-worx GmbH
 *
 *  \file        debug_priv.h \n
 *  \version     1.0 \n
 *  \author      Neugebauer \n
 *  \brief       <!-- DESCRIPTION --> Private header for debugging functions
 */
/*  HISTORY:
 *  build  | date       | author     | description
 *  1.0.00   22.09.2004   Neugebauer   created
 *  1.0.01   31.01.2006   Meinicke     revised include section
 *  1.0.02   06.02.2006   Vogt         DbgPrintf removed
 *  1.0.03   15.05.2006   Grönnert     Doxygen comments changed to get a better documentation
 */
/*****************************************************************************/

#ifndef _DBG_PRIV_H
#define _DBG_PRIV_H

// filter settings, change this to turn on output (1 = turn on)
#define DBG_FILTER_LEVEL  ( DBG_INFO | DBG_WARN | DBG_ERR )
#define DBG_FILTER_MODULE 0xffffff00


#endif //_DBG_PRIV_H
