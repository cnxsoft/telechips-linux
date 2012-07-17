/*****************************************************************************/
/*!
 *  <!-- DOXYGEN prepared -->
 *  Copyright(c)2004-2009 Silicon Image GmbH
 *
 *  \file        mrv_sls_exif.c \n
 *  \version     1.0 \n
 *  \author      Frommelt \n
 *  \brief       <!-- DESCRIPTION --> Implementation for second level support routines
 *               to support snapshots with EXIF features.
 */
/*  HISTORY:
 *  build  | date       | author     | description
 *  1.0.00   16.03.2009   Frommelt     created
 *  1.0.01   23.07.2009   Frommelt     linted
 *  1.0.02   28.07.2009   Frommelt     added Filter for fade in windows.
 *  1.0.03   29.07.2009   Frommelt     added name of the EXIF data file in config file.
 *                                     added horizontal averaging in MrvSls_ExifGenerateBmp().
 *                                     added to skip thumbnails if viewfinder settings used, 
 *                                     which change the thumbnail but not the main image.
 *  1.0.04   04.08.2009   Frommelt     renamed structure tMrvAEtoAWB to tMrvAEtoSLS and added EXIF support.
 *  1.0.05   05.08.2009   Frommelt     added Setpoint by screen evaluation into calculation exposure bias.
 *  1.0.06   19.10.2009   Frommelt     added brackets into line 909/913 for definition of s_iTime. 
 */
/*****************************************************************************/

#include "stdinc.h"

#ifdef TODO
	#include "IniFile.h"
	#include "BuildConfig.h"
	#include "frmfun_priv.h"
#endif // TODO

#if MRV_SUPPORT_SL
//#ifdef SL_SUPPORT_PARTLY


//! Abbreviations for local debug control ( level | module )
#define DERR  ( DBG_ERR  | DBG_MRV )    //!< errors
#define DWARN ( DBG_WARN | DBG_MRV )    //!< warnings
#define DINFO ( DBG_INFO | DBG_MRV )    //!< information

//! ExposureTime and Gain from camera AEC driver
extern tsMrvAEtoSLS tMrvAEtoSLS;

// The space for the EXIF header is defined as constant. 
// For further upgrading of the EXIF driver it has to new calculated.
#define MAX_EXIF_HEADER 1024

//! General definitions for EXIF header
#define START_TIFF_HEADER 0x0c
#define START_EXIF_DATA   4
#define EXIF_U16 2
#define EXIF_U32 4
#define EXIF_ENTRY_LENGTH 12
#define BIG_ENDIAN_ORDER 1
#define LITTLE_ENDIAN_ORDER 0
#define NO_ENTRY 254

UINT8  BYTE_ORDER = LITTLE_ENDIAN_ORDER;

//! Definitions for the config file
#define BUF_LENGTH  512

#define SECTION_EXIF_IFD0                "Ifd0"
#define SECTION_EXIF_SUBIFD              "SubIfd"
#define SECTION_EXIF_IFD1                "Ifd1"

//! Definitions for the thumbnail
#define THUMBNAIL_SKIP_MASK ( FF_VFFLAG_MODE_FULLLCD_USDS | \
                             FF_VFFLAG_MODE_LETTERBOX | \
                             FF_VFFLAG_MODE_USER | \
                             FF_VFFLAG_ROT90_CCW |\
                             FF_VFFLAG_V_FLIP |\
                             FF_VFFLAG_MIRROR |\
                             FF_VFFLAG_SHOWWND_AEC  |\
                             FF_VFFLAG_SHOW_LENSPOS  |\
                             FF_VFFLAG_SHOW_AF_SUM   |\
                             FF_VFFLAG_USE_MAINPATH  |\
                             FF_VFFLAG_SHOW_AF_TREND  )     

#define THUMBNAIL_WIDTH  160 // 640:4
#define THUMBNAIL_HEIGHT 120 // 480:4
#define THUMBNAIL_SIZE (THUMBNAIL_WIDTH * THUMBNAIL_HEIGHT * 2)
#define MAX_EXIF_BUFFER_LENGTH (THUMBNAIL_SIZE + BMP_DATA_OFFSET + MAX_EXIF_HEADER)
#define BMP_DATA_OFFSET        70
#define BMP_HEADER_SIZE        40
#define BITS_PER_PIXEL         16
#define COMPRESSION_METHOD     3
#define NUMBER_OF_COLOR_PLANES 1
#define RED_PIXEL_MASK         0xF800
#define GREEN_PIXEL_MASK       0x07E0
#define BLUE_PIXEL_MASK        0x1F

#define KEY_THUMBNAIL          "iThumbnail"

//! Structure for horizontal averaging of RGB
typedef struct 
{
    unsigned Rbit;
    unsigned Gbit;
    unsigned Bbit;  

}tsMrvBmpPixel;

//! Filter to filter out some fade in windows
#define FILTER_ON

//! Float to rational factor
#define FLOAT_RATIONAL_FACTOR 1000

//! Definitions for the Light Source TAG 0x9208
#define LIGHT_SOURCE_DAYLIGHT               1
#define LIGHT_SOURCE_FLUORESCENT            2
#define LIGHT_SOURCE_TUNGSTEN               3
#define LIGHT_SOURCE_FINE_WEATHER           9
#define LIGHT_SOURCE_CLOUDY_WEATHER         10
#define LIGHT_SOURCE_SHADE                  11
#define LIGHT_SOURCE_COOL_WHITE_FLUORESCENT 14
#define LIGHT_SOURCE_STANDARD_LIGHT_A       17
#define LIGHT_SOURCE_D65                    21
#define LIGHT_SOURCE_D75                    22
#define LIGHT_SOURCE_D50                    23
#define LIGHT_SOURCE_OTHER                  255

//! Definitions for the White balance TAG 0xA403
#define AUTO_WHITE_BALANCE 0
#define MANUAL_WHITE_BALANCE 1

//! Definitions for the exposure bias TAG 0x9204
#define TARGET_SETPOINT 45
#define CONST_LOG2   0.69314718f

//! Definitions for the iso bias TAG 0x9204
#define GAIN_TO_ISO_FACTOR  0.1f

//! Internal structur of TAG table
typedef struct {
    UINT16 usTag;
    UINT16 usDataTyp;
    char * strDesc;

}TagTable_t;

//! Constants for EXIF header
static const UINT8 EXIF_START[] = 
{
    0xFF,0xD8,0xFF,0xE1,0x00,0x00,0x45,0x78,
    0x69,0x66,0x00,0x00,0x49,0x49,0x2A,0x00,
    0x08,0x00,0x00,0x00
};

//! Data types for EXIF header
#define TYPE_STRING    2
#define TYPE_USHORT    3
#define TYPE_ULONG     4
#define TYPE_URATIONAL 5
#define TYPE_BINARY    7
#define TYPE_SRATIONAL 10

//! Tag values for EXIF header
#define TAG_MAKE                   0x010F
#define TAG_MODEL                  0x0110
#define TAG_ORIENTATION            0x0112
#define TAG_X_RESOLUTION           0x011A
#define TAG_Y_RESOLUTION           0x011B
#define TAG_RESOLUTION_UNIT        0x0128
#define TAG_SOFTWARE               0x0131
#define TAG_DATETIME               0x0132
#define TAG_COPYRIGHT              0x8298
#define TAG_EXPOSURETIME           0x829A
#define TAG_FNUMBER                0x829D
#define TAG_EXIF_OFFSET            0x8769
#define TAG_EXPOSURE_PROGRAM       0x8822
#define TAG_ISO_EQUIVALENT         0x8827
#define TAG_EXIF_VERSION           0x9000
#define TAG_DATETIME_ORIGINAL      0x9003
#define TAG_DATETIME_DIGITIZED     0x9004
#define TAG_COMPONENTS_CONFIG      0x9101
#define TAG_EXPOSURE_BIAS          0x9204
#define TAG_MAXAPERTURE            0x9205
#define TAG_METERING_MODE          0x9207
#define TAG_LIGHT_SOURCE           0x9208
#define TAG_FOCALLENGTH            0x920A
#define TAG_FLASH_PIX_VERSION      0xA000
#define TAG_COLOR_SPACE            0xA001
#define TAG_EXIF_IMAGEWIDTH        0xA002
#define TAG_EXIF_IMAGELENGTH       0xA003
#define TAG_SENSING_METHOD         0xA217
#define TAG_EXPOSURE_MODE          0xA402
#define TAG_WHITEBALANCE           0xA403
#define TAG_FOCALLENGTH_35MM       0xA405
#define TAG_SCENE_CAPTURE_TYPE     0xA406
#define TAG_GAIN_CONTROL           0xA407
#define TAG_CONTRAST               0xA408
#define TAG_SATURATION             0xA409
#define TAG_SHARPNESS              0xA40A
#define TAG_DISTANCE_RANGE         0xA40C

