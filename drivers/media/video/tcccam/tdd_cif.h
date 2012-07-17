/************************************************************************
*    Telechips Multi Media Player
*    ------------------------------------------------
*
*    FUNCTION    : CAMERA INTERFACE API
*    MODEL        : DMP
*    CPU NAME    : TCCXXX
*    SOURCE        : tdd_cif.h
*
*    START DATE    : 2008. 4. 17.
*    MODIFY DATE :
*    DEVISION    : DEPT. SYSTEM 3-2 TEAM
*                : TELECHIPS, INC.
************************************************************************/
#ifndef _TDD_CIF_H_ 
#define _TDD_CIF_H_

extern void TCC_CIF_CLOCK_Get(void);
extern void TCC_CIF_CLOCK_Put(void);
extern void TDD_CIF_Initialize(void);
//extern void TDD_CIF_Reset(void);
extern void TDD_CIF_ONOFF(unsigned int uiOnOff);
extern void TDD_CIF_Termination(void);


/****************************************************************************
*
*    Function of    
*
*    DDI_CIF_SetInfo
*
*    Input    :     uiFlag
*                uiBypass             : SEPARATE / NON_SEPARATE
*                uiBypassBusSel    : FIRST_MSB / FIRST LSB (by 16bit mode)
*                uiColorPattern        : YUV / RGB
*                uiPatternFormat    : YUV444 / YUV422 / YUV420 or RGB mode
*                uiRGBMode        : In RGB mode BAYER RGB, RGB565, RGB555 mode
*                uiRGBBitMode        : Data bus bit 16bit mode, 8bit (enable sync), 8bit (disable sync)
*                uiColorSequence    : Color sequence..
*                uiBusOrder        : Swap the MSB/LSB data bus
*                uiOverlayCNT        : Overlay Count
*    Output    : 
*    Return    :     none
*
*    Description :  Input Image Color/Pattern Configuration (ICPCR1)
*****************************************************************************/
extern void TDD_CIF_SetInfo(unsigned int uiFlag, unsigned int uiBypass, unsigned int uiBypassBusSel,
								unsigned int uiColorPattern, unsigned int uiPatternFormat, unsigned int uiRGBMode,
								unsigned int uiRGBBitMode, unsigned int uiColorSequence, unsigned int uiBusOrder);


/****************************************************************************
*
*    Function of    
*
*    DDI_CIF_SetTransfer
*
*    Input    :    uiFlag
*                uiTransMode        : BURST / INC
*                uiBurst            : BurstSize
*                uiLock            : Lock transfer
*    Output    : 
*    Return    :     none
*
*    Description : Camera I/F DMA transfer mode (CDCR1)
*****************************************************************************/
extern void TDD_CIF_SetTransfer(unsigned int uiFlag, unsigned int uiBurst, unsigned int uiLock,
									 unsigned int uiTransMode);


/****************************************************************************
*
*    Function of    
*
*    DDI_CIF_SetOverlay
*
*    Input    :     uiFlag
*                uiOverlayCNT        : Overlay Count
*                uiOverlayMethod    : Full / Block image
*                uiXOR1            : XOR operation 1
*                uiXOR0            : XOR operation 0
*                uiAlpha1            : Alpha value in alpha 1 (0-25%, 1-50%, 2-75%, 3-100%)
*                uiAlpha0            : Alpha value in alpha 0 (0-25%, 1-50%, 2-75%, 3-100%)
*                
*    Output    : 
*    Return    :     none
*
*    Description :     Camera I/F Overlay control (OCTRL1)
*****************************************************************************/
extern void TDD_CIF_SetOverlay(unsigned int uiFlag, unsigned int uiOverlayCNT, unsigned int uiOverlayMethod,
									unsigned int uiXOR1, unsigned int uiXOR0, unsigned int uiAlpha1, unsigned int uiAlpha0);


/****************************************************************************
*
*    Function of    
*
*    DDI_CIF_SetOverlayKey
*
*    Input    :     uiFlag
*                uiKEYR    :    Chroma-key value R(U)
*                uiKEYG    :    Chroma-key value G(Y)
*                uiKEYB    :    Chroma-key value B(V)
*                uiMKEYR    :    Mask Chroma-key value R(U)
*                uiMKEYG    :    Mask Chroma-key value G(Y)
*                uiMKEYB    :    Mask Chroma-key value B(V)
*    Output    : 
*    Return    :     none
*
*    Description : Camera I/F Overlay key value control 2/3 (OCTRL2/OCTRL3)
*****************************************************************************/
extern void TDD_CIF_SetOverlayKey(unsigned int uiFlag, unsigned int uiKEYR, unsigned int uiKEYG, unsigned int uiKEYB,
										unsigned int uiMKEYR, unsigned int uiMKEYG, unsigned int uiMKEYB);


/****************************************************************************
*
*    Function of    
*
*    DDI_CIF_SetSyncPol
*
*    Input    :     uiHPolarity             : Horizontal polarity (active high/low)
*                uiVPolarity            : Vertical polarity (active high/low)
*    Output    : 
*    Return    :     none
*
*    Description :  CIF_SYNC_POL (Camera I/F sync polarity)
*                
*****************************************************************************/
extern void TDD_CIF_SetSyncPol(unsigned int uiHPolarity, unsigned int uiVpolarity);


