#ifndef __ITVFRONTEND_H__
#define __ITVFRONTEND_H__

#include <asm/types.h>

typedef enum {
	FE_QPSK,
	FE_QAM,
	FE_OFDM,
	FE_ATSC
} itv_frontend_type_e;

typedef enum {
	FE_IS_STUPID					= 0,
	FE_CAN_INVERSION_AUTO			= 0x1,
	FE_CAN_FEC_1_2					= 0x2,
	FE_CAN_FEC_2_3					= 0x4,
	FE_CAN_FEC_3_4					= 0x8,
	FE_CAN_FEC_4_5					= 0x10,
	FE_CAN_FEC_5_6					= 0x20,
	FE_CAN_FEC_6_7					= 0x40,
	FE_CAN_FEC_7_8					= 0x80,
	FE_CAN_FEC_8_9					= 0x100,
	FE_CAN_FEC_AUTO					= 0x200,
	FE_CAN_QPSK						= 0x400,
	FE_CAN_QAM_16					= 0x800,
	FE_CAN_QAM_32					= 0x1000,
	FE_CAN_QAM_64					= 0x2000,
	FE_CAN_QAM_128					= 0x4000,
	FE_CAN_QAM_256					= 0x8000,
	FE_CAN_QAM_AUTO					= 0x10000,
	FE_CAN_TRANSMISSION_MODE_AUTO	= 0x20000,
	FE_CAN_BANDWIDTH_AUTO			= 0x40000,
	FE_CAN_GUARD_INTERVAL_AUTO		= 0x80000,
	FE_CAN_HIERARCHY_AUTO			= 0x100000,
	FE_CAN_8VSB						= 0x200000,
	FE_CAN_16VSB					= 0x400000,
	FE_HAS_EXTENDED_CAPS			= 0x800000, 
	FE_NEEDS_BENDING				= 0x20000000, 
	FE_CAN_RECOVER					= 0x40000000, 
	FE_CAN_MUTE_TS					= 0x80000000
} itv_frontend_caps_e;

typedef struct itv_frontend_info_t {
	char				name[128];
	itv_frontend_type_e	type;
	__u32				frequency_min;
	__u32				frequency_max;
	__u32				frequency_stepsize;
	__u32				frequency_tolerance;
	__u32				symbol_rate_min;
	__u32				symbol_rate_max;
	__u32				symbol_rate_tolerance;
	__u32				notifier_delay;
	itv_frontend_caps_e	caps;
} itv_frontend_info_t;

typedef struct itv_diseqc_master_cmd_t {
	__u8 msg [6];
	__u8 msg_len;
} itv_diseqc_master_cmd_t;

typedef struct itv_diseqc_slave_reply_t {
	__u8 msg [4];
	__u8 msg_len;
	int  timeout;
} itv_diseqc_slave_reply_t;

typedef enum {
	SEC_VOLTAGE_13,
	SEC_VOLTAGE_18,
	SEC_VOLTAGE_OFF
} itv_frontend_sec_voltage_e;

typedef enum {
	SEC_TONE_ON,
	SEC_TONE_OFF
} itv_frontend_sec_tone_mode_e;

typedef enum {
	SEC_MINI_A,
	SEC_MINI_B
} itv_frontend_sec_mini_cmd_e;

typedef enum {
	FE_HAS_SIGNAL = 0x01, 
	FE_HAS_CARRIER = 0x02, 
	FE_HAS_VITERBI = 0x04, 
	FE_HAS_SYNC = 0x08, 
	FE_HAS_LOCK = 0x10, 
	FE_TIMEOUT = 0x20, 
	FE_REINIT = 0x40
} itv_frontend_status_e;

typedef enum {
	INVERSION_OFF,
	INVERSION_ON,
	INVERSION_AUTO
} itv_frontend_inversion_e;

typedef enum {
	FEC_NONE = 0,
	FEC_1_2,
	FEC_2_3,
	FEC_3_4,
	FEC_4_5,
	FEC_5_6,
	FEC_6_7,
	FEC_7_8,
	FEC_8_9,
	FEC_AUTO,
	FEC_3_5,
	FEC_9_10,
} itv_frontend_code_rate_e;

