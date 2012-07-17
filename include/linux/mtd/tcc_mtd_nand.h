#ifndef __TCC_MTD_IO_H
#define __TCC_MTD_IO_H



#if defined(CONFIG_ARCH_TCC93XX)
#define TCC93XX
#elif defined(CONFIG_ARCH_TCC88XX)
#define TCC88XX
#else
#define TCC92XX
#endif

//*****************************************************************************
// Define Dev Board
//*****************************************************************************
//XXX
#if defined(CONFIG_MACH_TCC9200S) || defined(CONFIG_MACH_TCC9200S_SPACEY)
//#define TCC9200S_BOARD			// TCC9200S ND_RDY: GPIO_B31
#define TCC89_92_BOARD		// TCC89/91/9200 ND_RDY: GPIO_B28
#elif defined(CONFIG_MACH_TCC9200) || defined(CONFIG_MACH_TCC9201) || defined(CONFIG_MACH_TCC8900)
#define TCC89_92_BOARD		// TCC89/91/9200 ND_RDY: GPIO_B28
#elif defined(CONFIG_MACH_TCC9300)
#define TCC93D_93M_BOARD
#elif defined(CONFIG_MACH_TCC9300ST)
#define TCC930X_STB_BOARD
#elif defined(CONFIG_MACH_TCC9300CM)
#define TCC89D_93D_BOARD
#elif defined(CONFIG_MACH_M801_88) || defined(CONFIG_MACH_M803)
#define TCC8803_M801_BOARD
#elif defined(CONFIG_MACH_TCC8800) || defined(CONFIG_MACH_TCC8800ST)
#define TCC88D_88M_BOARD
#else
#error Board Definetion
#endif


#if defined(TCC9200S_BOARD)
#define TCC_NAND_WP_B22
#define TCC_NAND_RDY_B31
#undef  TCC_NAND_WP_B31
#undef  TCC_NAND_RDY_B28
#elif defined(TCC89_92_BOARD)
#define TCC_NAND_WP_B31
#define TCC_NAND_RDY_B28
#undef  TCC_NAND_WP_B22
#undef  TCC_NAND_RDY_B31
#elif defined(TCC93D_93M_BOARD)
#define TCC_NAND_RDY_B29
#define TCC_NAND_WP_A19
#elif defined(TCC930X_STB_BOARD)
#define TCC_NAND_RDY_B29
#define TCC_NAND_WP_B15
#elif defined(TCC89D_93D_BOARD)
#define TCC_NAND_RDY_B29
#define TCC_NAND_WP_B28
#elif defined(TCC88D_88M_BOARD) || defined(TCC8803_M801_BOARD)
#define TCC_NAND_RDY_B29
#define TCC_NAND_WP_B27
#else
#error Board Definetion
#endif


#if defined(TCC92XX)
#define TCC_MTD_DMA_ACCESS
//#define __USE_MTD_NAND_ISR__
#elif defined(TCC93XX)
#define TCC_MTD_DMA_ACCESS
//#define __USE_MTD_NAND_ISR__
#elif defined(TCC88XX)
#define TCC_MTD_DMA_ACCESS
//#define __USE_MTD_NAND_ISR__
#endif

#define __USE_MTD_EIL__

#define MTD_NAND_IRQ_NFC			IRQ_NFC

//**********************************************************************
//*		Define ECC Algorithm for ChipSet
//**********************************************************************
#if defined(TCC77X) || defined(TCC87XX) || defined(TCC82XX) || defined(TCC78X) || defined(TCC83XX) || defined(TCC79XX)
#define ECC_TYPE_RS
#elif defined(TCC81XX) || defined(TCC80XX) || defined(TCC92XX) || defined(TCC89XX) || defined(TCC93XX)|| defined(TCC88XX)
#define ECC_TYPE_BCH
#endif

//===================================================================
//
//		DRIVER SIGNATURE
//
//===================================================================
#define SIGBYAHONG				'S','I','G','B','Y','A','H','O','N','G','_'

#define TCC_MTD_IO_SIGNATURE	'T','C','C','_','M','T','D','_','I','O','_'

#if defined(_WINCE_)
#define SIGN_OS					'W','I','N','C','E','_'
#elif defined(_LINUX_)
#define SIGN_OS					'L','I','N','U','X','_'
#else
#define SIGN_OS					'N','U','_'
#endif

