#ifndef __ITS_H__
#define __ITS_H__

#define ITV_TS_SIZE				188
#define ITV_TS_SYNCBYTE			0x47
#define ITV_TS_WIDTH			(2 * ITV_TS_SIZE)
#define ITV_TS_MIN_BUFSIZE		188
#define ITV_TS_MAX_BUFSIZE		1410
#define ITV_TS_HEIGHT_MASK		0xf00
#define ITV_TS_WARNING_WAIT		(30 * HZ)

#define ITV_TS_FILTER_NUM		256

#define ITV_TS_PACKET		1
#define ITV_TS_PAYLOAD_ONLY	2

typedef enum {
	ITV_TS_FILTER_STATE_FREE, 
	ITV_TS_FILTER_STATE_UNSET, 
	ITV_TS_FILTER_STATE_SET
} itv_ts_filter_state_e;

typedef enum {
	ITV_TS_FILTER_TYPE_NONE, 
	ITV_TS_FILTER_TYPE_TS, 
	ITV_TS_FILTER_TYPE_SEC, 
	ITV_TS_FILTER_TYPE_PES
} itv_ts_filter_type_e;

typedef struct itv_ts_filter_t {
	int ts_type;
	unsigned short pid;

	itv_demux_output_e output;
	itv_ts_filter_type_e type;
	itv_ts_filter_state_e state;
	
	itv_demux_priv_t *priv;
	itv_ringbuffer_t buffer;

	struct list_head list_head;

	struct mutex mutex;	
} itv_ts_filter_t;

typedef struct itv_ts_buffer_t {	
	unsigned char *buffer;
	unsigned int bufp;
	unsigned int width;
	unsigned int height;
	unsigned int size;
	// warning
	unsigned int warning_threshold;
	unsigned int warnings;
	unsigned long warning_time;
} itv_ts_buffer_t;

struct itv_demux_priv_t {
	itv_ts_buffer_t buffer;

	itv_demux_t *idemux;

	struct tasklet_struct ts_tasklet;
	struct list_head filter_list;

	int filtering;
	itv_ts_filter_t filter[ITV_TS_FILTER_NUM];

	spinlock_t lock;
};

#endif	/* __ITS_H__ */
