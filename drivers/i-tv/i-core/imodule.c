#include <i-tv/itv_common.h>
#include <i-tv/itv_module.h>

static int itv_module_debug;
static int itv_module_debug_callstack;

module_param_named(imodule_debug, itv_module_debug, int, 0644);
module_param_named(imodule_debug_cs, itv_module_debug_callstack, int, 0644);

MODULE_PARM_DESC(imodule_debug, "Turn on/off module debugging (default:off).");
MODULE_PARM_DESC(imodule_debug_cs, "Turn on/off module callstack debugging (default:off).");

#ifdef dprintk
#undef dprintk
#endif

#define dprintk(args...) \
	do { if(itv_module_debug) { printk("[%s] ", __func__); printk(args); } } while (0)

#define DEBUG_CALLSTACK		\
	do { if(itv_module_debug_callstack) { printk("[Call] ----> %s()\n", __func__); } } while (0);

itv_module_t imodule_list = {
	.psz_module_name = "dummy", 
	.psz_capability = "idummy", 
	.i_score = 0, 
	.next = NULL, 
	.parent = NULL, 
	.submodule = NULL, 
	.i_submodules = 0
};

static void imodule_list_free(itv_module_t **p_list)
{
//	DEBUG_CALLSTACK
		
	if(!p_list) return;

	kfree(p_list);
}

static itv_module_t **imodule_list_get(int *p_count, const char *psz_capability, const char *psz_name)
{
	int i = 0;

	itv_module_t *imodule;
	itv_module_t **tab = NULL;

//	DEBUG_CALLSTACK

	for(imodule = &imodule_list; imodule; imodule = imodule->next) {
		itv_module_t **nt;

		if(strcmp(psz_capability, imodule->psz_capability) 
				|| strcmp(psz_name, imodule->psz_module_name)) {
			continue;
		}

		nt = krealloc(tab, (i + 1) * sizeof(*tab), GFP_KERNEL);
		if(nt == NULL) {
			imodule_list_free(tab);
			return NULL;
		}

		tab = nt;
		tab[i++] = imodule;
	}

	if(p_count)
		*p_count = i;

	return tab;
}

int itv_module_register(itv_module_t *p_module)
{
	itv_module_t *imodule;

//	DEBUG_CALLSTACK

	imodule = &imodule_list;
	while(imodule != NULL)
	{
		if(!strcmp(imodule->psz_capability, p_module->psz_capability) 
				&& !strcmp(imodule->psz_module_name, p_module->psz_module_name) 
				&& imodule->i_score == p_module->i_score)
		{
			wprintk("This module is already registered.\n");
			return -1;
		}

		if(imodule->next == NULL)
		{
			imodule->next = p_module;
			return 0;
		}
		
		imodule = imodule->next;
	}

	return -1;
}
EXPORT_SYMBOL(itv_module_register);

void itv_module_unregister(itv_module_t *p_module)
{
	itv_module_t *imodule;

//	DEBUG_CALLSTACK

	imodule = &imodule_list;
	while(imodule != NULL) {
		if(imodule->next == p_module) {
			imodule->next = imodule->next->next;
			break;
		}
		imodule = imodule->next;
	}
}
EXPORT_SYMBOL(itv_module_unregister);

itv_module_t *itv_module_need(itv_object_t *p_this, const char *psz_capability, const char *psz_name)
{
	int i;
	int count;
	itv_module_t **list;

	DEBUG_CALLSTACK

	list = imodule_list_get(&count, psz_capability, psz_name);
	if(!list) return NULL;

	for(i = 0; i < count; i++) {
		itv_module_t *imodule = list[i];

		if(imodule->pf_activate) {
			if(imodule->pf_activate(p_this) == 0) {
				imodule_list_free(list);
				return imodule;
			}
		}
	}
	imodule_list_free(list);
	
	return NULL;
}
EXPORT_SYMBOL(itv_module_need);

void itv_module_unneed(itv_object_t *p_this, itv_module_t *p_module)
{
	DEBUG_CALLSTACK
		
	if(p_module->pf_deactivate) {
		p_module->pf_deactivate(p_this);
	}
}
EXPORT_SYMBOL(itv_module_unneed);