typedef enum {
	QPSK,
	QAM_16,
	QAM_32,
	QAM_64,
	QAM_128,
	QAM_256,
	QAM_AUTO,
	VSB_8,
	VSB_16,
	PSK_8,
	APSK_16,
	APSK_32,
	DQPSK,
} itv_frontend_modulation_e;

typedef enum {
	TRANSMISSION_MODE_2K,
	TRANSMISSION_MODE_8K,
	TRANSMISSION_MODE_AUTO
} itv_frontend_transmit_mode_e;

typedef enum {
	BANDWIDTH_8_MHZ,
	BANDWIDTH_7_MHZ,
	BANDWIDTH_6_MHZ,
	BANDWIDTH_AUTO
} itv_frontend_bandwidth_e;

typedef enum {
	GUARD_INTERVAL_1_32,
	GUARD_INTERVAL_1_16,
	GUARD_INTERVAL_1_8,
	GUARD_INTERVAL_1_4,
	GUARD_INTERVAL_AUTO
} itv_frontend_guard_interval_e;

typedef enum {
	HIERARCHY_NONE,
	HIERARCHY_1,
	HIERARCHY_2,
	HIERARCHY_4,
	HIERARCHY_AUTO
} itv_frontend_hierarchy_e;

typedef struct itv_qpsk_parameters_t {
	__u32 symbol_rate;
	itv_frontend_code_rate_e fec_inner;
} itv_qpsk_parameters_t;

typedef struct itv_qam_parameters_t {
	__u32 symbol_rate;
	itv_frontend_code_rate_e fec_inner;
	itv_frontend_modulation_e modulation;
} itv_qam_parameters_t;

typedef struct itv_vsb_parameters_t {
	itv_frontend_modulation_e modulation;
} itv_vsb_parameters_t;

typedef struct itv_ofdm_parameters_t {
	itv_frontend_bandwidth_e 		bandwidth;
	itv_frontend_code_rate_e 		code_rate_HP;
	itv_frontend_code_rate_e 		code_rate_LP;
	itv_frontend_modulation_e 		constellation;
	itv_frontend_transmit_mode_e 	transmission_mode;
	itv_frontend_guard_interval_e 	guard_interval;
	itv_frontend_hierarchy_e 		hierarchy_information;
} itv_ofdm_parameters_t;

typedef struct itv_frontend_parameters_t {
	__u32 frequency;

	itv_frontend_inversion_e inversion;
	union {
		itv_qpsk_parameters_t qpsk;
		itv_qam_parameters_t  qam;
		itv_vsb_parameters_t vsb;
		itv_ofdm_parameters_t ofdm;		
	} u;
} itv_frontend_parameters_t;

typedef struct itv_frontend_event_t {
	itv_frontend_status_e status;
	itv_frontend_parameters_t params;
} itv_frontend_event_t;

#define ITV_DTV_UNDEFINED			0
#define ITV_DTV_TUNE				1
#define ITV_DTV_CLEAR				2
#define ITV_DTV_FREQUENCY			3
#define ITV_DTV_MODULATION			4
#define ITV_DTV_BANDWIDTH_HZ		5
#define ITV_DTV_INVERSION			6
#define ITV_DTV_DISEQC_MASTER		7
#define ITV_DTV_SYMBOL_RATE			8
#define ITV_DTV_INNER_FEC			9
#define ITV_DTV_VOLTAGE				10
#define ITV_DTV_TONE				11
#define ITV_DTV_PILOT				12
#define ITV_DTV_ROLLOFF				13
#define ITV_DTV_DISEQC_SLAVE_REPLY	14

#define ITV_DTV_FE_CAPABILITY_COUNT	15
#define ITV_DTV_FE_CAPABILITY		16
#define ITV_DTV_DELIVERY_SYSTEM		17

#define ITV_DTV_API_VERSION			35
#define ITV_DTV_API_VERSION			35
#define ITV_DTV_CODE_RATE_HP		36
#define ITV_DTV_CODE_RATE_LP		37
#define ITV_DTV_GUARD_INTERVAL		38
#define ITV_DTV_TRANSMISSION_MODE	39
#define ITV_DTV_HIERARCHY			40

