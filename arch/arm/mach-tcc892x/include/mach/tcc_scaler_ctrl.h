
/********************************************************************************
								Memory Scaler
********************************************************************************/
#include <mach/tcc_scaler_ioctrl.h>

typedef enum
{
	M2M_SCALER0,
	M2M_SCALER1
}M2M_TYPE;

#define SET_SCALE_TYPE_422SEQ0						0
#define SET_SCALE_TYPE_422SEQ1						1
#define SET_SCALE_TYPE_422SEPE						2
#define SET_SCALE_TYPE_INVALID						3

#define SET_SCALE_PATH_TOMEMORY 					0
#define SET_SCALE_PATH_TOLCD						1

#define SCALE_BUSY_INT_ENABLE						0
#define SCALE_BUSY_INT_DISABLE						1

#define SCALE_READY_INT_ENABLE						0
#define SCALE_READY_INT_DISABLE 					1


#define SET_M2MSCL_INT_ENABLE				0x00000001
#define SET_M2MSCL_INT_DISABLE				0x00000002


#define  HwMSC_SRC_Y_BASE_ADDR		   0xFFFFFFFC				 //   HwMSC_SRC_Y_BASE_ADDR [31: 2]  // Scaler source image Y base
#define  HwMSC_SRC_U_BASE_ADDR		   0xFFFFFFFC				// HwMSC_SRC_U_BASE_ADDR [31: 2]  // Scaler source image U base
#define  HwMSC_SRC_V_BASE_ADDR		   0xFFFFFFFC				 // HwMSC_SRC_V_BASE_ADDR [31: 2]  // Scaler source image V base

#define  HwMSC_DST_Y_BASE_ADDR 		0xFFFFFFFC				  //	 HwMSC_DST_Y_BASE_ADDR [31: 2]	// Scaler destination image Y base
#define  HwMSC_DST_U_BASE_ADDR 		0xFFFFFFFC				  //	 HwMSC_DST_U_BASE_ADDR [31: 2]	// Scaler destination image U base
#define  HwMSC_DST_V_BASE_ADDR 		0xFFFFFFFC				  //	 HwMSC_DST_V_BASE_ADDR [31: 2]	// Scaler destination image V base

#define  HwMSC_SRC_SIZE_H				0x00000FFF				  //	 HwMSC_SRC_SIZE_H [11: 0]	// Input image Horizontal size by pixel
#define  HwMSC_SRC_SIZE_V				0x0FFF0000				  //	HwMSC_SRC_SIZE_V [27: 16]  // Input image Vertical size by pixel

#define  HwMSC_DST_SIZE_H				0x00000FFF				  //HwMSC_DST_SIZE_H   [11: 0]	 // Input image Horizontal size by pixel
#define  HwMSC_DST_SIZE_V				0x0FFF0000				  //HwMSC_DST_SIZE_V   [27: 16]  // Input image Vertical size by pixel

#define  HwMSC_SRC_OFFSET_Y			0x00000FFF				  // HwMSC_SRC_OFFSET_Y [11: 0]   // Input image line offset of luminace
#define  HwMSC_SRC_OFFSET_C			0x0FFF0000				  // HwMSC_SRC_OFFSET_C [27: 16]  // Input image line offset of chrominance

#define  HwMSC_DST_OFFSET_Y			0x00000FFF				  //  HwMSC_DST_OFFSET_Y [11: 0]   // Input image line offset of luminace
#define  HwMSC_DST_OFFSET_C			0x0FFF0000				  //  HwMSC_DST_OFFSET_C [27: 16]  // Input image line offset of chrominance

#define  HwMSC_SRC_CFG_WAIT			0x00000700				  // WAIT	[10:8]

#define  HwMSC_HRATIO					0x00003FFF			 // HwMSC_HRATIO [29:16] Horizontal scale factor
#define  HwMSC_VRATIO					0x3FFF0000			 // HwMSC_VRATIO [13:0]  Vertical scale factor