#if defined(TCC77X)
#define SIGN_CHIPSET			'T','C','C','7','7','X','X','_'
#elif defined(TCC78X)
#define SIGN_CHIPSET			'T','C','C','7','8','X','X','_'
#elif defined(TCC79X)
#define SIGN_CHIPSET			'T','C','C','7','9','X','X','_'
#elif defined(TCC81XX)
#define SIGN_CHIPSET			'T','C','C','8','1','X','X','_'
#elif defined(TCC82XX)
#define SIGN_CHIPSET			'T','C','C','8','2','X','X','_'
#elif defined(TCC83XX)
#define SIGN_CHIPSET			'T','C','C','8','3','X','X','_'
#elif defined(TCC87XX)
#define SIGN_CHIPSET			'T','C','C','8','7','X','X','_'
#elif defined(TCC88XX)
#define SIGN_CHIPSET			'T','C','C','8','8','X','X','_'
#elif defined(TCC89XX)
#define SIGN_CHIPSET			'T','C','C','8','9','X','X','_'
#elif defined(TCC91XX)
#define SIGN_CHIPSET			'T','C','C','9','1','X','X','_'
#elif defined(TCC92XX)
#define SIGN_CHIPSET			'T','C','C','9','2','X','X','_'
#elif defined(TCC93XX)
#define SIGN_CHIPSET			'T','C','C','9','3','X','X','_'
#else
#error
#endif

//*****************************************************************************
//*
//*
//*                       [ General DEFINE & TYPEDEF ]
//*
//*
//*****************************************************************************
#ifndef DISABLE
#define DISABLE					0
#endif
#ifndef ENABLE
#define	ENABLE					1
#endif
#ifndef FALSE
#define FALSE           		0
#endif
#ifndef TRUE
#define TRUE            		1
#endif
#ifndef NULL
#define NULL            		0
#endif

#if defined(_LINUX_)
#ifndef _U8_
#define _U8_
typedef unsigned char			U8;
#endif
#ifndef _U16_
#define _U16_
typedef unsigned short int		U16;
#endif
#ifndef _U32_
#define _U32_
typedef unsigned int			U32;
#endif
#ifndef _BOOL_
#define _BOOL_
typedef unsigned int			BOOL;
#endif
#else
#ifndef U8
typedef unsigned char			U8;
#endif
#ifndef U16
typedef unsigned short int		U16;
#endif
#ifndef U32
typedef unsigned int			U32;
#endif
#ifndef _BOOL_
typedef unsigned int			BOOL;
#endif
#endif

#ifndef DWORD_BYTE
typedef	union {
	unsigned long		DWORD;
	unsigned short int	WORD[2];
	unsigned char		BYTE[4];
} DWORD_BYTE;
#endif

#ifndef BITSET
#define	BITSET(X, MASK)				( (X) |= (U32)(MASK) )
#endif
#ifndef BITSCLR
#define	BITSCLR(X, SMASK, CMASK)	( (X) = ((((U32)(X)) | ((U32)(SMASK))) & ~((U32)(CMASK))) )
#endif
#ifndef BITCSET
#define	BITCSET(X, CMASK, SMASK)	( (X) = ((((U32)(X)) & ~((U32)(CMASK))) | ((U32)(SMASK))) )
#endif
#ifndef BITCLR
#define	BITCLR(X, MASK)				( (X) &= ~((U32)(MASK)) )
#endif
#ifndef BITXOR
#define	BITXOR(X, MASK)				( (X) ^= (U32)(MASK) )
#endif
#ifndef ISZERO
#define	ISZERO(X, MASK)				(  ! (((U32)(X)) & ((U32)(MASK))) )
#endif
#ifndef BYTE_OF
#define	BYTE_OF(X)					( *(volatile unsigned char *)(&(X)) )
#endif
#ifndef HWORD_OF
#define	HWORD_OF(X)					( *(volatile unsigned short *)(&(X)) )
#endif
#ifndef WORD_OF
#define	WORD_OF(X)					( *(volatile unsigned int *)(&(X)) )
#endif

//**********************************************************************
//*		Define IRQ
//**********************************************************************
#define MTD_IRQ_NFC			IRQ_NFC

//*****************************************************************************
//*
//*
//*                         [ ERROR CODE ENUMERATION ]
//*
//*
//*****************************************************************************
#ifndef SUCCESS
#define SUCCESS		0
#endif

