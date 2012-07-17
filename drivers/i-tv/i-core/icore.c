#include <linux/mutex.h>
#include <i-tv/itv_common.h>
#include <i-tv/itv_adapter.h>
#include <i-tv/itv_core.h>

static int itv_core_debug;
static int itv_core_debug_callstack;

module_param_named(icore_debug, itv_core_debug, int, 0644);
module_param_named(icore_debug_cs, itv_core_debug_callstack, int, 0644);

MODULE_PARM_DESC(icore_debug, "Turn on/off core debugging (default:off).");
MODULE_PARM_DESC(icore_debug_cs, "Turn on/off core callstack debugging (default:off).");

#ifdef dprintk
#undef dprintk
#endif

static DEFINE_MUTEX(itv_core_mutex);

#define dprintk(args...) \
	do { if(itv_core_debug) { printk("[%s] ", __func__); printk(args); } } while (0)

#define DEBUG_CALLSTACK		\
	do { if(itv_core_debug_callstack) { printk("[Call] ----> %s()\n", __func__); } } while (0);

itv_core_t *p_icore = NULL;
EXPORT_SYMBOL(p_icore);

int itv_usercopy(struct file *file, unsigned int cmd, unsigned long arg, 
	int (*func)(struct file *file, unsigned int cmd, void *arg))
{
	char sbuf[128];
	void *mbuf = NULL;
	void *parg = NULL;
	int err = -EINVAL;

//	DEBUG_CALLSTACK

	switch(_IOC_DIR(cmd)) {
		case _IOC_NONE:
			parg = (void *)arg;
			break;
		case _IOC_READ:
		case _IOC_WRITE:
		case (_IOC_WRITE | _IOC_READ):
			if(_IOC_SIZE(cmd) <= sizeof(sbuf)) {
				parg = sbuf;
			}
			else {
				mbuf = kmalloc(_IOC_SIZE(cmd), GFP_KERNEL);
				if(mbuf == NULL)
					return -ENOMEM;
				parg = mbuf;
			}

			err = -EFAULT;
			if(copy_from_user(parg, (void __user *)arg, _IOC_SIZE(cmd)))
				goto out;
			break;
	}

	if((err = func(file, cmd, parg)) == -ENOIOCTLCMD)
		err = -EINVAL;

	if(err < 0)
		goto out;

	switch(_IOC_DIR(cmd)) {
		case _IOC_READ:
		case (_IOC_WRITE | _IOC_READ):
			if(copy_to_user((void __user *)arg, parg, _IOC_SIZE(cmd)))
				err = -EFAULT;
			break;
	}

out:
	kfree(mbuf);
	return err;
}

int itv_get_adapter_id(itv_core_t *p_this)
{
	int i;
	int adapter_id = ITV_UNSET;

//	DEBUG_CALLSTACK

	spin_lock(&p_this->lock);
	for(i = 0; i < ITV_MAX_ADAPTERS; i++) {
		if(p_this->adapter_ids[i] == ITV_UNSET) {
			adapter_id = p_this->adapter_ids[i] = i;
			break;
		}
	}
	spin_unlock(&p_this->lock);

	return adapter_id;
}
EXPORT_SYMBOL(itv_get_adapter_id);

void itv_put_adapter_id(itv_core_t *p_this, int adapter_id)
{
//	DEBUG_CALLSTACK
		
	spin_lock(&p_this->lock);
	p_this->adapter_ids[adapter_id] = ITV_UNSET;
	spin_unlock(&p_this->lock);
}
EXPORT_SYMBOL(itv_put_adapter_id);

static int itv_core_open(struct inode *inode, struct file *file)
{
	int i;

	itv_object_list_t *p_list;
	itv_device_t *idev = NULL;

//	DEBUG_CALLSTACK

	mutex_lock(&itv_core_mutex);

	p_list = itv_list_children(p_icore);
	if(p_list->i_count == 0) {
		itv_list_release(p_list);
		mutex_unlock(&itv_core_mutex);
		return -ENODEV;
	}
	for(i = 0; i < p_list->i_count; i++) {
		idev = itv_adapter_find_device(p_list->pp_object[i], iminor(inode));
		if(idev != NULL) break;
	}

	if(idev != NULL) {
		int err = 0;
		const struct file_operations *old_fops;

		file->private_data = idev;
		old_fops = file->f_op;
		file->f_op = fops_get(&idev->fops);
		if(file->f_op->open)
			err = file->f_op->open(inode, file);
		if(err) {
			fops_put(file->f_op);
			file->f_op = fops_get(old_fops);
		}
		fops_put(old_fops);
		itv_list_release(p_list);
		mutex_unlock(&itv_core_mutex);
		return err;
	}
	itv_list_release(p_list);

	mutex_unlock(&itv_core_mutex);
	
	return -ENODEV;
}

static int __init itv_core_init(void)
{
	int i;
	int retval;
	dev_t dev = MKDEV(ITV_MAJOR, 0);
	itv_core_t *icore;
	
	DEBUG_CALLSTACK

	if((icore = (itv_core_t *)itv_object_create(NULL, sizeof(*icore), ITV_OBJECT_CORE, "icore")) == NULL) {
		eprintk("itv_object_create() is failed\n");
		retval =  -1;
		goto out;
	}

	icore->psz_object_name = kstrdup("icore", GFP_KERNEL);
	spin_lock_init(&icore->lock);
	for(i = 0; i < ITV_MAX_ADAPTERS; i++) {
		icore->adapter_ids[i] = ITV_UNSET; 
	}

	if((retval = register_chrdev_region(dev, ITV_MINORS, "itv")) != 0) {
		eprintk("unable to get major %d\n", ITV_MAJOR);
		goto error1;
	}

	icore->fops.owner = THIS_MODULE;
	icore->fops.open = itv_core_open;

	cdev_init(&icore->cdev, &icore->fops);
	if((retval = cdev_add(&icore->cdev, dev, ITV_MINORS)) != 0) {
		eprintk("unable register character device\n");
		goto error2;
	}

	icore->class = class_create(THIS_MODULE, "itv");
	if(IS_ERR(icore->class)) {
		retval = PTR_ERR(icore->class);
		goto error3;
	}

	p_icore = icore;

	dprintk("iTV core init success\n");

out:
	return retval;

error1:
	itv_object_release(icore);
	goto out;

error2:
	unregister_chrdev_region(dev, ITV_MINORS);
	goto error1;

error3:
	cdev_del(&icore->cdev);
	goto error2;
}

static void __exit itv_core_exit(void)
{
	itv_core_t *icore = p_icore;

	DEBUG_CALLSTACK

	if(!icore) return;

	class_destroy(icore->class);
	cdev_del(&icore->cdev);
	unregister_chrdev_region(MKDEV(ITV_MAJOR, 0), ITV_MINORS);
	itv_object_release(icore);
	p_icore = NULL;

	dprintk("iTV core exit success\n");
}

subsys_initcall(itv_core_init);
module_exit(itv_core_exit);

MODULE_AUTHOR("JP");
MODULE_LICENSE("GPL");
