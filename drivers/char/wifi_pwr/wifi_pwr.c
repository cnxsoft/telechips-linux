#include <linux/init.h> 
#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/delay.h>
#include <linux/syscalls.h>
#include <linux/earlysuspend.h>
#include <linux/fs.h>            
#include <linux/mm.h>            
#include <linux/errno.h>         
#include <linux/types.h>         
#include <linux/fcntl.h>         
#include <linux/cdev.h>         
#include <linux/device.h>         
#include <linux/major.h>         
#include <linux/gpio.h>

#include <asm/uaccess.h>  
#include <asm/io.h>  
#include <asm/mach-types.h>

#include <mach/bsp.h>  
#include <linux/regulator/machine.h>
#include <linux/regulator/consumer.h>


#define GPIO_WF_EN            TCC_GPE(3)


#define WIFI_GPIO_DEV_NAME     "wifi_pwr"  
#define WIFI_PWR_DEV_MAJOR     0      
#define DEBUG_ON               1           
              
//#define PMU_WIFI_SWITCH			                    
//#define wifi_dbg(fmt,arg...)     if(DEBUG_ON) printk("== wifi debug == " fmt, ##arg)

static int      wifi_major = WIFI_PWR_DEV_MAJOR;
static dev_t    dev;
static struct   cdev  wifi_cdev;
static struct   class *wifi_class;
static struct   regulator *vdd_wifi = NULL;
extern int axp192_ldo_set_wifipwd(int mode);
int wifi_stat = 0;
EXPORT_SYMBOL(wifi_stat);


static int wifi_pwr_open (struct inode *inode, struct file *filp)  
{
    return 0;  
}

static int wifi_pwr_release (struct inode *inode, struct file *filp)  
{  
	//if(system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
	//    vdd_wifi = NULL;
    //printk("%s\n", __func__); 
    return 0;  
}  
  
int wifi_pwr_ioctl_hw( unsigned int cmd)
{
	printk("wifi_pwr_ioctl_hw : %d\n", cmd);	
    switch( cmd )  
    {  
        case 1 : // WIFI_On
#ifdef CONFIG_REGULATOR
			if(system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005) {
		        if (vdd_wifi) {			
		            regulator_enable(vdd_wifi);
					printk("wifi_prw: power on\n");
				}else
					printk("vdd_wifi is null???\n");
			}else
#endif			
				gpio_direction_output(GPIO_WF_EN, 1);	
            break;   
        case 0 : // WIFI_Off
#ifdef CONFIG_REGULATOR	
			if(system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005) {
	        	if (vdd_wifi){
            		regulator_disable(vdd_wifi);
					printk("wifi_prw: power off\n");
				}else
					printk("vdd_wifi is null???\n");
			}else
#endif
			gpio_direction_output(GPIO_WF_EN, 0);	
            break;
        default :
            break;
    };
    return 0;
}
EXPORT_SYMBOL(wifi_pwr_ioctl_hw);

static long wifi_pwr_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    printk("%s, %d\n", __func__, cmd);
    switch( cmd )  
    {  
        case 1 : // WIFI_On
		    wifi_stat = 1;
            wifi_pwr_ioctl_hw(cmd);
            break;   
			
        case 0 : // WIFI_Off
		    wifi_stat = 0;
            wifi_pwr_ioctl_hw(cmd);
            break;

        default :
            break;
    };
    return 0;  
}  

static struct file_operations wifi_pwr_fops =  
{  
    .owner    = THIS_MODULE,  
    .unlocked_ioctl    = wifi_pwr_ioctl,  
    .open     = wifi_pwr_open,       
    .release  = wifi_pwr_release,    
};  

static int __init wifi_pwr_init(void)  
{  
    int result;  
    printk("%s\n", __func__); 

	if (0 == wifi_major)
	{
		/* auto select a major */
		result = alloc_chrdev_region(&dev, 0, 1, WIFI_GPIO_DEV_NAME);
		wifi_major = MAJOR(dev);
	}
	else
	{
		/* use load time defined major number */
		dev = MKDEV(wifi_major, 0);
		result = register_chrdev_region(dev, 1, WIFI_GPIO_DEV_NAME);
	}

	memset(&wifi_cdev, 0, sizeof(wifi_cdev));

	/* initialize our char dev data */
	cdev_init(&wifi_cdev, &wifi_pwr_fops);

	/* register char dev with the kernel */
	result = cdev_add(&wifi_cdev, dev, 1);
    
	if (0 != result)
	{
		unregister_chrdev_region(dev, 1);
		printk("Error registrating mali device object with the kernel__wifi\n");
	}

    wifi_class = class_create(THIS_MODULE, WIFI_GPIO_DEV_NAME);
    device_create(wifi_class, NULL, MKDEV(wifi_major, MINOR(dev)), NULL,
                  WIFI_GPIO_DEV_NAME);

#if defined(CONFIG_REGULATOR)
    if(system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005) {
        //vdd_wifi =  regulator_get(NULL, "vdd_wifi30");
        vdd_wifi =  regulator_get(NULL, "vdd_wifi");
        if( IS_ERR(vdd_wifi))
        {
            vdd_wifi = NULL;
            printk("vdd_wifi--get ERROR!!!\n");
			return -1;
        }
        regulator_enable(vdd_wifi);    // default is on
        regulator_disable(vdd_wifi);    // default is off
    }else   
#endif
    {
	tcc_gpio_config(GPIO_WF_EN, GPIO_FN(0) );
	gpio_request(GPIO_WF_EN,"wifi_pwr");
	
	gpio_direction_output(GPIO_WF_EN, 0);//default-openit.	
    }
    if (result < 0)
        return result;  

    printk("wifi_pwd_ctl driver loaded\n");

    return 0;  
}  

static void __exit wifi_pwr_exit(void)  
{  
    printk("%s\n", __func__); 
	
    device_destroy(wifi_class, MKDEV(wifi_major, 0));
    class_destroy(wifi_class);

    cdev_del(&wifi_cdev);
    unregister_chrdev_region(dev, 1);

#ifdef CONFIG_REGULATOR
	if(system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005) {
		if (vdd_wifi) {
			regulator_disable(vdd_wifi);
			//regulator_put(vdd_wifi);
		}
		vdd_wifi = NULL;
	}else
#endif
	gpio_direction_output(GPIO_WF_EN, 1);
			
    printk("wifi_pwr_ctl driver unloaded");
}  

module_init(wifi_pwr_init);  
module_exit(wifi_pwr_exit);  
  
MODULE_LICENSE("Dual BSD/GPL");  
