#ifndef _JPEG_APP_ENC_H_
#define	_JPEG_APP_ENC_H_

#include "tcc_jpeg.h"


extern jpeg_encode_option_type gEncodeOption;

extern void JPEG_Exif_Header_Info_Init(void);
extern void JPEG_SW_Reset(JpegOperationType mode);
extern void JPEG_INTERRUPT_Enable(JpegOperationType mode);
extern void JPEG_INTERRUPT_Disable(JpegOperationType mode);
extern void JPEG_VIDEOCACHE_Enable(unsigned char on, unsigned int read_min_addr, unsigned int read_max_addr, 
									unsigned int wrtie_min_addr, unsigned int wrtie_max_addr);
extern void JPEG_VIDEOCACHE_Drain(void);
extern int JPEG_ENCODE_Start(int ImageWidth, int ImageHeight, unsigned int *uiBitStreamSize, unsigned int *uiHeaderSize);
extern int JPEG_ENCODE_Get_Result(int ImageWidth, int ImageHeight, unsigned int *uiBitStreamSize, unsigned int *uiHeaderSize);
extern void JPEG_SET_Encode_Config(JPEG_ENC_INFO *jpeg_enc_info);
extern void EXIF_Set_GPS_Position(exif_gps_position_type *position);
extern unsigned int JPEG_Encoded_Data_Size(void);
extern int Make_JPEG_Enc_Info(unsigned int jpeg_BufferAddr, unsigned int Width, unsigned int Height, unsigned short factor);
extern int TCCXXX_JPEG_Make_Header(unsigned int jpeg_BufferAddr, jpeg_encode_option_type *EncodeOption, jpeg_header_exif_rsp *jpeg_header_rsp);
extern int JPEG_Make_ExifHeader(TCCXXX_JPEG_ENC_EXIF_DATA *thumb_info, unsigned int virt_baseaddr);

#endif /* _JPEG_APP_ENC_H_ */
