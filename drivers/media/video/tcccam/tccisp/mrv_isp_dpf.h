/*!****************************************************************************
 *  <!-- DOXYGEN prepared -->
 *  Copyright(c)2009 Silicon Image GmbH
 *
 *  \file        mrv_isp_dpf.h \n
 *  \version     1.0 \n
 *  \author      Schueler \n
 *  \brief       <!-- DESCRIPTION --> Defect Pixel Cluster Correction driver, public header\n
 *
 *  \version <!-- HISTORY -->
 *  build  | date       | author     | description\n
 *  1.0.00   14.08.2009   Schueler     created\n
 *  1.0.01   18.08.2009   Schueler     added tsMrvDpfInvStrength\n
 *  1.0.02   18.08.2009   Schueler     added bDpfEnable\n
 *  1.0.03   21.08.2009   Schueler     remove tsMrvSlsDpfNLLElem\n
 *  1.0.04   09.09.2009   Schueler     remove boolean variables\n
 *  1.0.05   10.09.2009   Schueler     rename bMrvDpfNllSeg to ulMrvDpfNllSeg\n
 *  1.0.06   01.10.2009   Frommelt     moved structure tsMrvDpfNLLElem to mrv.h.
 *                                     renamed tsMrvDpfConfig to tsMrvDpfCfg.\n
 *****************************************************************************/

#ifndef _MRV_ISP_DPF_H
#define _MRV_ISP_DPF_H

#if( MARVIN_FEATURE_DPF == MARVIN_FEATURE_DPF_V1 )


/********************************
 * public variables
 ********************************/
//[DPF_CONFIG]

//[DPF_SPATIAL]
typedef struct
{
    UINT8 ucWeightCoeff1;
    UINT8 ucWeightCoeff2;
    UINT8 ucWeightCoeff3;
    UINT8 ucWeightCoeff4;
    UINT8 ucWeightCoeff5;
    UINT8 ucWeightCoeff6;
}tsDpfSpatial;

//[DPF_MODE]
typedef struct
{
    UINT32 ulDpfNlGainEnable;
    UINT32 ulDpfLscGainComp;
    UINT32 ulDpfAwbGainComp;
    UINT32 ulRBFilterSize;
    UINT32 ulRFilterOff;
    UINT32 ulGRFilterOff;
    UINT32 ulGBFilterOff;
    UINT32 ulBFilterOff;
    UINT32 ulDpfEnable;    
}tsDpfMode;

typedef struct
{
    tsDpfMode           tDpfMode;         
    tsDpfSpatial        tDpfSpatialG;   
    tsDpfSpatial        tDpfSpatialRB;      
}tsMrvDpfCfg;

typedef struct
{
    UINT8 ucInvWeightR;
    UINT8 ucInvWeightG;
    UINT8 ucInvWeightB;
}tsMrvDpfInvStrength;

/********************************
 * public functions
 ********************************/
RESULT MrvIspDpf_Enable( void );
RESULT MrvIspDpf_Disable( void );
RESULT MrvIspDpf_SetConfig (const tsMrvDpfCfg *ptMrvDpfCfg );
RESULT MrvIspDpf_GetConfig ( tsMrvDpfCfg *ptMrvDpfCfg );
RESULT MrvIspDpf_SetNoiseLevel (const tsMrvDpfNLLElem *ptMrvDpfNLLElem);
RESULT MrvIspDpf_GetNoiseLevel ( tsMrvDpfNLLElem *ptMrvDpfNLLElem);
RESULT MrvIspDpf_SetNLGain (const tsMrvDpfNLGain *ptMrvDpfNLGain );
RESULT MrvIspDpf_GetNLGain ( tsMrvDpfNLGain *ptMrvDpfNLGain );
RESULT MrvIspDpf_SetStrength (const tsMrvDpfInvStrength *ptMrvDpfInvStrength);
RESULT MrvIspDpf_GetStrength (tsMrvDpfInvStrength *ptMrvDpfInvStrength);

#elif ( MARVIN_FEATURE_DPF != MARVIN_FEATURE_EXIST_NOT )
#error unknown value for MARVIN_FEATURE_DPF
#endif  // MARVIN_FEATURE_DPF

#endif // _MRV_ISP_DPF_H

