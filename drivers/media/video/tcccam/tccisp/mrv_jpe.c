/*****************************************************************************/
/*!
 *  \file        mrv_jpe.c \n
 *  \version     1.0 \n
 *  \author      Friebe \n
 *  \brief       JPEG processing interface \n
 *
 *  \revision    $Revision: 524 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-09-03 19:12:46 +0200 (Do, 03 Sep 2009) $ \n
 *               $Id: mrv_jpe.c 524 2009-09-03 17:12:46Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#include "os.h"
#include "stdinc.h"

#include "reg_access.h"
#include "mrv_priv.h"
#include "mrv_jpe.h"


/*****************************************************************************
 * DEFINES
 *****************************************************************************/
// abbreviations for local debug control ( level | module )
#define DERR  ( DBG_ERR  | DBG_MRV )    //!< errors
#define DWARN ( DBG_WARN | DBG_MRV )    //!< warnings
#define DINFO ( DBG_INFO | DBG_MRV )    //!< information


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvJpeInitEx \n
 *  \RETURNVALUE RET_SUCCESS - everything ok \n
 *               RET_FAILURE - resolution not supported \n
 *  \PARAMETERS  width of the frame in pixels \n
 *               height of the frame in pixels \n
 *               one of the compression ratios defined in mrv.h \n
 *  \DESCRIPTION initialization of JPEG encoder \n
 */
/*****************************************************************************/
RESULT MrvJpeInitEx(UINT16 usHSize, UINT16 usVSize, UINT8 ucCompressionRatio, BOOL bJpeScale)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    // Reset JPEG-Encoder. In contrast to other software resets this
    // triggers the modules asynchronous reset resulting in loss of all data.
    REG_SET_SLICE(ptMrvReg->vi_ircl, MRV_VI_JPEG_SOFT_RST, ON);
    REG_SET_SLICE(ptMrvReg->vi_ircl, MRV_VI_JPEG_SOFT_RST, OFF);

    // set configuration for the Jpeg capturing
    MrvJpeSetConfig(usHSize, usVSize, bJpeScale);
    
    // Sleep a while before setting up tables because of the 400 clock cycles
    // required to initialize the table RAM after a reset was issued. On FPGA
    // we are running with only 30MHz, so at least 13us are required.
    OS_SleepUs( 15 );
    
    // program tables
    MrvJpeSetTables(ucCompressionRatio);
    
    // choose tables
    MrvJpeSelectTables(); 

    return RET_SUCCESS;       
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvJpeInit \n
 *  \RETURNVALUE RET_SUCCESS - everything ok \n
 *               RET_FAILURE - resolution not supported \n
 *  \PARAMETERS  one of the resolutions defined in isi.h \n
 *               one of the compression ratios defined in mrv.h \n
 *  \DESCRIPTION initialization of JPEG encoder \n
 */
