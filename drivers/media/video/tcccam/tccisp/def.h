/*****************************************************************************/
/*!
 *  \file        def.h \n
 *  \version     1.0 \n
 *  \author      Neugebauer \n
 *  \brief       Header of general defines. \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: def.h 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
*/
/*****************************************************************************/

#ifndef _DEF_H
#define _DEF_H

#ifndef TRUE
#define TRUE   1
#endif
#ifndef FALSE
#define FALSE  0
#endif

#ifndef ON
#define ON   -1  // all bits to '1' but prevent "shift overflow" warning
#endif
#ifndef OFF
#define OFF  0
#endif

#ifndef ENABLE
#define ENABLE   -1  // all bits to '1' but prevent "shift overflow" warning
#endif
#ifndef DISABLE
#define DISABLE  0
#endif


typedef int RESULT;
#define RET_SUCCESS             0   //!< this has to be 0, if clauses rely on it
#define RET_FAILURE             1   //!< general failure
#define RET_NOTSUPP             2   //!< feature not supported
#define RET_BUSY                3   //!< there's already something going on...
#define RET_CANCELED            4   //!< operation canceled
#define RET_OUTOFMEM            5   //!< out of memory
#define RET_OUTOFRANGE          6   //!< parameter/value out of range
#define RET_IDLE                7   //!< feature/subsystem is in idle state
#define RET_WRONG_HANDLE        8   //!< handle is wrong
#define RET_NULL_POINTER        9   //!< the/one/all parameter(s) is a(are) NULL pointer(s)
#define RET_NOTAVAILABLE       10   //!< profile not available
#define RET_DIVISION_BY_ZERO   11   //!< a divisor equals ZERO


typedef unsigned char  UINT8;
typedef unsigned short UINT16;
typedef unsigned long  UINT32;
typedef signed char    INT8;
typedef signed short   INT16;
typedef signed long    INT32;
typedef char           CHAR;   //!<  8 bits wide char; signedness depends on compiler settings
#ifndef BOOL
	#define BOOL int
#endif
typedef unsigned int   BITFIELD;
typedef float          FLOAT;
typedef double         DOUBLE;

   
#ifndef NULL
#define NULL  (void *) 0        //!< Define NULL pointer value.
#endif


#ifndef UNUSED_PARAM
#define UNUSED_PARAM(x)   ((x)=(x))
#endif

//to avoid Lint warnings, use it within const context
#ifndef UNUSED_PARAM1
#define UNUSED_PARAM1(x)   if((x) != (x)) {ASSERT (FALSE) }
#endif

// documentation keywords for pointer arguments, to tell the direction of the passing                     
#ifndef OUT
    #define OUT                 //!< pointer content is expected to be filled by called function                     
#endif
#ifndef IN
    #define IN                  //!< pointer content contains parameters from the caller                     
#endif
#ifndef INOUT   
    #define INOUT               //!< content is expected to be read and changed                     
#endif

// some useful macros
#ifndef MIN
    #define MIN(x,y)            ( (x) < (y) ? (x) : (y) )
#endif

#ifndef MAX
    #define MAX(x,y)            ( (x) > (y) ? (x) : (y) )
#endif

#ifndef ABS
    #define ABS(val)            ( (val) < 0 ? -(val) : (val) )
#endif

// converts a term to a string (two macros are required, never use _VAL2STR() directly)
#define _VAL2STR(x) #x
#define VAL2STR(x) _VAL2STR(x)

#endif //_DEF_H