typedef enum
{
	ERR_TCC_MTD_IO_FAILED_GET_DEVICE_INFO = 0xC100000,		//0xC100000
	ERR_TCC_MTD_IO_FAILED_CORRECTION_SLC_ECC,				//0xC100001
	ERR_TCC_MTD_IO_FAILED_CORRECTION_MLC_ECC,				//0xC100002
	ERR_TCC_MTD_IO_FAILED_GET_SHIFT_FACTOR_FOR_MULTIPLY,	//0xC100003
	ERR_TCC_MTD_IO_FAILED_GET_FACTORY_BAD_MARK_OF_PBLOCK,	//0xC100004
	ERR_TCC_MTD_IO_READ_STATUS_GENERAL_FAIL,				//0xC100005
	ERR_TCC_MTD_IO_TIME_OUT_READ_STATUS,					//0xC100006
	ERR_TCC_MTD_IO_WRONG_PARAMETER,							//0xC100007
	ERR_TCC_MTD_IO_WRONG_PARAMETER_ROW_COL_ADDRESS,			//0xC100008
	ERR_TCC_MTD_IO_NOT_READY_DEVICE_IO,						//0xC100009
	ERR_TCC_MTD_IO_OVER_BLOCK_ADDRESS,						//0xC10000A
	ERR_TCC_MTD_IO_FAILED_WRITE,							//0xC10000B
	ERR_TCC_MTD_IO_FAILED_READ,								//0xC10000C
	ERR_TCC_MTD_IO_FAILED_NO_GLUE_DRV,						//0xC10000D
	ERR_TCC_MTD_IO_ATTEMPT_REWRITE,
	ERR_TCC_MTD_IO_ATTEMPT_REREAD
} TCC_MTD_IO_ERROR;

//*****************************************************************************
//*
//*
//*                          [ INTERNAL DEFINATION ]
//*
//*
//*****************************************************************************

/* Maker Information of supported NANDFLASH */
enum SUPPORT_MAKER_NAND
{
	SAMSUNG = 0,
	TOSHIBA,
	HYNIX,
	ST,
	MICRON,
	INTEL,
	MAX_SUPPORT_MAKER_NAND
};

enum SUPPORT_MAKER_LBA_NAND
{
	TOSHIBA_LBA = 0,
	MAX_SUPPORT_MAKER_LBA_NAND
};

#define SAMSUNG_NAND_MAKER_ID					0xEC
#define TOSHIBA_NAND_MAKER_ID					0x98
#define HYNIX_NAND_MAKER_ID						0xAD
#define ST_NAND_MAKER_ID						0x20
#define MICRON_NAND_MAKER_ID					0x2C
#define INTEL_NAND_MAKER_ID						0x89

#define MAX_SUPPORT_SAMSUNG_NAND				32
#define MAX_SUPPORT_TOSHIBA_NAND				18
#define MAX_SUPPORT_HYNIX_NAND					31
#define MAX_SUPPORT_ST_NAND						15
#define MAX_SUPPORT_MICRON_NAND					16
#define MAX_SUPPORT_INTEL_NAND					 1

/* LBA NAND FLASH */
#define MAX_SUPPORT_TOSHIBA_LBA_NAND			3

/* Media Attribute */
#define A_08BIT									0x00000001		// 8Bit BUS
#define A_BIG									0x00000002		// Big Page Size
#define A_SMALL									0x00000004		// Small Page Size
#define A_16BIT									0x00000008		// 16Bit BUS
#define A_SLC									0x00000010		// Single Layer Cell
#define A_MLC									0x00000020		// Multi Layer Cell
#define A_MLC_8BIT								0x00000040		// 8Bit MLC ECC
#define A_MLC_12BIT								0x00000080		// 12Bit MLC ECC
#define A_MLC_16BIT								0x00000100		// 16Bit MLC ECC
#define A_MLC_24BIT								0x00000200		// 16Bit MLC ECC

#define A_PARALLEL								0x00000400		// Parallel Composition
#define A_DATA_WITDH_08BIT						0x00000800		// Data 8Bit Bus
#define A_DATA_WITDH_16BIT						0x00001000		// Data 16Bit Bus

