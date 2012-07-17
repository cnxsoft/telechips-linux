/**************************************************************************************
 *
 * FILE NAME   : tcc_otg_regs.h
 * DESCRIPTION : OTG registers
 *
 * ====================================================================================
 *
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 * ====================================================================================
 *
 * FILE HISTORY:
 * 	Date: 2009.03.05	Start source coding
 *
 **************************************************************************************/
#ifndef __TCC_OTG_REGS_H__
#define __TCC_OTG_REGS_H__

#ifndef Hw0
#define	Hw31								0x80000000
#define	Hw30								0x40000000
#define	Hw29								0x20000000
#define	Hw28								0x10000000
#define	Hw27								0x08000000
#define	Hw26								0x04000000
#define	Hw25								0x02000000
#define	Hw24								0x01000000
#define	Hw23								0x00800000
#define	Hw22								0x00400000
#define	Hw21								0x00200000
#define	Hw20								0x00100000
#define	Hw19								0x00080000
#define	Hw18								0x00040000
#define	Hw17								0x00020000
#define	Hw16								0x00010000
#define	Hw15								0x00008000
#define	Hw14								0x00004000
#define	Hw13								0x00002000
#define	Hw12								0x00001000
#define	Hw11								0x00000800
#define	Hw10								0x00000400
#define	Hw9									0x00000200
#define	Hw8									0x00000100
#define	Hw7									0x00000080
#define	Hw6									0x00000040
#define	Hw5									0x00000020
#define	Hw4									0x00000010
#define	Hw3									0x00000008
#define	Hw2									0x00000004
#define	Hw1									0x00000002
#define	Hw0									0x00000001
#endif

#define OTGCR_OTGAMAP(x)					(x<<2)		// 0~3
#define OTGCR_ENDS_little					0
#define OTGCR_ENDS_big						Hw1
#define OTGCR_ENDM_little					0
#define OTGCR_ENDM_big						Hw0

#define OTGID_ID_HOST						(~Hw9)
#define OTGID_ID_DEVICE						Hw9

//0000
#define GOTGCTL_BSesVld						Hw19
#define GOTGCTL_ASesVld						Hw18
#define GOTGCTL_DbncTime					Hw17		// 0:Long / 1:Short debounce time
#define GOTGCTL_ConIDSts					Hw16		// 0:A-Device mode / 1:B-Device mode
#define GOTGCTL_DevHNPEn					Hw11
#define GOTGCTL_HstSetHNPEn					Hw10
#define GOTGCTL_HNPReq						Hw9			// HNP request
#define GOTGCTL_HstNegScs					Hw8			// Host Negotiation Success
#define GOTGCTL_SesReq						Hw1			// Session Request
#define GOTGCTL_SesReqScs					Hw0			// Session Request Success

//0004
#define GOTGINT_DbnceDone					Hw19
#define GOTGINT_ADevTOUTChg					Hw18		// A-Device Timeout Change
#define GOTGINT_HstNegDet					Hw17		// Host Negotiation Detected
#define GOTGINT_HstNegSucStsChng			Hw9			// Host Negotiation Success Status Change
#define GOTGINT_SesReqSucStsChng			Hw8			// Session Request Success Status Change
#define GOTGINT_SesEndDet					Hw2			// Session End Detected

//0008
#define GAHBCFG_PTxFEmpLvl					Hw8			// Periodic TxFIFO Empty Level
#define GAHBCFG_PTxFEmpLvl_HALF				0
#define GAHBCFG_PTxFEmpLvl_COMPLETELY		Hw8
#define GAHBCFG_NPTxFEmpLvl					Hw7			// Non-Periodic TxFIFO Empty Level
#define GAHBCFG_NPTxFEmpLvl_HALF			0
#define GAHBCFG_NPTXFEmpLvl_COMPLETELY		Hw7
#define GAHBCFG_DMAEn						Hw5
#define GAHBCFG_HBstLen(x)					(x<<1)
#define GAHBCFG_HBstLen_SINGLE				GAHBCFG_HBstLen(0)
#define GAHBCFG_HBstLen_INCR				GAHBCFG_HBstLen(1)
#define GAHBCFG_HBstLen_INCR4				GAHBCFG_HBstLen(3)
#define GAHBCFG_HBstLen_INCR8				GAHBCFG_HBstLen(5)
#define GAHBCFG_HBstLen_INCR16				GAHBCFG_HBstLen(7)
#define GAHBCFG_GlblIntrMsk_MASK			0
#define GAHBCFG_GlblIntrMsk_UNMASK			Hw0