/*****************************************************************************/
RESULT MrvJpeInit(UINT32 ulResolution, UINT8 ucCompressionRatio, BOOL bJpeScale)
{    
    UINT16 usHSize;
    UINT16 usVSize;
    
    switch (ulResolution)
    {
#ifdef _DEBUG
    case ISI_RES_TEST:
        usHSize = TEST_SIZE_H;
        usVSize = TEST_SIZE_V;
        break;
#endif // #ifdef _DEBUG
    case ISI_RES_BP1:
        usHSize = BP1_SIZE_H; //352;
        usVSize = BP1_SIZE_V; //240;
        break;
    case ISI_RES_S_AFM:
        usHSize = S_AFM_SIZE_H; //64;
        usVSize = S_AFM_SIZE_V; //32;
        break;
    case ISI_RES_M_AFM:
        usHSize = M_AFM_SIZE_H; //128;
        usVSize = M_AFM_SIZE_V; //96;
        break;
    case ISI_RES_L_AFM:
        usHSize = L_AFM_SIZE_H; //720;
        usVSize = L_AFM_SIZE_V; //480;
        break;
    case ISI_RES_QQCIF:
        usHSize = QQCIF_SIZE_H; //88;
        usVSize = QQCIF_SIZE_V; //72;
        break;
    case ISI_RES_QQVGA:
        usHSize = QQVGA_SIZE_H; //160;
        usVSize = QQVGA_SIZE_V; //120;
        break;
    case ISI_RES_QCIF:
        usHSize = QCIF_SIZE_H; //176;
        usVSize = QCIF_SIZE_V; //144;
        break;
    case ISI_RES_QVGA:
        usHSize = QVGA_SIZE_H; //320;
        usVSize = QVGA_SIZE_V; //240;
        break;
    case ISI_RES_CIF:
        usHSize = CIF_SIZE_H; //352;
        usVSize = CIF_SIZE_V; //288;
        break;
    case ISI_RES_VGA:
        usHSize = VGA_SIZE_H; //640;
        usVSize = VGA_SIZE_V; //480;
        break;
    case ISI_RES_SVGA:
        usHSize = SVGA_SIZE_H; //800;
        usVSize = SVGA_SIZE_V; //600;
        break;
    case ISI_RES_XGA:
        usHSize = XGA_SIZE_H; //1024;
        usVSize = XGA_SIZE_V; //768;
        break;
    case ISI_RES_XGA_PLUS:
        usHSize = XGA_PLUS_SIZE_H; //1280;
        usVSize = XGA_PLUS_SIZE_V; // 960;
        break;
    case ISI_RES_SXGA:
        usHSize = SXGA_SIZE_H; //1280;
        usVSize = SXGA_SIZE_V; //1024;
        break;
    case ISI_RES_UXGA:
        usHSize = UXGA_SIZE_H; //1600;
        usVSize = UXGA_SIZE_V; //1200;
        break;
    case ISI_RES_QXGA:
        usHSize = QXGA_SIZE_H; //2048;
        usVSize = QXGA_SIZE_V; //1536;
        break;
    case ISI_RES_QSXGA:
        usHSize = QSXGA_SIZE_H; //2586;
        usVSize = QSXGA_SIZE_V; //2048;
        break;
    case ISI_RES_QSXGA_PLUS:
        usHSize = QSXGA_PLUS_SIZE_H; //2600;
        usVSize = QSXGA_PLUS_SIZE_V; //2048;
        break;
    case ISI_RES_QSXGA_PLUS2:
        usHSize = QSXGA_PLUS2_SIZE_H; //2600;
        usVSize = QSXGA_PLUS2_SIZE_V; //1950;
        break;
    case ISI_RES_QSXGA_PLUS3:
        usHSize = QSXGA_PLUS3_SIZE_H; //2686;
        usVSize = QSXGA_PLUS3_SIZE_V; //2048;
        break;
    case ISI_RES_WQSXGA:
        usHSize = WQSXGA_SIZE_H; //3200
        usVSize = WQSXGA_SIZE_V; //2048
        break;
    case ISI_RES_QUXGA:
        usHSize = QUXGA_SIZE_H; //3200
        usVSize = QUXGA_SIZE_V; //2400
        break;
    case ISI_RES_WQUXGA:
        usHSize = WQUXGA_SIZE_H; //3840
        usVSize = WQUXGA_SIZE_V; //2400
        break;
    case ISI_RES_HXGA:
        usHSize = HXGA_SIZE_H; //4096
        usVSize = HXGA_SIZE_V; //3072
        break;
    default:
        DBG_OUT((DERR, "MrvJpeInit: ulResolution not supported\n"));
        return RET_NOTSUPP;
    }
    
    return MrvJpeInitEx(usHSize, usVSize, ucCompressionRatio, bJpeScale);
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvJpeSetTables \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  compression ratio to use \n
 *
 *  \NOTES       See code in user manual page 43. In general there is no
 *               need to program other AC/DC-tables than the ones proposed
 *               in the ISO/IEC 10918-1 annex K. So we just use static table
 *               data here. Thus MrvJpeSetTables and MrvJpeSelectTables do
 *               not provide a way to switch between different tables
 *               at the moment. \n
 *               After any reset the internal RAM is filled with 0xFF.
 *               This filling takes approximately 400 clock cycles. So do
 *               not start any table programming during the first 400 clock
 *               cycles after reset is deasserted. \n
 *
 *  \DESCRIPTION programs q-, AC- and DC-tables \n
 *  \NOTES       The jpg Quantization Tables were parsed by jpeg_parser from
 *               jpg images generated by Jasc PaintShopPro. \n
 */
/*****************************************************************************/
void MrvJpeSetTables(UINT8 ucCompressionRatio)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 jpe_table_data = 0; // required because auto-increment register
    
    UINT8 ucIdx, ucSize;
    const UINT8 *pYQTable = NULL;
    const UINT8 *pUVQTable = NULL;
    
    
    switch (ucCompressionRatio)
    {
    case JPEG_LOW_COMPRESSION:
        pYQTable  = YQTableLowComp1;
        pUVQTable = UVQTableLowComp1;
        break;
    case JPEG_01_PERCENT:
        pYQTable  = YQTable01PerCent;
        pUVQTable = UVQTable01PerCent;
        break;
    case JPEG_20_PERCENT:
        pYQTable  = YQTable20PerCent;
        pUVQTable = UVQTable20PerCent;
        break;
    case JPEG_30_PERCENT:
        pYQTable  = YQTable30PerCent;
        pUVQTable = UVQTable30PerCent;
        break;
    case JPEG_40_PERCENT:
        pYQTable  = YQTable40PerCent;
        pUVQTable = UVQTable40PerCent;
        break;
    case JPEG_50_PERCENT:
        pYQTable  = YQTable50PerCent;
        pUVQTable = UVQTable50PerCent;
        break;
    case JPEG_60_PERCENT:
        pYQTable  = YQTable60PerCent;
        pUVQTable = UVQTable60PerCent;
        break;
    case JPEG_70_PERCENT:
        pYQTable  = YQTable70PerCent;
        pUVQTable = UVQTable70PerCent;
        break;
    case JPEG_80_PERCENT:
        pYQTable  = YQTable80PerCent;
        pUVQTable = UVQTable80PerCent;
        break;
    case JPEG_90_PERCENT:
        pYQTable  = YQTable90PerCent;
        pUVQTable = UVQTable90PerCent;
        break;
    case JPEG_99_PERCENT:
        pYQTable  = YQTable99PerCent;
        pUVQTable = UVQTable99PerCent;
        break;
    case JPEG_HIGH_COMPRESSION:
    default:
        // in the case an unknown value is set, use JPEG_HIGH_COMPRESSION
        pYQTable  = YQTable75PerCent;
        pUVQTable = UVQTable75PerCent;
        break;
    }
    
    
    // Y q-table 0 programming
    ucSize = sizeof(YQTable75PerCent) / sizeof(YQTable75PerCent[0]); // all possible assigned tables have same size
    REG_SET_SLICE(ptMrvReg->jpe_table_id, MRV_JPE_TABLE_ID, MRV_JPE_TABLE_ID_QUANT0);
    for (ucIdx = 0; ucIdx < (ucSize - 1); ucIdx += 2)
    {
        REG_SET_SLICE(jpe_table_data, MRV_JPE_TABLE_WDATA_H, pYQTable[ucIdx]);
        REG_SET_SLICE(jpe_table_data, MRV_JPE_TABLE_WDATA_L, pYQTable[ucIdx+1]);
        REG_WRITE(ptMrvReg->jpe_table_data, jpe_table_data); // auto-increment register!
    }
    
    // U/V q-table 0 programming
    ucSize = sizeof(UVQTable75PerCent) / sizeof(UVQTable75PerCent[0]); // all possible assigned tables have same size
    REG_SET_SLICE(ptMrvReg->jpe_table_id, MRV_JPE_TABLE_ID, MRV_JPE_TABLE_ID_QUANT1);
    for (ucIdx = 0; ucIdx < (ucSize - 1); ucIdx += 2)
    {
        REG_SET_SLICE(jpe_table_data, MRV_JPE_TABLE_WDATA_H, pUVQTable[ucIdx]);
        REG_SET_SLICE(jpe_table_data, MRV_JPE_TABLE_WDATA_L, pUVQTable[ucIdx+1]);
        REG_WRITE(ptMrvReg->jpe_table_data, jpe_table_data); // auto-increment register!
    }
    
    // Y AC-table 0 programming
    ucSize = sizeof(ACLumaTableAnnexK) / sizeof(ACLumaTableAnnexK[0]);
    REG_SET_SLICE(ptMrvReg->jpe_table_id, MRV_JPE_TABLE_ID, MRV_JPE_TABLE_ID_VLC_AC0);
    REG_SET_SLICE(ptMrvReg->jpe_tac0_len, MRV_JPE_TAC0_LEN, ucSize);
    for (ucIdx = 0; ucIdx < (ucSize - 1); ucIdx += 2)
    {
        REG_SET_SLICE(jpe_table_data, MRV_JPE_TABLE_WDATA_H, ACLumaTableAnnexK[ucIdx]);
        REG_SET_SLICE(jpe_table_data, MRV_JPE_TABLE_WDATA_L, ACLumaTableAnnexK[ucIdx+1]);
        REG_WRITE(ptMrvReg->jpe_table_data, jpe_table_data); // auto-increment register!
    }
    
    // U/V AC-table 1 programming
    ucSize = sizeof(ACChromaTableAnnexK) / sizeof(ACChromaTableAnnexK[0]);
    REG_SET_SLICE(ptMrvReg->jpe_table_id, MRV_JPE_TABLE_ID, MRV_JPE_TABLE_ID_VLC_AC1);
    REG_SET_SLICE(ptMrvReg->jpe_tac1_len, MRV_JPE_TAC1_LEN, ucSize);
    for (ucIdx = 0; ucIdx < (ucSize - 1); ucIdx += 2)
    {
        REG_SET_SLICE(jpe_table_data, MRV_JPE_TABLE_WDATA_H, ACChromaTableAnnexK[ucIdx]);
        REG_SET_SLICE(jpe_table_data, MRV_JPE_TABLE_WDATA_L, ACChromaTableAnnexK[ucIdx+1]);
        REG_WRITE(ptMrvReg->jpe_table_data, jpe_table_data); // auto-increment register!
    }
    
    // Y DC-table 0 programming
    ucSize = sizeof(DCLumaTableAnnexK) / sizeof(DCLumaTableAnnexK[0]);
    REG_SET_SLICE(ptMrvReg->jpe_table_id, MRV_JPE_TABLE_ID, MRV_JPE_TABLE_ID_VLC_DC0);
    REG_SET_SLICE(ptMrvReg->jpe_tdc0_len, MRV_JPE_TDC0_LEN, ucSize);
    for (ucIdx = 0; ucIdx < (ucSize - 1); ucIdx += 2)
    {
        REG_SET_SLICE(jpe_table_data, MRV_JPE_TABLE_WDATA_H, DCLumaTableAnnexK[ucIdx]);
        REG_SET_SLICE(jpe_table_data, MRV_JPE_TABLE_WDATA_L, DCLumaTableAnnexK[ucIdx+1]);
        REG_WRITE(ptMrvReg->jpe_table_data, jpe_table_data); // auto-increment register!
    }
    
    // U/V DC-table 1 programming
    ucSize = sizeof(DCChromaTableAnnexK) / sizeof(DCChromaTableAnnexK[0]);
    REG_SET_SLICE(ptMrvReg->jpe_table_id, MRV_JPE_TABLE_ID, MRV_JPE_TABLE_ID_VLC_DC1);
    REG_SET_SLICE(ptMrvReg->jpe_tdc1_len, MRV_JPE_TDC1_LEN, ucSize);
    for (ucIdx = 0; ucIdx < (ucSize - 1); ucIdx += 2)
    {
        REG_SET_SLICE(jpe_table_data, MRV_JPE_TABLE_WDATA_H, DCChromaTableAnnexK[ucIdx]);
        REG_SET_SLICE(jpe_table_data, MRV_JPE_TABLE_WDATA_L, DCChromaTableAnnexK[ucIdx+1]);
        REG_WRITE(ptMrvReg->jpe_table_data, jpe_table_data); // auto-increment register!
    }
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvJpeSelectTables \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  none \n
 *  \DESCRIPTION selects tables to be used by encoder \n
 */
/*****************************************************************************/
void MrvJpeSelectTables(void)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    
    REG_SET_SLICE(ptMrvReg->jpe_tq_y_select, MRV_JPE_TQ0_SELECT, MRV_JPE_TQ_SELECT_TAB0); // selects quantization table for Y
    REG_SET_SLICE(ptMrvReg->jpe_tq_u_select, MRV_JPE_TQ1_SELECT, MRV_JPE_TQ_SELECT_TAB1); // selects quantization table for U
    REG_SET_SLICE(ptMrvReg->jpe_tq_v_select, MRV_JPE_TQ2_SELECT, MRV_JPE_TQ_SELECT_TAB1); // selects quantization table for V
    REG_SET_SLICE(ptMrvReg->jpe_dc_table_select, MRV_JPE_DC_TABLE_SELECT_Y, 0);           // selects Huffman DC table
    REG_SET_SLICE(ptMrvReg->jpe_dc_table_select, MRV_JPE_DC_TABLE_SELECT_U, 1);
    REG_SET_SLICE(ptMrvReg->jpe_dc_table_select, MRV_JPE_DC_TABLE_SELECT_V, 1);
    REG_SET_SLICE(ptMrvReg->jpe_ac_table_select, MRV_JPE_AC_TABLE_SELECT_Y, 0);           // selects Huffman AC table
    REG_SET_SLICE(ptMrvReg->jpe_ac_table_select, MRV_JPE_AC_TABLE_SELECT_U, 1);
    REG_SET_SLICE(ptMrvReg->jpe_ac_table_select, MRV_JPE_AC_TABLE_SELECT_V, 1);
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvJpeSetConfig \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  usHSize and usVSize of the picture \n
 *
 *  \NOTES       usHSize and usVSize must exactly match the incoming image,
 *               otherwise the error event pic_size_err is thrown. \n
 *
 *  \DESCRIPTION configure JPEG encoder \n
 */
/*****************************************************************************/
void MrvJpeSetConfig(UINT16 usHSize, UINT16 usVSize, BOOL bJpeScale)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    
    // JPEG image size
    REG_SET_SLICE(ptMrvReg->jpe_enc_hsize, MRV_JPE_ENC_HSIZE, usHSize);
    REG_SET_SLICE(ptMrvReg->jpe_enc_vsize, MRV_JPE_ENC_VSIZE, usVSize);
    
    if(bJpeScale)
    {
        // upscaling of BT601 color space to full range 0..255
        REG_SET_SLICE(ptMrvReg->jpe_y_scale_en,    MRV_JPE_Y_SCALE_EN,    ENABLE);
        REG_SET_SLICE(ptMrvReg->jpe_cbcr_scale_en, MRV_JPE_CBCR_SCALE_EN, ENABLE);   	
    }
    else
    {
        // bypass scaler
        REG_SET_SLICE(ptMrvReg->jpe_y_scale_en,    MRV_JPE_Y_SCALE_EN,    DISABLE);
        REG_SET_SLICE(ptMrvReg->jpe_cbcr_scale_en, MRV_JPE_CBCR_SCALE_EN, DISABLE);	
    }
    
    
    // picture format YUV 422
    REG_SET_SLICE(ptMrvReg->jpe_pic_format, MRV_JPE_ENC_PIC_FORMAT, MRV_JPE_ENC_PIC_FORMAT_422);
    
    // set to normal operation (wait for encoded image data to fill output buffer)
    REG_SET_SLICE(ptMrvReg->jpe_table_flush, MRV_JPE_TABLE_FLUSH, 0);
}



