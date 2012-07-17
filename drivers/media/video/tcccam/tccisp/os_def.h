/*****************************************************************************/
/*!
 *  \file        os_def.h \n
 *  \version     1.0 \n
 *  \author      Vahlsing \n
 *  \brief       Global defines of types to be used in conjunction with OS layer.
 *               Included by \ref os.h and the target OS implementations. \n
 *
 *  \revision    $Revision: 714 $ \n
 *               $Author: vahlsing $ \n
 *               $Date: 2009-11-28 00:32:26 +0100 (Sa, 28 Nov 2009) $ \n
 *               $Id: os_def.h 714 2009-11-27 23:32:26Z vahlsing $ \n
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

#ifndef _OS_DEF_H
#define _OS_DEF_H

#ifdef __cplusplus
extern "C"
{
#endif

/*
typedef unsigned char       UINT8;  //!<  8 bits wide unsigned integer
typedef unsigned short      UINT16; //!< 16 bits wide unsigned integer
typedef unsigned long       UINT32; //!< 32 bits wide unsigned integer
typedef unsigned long long  UINT64; //!< 64 bits wide unsigned integer
typedef signed char         INT8;   //!<  8 bits wide signed integer
typedef signed short        INT16;  //!< 16 bits wide signed integer
typedef signed long         INT32;  //!< 32 bits wide signed integer
typedef char                CHAR;   //!<  8 bits wide char; signedness depends on compiler settings
typedef int                 BOOL;   //!< boolean using platform base type
typedef float               FLOAT;  //!< float
typedef double              DOUBLE; //!< double
*/
   
#ifndef TRUE
#define TRUE   1                //!< Define TRUE value if undefined so far.
#endif
#ifndef FALSE
#define FALSE  0                //!< Define FALSE value if undefined so far.
#endif

#ifndef NULL
#define NULL  (void *) 0        //!< Define NULL pointer value if undefined so far.
#endif


#ifndef UNUSED_PARAM
#define UNUSED_PARAM(x) (void)(x) //!< Mark parameter as being unused to avoid compiler warnings.
#endif

#ifndef UNUSED_VAR
#define UNUSED_VAR(x) (void)(x)	//!< Mark variable as being unused to avoid compiler warnings.
#endif

//to avoid Lint warnings, use it within const context
#ifndef UNUSED_PARAM1
#define UNUSED_PARAM1(x) if((x) != (x)) {ASSERT (FALSE) } //!< Mark parameter as being unused to avoid Lint warnings.
#endif

// documentation keywords for pointer arguments, to tell the direction of the passing                     
#ifndef OUT
    #define OUT                 //!< pointer content is expected to be filled by called function                     
#endif
#ifndef IN
    #define IN                  //!< pointer content contains parameters from the caller                     
#endif
#ifndef INOUT   
    #define INOUT               //!< pointer content is expected to be read and changed                     
#endif

// some helpfull stuff
#define __STRINGIFY(x) #x
#define TOSTRING(x) __STRINGIFY(x)

#ifndef MIN
    #define MIN(a,b) ((a)<(b))?(a):(b)
#endif

#ifdef __cplusplus
}
#endif

#endif //_OS_DEF_H
