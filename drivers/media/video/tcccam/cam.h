/************************************************************************
*   Telechips Multi Media Player
*    ------------------------------------------------
*
*   FUNCTION    : CAMERA INTERFACE API
*   MODEL       : DMP
*   CPU NAME    : TCCXXX
*   SOURCE      : cam.h
*
*   START DATE  : 2008. 4. 17.
*   MODIFY DATE :
*   DEVISION    : DEPT. SYSTEM 3-2 TEAM
*               : TELECHIPS, INC.
************************************************************************/
#ifndef _CAM_H_ 
#define _CAM_H_

//CIF_SET_INFO
#define SET_CIF_BYPASS_MODE             0x00000001
#define SET_CIF_BYPASS_BUS              0x00000002
#define SET_CIF_COLOR_PATTERN           0x00000004
#define SET_CIF_PATTERN_FORMAT          0x00000008
#define SET_CIF_RGB_MODE                0x00000010
#define SET_CIF_RGBBIT_MODE             0x00000020
#define SET_CIF_COLOR_SEQUENCE          0x00000040
#define SET_CIF_BUS_ORDER               0x00000080
#define SET_CIF_ALL                     0x000000FF

//CIF_SET_INFO_II 
#define SET_CIF_PWDN                    0x00000001
#define SET_CIF_BYPASS_SCALER           0x00000002
#define SET_CIF_PXCLK_POL               0x00000004
#define SET_CIF_SKPF                    0x00000008
#define SET_CIF_M42_FC                  0x00000010
#define SET_CIF_C656                    0x00000020
#define SET_CIF_ATV						0x00000040
#define SET_CIF_II_ALL                  0x0000007F

#define SET_CIF_TRANSFER_MODE           0x00000001
#define SET_CIF_TRANSFER_BURST          0x00000002
#define SET_CIF_TRANSFER_LOCK           0x00000004
#define SET_CIF_TRANSFER_ALL            0x00000007

#define SET_CIF_OVERLAY_COUNT           0x00000001
#define SET_CIF_OVERLAY_METHOD          0x00000002
#define SET_CIF_OVERLAY_XOR0            0x00000004
#define SET_CIF_OVERLAY_XOR1            0x00000008
#define SET_CIF_OVERLAY_ALPHA0          0x00000010
#define SET_CIF_OVERLAY_ALPHA1          0x00000020
#define SET_CIF_OVERLAY_ALL             0x0000003F

#define SET_CIF_OVERLAY_KEY             0x00000001
#define SET_CIF_OVERLAY_MASKKEY         0x00000002
#define SET_CIF_OVERLAYKEY_ALL          0x00000003

#define SET_CIF_ALPHA_ENABLE            0x00000001
#define SET_CIF_ALPHA_DISABLE           0x00000002
#define SET_CIF_CHROMA_ENABLE           0x00000004
#define SET_CIF_CHROMA_DISABLE          0x00000008
#define SET_CIF_OVERLAY_ENABLE          0x00000010
#define SET_CIF_OVERLAY_DISABLE         0x00000020
#define SET_CIF_COLOR_CONV_ENABLE       0x00000040
#define SET_CIF_COLOR_CONV_DISABLE      0x00000080
//#define SET_CIF_OVERLAY_RGB_MODE      0x00000100
#define SET_CIF_COLOR_MODE_RGB          0x00000200
#define SET_CIF_COLOR_MODE_YUV          0x00000400

#define SET_CIF_CR2Y_FMT                0x00000001
#define SET_CIF_CR2Y_EN                 0x00000002
#define SET_CIF_CR2Y_ALL                0x00000003

#define INPUT_IMG                       0x00000001
#define OVERLAY_IMG                     0x00000002
#define IN_IMG_ROLLING                  0x00000004
#define IN_ENC_START_ADDR               0x00000008



#ifndef _82x  // CIF_API_INTERRUPT_FEATURE

/////////////////////////////////////////////////////////////////////////////
// CIF_SET_INTERRUPT
/////////////////////////////////////////////////////////////////////////////