#define S_NOR									0x00010000		// NORMAL [ ReadID, Reset, Read, Page Program, Block Erase, Read Status ]
#define S_CB									0x00020000		// COPY BACK
#define S_CP									0x00040000 		// CACHE PROGRAM
#define S_MP									0x00080000		// MULTI PLANE
#define S_MP2									0x00100000		// MULTI PLANE
#define S_MCP									0x00200000		// MULTI PLANE CACHE 
#define S_IL									0x00400000		// INTER LEAVE
#define S_EIL									0x00800000		// EXTERNAL INTER LEAVE 
#define S_EPW									0x01000000		// EXCEPT PARTIAL WRITE
#define S_NPW									0x02000000		// Not-Support PARTIAL WRITE
#define S_MP_READ								0x04000000		// MULTI PLANE READ
#define S_LBA									0x08000000		// LBA NAND
#define S_RAND_IO								0x10000000		// Random Data In/Out
#define S_EB									0x20000000		// Extend Block 

#define	ECC_OFF                     			0
#define	ECC_ON                      			1
#define SLC_ECC_TYPE							0x0001			// 10 Byte
#define MLC_ECC_4BIT_TYPE						0x0002			// BCH: 7 Byte( 52 bit)	turn: 2, byte remain: 3, register mask: 0x0F
#define MLC_ECC_8BIT_TYPE						0x0004			// BCH:13 Byte (104 bit)	turn: 4, byte remain: 1, register mask: 0xFF
#define MLC_ECC_12BIT_TYPE						0x0010			// BCH:20 Byte (156 bit)	turn: 5, byte remain: 4, register mask: 0x0F
#define MLC_ECC_14BIT_TYPE						0x0020			// BCH:23 Byte (184 bit)	turn: 6, byte remain: 3, register mask: 0xFF
#define MLC_ECC_16BIT_TYPE						0x0040			// BCH:26 Byte (207 bit)turn: 7, byte remain: 2, register mask: 0x7F
#define MLC_ECC_24BIT_TYPE						0x0080			// TCC93X ( Writing After Porting )

#define TYPE_ECC_FOR_1BIT_SLC_NANDFLASH			SLC_ECC_TYPE
#define TYPE_ECC_FOR_4BIT_MLC_NANDFLASH			MLC_ECC_4BIT_TYPE
#define TYPE_ECC_FOR_8BIT_MLC_NANDFLASH			MLC_ECC_12BIT_TYPE
#define TYPE_ECC_FOR_12BIT_MLC_NANDFLASH		MLC_ECC_12BIT_TYPE
#define TYPE_ECC_FOR_14BIT_MLC_NANDFLASH		MLC_ECC_14BIT_TYPE
#define TYPE_ECC_FOR_16BIT_MLC_NANDFLASH		MLC_ECC_16BIT_TYPE
#define TYPE_ECC_FOR_24BIT_MLC_NANDFLASH		MLC_ECC_24BIT_TYPE

#if defined(TCC89XX) || defined(TCC92XX)
#define ECC_SHIFT_DATASIZE						4
#elif defined(TCC93XX) || defined(TCC88XX)
#define ECC_SHIFT_DATASIZE						16
#endif

#define NAND_IO_SPARE_SIZE_BIG					16
#define NAND_IO_SPARE_SIZE_SMALL				8

#define PAGE_ECC_OFF							0
#define PAGE_ECC_ON								1

#define ECC_DECODE								0
#define ECC_ENCODE								1

#define NAND_MCU_ACCESS							0
#define NAND_DMA_ACCESS							1


#define TCC_MTD_IO_DATA_WITDH_8BIT				0
#define TCC_MTD_IO_DATA_WITDH_16BIT				1

#define TCC_MTD_IO_SERIAL_COMBINATION_MODE		0
#define TCC_MTD_IO_PARALLEL_COMBINATION_MODE	1

#define TCC_MTD_IO_STATUS_ENABLE				0x0001

#define TCC_MTD_IO_STATUS_FAIL_CS0_SERIAL		0x0001
#define TCC_MTD_IO_STATUS_FAIL_CS0_PARALLEL		0x0010
#define TCC_MTD_IO_STATUS_FAIL_CS1_PARALLEL		0x0100

#define TCC_MTD_IO_WRITE_MODE					0
#define TCC_MTD_IO_READ_MODE					1

#define TNFTL_READ_SPARE_ON						0
#define TNFTL_READ_SPARE_OFF					1

#define NAND_IO_DMA_WRITE						0x0001
#define NAND_IO_DMA_READ						0x0002

