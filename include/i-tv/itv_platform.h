#ifndef __ITV_PLATFORM_H__
#define __ITV_PLATFORM_H__

/* frontend operations */
typedef struct itv_platform_frontend_operations_t {
	// demodulator operations
	void (*demod_reset)(void);
	void (*demod_pwroff)(int tsif_mod);
	
	// i2c operations
	struct i2c_adapter *(*i2c_get_adapter)(void);
	void (*i2c_put_adapter)(struct i2c_adapter *);
} itv_platform_frontend_operations_t;

/* ts interface operations */
typedef struct itv_platform_tsif_operations_t {
	// ts parallel interface operations
	int (*tsif_p_init)(unsigned char **buffer_addr, unsigned int *real_buffer_size, unsigned int buffer_size, 
			unsigned int pkt_intr_cnt, unsigned int timing_mode, unsigned int pid_mode, unsigned int serial_mode, 
			void (*callback)(void *), void *cb_data);
	void (*tsif_p_deinit)(void);
	int (*tsif_p_get_pos)(void);
	int (*tsif_p_start)(void);
	int (*tsif_p_stop)(void);
	void (*tsif_p_insert_pid)(int pid);
	void (*tsif_p_remove_pid)(int pid);
	void (*tsif_p_set_packetcnt)(int cnt);
} itv_platform_tsif_operations_t;

/* frontend operations */
ITV_EXPORT(itv_platform_frontend_operations_t *, itv_platform_get_frontend_operations, (void));
ITV_EXPORT(void, itv_platform_put_frontend_operations, (itv_platform_frontend_operations_t *));
/* ts interface operations */
ITV_EXPORT(itv_platform_tsif_operations_t *, itv_platform_get_tsif_operations, (void));
ITV_EXPORT(void, itv_platform_put_tsif_operations, (itv_platform_tsif_operations_t *));

#endif	/* __ITV_PLATFORM_H__ */
