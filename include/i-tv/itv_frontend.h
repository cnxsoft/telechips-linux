#ifndef __ITV_FRONTEND_H__
#define __ITV_FRONTEND_H__

#include <i-tv/itv_platform.h>

typedef struct itv_frontend_t itv_frontend_t;
typedef struct itv_frontend_priv_t itv_frontend_priv_t;
typedef struct itv_demod_priv_t itv_demod_priv_t;
typedef struct itv_tuner_priv_t itv_tuner_priv_t;

typedef struct itv_frontend_tune_settings_t {
	int min_delay_ms;
	int step_size;
	int max_drift;
	itv_frontend_parameters_t params;
} itv_frontend_tune_settings_t;

typedef struct itv_demod_t {
	itv_frontend_info_t info;

	int (*init)(itv_frontend_t *);
	int (*sleep)(itv_frontend_t *);

	int (*tune)(itv_frontend_t *, itv_frontend_parameters_t *, unsigned int, unsigned int *, itv_frontend_status_e *);
	int (*get_frontend_algo)(itv_frontend_t *);

	int (*set_frontend)(itv_frontend_t *, itv_frontend_parameters_t *);
	int (*set_frontend_analog)(itv_frontend_t *, itv_frontend_parameters_t *);
	int (*get_frontend)(itv_frontend_t *, itv_frontend_parameters_t *);
	int (*get_tune_settings)(itv_frontend_t *, itv_frontend_tune_settings_t *);

	int (*read_status)(itv_frontend_t *, itv_frontend_status_e *);
	int (*read_ber)(itv_frontend_t *, unsigned int *);
	int (*read_signal_strength)(itv_frontend_t *, unsigned short *);
	int (*read_snr)(itv_frontend_t *, unsigned short *);
	int (*read_ucblocks)(itv_frontend_t *, unsigned int *);

	int (*diseqc_reset_overload)(itv_frontend_t *);
	int (*diseqc_send_master_cmd)(itv_frontend_t *, itv_diseqc_master_cmd_t *);
	int (*diseqc_recv_slave_reply)(itv_frontend_t *, itv_diseqc_slave_reply_t *);
	int (*diseqc_send_burst)(itv_frontend_t *, itv_frontend_sec_mini_cmd_e);
	int (*set_tone)(itv_frontend_t *, itv_frontend_sec_tone_mode_e);
	int (*set_voltage)(itv_frontend_t *, itv_frontend_sec_voltage_e);
	int (*enable_high_lnb_voltage)(itv_frontend_t *, long);
	int (*dishnetwork_send_legacy_command)(itv_frontend_t *, unsigned long);
	int (*i2c_gate_ctrl)(itv_frontend_t *, int);
	int (*ts_bus_ctrl)(itv_frontend_t *, int);

	int (*set_property)(itv_frontend_t *, itv_dtv_property_t *);
	int (*get_property)(itv_frontend_t *, itv_dtv_property_t *);

#define ITV_FRONTEND_COMPONENT_TUNER 0
	int (*reset_tuner)(itv_frontend_t *, int, int, int);

	itv_demod_priv_t *priv;
} itv_demod_t;

typedef struct itv_tuner_info_t {
	char name[128];

	unsigned int frequency_min;
	unsigned int frequency_max;
	unsigned int frequency_step;

	unsigned int bandwidth_min;
	unsigned int bandwidth_max;
	unsigned int bandwidth_step;
} itv_tuner_info_t;

typedef struct itv_tuner_t {
	itv_tuner_info_t info;

	int (*init)(itv_frontend_t *);
	int (*sleep)(itv_frontend_t *);

	int (*set_params)(itv_frontend_t *, itv_frontend_parameters_t *);

	//20100413 koo : set_analog_params define
	int (*set_analog_params)(itv_frontend_t *, itv_frontend_parameters_t *);

	int (*get_frequency)(itv_frontend_t *, unsigned int *);
	int (*get_bandwidth)(itv_frontend_t *, unsigned int *);

#define ITV_TUNER_STATUS_LOCKED 1
#define ITV_TUNER_STATUS_STEREO 2
	int (*get_status)(itv_frontend_t *, unsigned int *);
	
	itv_tuner_priv_t *priv;
} itv_tuner_t;

typedef struct itv_dtv_frontend_properties_t {
	unsigned int state;
	
	unsigned int frequency;
	itv_frontend_modulation_e modulation;

	itv_frontend_sec_voltage_e voltage;
	itv_frontend_sec_tone_mode_e sectone;
	itv_frontend_inversion_e inversion;
	itv_frontend_code_rate_e fec_inner;
	itv_frontend_transmit_mode_e transmission_mode;
	unsigned int bandwidth_hz;
	itv_frontend_guard_interval_e guard_interval;
	itv_frontend_hierarchy_e hierarchy;
	unsigned int symbol_rate;
	itv_frontend_code_rate_e code_rate_HP;
	itv_frontend_code_rate_e code_rate_LP;

	itv_frontend_pilot_e pilot;
	itv_frontend_rolloff_e rolloff;

	itv_frontend_delivery_system_e delivery_system;
} itv_dtv_frontend_properties_t;

struct itv_frontend_t {
	ITV_DEVICE_BASE_MEMBERS

	// platform frontend operations
	itv_platform_frontend_operations_t *fe_ops;
	
	itv_dtv_frontend_properties_t dtv_property_cache;

	// demodulator
	itv_demod_t idemod;
	itv_tuner_t ituner;

	itv_tsif_mode_e tsif_mode;

	itv_frontend_priv_t *priv;

	int tune_digital;
};

ITV_EXPORT(itv_frontend_t *, itv_frontend_create, (itv_object_t *));
ITV_EXPORT(void, itv_frontend_destroy, (itv_frontend_t *));

#endif	/* __ITV_FRONTEND_H__ */