#define TCC_MTD_CMD_CLEAR						0x0000
#define TCC_MTD_CMD_PROGRAM_START				0x0001
#define TCC_MTD_CMD_READ_PAGE					0x0002
#define TCC_MTD_CMD_READ_SPARE					0x0003
#define TCC_MTD_CMD_READ_SPARE_SEQ				0x0004
#define TCC_MTD_CMD_READ_ID_1ST					0x0005
#define TCC_MTD_CMD_READ_ID_2ND					0x0006
#define TCC_MTD_CMD_READ_ID_3RD					0x0007
#define TCC_MTD_CMD_READ_ID_4TH					0x0008
#define TCC_MTD_CMD_READSTATUS					0x0010
#define TCC_MTD_CMD_BLOCKERASE					0x0020

#define TCC_MTD_MAX_LPT_SIZE					16384		
#define TCC_MTD_MAX_SUPPORT_NAND_IO_PAGE_SIZE					16384		
#define TCC_MTD_MAX_SUPPORT_NAND_IO_SPARE_SIZE					1024			

typedef struct __tag_TNFTL_INIT_INFO
{
	U8	SerialNumber[64];
	U8	BMPSignature[4];
	U32	ExtendedPartitionNo; 				// Extended Partition Number
	U32	ExtendedPartitionSectorSize[10];
	U32 ROAreaSize;
	U32 ROAreaStPB;
} TNFTL_INIT_INFO;

#if defined(TCC89XX) || defined(TCC92XX)
#define NAND_SB_BOOT_PAGE_SIZE					260
#define NAND_SB_BOOT_PAGE_ECC_SIZE				288
#elif defined(TCC93XX) || defined(TCC88XX)
#define NAND_SB_BOOT_PAGE_SIZE					260
#define NAND_SB_BOOT_PAGE_ECC_SIZE				304
#endif


#define GMC_NAND_NOMAL_BOOT_TYPE				0x54435342		// TCSB
#define GMC_NAND_SECURE_BOOT_TYPE				0x54435353		// TCSS

typedef struct __tag_TCC_MTD_IO_ECCSizeInfo
{
	unsigned int				EncodeFlag;
	unsigned int				DecodeFlag;
	unsigned int				ErrorNum;
	unsigned char				EccDataSize;		// Register Num
	unsigned char				*All_FF_512_ECC_Code;
} TCC_MTD_IO_ECC_INFO;

typedef struct __tag_TCC_MTD_IO_Cycle
{
	unsigned char				STP;
	unsigned char				PW;
	unsigned char				HLD;
	unsigned int				RegValue;
} TCC_MTD_IO_CYCLE;

typedef struct __tag_TCC_MTD_IO_DeviceCode
{
	unsigned short int			Code[6];			// Factory ID code
} TCC_MTD_IO_DEVID;

typedef struct __tag_TCC_MTD_IO_Feature
{
	TCC_MTD_IO_DEVID			DeviceID;			// Maker & Device ID Code
	unsigned short int  		PBpV;				// Physical all Block Number
	unsigned short int			BBpZ;				// Total Bad Block in one ZONE
	unsigned short int  		PpB;				// Page Number Per Block
	unsigned short int  		PageSize;			// Page Size
	unsigned short int  		SpareSize;			// Spare Size
	unsigned short int			ColCycle;			// Column Address Cycle
	unsigned short int			RowCycle;			// Row Address Cycle
	unsigned short int			WCtime;				// Write Cyclte time
	unsigned short int			WriteSTP;
	unsigned short int			WriteWP;
	unsigned short int			WriteHLD;
	unsigned short int			ReadSTP;
	unsigned short int			ReadPW;
	unsigned short int			ReadHLD;
	unsigned long int			MediaType;			// Chracters of NANDFLASH
} TCC_MTD_IO_FEATURE;

typedef struct __tag_TCC_MTD_IO_CmdBuf
{
	unsigned  int				CmdStatus;
	unsigned  int				ColAddr;
	unsigned  int				RowAddr;
	unsigned  char				ResStatus;
} TCC_MTD_IO_CMDBUF;