/*****************************************************************************/
/*!
 *  \FUNCTION    MrvJpeGenerateHeader \n
 *  \RETURNVALUE RET_SUCCESS     - everything ok \n
 *               RET_FAILURE     - timeout during wait for header done \n
 *  \PARAMETERS  MRV_HEADER_JFIF - generate JFIF-header \n
 *               MRV_HEADER_NO   - no APPn header \n
 *  \DESCRIPTION generates JPEG header \n
 */
/*****************************************************************************/
RESULT MrvJpeGenerateHeader(UINT8 ucHeaderMode)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    
    ASSERT((ucHeaderMode == MRV_JPE_HEADER_MODE_JFIF) || (ucHeaderMode == MRV_JPE_HEADER_MODE_NO));
    
    // clear jpeg gen_header_done interrupt
    // since we poll them later to detect command completion
    REG_SET_SLICE(ptMrvReg->jpe_status_icr, MRV_JPE_GEN_HEADER_DONE, 1);
    
    REG_SET_SLICE(ptMrvReg->jpe_header_mode, MRV_JPE_HEADER_MODE, ucHeaderMode);
    REG_SET_SLICE(ptMrvReg->jpe_gen_header,  MRV_JPE_GEN_HEADER,  ON); //start header generation
    
    return MrvJpeWaitForHeaderGenDone();
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvJpePrepEnc \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  teMrvJpeEncMode - selects the kind of motion JPEG or not \n
 *  \DESCRIPTION prepare the JPEG encoder for input streaming \n
 */
