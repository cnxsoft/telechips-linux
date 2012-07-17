#ifndef _JPEG_APP_DEC_H_
#define	_JPEG_APP_DEC_H_

#include "tcc_jpeg.h"

extern int JPEG_DECODE_Start(JpegOperationType type);
extern JPEG_PARSER_STATUS JPEG_IMAGE_Parser(unsigned int file_length, unsigned int *BitStreamBuff, unsigned char IsEhiInterface, jpeg_parser_rsp *JpegImageInfo);
extern unsigned int JPEG_Dec_Scale_Set(unsigned short usWidth, unsigned short usHeight, unsigned char ImageFormat);
extern unsigned int JPEG_Dec_IFrame_Width(unsigned char ucScale, unsigned short usJpegHsize);
extern unsigned int JPEG_Dec_IBuf_Height(unsigned char ucScale, unsigned short usJpegVsize);
extern JPEG_PARSER_STATUS ImagePaddingForDecode(unsigned int Chroma, unsigned int ImgWidth, unsigned int ImgHeight,
								unsigned int *NeedsPadding, unsigned int *pad_width, unsigned int *pad_height);
extern int JPEG_SET_Decode_Config (JPEG_DEC_INFO *jpeg_dec_info);
extern int JPEG_RESIZE_For_Thumbnail(unsigned src_width, unsigned src_height, unsigned int src_fmt);
extern int JPEG_DECODE_For_Thumbnail(unsigned int data_size);

#endif /* _JPEG_APP_DEC_H_ */
