#ifndef __IFRONTEND_H__
#define __IFRONTEND_H__

#define FE_STATE_IDLE 			1
#define FE_STATE_RETUNE 		2
#define FE_STATE_TUNING_FAST 	4
#define FE_STATE_TUNING_SLOW 	8
#define FE_STATE_TUNED 			16
#define FE_STATE_ZIGZAG_FAST 	32
#define FE_STATE_ZIGZAG_SLOW 	64
#define FE_STATE_DISEQC 		128
#define FE_STATE_WAITFORLOCK (FE_STATE_TUNING_FAST | FE_STATE_TUNING_SLOW | FE_STATE_ZIGZAG_FAST | FE_STATE_ZIGZAG_SLOW | FE_STATE_DISEQC)
#define FE_STATE_SEARCHING_FAST (FE_STATE_TUNING_FAST | FE_STATE_ZIGZAG_FAST)
#define FE_STATE_SEARCHING_SLOW (FE_STATE_TUNING_SLOW | FE_STATE_ZIGZAG_SLOW)
#define FE_STATE_LOSTLOCK (FE_STATE_ZIGZAG_FAST | FE_STATE_ZIGZAG_SLOW)

#define FE_ALGO_HW	1

//20110523 koo : power save mode ¡¯¿‘ Ω√ pwdn & rst signal control
#define FE_POWER_SAVE_ENABLE

#define MAX_FE_EVENT 8

typedef struct itv_frontend_events_t {
	itv_frontend_event_t events[MAX_FE_EVENT];
	int eventw;
	int eventr;
	int overflow;
	wait_queue_head_t wait_queue;
	struct mutex mutex;
} itv_frontend_events_t;

struct itv_frontend_priv_t {
	struct task_struct *thread;

	int tone;
	int voltage;
	unsigned int wakeup;
	unsigned int delay;
	unsigned int reinitialise;
	unsigned long tune_mode_flags;
	unsigned long release_jiffies;	
	itv_frontend_events_t events;
	itv_frontend_status_e status;
	itv_frontend_parameters_t params;
	wait_queue_head_t wait_queue;
	struct semaphore sem;
	
	int quality;
	int lnb_drift;
	unsigned int state;
	unsigned int inversion;
	unsigned int auto_step;
	unsigned int auto_sub_step;
	unsigned int started_auto_step;
	unsigned int min_delay;
	unsigned int max_drift;
	unsigned int step_size;
	unsigned int check_wrapped;

	itv_module_t *idemod;
	itv_module_t *ituner;
};

#endif	/* __IFRONTEND_H__ */