//IEN     Hw31  Interrupt Enable  0:interrupt disable, 1:interrupt enable      
#define  SET_CIF_INT_ENABLE                         0x00000001
#define  SET_CIF_INT_DISABLE                        0x00000002
//URV    Hw30  Update Register in VSYNC   0:Register is update without VSYNC , 1:When VSYNC is posedge, register is updated. 
#define  SET_CIF_UPDATE_IN_VSYNC                    0x00000004
#define  SET_CIF_UPDATE_WITHOUT_VSYNC               0x00000008
//ITY     Hw29  Interrupt Type  0:Pulse type, 1:Hold-up type when respond signal(ICR) is high  
#define  SET_CIF_INT_TYPE_HOLDUP                    0x00000010
#define  SET_CIF_INT_TYPE_PULSE                     0x00000020
//Hw28  Interrupt Clear 0:.... , 1:Interrupt Clear (using ITY is Hold-up type)  //Hw28
#define  SET_CIF_INT_HOLD_CLEAR                     0x00000040

/////////////////////////////////////////////////////////////////////////////
// CIF_SET_INTERRUPT_MASK
/////////////////////////////////////////////////////////////////////////////
//MVN    Hw26           // Mask interrupt of VS negative edge,  0:Don't mask, 1:Mask
#define  SET_CIF_INT_VS_NEGA_MASK                   0x00000001
#define  SET_CIF_INT_VS_NEGA_NOT_MASK               0x00000002
//MVP    Hw25           // Mask interrupt of VS negative edge,  0:Don't mask, 1:Mask
#define  SET_CIF_INT_VS_POSI_MASK                   0x00000004
#define  SET_CIF_INT_VS_POSI_NOT_MASK               0x00000008
//MVIT   Hw24           // Mask interrupt of VCNT Interrupt,  0:Don't mask, 1:Mask
#define  SET_CIF_INT_VCNT_MASK                      0x00000010
#define  SET_CIF_INT_VCNT_NOT_MASK                  0x00000020
//MSE    Hw23           // Mask interrupt of Scaler Error,  0:Don't mask, 1:Mask
#define  SET_CIF_INT_SCALER_ERR_MASK                0x00000040
#define  SET_CIF_INT_SCALER_ERR_NOT_MASK            0x00000080
//MSF    Hw22           // Mask interrupt of Scaler finish,  0:Don't mask, 1:Mask
#define  SET_CIF_INT_SCALER_FINISH_MASK             0x00000100
#define  SET_CIF_INT_SCALER_FINISH_NOT_MASK         0x00000200
//MENS    Hw21           // Mask interrupt of Encoding start,  0:Don't mask, 1:Mask
#define  SET_CIF_INT_ENC_STRT_MASK                  0x00000400
#define  SET_CIF_INT_ENC_STRT_NOT_MASK              0x00000800
//MRLV  Hw20           // Mask interrupt of Rolling V address,  0:Don't mask, 1:Mask
#define  SET_CIF_INT_ROLL_VADDR_MASK                0x00001000
#define  SET_CIF_INT_ROLL_VADDR_NOT_MASK            0x00002000
//MRLU  Hw19           // Mask interrupt of Rolling U address,  0:Don't mask, 1:Mask
#define  SET_CIF_INT_ROLL_UADDR_MASK                0x00004000
#define  SET_CIF_INT_ROLL_UADDR_NOT_MASK            0x00008000
//MRLY   Hw18           // Mask interrupt of Rolling Y address,  0:Don't mask, 1:Mask
#define  SET_CIF_INT_ROLL_YADDR_MASK                0x00010000
#define  SET_CIF_INT_ROLL_YADDR_NOT_MASK            0x00020000
//MSCF  Hw17           // Mask interrupt of Capture frame,  0:Don't mask, 1:Mask
#define  SET_CIF_INT_CAPTURE_FRM_MASK               0x00040000
#define  SET_CIF_INT_CAPTURE_FRM_NOT_MASK           0x00080000
//MSOF  Hw16           // Mask interrupt of Stored one frame,  0:Don't mask, 1:Mask
#define  SET_CIF_INT_STORE_1FRM_MASK                0x00100000
#define  SET_CIF_INT_STORE_1FRM_NOT_MASK            0x00200000
#define  SET_CIF_INT_ALL_MASK                       0x00400000
#define  SET_CIF_INT_ALL_CLEAR_MASK                 0x00800000
//VSS    Hw12           // Status of vertical sync, 0: Non-vertical sync blank area. , 1: vertical sync blank area
//#define  SET_CIF_INT_VS_STATUS                     0x00400000
//#define  SET_CIF_INT_NON_VS_STATUS                 0x00800000

