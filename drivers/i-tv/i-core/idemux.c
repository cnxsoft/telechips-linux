#include <linux/i-tv/demux.h>
#include <i-tv/itv_common.h>
#include <i-tv/itv_adapter.h>
#include <i-tv/itv_platform.h>
#include <i-tv/itv_demux.h>

static int itv_demux_debug;
static int itv_demux_debug_callstack;

module_param_named(idemux_debug, itv_demux_debug, int, 0644);
module_param_named(idemux_debug_cs, itv_demux_debug_callstack, int, 0644);

MODULE_PARM_DESC(idemux_debug, "Turn on/off demux debugging (default:off).");
MODULE_PARM_DESC(idemux_debug_cs, "Turn on/off demux callstack debugging (default:off).");

#ifdef dprintk
#undef dprintk
#endif

#define dprintk(args...) \
	do { if(itv_demux_debug) { printk("[%s] ", __func__); printk(args); } } while (0)

#define DEBUG_CALLSTACK		\
	do { if(itv_demux_debug_callstack) { printk("[Call] ----> %s()\n", __func__); } } while (0);

static void itv_demux_destructor(itv_object_t *p_this)
{
	itv_demux_t *idemux = (itv_demux_t *)p_this;

	DEBUG_CALLSTACK

	if(idemux->imodule)
		itv_module_unneed(ITV_OBJECT(idemux), idemux->imodule);

	// put platform tsif operations
	if(idemux->tsif_ops)
		itv_platform_put_tsif_operations(idemux->tsif_ops);

	itv_ringbuffer_deinit(&idemux->stream_buffer);
}

static ssize_t itv_demux_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	demux_filter_handle_t *hfilter = (demux_filter_handle_t *)file->private_data;
	itv_demux_t *idemux;

	DEBUG_CALLSTACK

	if(!hfilter)
		return -ENODEV;

	idemux = hfilter->idemux;

	return idemux->demux_read(hfilter->handle, file->f_flags & O_NONBLOCK, buf, count, ppos);
}

static int itv_demux_do_ioctl(struct file *file, unsigned int cmd, void *parg)
{
	int ret = 0;
//	unsigned long arg = (unsigned long)parg;
	
	demux_filter_handle_t *hfilter = (demux_filter_handle_t *)file->private_data;
	itv_demux_t *idemux;

//	DEBUG_CALLSTACK

	if(!hfilter)
		return -ENODEV;

	idemux = hfilter->idemux;

	if(mutex_lock_interruptible(&idemux->mutex))
		return -ERESTARTSYS;

	switch(cmd) {
		case ITV_DMX_START:
//			dprintk("ITV_DMX_START\n");
			break;
		case ITV_DMX_STOP:
//			dprintk("ITV_DMX_STOP\n");
			break;
		case ITV_DMX_SET_SEC_FILTER:
//			ret = idemux->filter_set(hfilter->handle, ITV_DMX_FILTER_TYPE_TS_SEC, parg);
			break;
		case ITV_DMX_SET_PES_FILTER:
//			dprintk("ITV_DMX_SET_PES_FILTER\n");
			ret = idemux->filter_set(hfilter->handle, ITV_DMX_FILTER_TYPE_TS_PES, parg);
			break;
		default:
			ret = -EINVAL;
			break;
	}
	
	mutex_unlock(&idemux->mutex);
	
	return ret;
}

static int itv_demux_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
//	DEBUG_CALLSTACK

	return itv_usercopy(file, cmd, arg, itv_demux_do_ioctl);
}

static int itv_demux_open(struct inode *inode, struct file *file)
{	
	itv_demux_t *idemux = (itv_demux_t *)file->private_data;
	demux_filter_handle_t *hfilter;

	DEBUG_CALLSTACK

	if(!idemux)
		return -ENODEV;

	if(mutex_lock_interruptible(&idemux->mutex))
		return -ERESTARTSYS;

	hfilter = kzalloc(sizeof(*hfilter), GFP_KERNEL);
	if(!hfilter) {
		mutex_unlock(&idemux->mutex);
		return -EINVAL;
	}
	hfilter->idemux = idemux;
	
	hfilter->handle = idemux->filter_get(idemux);
	if(!hfilter->handle) {
		kfree(hfilter);
		mutex_unlock(&idemux->mutex);
		return -EINVAL;
	}
	
	file->private_data = hfilter;
	
	mutex_unlock(&idemux->mutex);
	
	return itv_device_open(idemux);
}