//000C
#define GUSBCFG_ForceDevMode				Hw30
#define GUSBCFG_ForceHstMode				Hw29
#define GUSBCFG_TermSelDLPulse_UTMI_TXVALID	0
#define GUSBCFG_TermSelDLPulse_UTMI_TERMSEL	Hw22
#define GUSBCFG_OtgI2CSel_UTMIFS			0
#define GUSBCFG_OtgI2CSel_I2C				Hw16
#define GUSBCFG_PhyLPwrClkSel_480MHz		0
#define GUSBCFG_PhyLPwrClkSel_48MHz			Hw15
#define GUSBCFG_USBTrdTim(x)				(x<<10)
#define GUSBCFG_USBTrdTim_16BIT_UTMIP		GUSBCFG_USBTrdTim(5)
#define GUSBCFG_USBTrdTim_8BIT_UTMIP		GUSBCFG_USBTrdTim(9)
#define GUSBCFG_HNPCap						Hw9			// HNP-Capable
#define GUSBCFG_SRPCap						Hw8			// SRP-Capable
#define GUSBCFG_PHYSel_USB20				0
#define GUSBCFG_PHYSel_USB11				Hw6
#define GUSBCFG_FSIntf_6PIN_UNIDIR			0
#define GUSBCFG_FSIntf_3PIN_BIDIR			Hw5
#define GUSBCFG_ULPI_UTMI_Sel_UTMIP			0
#define GUSBCFG_ULPI_UTMI_Sel_ULPI			Hw4
#define GUSBCFG_PHYIf_16BITS				Hw3
#define GUSBCFG_TOutCal(x)					(x)			// ~7

//0010
#define GRSTCTL_AHBIdle						Hw31
#define GRSTCTL_DMAReq						Hw30
#define GRSTCTL_TXFNum_MASK					(0x1F<<6)
#define GRSTCTL_TxFNum(x)					(x<<6)		// 0~15:flush Tx FIFO n, 0x10:flush all
#define GRSTCTL_TxFNum_Write(reg,x)			( reg = ( ((reg)&(~GRSTCTL_TXFNum_MASK)) | GRSTCTL_TxFNum(x) ) )
#define GRSTCTL_TxFFlsh						Hw5			// TxFIFO Flush
#define GRSTCTL_RxFFlsh						Hw4			// RxFIFO Flush
#define GRSTCTL_INTknQFlsh					Hw3			// IN Token Sequence Learning Queue Flush
#define GRSTCTL_FrmCntrRst					Hw2			// Host Frame Counter Reset
#define GRSTCTL_HSftRst						Hw1			// HClk Soft Reset
#define GRSTCTL_CSftRst						Hw0			// Core Soft Reset