//////////////////////////////////////////////////////////////////////////////
// CIF_GET_INTERRUPT_STATUS
//////////////////////////////////////////////////////////////////////////////
// VSS    Hw12           // Status of vertical sync, 0: Non-vertical sync blank area. , 1: vertical sync blank area
#define  GET_CIF_INT_VS_STATUS                      0x00000001
//#define           HwCIRQ_VN                                       Hw10            // VS positive, 0:-, 1:When VS is generated if Negative edge
#define  GET_CIF_INT_NEGA_VS_GEN                    0x00000002
//#define           HwCIRQ_VP                                       Hw9            // VS positive, 0:-, 1:When VS is generated if positive edge
#define  GET_CIF_INT_POSI_VS_GEN                    0x00000004
//#define           HwCIRQ_VIT                                      Hw8            // VCNT Interrupt, 0:-, 1:When VCNT is generated....
#define  GET_CIF_INT_VCNT_GEN                       0x00000008
//#define           HwCIRQ_SE                                       Hw7            // Scaler Error, 0:-, 1:When Scale operation is not correct.
#define  GET_CIF_INT_SCALER_ERR                     0x00000010
//#define           HwCIRQ_SF                                       Hw6            // Scaler Finish, 0:-, 1:When Scale operation is finished
#define  GET_CIF_INT_SCALER_FINISH                  0x00000020
//#define           HwCIRQ_ENS                                      Hw5            // Encoding start status, 0:-, 1:When Y address is bigger than encoding start address, this bit is high
#define  GET_CIF_INT_ENC_STRT                       0x00000040
//#define           HwCIRQ_ROLV                                     Hw4            // Rolling V address status, 0:-, 1:If V address is move to start address, this bit is high
#define  GET_CIF_INT_ROLL_VADDR_STRT                0x00000080
//#define           HwCIRQ_ROLU                                     Hw3            // Rolling U address starus, 0:-, 1:If U address is move to start address, this bit is high 
#define  GET_CIF_INT_ROLL_UADDR_STRT                0x00000100
//#define           HwCIRQ_ROLY                                     Hw2            // Rolling Y address starus, 0:-, 1:If Y address is move to start address, this bit is high 
#define  GET_CIF_INT_ROLL_YADDR_STRT                0x00000200
//#define           HwCIRQ_SCF                                      Hw1            // Stored captured frame,  0:-, 1:If Captured frame is stored, this bit is high
#define  GET_CIF_INT_CAPTURE_FRM_STORE              0x00000800
//#define           HwCIRQ_SOF                                      Hw0            // Stored One frame, 0-, 1:If one frame if stored, this bit is high.
#define  GET_CIF_INT_ONEFRAME_STORE                 0x00001000
#endif  // _82x

#define SET_CIF_CLOCK_DIV0                          0x00000001
#define SET_CIF_CLOCK_DIV1                          0x00000002
#define SET_CIF_CLOCK_DIV2                          0x00000004
#define SET_CIF_CLOCK_DIV3                          0x00000008
#define SET_CIF_CLOCKPOLHIGH_ENABLE                 0x00000010
#define SET_CIF_CLOCKPOLHIGH_DISABLE                0x00000020
#define SET_CIF_CLOCKINT_ENABLE                     0x00000040
#define SET_CIF_CLOCKINT_DISABLE                    0x00000080
#define SET_CIF_CLOCK_ALL                           0x000000FF

#define SET_CIF_656_PSL                             0x00000001
#define SET_CIF_656_FPV                             0x00000002
#define SET_CIF_656_SPV                             0x00000004
#define SET_CIF_656_TPV                             0x00000008
#define SET_CIF_656_H_BLANK                         0x00000010
#define SET_CIF_656_V_BLANK                         0x00000020
#define SET_CIF_656_FIELD_INFO						0x00000040
#define SET_CIF_656_ALL                             0x0000007F

#define GET_CIF_OVERLAY_READ_ERR                    0x00000001
#define GET_CIF_VCH_READ_ERR                        0x00000002
#define GET_CIF_UCH_READ_ERR                        0x00000004
#define GET_CIF_YCH_READ_ERR                        0x00000008
#define GET_CIF_OVERLAY_WRITE_ERR                   0x00000010
#define GET_CIF_VCH_WRITE_ERR                       0x00000020
#define GET_CIF_UCH_WRITE_ERR                       0x00000040
#define GET_CIF_YCH_WRITE_ERR                       0x00000080
#define GET_CIF_OVERLAY_EMPTY_ERR                   0x00000100
#define GET_CIF_VCH_EMPTY_ERR                       0x00000200
#define GET_CIF_UCH_EMPTY_ERR                       0x00000400
#define GET_CIF_YCH_EMPTY_ERR                       0x00000800
#define GET_CIF_OVERLAY_FULL_ERR                    0x00001000
#define GET_CIF_VCH_FULL_ERR                        0x00002000
#define GET_CIF_UCH_FULL_ERR                        0x00004000
#define GET_CIF_YCH_FULL_ERR                        0x00008000

