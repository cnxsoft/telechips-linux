/*****************************************************************************/
/*!
 *  \file        mrv_sls_snap.c \n
 *  \version     1.0 \n
 *  \author      Vogt \n
 *  \brief       Implementation for second level support routines
 *               to support still picture snapshots.  \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_sls_snap.c 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#include "stdinc.h"

#ifdef MRV_SUPPORT_SL

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
// abbreviations for local debug control ( level | module )
#define DERR  ( DBG_ERR  | DBG_MRV )    //!< errors
#define DWARN ( DBG_WARN | DBG_MRV )    //!< warnings
#define DINFO ( DBG_INFO | DBG_MRV )    //!< information


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSls_DoSnapshot \n
 *  \RETURNVALUE RET_SUCCESS in case of success \n
 *  \PARAMETERS  ptSysConf .. pointer to the system configuration \n
 *               ptMain .. pointer to the datapath description for the 
 *                        main datapath. The MRV_DPD_MODE_xxx bits of 
 *                        the ulFlags-field are used to control wether
 *                        a YUV, RAW or JPEG snapshot is to be taken.  \n
 *               ucJpegCompression .. compression ratio to be used for
 *                        JPEG snapshots (one of the JPEG_ \#defines 
 *                        defined in mrv.h). This parameter is ignored
 *                        for RAW and YUV snapshots. \n
 *               iZoom .. digital zoom factor to be applied. The zoom
 *                        factor N can be calculated as follows: \n
 *                             N = 1 + (iZoom / 1024) \n
 *                        So a value of iZoom=0 result in a factor of 1, 
 *                        which means 'no zoom'.  \n
 *               ptMrvMiPathConf .. specifies the buffers to store the snapshot
 *                        data. For RAW and JPEG snapshots, only the tY 
 *                        field is of interest, tU and tV will only be 
 *                        be used for YUV snapshots. The ulOffs field of
 *                        the sub-descriptos will be incremented for the
 *                        amount of data written to the buffer.  \n
 *                        
 *  \DESCRIPTION configures main datapath, scaler and memory interface for
 *  a still image snapshot. Initiates the snapshot and wait until the data
 *  is written to the memory.  \n
 *  \n
 *  Following marvin subsystems are programmed: \n
 *     - ISP output formatter \n
 *     - ISP input aquisition (for RAW snapshots only) \n
 *     - YC-Splitter \n
 *     - Main path multiplexer \n
 *     - Main resizer \n
 *     - JPEG encode subsystem (quantisation tables etc.) \n
 *     - Memory Interface (MI) input source, en/disable and dataformat \n
 *     - Memory Interface (MI) output buffer addresses and sizes \n
 *     - Self path is being disabled, but not reconfigured. \n
 *  Following marvin subsystems are *NOT* programmed: \n
 *     - All ISP functionality but the output formatter and the 
 *       input aquisition in case of a non-RAW-snapshot \n
 *     - Color Processing block \n
 *  \n
 *  Please note that the (re-)programming of then marvin is _not_ reverted
 *  back to the previous settings after the snapshot. it#s up to the caller
 *  to re-initialize e.g. viewfinding mode after the snapshot if this is 
 *  the desired behaviour. \n
 */