typedef struct __tag_TCC_MTD_IO_DevInfo
{
	TCC_MTD_IO_FEATURE			Feature;			// Feature of NANDFLASH
	TCC_MTD_IO_CMDBUF			CmdBuf;
	unsigned short int			IoStatus;			// IO Status
	unsigned short int			ChipNo;				// ChipSelect Number
	unsigned short int			CmdMask;			// Command Mask Bit
	unsigned short int			EccType;			// Type of ECC [ SLC , MLC4 ]
	unsigned short int			BytesPerSector;
	unsigned short int			EccDataSize;
	unsigned short int			EccWholeDataSize;
	unsigned short int 			ExtInterleaveUsable;

	unsigned short int			PPages;				// Total Partial Page [512Bytes]
	unsigned short int			UsablePPages;
	
	unsigned short int			ShiftPBpV;
	unsigned short int			ShiftPpB;
	unsigned short int			ShiftPageSize;
	unsigned short int			ShiftPPages;
	unsigned short int			ShiftBytesPerSector;
	unsigned short int			ShiftUsablePPages;
} TCC_MTD_IO_DEVINFO;

typedef struct __tag_TCC_MTD_IO_MakerInfo
{
	unsigned short int			MaxSupportNAND[MAX_SUPPORT_MAKER_NAND];
	unsigned short int			MakerID[MAX_SUPPORT_MAKER_NAND];
	TCC_MTD_IO_FEATURE*			DevInfo[MAX_SUPPORT_MAKER_NAND];
} TCC_MTD_IO_MAKERINFO;

//==============================================
// MTD GLUE structure
//==============================================
struct tcc_mtd_glue_info {
	unsigned char*	gTCC_MTD_PageBuffer;
	unsigned int 	gTCC_GoldenBadCnt[2];
	unsigned int**	gTCC_GoldenBadTable;
	unsigned long*	tcc_mtd_LPT;
	unsigned char	gTCC_MTD_CurrCS;
	unsigned char*	tcc_mtd_LPT_CS;
	unsigned int	gMTDStBlkAddr;
	unsigned int 	gMTDEdBlkAddr;	
};

//==============================================
// MTD structure for NAND controller
//==============================================
struct tcc_nand_info {
	struct mtd_info mtd;
	struct mtd_partition *parts;
	struct nand_chip tcc_nand;
	struct device *dev;
	struct tcc_mtd_glue_info glue_info;
};

typedef struct _tag_TCC_MTD_GLUE_DRV_T {
	TCC_MTD_IO_ERROR (*MTDAreaScanInfo)(struct tcc_nand_info *info, TCC_MTD_IO_DEVINFO *nDevInfo, U32 nMediaNum, U32 *rStartOffSet, U32 *rPartSize, U32 nBootECCSize );
	TCC_MTD_IO_ERROR (*locGetPhyBlkAddr)( struct tcc_mtd_glue_info *glue_info, TCC_MTD_IO_DEVINFO *nDevInfo, U32 nMTDPageAddr, U32 *rMTDPhyPageAddr, U32 *rChipNo  );
	//unsigned long (*GetStartBlkAddr)(void);
	//unsigned long (*GetEndBlkAddr)(void);
	//unsigned long (*GetLPT)(unsigned int index);
} TCC_MTD_GLUE_DRV_T;

