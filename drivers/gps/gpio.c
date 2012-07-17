// ****************************************** //
// Temporary Driver for GPS on Android
// 
// Title  : UART5 and GPIO (GPS_ON)
// Target : TCC92xx
//
// Author : Hae Jung Kim
// ****************************************** //

#include <linux/init.h> 
#include <linux/module.h> 
#include <linux/kernel.h> 
  
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
  
#define GPS_GPIO_DEV_NAME      "gps_gpio"   // device name
#define GPS_GPIO_DEV_MAJOR     240          // device majer number
#define DEBUG_ON               0            // message for debug  1 : ON : message ON , 0 : OFF : massage OFF
                                
#define gps_dbg(fmt,arg...)     if(DEBUG_ON) printk("== gps debug == " fmt, ##arg)

static int      gps_major = GPS_GPIO_DEV_MAJOR;
static dev_t    dev;
static struct   cdev gps_cdev;
static struct   class *gps_class;
int gps_k_flag;


//  ************************************************************ //
//  Device Open : 
//  when open, yet not ativity UART port
//  GPS device is yet disable status.
//  ************************************************************ //
static int gps_gpio_open (struct inode *inode, struct file *filp)  
{
    gps_k_flag = 0;   
    // Set the Port Configure for the UART5
    // GPIO SETTING
    gps_dbg("gps_gpio_open\n");
#if defined(CONFIG_MACH_TCC9300) || defined(CONFIG_MACH_TCC8800) || defined(CONFIG_MACH_TCC8920)
    if(machine_is_m801_88() || machine_is_m803()) // GPIOG[4]
    {
        gpio_set_value(TCC_GPG(4), 0);
    }
    else if(machine_is_tcc8800() || machine_is_tcc8920())
    {
        gps_dbg("gps_gpio_open -> set_velue");
        gpio_set_value(TCC_GPEXT1(6), 0);
    }
#elif defined(CONFIG_MACH_TCC8900)
    if(machine_is_tcc8900())
    {
        gps_dbg("machine_is_tcc8900 : gps_gpio_open\n\n");
        gpio_set_value(TCC_GPD(25), 0);    
    }
#elif defined(CONFIG_MACH_M805_892X)
    if(machine_is_m805_892x())
    {
		if (system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
			gpio_set_value(TCC_GPE(14), 0);
		else
        	gpio_set_value(TCC_GPC(6), 0);
    }
#else

#endif


    gps_dbg("tcc92xx : gps_gpio_open\n\n");
    return 0;  
}

//  ************************************************************ //
//  Device Release : 
//  
//  
//  ************************************************************ //
static int gps_gpio_release (struct inode *inode, struct file *filp)  
{  
    gps_k_flag = 0;   
#if defined(CONFIG_MACH_TCC9300)||defined(CONFIG_MACH_TCC8800) || defined(CONFIG_MACH_TCC8920)
    if(machine_is_m801_88() || machine_is_m803()) // GPIOG[4]
    {
        gpio_set_value(TCC_GPG(4), 0);
    }
    else if(machine_is_tcc8800() || machine_is_tcc8920())
    {
        gpio_set_value(TCC_GPEXT1(6), 0);
    }
#elif defined(CONFIG_MACH_TCC8900)
    if(machine_is_tcc8900())
    {
        gpio_set_value(TCC_GPD(25), 0);    
    }
#elif defined(CONFIG_MACH_M805_892X)
    if(machine_is_m805_892x())
    {
    	if (system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
    		gpio_set_value(TCC_GPC(14), 0);
    	else
        	gpio_set_value(TCC_GPC(6), 0);
    }    
#endif
    gps_dbg("tcc92xx : gps_gpio_close\n");
    return 0;  
}  
  
//  ************************************************************ //
//  Device Release : 
//  
//  
//  ************************************************************ //
//static int gps_gpio_ioctl (struct inode *inode, struct file *filp,
//                           unsigned int cmd, unsigned long arg)  
static long gps_gpio_ioctl (struct file *filp, unsigned int cmd, void *arg)  
{
    gps_dbg("gps_gpio_ioctl\n");
    switch( cmd )  
    {  
#if defined(CONFIG_MACH_TCC9300)||defined(CONFIG_MACH_TCC8800) || defined(CONFIG_MACH_TCC8920)
        case 0 : // GPS_On
            gps_k_flag = 1;   
            if(machine_is_m801_88() || machine_is_m803()) // GPIOG[4]
            {
                gpio_set_value(TCC_GPG(4), 1);
            }
            else if(machine_is_tcc8800() || machine_is_tcc8920())
            {
                gpio_set_value(TCC_GPEXT1(6), 1);
            }
            gps_dbg("tccxxxx : gps_gpio_on\n");
            break;   
        case 1 : // GPS_Off
            gps_k_flag = 0;   
            if(machine_is_m801_88() || machine_is_m803()) // GPIOG[4]
            {
                gpio_set_value(TCC_GPG(4), 0);
            }
            else if(machine_is_tcc8800() || machine_is_tcc8920()) 
            {
                gpio_set_value(TCC_GPEXT1(6), 0);
            }
            gps_dbg("tccxxxx : gps_gpio_off\n");
            break;
#elif defined(CONFIG_MACH_TCC8900)
 
        case 0 : // GPS_On
            gps_k_flag = 1;   
            if(machine_is_tcc8900())
            {
                gpio_set_value(TCC_GPD(25), 1);   
            }
            break;   
        case 1 : // GPS_Off
            gps_k_flag = 0;   
            if(machine_is_tcc8900())
            {
                gpio_set_value(TCC_GPD(25), 0);   
            }
            break;            
#elif defined(CONFIG_MACH_M805_892X)
        case 0 : // GPS_On
            gps_k_flag = 1;
            if(machine_is_m805_892x())
            {
            	if (system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
                	gpio_set_value(TCC_GPE(14), 1);
                else
                	gpio_set_value(TCC_GPC(6), 1);
            }
            gps_dbg("tccxxxx : gps_gpio_on\n");
            break;	 
        case 1 : // GPS_Off
            gps_k_flag = 0;
            if(machine_is_m805_892x())
            {
				if (system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
                	gpio_set_value(TCC_GPE(14), 0);
                else
                	gpio_set_value(TCC_GPC(6), 0);
            }
            gps_dbg("tccxxxx : gps_gpio_off\n");
            break;
#else
        case 0 : // GPS_On
            gps_k_flag = 1;   

            break;   
        case 1 : // GPS_Off
            gps_k_flag = 0;   

            break;
#endif  // #if defined(CONFIG_MACH_TCC9300)
        default :
            break;
    };
    return 0;  
}  
  
//  ************************************************************ //
//  File Operation Struct :
//  
//  ************************************************************ //
static struct file_operations gps_gpio_fops =  
{  
    .owner    = THIS_MODULE,  
    .unlocked_ioctl    = gps_gpio_ioctl,  
    .open     = gps_gpio_open,       
    .release  = gps_gpio_release,    
};  

//  ************************************************************ //
//  Device Init :
//  
//  
//
//  ************************************************************ //
static int __init gps_gpio_init(void)  
{  
    int result;  
        gps_dbg("gps_gpio_init\n"); 

  
	if (0 == gps_major)
	{
		/* auto select a major */
		result = alloc_chrdev_region(&dev, 0, 1, GPS_GPIO_DEV_NAME);
		gps_major = MAJOR(dev);
	}
	else
	{
		/* use load time defined major number */
		dev = MKDEV(gps_major, 0);
		result = register_chrdev_region(dev, 1, GPS_GPIO_DEV_NAME);
	}

	memset(&gps_cdev, 0, sizeof(gps_cdev));

	/* initialize our char dev data */
	cdev_init(&gps_cdev, &gps_gpio_fops);

	/* register char dev with the kernel */
	result = cdev_add(&gps_cdev, dev, 1);
    
	if (0 != result)
	{
		unregister_chrdev_region(dev, 1);
		gps_dbg("Error registrating mali device object with the kernel\n");
	}

    gps_class = class_create(THIS_MODULE, GPS_GPIO_DEV_NAME);
    device_create(gps_class, NULL, MKDEV(gps_major, MINOR(dev)), NULL,
                  GPS_GPIO_DEV_NAME);

    if (result < 0)
        return result;  

#if defined(CONFIG_MACH_TCC9300) || defined(CONFIG_MACH_TCC8800) || defined(CONFIG_MACH_TCC8920)
    if(machine_is_m801_88() || machine_is_m803()) // GPIOG[4]
    {
        gps_dbg("GPS_PWREN on\n");
        tcc_gpio_config(TCC_GPG(4), GPIO_FN(0));
        gpio_request(TCC_GPG(4), "GPIO_PWREN");
        gpio_direction_output(TCC_GPG(4), 0);
    }
    else if(machine_is_tcc8800() || machine_is_tcc8920()) 
    {
        gps_dbg("gpio_direction_output__gps\n");
        gpio_direction_output(TCC_GPEXT1(6), 0);    // GPS Power On
    }
#elif defined(CONFIG_MACH_TCC8900)
    if(machine_is_tcc8900())
    {
        gps_dbg("GPS_8900_PWREN on\n");
        tcc_gpio_config(TCC_GPD(25), GPIO_FN(0));
        gpio_request(TCC_GPD(25), "GPIO_PWREN");
        gpio_set_value(TCC_GPD(25), 0);
    }
#elif defined(CONFIG_MACH_M805_892X)
    if(machine_is_m805_892x())
    {
        gps_dbg("GPS_PWREN on\n");
		if (system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005) {
			tcc_gpio_config(TCC_GPE(14), GPIO_FN(0));
			gpio_request(TCC_GPE(14), "GPIO_PWREN");
			gpio_direction_output(TCC_GPE(14), 0);

		} else {
			tcc_gpio_config(TCC_GPC(6), GPIO_FN(0));
			gpio_request(TCC_GPC(6), "GPIO_PWREN");
			gpio_direction_output(TCC_GPC(6), 0);
        }
    }
#endif

    gps_dbg("GPS driver loaded\n");

    return 0;  
}  

//  ************************************************************ //
//  Device Exit :
//
//  
//  ************************************************************ //
static void __exit gps_gpio_exit(void)  
{  

    gps_dbg("gps_gpio_exit");
	
    device_destroy(gps_class, MKDEV(gps_major, 0));
    class_destroy(gps_class);

    cdev_del(&gps_cdev);
    unregister_chrdev_region(dev, 1);

#if defined(CONFIG_MACH_TCC9300) || defined(CONFIG_MACH_TCC8800) || defined(CONFIG_MACH_TCC8920)
    // GPS Power off
    gps_dbg("GPS_PWREN off");
    if(machine_is_m801_88() || machine_is_m803()) // demo set
    {
        gpio_set_value(TCC_GPG(4), 0);
    }
    else if(machine_is_tcc8800() || machine_is_tcc8920())
    {
        gpio_direction_output(TCC_GPEXT1(6), 0);
    }
#elif defined(CONFIG_MACH_TCC8900)
    if(machine_is_tcc8900())    
    {
        gps_dbg("GPS_8900_PWREN off");
        gpio_set_value(TCC_GPD(25), 0);
    }
#elif defined(CONFIG_MACH_M805_892X)
    // GPS Power off
    gps_dbg("GPS_PWREN off");
    if(machine_is_m805_892x())
    {
		if (system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
			gpio_set_value(TCC_GPE(14), 0);
		else
			gpio_set_value(TCC_GPC(6), 0);
    }
#endif
    gps_dbg("GPS driver unloaded");
}  

EXPORT_SYMBOL(gps_k_flag);

module_init(gps_gpio_init);  
module_exit(gps_gpio_exit);  
  
MODULE_LICENSE("Dual BSD/GPL");  
