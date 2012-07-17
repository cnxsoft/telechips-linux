/*****************************************************************************/
/*!
 *  \file        reg_access.h \n
 *  \version     1.0 \n
 *  \author      Hoffmann \n
 *  \brief       Makros to read and write parts of register values
 *               or elements of register arrays.
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: reg_access.h 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
*/
/*****************************************************************************/

#ifndef _REG_ACCESS_H
#define _REG_ACCESS_H


/* Notes:

 registers:
   - use these macros to allow a central way e.g. to print out debug information on register access

 slices:
   - "parameter" \a reg could be a hardware register or a (32bit) variable, but not a pointer!
   - each slice (specified as "parameter" \a name) requires two \#defines:
       \b \<name\>_MASK  : defines the mask to use on register side
       \b \<name\>_SHIFT : defines the shift value to use (left on write, right on read)

 arrays:
   - "parameter" \a areg could be an (32bit) array or a pointer to the first array element
   - each one-dimensional array (specified as "parameter" \a name) requires one \#define:
      - <tt> \<name\>_ARR_SIZE </tt>: number of elements
   - each two-dimensional array (specified as "parameter" <name>) requires four \#defines:
      - <tt> \<name\>_ARR_SIZE1 </tt>: number of elements in first dimension
      - <tt> \<name\>_ARR_SIZE2 </tt>: number of elements in second dimension
      - <tt> \<name\>_ARR_OFS1  </tt>: offset between two consecutive elements in first dimension
      - <tt> \<name\>_ARR_OFS2  </tt>: offset between two consecutive elements in second dimension
 */


// =========================================================================================
/*! reads and returns the complete value of register \a reg
 * \note Use these macro to allow a central way e.g. to print out debug information on register access.
 */
/*
#define REG_READ(reg) __REG_READ((reg), __FILE__ "(%d) : REG_READ(" VAL2STR(reg) "): 0x%08X\n", __LINE__)

// helper function to let REG_READ return the value
UINT32 __REG_READ(UINT32 reg, const char* text, UINT32 line)
{
	UINT32 variable = reg;
	DBG_OUT(((DBG_INFO|DBG_REG), text, line, variable));
	return variable;
}
*/
/*
UINT32 REG_READ(UINT32 reg)
{
	UINT32 variable = (unsigned *) reg;
	DBG_OUT(((DBG_INFO|DBG_REG), "REG_READ(" VAL2STR(reg) ": 0x%08X\n", (variable)));
	return variable;
}
*/
#define REG_READ(reg) ((unsigned *) (reg))


// =========================================================================================
/*! writes the complete value \a value into register \a reg
 * \note Use these macro to allow a central way e.g. to print out debug information on register access.
 */
#define REG_WRITE(reg, value) \
	{ \
		DBG_OUT(((DBG_INFO|DBG_REG), \
		         "REG_WRITE(" VAL2STR(reg) ", " VAL2STR(value) "): 0x%08X\n", (value))); \
		(reg) = (value); \
	}


// =========================================================================================
/*! returns the value of slice \a name from register or variable \a reg
 * \note "parameter" \a reg could be a hardware register or a (32bit) variable, but not a pointer! \n
 *       each slice (specified as "parameter" \a name) requires two \#defines: \n
 *        - <tt>\<name\>_MASK  </tt>: defines the mask to use on register side
 *        - <tt>\<name\>_SHIFT </tt>: defines the shift value to use (left on write, right on read)
 */