//0014
#define GINTSTS_WkUpInt						Hw31
#define GINTSTS_SessReqInt					Hw30
#define GINTSTS_DisconnInt					Hw29
#define GINTSTS_ConIDStsChng				Hw28
#define GINTSTS_PTxFEmp						Hw26
#define GINTSTS_HChInt						Hw25
#define GINTSTS_PrtInt						Hw24
#define GINTSTS_FetSusp						Hw22		// Data Fetch Suspended
#define GINTSTS_incompP						Hw21		// Incomplete Periodic Transfer
#define GINTSTS_incompISOOUT				Hw21		// Incomplete isochronous OUT Transfer
#define GINTSTS_incompISOIN					Hw20		// Incomplete isochronous IN Transfer
#define GINTSTS_OEPInt_MASK					Hw19
#define GINTSTS_IEPInt_MASK					Hw18
#define GINTSTS_EPMis						Hw17
#define GINTSTS_EOPF						Hw15		// End of Periodic Frame Interrupt
#define GINTSTS_ISOOutDrop					Hw14		// Isochronous OUT Packet Dropped Interrupt
#define GINTSTS_EnumDone					Hw13		// Enumeration Done
#define GINTSTS_USBRst						Hw12		// USB Reset
#define GINTSTS_USBSusp						Hw11		// USB Suspend
#define GINTSTS_ErlySusp					Hw10		// Early Suspend
#define GINTSTS_GOUTNakEff					Hw7			// Global OUT NAK Effective
#define GINTSTS_GINNakEff					Hw6			// Global IN Non-periodic NAK Effective
#define GINTSTS_RxFLvl_MASK					Hw4			// RxFIFO Non-Empty
#define GINTSTS_Sof							Hw3
#define GINTSTS_OTGInt						Hw2
#define GINTSTS_ModeMis						Hw1
#define GINTSTS_CurMod						Hw0
#define GINTSTS_CurMod_HOST					Hw0
#define GINTSTS_CurMod_DEVICE				0

//0018
#define GINTMSK_WkUpIntMsk_UNMASK			Hw31
#define GINTMSK_SessReqIntMsk_UNMASK		Hw30
#define GINTMSK_DisconnIntMsk_UNMASK		Hw29
#define GINTMSK_ConIDStsChngMsk_UNMASK		Hw28
#define GINTMSK_PTxFEmpMsk_UNMASK			Hw26
#define GINTMSK_HChIntMsk_UNMASK			Hw25
#define GINTMSK_PrtIntMsk_UNMASK			Hw24
#define GINTMSK_FetSuspMsk_UNMASK			Hw22
#define GINTMSK_incompPMsk_UNMASK			Hw21
#define GINTMSK_incompISOOUTMsk_UNMASK		Hw21
#define GINTMSK_incompISOINMsk_UNMASK		Hw20
#define GINTMSK_OEPIntMsk_UNMASK			Hw19
#define GINTMSK_IEPIntMsk_UNMASK			Hw18
#define GINTMSK_EPMisMsk_UNMASK				Hw17
#define GINTMSK_EOPFMsk_UNMASK				Hw15
#define GINTMSK_ISOOutDropMsk_UNMASK		Hw14
#define GINTMSK_EnumDoneMsk_UNMASK			Hw13
#define GINTMSK_USBRstMsk_UNMASK			Hw12
#define GINTMSK_USBSuspMsk_UNMASK			Hw11
#define GINTMSK_ErlySuspMsk_UNMASK			Hw10
#define GINTMSK_GOUTNakEffMsk_UNMASK		Hw7
#define GINTMSK_GINNakEffMsk_UNMASK			Hw6
#define GINTMSK_RxFLvlMsk_UNMASK			Hw4
#define GINTMSK_SofMsk_UNMASK				Hw3
#define GINTMSK_OTGIntMsk_UNMASK			Hw2
#define GINTMSK_ModeMisMsk_UNMASK			Hw1

//0020
#define GRXSTSP_PktSts_MASK					(0xF<<17)
#define GRXSTSP_PktSts_Read(reg)			( (GRXSTSP_PktSts_MASK&(reg)) >> 17 )
#define GRXSTSP_DPID_MASK					(0x3<<15)
#define GRXSTSP_BCnt_MASK					(0x7FF<<4)
#define GRXSTSP_BCnt_Read(reg)				( (GRXSTSP_BCnt_MASK&(reg)) >> 4 )
#define GRXSTSP_ChNum_MASK					(0xF)
#define GRXSTSP_FN_MASK						(0xF<<21)
#define GRXSTSP_EPNum_MASK					(0xF)
#define GRXSTSP_EPNum_Read(reg)				(GRXSTSP_EPNum_MASK&(reg))
typedef struct _tag_GRXSTSP_DEV_T {
	unsigned int	EPNum	: 4;
	unsigned int	BCnt	: 11;
	unsigned int	DPID	: 2;
	unsigned int	PktSts	: 4;
	unsigned int	FN		: 4;
	unsigned int	rsvd	: 7;
} GRXSTSP_DEV_T, *PGRXSTSP_DEV_T;