/****************************************************************************
*
*    Function of    
*
*    DDI_CIF_SetImage
*
*    Input    :    uiType                 : Input imge(INPUT_IMG), overlay image (OVERLAY_IMG)
*                uiHsize                : Horizontal size
*                uiVSize                : vertical size
*                uiHorWindowingStart    : start X position of windowing image
*                uiHorWindowingEnd    : end X position of windowing image
*                uiVerWindowingStart    : start Y position of windowing image
*                uiVerWindowingEnd    : end Y position of windowing image
*                BaseAddress0            : Y channel base address In Overlay, overlay image adress
*                BaseAddress1            : U channel base address (don't use overlay image)
*                BaseAddress2            : V channel base address (don't use overlay image)
*    Output    : 
*    Return    :     none
*
*    Description : CIF_SETIMG (Camera I/F setting image)
*                
*****************************************************************************/
extern void TDD_CIF_SetImage(unsigned int uiType, unsigned int uiHsize, unsigned int uiVsize,
								  unsigned int uiHorWindowingStart, unsigned int uiHorWindowingEnd,
								  unsigned int uiVerWindowingStart, unsigned int uiVerWindowingEnd);


/****************************************************************************
*
*    Function of    
*
*    DDI_CIF_SetScale
*
*    Input    :     uiScaleEnable    :    Scale enable
*                uiXScale        :    Horizontal scale factor
*                uiYScale        :    Vertical scale factor
*    Output    : 
*    Return    :     none
*
*    Description :     Camera I/F Down scaler (CDS)
*****************************************************************************/
extern void TDD_CIF_SetDownScale(unsigned int uiScaleEnable, unsigned int uiXScale, unsigned int uiYScale);


/****************************************************************************
*
*    Function of    
*
*    DDI_CIF_SetBaseAddr
*
*    Input    :     uiType        :    IN IMAGE                /    OVERLAY IMAGE
*                uiBaseAddr0    :    Input image Y(G)        /    Overlay image base address
*                uiBaseAddr1    :    Input image U(R)        /    none
*                uiBaseAddr2    :    Input image V(B)        /    none
*    Output    : 
*    Return    :     none
*
*    Description    :  IN IMAGE - Camera I/F DMA Configuration (CDCR2/CDCR3/CDCR4)
*                   OVERLAY IMAGE - Camera I/F Overlay base address set (COBA)
*****************************************************************************/
extern void TDD_CIF_SetBaseAddr(unsigned int uiType, unsigned int uiBaseAddr0,
										unsigned int uiBaseAddr1, unsigned int uiBaseAddr2);

extern void TDD_CIF_SetBaseAddr_offset(unsigned int uiType, unsigned int uiOFFSET_Y, unsigned int uiOFFSET_C);

/****************************************************************************
*
*    Function of    
*
*    DDI_CIF_OverlayCtrl
*
*    Input    :     uiFlag (Alpha/Chromakey/Overlay)
*                uiAlphaEnable (Enable/Disable)
*                uiChromakeyEnable (Enable/Disable)
*                uiOverlayEnable (Enable/Disable)
*    Output    : 
*    Return    :     none
*
*    Description    : Camera I/F Alpha/Chromakey/Overlay enable/disable
*****************************************************************************/
extern void TDD_CIF_OverlayCtrl(unsigned int uiFlag, unsigned int uiRgb);


extern void TDD_CIF_SetInterrupt(unsigned int uiFlag);
extern unsigned int TDD_CIF_GetIntStatus(unsigned int uiFlag);
extern void TDD_CIF_Set656FormatConfig(unsigned int uiType, unsigned int uiPsl, unsigned int uiFpv,
										unsigned int uiSpv, unsigned int uiTpv, unsigned int uiFb, unsigned int uiHb, unsigned int uiVb);
extern void TDD_CIF_SetCaptureCtrl(unsigned int uiType,  unsigned int uiSkipNum,
										 unsigned int uiVCnt, unsigned int uiFlag);
extern void TDD_CIF_SetCtrl(unsigned int uiFlag, unsigned int uiPWDN, unsigned int uiBypass_Scaler,
								unsigned int uiPXCLK_POL, unsigned int uiSKPF, unsigned int uiM420_FC,
								unsigned int uiC656, bool bEnableOfATV);
extern void TDD_CIF_SetMaskIntStatus(unsigned int uiFlag);
extern void TDD_CIF_SetSensorOutImgSize(unsigned int uiHsize, unsigned int uiVsize);
extern void TDD_CIF_SetScalerCtrl(unsigned int uiEN);
extern void TDD_CIF_SetFreeScale(unsigned int uiType, unsigned int uiSrcHSize, unsigned int uiSrcVSize,
										unsigned int uiOffH, unsigned int uiOffV, unsigned int uiDstHSize,
										unsigned int uiDstVSize, unsigned int uiHFactor, unsigned int uiVFactor);
extern void TDD_CIF_SetEffectMode(unsigned int uiFlag);
extern void TDD_CIF_SetEffectSepiaUV(unsigned int uiSepiaU, unsigned int uiSepiaV);
extern void TDD_CIF_SetEffectColorSelect(unsigned int uiUStart, unsigned int uiUEnd, 
												unsigned int uiVStart, unsigned int uiVEnd);
extern void TDD_CIF_SetEffectHFilterCoeff(unsigned int uiCoeff0, unsigned int uiCoeff1, unsigned int uiCoeff2);
extern void TDD_CIF_SetEffectSketchTh(unsigned int uithreshold);
extern void TDD_CIF_SetEffectClampTh(unsigned int uithreshold);
extern void TDD_CIF_SetEffectBias(unsigned int uiYBias, unsigned int uiUBias, unsigned int uiVBias);

#if defined(CONFIG_USE_ISP)
void ISP_INTERRUPT_SET(void);
void ISP_INTERRUPT_CLEAR(void);
#endif
extern void TDD_CIF_SWReset(void);
#endif  //  _TDD_CIF_H_
