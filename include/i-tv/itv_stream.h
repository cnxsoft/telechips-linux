#ifndef __ITV_STREAM_H__
#define __ITV_STREAM_H__

typedef struct itv_stream_priv_t itv_stream_priv_t;

typedef struct itv_stream_t {
	ITV_DEVICE_BASE_MEMBERS

	itv_object_t *idemux;

	itv_stream_priv_t *priv;
} itv_stream_t;

ITV_EXPORT(itv_stream_t *, itv_stream_create, (itv_object_t *));
ITV_EXPORT(void, itv_stream_destroy, (itv_stream_t *));

#endif	/* __ITV_STREAM_H__ */