/*****************************************************************************/
void MrvJpePrepEnc(teMrvJpeEncMode tMrvJpeEncMode)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 jpe_encode = REG_READ(ptMrvReg->jpe_encode);
#if (MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V2)        
    UINT32 jpe_config = REG_READ(ptMrvReg->jpe_config);
#endif //(MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V2)        
    
    // clear jpeg encode_done interrupt
    // since we poll them later to detect command completion
    REG_SET_SLICE(ptMrvReg->jpe_status_icr, MRV_JPE_ENCODE_DONE, 1);
    
    REG_SET_SLICE(jpe_encode, MRV_JPE_ENCODE, ON);
#if (MARVIN_FEATURE_JPE == MARVIN_FEATURE_JPE_V3)
    switch (tMrvJpeEncMode)
    {
#if(MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V2)        
     case eMrvJpe_ScaladoMode:
        // single snapshot with Scalado encoding
        REG_SET_SLICE(jpe_config, MRV_JPE_SPEEDVIEW, MRV_JPE_SPEEDVIEW_ENA);
        REG_SET_SLICE(jpe_config, MRV_JPE_CONT_MODE, MRV_JPE_CONT_MODE_STOP);
        break;
#endif //(MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V2)        
     case eMrvJpe_LargeContMode:
        // motion JPEG with header generation
#if(MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V1)        
        REG_SET_SLICE(jpe_encode, MRV_JPE_CONT_MODE, MRV_JPE_CONT_MODE_HEADER);
#else //(MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V1)        
        REG_SET_SLICE(jpe_config, MRV_JPE_CONT_MODE, MRV_JPE_CONT_MODE_HEADER);
        REG_SET_SLICE(jpe_config, MRV_JPE_SPEEDVIEW, MRV_JPE_SPEEDVIEW_DIS);
#endif //(MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V1)        
        break;
     case eMrvJpe_ShortContMode:
        // motion JPEG only first frame with header
#if(MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V1)        
        REG_SET_SLICE(jpe_encode, MRV_JPE_CONT_MODE, MRV_JPE_CONT_MODE_NEXT);
#else //(MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V1)        
        REG_SET_SLICE(jpe_config, MRV_JPE_CONT_MODE, MRV_JPE_CONT_MODE_NEXT);
        REG_SET_SLICE(jpe_config, MRV_JPE_SPEEDVIEW, MRV_JPE_SPEEDVIEW_DIS);
#endif //(MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V1)        
        break;
     default:
        // single shot JPEG
#if(MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V1)        
        REG_SET_SLICE(jpe_encode, MRV_JPE_CONT_MODE, MRV_JPE_CONT_MODE_STOP);
#else //(MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V1)        
        REG_SET_SLICE(jpe_config, MRV_JPE_CONT_MODE, MRV_JPE_CONT_MODE_STOP);
        REG_SET_SLICE(jpe_config, MRV_JPE_SPEEDVIEW, MRV_JPE_SPEEDVIEW_DIS);
#endif //(MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V1)        
        break;
    }
