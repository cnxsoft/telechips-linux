#include <i-tv/itv_common.h>
#include <i-tv/itv_adapter.h>
#include <i-tv/itv_demux.h>
#include <i-tv/itv_stream.h>

#include "istream.h"

static int itv_stream_debug;
static int itv_stream_debug_callstack;

module_param_named(istream_debug, itv_stream_debug, int, 0644);
module_param_named(istream_debug_cs, itv_stream_debug_callstack, int, 0644);

MODULE_PARM_DESC(istream_debug, "Turn on/off stream debugging (default:off).");
MODULE_PARM_DESC(istream_debug_cs, "Turn on/off stream callstack debugging (default:off).");

#ifdef dprintk
#undef dprintk
#endif

#define dprintk(args...) \
	do { if(itv_stream_debug) { printk("[%s] ", __func__); printk(args); } } while (0)

#define DEBUG_CALLSTACK		\
	do { if(itv_stream_debug_callstack) { printk("[Call] ----> %s()\n", __func__); } } while (0);

static void itv_stream_destructor(itv_object_t *p_this)
{
	itv_stream_t *istream = (itv_stream_t *)p_this;

	DEBUG_CALLSTACK

	if(istream->idemux)
		itv_object_release(istream->idemux);
}

static ssize_t itv_stream_buffer_read(itv_ringbuffer_t *src, int non_blocking, char __user *buf, size_t count, loff_t *ppos)
{
	size_t todo;
	ssize_t avail;
	ssize_t ret = 0;

//	DEBUG_CALLSTACK

	if(!src->data)
		return 0;

	if(src->error) {
		ret = src->error;
		itv_ringbuffer_flush(src);
		return ret;
	}

	for(todo = count; todo > 0; todo -= ret) {
		if(non_blocking && itv_ringbuffer_empty(src)) {
			ret = -EWOULDBLOCK;
			break;
		}

		ret = wait_event_interruptible(src->queue, !itv_ringbuffer_empty(src) || (src->error != 0));
		if(ret < 0) break;

		if(src->error) {
			ret = src->error;
			itv_ringbuffer_flush(src);
			break;
		}

		avail = itv_ringbuffer_avail(src);
		if(avail > todo)
			avail = todo;

		ret = itv_ringbuffer_read_user(src, buf, avail);
		if(ret < 0) break;

		buf += ret;
	}

	return (count - todo) ? (count - todo) : ret;
}	

static int itv_stream_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	itv_stream_t *istream = (itv_stream_t *)file->private_data;
	itv_demux_t *idemux;

//	DEBUG_CALLSTACK

	if(!istream)
		return -ENODEV;

	idemux = (itv_demux_t *)istream->idemux;

	if(idemux->exit)
		return -ENODEV;

	return itv_stream_buffer_read(&idemux->stream_buffer, file->f_flags & O_NONBLOCK, buf, count, ppos);
}

static int itv_stream_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	DEBUG_CALLSTACK

	return 0;
}

static int itv_stream_do_ioctl(struct file *file, unsigned int cmd, void *parg)
{
	DEBUG_CALLSTACK
		
	return 0;
}

static int itv_stream_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	DEBUG_CALLSTACK
		
	return itv_usercopy(file, cmd, arg, itv_stream_do_ioctl);
}

static int itv_stream_open(struct inode *inode, struct file *file)
{
	itv_stream_t *istream = (itv_stream_t *)file->private_data;
	itv_demux_t *idemux;
	
	DEBUG_CALLSTACK

	if(!istream)
		return -ENODEV;

	idemux = (itv_demux_t *)istream->idemux;

	if(mutex_lock_interruptible(&idemux->mutex))
		return -ERESTARTSYS;

	if((file->f_flags & O_ACCMODE) == O_RDWR) {
		mutex_unlock(&idemux->mutex);
		return -EOPNOTSUPP;
	}

	if((file->f_flags & O_ACCMODE) == O_RDONLY) {
		void *mem = vmalloc(STREAM_BUFFER_SIZE);
		if(!mem) {
			mutex_unlock(&idemux->mutex);
			return -ENOMEM;
		}
		itv_ringbuffer_init(&idemux->stream_buffer, mem, STREAM_BUFFER_SIZE);
	}

	if((file->f_flags & O_ACCMODE) == O_WRONLY) {
		mutex_unlock(&idemux->mutex);
		return -EOPNOTSUPP;
	}

	mutex_unlock(&idemux->mutex);

	return itv_device_open(istream);
}

