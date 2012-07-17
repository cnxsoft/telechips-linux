/****************************************************************************
 *   FileName    : TSDEMUX_sys.h
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/

/****************************************************************************

  Revision History

 ****************************************************************************/

#ifndef _TSDEMUX_SYS_H_
#define _TSDEMUX_SYS_H_

typedef enum
{
	TS_ADAPTATION_RESERVED = 0x0,
	TS_ADAPTATION_PAYLOAD_ONLY,
	TS_ADAPTATION_ONLY,
	TS_ADAPTATION_AND_PAYLOAD
} MpegTsAdaptationCtrl;

typedef struct
{
	unsigned char     ext_flag:1;
	unsigned char     private_data_flag:1;
	unsigned char     splicing_point_flag:1;
	unsigned char     OPCR:1;
	unsigned char     PCR:1;
	unsigned char     priority_indicator:1;
	unsigned char     random_access_indicator:1;
	unsigned char     discontinuity_indicator:1;
} MpegTsAdaptionFlg;

typedef struct
{
	int       length;
	long long     PCR;
	MpegTsAdaptionFlg flag;
} MpegTsAdaptation;

typedef struct
{
	int       error_indicator;
	int       pusi;
	int       priority;
	int       pid;
	int       scrambling_control; /* This is ARIB Standard from B25
									*	adatation_contrl : 01(PO), 11(AP) => 00(Clean), 01(NotDefined), 10(SC-e), 11(SC-o)
									*  				       00(rev),10(AO) => XX(NotDefined)
									*/
	MpegTsAdaptationCtrl adaptation_control;
	int       cc;
	int       payload_size;
	char     *payload;
	MpegTsAdaptation adap;
} MpegTsHeader;

typedef struct
{
	unsigned short    PES_extension_flag:1;
	unsigned short    PES_CRC_flag:1;
	unsigned short    additional_copy_info_flag:1;
	unsigned short    DSM_trick_mode_flag:1;
	unsigned short    ES_rate_flag:1;
	unsigned short    ESCR_flag:1;
	unsigned short    PTS_DTS_flags:2;
	unsigned short    original_or_copy:1;
	unsigned short    copyright:1;
	unsigned short    data_alignment_indicator:1;
	unsigned short    PES_priority:1;
	unsigned short    PES_scrambling_control:2;
	unsigned short    dummy_pes_data:2;
} MpegPesFlag;

typedef struct
{
	int       stream_id;
	int       length;
	int       header_length;
	int       payload_size;
	char     *payload;
	MpegPesFlag flag;
	long long     pts;
	long long     dts;
} MpegPesHeader;

typedef enum
{
	STREAM_ID_PROGRAM_STREAM_MAP = 0xBC,
	STREAM_ID_PADDING_STREAM = 0xBE,
	STREAM_ID_PRIVATE_STREAM_2 = 0xBF,
	STREAM_ID_ECM_STREAM = 0xF0,
	STREAM_ID_EMM_STREAM = 0xF1,
	STREAM_ID_DSMCC = 0xF2,
	STREAM_ID_ITU_T_REC_H222_1_TYPE_E = 0xF8,
	STREAM_ID_PROGRAM_STREAM_DIRECTORY = 0xFF,
	STREAM_ID_MAX
} MPEGPES_STREAM_ID;

#define	MPEGSYS_TS_PACKETSIZE	188
#define	MPEGSYS_TS_PACKETSYNC   0x47

int TSDEMUX_MakeSTC (unsigned char *pucTS, unsigned int uiTSSize, unsigned int uiPCR);
int TSDEMUX_Open(void);
void TSDEMUX_Close(void);
unsigned int TSDEMUX_GetSTC(void);
#endif /* _TSDEMUX_SYS_H_ */