#define SET_CIF_SKIP_NUM                            0x00000001
#define SET_CIF_VCNT_NUM                            0x00000002

#define SET_CIF_CCM1_ENCNUM                         0x00000001  // Encode INT number (using CAP mode) [31:28], value area (0~15), Encode interrupt number
#define SET_CIF_CCM1_ROLNUMV                        0x00000002   // Rolling number in V (using CAP mode) [27:24], value area (0~15), Rolling number
#define SET_CIF_CCM1_ROLNUMU                        0x00000004  // Rolling number in U (using CAP mode) [23:20], value area (0~15), Rolling number
#define SET_CIF_CCM1_ROLNUMY                        0x00000008  // Rolling number in Y (using CAP mode) [19:16], value area (0~15), Rolling number
#define SET_CIF_CCM1_CB                             0x00000010  // Capture Busy,   0:-, 1:Capture busy

#define SET_CIF_EIT_ENC_INT                         0x00000001
#define SET_CIF_EIT_ALWAYS_1_PULSE                  0x00000002
#define SET_CIF_UES_ENABLE                          0x00000004
#define SET_CIF_UES_DISABLE                         0x00000008
#define SET_CIF_RLV_ENABLE                          0x00000010
#define SET_CIF_RLV_DISABLE                         0x00000020
#define SET_CIF_RLU_ENABLE                          0x00000040
#define SET_CIF_RLU_DISABLE                         0x00000080
#define SET_CIF_RLY_ENABLE                          0x00000100
#define SET_CIF_RLY_DISABLE                         0x00000200
#define SET_CIF_CAP_ENABLE                          0x00000400
#define SET_CIF_CAP_DISABLE                         0x00000800
#define SET_CIF_VEN_ENABLE                          0x00001000
#define SET_CIF_VEN_DISABLE                         0x00002000

#define READ_CIF_CUR_ADDR_Y                         0x00000001
#define READ_CIF_CUR_ADDR_U                         0x00000002
#define READ_CIF_CUR_ADDR_V                         0x00000004
#define READ_CIF_CUR_LINE_CNT                       0x00000008

#define SET_CIF_CEM_UVS                             0x00000001  // UV Swap  0:u-v-u-v sequence, 1:v-u-v-u sequence
#define SET_CIF_CEM_VB                              0x00000002  // V Bias (V channel value offset),   0:disable, 1:Enable 
#define SET_CIF_CEM_UB                              0x00000004  // U Bias (U channel value offset),   0:disable, 1:Enable
#define SET_CIF_CEM_YB                              0x00000008  // Y Bias (Y channel value offset),   0:disable, 1:Enable
#define SET_CIF_CEM_YCS                             0x00000010  // YC Swap  0:u-y-v-y sequence, 1:y-u-y-v sequence
#define SET_CIF_CEM_IVY                             0x00000020  // Invert Y,   0:disable, 1:Enable 
#define SET_CIF_CEM_STC                             0x00000040  // Strong C,   0:disable, 1:Enable 
#define SET_CIF_CEM_YCL                             0x00000080  // Y Clamp (Y value clipping),   0:disable, 1:Enable 
#define SET_CIF_CEM_CS                              0x00000100  // C Select (Color filter),   0:disable, 1:Enable(Color filter)  
#define SET_CIF_CEM_SKT                             0x00000200  // Sketch Enable,   0:disable, 1:Enable 
#define SET_CIF_CEM_EMM                             0x00000400  // Emboss mode,   0:Positive emboss, 1:Negative emboss  // Only 0 SET  ¾Êº»´Ù. 
#define SET_CIF_CEM_EMB                             0x00000800  // Emboss,   0:disable, 1:Enable                          
#define SET_CIF_CEM_NEGA                            0x00001000  // Negative mode,   0:disable, 1:Enable
#define SET_CIF_CEM_GRAY                            0x00002000  // Gray mode,   0:disable, 1:Enable 
#define SET_CIF_CEM_SEPI                            0x00004000  // Sepia mode,   0:disable, 1:Enable    
#define SET_CIF_CEM_NOR                             0x00008000  // Normal mode,   0:Effect mode, 1:Normal mode  
#define SET_CIF_CEM_ALL_CLEAR                       0x00010000

