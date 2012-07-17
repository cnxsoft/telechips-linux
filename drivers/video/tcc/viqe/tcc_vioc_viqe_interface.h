#ifndef TCC_VIOC_VIQE_INTERFACE_H_INCLUDED
#define TCC_VIOC_VIQE_INTERFACE_H_INCLUDED


 void TCC_VIQE_DI_Init(int scalerCh, int useWMIXER, unsigned int FrmWidth, unsigned int FrmHeight, int crop_top, int crop_bottom, int crop_left, int crop_right, int OddFirst);
 void TCC_VIQE_DI_Run(unsigned int FrmWidth, unsigned int FrmHeight, int crop_top, int crop_bottom, int crop_left, int crop_right, int OddFirst);
 void TCC_VIQE_DI_DeInit(void);
 void TCC_VIQE_DI_Init60Hz(int lcdCtrlNum, int Lcdc_layer, int useSCALER, unsigned int img_fmt, 
						unsigned int srcWidth, unsigned int srcHeight,
						unsigned int destWidth, unsigned int destHeight, unsigned int offset_x, unsigned int offset_y, int OddFirst);
void TCC_VIQE_DI_Swap60Hz(int mode);
void TCC_VIQE_DI_SetFMT60Hz(int enable);
void TCC_VIQE_DI_Run60Hz(int useSCALER, unsigned int addr0, unsigned int addr1, unsigned int addr2,
						unsigned int srcWidth, unsigned int srcHeight,	
						int crop_top, int crop_bottom, int crop_left, int crop_right,
						unsigned int destWidth, unsigned int destHeight,
						unsigned int offset_x, unsigned int offset_y, int OddFirst, int FrameInfo_Interlace);
 void TCC_VIQE_DI_DeInit60Hz(void);


#endif
