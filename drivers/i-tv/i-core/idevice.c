#include <i-tv/itv_common.h>
#include <i-tv/itv_adapter.h>

static int itv_device_debug;
static int itv_device_debug_callstack;

module_param_named(idevice_debug, itv_device_debug, int, 0644);
module_param_named(idevice_debug_cs, itv_device_debug_callstack, int, 0644);

MODULE_PARM_DESC(idevice_debug, "Turn on/off device debugging (default:off).");
MODULE_PARM_DESC(idevice_debug_cs, "Turn on/off device callstack debugging (default:off).");

#ifdef dprintk
#undef dprintk
#endif

#define dprintk(args...) \
	do { if(itv_device_debug) { printk("[%s] ", __func__); printk(args); } } while (0)

#define DEBUG_CALLSTACK		\
	do { if(itv_device_debug_callstack) { printk("[Call] ----> %s()\n", __func__); } } while (0);


static const char *const device_names[] = {
	"video", "audio", "sec", "frontend", "demux", "stream", "ca",
	"net", "osd"
};

int __itv_device_inc_readers(itv_device_t *p_idev)
{
	DEBUG_CALLSTACK
		
	if(!p_idev)
		return -1;

	if(p_idev->users <= 0)
		return 0;

	p_idev->readers++;
	return p_idev->readers;
}
EXPORT_SYMBOL(__itv_device_inc_readers);

int __itv_device_dec_readers(itv_device_t *p_idev)
{
	DEBUG_CALLSTACK
		
	if(!p_idev)
		return -1;

	if(p_idev->users <= 0 || p_idev->readers <= 0)
		return 0;

	p_idev->readers--;
	return p_idev->readers;
}
EXPORT_SYMBOL(__itv_device_dec_readers);

int __itv_device_inc_writers(itv_device_t *p_idev)
{
	DEBUG_CALLSTACK
		
	if(!p_idev)
		return -1;

	if(p_idev->users <= 0)
		return 0;

	p_idev->writers++;
	return p_idev->writers;
}
EXPORT_SYMBOL(__itv_device_inc_writers);

int __itv_device_dec_writers(itv_device_t *p_idev)
{
	DEBUG_CALLSTACK
		
	if(!p_idev)
		return -1;

	if(p_idev->users <= 0 || p_idev->writers <= 0)
		return 0;

	p_idev->writers--;
	return p_idev->writers;
}
EXPORT_SYMBOL(__itv_device_dec_writers);

int __itv_device_open(itv_device_t *p_idev)
{
//	DEBUG_CALLSTACK

	p_idev->users++;
	if(p_idev->exit == 1)
		p_idev->exit = 0;

	return 0;
}
EXPORT_SYMBOL(__itv_device_open);

int __itv_device_release(itv_device_t *p_idev, struct file *file)
{
	DEBUG_CALLSTACK

	if(p_idev->users <= 0)
		return 0;

	p_idev->users--;
	if(p_idev->users == 0 && p_idev->readers == 0 && p_idev->writers == 0) {
		p_idev->exit = 1;
		fops_put(file->f_op);
		file->f_op = NULL;
		wake_up(&p_idev->wait_queue);
	}

	dprintk("users : %d readers : %d writers : %d\n", p_idev->users, p_idev->readers, p_idev->writers);

	return 0;
}
EXPORT_SYMBOL(__itv_device_release);

static void itv_device_init(itv_device_t *p_idev)
{
//	DEBUG_CALLSTACK
		
	p_idev->id		= ITV_UNSET;
	p_idev->type	= ITV_DEV_TYPE_NONE;

	p_idev->b_registered = false;

	p_idev->readers	= 0;
	p_idev->writers	= 0;
	p_idev->users	= 0;
	p_idev->exit	= 0;

	init_waitqueue_head(&p_idev->wait_queue);

	memset(&p_idev->fops, 0, sizeof(struct file_operations));
}

int __itv_device_register(itv_device_t *p_idev, itv_device_type_e type, int id, struct file_operations *user_fops)
{	
	itv_core_t *icore = p_idev->icore;
	itv_adapter_t *iadapter = (itv_adapter_t *)p_idev->iadapter;
	struct device *clsdev;

	DEBUG_CALLSTACK

	if(!p_idev || !user_fops)
		return -1;

	if(!user_fops->open || !user_fops->release)
		return -1;

	if(type == ITV_DEV_TYPE_NONE)
		return -1;

	p_idev->type = type;
	p_idev->id = id;

	memcpy(&p_idev->fops, user_fops, sizeof(struct file_operations));

	clsdev = device_create(icore->class, iadapter->dev, MKDEV(ITV_MAJOR, CONV2MINOR(iadapter->id, p_idev->type, p_idev->id)), 
			NULL, "itv%d.%s%d", iadapter->id, device_names[p_idev->type], p_idev->id);
	if(IS_ERR(clsdev)) {
		eprintk("failed to create device itv%d.%s.%d (%ld)\n", 
				iadapter->id, device_names[p_idev->type], p_idev->id, PTR_ERR(clsdev));
		itv_device_init(p_idev);
		return PTR_ERR(clsdev);
	}

	p_idev->b_registered = true;

	itv_object_attach(p_idev, iadapter);

	return 0;
}
EXPORT_SYMBOL(__itv_device_register);

void __itv_device_unregister(itv_device_t *p_idev)
{
	itv_core_t *icore = p_idev->icore;
	itv_adapter_t *iadapter = (itv_adapter_t *)p_idev->iadapter;

	DEBUG_CALLSTACK

	device_destroy(icore->class, MKDEV(ITV_MAJOR, CONV2MINOR(iadapter->id, p_idev->type, p_idev->id)));
}
EXPORT_SYMBOL(__itv_device_unregister);

itv_device_t *itv_device_create(itv_object_t *p_this, size_t i_size, const char *psz_type)
{
	itv_adapter_t *iadapter = (itv_adapter_t *)p_this;
	itv_device_t *idev;

	DEBUG_CALLSTACK
	
	if((idev = (itv_device_t *)itv_object_create(iadapter, i_size, ITV_OBJECT_DEVICE, psz_type)) == NULL) {
		eprintk("itv_object_create() is failed\n");
		return NULL;
	}
	idev->iadapter = ITV_OBJECT(iadapter);

	itv_device_init(idev);
											
	return idev;
}
EXPORT_SYMBOL(itv_device_create);

void __itv_device_destroy(itv_device_t *p_idev)
{
	DEBUG_CALLSTACK
		
	if(!p_idev)
		return;
	
	if(p_idev->exit == 0 && (p_idev->users > 0 || p_idev->readers > 0 || p_idev->writers > 0)) {
		wait_event(p_idev->wait_queue, p_idev->exit == 1);
	}

	if(p_idev->b_registered)
		itv_device_unregister(p_idev);

	itv_object_release(p_idev);
}
EXPORT_SYMBOL(__itv_device_destroy);
