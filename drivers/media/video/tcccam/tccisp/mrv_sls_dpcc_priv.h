/*****************************************************************************/
/*!
 *  <!-- DOXYGEN prepared -->
 *  Copyright(c)2004-2009 Silicon Image GmbH
 *
 *  \file        mrv_sls_dpcc_priv.h \n
 *  \version     1.0 \n
 *  \author      Hoffmann \n
 *  \brief       <!-- DESCRIPTION --> Private header for all second level support routines
 *               of the MARVIN second level Defect Pixel Cluster Correction support routines.
 *               These are routines using the plain routines of mrv.h to implement a kind of
 *               "helper" to program/access/use the MARVIN with a bit more abstraction.
 *
 */
/*  HISTORY:
 *  build  | date       | author     | description
 *  1.0.00   20.10.2009   Hoffmann     created
 *  1.0.01   19.11.2009   Hoffmann     split dpcc path selection/deselection into two functions
 *                                     (MrvSlsDpccBpt_SetBpListPath(), MrvSlsDpccBpt_RestorePath())
 *                                     to let Framefun main loop process the frame; notes updated
 */
/*****************************************************************************/

#ifndef _MRV_SLS_DPCC_PRIV_H
#define _MRV_SLS_DPCC_PRIV_H

#if (MARVIN_FEATURE_DPCC == MARVIN_FEATURE_DPCC_V1)

/*****************************************************************************
 * DEFINES
 *****************************************************************************/

//! abbreviations for local debug control ( level | module )
#define DERR  ( DBG_ERR  | DBG_MRV )    //!< errors
#define DWARN ( DBG_WARN | DBG_MRV )    //!< warnings
#define DINFO ( DBG_INFO | DBG_MRV )    //!< information

//! Invalid pixel coordinate; used to identify end of HW generated DPCC list or unused list elements
#define INVALID_PIXEL               (0xFFFFFFFFu)

//! Invalid pixel list index; used to detect end of internal lists
#define INVALID_INDEX               (0u)

//! Number of bits used to identify corresponding pixel array
#define NUM_ARRAY_INDEX_BITS        (4)

//! Number of bits used to identify pixel element in an array
#define NUM_PIXEL_INDEX_BITS        (16 - NUM_ARRAY_INDEX_BITS)

//! Max. number of pixel arrays supported (don't change directly)
#define MAX_NUM_PIXEL_ARRAYS        (1 << NUM_ARRAY_INDEX_BITS)

//! Number of elements in each pixel array
#define NUM_PIXEL_ARRAY_ELEMENTS    (1 << NUM_PIXEL_INDEX_BITS)

//! Size of bad pixel table of HW
#define DPCC_HW_TABLE_SIZE          (MRV_DPCC_BP_TABLE_ADDR_MASK + 1)

/*****************************************************************************
 * TYPEDEFS
 *****************************************************************************/

//! structure to hold information regarding one pixel sometimes detected as defect
/*  These elements are hold in an array (or some arrays allocated on demand - tbd)
 *  Also they are linked together in one of two lists: pixel list and free list.
 *  At initialization all elements are part of the free list where the pixel list
 *  is empty.
 *  While reading a DPCC list received from HW pixels are searched in pixel list.
 *  If the pixel is found ucDetectCount is incremented.
 *  Otherwise an element is taken from free list and inserted in pixel list.
 *  Finally pixel list is checked for false positives which are removed from list
 *  (or kept if a last positives history is used/implemented - tbd)
 *  All positives then are written to HW.
 *
 *  Restrictions:
 *  - the number of defect pixel candidates is limited to 65535 (0xFFFF)
 *
 *  tbd1: How to reduce list if HW limit is reached? They are ordered by coord,
 *        not by number of detections.
 *   -->  Currently we count limit, limit * 1.25 and limit * 1.5; e.g. 20, 25, 30
 *        If too much pixels reach limit * 1.5 RET_OUTOFRANGE is returned.
 *        In this case caller (application) must stop DPCC, select another register set
 *        and restart DPCC.
 *
 *  tbd2: Some kind of history
 *        On final step shift all ucPositives one bit to the right and
 *        set bit7 if pixel is written into HW list.
 *        This history could also be used to write pixels into HW list which
 *        do not fullfill criteria in current test sequence
 *        (but a second criteria, e.g. 50% of first criteria)
 *   -->  Currently there is a second limit which must be reached if pixel was detected
 *        as defect in previous sequence. Then pixel is also put into HW table.
 *
 *  tbd3: dynamic memory allocation
 *        The number of elements required can vary in a wide range. We can
 *        a) use a fixed sized array large enough for "normal" cases
 *           (65536 elements = 512kByte)
 *        b) allocate memory as needed (but in chunks of e.g. 4096 elements = 32kByte)
 *           In this case usNextPixel could contain array id in bit15..12
 *           and elements index in that array in bit11..0
 *           This allows up to 16 arrays with 4096 elements each.
 *           The pointers to the arrays must be stored in a separate array.
 *           (so we should keep max. number of arrays at a minimum)
 *        If dynamic memory allocation and history are used together we need
 *        a function to "compress" elements to reduce number of arrays used.
 *   -->  Currently we use 16 arrays of 4096 elements each, allocated as needed.
 *        Memory is freed only if MrvSlsDpcc_End() is called.
 */
