#ifndef __ITV_ADAPTER_H__
#define __ITV_ADAPTER_H__

typedef struct itv_adapter_priv_t itv_adapter_priv_t;

typedef struct itv_adapter_t {
	ITV_OBJECT_BASE_MEMBERS

	int id;
	struct device *dev;
	struct module *owner;

	itv_std_type_e std_type;
	itv_if_type_e if_type;
	itv_tsif_mode_e tsif_mode;

	itv_object_t *idemux;
	itv_object_t *istream;
	itv_object_t *ifrontend;

	spinlock_t register_lock;

	itv_adapter_priv_t *priv;
} itv_adapter_t;

ITV_EXPORT(itv_device_t *, itv_adapter_find_device, (itv_object_t *, int));
ITV_EXPORT(int, itv_adapter_register, (itv_adapter_t *, struct module *, struct device *));
ITV_EXPORT(void, itv_adapter_unregister, (itv_adapter_t *));
ITV_EXPORT(itv_adapter_t *, itv_adapter_create, (const char *));
ITV_EXPORT(void, itv_adapter_destroy, (itv_adapter_t *));

#endif	/* __ITV_ADAPTER_H__ */
