/************************************************************************
*    Telechips Multi Media Player
*    ------------------------------------------------
*
*    FUNCTION    : CAMERA INTERFACE API
*    MODEL        : DMP
*    CPU NAME    : TCCXXX
*    SOURCE        : cam_reg.h
*
*    START DATE    : 2008. 4. 17.
*    MODIFY DATE :
*    DEVISION    : DEPT. SYSTEM 3-2 TEAM
*                : TELECHIPS, INC.
************************************************************************/
#ifndef _CAM_REG_H_ 
#define _CAM_REG_H_

#define ON                              1
#define OFF                             0

#define ENABLE                          1
#define DISABLE                         0

// 1. On/Off on CIF >> 0:Can't operate CIF , 1:Operating CIF
#define CIF_ON                          1
#define CIF_OFF                         0

// 4.  HwICPCR1_POL                            Hw21         // PXCLK Polarity >> 0:Positive edge, 1:Negative edge
#define POSITIVE                        1
#define NEGATIVE                        0


#define CIF_PWDN_ENABLE                 1
#define CIF_PWDN_DISABLE                0
#define CIF_BYPASS_SCALER_ENABLE        1
#define CIF_BYPASS_SCALER_DISABLE       0
#define CIF_PXCLK_NEGATIVE_EDGE         1
#define CIF_PXCLK_POSITIVE_EDGE         0
#define CIF_656CONVERT_ENABLE           1
#define CIF_656CONVERT_DISABLE          0



// 5.  HwICPCR1_SKPF                                        // Skip Frame >> 0~7 #Frames skips   [20:18]
#define FRAME_0                         0
#define FRAME_1                         1
#define FRAME_2                         2
#define FRAME_3                         3
#define FRAME_4                         4
#define FRAME_5                         5
#define FRAME_6                         6
#define FRAME_7                         7
// 6.
//  HwICPCR1_M420_ZERO                      HwZERO        // Format Convert (YUV422->YUV420) , Not-Convert
//  HwICPCR1_M420_ODD                       Hw17             // converted in odd line skip   // 10
//  HwICPCR1_M420_EVEN                      (Hw17|Hw16)  // converted in even line skip  //11
#define M420_ZERO                       0 // YUV422
#define M420_ODD                        2 // YUV420
#define M420_EVEN                       3

// BP,  Hw15, Bypass (Non-Separate)
#define SEPARATE                        0
#define NON_SEPARATE                    1
//#define NON_BYPASS                  0  // Not Bypass = Separate
//#define BYPASS                           1  // Bypass = NON_Separate

//BBS, Hw14, Bypass BUS Select
#define MSB_FIRST                       0
#define LSB_FIRST                       1

// CP , Hw12, Color Pattern
#define MODE_YUV                        0  //Ycbcr(YUV, RGB)
#define MODE_RGB                        1  // RGB(555,565,bayer)

// PF , Hw11,Hw10  , Pattern Format
#define FMT444                          0  
#define FMT422                          1
#define FMTRGB                          2  // 4:2:0 format or RGB(555,565,bayer) color pattern

//RGBM , Hw9,Hw8 , RGB mode 
#define BAYER_RGB                       0  
#define RGB555                          1
#define RGB565                          2

//RGBBM, Hw7,Hw6 , RGB Bit Mode
#define MODE16                          0  // 16bit mode (4:2:0 Ycbcr/ YUV, RGB555/565, 4:2:2/4:4:4 format)
#define MODE8                           1  // 8bit non-sync
#define MODE8SYNC                       2  // 8bit (Bayer/555/565RGB), 8bit enable sync (sync-port)

// CS Hw5, Hw4 , Color Sequence
#define SEQYUYV                         0
#define SEQYVYU                         1
#define SEQUYVY                         2
#define SEQVYUY                         3
//#define SEQ_YBYR                     0
//#define SEQ_YRYB                     1
//#define SEQ_BYRY                     2
//#define SEQ_RYBY                     3

// BO Hw2 , BUS Order
#define SWICH_BUS                       1
#define NON_SWICH_BUS                   0

/////////////////////////////////////////////////////////////////////////////

#define ACT_HIGH                        1
#define ACT_LOW                         0

#define NEGATIVE_EDGE                   1
#define POSITIVE_EDGE                   0

// Hw0, Hw1, BS , preamble and Status Location
#define BURST1                          0
#define BURST2                          1
#define BURST4                          2
#define BURST8                          3

// Hw2, LOCK , Lock Transfer
#define LOCK_TR                         1
#define NON_LOCK_TR                     0

// Hw3, TM, Transfer Method
#define INC_TRANS                       1
#define BURST_TRANS                     0

#define OL_RGB_565                      0    // RGB mode 565RGB 
#define OL_RGB_555                      1   // RGB mode 555RGB
#define OL_RGB_444                      2   // RGB mode 444RGB
#define OL_RGB_332                      3   // RGB mode 332RGB

#define D_SCALE1                        0
#define D_SCALE2                        1
#define D_SCALE4                        2
#define D_SCALE8                        3

#define RGB_SEQ_16                      0
#define BGR_SEQ_16                      1
#define RGB_GBG_16                      4
#define BGR_GBG_16                      5
#define GBG_RGB_16                      6
#define GBG_BGR_16                      7
#define RGB_SEQ_8_565                   8
#define BGR_SEQ_8_565                   9
#define RGB_GBG_8_555                   12  //C
#define BGR_GBG_8_555                   13  //D
#define GBG_RGB_8_555                   14  //E
#define GBG_BGR_8_555                   15  //F

#define FULL_OVERLAY                    0
#define BLOCK_OVERLAY                   1

#define OP_XOR                          0
#define OP_ALPHA                        1

#define ALPHA25                         0
#define ALPHA50                         1
#define ALPHA75                         2
#define ALPHA100                        3


#define PSL_1ST                         0
#define PSL_2ND                         1
#define PSL_3RD                         2
#define PSL_4TH                         3

#define INPATH_422SEQ0                  0
#define INPATH_422SEQ1                  1
#define INPATH_422SEPA                  2
#define INPATH_420SEPA                  3


#endif