#define SET_CIF_INPATH_H_WAIT                       0x00000001
#define SET_CIF_INPATH_S_CYCLE                      0x00000002
#define SET_CIF_INPATH_I_WAIT                       0x00000004
#define SET_CIF_INPATH_R_ENABLE                     0x00000008
#define SET_CIF_INPATH_R_DISABLE                    0x00000010
#define SET_CIF_INPATH_FLUSH_ENABLE                 0x00000020
#define SET_CIF_INPATH_FLUSH_DISABLE                0x00000040
#define SET_CIF_INPATH_ENABLE                       0x00000080
#define SET_CIF_INPATH_DISABLE                      0x00000100
#define SET_CIF_INPATH_MEM                          0x00000200
#define SET_CIF_INPATH_CAM                          0x00000400

#define SET_CIF_INPATH_SRC_SIZE                     0x00000001
#define SET_CIF_INPATH_SRC_OFFSET                   0x00000002
#define SET_CIF_INPATH_DST_SIZE                     0x00000004
#define SET_CIF_INPATH_SCALE                        0x00000008
#define SET_CIF_INPATH_ALL                          0x0000000F

#define SET_CIF_SCALER_ENABLE                       1
#define SET_CIF_SCALER_DISABLE                      0
                            
#define SET_CIF_SCALER_SRC_SIZE                     0x00000001
#define SET_CIF_SCALER_SRC_OFFSET                   0x00000002
#define SET_CIF_SCALER_DST_SIZE                     0x00000004
#define SET_CIF_SCALER_FACTOR                       0x00000008
#define SET_CIF_SCALER_ALL                          0x0000000F

/*
#define SET_SCALE_TYPE_422SEQ0                      0
#define SET_SCALE_TYPE_422SEQ1                      1
#define SET_SCALE_TYPE_422SEPE                      2
#define SET_SCALE_TYPE_INVALID                      3

#define SET_SCALE_PATH_TOMEMORY                     0
#define SET_SCALE_PATH_TOLCD                        1

#define SCALE_BUSY_INT_ENABLE                       0
#define SCALE_BUSY_INT_DISABLE                      1

#define SCALE_READY_INT_ENABLE                      0
#define SCALE_READY_INT_DISABLE                     1

#define MSC_CFG_WAIT                                0x00000001
#define MSC_CFG_RDY                                 0x00000002
#define MSC_CFG_PATH                                0x00000004
#define MSC_CFG_TYPE                                0x00000008
#define MSC_CFG_ALL                                 0x0000000F

#define MSC_CTRL_BUSY_INT_EN                        0x00000001
#define MSC_CTRL_BUSY_INT_DIS                       0x00000002
#define MSC_CTRL_RDY_INT_EN                         0x00000004
#define MSC_CTRL_RDY_INT_DIS                        0x00000008
#define MSC_CTRL_EN                                 0x00000010 
#define MSC_CTRL_DIS                                0x00000020
#define MSC_CTRL_ROLL_EN                            0x00000040
#define MSC_CTRL_ROLL_DIS                           0x00000080
#define MSC_CTRL_MIDDLE_EN                          0x00000100
#define MSC_CTRL_MIDDLE_DIS                         0x00000200
#define MSC_CTRL_ROLL_RESTR_SET                     0x00000400
#define MSC_CTRL_ROLL_GOSTOP_EN                     0x00000800
#define MSC_CTRL_ROLL_GOSTOP_DIS                    0x00001000
#define MSC_CTRL_MIDDLE_GOSTOP_EN                   0x00002000
#define MSC_CTRL_MIDDLE_GOSTOP_DIS                  0x00004000
#define MSC_CTRL_ROLL_INT_EN                        0x00008000
#define MSC_CTRL_ROLL_INT_DIS                       0x00010000
#define MSC_CTRL_MIDDLE_INT_EN                      0x00020000
#define MSC_CTRL_MIDDLE_INT_DIS                     0x00040000
#define MSC_CTRL_ALL                                0x0007FFFF
*/

/* Drivers IDs */
enum
{
	MON = 0,
	SEQ,
	AE,
	AWB,
	FD,
	AF,
	AFM,
	MODE,
	JPEG = 9,
	HG = 11,
	MON_EXT = 16,
	SEQ_EXT,
	AE_EXT,
	AWB_EXT,
	FD_EXT,
	AF_EXT,
	AFM_EXT,
	MODE_EXT,
	JPEG_EXT = 25,
	HG_EXT
};

#define     CAM_BW_16BIT                            0x00
#define     CAM_BW_8BIT                             0x01