//0028
#define GNPTXFSIZ_NPTxFDep(x)				(x<<16)		// 16~4160, Host: Non-periodic TxFIFO Depth
#define GNPTXFSIZ_NPTxFStAddr(x)			(x)			// Host: Non-periodic Transmit RAM Start Address
#define GNPTXFSIZ_INEPTxF0Dep(x)			(x<<16)		// 16~4160, Device: IN Endpoint TxFIFO 0 Depth
#define GNPTXFSIZ_INEPTxF0StAddr(x)			(x)			// Device: IN Endpoint FIFO0 Transmit RAM Start Address

//0044
#define GHWCFG1_epdir(ep,dir)				(dir<<(ep<<1))
#define GHWCFG1_epdir_MASK(ep)				GHWCFG1_epdir(ep,3)
#define GHWCFG1_epdir_Read(ep,reg)			( ((GHWCFG1_epdir_MASK(ep) & (reg)) >> (ep<<1) )

//0048
#define GHWCFG2_NumHstChnl_MASK				(0xF<<14)
#define GHWCFG2_NumHstChnl_Read(reg)		( (GHWCFG2_NumHstChnl_MASK&(reg)) >> 14 )
#define GHWCFG2_NumDevEps_MASK				(0xF<<10)
#define GHWCFG2_NumDevEps_Read(reg)			( (GHWCFG2_NumDevEps_MASK&(reg)) >> 10 )
#define GHWCFG2_FSPhyType_MASK				(0x3<<8)
#define GHWCFG2_HSPhyType_MASK				(0x3<<6)
#define GHWCFG2_SingPnt_MASK				Hw5
#define GHWCFG2_OtgArch_MASK				(0x3<<3)
#define GHWCFG2_OtgMode_MASK				0x7

//004C
#define GHWCFG3_DfifoDepth_MASK				(0xFFFF<<16)
#define GHWCFG3_DfifoDepth_Read(reg)		( (GHWCFG3_DfifoDepth_MASK&(reg)) >> 16 )
#define GHWCFG3_RstType_MASK				Hw11
#define GHWCFG3_OptFeature_MASK				Hw10
#define GHWCFG3_OtgEn_MASK					Hw7
#define GHWCFG3_PktSizeWidth_MASK			(0x7<<4)
#define GHWCFG3_PktSizeWidth_Read(reg)		( (GHWCFG3_OptFeature_MASK&(reg)) >> 4 )
#define GHWCFG3_XferSizeWidth_MASK			(0xF)
#define GHWCFG3_XferSizeWidth_Read(reg)		(GHWCFG3_XferSizeWidth_MASK&(reg))

//0050
#define GHWCFG4_INEps_MASK					(0xF<<26)
#define GHWCFG4_INEps_Read(reg)				( (GHWCFG4_INEps_MASK&(reg)) >> 26 )
#define GHWCFG4_DedFifoMode_MASK			Hw25
#define GHWCFG4_SessEndFltr_MASK			Hw24
#define GHWCFG4_BValidFltr_MASK				Hw23
#define GHWCFG4_AValidFltr_MASK				Hw22
#define GHWCFG4_VBusValidFltr_MASK			Hw21
#define GHWCFG4_IddgFltr_MASK				Hw20
#define GHWCFG4_NumCtlEps_MASK				(0xF<<16)
#define GHWCFG4_NumCtlEps_Read(reg)			( (GHWCFG4_NumCtlEps_MASK&(reg)) >> 16 )
#define GHWCFG4_PhyDataWidth_MASK			(0x3<<14)
#define GHWCFG4_AhbFreq_MASK				Hw5
#define GHWCFG4_EnablePwrOpt_MASK			Hw4
#define GHWCFG4_NumDevPerioEps_MASK			0xF
#define GHWCFG4_NumDevPerioEps_Read(reg)	(GHWCFG4_NumDevPerioEps_MASK&(reg))