#define  HwMSC_DST_ROLL_CNT 			0x0FFF0000
#define  HwMSC_DST_MID_CNT				0x00000FFF

#define  HwMSC_C_R_CNT				   0x0FFF0000



#define MSC_CFG_COP                                 				0x00000001
#define MSC_CFG_WAIT                               				0x00000002
#define MSC_CFG_RDY                                 				0x00000004
#define MSC_CFG_INTM                                				0x00000008
#define MSC_CFG_PATH                               			 	0x00000010
#define MSC_CFG_INTLV                               				0x00000020
#define MSC_CFG_TYPE                                				0x00000040
#define MSC_CFG_ALL                                 				0x0000007F

#define MSC_CFG_ARGB				0x00010000				
#define MSC_CFG_IMODE				0x00020000
#define MSC_CFG_OMODE				0x00080000
#define MSC_CFG_MSBF				0x80000000
#define MSC_CFG_ALL_ALPHA			0x801F0000

#define MSC_CTRL_BUSY_INT_EN						0x00000001
#define MSC_CTRL_BUSY_INT_DIS						0x00000002
#define MSC_CTRL_RDY_INT_EN 						0x00000004
#define MSC_CTRL_RDY_INT_DIS						0x00000008
#define MSC_CTRL_EN 								0x00000010 
#define MSC_CTRL_DIS								0x00000020
#define MSC_CTRL_ROLL_EN							0x00000040
#define MSC_CTRL_ROLL_DIS							0x00000080
#define MSC_CTRL_MIDDLE_EN							0x00000100
#define MSC_CTRL_MIDDLE_DIS 						0x00000200
#define MSC_CTRL_ROLL_RESTR_SET 					0x00000400
#define MSC_CTRL_ROLL_GOSTOP_EN 					0x00000800
#define MSC_CTRL_ROLL_GOSTOP_DIS					0x00001000
#define MSC_CTRL_MIDDLE_GOSTOP_EN					0x00002000
#define MSC_CTRL_MIDDLE_GOSTOP_DIS					0x00004000
#define MSC_CTRL_ROLL_INT_EN						0x00008000
#define MSC_CTRL_ROLL_INT_DIS						0x00010000
#define MSC_CTRL_MIDDLE_INT_EN						0x00020000
#define MSC_CTRL_MIDDLE_INT_DIS 					0x00040000
#define MSC_CTRL_CON_EN                         	0x00080000
#define MSC_CTRL_CON_DIS                        	0x00100000
#define MSC_CTRL_INPAHT_EN                         	0x00200000
#define MSC_CTRL_INPAHT_DIS                        	0x00400000

#define MSC_CTRL_ALL								0x0007FFFF


extern void IO_CKC_EnableBUS_MSCL(M2M_TYPE type);
extern void IO_CKC_DisableBUS_MSCL(M2M_TYPE type);
extern void M2M_Scaler_SW_Reset(M2M_TYPE type);
extern void MEMSCL_SET_INTERRUPT_INIT(M2M_TYPE type);
extern void MEMSCL_SET_INTERRUPT(M2M_TYPE type, unsigned int uiFlag);


extern void MEM_SCALER_SRC_ADDR(unsigned int uiMSC_SYaddr, unsigned int uiMSC_SUaddr, unsigned int uiMSC_SVaddr);
extern void MEM_SCALER_DST_ADDR(unsigned int uiMSC_DYaddr, unsigned int uiMSC_DUaddr, unsigned int uiMSC_DVaddr);
extern void MEM_SCALER_SRC_SIZE(unsigned int uiSRC_HSize,unsigned int uiSRC_VSize);
extern void MEM_SCALER_DST_SIZE(unsigned int uiDST_HSize,unsigned int uiDST_VSize);
extern void MEM_SCALER_SRC_OFFSET_YC(unsigned int uiOFFSET_Y,unsigned int uiOFFSET_C );
extern void MEM_SCALER_DST_OFFSET_YC(unsigned int uiOFFSET_Y, unsigned int uiOFFSET_C );
extern void MEM_SCALER_SRC_CONFIG(unsigned int uiFlag, unsigned int uiInterleaved_mode, unsigned int uiInterleaved_type, unsigned int uiType);
extern void MEM_SCALER_DST_CONFIG(unsigned int uiFlag, unsigned int uiCbCrWriting_mode, unsigned int uiWait, unsigned int uiReady, 
											unsigned int uiInterleaved_mode, unsigned int uiPath, unsigned int uiInterleaved_type, unsigned int uiType);