#define ITV_DTV_MAX_COMMAND			ITV_DTV_HIERARCHY

typedef enum {
	PILOT_ON,
	PILOT_OFF,
	PILOT_AUTO,
} itv_frontend_pilot_e;

typedef enum {
	ROLLOFF_35, 
	ROLLOFF_20,
	ROLLOFF_25,
	ROLLOFF_AUTO,
} itv_frontend_rolloff_e;

typedef enum {
	SYS_UNDEFINED,
	SYS_DVBC_ANNEX_AC,
	SYS_DVBC_ANNEX_B,
	SYS_DVBT,
	SYS_DSS,
	SYS_DVBS,
	SYS_DVBS2,
	SYS_DVBH,
	SYS_ISDBT,
	SYS_ISDBS,
	SYS_ISDBC,
	SYS_ATSC,
	SYS_ATSCMH,
	SYS_DMBTH,
	SYS_CMMB,
	SYS_DAB,
} itv_frontend_delivery_system_e;

typedef struct itv_dtv_cmds_h_t {
	char	*name;
	__u32	cmd;
	__u32	set:1;
	__u32	buffer:1;
	__u32	reserved:30;
} itv_dtv_cmds_h_t;

struct itv_dtv_property_t {
	__u32 cmd;
	__u32 reserved[3];
	union {
		__u32 data;
		struct {
			__u8 data[32];
			__u32 len;
			__u32 reserved1[3];
			void *reserved2;
		} buffer;
	} u;
	int result;
} __attribute__ ((packed));

typedef struct itv_dtv_property_t itv_dtv_property_t;

#define ITV_DTV_IOCTL_MAX_MSGS 64

typedef struct itv_dtv_properties_t {
	__u32 num;
	itv_dtv_property_t *props;
} itv_dtv_properties_t;

#define ITV_FE_SET_PROPERTY		   _IOW('o', 82, itv_dtv_properties_t)
#define ITV_FE_GET_PROPERTY		   _IOR('o', 83, itv_dtv_properties_t)

#define ITV_FE_TUNE_MODE_ONESHOT 0x01

#define ITV_FE_GET_INFO						_IOR('o', 61, itv_frontend_info_t)

#define ITV_FE_DISEQC_RESET_OVERLOAD		_IO('o', 62)
#define ITV_FE_DISEQC_SEND_MASTER_CMD		_IOW('o', 63, itv_diseqc_master_cmd_t)
#define ITV_FE_DISEQC_RECV_SLAVE_REPLY		_IOR('o', 64, itv_diseqc_slave_reply_t)
#define ITV_FE_DISEQC_SEND_BURST			_IO('o', 65)

#define ITV_FE_SET_TONE						_IO('o', 66)
#define ITV_FE_SET_VOLTAGE					_IO('o', 67)
#define ITV_FE_ENABLE_HIGH_LNB_VOLTAGE		_IO('o', 68)

#define ITV_FE_READ_STATUS					_IOR('o', 69, itv_frontend_status_e)
#define ITV_FE_READ_BER						_IOR('o', 70, __u32)
#define ITV_FE_READ_SIGNAL_STRENGTH			_IOR('o', 71, __u16)
#define ITV_FE_READ_SNR						_IOR('o', 72, __u16)
#define ITV_FE_READ_UNCORRECTED_BLOCKS		_IOR('o', 73, __u32)

#define ITV_FE_SET_FRONTEND					_IOW('o', 76, itv_frontend_parameters_t)
#define ITV_FE_GET_FRONTEND					_IOR('o', 77, itv_frontend_parameters_t)
#define ITV_FE_SET_FRONTEND_TUNE_MODE		_IO('o', 81)
#define ITV_FE_GET_EVENT					_IOR('o', 78, itv_frontend_event_t)

#define ITV_FE_DISHNETWORK_SEND_LEGACY_CMD	_IO('o', 80)

//20100413 koo : analog tv set cmd
#define ITV_FE_SET_FRONTEND_ANALOG			_IOW('o', 90, itv_frontend_parameters_t)

#endif	/* __ITVFRONTEND_H__ */