/*****************************************************************************/
RESULT MrvSls_DoSnapshot(const tsPlSystemConfig *ptSysConf,
                         const tsMrvDatapathDesc *ptMain,
                         int iZoom, 
                         UINT8 ucJpegCompression, 
                         tsMrvMiPathConf* ptMrvMiPathConf)
{
   RESULT Result;
   UINT32 ulDpMode;
   UINT16 usVSize;
   UINT16 usHSize;
   
   ulDpMode = ptMain->ulFlags & MRV_DPD_MODE__MASK;
   Result = RET_SUCCESS;
   
   // get us the desired size of the snapshot
   if (ptMain->ulFlags & MRV_DPD_NORESIZE) {
      (void)IsiRes2Size(ptSysConf->ptIsiConfig->ulRes, &usHSize, &usVSize);
   } else {
      usHSize = ptMain->usOutW;
      usVSize = ptMain->usOutH;
   }
   
   // For RAW snapshots, we have to bypass the ISP too. This is done by 
   // temporal re-programming of the input aquisition
   if ((ulDpMode == MRV_DPD_MODE_ISPRAW)||(ulDpMode == MRV_DPD_MODE_ISPRAW_16B))
   {
      tsIsiSensorConfig tIsiConf;
      tIsiConf = *(ptSysConf->ptIsiConfig);
      tIsiConf.ulMode = ISI_MODE_PICT;
      Result = MrvIspSetInputAquisition(&tIsiConf);
      if (Result != RET_SUCCESS) {
         DBG_OUT( (DERR, "ERR: MrvSls_DoSnapshot: failed to re-init ISP input aquisition\n") );
      }
   }
      
   // setup datapathes
   if (Result == RET_SUCCESS)
   {
      Result = MrvSls_DatapathIsp(ptSysConf, ptMain, NULL, iZoom);   
      if (Result != RET_SUCCESS) {
         DBG_OUT( (DERR, "ERR: MrvSls_DoSnapshot: failed to init/setup main datapath\n") );
      }
   }
   
   // init JPEG subsystem for JPEG snapshots
   if ((Result == RET_SUCCESS) &&
       (ulDpMode == MRV_DPD_MODE_ISPJPEG))
   {    
      BOOL bJpeScale; 
      if(ptSysConf->ptConfig->Flags.fYCbCrFullRange)
      {
          bJpeScale = FALSE; //upscale from BT601 to Level256 not needed
      }
      else
      {
          bJpeScale = TRUE;
      }

      Result = MrvJpeInitEx(usHSize, usVSize, ucJpegCompression, bJpeScale);
      if (Result != RET_SUCCESS) {
         DBG_OUT( (DERR, "ERR: MrvSls_DoSnapshot: failed to init/setup JPEG encode subsystem\n") );
      }      
   }

   // configure MI output ant take the snapshot
   if (Result == RET_SUCCESS)
   {  // the driver does not allow us to configure the Y-path only (for non YUV snapshots)
      (void)MrvMif_SetMainBuffer( ptMrvMiPathConf, eMrvCfgUpdateImmediate );
      
      if (ulDpMode == MRV_DPD_MODE_ISPJPEG)
      {
         if (MrvSls_JpeCapture(eMrvCfgUpdateFrameSync) == 0) {
            DBG_OUT( (DERR, "ERR: MrvSls_DoSnapshot: failure during JPEG encode\n") );
            Result = RET_FAILURE;
         }
      }
      else
      {
         MrvResetInterruptStatus();
         MrvStart(1, eMrvCfgUpdateFrameSync); //start Marvin for 1 frame to capture
         Result = MrvWaitForFrameEnd();
         if (Result != RET_SUCCESS) {
            DBG_OUT( (DERR, "ERR: MrvSls_DoSnapshot: failure while waiting for frame end\n") );
         }      
      }
   }

   // get offset counters from MI and update ptMrvMiPathConf
   if (Result == RET_SUCCESS)
   {
      if (ulDpMode == MRV_DPD_MODE_ISPYC)
      {  // [vogt] the following does not work. investigations were underway to understand
         // if this is a bug or not. As a workaound, we just calculate the sizes of the buffers.
         //MrvMif_GetMainBuffer( &(ptMrvMiPathConf->tYBuffer.ulOffs), &(ptMrvMiPathConf->tCbBuffer.ulOffs), &(ptMrvMiPathConf->tCrBuffer.ulOffs) );         
         //////
         // workaround, try to figure out color format by the subsampling settings. 
         // If no subsampling takes place, assume YUV 4:2:2 output
         ptMrvMiPathConf->tYBuffer.ulOffs = (UINT32)(usHSize) * (UINT32)(usVSize);
         ptMrvMiPathConf->tCbBuffer.ulOffs = ptMrvMiPathConf->tYBuffer.ulOffs / 2;
         
         switch (ptMain->ulFlags & MRV_DPD_CSS_H__MASK)
         {
            case MRV_DPD_CSS_H2: ptMrvMiPathConf->tCbBuffer.ulOffs /= 2; break;
            case MRV_DPD_CSS_H4: ptMrvMiPathConf->tCbBuffer.ulOffs /= 4; break;
            default:             ptMrvMiPathConf->tCbBuffer.ulOffs += 0; break;
         }
         switch (ptMain->ulFlags & MRV_DPD_CSS_V__MASK)
         {
            case MRV_DPD_CSS_V2: ptMrvMiPathConf->tCbBuffer.ulOffs /= 2; break;
            case MRV_DPD_CSS_V4: ptMrvMiPathConf->tCbBuffer.ulOffs /= 4; break;
            default:             ptMrvMiPathConf->tCbBuffer.ulOffs += 0; break;
         }
         ptMrvMiPathConf->tCrBuffer.ulOffs = ptMrvMiPathConf->tCbBuffer.ulOffs;
         // /workaround
         //////
      }
      else
      {  // for RAW and JPEG snapshots, the MI seems not to update the offset registers. Instead, 
         // the byte count register is to be used
         ptMrvMiPathConf->tYBuffer.ulOffs = MrvMifGetByteCnt();
      }
   }

   return Result;
}

#endif //#ifdef MRV_SUPPORT_SL
