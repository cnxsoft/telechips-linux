#ifndef __ITV_CORE_H__
#define __ITV_CORE_H__

struct itv_core_t {
	ITV_OBJECT_BASE_MEMBERS

	struct cdev cdev;
	struct class *class;
	struct file_operations fops;

	spinlock_t lock;
	int adapter_ids[ITV_MAX_ADAPTERS];

	spinlock_t structure_lock;
};

ITV_EXPORT(int, itv_usercopy, (struct file *, unsigned int, unsigned long, 
	int (*func)(struct file *, unsigned int, void *)));
ITV_EXPORT(int, itv_get_adapter_id, (itv_core_t *));
ITV_EXPORT(void, itv_put_adapter_id, (itv_core_t *, int));

#endif	/* __ITV_CORE_H__ */
