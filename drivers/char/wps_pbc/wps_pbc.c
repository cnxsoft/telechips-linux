#include <linux/init.h> 
#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/delay.h>
#include <linux/syscalls.h>
#include <linux/fs.h>            
#include <linux/mm.h>            
#include <linux/errno.h>         
#include <linux/types.h>         
#include <linux/fcntl.h>         
#include <linux/cdev.h>         
#include <linux/device.h>         
#include <linux/major.h>         
#include <linux/gpio.h>
#include <linux/slab.h>
#include <asm/uaccess.h>  
#include <asm/io.h>  
#include <asm/mach-types.h>
#include <mach/bsp.h>  

#define GPIO_WPS_PBC            TCC_GPE(20)

#define WPS_PBC_DEV_NAME     "wps_pbc"  
#define WPS_PBC_DEV_MAJOR     0               

static int      wps_pbc_major = WPS_PBC_DEV_MAJOR;
static dev_t    dev;
static struct   cdev  wps_pbc_cdev;
static struct   class *wps_pbc_class;
extern int axp192_ldo_set_wifipwd(int mode);
int wifi_stat = 0;
EXPORT_SYMBOL(wifi_stat);


static int wps_pbc_open (struct inode *inode, struct file *filp)  
{
    return 0;  
}

static int wps_pbc_release (struct inode *inode, struct file *filp)  
{  
    printk("%s\n", __func__); 
    return 0;  
}  

static ssize_t wps_pbc_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)  
{  
    int ret = gpio_get_value(GPIO_WPS_PBC);
	char* input = kmalloc(1, GFP_KERNEL);
	sprintf(input, "%d", ret);
	if(copy_to_user(buf,input,1)) {
		return 0;
	}
    return -1;  
}  

static struct file_operations wps_pbc_fops =  
{  
    .owner    = THIS_MODULE,  
    .open     = wps_pbc_open,       
    .read	  = wps_pbc_read,
    .release  = wps_pbc_release,    
};  

static int __init wps_pbc_init(void)  
{  
    int result;  

	if (0 == wps_pbc_major)
	{
		/* auto select a major */
		result = alloc_chrdev_region(&dev, 0, 1, WPS_PBC_DEV_NAME);
		wps_pbc_major = MAJOR(dev);
	}
	else
	{
		/* use load time defined major number */
		dev = MKDEV(wps_pbc_major, 0);
		result = register_chrdev_region(dev, 1, WPS_PBC_DEV_NAME);
	}

	memset(&wps_pbc_cdev, 0, sizeof(wps_pbc_cdev));

	/* initialize our char dev data */
	cdev_init(&wps_pbc_cdev, &wps_pbc_fops);

	/* register char dev with the kernel */
	result = cdev_add(&wps_pbc_cdev, dev, 1);
    
	if (0 != result)
	{
		unregister_chrdev_region(dev, 1);
		printk("Error registrating mali device object\n");
	}

    wps_pbc_class = class_create(THIS_MODULE, WPS_PBC_DEV_NAME);
    device_create(wps_pbc_class, NULL, MKDEV(wps_pbc_major, MINOR(dev)), NULL,
                  WPS_PBC_DEV_NAME);

	tcc_gpio_config(GPIO_WPS_PBC, GPIO_FN(0) );
	gpio_request(GPIO_WPS_PBC, "wps_pbc");
	
	gpio_direction_input(GPIO_WPS_PBC);
	
    if (result < 0)
        return result;  

    printk("wps_pbc_ctl driver loaded\n");

    return 0;  
}  

static void __exit wps_pbc_exit(void)  
{  
    printk("%s\n", __func__); 
	
    device_destroy(wps_pbc_class, MKDEV(wps_pbc_major, 0));
    class_destroy(wps_pbc_class);

    cdev_del(&wps_pbc_cdev);
    unregister_chrdev_region(dev, 1);

	gpio_direction_output(GPIO_WPS_PBC, 1);
			
    printk("wps_pbc_ctl driver unloaded");
}  

module_init(wps_pbc_init);  
module_exit(wps_pbc_exit);  
  
MODULE_LICENSE("Dual BSD/GPL");  