static int itv_stream_release(struct inode *inode, struct file *file)
{
	itv_stream_t *istream = (itv_stream_t *)file->private_data;
	itv_demux_t *idemux;

	DEBUG_CALLSTACK

	if(!istream)
		return -ENODEV;
	
	idemux = (itv_demux_t *)istream->idemux;

	mutex_lock(&idemux->mutex);

	if((file->f_flags & O_ACCMODE) == O_WRONLY) {
		mutex_unlock(&idemux->mutex);
		return -EOPNOTSUPP;
	}
	
	if((file->f_flags & O_ACCMODE) == O_RDONLY) {
		if(idemux->stream_buffer.data) {
			void *mem = idemux->stream_buffer.data;
			mb();
			spin_lock_irq(&idemux->lock);
			idemux->stream_buffer.data = NULL;
			spin_unlock_irq(&idemux->lock);
			vfree(mem);
		}
	}

	mutex_unlock(&idemux->mutex);

	return itv_device_release(istream, file);
}

static unsigned int itv_stream_poll(struct file *file, poll_table *wait)
{	
	unsigned int mask = 0;

	itv_stream_t *istream = (itv_stream_t *)file->private_data;
	itv_demux_t *idemux;

//	DEBUG_CALLSTACK

	if(!istream)
		return -ENODEV;
	
	idemux = (itv_demux_t *)istream->idemux;

	poll_wait(file, &idemux->stream_buffer.queue, wait);

	if((file->f_flags & O_ACCMODE) == O_RDONLY) {
		if(idemux->stream_buffer.error)
			mask |= (POLLIN | POLLRDNORM | POLLPRI | POLLERR);

		if(!itv_ringbuffer_empty(&idemux->stream_buffer))
			mask |= (POLLIN | POLLRDNORM | POLLPRI);
	}
	else
		mask |= (POLLOUT | POLLWRNORM | POLLPRI);

	return mask;
}

struct file_operations istream_fops = {
	.owner 			= THIS_MODULE, 
	.read 			= itv_stream_read, 
	.write 			= itv_stream_write, 
	.unlocked_ioctl 	= itv_stream_ioctl, 
	.open 			= itv_stream_open, 
	.release 			= itv_stream_release, 
	.poll 			= itv_stream_poll
};

itv_stream_t *itv_stream_create(itv_object_t *p_this)
{
	itv_adapter_t *iadapter = (itv_adapter_t *)p_this;
	itv_stream_t *istream;

	DEBUG_CALLSTACK

	if(!iadapter->idemux) {
		eprintk("demux object is NULL\n");
		return NULL;
	}

	if((istream = (itv_stream_t *)itv_device_create(ITV_OBJECT(iadapter), sizeof(*istream), "istream")) == NULL) {
		eprintk("itv_device_create() is failed\n");
		return NULL;
	}

	itv_object_set_destructor(istream, itv_stream_destructor);

	// demux object
	istream->idemux = itv_object_hold(iadapter->idemux);

	istream->psz_object_name = kstrdup("istream", GFP_KERNEL);

	if(itv_device_register(ITV_OBJECT(istream), ITV_DEV_TYPE_STREAM, 0, &istream_fops) != 0) {
		eprintk("itv_device_register() is failed\n");
		itv_device_destroy(istream);
		return NULL;
	}

	dprintk("iTV stream device create and register success\n");

	return istream;
}
EXPORT_SYMBOL(itv_stream_create);

void itv_stream_destroy(itv_stream_t *p_istream)
{
	DEBUG_CALLSTACK
	
	if(!p_istream) return;

	itv_device_destroy(p_istream);

	dprintk("iTV stream device unregister and destroy success\n");
}
EXPORT_SYMBOL(itv_stream_destroy);
