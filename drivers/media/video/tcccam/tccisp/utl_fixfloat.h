/*****************************************************************************/
/*!
 *  \file        utl_fixfloat.h \n
 *  \version     1.0 \n
 *  \author      Meinicke \n
 *  \brief       Floatingpoint to Fixpoint and vice versa conversion
 *               routines. \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: utl_fixfloat.h 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
*/
/*****************************************************************************/

#ifndef _UTL_FIXFLOAT_H
#define _UTL_FIXFLOAT_H

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

UINT32 UtlFloatToFix_U0208( FLOAT fFloat );
FLOAT  UtlFixToFloat_U0208( UINT32 ulFix );

UINT32 UtlFloatToFix_U0408( FLOAT fFloat );
FLOAT  UtlFixToFloat_U0408( UINT32 ulFix );

UINT32 UtlFloatToFix_U0800( FLOAT fFloat );
FLOAT  UtlFixToFloat_U0800( UINT32 ulFix );

UINT32 UtlFloatToFix_U1000( FLOAT fFloat );
FLOAT  UtlFixToFloat_U1000( UINT32 ulFix );

UINT32 UtlFloatToFix_U1200( FLOAT fFloat );
FLOAT  UtlFixToFloat_U1200( UINT32 ulFix );

UINT32 UtlFloatToFix_S0207( FLOAT fFloat );
FLOAT  UtlFixToFloat_S0207( UINT32 ulFix );

UINT32 UtlFloatToFix_S0307( FLOAT fFloat );
FLOAT  UtlFixToFloat_S0307( UINT32 ulFix );

UINT32 UtlFloatToFix_S0407( FLOAT fFloat );
FLOAT  UtlFixToFloat_S0407( UINT32 ulFix );

UINT32 UtlFloatToFix_S0808( FLOAT fFloat );
FLOAT  UtlFixToFloat_S0808( UINT32 ulFix );

UINT32 UtlFloatToFix_S1200( FLOAT fFloat );
FLOAT  UtlFixToFloat_S1200( UINT32 ulFix );

#endif // _UTL_FIXFLOAT_H