#else // #if (MARVIN_FEATURE_JPE == MARVIN_FEATURE_JPE_V3)
    UNUSED_PARAM(tMrvJpeEncMode);
#endif // #if (MARVIN_FEATURE_JPE == MARVIN_FEATURE_JPE_V3)

#if (MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V2)        
   REG_WRITE(ptMrvReg->jpe_config, jpe_config);
#endif //(MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V2)        
   REG_WRITE(ptMrvReg->jpe_encode, jpe_encode);

//#if (MARVIN_FEATURE_JPE == MARVIN_FEATURE_JPE_V2)
#if ((MARVIN_FEATURE_JPE >= MARVIN_FEATURE_JPE_V2) && (MARVIN_FEATURE_JPE <= MARVIN_FEATURE_JPE_V3))
    REG_SET_SLICE(ptMrvReg->jpe_init, MRV_JPE_JP_INIT, 1);
#endif
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvJpeWaitForHeaderGenDone \n
 *  \RETURNVALUE RET_SUCCESS - everything ok \n
 *               RET_FAILURE - timeout during wait for table end \n
 *  \PARAMETERS  none \n
 *
 *  \NOTES        \n
 *
 *  \DESCRIPTION wait until JPG Header is generated (MRV_JPGINT_GEN_HEADER_DONE
 *               interrupt occurs) \n
 *               waiting for JPG Header to be generated \n
 */