#if defined(_LINUX_)
#if defined(TCC89XX) || defined(TCC92XX)
#define	NAND_IO_HwCMD_PA							*(volatile unsigned long*)0xF05B0000		//( gNAND_IO_pHwND->CMD )
#define	NAND_IO_HwLADR_PA							*(volatile unsigned long*)0xF05B0004		//( gNAND_IO_pHwND->LADR )
#define	NAND_IO_HwDATA_PA							*(volatile unsigned long*)0xF05B0010		//( gNAND_IO_pHwND->WDATA.D32 )
#define	NAND_IO_HwLDATA_PA							*(volatile unsigned long*)0xF05B0020		//( gNAND_IO_pHwND->LDATA )
#define	NAND_IO_HwSDATA_PA							*(volatile unsigned long*)0xF05B0040		//(/*( gNAND_IO_DataBusType == NAND_IO_MEM_BUS ) ? gNAND_IO_pHwND->WDATA.D8 : */gNAND_IO_pHwND->SDATA.D32 )
#define	NAND_IO_HwSADR_PA							*(volatile unsigned long*)0xF05B000C		//( gNAND_IO_pHwND->SADR )
#elif defined(TCC88XX)
#define	NAND_IO_HwCMD_PA							*(volatile unsigned long*)0xF05B0000		//( gNAND_IO_pHwND->CMD )
#define	NAND_IO_HwLADR_PA							*(volatile unsigned long*)0xF05B0004		//( gNAND_IO_pHwND->LADR )
#define	NAND_IO_HwDATA_PA							*(volatile unsigned long*)0xF05B0010		//( gNAND_IO_pHwND->WDATA.D32 )
#define	NAND_IO_HwLDATA_PA							*(volatile unsigned long*)0xF05B0020		//( gNAND_IO_pHwND->LDATA )
#define	NAND_IO_HwSDATA_PA							*(volatile unsigned long*)0xF05B000C		//(( gNAND_IO_DataBusType == NAND_IO_MEM_BUS ) ? gNAND_IO_pHwND->WDATA.D8 : gNAND_IO_pHwND->SDATA.D32 )
#define	NAND_IO_HwSADR_PA							*(volatile unsigned long*)0xF05B0008		//( gNAND_IO_pHwND->SADR )
#elif defined(TCC93XX)
#define	NAND_IO_HwCMD_PA							*(volatile unsigned long*)0xF0050000		//( gNAND_IO_pHwND->CMD )
#define	NAND_IO_HwLADR_PA							*(volatile unsigned long*)0xF0050004		//( gNAND_IO_pHwND->LADR )
#define	NAND_IO_HwDATA_PA							*(volatile unsigned long*)0xF0050010		//( gNAND_IO_pHwND->WDATA.D32 )
#if defined(TCC_MTD_DMA_ACCESS) || defined(__USE_MTD_NAND_ISR__)
#define	NAND_IO_HwLDATA_PA							*(volatile unsigned long*)0xB0050020		//( gNAND_IO_pHwND->LDATA )
#else
#define	NAND_IO_HwLDATA_PA							*(volatile unsigned long*)0xF0050020		//( gNAND_IO_pHwND->LDATA )
#endif 
#define	NAND_IO_HwSDATA_PA							*(volatile unsigned long*)0xF005000C		//(( gNAND_IO_DataBusType == NAND_IO_MEM_BUS ) ? gNAND_IO_pHwND->WDATA.D8 : gNAND_IO_pHwND->SDATA.D32 )
#define	NAND_IO_HwSADR_PA							*(volatile unsigned long*)0xF0050008		//( gNAND_IO_pHwND->SADR )
#endif
#endif

#if defined(TCC_NAND_WP_B22)
#define NAND_IO_NFC_nWPEn							HwGPIOB->GPEN   //Arena2
#define	TCC_MTD_IO_NFC_nWP							HwGPIOB->GPDAT
#define	TCC_MTD_IO_NFC_nWPBit						Hw22
#elif defined(TCC_NAND_WP_B31)
#define TCC_MTD_IO_NFC_nWP							HwGPIOB->GPDAT
#define TCC_MTD_IO_NFC_nWPBit						Hw31
#elif defined(TCC_NAND_WP_B28)
#define	TCC_MTD_IO_NFC_nWP							HwGPIOB->GPDAT
#define	TCC_MTD_IO_NFC_nWPBit						Hw28
#elif defined(TCC_NAND_WP_B15)
#define TCC_MTD_IO_NFC_nWP                          HwGPIOB->GPDAT
#define TCC_MTD_IO_NFC_nWPBit                       Hw15        
#elif defined(TCC_NAND_WP_B27)
#define	TCC_MTD_IO_NFC_nWP							HwGPIOB->GPDAT
#define	TCC_MTD_IO_NFC_nWPBit						Hw27
#elif defined(TCC_NAND_WP_A19)					
#define	TCC_MTD_IO_NFC_nWP							HwGPIOA->GPDAT
#define	TCC_MTD_IO_NFC_nWPBit						Hw19
#else
#err
#endif

#define TCC_MTD_IO_PORT_NAND_ON_CS0					0x0001
#define TCC_MTD_IO_PORT_NAND_ON_CS1					0x0002
#define TCC_MTD_IO_PORT_NAND_ON_CS2					0x0004
#define TCC_MTD_IO_PORT_NAND_ON_CS3					0x0008
#define TCC_MTD_IO_PORT_DATA_WITDH_8BIT				0x0010
#define TCC_MTD_IO_PORT_DATA_WITDH_16BIT			0x0020