#define TAG_IMAGE_WIDTH            0x0100
#define TAG_IMAGE_LENGTH           0x0101
#define TAG_THUMBNAIL_OFFSET       0x0201
#define TAG_THUMBNAIL_LENGTH       0x0202

//! TAG table for IFD0 header
static const TagTable_t TagTable[] = {
  { TAG_MAKE,                   TYPE_STRING,      "sMake"},
  { TAG_MODEL,                  TYPE_STRING,      "sModel"}, 
  { TAG_ORIENTATION,            TYPE_USHORT,      "iOrientation"},
  { TAG_X_RESOLUTION,           TYPE_URATIONAL,   "fXResolution"},
  { TAG_Y_RESOLUTION,           TYPE_URATIONAL,   "fYResolution"},
  { TAG_RESOLUTION_UNIT,        TYPE_USHORT,      "iResolutionUnit"},
  { TAG_SOFTWARE,               TYPE_STRING,      "sSoftware"},
  { TAG_DATETIME,               TYPE_STRING,      "sDateTime"},
  { TAG_COPYRIGHT,              TYPE_STRING,      "sCopyright"},
  { TAG_EXIF_OFFSET,            TYPE_ULONG,       "iExifOffset"},
};

//! TAG table for SubIfd header
static const TagTable_t TagTable2[] = {
  { TAG_EXPOSURETIME,           TYPE_URATIONAL,   "fExposureTime"},
  { TAG_FNUMBER,                TYPE_URATIONAL,   "fFNumber"},
  { TAG_EXPOSURE_PROGRAM,       TYPE_USHORT,      "iExposureProgram"},
  { TAG_ISO_EQUIVALENT,         TYPE_USHORT,      "iISOSpeedRatings"},
  { TAG_EXIF_VERSION,           TYPE_BINARY,      "sExifVersion"},
  { TAG_DATETIME_ORIGINAL,      TYPE_STRING,      "sDateTimeOriginal"},
  { TAG_DATETIME_DIGITIZED,     TYPE_STRING,      "sDateTimeDigitized"},
  { TAG_COMPONENTS_CONFIG,      TYPE_BINARY,      "iComponentsConfiguration"},
  { TAG_EXPOSURE_BIAS,          TYPE_SRATIONAL,   "fExposureBiasValue"},
  { TAG_MAXAPERTURE,            TYPE_URATIONAL,   "fMaxApertureValue"},
  { TAG_METERING_MODE,          TYPE_USHORT,      "iMeteringMode"},
  { TAG_LIGHT_SOURCE,           TYPE_USHORT,      "iLightSource"},
  { TAG_FOCALLENGTH,            TYPE_URATIONAL,   "fFocalLength"},
  { TAG_FLASH_PIX_VERSION,      TYPE_BINARY,      "sFlashPixVersion"},
  { TAG_COLOR_SPACE,            TYPE_USHORT,      "iColorSpace"},
  { TAG_EXIF_IMAGEWIDTH,        TYPE_ULONG,       "iExifImageWidth"},
  { TAG_EXIF_IMAGELENGTH,       TYPE_ULONG,       "iExifImageLength"},
  { TAG_SENSING_METHOD,         TYPE_USHORT,      "iSensingMethod"},            
  { TAG_EXPOSURE_MODE,          TYPE_USHORT,      "iExposureMode"},
  { TAG_WHITEBALANCE,           TYPE_USHORT,      "iWhiteBalance"},
  { TAG_FOCALLENGTH_35MM,       TYPE_USHORT,      "iFocalLengthIn35mmFilm"},
  { TAG_SCENE_CAPTURE_TYPE,     TYPE_USHORT,      "iSceneCaptureType"},
  { TAG_GAIN_CONTROL,           TYPE_USHORT,      "iGainControl"},
  { TAG_CONTRAST,               TYPE_USHORT,      "iContrast"},
  { TAG_SATURATION,             TYPE_USHORT,      "iSaturation"},
  { TAG_SHARPNESS,              TYPE_USHORT,      "iSharpness"},
  { TAG_DISTANCE_RANGE,         TYPE_USHORT,      "iSubjectDistanceRange"},
};

#define TAG_TABLE_SIZE  (sizeof(TagTable) / sizeof(TagTable_t))
#define TAG_TABLE_SIZE2  (sizeof(TagTable2) / sizeof(TagTable_t))


//! TAG table for IFD1 header
static const TagTable_t TagTable3[] = {
  { TAG_IMAGE_WIDTH,            TYPE_ULONG,       "ThumbnailWidth"},
  { TAG_IMAGE_LENGTH,           TYPE_ULONG,       "ThumbnailHeight"},
  { TAG_THUMBNAIL_OFFSET,       TYPE_ULONG,       "ThumbnailOffset"},
  { TAG_THUMBNAIL_LENGTH,       TYPE_ULONG,       "ThumbnailLength"},
};
#define TAG_TABLE_SIZE3  (sizeof(TagTable3) / sizeof(TagTable_t))

/*****************************************************************************/
/*!
 *
 *  \FUNCTION    Put16ui \n
 *
 *  \RETURNVALUE none \n
 *
 *  \PARAMETERS  pointer to Short
 *               UINT16 PutValue\n
 *
 *  \DESCRIPTION Convert a 16 bit unsigned value to file's native byte order \n
 *
 */
/*****************************************************************************/
static void Put16ui(void * pShort, UINT16 usPutValue)
{
    // Check for NULL pointer
    if(pShort != NULL)
    {

        if (BYTE_ORDER){
            ((UINT8 *)pShort)[0] = (UINT8)(usPutValue>>8);
            ((UINT8 *)pShort)[1] = (UINT8)usPutValue;
        }else{
            ((UINT8 *)pShort)[0] = (UINT8)usPutValue;
            ((UINT8 *)pShort)[1] = (UINT8)(usPutValue>>8);
        }
    }
}

/*****************************************************************************/
/*!
 *
 *  \FUNCTION    Put32ui \n
 *
 *  \RETURNVALUE none \n
 *
 *  \PARAMETERS  pointer to Value
 *               UINT32 PutValue\n
 *
 *  \DESCRIPTION Convert a 32 bit unsigned value to file's native byte order \n
 *
 */
/*****************************************************************************/
static void Put32ui(void * pValue, UINT32 ulPutValue)
{
    // Check for NULL pointer
    if(pValue != NULL)
    {
        if (BYTE_ORDER){
            ((UINT8 *)pValue)[0] = (UINT8)(ulPutValue>>24);
            ((UINT8 *)pValue)[1] = (UINT8)(ulPutValue>>16);
            ((UINT8 *)pValue)[2] = (UINT8)(ulPutValue>>8);
            ((UINT8 *)pValue)[3] = (UINT8)ulPutValue;
        }else{
            ((UINT8 *)pValue)[0] = (UINT8)ulPutValue;
            ((UINT8 *)pValue)[1] = (UINT8)(ulPutValue>>8);
            ((UINT8 *)pValue)[2] = (UINT8)(ulPutValue>>16);
            ((UINT8 *)pValue)[3] = (UINT8)(ulPutValue>>24);
        } 
    }
}

/*****************************************************************************/
/*!
 *
 *  \FUNCTION    MrvSls_ExifAddString \n
 *
 *  \RETURNVALUE RET_SUCCESS      - everything ok \n
 *               RET_NULL_POINTER  \n
 *
 *  \PARAMETERS  pointer to pExifBuffer
 *               pointer to string value
 *               pointer to piEntryBuf
 *               pointer to piStringBuf
 *               pointer to pTable \n
 *
 *  \DESCRIPTION added a string value to the EXIF header \n
 *
 */
