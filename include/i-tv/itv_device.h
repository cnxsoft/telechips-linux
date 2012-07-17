#ifndef __ITV_DEVICE_H__
#define __ITV_DEVICE_H__

typedef enum {
	ITV_DEV_TYPE_NONE 		= -1, 
	ITV_DEV_TYPE_VIDEO, 
	ITV_DEV_TYPE_AUDIO, 
	ITV_DEV_TYPE_SEC, 
	ITV_DEV_TYPE_FRONTEND, 
	ITV_DEV_TYPE_DEMUX, 
	ITV_DEV_TYPE_STREAM, 
	ITV_DEV_TYPE_CA, 
	ITV_DEV_TYPE_NET, 
	ITV_DEV_TYPE_OSD, 
} itv_device_type_e;

#define ITV_DEVICE(x)	((itv_device_t *)(x))

#define ITV_DEVICE_BASE_MEMBERS						\
	ITV_OBJECT_BASE_MEMBERS							\
													\
	itv_object_t *iadapter;							\
	int id;											\
	itv_device_type_e type;							\
													\
	bool b_registered;								\
													\
	int readers;									\
	int writers;									\
	int users;										\
	int exit;										\
													\
	wait_queue_head_t wait_queue;					\
													\
	struct file_operations fops;

typedef struct itv_device_t {
	ITV_DEVICE_BASE_MEMBERS
} itv_device_t;

ITV_EXPORT(itv_device_t *, itv_device_create, (itv_object_t *, size_t, const char *));
ITV_EXPORT(void, __itv_device_destroy, (itv_device_t *));
ITV_EXPORT(int, __itv_device_register, (itv_device_t *, itv_device_type_e, int, struct file_operations *));
ITV_EXPORT(void, __itv_device_unregister, (itv_device_t *));
ITV_EXPORT(int, __itv_device_open, (itv_device_t *));
ITV_EXPORT(int, __itv_device_release, (itv_device_t *, struct file *));
ITV_EXPORT(int, __itv_device_inc_readers, (itv_device_t *));
ITV_EXPORT(int, __itv_device_dec_readers, (itv_device_t *));
ITV_EXPORT(int, __itv_device_inc_writers, (itv_device_t *));
ITV_EXPORT(int, __itv_device_dec_writers, (itv_device_t *));

#define itv_device_destroy(a)				__itv_device_destroy(ITV_DEVICE(a))
#define itv_device_register(a, b, c, d)		__itv_device_register(ITV_DEVICE(a), b, c, d)
#define itv_device_unregister(a)			__itv_device_unregister(ITV_DEVICE(a))
#define itv_device_open(a)					__itv_device_open(ITV_DEVICE(a))
#define itv_device_release(a, b)			__itv_device_release(ITV_DEVICE(a), b)
#define itv_device_inc_readers(a)			__itv_device_inc_readers(ITV_DEVICE(a))
#define itv_device_dec_readers(a)			__itv_device_dec_readers(ITV_DEVICE(a))
#define itv_device_inc_writers(a)			__itv_device_inc_writers(ITV_DEVICE(a))
#define itv_device_dec_writers(a)			__itv_device_dec_writers(ITV_DEVICE(a))

#endif	/* __ITV_DEVICE_H__ */