//0100
#define HPTXFSIZ_PTxFSize(x)				(x<<16)
#define HPTXFSIZ_PTxFStAddr(x)				(x&0xFFFF)

//0104
#define DIEPTXF_INEPnTxFDep(x)				(x<<16)
#define DIEPTXF_INEPnTxFStAddr(x)			(x&0xFFFF)

//0800
#define DCFG_EPMisCnt_MASK					(0x7F<<16)
#define DCFG_EPMisCnt(x)					(x<<16)
#define DCFG_PerFrInt(x)					(x<<11)
#define DCFG_DevAddr(x)						((x&0x7F)<<4)		// 0~0x7F
#define DCFG_DevAddr_MASK					DCFG_DevAddr(0x7F)
#define DCFG_NZStsOUTHShk_STALL				Hw2
#define DCFG_NZStsOUTHShk_NORMAL			0
#define DCFG_DevSpd_MASK					0x2
#define DCFG_DevSpd_HS						0x0
#define DCFG_DevSpd_FS						0x1
#define DCFG_DevSpd_LS						0x2

//0804
#define DCTL_PWROnPrgDone					Hw11
#define DCTL_CGOUTNak						Hw10
#define DCTL_SGOUTNak						Hw9
#define DCTL_CGNPInNak						Hw8
#define DCTL_SGNPInNak						Hw7
#define DCTL_TstCtl(x)						(x<<4)
#define DCTL_TstCtl_MASK					DCTL_TstCtl(0x7)
#define DCTL_TstCtl_DISABLED				DCTL_TstCtl(0)
#define DCTL_TstCtl_TEST_J					DCTL_TstCtl(1)
#define DCTL_TstCtl_TEST_K					DCTL_TstCtl(2)
#define DCTL_TstCtl_TEST_SE0_NAK			DCTL_TstCtl(3)
#define DCTL_TstCtl_TEST_PACKET				DCTL_TstCtl(4)
#define DCTL_TstCtl_TEST_FORCE_ENABLE		DCTL_TstCtl(5)
#define DCTL_GOUTNakSts_MASK				Hw3
#define DCTL_GNPINNakSts_MASK				Hw2
#define DCTL_SftDiscon_DISCONNECT			Hw1
#define DCTL_SftDiscon_NORMAL				0
#define DCTL_RmtWkUpSig						Hw0

//0808
#define DSTS_SOFFN_MASK						(0x3FFF<<8)
#define DSTS_SOFFN_Read(reg)				( (DSTS_SOFFN_MASK&(reg)) >> 8 )
#define DSTS_ErrticErr_MASK					Hw3
#define DSTS_EnumSpd_MASK					(0x3<<1)
#define DSTS_EnumSpd_Read(reg)				( (DSTS_EnumSpd_MASK&(reg)) >> 1 )
#define DSTS_SuspSts_MASK					Hw0

//0810
#define DIEPMSK_TxfifoUndrnMsk_UNMASK		Hw8
#define DIEPMSK_INEPNakEffMsk_UNMASK		Hw6
#define DIEPMSK_INTknEPMisMsk_UNMASK		Hw5
#define DIEPMSK_INTknTXFEmpMsk_UNMASK		Hw4
#define DIEPMSK_TimeOUTMsk_UNMASK			Hw3
#define DIEPMSK_AHBErrMsk_UNMASK			Hw2
#define DIEPMSK_EPDisbldMsk_UNMASK			Hw1
#define DIEPMSK_XferComplMsk_UNMASK			Hw0