#define	HwCHCTRL_CONT_C				Hw15				// DMA transfer begins from C_SADR/C_DADR Address. It must be used after the former transfer has been executed, so that C_SADR and C_DADR contain a meaningful value.
#define	HwCHCTRL_CONT_ST			(0)					// DMA trnaster begins from ST_SADR/ST_DADR Address
#define	HwCHCTRL_DTM_EN				Hw14				// Differential Transfer Mode Enable
#define	HwCHCTRL_DTM_ON				Hw14				// Differential Transfer Mode Enable
#define	HwCHCTRL_DTM_OFF			(0)					// Differential Transfer Mode Disable
#define	HwCHCTRL_SYNC_ON			Hw13				// Synchronize HardWare Request
#define	HwCHCTRL_SYNC_OFF			(0)					// Do not Synchronize HardWare Request
#define HwCHCTRL_SYNC_EN			Hw13				// Synchronize Hardware Request
#define	HwCHCTRL_HRD_W				Hw12				// ACK/EOT signals are issued When DMA-Write Operation
#define	HwCHCTRL_LOCK_EN			Hw11				// DMA transfer executed with lock transfer
#define	HwCHCTRL_BST_NOARB			Hw10				// DMA transfer executed with no arbitration(burst operation)
#define	HwCHCTRL_HRD_WR				Hw12				// ACK/EOT signals are issued When DMA-Write Operation
#define	HwCHCTRL_HRD_RD				(0)					// ACK/EOT signals are issued When DMA-Read Operation
#define	HwCHCTRL_LOCK_ON			Hw11				// DMA transfer executed with lock transfer
#define	HwCHCTRL_LOCK_OFF			(0)					//
#define	HwCHCTRL_BST_BURST			Hw10				// DMA transfer executed with no arbitration(burst operation)
#define	HwCHCTRL_BST_ARB			(0)					// DMA transfer executed wth arbitration
#define	HwCHCTRL_TYPE_SINGE			(0)					// SINGLE transfer with edge-triggered detection
#define	HwCHCTRL_TYPE_HW			Hw8					// HW Transfer
#define	HwCHCTRL_TYPE_SW			Hw9					// SW transfer
#define	HwCHCTRL_TYPE_SINGL			(Hw9|Hw8)			// SINGLE transfer with level-triggered detection
#define	HwCHCTRL_TYPE_SL			(Hw9|Hw8)			// SINGLE transfer with level-triggered detection
#define HwCHCTRL_TYPE_SE			HwZERO				// SINGLE transfer with edge-triggered detection

#define	HwCHCTRL_BSIZE_1			(0)					// 1 Burst transfer consists of 1 read or write cycle
#define	HwCHCTRL_BSIZE_2			Hw6					// 1 Burst transfer consists of 2 read or write cycles
#define	HwCHCTRL_BSIZE_4			Hw7					// 1 Burst transfer consists of 4 read or write cycles
#define	HwCHCTRL_BSIZE_8			(Hw6|Hw7)			// 1 Burst transfer consists of 8 read or write cycles

#define	HwCHCTRL_WSIZE_8			(0)					// Each cycle read or write 8bit data
#define	HwCHCTRL_WSIZE_16			Hw4					// Each cycle read or write 16bit data
#define	HwCHCTRL_WSIZE_32			Hw5					// Each cycle read or write 32bit data

#define	HwCHCTRL_FLAG				Hw3					// Clears FLAG to 0
#define	HwCHCTRL_IEN_ON				Hw2					// At the same time the FLAG goes to 1, DMA interrupt request is generated
#define HwCHCTRL_IEN_EN				Hw2					// At the same time the FLAG goes to 1, DMA interrupt request is generated
#define	HwCHCTRL_IEN_OFF			~Hw2				//
#define	HwCHCTRL_REP_EN				Hw1					// The DMA channel remains enabled
#define	HwCHCTRL_REP_DIS			~Hw1				// After all of hop transfer has executed, the DMA channel is disabled
#define	HwCHCTRL_EN_ON				Hw0					// DMA channel is Enabled
#define	HwCHCTRL_EN_OFF				~Hw0				// DMA channel is terminated and disabled/*}}}*/
#define HwCHCTRL_EN_EN				Hw0					// DMA channel is enabled. If software type transfer is selected, this bit generates DMA request directly, or if hardware type transfer is used, the selected interrupt request flag generate DMA request

TCC_MTD_IO_ERROR TCC_MTD_IO_locReadPage( TCC_MTD_IO_DEVINFO *nDevInfo, U32 nPageAddr,
										 U16 nStartPPage, U16 nReadPPSize,
										 U8 *nPageBuffer, U8 *nSpareBuffer, int nEccOnOff );

#endif