/*****************************************************************************/
static RESULT MrvSls_ExifAddString
(
    char *pExifBuffer, 
    const char *pString , 
    UINT16 *piEntryBuf , 
    UINT16 *piStringBuf, 
    const TagTable_t *pTable 
)
{

    // Check for NULL pointer
    if((pExifBuffer == NULL) || (pString == NULL) || (piEntryBuf == NULL) || (piStringBuf == NULL) || (pTable == NULL))
    {
        return RET_NULL_POINTER;
    }

    Put16ui(pExifBuffer + *piEntryBuf, pTable->usTag);
    Put16ui(pExifBuffer + *piEntryBuf + 2, pTable->usDataTyp);
    Put32ui(pExifBuffer + *piEntryBuf + 4, strlen(pString) + 1);   
    Put32ui(pExifBuffer + *piEntryBuf + 8, (*piStringBuf - START_TIFF_HEADER));
    
    
    *piEntryBuf +=EXIF_ENTRY_LENGTH;
    
    memcpy(pExifBuffer + *piStringBuf, pString, strlen(pString) + 1);
    *piStringBuf += (UINT16)strlen(pString) + 1;
    return RET_SUCCESS;
}

/*****************************************************************************/
/*!
 *
 *  \FUNCTION    MrvSls_ExifAddRational \n
 *
 *  \RETURNVALUE RET_SUCCESS      - everything ok \n
 *               RET_NULL_POINTER  \n
 *
 *  \PARAMETERS  pointer to pExifBuffer
 *               UINT32 ulDenominator
 *               UINT32 ulNumerator
 *               pointer to piEntryBuf
 *               pointer to piStringBuf
 *               pointer to pTable \n
 *
 *  \DESCRIPTION added an unsigned rational value to the EXIF header \n
 *
 */
/*****************************************************************************/
static RESULT MrvSls_ExifAddRational
(
    char *pExifBuffer, 
    UINT32 ulDenominator ,
    UINT32 ulNumerator , 
    UINT16 *piEntryBuf , 
    UINT16 *piStringBuf, 
    const TagTable_t *pTable 
)
{

    // Check for NULL pointer
    if((pExifBuffer == NULL) || (piEntryBuf == NULL) || (piStringBuf == NULL) || (pTable == NULL))
    {
        return RET_NULL_POINTER;
    }

    Put16ui(pExifBuffer + *piEntryBuf, pTable->usTag);
    Put16ui(pExifBuffer + *piEntryBuf + 2, pTable->usDataTyp);
    Put32ui(pExifBuffer + *piEntryBuf + 4, 1);   
    Put32ui(pExifBuffer + *piEntryBuf + 8, (*piStringBuf - START_TIFF_HEADER));
    
    
    *piEntryBuf +=EXIF_ENTRY_LENGTH;
    
    Put32ui(pExifBuffer + *piStringBuf, ulNumerator);
    *piStringBuf += EXIF_U32;
    Put32ui(pExifBuffer + *piStringBuf, ulDenominator);
    *piStringBuf += EXIF_U32;
    return RET_SUCCESS;
}

/*****************************************************************************/
/*!
 *
 *  \FUNCTION    MrvSls_ExifAddSRational \n
 *
 *  \RETURNVALUE RET_SUCCESS      - everything ok \n
 *               RET_NULL_POINTER  \n
 *
 *  \PARAMETERS  pointer to pExifBuffer
 *               INT32  ulDenominator
 *               UINT32 ulNumerator
 *               pointer to piEntryBuf
 *               pointer to piStringBuf
 *               pointer to pTable \n
 *
 *  \DESCRIPTION added a signed rational value to the EXIF header \n
 *
 */
/*****************************************************************************/
static RESULT MrvSls_ExifAddSRational
(
    char *pExifBuffer, 
    UINT32 ulDenominator ,
    INT32 ulNumerator , 
    UINT16 *piEntryBuf , 
    UINT16 *piStringBuf, 
    const TagTable_t *pTable 
)
{

    // Check for NULL pointer
    if((pExifBuffer == NULL) || (piEntryBuf == NULL) || (piStringBuf == NULL) || (pTable == NULL))
    {
        return RET_NULL_POINTER;
    }

    Put16ui(pExifBuffer + *piEntryBuf, pTable->usTag);
    Put16ui(pExifBuffer + *piEntryBuf + 2, pTable->usDataTyp);
    Put32ui(pExifBuffer + *piEntryBuf + 4, 1);   
    Put32ui(pExifBuffer + *piEntryBuf + 8, (*piStringBuf - START_TIFF_HEADER));
    
    
    *piEntryBuf +=EXIF_ENTRY_LENGTH;
    
    Put32ui(pExifBuffer + *piStringBuf, (UINT32)ulNumerator);
    *piStringBuf += EXIF_U32;
    Put32ui(pExifBuffer + *piStringBuf, ulDenominator);
    *piStringBuf += EXIF_U32;
    return RET_SUCCESS;
}

/*****************************************************************************/
/*!
 *
 *  \FUNCTION    MrvSls_ExifAddBinary \n
 *
 *  \RETURNVALUE RET_SUCCESS      - everything ok \n
 *               RET_NULL_POINTER  \n
 *
 *  \PARAMETERS  pointer to pExifBuffer
 *               UINT32 ulValue
 *               pointer to piEntryBuf
 *               pointer to pTable \n
 *
 *  \DESCRIPTION added a binary value to the EXIF header \n
 *
 */
/*****************************************************************************/
static RESULT MrvSls_ExifAddBinary
(
    char *pExifBuffer, 
    UINT32 ulValue , 
    UINT16 *piEntryBuf , 
    const TagTable_t *pTable 
)
{

        // Check for NULL pointer
    if((pExifBuffer == NULL) || (piEntryBuf == NULL) || (pTable == NULL))
    {
        return RET_NULL_POINTER;
    }

    Put16ui(pExifBuffer + *piEntryBuf, pTable->usTag);
    Put16ui(pExifBuffer + *piEntryBuf + 2, pTable->usDataTyp);
    Put32ui(pExifBuffer + *piEntryBuf + 4, 4);   
    *((UINT8*)pExifBuffer + *piEntryBuf + 8) = (UINT8)(ulValue>>24);
    *((UINT8*)pExifBuffer + *piEntryBuf + 9) = (UINT8)(ulValue>>16);
    *((UINT8*)pExifBuffer + *piEntryBuf + 10) = (UINT8)(ulValue>>8);
    *((UINT8*)pExifBuffer + *piEntryBuf + 11) = (UINT8)ulValue;
    
    *piEntryBuf +=EXIF_ENTRY_LENGTH;   
    return RET_SUCCESS; 
}

/*****************************************************************************/
/*!
 *
 *  \FUNCTION    MrvSls_ExifAddUshort \n
 *
 *  \RETURNVALUE RET_SUCCESS      - everything ok \n
 *               RET_NULL_POINTER  \n
 *
 *  \PARAMETERS  pointer to pExifBuffer
 *               UINT32 ulValue
 *               pointer to piEntryBuf
 *               pointer to pTable \n
 *
 *  \DESCRIPTION added a unsigned short value to the EXIF header \n
 *
 */
/*****************************************************************************/
static RESULT MrvSls_ExifAddUshort
(
    char *pExifBuffer, 
    UINT16 usValue , 
    UINT16 *piEntryBuf , 
    const TagTable_t *pTable 
)
{

    // Check for NULL pointer
    if((pExifBuffer == NULL) || (piEntryBuf == NULL) || (pTable == NULL))
    {
        return RET_NULL_POINTER;
    }

    Put16ui(pExifBuffer + *piEntryBuf, pTable->usTag);
    Put16ui(pExifBuffer + *piEntryBuf + 2, pTable->usDataTyp);
    Put32ui(pExifBuffer + *piEntryBuf + 4, 1);   
    Put32ui(pExifBuffer + *piEntryBuf + 8, usValue);
    
    
    *piEntryBuf +=EXIF_ENTRY_LENGTH;   
    return RET_SUCCESS; 
}

/*****************************************************************************/
/*!
 *
 *  \FUNCTION    MrvSls_ExifAddUlong \n
 *
 *  \RETURNVALUE RET_SUCCESS      - everything ok \n
 *               RET_NULL_POINTER  \n
 *
 *  \PARAMETERS  pointer to pExifBuffer
 *               UINT32 ulValue
 *               pointer to piEntryBuf
 *               pointer to pTable \n
 *
 *  \DESCRIPTION added a unsigned long value to the EXIF header \n
 *
 */