#define     CAM_HSIZE_PREV                          320
#define     CAM_VSIZE_PREV                          240

#define     CAM_HSIZE                               320
#define     CAM_VSIZE                               240

#if defined (FEATURE_LCD_128x160)
#define     CAMLCD_HSIZE                            128
#define     CAMLCD_VSIZE                            160
#elif defined (FEATURE_LCD_176x220)
#define     CAMLCD_HSIZE                            176
#define     CAMLCD_VSIZE                            220
#define     CAMLCD_ROT90_HSIZE                      224
#define     CAMLCD_ROT90_VSIZE                      176
#elif defined (FEATURE_LCD_240x320)
#define     CAMLCD_HSIZE                            240
#define     CAMLCD_VSIZE                            320
#define     CAMLCD_ROT90_HSIZE                      320
#define     CAMLCD_ROT90_VSIZE                      240
#else
#define     CAMLCD_HSIZE                            240
#define     CAMLCD_VSIZE                            320
#endif

#define     VOD_HSIZE                               176
#define     VOD_VSIZE                               144

#define     CAM_HSIZE_TV                            640
#define     CAM_VSIZE_TV                            480

typedef struct _CIF_CONTROL_INFO
{
	unsigned int uiFlag;
	unsigned int uiBypass;
	unsigned int uiBypassBusSel;
	unsigned int uiColorPattern;
	unsigned int uiPatternFormat;
	unsigned int uiRGBMode;
	unsigned int uiRGBBitMode;
	unsigned int uiColorSequence;
	unsigned int uiBusOrder;
	unsigned int uiOverlayCNT;
} CIF_CONTROL_INFO;

typedef struct _CIF_TRANSFER_CONFIG
{
	unsigned int uiFlag;
	unsigned int uiTransMode;
	unsigned int uiBurst;
	unsigned int uiLock;
} CIF_TRANSFER_CONFIG;

typedef struct _CIF_OVERLAY_CONFIG
{
	unsigned int uiFlag;
	unsigned int uiOverlayCNT;
	unsigned int uiOverlayMethod;
	unsigned int uiXOR1;
	unsigned int uiXOR0;
	unsigned int uiAlpha1;
	unsigned int uiAlpha0;
} CIF_OVERLAY_CONFIG;

typedef struct _CIF_OVERLAY_KEY
{
	unsigned int uiFlag;
	unsigned int uiKEYR;
	unsigned int uiKEYG;
	unsigned int uiKEYB;
	unsigned int uiMKEYR;
	unsigned int uiMKEYG;
	unsigned int uiMKEYB;
} CIF_OVERLAY_KEY;

typedef struct _CIF_SYNC_POLARITY
{
	unsigned int uiHPolarity;
	unsigned int uiVpolarity;
} CIF_SYNC_POLARITY;

typedef struct _CIF_IMAGE_CONFIG
{
	unsigned int uiType;
	unsigned int uiHsize;
	unsigned int uiVsize;
	unsigned int uiHorWindowingStart;
	unsigned int uiHorWindowingEnd;
	unsigned int uiVerWindowingStart;
	unsigned int uiVerWindowingEnd;
} CIF_IMAGE_CONFIG;

typedef struct _CIF_SCALE_CONFIG
{
	unsigned int uiScaleEnable;
	unsigned int uiXScale;
	unsigned int uiYScale;
} CIF_SCALE_CONFIG;

typedef struct _CIF_BASE_ADDRESS
{
	unsigned int uiType;
	unsigned int uiBaseAddr0;
	unsigned int uiBaseAddr1;
	unsigned int uiBaseAddr2;
} CIF_BASE_ADDRESS;    

typedef struct _CIF_OVERLAY_CONTROL
{
	unsigned int uiFlag;
	unsigned int uiAlphaEnable;
	unsigned int uiChromakeyEnable;
	unsigned int uiOverlayEnable;
} CIF_OVERLAY_CONTROL;        

typedef struct _CIF_INTANDOPERATING_CONTROL
{
	unsigned int uiFlag;
	unsigned int uiReadOneFrameStatus;
} CIF_INTANDOPERATING_CONTROL;    

extern void CIF_Clock_Get(void);
extern void CIF_Clock_Put(void);
extern void CIF_Open(void);
extern void CIF_Close(void);
extern void CIF_ONOFF(unsigned int uiOnOff);
extern void CIF_OpStop (char wait_SOF, char sw_reset );
extern void CIF_WaitFrameSync(unsigned int exp_timer);
#endif