typedef struct tsMrvSlsDpccBptPixel_
{
	UINT32    ulPixelCoord;  //!< offset of the pixel; used as sorting criteria
    UINT8     ucDetectCount; //!< number of detections in current test sequence
    UINT8     ucPositives;   //!< last positives (as bit field, bit0 = oldest test sequence)
    UINT16    usNextPixel;   //!< index of next pixel in list or INVALID_INDEX
} tsMrvSlsDpccBptPixel;

//! structure to hold information regarding complete pixel list
/*  list handling strategy:
 *  To simplify list navigation first element of first array (pixel index 0) is
 *  always marked as INVALID_PIXEL and always refered to by usFreePixel.
 *  It's usNextPixel points to next unused element.
 *  The second element of first array has always ulPixelCoord set to 0 and refered
 *  to by usFirstPixel. It could be used if pixel at coordinates (0, 0) is detected
 *  as defect.
 *  When this module becomes activated the first array is created and its elements
 *  where connected as described above.
 */
typedef struct tsMrvSlsDpccBptContext_
{
	tsMrvSlsDpccBptPixel* apPixelArrays[MAX_NUM_PIXEL_ARRAYS]; //!< array of pointers to allocated pixel arrays
	tsMrvSlsDpccBptPixel* pCurrentPixel;                       //!< pointer to current pixel (start pos to check/insert next pixel)
	tsMrvSlsDpccBptPixel* pFreePixel;                          //!< pointer to first pixel in free list or NULL
	UINT16                usPixelArrayCount;                   //!< number of arrays allocated
	UINT16                usFirstPixel;                        //!< index of first pixel in pixel list or INVALID_INDEX
	UINT16                usMeasurementsLeft;                  //!< current number of measurements until series is complete
	UINT16                usFramesToSkip;                      //!< current number of frames to skip before next measurement
	BOOL                  bDpccPathActive;                     //!< set to TRUE while bp list is generated (DPCC path is active)
	tsMrvMiPathConf       tMrvMiPathConfOld;                   //!< stores previous MI path configuration while bp list is generated
	tsMrvMiCtrl           tMrvMiCtrlOld;                       //!< stores previous MI ctrl while bp list is generated
} tsMrvSlsDpccBptContext;


/*****************************************************************************
 * GLOBALS
 *****************************************************************************/


/*****************************************************************************
 * PRIVATE PROTOTYPES
 *****************************************************************************/

static RESULT MrvSlsDpccBpt_Init(const tsMrvDpccBptConfig *ptDpccBptConfig);

static RESULT MrvSlsDpccBpt_End(void);

static RESULT MrvSlsDpccBpt_AnalyseMeasurement(const tsMrvDpccBptConfig *ptDpccBptConfig, UINT32 *pPixelCoords);

static RESULT MrvSlsDpccBpt_WriteNewTable(const tsMrvDpccBptConfig *ptDpccBptConfig);

static RESULT MrvSlsDpccBpt_AddArray(void);

static RESULT MrvSlsDpccBpt_AddPixel(UINT32 ulPixelCoord, UINT8 ucDetectVal);

static RESULT MrvSlsDpccBpt_CleanupPixelList(UINT8 ucDetectLimit, UINT8 ucPositivesLimit);

static tsMrvSlsDpccBptPixel* MrvSlsDpccBpt_GetPixelPtr(UINT16 usPixel);

static RESULT MrvSlsDpccBpt_RequestBpList(tsPlSystemConfig *ptSysConf);
static RESULT MrvSlsDpccBpt_SetBpListPath(tsPlSystemConfig *ptSysConf);
static RESULT MrvSlsDpccBpt_RestorePath(tsPlSystemConfig *ptSysConf);

static RESULT MrvSlsDpccBpt_DumpPixelList(const tsMrvDpccBptConfig *ptDpccBptConfig, UINT8 ucSet, UINT32 ulResolution);

#endif /* (MARVIN_FEATURE_DPCC == MARVIN_FEATURE_DPCC_V1) */

#endif /* _MRV_SLS_DPCC_PRIV_H */