/*****************************************************************************/
static RESULT MrvSls_ExifAddUlong
(
    char *pExifBuffer, 
    UINT32 ulValue , 
    UINT16 *piEntryBuf , 
    const TagTable_t *pTable 
)
{

    // Check for NULL pointer
    if((pExifBuffer == NULL) || (piEntryBuf == NULL) || (pTable == NULL))
    {
        return RET_NULL_POINTER;
    }

    Put16ui(pExifBuffer + *piEntryBuf, pTable->usTag);
    Put16ui(pExifBuffer + *piEntryBuf + 2, pTable->usDataTyp);
    Put32ui(pExifBuffer + *piEntryBuf + 4, 1);   
    Put32ui(pExifBuffer + *piEntryBuf + 8, ulValue);
    
    
    *piEntryBuf +=EXIF_ENTRY_LENGTH;   
    return RET_SUCCESS; 
}

/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSls_InitExif \n
 *  \RETURNVALUE RET_SUCCESS      - everything ok \n
 *               RET_FAILURE = cannot allocate enough buffer space \n
 *               
 *  \PARAMETERS  pointer to pointer to pExifBuffer \n
 *
 *  \DESCRIPTION Initialization of the EXIF buffer \n
 */
/*****************************************************************************/
RESULT MrvSls_InitExif
(
    char **pExifBuffer   
)
{
    
    *pExifBuffer = DBG_MALLOC(MAX_EXIF_BUFFER_LENGTH);
    if(!*pExifBuffer) 
    {
        return RET_FAILURE;
    }

    return RET_SUCCESS;
}

/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSls_ExitExif \n
 *  \RETURNVALUE none \n
 *              
 *               
 *  \PARAMETERS  pointer to pExifBuffer \n
 *
 *  \DESCRIPTION Releases the EXIF buffer \n
 */
/*****************************************************************************/
void MrvSls_ExitExif
(
    char *pExifBuffer   
)
{
    if(pExifBuffer) 
    {
        DBG_FREE(pExifBuffer);
    }
}

/*****************************************************************************/
/*!
 *
 *  \FUNCTION    MrvSls_ExifGenerateBmp \n
 *
 *  \RETURNVALUE RET_SUCCESS      - everything ok \n
 *               RET_NULL_POINTER  \n
 *
 *  \PARAMETERS  pointer to ptSysConf structure
 *               pointer to the BMP buffer \n
 *
 *  \DESCRIPTION Reads the image from LCD buffer and converts them to bmp format \n
 *
 */
/*****************************************************************************/
RESULT MrvSls_ExifGenerateBmp
(
    const tsPlSystemConfig *ptSysConf, 
    char *pBmpBuffer   
)
{
    tsVBufDesc    tLcdBuf;
    int           iBufferset;
    UINT8*        pucY;
    UINT32        i=0,j=0,k=0,l=0;
    UINT32        Sumr,Sumb,Sumg,ulHigh,ulLow;
    tsMrvBmpPixel BmpPixel[4];

    
    // Check for NULL pointer
    if((ptSysConf == NULL) || (pBmpBuffer == NULL))
    {
        return RET_NULL_POINTER;
    }

    memset( &tLcdBuf, 0, sizeof(tsVBufDesc) );

    if (ptSysConf->ptConfig->tViewFinder.ulFlags & FF_VFFLAG_DIRECT_FB_ACCESS)
    {   // If direct frame buffer access is to be used, let marvin write to the
        // next buffer to hide the buffer filling process in case we use multi-buffering.
        iBufferset = LCDSLS_BUFFERSET_NEXT;
    }
    else
    {   // If direct frame buffer access is NOT to be used, let marvin write to the current
        // video buffer. Due to the physically different video and frame buffer locations
        // this will not be visible anyway, but we may copy the next video buffer to the next
        // frame buffer (which are also the previous ones in the ususal case of double buffering)
        // while marvin is filling the buffer (bCopyInBackground = TRUE).
        iBufferset = LCDSLS_BUFFERSET_CURRENT;
    }
    LcdSls_VBufGetDesc(&tLcdBuf, iBufferset);
    k = THUMBNAIL_SIZE;    // THUMBNAIL_WIDTH * THUMBNAIL_HEIGHT * 2;
    pucY = (UINT8*)tLcdBuf.pucY;
    for(j=0;j<VGA_SIZE_V;j++)
    {
        if(!(j%4)) // vertical subsampling 
        {
            for(i=0;i<VGA_SIZE_H;i++)
            {
                // get RGB 565 pixel from video buffer
                ulLow = *(pucY + (j * VGA_SIZE_H * 2) + ((VGA_SIZE_H - (i+1)) * 2)); 
                ulHigh = *(pucY + (j * VGA_SIZE_H * 2) + ((VGA_SIZE_H - (i+1)) * 2) + 1); 
                #ifdef FILTER_ON                 
                if(((ulLow == 0 && ulHigh == 0) || (ulLow == 0xFF && ulHigh == 0xFF)) && j<(VGA_SIZE_V-2) && i<(VGA_SIZE_H-3))
                {
                    ulLow = *(pucY + ((j+2) * VGA_SIZE_H * 2) + ((VGA_SIZE_H - (i+3)) * 2)); 
                    ulHigh = *(pucY + ((j+2) * VGA_SIZE_H * 2) + ((VGA_SIZE_H - (i+3)) * 2) + 1); 
                }
                #endif // FILTER_ON 
                
                // convert RGB 565 to RGB pixel
                BmpPixel[l].Rbit = ulHigh >> 3;
                BmpPixel[l].Gbit = ((ulHigh & 7) << 3)|(ulLow >> 5);
                BmpPixel[l].Bbit = ulLow & 31;
              
                l++;
                if(l==4)
                {
                    k-=2;
                    // horizontal averaging of 4 neighboring pixels 
                    // and subsampling to perform horizontal downscaling 
                    Sumr=(BmpPixel[0].Rbit + BmpPixel[1].Rbit + BmpPixel[2].Rbit + BmpPixel[3].Rbit);
                    Sumb=(BmpPixel[0].Bbit + BmpPixel[1].Bbit + BmpPixel[2].Bbit + BmpPixel[3].Bbit);
                    Sumg=(BmpPixel[0].Gbit + BmpPixel[1].Gbit + BmpPixel[2].Gbit + BmpPixel[3].Gbit);
                    Sumr=Sumr>>2;
                    Sumb=Sumb>>2;
                    Sumg=Sumg>>2;

                    // convert RGB pixel to RGB 565
                    ulHigh=(Sumr<<3)|(Sumg>>3);
                    ulLow=((Sumg&7)<<5)| Sumb;

                    // put RGB 565 pixel to EXIF buffer
                    *(pBmpBuffer + k) =(char) ulLow; 
                    *(pBmpBuffer + k + 1) =(char) ulHigh; 
                    
                    l=0;
                }  
            }
        }
    }
    return RET_SUCCESS;
} 

/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSls_ExifGenerateHeader \n
 *  \RETURNVALUE RET_SUCCESS      - everything ok \n
 *               RET_NULL_POINTER  \n
 *
 *  \PARAMETERS  pointer to ptSysConf structure 
 *               pointer to ptSnapConf structure 
 *               pointer to pExifBuffer 
 *               pointer to pulSize \n  
 *
 *  \DESCRIPTION Generates the EXIF header with embedded thumbnail \n
 */