extern void MEM_SCALER_RATIO(unsigned int uiHRatio,unsigned int uiVRatio );
extern void MEM_SCALER_CTRL(unsigned int uiType);
extern void MEM_SCALER_RM_CNT(unsigned int uiRollAddr, unsigned int uiMidAddr );
extern void MEM_SCALER_CURRENT_ROLL_CNT(unsigned int uiCRollAddr );
extern int MEM_SCALER_POLLING_CHECK(void);
extern int MEM_SCALER_POLLING_CHECK_External(void);

extern void MEM_SCALER1_SRC_ADDR(unsigned int uiMSC_SYaddr, unsigned int uiMSC_SUaddr, unsigned int uiMSC_SVaddr);
extern void MEM_SCALER1_DST_ADDR(unsigned int uiMSC_DYaddr, unsigned int uiMSC_DUaddr, unsigned int uiMSC_DVaddr);
extern void MEM_SCALER1_SRC_SIZE(unsigned int uiSRC_HSize,unsigned int uiSRC_VSize);
extern void MEM_SCALER1_DST_SIZE(unsigned int uiDST_HSize,unsigned int uiDST_VSize);
extern void MEM_SCALER1_SRC_OFFSET_YC(unsigned int uiOFFSET_Y,unsigned int uiOFFSET_C );
extern void MEM_SCALER1_DST_OFFSET_YC(unsigned int uiOFFSET_Y, unsigned int uiOFFSET_C );
extern void MEM_SCALER1_SRC_CONFIG(unsigned int uiFlag, unsigned int uiInterleaved_mode, unsigned int uiInterleaved_type, unsigned int uiType);
extern void MEM_SCALER1_SRC_CONFIG1(unsigned int uiFlag, unsigned int uiARGB, unsigned int uiImode, unsigned int uiOmode, unsigned int uiMSBF);
extern void MEM_SCALER1_DST_CONFIG(unsigned int uiFlag, unsigned int uiCbCrWriting_mode, unsigned int uiWait, unsigned int uiReady, 
												unsigned int uiInterleaved_mode, unsigned int uiPath, unsigned int uiInterleaved_type, unsigned int uiType);
extern void MEM_SCALER1_RATIO(unsigned int uiHRatio,unsigned int uiVRatio );
extern void MEM_SCALER1_CTRL(unsigned int uiType);
extern void MEM_SCALER1_RM_CNT(unsigned int uiRollAddr, unsigned int uiMidAddr );
extern void MEM_SCALER1_CURRENT_ROLL_CNT(unsigned int uiCRollAddr );
extern int MEM_SCALER1_POLLING_CHECK(void);
extern int MEM_SCALER1_POLLING_CHECK_External(void);
#define M2M_WAIT_MAX	0xFFFFFFF
extern unsigned int DEV_M2M_Wait_signal_disable(char scaler);

extern void scale_get_address(SCALER_DATA_FM format, unsigned int base_Yaddr, unsigned int  src_imgx, unsigned int  src_imgy, unsigned int start_x, unsigned int start_y,
							unsigned int* Y, unsigned int* U,unsigned int* V);
extern char scaler_image_converter(SCALER_DATA_FM UiFormat, unsigned int *DrvierForamt, int *bpp);

