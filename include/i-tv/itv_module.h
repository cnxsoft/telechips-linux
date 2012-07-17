#ifndef __ITV_MODULE_H__
#define __ITV_MODULE_H__

typedef struct itv_module_t itv_module_t;

struct itv_module_t {
	char *psz_module_name;

	char *psz_capability;
	int i_score;

	int (*pf_activate)(itv_object_t *);
	void (*pf_deactivate)(itv_object_t *);

	itv_module_t *next;
	itv_module_t *parent;
	itv_module_t *submodule;
	int i_submodules;
};

ITV_EXPORT(int, itv_module_register, (itv_module_t *));
ITV_EXPORT(void, itv_module_unregister, (itv_module_t *));
ITV_EXPORT(itv_module_t *, itv_module_need, (itv_object_t *, const char *, const char *));
ITV_EXPORT(void, itv_module_unneed, (itv_object_t *, itv_module_t *));

#endif	/* __ITV_MODULE_H__ */