/*****************************************************************************/
RESULT MrvJpeWaitForHeaderGenDone(void)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
 	UINT32 ulStartTick = OS_GetTick();
	UINT32 ulTickFreq  = OS_GetFrequency();
   
    
    while (!REG_GET_SLICE(ptMrvReg->jpe_status_ris, MRV_JPE_GEN_HEADER_DONE))
    {
        //timeout 2 seconds, works also if OS_GetTick overflows (don't change types, first subtraction)
        if( ((OS_GetTick() - ulStartTick) / ulTickFreq) > 2)
        {
            DBG_OUT((DERR,  "MrvJpeWaitForHeaderGenDone: timeout\n"));
            DBG_OUT((DINFO, "MrvJpeWaitForHeaderGenDone: Register ISP_RIS        = 0x%08X\n", ptMrvReg->isp_ris));
            DBG_OUT((DINFO, "MrvJpeWaitForHeaderGenDone: Register MI_RIS         = 0x%08X\n", ptMrvReg->mi_ris));
            DBG_OUT((DINFO, "MrvJpeWaitForHeaderGenDone: Register JPE_DEBUG      = 0x%08X\n", ptMrvReg->jpe_debug));
            DBG_OUT((DINFO, "MrvJpeWaitForHeaderGenDone: Register JPE_ERROR_RIS  = 0x%08X\n", ptMrvReg->jpe_error_ris));
            DBG_OUT((DINFO, "MrvJpeWaitForHeaderGenDone: Register JPE_STATUS_RIS = 0x%08X\n", ptMrvReg->jpe_status_ris));
            return RET_FAILURE;
        }
    }

     