//0814
#define DOEPMSK_OutPktErrMsk_UNMASK			Hw8
#define DOEPMSK_Back2BackSETup_UNMASK		Hw6
#define DOEPMSK_OUTTknEPDisMsk_UNMASK		Hw4
#define DOEPMSK_SetUPMsk_UNMASK				Hw3
#define DOEPMSK_AHBErrMsk_UNMASK			Hw2
#define DOEPMSK_EPDisbldMsk_UNMASK			Hw1
#define DOEPMSK_XferComplMsk_UNMASK			Hw0

//0818
#define DAINT_OutEPInt_MASK					(0xFFFF<<16)
#define DAINT_OutEPInt_Read(reg)			( (DAINT_OutEPInt_MASK&(reg)) >> 16 )
#define DAINT_InEpInt_MASK					0xFFFF
#define DAINT_InEpInt_Read(reg)				(DAINT_InEpInt_MASK&(reg))

//081C
#define DAINTMSK_OutEPInt(ep)				(0x10000<<ep)
#define DAINTMSK_InEPInt(ep)				(1<<ep)

//0830
#define DTHRCTL_ArbPrkEn					Hw27
#define DTHRCTL_RxThrLen(x)					(x<<17)	// 0~0x1FF
#define DTHRCTL_RxThrEn						Hw16
#define DTHRCTL_TxThrLen(x)					(x<<2)		// 0~0x1FF
#define DTHRCTL_ISOThrEn					Hw1
#define DTHRCTL_NonISOThrEn					Hw0

//0834
#define DIEPEMPMSK_InEpTxfEmpMsk(ep)		(1<<ep)

//////////////////////
// IN End Point 0
//////////////////////
//0900
#define DIEPCTL0_EPEna						Hw31
#define DIEPCTL0_EPDis						Hw30
#define DIEPCTL0_SNAK						Hw27
#define DIEPCTL0_CNAK						Hw26
#define DIEPCTL0_TxFNum(x)					(x<<22)	// 0~15
#define DIEPCTL0_Stall						Hw21
#define DIEPCTL0_EPType						0			// Hardcoded
#define DIEPCTL0_NAKSts_MASK				Hw17
#define DIEPCTL0_USBActEP_MASK				Hw15
#define DIEPCTL0_MPS_MASK					3
#define DIEPCTL0_MPS_64BYTES				0
#define DIEPCTL0_MPS_32BYTES				1
#define DIEPCTL0_MPS_16BYTES				2
#define DIEPCTL0_MPS_8BYTES					3

//0910
#define DIEPTSIZ0_PktCnt(x)					(x<<19)		// ~3
#define DIEPTSIZ0_XferSize(x)				x				// 0~0x7F


//////////////////////
// IN End Point 0~15
//////////////////////
//0908
#define DIEPINT_TxfifoUndrn					Hw8
#define DIEPINT_TxFEmp_MASK					Hw7
#define DIEPINT_INEPNakEff					Hw6
#define DIEPINT_INTknEPMis					Hw5
#define DIEPINT_INTknTXFEmp					Hw4
#define DIEPINT_TimeOUT						Hw3
#define DIEPINT_AHBErr						Hw2
#define DIEPINT_EPDisabld					Hw1
#define DIEPINT_XferCompl					Hw0


