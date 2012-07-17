/*****************************************************************************/
/*!
 *  \file        debug.h \n
 *  \version     1.0 \n
 *  \author      Neugebauer \n
 *  \brief       Public header for debugging functions
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: debug.h 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#ifndef _DBG_H
#define _DBG_H

#include "debug_priv.h"

// #define DBG_PRINTREGS // set for regs output
// #define CAMERARAW     // for raw pictures, test of CEM

//! macro for debug-specific output which is not needed for the release version
#ifdef _DEBUG
#define DBG_OUT( x ) UiDbgPrintf x  // do not add parentheses here!
#else
#define DBG_OUT( x )                // release version: no code
#endif //_DEBUG


// the assertion, pre & post condition handler macros
#ifdef _DEBUG   // debug version: with code to make the debugging much more easy
    #define ASSERT( x ) if( !(x) ){ UiDbgPrintf(DBG_ASSERT|DBG_MODULE__ALL, "ASSERT( %s ) failed! %s line %d\n", #x, __FILE__, __LINE__ ); for(;;){} }                                  //!< assertion macro
    #define PRE_CONDITION(condition)   if ( !(condition) ) {UiDbgPrintf(DBG_ASSERT|DBG_MODULE__ALL, "PRE_CONDITION FAILURE:" #condition "%s line %d", __FILE__, __LINE__); for(;;){} }  //!< pre-condition macro
    #define POST_CONDITION(condition)  if ( !(condition) ) {UiDbgPrintf(DBG_ASSERT|DBG_MODULE__ALL, "POST_CONDITION FAILURE:"#condition "%s line %d", __FILE__, __LINE__); for(;;){} }  //!< post-condition macro
#else           // release version: no code
    #define ASSERT( x )                     //!< assertion macro
    #define PRE_CONDITION(condition)        //!< pre-condition macro
    #define POST_CONDITION(condition)       //!< post-condition macro
#endif //_DEBUG


// Set the define to a non-zero value to be able to recognize memory leaks.
#define DBG_COUNT_MALLOC_FREE               (0)
#define DBG_COUNT_MALLOC_FREE_SHOW_LOCATION (0)

#if DBG_COUNT_MALLOC_FREE
    extern void *DbgMalloc( UINT32 ulAmountOfBytes );
    // count the malloc() and free() calls
    #define DBG_MALLOC( x ) DbgMalloc( x );                                             \
                            if( DBG_COUNT_MALLOC_FREE_SHOW_LOCATION )                   \
                            {                                                           \
                                DBG_OUT( (DINFO, "%s line %d\n", __FILE__, __LINE__) ); \
                            }                                                           \
                            g_lDbgMallocFreeCounter++       //!< malloc() call with count of the amount of malloc() calls
    #define DBG_FREE( x )   free( x );                                                                          \
                            if( DBG_COUNT_MALLOC_FREE_SHOW_LOCATION )                                           \
                            {                                                                                   \
                                DBG_OUT( (DINFO, "free:   0x%08X file %s line %d\n", x, __FILE__, __LINE__) );  \
                            }                                                                                   \
                            x = NULL;                                                                           \
                            g_lDbgMallocFreeCounter--       //!< free() call with count of the amount of free() calls
    extern INT32 g_lDbgMallocFreeCounter;
#else   // #if COUNT_ALLOC_FREE
    // don't count the malloc() and free() calls
    #define DBG_MALLOC( x ) malloc( x )     //!< standard malloc() call without extra functionality
    #define DBG_FREE( x )   free( x ); x = NULL      //!< standard free() call without extra functionality
#endif  // #if COUNT_ALLOC_FREE

#endif //_DBG_H