/*
    DBG_OUT( (DINFO, "MrvJpeWaitForHeaderGenDone: Register ISP_RIS        = 0x%08X\n", ptMrvReg->isp_ris) );
    DBG_OUT( (DINFO, "MrvJpeWaitForHeaderGenDone: Register MI_RIS         = 0x%08X\n", ptMrvReg->mi_ris) );
    DBG_OUT( (DINFO, "MrvJpeWaitForHeaderGenDone: Register JPE_DEBUG      = 0x%08X\n", ptMrvReg->jpe_debug) );
    DBG_OUT( (DINFO, "MrvJpeWaitForHeaderGenDone: Register JPE_ERROR_RIS  = 0x%08X\n", ptMrvReg->jpe_error_ris) );
    DBG_OUT( (DINFO, "MrvJpeWaitForHeaderGenDone: Register JPE_STATUS_RIS = 0x%08X\n", ptMrvReg->jpe_status_ris) );
*/

    return RET_SUCCESS;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvJpeWaitForEncodeDone \n
 *  \RETURNVALUE RET_SUCCESS - everything ok \n
 *               RET_FAILURE - timeout during wait for table end \n
 *  \PARAMETERS  none \n
 *
 *  \NOTES        \n
 *
 *  \DESCRIPTION wait until JPG Encoder is done  (MRV_JPGINT_ENCODE_DONE
 *               interrupt occurs) \n
 *               waiting for the JPG Encoder to be done \n
 */