//////////////////////
// IN End Point 1~15
//////////////////////
//0920
#define DIEPCTL_EPEna						Hw31
#define DIEPCTL_EPDis						Hw30
#define DIEPCTL_SetD1PID					Hw29
#define DIEPCTL_SetOddFr					Hw29
#define DIEPCTL_SetD0PID					Hw28
#define DIEPCTL_SetEvenFr					Hw28
#define DIEPCTL_SNAK						Hw27
#define DIEPCTL_CNAK						Hw26
#define DIEPCTL_TxFNum(x)					(x<<22)		// 0~15, TxFIFO Number
#define DIEPCTL_Stall						Hw21
#define DIEPCTL_EPType(x)					(x<<18)
#define DIEPCTL_EPType_CONTROL				DIEPCTL_EPType(0)
#define DIEPCTL_EPType_ISOCHRONOUS			DIEPCTL_EPType(1)
#define DIEPCTL_EPType_BULK					DIEPCTL_EPType(2)
#define DIEPCTL_EPType_INTERRUPT			DIEPCTL_EPType(3)
#define DIEPCTL_NakSts_MASK					Hw17
#define DIEPCTL_DPID_MASK					Hw16
#define DIEPCTL_EO_FrNum_MASK				Hw16
#define DIEPCTL_USBActEP					Hw15
#define DIEPCTL_MPS(x)						x				// 0~0x7FF

//0930
#define DIEPTSIZ_MC_MASK					(3<<29)
#define DIEPTSIZ_PktCnt_MASK				(0x3FF<<19)
#define DIEPTSIZ_PktCnt(x)					(x<<19)		// ~0x3FF
#define DIEPTSIZ_XferSize_MASK				0x7FFFF
#define DIEPTSIZ_XferSize(x)				x				// ~0x7FFFF


//////////////////////
// OUT End Point 0
//////////////////////
//0B00
#define DOEPCTL0_EPEna						Hw31			// Endpoint Enable
//#define DOEPCTL0_EPDis					Hw30			// unavailable
#define DOEPCTL0_SNAK						Hw27
#define DOEPCTL0_CNAK						Hw26			// Clear NAK
#define DOEPCTL0_Stall						Hw21
#define DOEPCTL0_Snp						Hw20
#define DOEPCTL0_EPType						0				// Hardcoded
#define DOEPCTL0_NAKSts_MASK				Hw17
#define DOEPCTL0_USBActEP_MASK				Hw15			// always 1
#define DOEPCTL0_MPS_64BYTES				0
#define DOEPCTL0_MPS_32BYTES				1
#define DOEPCTL0_MPS_16BYTES				2
#define DOEPCTL0_MPS_8BYTES					3

//0B10
#define DOEPTSIZ0_SUPCnt(x)					(x<<29)		// ~3
#define DOEPTSIZ0_PktCnt(x)					(x<<19)		// ~1
#define DOEPTSIZ0_XferSize(x)				x				// 0~0x7F


//////////////////////
// OUT End Point 0~15
//////////////////////
//0B08
#define DOEPINT_OUTPktErr					Hw8
#define DOEPINT_Back3BackSETup				Hw6
#define DOEPINT_OUTTknEPdis					Hw4
#define DOEPINT_SetUP						Hw3
#define DOEPINT_AHBErr						Hw2
#define DOEPINT_EPDisabld					Hw1
#define DOEPINT_XferCompl					Hw0

//////////////////////
// OUT End Point 1~15
//////////////////////
//0B20
#define DOEPCTL_EPEna						Hw31
#define DOEPCTL_EPDis						Hw30
#define DOEPCTL_SetD1PID					Hw29
#define DOEPCTL_SetOddFr					Hw29
#define DOEPCTL_SetD0PID					Hw28
#define DOEPCTL_SetEvenFr					Hw28
#define DOEPCTL_SNAK						Hw27
#define DOEPCTL_CNAK						Hw26
#define DOEPCTL_Stall						Hw21
#define DOEPCTL_Snp							Hw20
#define DOEPCTL_EPType(x)					(x<<18)
#define DOEPCTL_EPType_CONTROL				DOEPCTL_EPType(0)
#define DOEPCTL_EPType_ISOCHRONOUS			DOEPCTL_EPType(1)
#define DOEPCTL_EPType_BULK					DOEPCTL_EPType(2)
#define DOEPCTL_EPType_INTERRUPT			DOEPCTL_EPType(3)
#define DOEPCTL_NakSts_MASK					Hw17
#define DOEPCTL_DPID_MASK					Hw16
#define DOEPCTL_EO_FrNum_MASK				Hw16
#define DOEPCTL_USBActEP					Hw15
#define DOEPCTL_MPS(x)						x				// 0~0x7FF
#define DOEPCTL_MPS_MASK					0x7FF
#define DOEPCTL_MPS_Read(reg)				(DOEPCTL_MPS_MASK&(reg))

