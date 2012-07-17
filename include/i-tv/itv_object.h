#ifndef __ITV_OBJECT_H__
#define __ITV_OBJECT_H__

#define ITV_OBJECT_CORE		0
#define ITV_OBJECT_ADAPTER	1
#define ITV_OBJECT_DEVICE	2

#define FIND_PARENT		0x0001
#define FIND_CHILD		0x0002
#define FIND_ANYWHERE	0x0003

typedef struct itv_core_t	itv_core_t;
typedef struct itv_object_t itv_object_t;
typedef struct itv_object_list_t itv_object_list_t;

#define ITV_OBJECT(x)	((itv_object_t *)(x))

#define ITV_OBJECT_BASE_MEMBERS				\
	const char *psz_object_type;			\
	char *psz_object_name;					\
											\
	bool b_error;							\
	bool b_die;								\
											\
	itv_core_t *icore;						\
	itv_object_t *parent;

struct itv_object_t {
	ITV_OBJECT_BASE_MEMBERS
};

struct itv_object_list_t {
	int i_count;
	itv_object_t **pp_object;
};

typedef void (*itv_destructor_t)(struct itv_object_t *);

ITV_EXPORT(itv_object_t *, __itv_object_hold, (itv_object_t *));
ITV_EXPORT(void, __itv_object_set_destructor, (itv_object_t *, itv_destructor_t));
ITV_EXPORT(itv_object_t *, __itv_object_find, (itv_object_t *, int, int));
ITV_EXPORT(itv_object_t *, __itv_object_find_name, (itv_object_t *, const char *, int));
ITV_EXPORT(itv_object_list_t *, __itv_list_find, (itv_object_t *, int, int));
ITV_EXPORT(itv_object_list_t *, __itv_list_children, (itv_object_t *));
ITV_EXPORT(void, __itv_list_release, (itv_object_list_t *));
ITV_EXPORT(void, __itv_object_attach, (itv_object_t *, itv_object_t *));
ITV_EXPORT(void, __itv_object_detach, (itv_object_t *));
ITV_EXPORT(itv_object_t *, __itv_object_create, (itv_object_t *, size_t, int, const char *));
ITV_EXPORT(void, __itv_object_release, (itv_object_t *));

#define itv_object_hold(a)					__itv_object_hold(ITV_OBJECT(a))
#define itv_object_set_destructor(a, b)		__itv_object_set_destructor(ITV_OBJECT(a), b)
#define itv_object_find(a, b, c)			__itv_object_find(ITV_OBJECT(a), b, c)
#define itv_object_find_name(a, b, c)		__itv_object_find_name(ITV_OBJECT(a), b, c)
#define itv_list_find(a, b, c)				__itv_list_find(ITV_OBJECT(a), b, c)
#define itv_list_children(a)				__itv_list_children(ITV_OBJECT(a))
#define itv_list_release(a)					__itv_list_release(a)
#define itv_object_attach(a, b)				__itv_object_attach(ITV_OBJECT(a), ITV_OBJECT(b))
#define itv_object_detach(a)				__itv_object_detach(ITV_OBJECT(a))
#define itv_object_create(a, b, c, d)		__itv_object_create(ITV_OBJECT(a), b, c, d)
#define itv_object_release(a)				__itv_object_release(ITV_OBJECT(a))

#endif	/* __ITV_OBJECT_H__ */
