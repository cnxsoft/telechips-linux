#ifndef __ITV_DEMUX_H__
#define __ITV_DEMUX_H__

#include <i-tv/itv_ringbuffer.h>
#include <i-tv/itv_platform.h>

typedef void * DMX_FILTER_HANDLE;

typedef struct itv_demux_t itv_demux_t;
typedef struct itv_demux_priv_t itv_demux_priv_t;

struct itv_demux_t {
	ITV_DEVICE_BASE_MEMBERS

	itv_if_type_e if_type;
	itv_tsif_mode_e tsif_mode;

	itv_module_t *imodule;

	itv_ringbuffer_t stream_buffer;
	
	struct mutex mutex;
	spinlock_t lock;

	// platform tsif operations
	itv_platform_tsif_operations_t *tsif_ops;

	int (*demux_start)(itv_demux_t *);
	int (*demux_stop)(itv_demux_t *);
	int (*demux_poll)(DMX_FILTER_HANDLE, struct file *, poll_table *);
	ssize_t (*demux_read)(DMX_FILTER_HANDLE, int, char __user *, size_t, loff_t *);

	DMX_FILTER_HANDLE (*filter_get)(itv_demux_t *);
	void (*filter_put)(DMX_FILTER_HANDLE);
	int (*filter_set)(DMX_FILTER_HANDLE, int, void *);

	itv_demux_priv_t *priv;
};

typedef struct demux_filter_handle_t {
	DMX_FILTER_HANDLE handle;
	itv_demux_t *idemux;
} demux_filter_handle_t;

ITV_EXPORT(itv_demux_t *, itv_demux_create, (itv_object_t *));
ITV_EXPORT(void, itv_demux_destroy, (itv_demux_t *));
ITV_EXPORT(int, itv_demux_start, (itv_demux_t *));
ITV_EXPORT(int, itv_demux_stop, (itv_demux_t *));

#endif	/* __ITV_DEMUX_H__ */