static int itv_demux_release(struct inode *inode, struct file *file)
{
	demux_filter_handle_t *hfilter = (demux_filter_handle_t *)file->private_data;
	itv_demux_t *idemux;

	DEBUG_CALLSTACK

	if(!hfilter)
		return -ENODEV;

	idemux = hfilter->idemux;

	if(mutex_lock_interruptible(&idemux->mutex))
		return -ERESTARTSYS;

	idemux->filter_put(hfilter->handle);
	kfree(hfilter);

	mutex_unlock(&idemux->mutex);
	
	return itv_device_release(idemux, file);
}

static unsigned int itv_demux_poll(struct file *file, poll_table *wait)
{
	demux_filter_handle_t *hfilter = (demux_filter_handle_t *)file->private_data;
	itv_demux_t *idemux;

//	DEBUG_CALLSTACK

	if(!hfilter)
		return -ENODEV;

	idemux = hfilter->idemux;

	return idemux->demux_poll(hfilter->handle, file, wait);
}

int itv_demux_start(itv_demux_t *p_idemux)
{
	DEBUG_CALLSTACK
		
	if(!p_idemux->demux_start)
		return -1;

	return p_idemux->demux_start(p_idemux);
}
EXPORT_SYMBOL(itv_demux_start);

int itv_demux_stop(itv_demux_t *p_idemux)
{
	DEBUG_CALLSTACK
		
	if(!p_idemux->demux_stop)
		return -1;

	return p_idemux->demux_stop(p_idemux);
}
EXPORT_SYMBOL(itv_demux_stop);

struct file_operations idemux_fops = {
	.owner 			= THIS_MODULE, 
	.read 			= itv_demux_read, 
	.unlocked_ioctl 	= itv_demux_ioctl, 
	.open 			= itv_demux_open, 
	.release 			= itv_demux_release, 
	.poll 			= itv_demux_poll
};

itv_demux_t *itv_demux_create(itv_object_t *p_this)
{
	char sz_module_name[10];

	itv_adapter_t *iadapter = (itv_adapter_t *)p_this;
	itv_demux_t *idemux;

	DEBUG_CALLSTACK

	if((idemux = (itv_demux_t *)itv_device_create(ITV_OBJECT(iadapter), sizeof(*idemux), "idemux")) == NULL) {
		eprintk("itv_device_create() is failed\n");
		return NULL;
	}

	mutex_init(&idemux->mutex);
	spin_lock_init(&idemux->lock);

	// get platform tsif operations
	idemux->tsif_ops = itv_platform_get_tsif_operations();
	if(!idemux->tsif_ops) {
		eprintk("tv_platform_get_tsif_operations() is failed\n");
		itv_device_destroy(idemux);
		return NULL;
	}

	itv_object_set_destructor(idemux, itv_demux_destructor);

	switch(iadapter->std_type) {
		case ITV_STD_TYPE_ATSC:
		case ITV_STD_TYPE_DVBC:
			sprintf(sz_module_name, "ts");
			break;
		default:
			eprintk("Can not support standard type : 0x%x\n", iadapter->std_type);
			itv_device_destroy(idemux);
			return NULL;
	}

	idemux->if_type = iadapter->if_type;
	idemux->tsif_mode = iadapter->tsif_mode;

	itv_ringbuffer_init(&idemux->stream_buffer, NULL, 8192);

	if((idemux->imodule = itv_module_need(ITV_OBJECT(idemux), "idemux", sz_module_name)) == NULL) {
		eprintk("itv_module_need() is failed\n");
		itv_device_destroy(idemux);
		return NULL;
	}
	
	idemux->psz_object_name = kstrdup("idemux", GFP_KERNEL);

	if(itv_device_register(ITV_OBJECT(idemux), ITV_DEV_TYPE_DEMUX, 0, &idemux_fops) != 0) {
		eprintk("itv_device_register() is failed\n");
		itv_device_destroy(idemux);
		return NULL;
	}

	dprintk("iTV demux device create and register success\n");

	return idemux;
}
EXPORT_SYMBOL(itv_demux_create);

void itv_demux_destroy(itv_demux_t *p_idemux)
{
	DEBUG_CALLSTACK
		
	if(!p_idemux) return;

	itv_device_destroy(p_idemux);

	dprintk("iTV demux device unregister and destroy success\n");
}
EXPORT_SYMBOL(itv_demux_destroy);