#define REG_GET_SLICE(reg, name) \
	(((reg) & (name##_MASK)) >> (name##_SHIFT))

// =========================================================================================
/*! writes the value \a value into slice \a name of register or variable \a reg
 * \note "parameter" \a reg could be a hardware register or a (32bit) variable, but not a pointer! \n
 *       each slice (specified as "parameter" \a name) requires two \#defines: \n
 *        - <tt>\<name\>_MASK  </tt>: defines the mask to use on register side
 *        - <tt>\<name\>_SHIFT </tt>: defines the shift value to use (left on write, right on read)
 */
#define REG_SET_SLICE(reg, name, value) \
	{ \
		DBG_OUT(((DBG_INFO|DBG_REG), \
		         "REG_SET_SLICE(" VAL2STR(reg) ", " VAL2STR(value) "): 0x%08X\n", \
		          (((reg) & ~(name##_MASK)) | (((value) << (name##_SHIFT)) & (name##_MASK))) )); \
	((reg) = (((reg) & ~(name##_MASK)) | (((value) << (name##_SHIFT)) & (name##_MASK)))); \
	}


// =========================================================================================
/*! returns the value of element \a idx from register array or array variable \a areg
 * \note "parameter" \a areg could be an (32bit) array or a pointer to the first array element \n
 *       each one-dimensional array (specified as "parameter" \a name) requires one \#define: \n
 *        - <tt>\<name\>_ARR_SIZE </tt>: number of elements
 */
#define REG_GET_ARRAY_ELEM1(areg, name, idx) \
	( (idx < name##_ARR_SIZE) \
	  ? areg[idx] \
	  : 0 )

// =========================================================================================
/*! writes the value \a value into element \a idx of register array or array variable \a areg
 * \note "parameter" \a areg could be an (32bit) array or a pointer to the first array element \n
 *       each one-dimensional array (specified as "parameter" \a name) requires one \#define: \n
 *        - <tt>\<name\>_ARR_SIZE </tt>: number of elements
 */
#define REG_SET_ARRAY_ELEM1(areg, name, idx, value) \
	( (idx < name##_ARR_SIZE) \
	  ? areg[idx] = value \
	  : 0 )

// =========================================================================================
/*! returns the value of element \a idx1, \a idx2 from two-dimensional register array or
 *  array variable \a areg
 * \note "parameter" \a areg could be an (32bit) array or a pointer to the first array element \n
 *       each two-dimensional array (specified as "parameter" \a name) requires four \#defines:
 *        - <tt>\<name\>_ARR_SIZE1 </tt>: number of elements in first dimension
 *        - <tt>\<name\>_ARR_SIZE2 </tt>: number of elements in second dimension
 *        - <tt>\<name\>_ARR_OFS1  </tt>: offset between two consecutive elements in first dimension
 *        - <tt>\<name\>_ARR_OFS2  </tt>: offset between two consecutive elements in second dimension
 */
#define REG_GET_ARRAY_ELEM2(areg, name, idx1, idx2) \
	( ((idx1 < name##_ARR_SIZE1) && (idx2 < name##_ARR_SIZE2)) \
	  ? areg[(idx1 * name##_ARR_OFS1) + (idx2 * name##_ARR_OFS2)] \
	  : 0 )

// =========================================================================================
/*! writes the value \a value into element \a idx1, \a idx2 of two-dimensional register array or
 *  array variable \a areg
 * \note "parameter" \a areg could be an (32bit) array or a pointer to the first array element \n
 *       each two-dimensional array (specified as "parameter" \a name) requires four \#defines:
 *        - <tt>\<name\>_ARR_SIZE1 </tt>: number of elements in first dimension
 *        - <tt>\<name\>_ARR_SIZE2 </tt>: number of elements in second dimension
 *        - <tt>\<name\>_ARR_OFS1  </tt>: offset between two consecutive elements in first dimension
 *        - <tt>\<name\>_ARR_OFS2  </tt>: offset between two consecutive elements in second dimension
 */
#define REG_SET_ARRAY_ELEM2(areg, name, idx1, idx2, value) \
	( ((idx1 < name##_ARR_SIZE1) && (idx2 < name##_ARR_SIZE2)) \
	  ? areg[(idx1 * name##_ARR_OFS1) + (idx2 * name##_ARR_OFS2)] = value \
	  : 0 )


#endif // _REG_ACCESS_H