/*****************************************************************************/
RESULT MrvJpeWaitForEncodeDone(void)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    //int i,a;
	UINT32 ulStartTick = OS_GetTick();
	UINT32 ulTickFreq  = OS_GetFrequency();
    
    
    while (!REG_GET_SLICE(ptMrvReg->jpe_status_ris, MRV_JPE_ENCODE_DONE))
    {
        //timeout 20 seconds, works also if OS_GetTick overflows (don't change types, first subtraction)
        if( ((OS_GetTick() - ulStartTick) / ulTickFreq) > 20)
            {
                DBG_OUT( (DERR,  "MrvJpeWaitForEncodeDone: timeout\n") );
                DBG_OUT( (DINFO, "MrvJpeWaitForEncodeDone: Register ISP_RIS        = 0x%08X\n", ptMrvReg->isp_ris) );
                DBG_OUT( (DINFO, "MrvJpeWaitForEncodeDone: Register MI_RIS         = 0x%08X\n", ptMrvReg->mi_ris) );
                DBG_OUT( (DINFO, "MrvJpeWaitForEncodeDone: Register JPE_DEBUG      = 0x%08X\n", ptMrvReg->jpe_debug) );
                DBG_OUT( (DINFO, "MrvJpeWaitForEncodeDone: Register JPE_ERROR_RIS  = 0x%08X\n", ptMrvReg->jpe_error_ris) );
                DBG_OUT( (DINFO, "MrvJpeWaitForEncodeDone: Register JPE_STATUS_RIS = 0x%08X\n", ptMrvReg->jpe_status_ris) );
                return RET_FAILURE;
            }
    }


    // clear jpeg encode_done interrupt
    // since we poll them later to detect command completion
    // the clearing is needed for motion JPEG
    REG_SET_SLICE(ptMrvReg->jpe_status_icr, MRV_JPE_ENCODE_DONE, 1);

#if (MARVIN_FEATURE_ISP_ENABLE == MARVIN_FEATURE_EXIST)
    // set isp_enable to zero if isp status is off
    if(!REG_GET_SLICE(ptMrvReg->isp_flags_shd, MRV_ISP_ISP_ON_SHD ))
    {
    	REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_ENABLE, OFF)
    }
#endif

/*
    DBG_OUT( (DINFO, "MrvJpeWaitForEncodeDone:    Register ISP_RIS        = 0x%08X\n", ptMrvReg->isp_ris) );
    DBG_OUT( (DINFO, "MrvJpeWaitForEncodeDone:    Register MI_RIS         = 0x%08X\n", ptMrvReg->mi_ris) );
    DBG_OUT( (DINFO, "MrvJpeWaitForEncodeDone:    Register JPE_DEBUG      = 0x%08X\n", ptMrvReg->jpe_debug) );
    DBG_OUT( (DINFO, "MrvJpeWaitForEncodeDone:    Register JPE_ERROR_RIS  = 0x%08X\n", ptMrvReg->jpe_error_ris) );
    DBG_OUT( (DINFO, "MrvJpeWaitForEncodeDone:    Register JPE_STATUS_RIS = 0x%08X\n", ptMrvReg->jpe_status_ris) );
*/

    return RET_SUCCESS;
}

