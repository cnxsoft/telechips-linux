#include <i-tv/itv_common.h>
#include <i-tv/itv_adapter.h>

static int itv_adapter_debug;
static int itv_adapter_debug_callstack;

module_param_named(iadapter_debug, itv_adapter_debug, int, 0644);
module_param_named(iadapter_debug_cs, itv_adapter_debug_callstack, int, 0644);

MODULE_PARM_DESC(iadapter_debug, "Turn on/off adapter debugging (default:off).");
MODULE_PARM_DESC(iadapter_debug_cs, "Turn on/off adapter callstack debugging (default:off).");

#ifdef dprintk
#undef dprintk
#endif

#define dprintk(args...) \
	do { if(itv_adapter_debug) { printk("[%s] ", __func__); printk(args); } } while (0)

#define DEBUG_CALLSTACK		\
	do { if(itv_adapter_debug_callstack) { printk("[Call] ----> %s()\n", __func__); } } while (0);

ITV_EXTERN itv_core_t *p_icore;

itv_device_t *itv_adapter_find_device(itv_object_t *p_this, int minor)
{
	int i;
	
	itv_object_list_t *p_list;
	itv_device_t *idev = NULL;

//	DEBUG_CALLSTACK

	p_list = itv_list_children(p_this);
	if(p_list->i_count == 0) {
		itv_list_release(p_list);
		return NULL;
	}
	for(i = 0; i < p_list->i_count; i++) {
		idev = (itv_device_t *)p_list->pp_object[i];
		if(CONV2MINOR(((itv_adapter_t *)idev->iadapter)->id, idev->type, idev->id) == minor)
			break;
	}
	itv_list_release(p_list);
	
	return idev;
}
EXPORT_SYMBOL(itv_adapter_find_device);

int itv_adapter_register(itv_adapter_t *p_iadapter, struct module *p_owner, struct device *p_dev)
{
	int retval = 0;
	int adapter_id;

	itv_object_t *iadapter;

	DEBUG_CALLSTACK

	spin_lock(&p_iadapter->register_lock);
	if((iadapter = itv_object_find_name(p_iadapter->icore, p_iadapter->psz_object_name, FIND_CHILD)) != NULL) {
		wprintk("This adapter is already exist : %s\n",p_iadapter->psz_object_name);
		spin_unlock(&p_iadapter->register_lock);
		return -1;
	}

	if((adapter_id = itv_get_adapter_id(p_iadapter->icore)) == ITV_UNSET) {
		eprintk("itv_get_adapter_id() is failed\n");
		spin_unlock(&p_iadapter->register_lock);
		return -1;
	}

	p_iadapter->id		= adapter_id;
	p_iadapter->dev		= p_dev;
	p_iadapter->owner	= p_owner;

	itv_object_attach(p_iadapter, p_iadapter->icore);

	spin_unlock(&p_iadapter->register_lock);

	return retval;
}
EXPORT_SYMBOL(itv_adapter_register);

void itv_adapter_unregister(itv_adapter_t *p_iadapter)
{
	DEBUG_CALLSTACK
		
	spin_lock(&p_iadapter->register_lock);
	itv_put_adapter_id(p_iadapter->icore, p_iadapter->id);
	itv_object_detach(p_iadapter);
	spin_unlock(&p_iadapter->register_lock);
}
EXPORT_SYMBOL(itv_adapter_unregister);

itv_adapter_t *itv_adapter_create(const char *psz_name)
{
	itv_adapter_t *iadapter;

	DEBUG_CALLSTACK

	if((iadapter = (itv_adapter_t *)itv_object_create(p_icore, sizeof(*iadapter), ITV_OBJECT_ADAPTER, "iadapter")) == NULL) {
		eprintk("itv_object_create() is failed\n");
		return NULL;
	}

	spin_lock_init(&iadapter->register_lock);

	iadapter->psz_object_name = kstrdup(psz_name, GFP_KERNEL);

	return iadapter;
}
EXPORT_SYMBOL(itv_adapter_create);

void itv_adapter_destroy(itv_adapter_t *p_iadapter)
{
	DEBUG_CALLSTACK
		
	itv_object_release(p_iadapter);
}
EXPORT_SYMBOL(itv_adapter_destroy);