/*****************************************************************************/
RESULT MrvSls_ExifGenerateHeader
(
    const   tsPlSystemConfig *ptSysConf, 
    const   tsSnapshotConfig *ptSnapConf, 
    char    *pExifBuffer, 
    UINT32  *pulSize   
)
{
    RESULT  iError = RET_SUCCESS;
    UINT16  iStringBuf = 0, i;
    UINT16  iEntryBuf = 0; 
    UINT16  iExifOffset = 0;
    char    szTime [30] = {0}; 
    UINT16  ulWidth, ulHeight;
    char    szBuf[BUF_LENGTH];
    UINT16  usInput, usEntryCount=0;
    FLOAT   fInput;
    UINT32  ulSize = 0;
    UINT32  ulValue;
    char    *pEntryMarker;
    char    *pszFullName;
   
  
    // Check for NULL pointer
    if((ptSysConf == NULL) || (ptSnapConf == NULL) || (pExifBuffer == NULL) || (pulSize == NULL))
    {
        DBG_OUT( (DERR, "EXIF: NULL pointer received!!\n") );
        return RET_NULL_POINTER;
    }
  
    ulSize += strlen( FRAMEFUN_CONFIG_PATH ) + strlen( g_acCameraName ) + 2; // +2 for additional "\" and terminating "0"
    if(ptSnapConf->pszFileNameExifData != NULL)
    {
        ulSize += strlen( ptSnapConf->pszFileNameExifData );
    }
    else
    {
        ulSize += 8;
    }
    pszFullName = ( char* )DBG_MALLOC( ulSize );
    if ( pszFullName != NULL )
    {
        // add path to file name
        if(ptSnapConf->pszFileNameExifData != NULL)
        {
            sprintf( pszFullName, "%s%s\\%s", FRAMEFUN_CONFIG_PATH, g_acCameraName, ptSnapConf->pszFileNameExifData );
        }
        else
        {
            sprintf( pszFullName, "%s%s\\%s", FRAMEFUN_CONFIG_PATH, g_acCameraName, "Exif.cfg" ); 
        }
    }
    else
    {
        DBG_OUT( (DERR, "EXIF: Exif.cfg not found\n" ) );
    }

    // Exif marker and Tiff header
    memcpy(pExifBuffer, EXIF_START, sizeof(EXIF_START));
    
    iEntryBuf += sizeof(EXIF_START);
    
    //-------------Start Ifd0---------------------
    // add exif entries (number * 12 byte) and 4 byte for pointer to IFD1
    iStringBuf += iEntryBuf + (TAG_TABLE_SIZE * EXIF_ENTRY_LENGTH) + EXIF_U32;
    
    // save pointer of exif number of entries for Ifd0
    pEntryMarker = pExifBuffer + iEntryBuf;
    // set real number of entries counter to 0
    usEntryCount = 0;
    // write number of Ifd0 exif entries to header
    Put16ui(pExifBuffer + iEntryBuf, TAG_TABLE_SIZE);
    iEntryBuf += EXIF_U16;
    iStringBuf += EXIF_U16;
    
    // exif entries Ifd0
    for (i=0;i<TAG_TABLE_SIZE; i++)
    {
        
        //DBG_OUT( (DINFO, "Exif = %x\n",TagTable[i].usTag ) );
        switch(TagTable[i].usTag)
        {
            
            case TAG_X_RESOLUTION: // CFG-File
                fInput = InifileReadFloat(SECTION_EXIF_IFD0, TagTable[i].strDesc, 0.0f, pszFullName);
                if(fInput != 0.0f)
                {
                   iError |= MrvSls_ExifAddRational(pExifBuffer, FLOAT_RATIONAL_FACTOR, (UINT32)(fInput * FLOAT_RATIONAL_FACTOR), &iEntryBuf, &iStringBuf, &TagTable[i] );                      
                   usEntryCount++;
                }
                break;
            case TAG_Y_RESOLUTION: // CFG-File
                fInput = InifileReadFloat(SECTION_EXIF_IFD0, TagTable[i].strDesc, 0.0f, pszFullName);
                if(fInput != 0.0f)
                {
                   iError |= MrvSls_ExifAddRational(pExifBuffer, FLOAT_RATIONAL_FACTOR, (UINT32)(fInput * FLOAT_RATIONAL_FACTOR), &iEntryBuf, &iStringBuf, &TagTable[i]);  
                   usEntryCount++;                
                }
                break;
            case TAG_RESOLUTION_UNIT: // CFG-File
                usInput = (UINT16)InifileReadInt(SECTION_EXIF_IFD0, TagTable[i].strDesc, 0, pszFullName);
                if(usInput != 0)
                {
                   iError |= MrvSls_ExifAddUshort(pExifBuffer, usInput, &iEntryBuf, &TagTable[i]);  
                   usEntryCount++;                
                }
                break;
            case TAG_ORIENTATION: // CFG-File
                usInput = (UINT16)InifileReadInt(SECTION_EXIF_IFD0, TagTable[i].strDesc, 0, pszFullName);
                if(usInput != 0)
                {
                   iError |= MrvSls_ExifAddUshort(pExifBuffer, usInput, &iEntryBuf, &TagTable[i]);  
                   usEntryCount++;                
                }
                break;
            case TAG_MAKE: // CFG-File
               (void)InifileReadString(SECTION_EXIF_IFD0, TagTable[i].strDesc, "", szBuf, BUF_LENGTH, pszFullName);
                if(szBuf[0] != 0)
                {
                   iError |= MrvSls_ExifAddString(pExifBuffer, szBuf , &iEntryBuf, &iStringBuf, &TagTable[i] );  
                   usEntryCount++;                
                }
                break;
    
            case TAG_MODEL: // CFG-File
                (void)InifileReadString(SECTION_EXIF_IFD0, TagTable[i].strDesc, "", szBuf, BUF_LENGTH, pszFullName);
                if(szBuf[0] != 0)
                {                
                   iError |= MrvSls_ExifAddString(pExifBuffer, szBuf , &iEntryBuf, &iStringBuf, &TagTable[i] );  
                   usEntryCount++;                
                }
                break;
            case TAG_SOFTWARE: // CFG-File
                (void)InifileReadString(SECTION_EXIF_IFD0, TagTable[i].strDesc, "", szBuf, BUF_LENGTH, pszFullName);
                if(szBuf[0] != 0)
                {
                   iError |= MrvSls_ExifAddString(pExifBuffer, szBuf , &iEntryBuf, &iStringBuf, &TagTable[i] );  
                   usEntryCount++;                
                }
                break;
            case TAG_DATETIME: // SYSTEM
#ifdef NO_SEMIHOSTING
                {
                    static int s_iTime = 0;
                    s_iTime++;
                    sprintf(szTime, "%02d:%02d", (s_iTime / 60), (s_iTime % 60));
                }
#else    
                {
                    time_t     tTimeT = 0;  //used for filename creation
                    struct tm *ptTimeTM;    //used for filename creation
                    (void)time( &tTimeT );  //get current time
                    ptTimeTM = localtime( &tTimeT ); //build tm time struct 
                    (void)strftime( szTime, sizeof(szTime), "%Y:%m:%d %H:%M:%S", ptTimeTM );
                }
#endif
                iError |= MrvSls_ExifAddString(pExifBuffer, szTime , &iEntryBuf, &iStringBuf, &TagTable[i] );  
                usEntryCount++;
                break;
            case TAG_COPYRIGHT: // CFG-File
                (void)InifileReadString(SECTION_EXIF_IFD0, TagTable[i].strDesc, "", szBuf, BUF_LENGTH, pszFullName);
                if(szBuf[0] != 0)
                {
                   iError |= MrvSls_ExifAddString(pExifBuffer, szBuf, &iEntryBuf, &iStringBuf, &TagTable[i] );  
                   usEntryCount++;                
                }
                break;
            
            case TAG_EXIF_OFFSET: // SYSTEM
            
                iError |= MrvSls_ExifAddUlong(pExifBuffer, (iStringBuf - START_TIFF_HEADER), &iEntryBuf, &TagTable[i] );  
                // save offset to pointer to Ifd1
                iExifOffset = iEntryBuf;
                // accocate space for pointer to Ifd1
                iEntryBuf += EXIF_U32;
                usEntryCount++;                
                break;
            default:
              break;                

        }
    }
    // compare max entries with current entries and correct entrycounter for Ifd0
    if(TAG_TABLE_SIZE != usEntryCount)
    {
        Put16ui(pEntryMarker, usEntryCount); 
    }
    //-------------End Ifd0---------------------
    //-------------Start SubIfd-----------------
    // calculate number of exif entries SubIfd
    iEntryBuf = iStringBuf;
    iStringBuf = iEntryBuf + (TAG_TABLE_SIZE2 * EXIF_ENTRY_LENGTH);
    // mark pointer of exif number of entries for SubIfd
    pEntryMarker = pExifBuffer + iEntryBuf;
    // set real number counter to 0    
    usEntryCount = 0; 
    // write number of SubIfd entries to header           
    Put16ui(pExifBuffer + iEntryBuf, TAG_TABLE_SIZE2);
    iEntryBuf += EXIF_U16;
    iStringBuf += EXIF_U16;
    // Write pointer to SubIfd2 to NULL. No further sub directory records
    Put32ui(pExifBuffer + iStringBuf, 0);
    // allocate space for pointer to SubIfd2
    iStringBuf += EXIF_U32;


    // exif entries SubIfd
    for (i=0;i<TAG_TABLE_SIZE2; i++)
    {
        
        //DBG_OUT( (DINFO, "Exif = %x\n",TagTable2[i].usTag ) );
        switch(TagTable2[i].usTag)
        {
            case TAG_EXIF_VERSION: // CFG-File
                (void)InifileReadString(SECTION_EXIF_SUBIFD, TagTable2[i].strDesc, "", szBuf, BUF_LENGTH, pszFullName);
                if(szBuf[0] != 0)
                {
                    ulValue = (UINT8)szBuf[0]; 
                    ulValue = ulValue*256 + (UINT8)szBuf[1];
                    ulValue = ulValue*256 + (UINT8)szBuf[2];
                    ulValue = ulValue*256 + (UINT8)szBuf[3];                
                    iError |= MrvSls_ExifAddBinary(pExifBuffer, ulValue, &iEntryBuf, &TagTable2[i]);  
                    usEntryCount++;                
                }
                break;
            case TAG_FNUMBER: // CFG-File
                fInput = InifileReadFloat(SECTION_EXIF_SUBIFD, TagTable2[i].strDesc, 0.0f, pszFullName);
                if(fInput != 0.0f)
                {
                   iError |= MrvSls_ExifAddRational(pExifBuffer, FLOAT_RATIONAL_FACTOR, (UINT32)(fInput * FLOAT_RATIONAL_FACTOR), &iEntryBuf, &iStringBuf, &TagTable2[i]);  
                   usEntryCount++;                               
                }
                break;
            case TAG_EXPOSURETIME: // SENSOR
                // exposertime 
                fInput = tMrvAEtoSLS.fIntegrationTime * FLOAT_RATIONAL_FACTOR;
                iError |= MrvSls_ExifAddRational(pExifBuffer, FLOAT_RATIONAL_FACTOR, (UINT32)(fInput), &iEntryBuf, &iStringBuf, &TagTable2[i]);  
                usEntryCount++;
                break;
            case TAG_EXPOSURE_PROGRAM: // CFG-File
                usInput = (UINT16)InifileReadInt(SECTION_EXIF_SUBIFD, TagTable2[i].strDesc, NO_ENTRY, pszFullName);
                if(usInput != NO_ENTRY)
                {
                   iError |= MrvSls_ExifAddUshort(pExifBuffer, usInput, &iEntryBuf, &TagTable2[i]);  
                   usEntryCount++;
                }
                break;
            case TAG_COMPONENTS_CONFIG: // CFG-File
                ulValue = (UINT32)InifileReadInt(SECTION_EXIF_SUBIFD, TagTable2[i].strDesc, NO_ENTRY, pszFullName);
                if(ulValue != NO_ENTRY)
                {
                   iError |= MrvSls_ExifAddBinary(pExifBuffer, ulValue, &iEntryBuf, &TagTable2[i]);  
                   usEntryCount++;                
                }
                break;
            case TAG_LIGHT_SOURCE: // CAMERA DRIVER
                switch(ptSysConf->ptConfig->WbConfig.eMrvWbSubMode)
                {
                    case eMrvAwbAutoOn: 
                        iError |= MrvSls_ExifAddUshort(pExifBuffer, LIGHT_SOURCE_OTHER, &iEntryBuf, &TagTable2[i]);
                        break;
                    case eMrvAwbManDay: 
                        iError |= MrvSls_ExifAddUshort(pExifBuffer, LIGHT_SOURCE_DAYLIGHT, &iEntryBuf, &TagTable2[i]);
                        break;  
                    case eMrvAwbManCieA: 
                        iError |= MrvSls_ExifAddUshort(pExifBuffer, LIGHT_SOURCE_STANDARD_LIGHT_A, &iEntryBuf, &TagTable2[i]);
                        break; 
                    case eMrvAwbManFluorcnt: 
                        iError |= MrvSls_ExifAddUshort(pExifBuffer, LIGHT_SOURCE_FLUORESCENT, &iEntryBuf, &TagTable2[i]);
                        break;  
                    case eMrvAwbManTungsten: 
                        iError |= MrvSls_ExifAddUshort(pExifBuffer, LIGHT_SOURCE_TUNGSTEN, &iEntryBuf, &TagTable2[i]);
                        break; 
                    case eMrvAwbManCieD50: 
                        iError |= MrvSls_ExifAddUshort(pExifBuffer, LIGHT_SOURCE_D50, &iEntryBuf, &TagTable2[i]);
                        break; 
                    case eMrvAwbManCieD65: 
                        iError |= MrvSls_ExifAddUshort(pExifBuffer, LIGHT_SOURCE_D65, &iEntryBuf, &TagTable2[i]);
                        break; 
                    case eMrvAwbManCieD75: 
                        iError |= MrvSls_ExifAddUshort(pExifBuffer, LIGHT_SOURCE_D75, &iEntryBuf, &TagTable2[i]);
                        break; 
                    case eMrvAwbManShade:
                        iError |= MrvSls_ExifAddUshort(pExifBuffer, LIGHT_SOURCE_SHADE, &iEntryBuf, &TagTable2[i]);
                        break; 
                    case eMrvAwbManCieF2:
                        iError |= MrvSls_ExifAddUshort(pExifBuffer, LIGHT_SOURCE_COOL_WHITE_FLUORESCENT, &iEntryBuf, &TagTable2[i]);
                        break;  
                    case eMrvAwbManCloudy:
                        iError |= MrvSls_ExifAddUshort(pExifBuffer, LIGHT_SOURCE_CLOUDY_WEATHER, &iEntryBuf, &TagTable2[i]);
                        break;                      
                    case eMrvAwbManSunny:
                        iError |= MrvSls_ExifAddUshort(pExifBuffer, LIGHT_SOURCE_FINE_WEATHER, &iEntryBuf, &TagTable2[i]);
                        break;  
                    case eMrvAwbManCieF11: 
                    case eMrvAwbManCieF12: 
                    case eMrvAwbManFlash: 
                    case eMrvAwbManSunset: 
                    case eMrvAwbManTwilight:
                    case eMrvAwbManFluorcntH:
                    case eMrvAwbSubOff:
                        iError |= MrvSls_ExifAddUshort(pExifBuffer, LIGHT_SOURCE_OTHER, &iEntryBuf, &TagTable2[i]);
                        break;
                    default:
                        iError |= MrvSls_ExifAddUshort(pExifBuffer, LIGHT_SOURCE_OTHER, &iEntryBuf, &TagTable2[i]);
                        break;  
                }
                usEntryCount++;                
                break;
            case TAG_MAXAPERTURE: // CFG-File
                fInput = InifileReadFloat(SECTION_EXIF_SUBIFD, TagTable2[i].strDesc, 0.0f, pszFullName);
                if(fInput != 0.0f)
                {
                   iError |= MrvSls_ExifAddRational(pExifBuffer, FLOAT_RATIONAL_FACTOR, (UINT32)(fInput * FLOAT_RATIONAL_FACTOR), &iEntryBuf, &iStringBuf, &TagTable2[i]);  
                   usEntryCount++;                
                }
                break;
            case TAG_METERING_MODE: // CFG-File
                usInput = (UINT16)InifileReadInt(SECTION_EXIF_SUBIFD, TagTable2[i].strDesc, NO_ENTRY, pszFullName);
                if(usInput != NO_ENTRY)
                {
                   iError |= MrvSls_ExifAddUshort(pExifBuffer, usInput, &iEntryBuf, &TagTable2[i]);  
                   usEntryCount++;                
                }
                break;
            case TAG_FOCALLENGTH: // CFG-File
                fInput = InifileReadFloat(SECTION_EXIF_SUBIFD, TagTable2[i].strDesc, 0.0f, pszFullName);
                if(fInput != 0.0f)
                {                
                   iError |= MrvSls_ExifAddRational(pExifBuffer, FLOAT_RATIONAL_FACTOR, (UINT32)(fInput * FLOAT_RATIONAL_FACTOR), &iEntryBuf, &iStringBuf, &TagTable2[i]);  
                   usEntryCount++;                
                }
                break;
            case TAG_FLASH_PIX_VERSION: // CFG-File
                (void)InifileReadString(SECTION_EXIF_SUBIFD, TagTable2[i].strDesc, "", szBuf, BUF_LENGTH, pszFullName);
                if(szBuf[0] != 0)
                {                
                    ulValue = (UINT8)szBuf[0]; 
                    ulValue = ulValue*256 + (UINT8)szBuf[1];
                    ulValue = ulValue*256 + (UINT8)szBuf[2];
                    ulValue = ulValue*256 + (UINT8)szBuf[3];                
                    iError |= MrvSls_ExifAddBinary(pExifBuffer, ulValue, &iEntryBuf, &TagTable2[i]);  
                    usEntryCount++;                
                }
                break;
            case TAG_COLOR_SPACE: // CFG-File
                usInput = (UINT16)InifileReadInt(SECTION_EXIF_SUBIFD, TagTable2[i].strDesc, NO_ENTRY, pszFullName);
                if(usInput != NO_ENTRY)
                {                
                   iError |= MrvSls_ExifAddUshort(pExifBuffer, usInput, &iEntryBuf, &TagTable2[i]);  
                   usEntryCount++;                
                }
                break;
            case TAG_SENSING_METHOD: // CFG-File
                usInput = (UINT16)InifileReadInt(SECTION_EXIF_SUBIFD, TagTable2[i].strDesc, NO_ENTRY, pszFullName);
                if(usInput != NO_ENTRY)
                {                
                   iError |= MrvSls_ExifAddUshort(pExifBuffer, usInput, &iEntryBuf, &TagTable2[i]);  
                   usEntryCount++;                
                }
                break;
            case TAG_EXPOSURE_MODE: // CFG-File
                usInput = (UINT16)InifileReadInt(SECTION_EXIF_SUBIFD, TagTable2[i].strDesc, NO_ENTRY, pszFullName);
                if(usInput != NO_ENTRY)
                {                
                   iError |= MrvSls_ExifAddUshort(pExifBuffer, usInput, &iEntryBuf, &TagTable2[i]);  
                   usEntryCount++;                
                }
                break;
            case TAG_FOCALLENGTH_35MM: // CFG-File
                usInput = (UINT16)InifileReadInt(SECTION_EXIF_SUBIFD, TagTable2[i].strDesc, NO_ENTRY, pszFullName);
                if(usInput != NO_ENTRY)
                {                
                   iError |= MrvSls_ExifAddUshort(pExifBuffer, usInput, &iEntryBuf, &TagTable2[i]);  
                   usEntryCount++;                
                }
                break;
            case TAG_SCENE_CAPTURE_TYPE: // CFG-File
                usInput = (UINT16)InifileReadInt(SECTION_EXIF_SUBIFD, TagTable2[i].strDesc, NO_ENTRY, pszFullName);
                if(usInput != NO_ENTRY)
                {                
                   iError |= MrvSls_ExifAddUshort(pExifBuffer, usInput, &iEntryBuf, &TagTable2[i]);  
                   usEntryCount++;                
                }
                break;
            case TAG_GAIN_CONTROL: // CFG-File
                usInput = (UINT16)InifileReadInt(SECTION_EXIF_SUBIFD, TagTable2[i].strDesc, NO_ENTRY, pszFullName);
                if(usInput != NO_ENTRY)
                {
                   iError |= MrvSls_ExifAddUshort(pExifBuffer, usInput, &iEntryBuf, &TagTable2[i]);  
                   usEntryCount++;                
                }
                break;
            case TAG_CONTRAST: // CFG-File
                usInput = (UINT16)InifileReadInt(SECTION_EXIF_SUBIFD, TagTable2[i].strDesc, NO_ENTRY, pszFullName);
                if(usInput != NO_ENTRY)
                {                
                   iError |= MrvSls_ExifAddUshort(pExifBuffer, usInput, &iEntryBuf, &TagTable2[i]);  
                   usEntryCount++;                
                }
                break;
            case TAG_SATURATION: // CFG-File
                usInput = (UINT16)InifileReadInt(SECTION_EXIF_SUBIFD, TagTable2[i].strDesc, NO_ENTRY, pszFullName);
                if(usInput != NO_ENTRY)
                {
                   iError |= MrvSls_ExifAddUshort(pExifBuffer, usInput, &iEntryBuf, &TagTable2[i]);  
                   usEntryCount++;                
                }
                break;
            case TAG_SHARPNESS: // CFG-File
                usInput = (UINT16)InifileReadInt(SECTION_EXIF_SUBIFD, TagTable2[i].strDesc, NO_ENTRY, pszFullName);
                if(usInput != NO_ENTRY)
                {                
                   iError |= MrvSls_ExifAddUshort(pExifBuffer, usInput, &iEntryBuf, &TagTable2[i]);  
                   usEntryCount++;                
                }
                break;
            case TAG_DISTANCE_RANGE: // CFG-File
                usInput = (UINT16)InifileReadInt(SECTION_EXIF_SUBIFD, TagTable2[i].strDesc, NO_ENTRY, pszFullName);
                if(usInput != NO_ENTRY)
                {                
                   iError |= MrvSls_ExifAddUshort(pExifBuffer, usInput, &iEntryBuf, &TagTable2[i]);  
                   usEntryCount++;                
                }
                break;
            case TAG_DATETIME_ORIGINAL: // SYSTEM
                if(szTime[0])
                {
                   iError |= MrvSls_ExifAddString(pExifBuffer, szTime , &iEntryBuf, &iStringBuf, &TagTable2[i] );  
                   usEntryCount++;                
                } 
                break; 
            case TAG_DATETIME_DIGITIZED: // SYSTEM
                if(szTime[0])
                {
                   iError |= MrvSls_ExifAddString(pExifBuffer, szTime , &iEntryBuf, &iStringBuf, &TagTable2[i] );  
                   usEntryCount++;                
                } 
                break; 

            case TAG_EXIF_IMAGEWIDTH: // CAMERA DRIVER
                iError |= IsiRes2Size(ptSysConf->ptIsiConfig->ulRes, &ulWidth, &ulHeight );
                iError |= MrvSls_ExifAddUlong(pExifBuffer, ulWidth, &iEntryBuf, &TagTable2[i] ); 
                usEntryCount++;                
                break;
            case TAG_EXIF_IMAGELENGTH: // CAMERA DRIVER
                iError |= IsiRes2Size(ptSysConf->ptIsiConfig->ulRes, &ulWidth, &ulHeight );            
                iError |= MrvSls_ExifAddUlong(pExifBuffer, ulHeight, &iEntryBuf, &TagTable2[i] );  
                usEntryCount++;                
                break;
            case TAG_WHITEBALANCE: // CAMERA DRIVER
                if(ptSysConf->ptConfig->WbConfig.eMrvWbSubMode == eMrvAwbAutoOn)
                {
                    iError |= MrvSls_ExifAddUshort(pExifBuffer, AUTO_WHITE_BALANCE, &iEntryBuf, &TagTable2[i]);  
  
                }
                else
                {
                    iError |= MrvSls_ExifAddUshort(pExifBuffer, MANUAL_WHITE_BALANCE, &iEntryBuf, &TagTable2[i]);  
                }
                usEntryCount++;                
                break;
             case TAG_ISO_EQUIVALENT: // SENSOR
                // current gain value from sensor;
                fInput = tMrvAEtoSLS.fGain * FLOAT_RATIONAL_FACTOR * GAIN_TO_ISO_FACTOR;
                iError |= MrvSls_ExifAddUshort(pExifBuffer, (UINT16) fInput, &iEntryBuf, &TagTable2[i]);  
                usEntryCount++;
                break;
             case TAG_EXPOSURE_BIAS:  // CAMERA DRIVER
                  // Exposure Bias = Ln (akt. Setpoint/ Target Setpoint) / Ln(2);
                  if(ptSysConf->ptConfig->Flags.fAEC_ScEval)
                  {
                      fInput = (FLOAT)log((double)ptSysConf->ptConfig->AecCfg.rSetpointBySceneEval / TARGET_SETPOINT) / CONST_LOG2;
                  }
                  else
                  {
                      fInput = (FLOAT)log((double)ptSysConf->ptConfig->AecCfg.rSetpoint / TARGET_SETPOINT) / CONST_LOG2;
                  }
                  iError |= MrvSls_ExifAddSRational(pExifBuffer, FLOAT_RATIONAL_FACTOR, (INT32)(fInput * FLOAT_RATIONAL_FACTOR), &iEntryBuf, &iStringBuf, &TagTable2[i]);  
                  usEntryCount++;                
                break;
             default:
                break;   
        }
    }
    
    // compare max entries with current entries and correct entrycounter
    if(TAG_TABLE_SIZE2 != usEntryCount)
    {
        Put16ui(pEntryMarker, usEntryCount); 
        usEntryCount = 0; 
        // Write pointer to SubIfd2 to NULL on the corrected place. No further sub directory records
        Put32ui(pExifBuffer + iEntryBuf, 0);
    }
    //-------------End SubIfd-----------------  
    
    // read if thumbnail will be added from EXIF configuration file. If file or section not found add thumbnail.
    usInput = (UINT16)InifileReadInt(SECTION_EXIF_IFD1, KEY_THUMBNAIL, 1, pszFullName);

    if(!(ptSysConf->ptConfig->tViewFinder.ulFlags & THUMBNAIL_SKIP_MASK) && (ptSysConf->ptConfig->tViewFinder.ulFlags & FF_VFFLAG_HWRGB) && usInput)
    {

        //-------------Start Ifd1-----------------      
        // number of exif entries Ifd1
        iEntryBuf = iStringBuf;
        // add exif entries (number * 12 byte)
        iStringBuf = iEntryBuf + (TAG_TABLE_SIZE3 * EXIF_ENTRY_LENGTH);
        // write pointer to Ifd1 record to header
        Put32ui(pExifBuffer + iExifOffset, iEntryBuf - START_TIFF_HEADER);
        // write number of exif entries for Ifd1 to header           
        Put16ui(pExifBuffer + iEntryBuf, TAG_TABLE_SIZE3);
        iEntryBuf += EXIF_U16;
        iStringBuf += EXIF_U16;
        // Write pointer to Ifd2 to NULL. No further records
        Put32ui(pExifBuffer + iStringBuf, 0);
        iStringBuf += EXIF_U32;

        

        // exif entries Ifd1
        for (i=0;i<TAG_TABLE_SIZE3; i++)
        {
            
            //DBG_OUT( (DINFO, "Exif = %x\n",TagTable3[i].usTag ) );
            switch(TagTable3[i].usTag)
            {
                case TAG_IMAGE_WIDTH:
                    iError |= MrvSls_ExifAddUlong(pExifBuffer, THUMBNAIL_WIDTH, &iEntryBuf, &TagTable3[i] ); 
                    break;
                case TAG_IMAGE_LENGTH:
                    iError |= MrvSls_ExifAddUlong(pExifBuffer, THUMBNAIL_HEIGHT, &iEntryBuf, &TagTable3[i] ); 
                    break;

                case TAG_THUMBNAIL_OFFSET:
                    iError |= MrvSls_ExifAddUlong(pExifBuffer, iStringBuf - START_TIFF_HEADER, &iEntryBuf, &TagTable3[i] ); 
                    break;

                case TAG_THUMBNAIL_LENGTH:
                    iError |= MrvSls_ExifAddUlong(pExifBuffer, THUMBNAIL_SIZE + BMP_DATA_OFFSET, &iEntryBuf, &TagTable3[i] ); 
                    break;
                default:
                    break;    
            }
        }
    //-------------End Ifd1----------------- 
      
        // generation of bmp header
        iEntryBuf = iStringBuf;
        memset( (pExifBuffer + iEntryBuf), 0, BMP_DATA_OFFSET );

        *(pExifBuffer + iEntryBuf) = 'B';
        *(pExifBuffer + iEntryBuf + 1) = 'M';
        iEntryBuf += EXIF_U16;
        Put32ui(pExifBuffer + iEntryBuf, (THUMBNAIL_SIZE + BMP_DATA_OFFSET)); // size of bmp file
        iEntryBuf += EXIF_U32;
        Put32ui(pExifBuffer + iEntryBuf, 0);
        iEntryBuf += EXIF_U32;
        Put32ui(pExifBuffer + iEntryBuf, BMP_DATA_OFFSET); // 70: start of bitmap data
        iEntryBuf += EXIF_U32;
        Put32ui(pExifBuffer + iEntryBuf, BMP_HEADER_SIZE); // 40: size of bmp header
        iEntryBuf += EXIF_U32;
        Put32ui(pExifBuffer + iEntryBuf, THUMBNAIL_WIDTH); // width
        iEntryBuf += EXIF_U32;
        Put32ui(pExifBuffer + iEntryBuf, THUMBNAIL_HEIGHT); // height
        iEntryBuf += EXIF_U32;
        Put16ui(pExifBuffer + iEntryBuf, NUMBER_OF_COLOR_PLANES); // 1: number of color planes
        iEntryBuf += EXIF_U16;
        Put16ui(pExifBuffer + iEntryBuf, BITS_PER_PIXEL); // 16: bits per Pixel
        iEntryBuf += EXIF_U16;
        Put32ui(pExifBuffer + iEntryBuf, COMPRESSION_METHOD); // 3: compression method 
        iEntryBuf += EXIF_U32;
        Put32ui(pExifBuffer + iEntryBuf, THUMBNAIL_SIZE); // image size
        iEntryBuf += EXIF_U32;
        
        Put32ui(pExifBuffer + iEntryBuf, 0);
        iEntryBuf += EXIF_U32;
        Put32ui(pExifBuffer + iEntryBuf, 0);
        iEntryBuf += EXIF_U32;    
        Put32ui(pExifBuffer + iEntryBuf, 0);
        iEntryBuf += EXIF_U32;
        Put32ui(pExifBuffer + iEntryBuf, 0);
        iEntryBuf += EXIF_U32;  
        
        Put32ui(pExifBuffer + iEntryBuf, RED_PIXEL_MASK); // Red Pixel Mask: F8 00
        iEntryBuf += EXIF_U32;  
        Put32ui(pExifBuffer + iEntryBuf, GREEN_PIXEL_MASK); // Green Pixel Mask: 07 E0
        iEntryBuf += EXIF_U32;  
        Put32ui(pExifBuffer + iEntryBuf, BLUE_PIXEL_MASK);  // Blue Pixel Mask: 00 1F
        iEntryBuf += EXIF_U32;  
          
        // add binary thumbnail data
        iError |= MrvSls_ExifGenerateBmp(ptSysConf, pExifBuffer + iStringBuf + BMP_DATA_OFFSET);
        
        
        // begin of Jpeg data
        BYTE_ORDER = BIG_ENDIAN_ORDER;
        Put16ui(pExifBuffer + START_EXIF_DATA, (iStringBuf + THUMBNAIL_SIZE + BMP_DATA_OFFSET - START_EXIF_DATA));
        BYTE_ORDER = LITTLE_ENDIAN_ORDER;
        
        // return image size with thumbnail
        *pulSize = iStringBuf + THUMBNAIL_SIZE + BMP_DATA_OFFSET;
    }
    else // if(ptSysConf->ptConfig->tViewFinder.ulFlags & FF_VFFLAG_MODE__MASK)
    {
        if(((ptSysConf->ptConfig->tViewFinder.ulFlags & THUMBNAIL_SKIP_MASK) || !(ptSysConf->ptConfig->tViewFinder.ulFlags & FF_VFFLAG_HWRGB)) && usInput)
        {
            DBG_OUT( (DERR, "EXIF:thumbnail error 0x%x: Viewfinder must be in full lcd mode and without fade in windows and flipping effects\n",ptSysConf->ptConfig->tViewFinder.ulFlags) );
        }
        // write NULL pointer to Ifd1 record to header
        Put32ui(pExifBuffer + iExifOffset, 0);

         // begin of Jpeg data
        BYTE_ORDER = BIG_ENDIAN_ORDER;
        Put16ui(pExifBuffer + START_EXIF_DATA, (iStringBuf - START_EXIF_DATA));
        BYTE_ORDER = LITTLE_ENDIAN_ORDER;
        
        // return image size without thumbnail
        *pulSize = iStringBuf;

    } // if(ptSysConf->ptConfig->tViewFinder.ulFlags & FF_VFFLAG_MODE__MASK)
 
    if(pszFullName)
    {
        DBG_FREE(pszFullName);
    }
    return iError;
}


//#endif    // #ifdef SL_SUPPORT_PARTLY
#endif //if MRV_SUPPORT_SL