//0B30
#define DOEPTSIZ_RxDPID_MASK				(3<<29)			// Received Data PID
#define DOEPTSIZ_SUPCnt_MASK				(3<<29)			// SETUP Packet Count (number of back-to-back SETUP data packets)
#define DOEPTSIZ_SUPCnt_1PACKET				(1<<29)
#define DOEPTSIZ_SUPCnt_2PACKETS			(2<<29)
#define DOEPTSIZ_SUPCnt_3PACKETS			(3<<29)
#define DOEPTSIZ_PktCnt(x)					(x<<19)		// ~0x3FF
#define DOEPTSIZ_XferSize(x)				x				// ~0x7FFFF


#define DIEPCTL_INDEX						0				// 0x00
#define DIEPINT_INDEX						2				// 0x08
#define DIEPTSIZ_INDEX						4				// 0x10
#define DIEPDMA_INDEX						5				// 0x14
#define DTXFSTS_INDEX						6				// 0x18
#define DOEPCTL_INDEX						0				// 0x00
#define DOEPINT_INDEX						2				// 0x08
#define DOEPTSIZ_INDEX						4				// 0x10
#define DOEPDMA_INDEX						5				// 0x14


//0E00
#define PCGCCTL_PhySuspended_MASK			Hw4
#define PCGCCTL_RstPdwnModule				Hw3
#define PCGCCTL_PwrClmp						Hw2
#define PCGCCTL_GateHclk					Hw1
#define PCGCCTL_StopPclk					Hw0

////////////////////////////////////////////
// Common Bit Field for IN/OUT End Point
////////////////////////////////////////////
#define DIOEPCTL_SetD0PID					Hw28
#define DIOEPCTL_EPType(x)					((x&0x3)<<18)
#define DIOEPCTL_EPType_CONTROL				DIOEPCTL_EPType(0)
#define DIOEPCTL_EPType_ISOCHRONOUS			DIOEPCTL_EPType(1)
#define DIOEPCTL_EPType_BULK				DIOEPCTL_EPType(2)
#define DIOEPCTL_EPType_INTERRUPT			DIOEPCTL_EPType(3)
#define DIOEPCTL_USBActEP					Hw15
#define DIOEPCTL_MPS(x)						(x&0x7FF)		// 0~0x7FF
typedef struct _tag_DIEREG_T {		// Device Endpoint-n...
	volatile unsigned int	DIEPCTL;				// Control Register
	volatile unsigned int	reserved0;
	volatile unsigned int	DIEPINT;				// Interrupt Register
	volatile unsigned int	reserved1;
	volatile unsigned int	DIEPSIZ;				// Transfer Size Register
	volatile unsigned int	DIEPDMA;				// DMA Address Register
	volatile unsigned int	DTXFSTS;				// Device IN Endpoint Transmit FIFO Status Register
	volatile unsigned int	reserved2;
} DIEPREG_T, *PDIEPREG_T;

typedef struct _tag_DOEPREG_T {		// Device Endpoint-n...
	volatile unsigned int	DOEPCTL;				// Control Register
	volatile unsigned int	reserved0;
	volatile unsigned int	DOEPINT;				// Interrupt Register
	volatile unsigned int	reserved1;
	volatile unsigned int	DOEPSIZ;				// Transfer Size Register
	volatile unsigned int	DOEPDMA;				// DMA Address Register
	volatile unsigned int	reserved2[2];
} DOEPREG_T, *PDOEPREG_T;

#endif	/*__TCC_OTG_REGS_H__*/
