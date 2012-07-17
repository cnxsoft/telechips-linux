/*
 * File:        drivers/char/tcc_bt_dev.c
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/ioctl.h>
#include <linux/device.h>

#include <mach/bsp.h>
#include <asm/io.h>
#include <linux/delay.h>

#include <linux/tcc_bt_dev.h>
#include <mach/tcc_pca953x.h>

#include <linux/gpio.h>
#include <mach/gpio.h>
#include <asm/mach-types.h>
#include <linux/cpufreq.h>

#ifndef ON
#define ON 		1
#endif

#ifndef OFF
#define OFF 	0
#endif

#define DEV_NAME "tcc_bt_dev"
static struct class *bt_dev_class;

typedef struct {
	int module;  // 0x12:CSR, 0x34:Broadcom
	int TMP1;
	int TMP2;
} tcc_bt_info_t;

#if defined(CONFIG_CPU_FREQ)
extern struct tcc_freq_table_t gtBtClockLimitTable;
#endif

#if 0//def CONFIG_TCC_RDA_587X_MODULE_SUPPORT  , moved to arch/arm/mach-tcc8920/board-tcc-bluetooth.c
#if 0
#include <linux/i2c.h>

#define RDA5870E_REG_INDEX    16
#define RDA_RF_ADRW 0x2c
#define RDA_RF_ADRR 0x2d

#define RDA_CORE_ADRW 0x2a
#define RDA_CORE_ADRR 0x2b

struct rda5870e_device {
	struct i2c_client		*i2c_client;

	/* driver management */
	int 					users;

	/* buffer locking */
	struct mutex			mutex;

	uint8					rda_reg[RDA5870E_REG_INDEX];
	uint16					ChipID;
};

//================================================================
int rda5870e_Set_Register(struct rda5870e_device *device, int reqnr)
{
	int i;
	uint8 wData[RDA5870E_REG_INDEX];

	for (i=0; i<reqnr; i++) {
		wData[i] = device->rda_reg[i];
	}

	if (i2c_master_send(device->i2c_client, wData, reqnr) != reqnr) {
		printk(KERN_INFO "%s : i2c write error !!!\n", __func__);
		return -EIO;
	}

	return 0;
}

//================================================================
int rda5870e_Get_Register(struct rda5870e_device *device, uint8 *buffer, int count)
{
	if (i2c_master_recv(device->i2c_client, buffer, count) != count) {
		printk(KERN_INFO "%s : i2c read error !!!\n", __func__);
		return -EIO;
	}

	return 0;
}

uint16 RDA5870e_GetChipID(struct rda5870e_device *device)
{
	uint8 RDAFM_reg_data[6] = {0};
	uint16 cChipID;

	RDAFM_reg_data[0] = 0x00;
	RDAFM_reg_data[1] = 0x00;
	memcpy(device->rda_reg, RDAFM_reg_data, 2);
	rda5870e_Set_Register(device, 2);
	msleep(50);

	rda5870e_Get_Register(device, &(RDAFM_reg_data[0]), 6);
	cChipID = (RDAFM_reg_data[4]*0x100) + RDAFM_reg_data[5];

	return cChipID;
}
#endif

void RDA_bt_Power_On(void)
{
	volatile PGPIO pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);


	if(machine_is_tcc9300() || machine_is_tcc8800() || machine_is_tcc8920()) { 	 // #elif defined (CONFIG_MACH_TCC9300)
		//gpio_set_value(TCC_GPEXT1(7), 1);	 /* BT-ON Enable */
		gpio_set_value(TCC_GPEXT3(2), 1);   /* BT_WAKE Enable , pwr_ctrl*/
		gpio_set_value(TCC_GPEXT2(4), 0);   /* reset */
		msleep(500);
		gpio_set_value(TCC_GPEXT2(4), 1);   /* reset */
	} else if(machine_is_m801_88() || machine_is_m803()){
		gpio_set_value(TCC_GPA(13),1);
		//msleep(1);
	}
}

void RDA_bt_Power_Off(void)
{
	volatile PGPIO pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);

	if (machine_is_tcc9300() || machine_is_tcc8800()|| machine_is_tcc8920()) {	   // #elif defined (CONFIG_MACH_TCC9300)
		//gpio_set_value(TCC_GPEXT1(7), 0);   /* BT-ON Disable */
		gpio_set_value(TCC_GPEXT3(2), 0);	/* BT_WAKE Disable */
		gpio_set_value(TCC_GPEXT2(4), 0);	/* reset */
	}
	else if (machine_is_m801_88() || machine_is_m803())
	{
		gpio_set_value(TCC_GPA(13),0);
		//msleep(1);
	}
}
#endif

static int tcc_bt_dev_open(struct inode *inode, struct file *file)
{
	printk("[## BT ##] tcc_bt_dev_open\n");
    return 0;
}

static int tcc_bt_dev_release(struct inode *inode, struct file *file)
{
	printk("[## BT ##] tcc_bt_dev_release\n");
    return 0;
}

#if 0  // moved to arch/arm/mach-tcc8920/board-tcc-bluetooth.c
int tcc_bt_power_control(int on_off)
{
	volatile PGPIO pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);
    
	printk("[## BT ##] tcc_bt_power_control input[%d]\n", on_off);
	
	if(on_off == BT_DEV_ON)
	{	    
  	      if (machine_is_tcc8900()) {
            // power on
			gpio_set_value(TCC_GPB(25), 1);			/* bt_power, mapped on BT_WAKE */
			msleep(500);

            // reset
			gpio_set_value(TCC_GPEXT2(9), 0); /* /reset, mapped on BT_HWAKE */
			msleep(500);
			gpio_set_value(TCC_GPEXT2(9), 1);

			msleep(300);
		}
		else if (machine_is_tcc9300() || machine_is_tcc8800() || machine_is_tcc8920()) {      // #elif defined (CONFIG_MACH_TCC9300)
			#if defined (CONFIG_TCC_CSR_BC0406_MODULE_SUPPORT)|| defined(CONFIG_TCC_ATHEROS_AR3002_MODULE_SUPPORT)
			//gpio_set_value(TCC_GPEXT1(7), 1);   /* BT-ON Enable */
			gpio_set_value(TCC_GPEXT3(2), 1);   /* BT_WAKE Enable , pwr_ctrl*/
			gpio_set_value(TCC_GPEXT2(4), 0);   /* reset */
			msleep(500);
			gpio_set_value(TCC_GPEXT2(4), 1);   /* reset */
			#elif defined (CONFIG_TCC_RDA_587X_MODULE_SUPPORT)
			RDA_bt_Power_On();
			#endif	
		}
		else if (machine_is_m801_88() || machine_is_m803())
		{
			#if defined (CONFIG_TCC_CSR_BC0406_MODULE_SUPPORT)
			gpio_set_value(TCC_GPB(22),1); // BT WAKE
			gpio_set_value(TCC_GPA(13),0); // reset
			msleep(500);
			gpio_set_value(TCC_GPA(13),1); // reset
			#elif defined (CONFIG_TCC_RDA_587X_MODULE_SUPPORT)
			RDA_bt_Power_On();
			#endif	
		}
		else if(machine_is_tcc8800st())
		{
			gpio_set_value(TCC_GPC(31), 1); 		// BT pwr enable
			gpio_set_value(TCC_GPD(12), 0);			// BT rst 0
			msleep(500);
			gpio_set_value(TCC_GPD(12), 1);
		}
		else if (0/*machine_is_tcc9300ST()*/) {//#elif defined (CONFIG_MACH_TCC9300ST)
#if 0
			pGPIO->GPFEN	|= Hw1|Hw0;		   
			pGPIO->GPFDAT	|= Hw0;			/* BT-On Enbale */
			pGPIO->GPFDAT	&= ~Hw1;		/* BT_HWAKE Enable, reset */
			msleep(500);
			pGPIO->GPFDAT	|= Hw1;			/* BT_HWAKE Enable, reset */	
#endif
		}

	}
	else if(on_off == BT_DEV_OFF)
	{

  	      if (machine_is_tcc8900()) {
			// power off
			gpio_set_value(TCC_GPB(25), 0);			/* bt_power, mapped on BT_WAKE */
			msleep(500);

            // reset
			//gpio_direction_output(TCC_GPEXT2(9), 0);
		}else if (machine_is_tcc9300() || machine_is_tcc8800() || machine_is_tcc8920()) {      // #elif defined (CONFIG_MACH_TCC9300)
			#if defined (CONFIG_TCC_CSR_BC0406_MODULE_SUPPORT)|| defined(CONFIG_TCC_ATHEROS_AR3002_MODULE_SUPPORT)
			//gpio_set_value(TCC_GPEXT1(7), 0);   /* BT-ON Disable */
			gpio_set_value(TCC_GPEXT3(2), 0);   /* BT_WAKE Disable */
			gpio_set_value(TCC_GPEXT2(4), 0);   /* reset */
			#elif defined (CONFIG_TCC_RDA_587X_MODULE_SUPPORT)
			RDA_bt_Power_Off();
			#endif			
		}
		else if (machine_is_m801_88() || machine_is_m803())
		{
			#if defined (CONFIG_TCC_CSR_BC0406_MODULE_SUPPORT)
			gpio_set_value(TCC_GPB(22), 0); // BT WAKE
			gpio_set_value(TCC_GPA(13), 0); // reset
			#elif defined (CONFIG_TCC_RDA_587X_MODULE_SUPPORT)
			RDA_bt_Power_Off();
			#endif			
		}
		else if(machine_is_tcc8800st())
		{
			gpio_set_value(TCC_GPC(31), 0); 		// BT pwr enable
			gpio_set_value(TCC_GPD(12), 0);			// BT rst 0
		}
		else if (0/*machine_is_tcc9300ST()*/) {      // #elif defined (CONFIG_MACH_TCC9300ST)
#if 0
			pGPIO->GPFEN    |= Hw1|Hw0;
			pGPIO->GPFDAT   &= ~Hw0;                /* BT-On Enbale */
			pGPIO->GPFDAT   &= ~Hw1;                /* BT_HWAKE Enable, reset */
#endif
		}

	}
	else
	{
		printk("[## BT_ERR ##] input_error On[%d] Off[%d]\n", BT_DEV_ON, BT_DEV_OFF);
	}

	return 0;
}
#endif

static int tcc_bt_get_info(tcc_bt_info_t* arg)
{
	tcc_bt_info_t *info_t;
	int module_t;
	
	info_t = (tcc_bt_info_t *)arg;
	copy_from_user(info_t, (tcc_bt_info_t *)arg, sizeof(tcc_bt_info_t));

	module_t = 0;

  	if (machine_is_tcc8900() || machine_is_tcc9201() || machine_is_tcc9300()|| machine_is_tcc8800()|| machine_is_m801_88()|| machine_is_m803()|| machine_is_tcc8920())   {
		// CSR Only
		#if defined (CONFIG_TCC_CSR_BC0406_MODULE_SUPPORT)		
        	module_t = 0x12;
		#elif defined (CONFIG_TCC_RDA_587X_MODULE_SUPPORT)
	        module_t = 0x56;	
		#elif defined (CONFIG_TCC_BRCM_BCM4330_MODULE_SUPPORT)
		module_t = 0x34;	
		#elif defined (CONFIG_TCC_ATHEROS_AR3002_MODULE_SUPPORT)
			module_t = 0x78;	
		#endif
	}else if(machine_is_tcc9200s()){ //#elif defined (CONFIG_MACH_TCC9200S)	
		#if defined (CONFIG_TCC_BRCM_BCM4325D0_MODULE_SUPPORT)
		module_t = 0x34;
		#elif defined (CONFIG_TCC_CSR_BC0406_MODULE_SUPPORT)				
        module_t = 0x12;
		#endif		
	} //#endif
	printk("[## BT ##] module[0x%x]\n", module_t);

	info_t->module = module_t;

	copy_to_user((tcc_bt_info_t *)arg, info_t, sizeof(tcc_bt_info_t));

	return 0;
}


int tcc_bt_dev_ioctl(struct file *file,
                 unsigned int cmd, void *arg)
{
    int *parm1;
    
    memset(&parm1, 0, sizeof(int));
	printk("[## BT ##] tcc_bt_dev_ioctl cmd[%x] arg[%x]\n", cmd, arg);
	
    switch(cmd)
    {
	    //case IOCTL_BT_DEV_POWER:		// moved to arch/arm/mach-tcc8920/board-tcc-bluetooth.c	
    		//parm1 = (int*)arg;
    		//printk("[## BT ##] IOCTL_BT_DEV_POWER cmd[%x] parm1[%x]\n", cmd, *parm1);
		//	tcc_bt_power_control(*parm1);
	        // GPIO Control
	       // break;
	        
	     case IOCTL_BT_DEV_SPECIFIC:
			printk("[## BT ##] IOCTL_BT_DEV_SPECIFIC cmd[%x]\n", cmd);
			tcc_bt_get_info((tcc_bt_info_t*)arg);
			break;

             case IOCTL_BT_DEV_CLOCK_LIMIT:
		parm1 = (int*)arg;
		printk("[## BT ##] IOCTL_BT_SET_LIMIT_CLOCK cmd[%x] parm1[%x]\n", cmd, *parm1);
        	tcc_cpufreq_set_limit_table(&gtBtClockLimitTable, TCC_FREQ_LIMIT_BT, *parm1);
		break;

        
	    default :
    		printk("[## BT ##] tcc_bt_dev_ioctl cmd[%x]\n", cmd);
	        break;
    }

    return 0;
}

struct file_operations tcc_bt_dev_ops = {
    .owner      = THIS_MODULE,
    .unlocked_ioctl      = tcc_bt_dev_ioctl,
    .open       = tcc_bt_dev_open,
    .release    = tcc_bt_dev_release,
};


int init_module(void)
{
    int ret;

	printk("[## BT ##] init_module\n");
    ret = register_chrdev(BT_DEV_MAJOR_NUM, DEV_NAME, &tcc_bt_dev_ops);

	bt_dev_class = class_create(THIS_MODULE, DEV_NAME);
	device_create(bt_dev_class, NULL, MKDEV(BT_DEV_MAJOR_NUM, BT_DEV_MINOR_NUM), NULL, DEV_NAME);

#if 0  //  moved to arch/arm/mach-tcc8920/board-tcc-bluetooth.c
	if(machine_is_tcc8900()){
	    gpio_request(TCC_GPB(25), "bt_power");
	    gpio_request(TCC_GPEXT2(9), "bt_reset");
		gpio_direction_output(TCC_GPB(25), 0); // output
		gpio_direction_output(TCC_GPEXT2(9), 0);
	}else if(machine_is_tcc9300() || machine_is_tcc8800()|| machine_is_tcc8920()) {      // #elif defined (CONFIG_MACH_TCC9300)
			//gpio_set_value(TCC_GPEXT1(7), 0);   /* BT-ON Disable */
		gpio_request(TCC_GPEXT3(2), "bt_wake");
	    gpio_request(TCC_GPEXT2(4), "bt_reset");
		gpio_direction_output(TCC_GPEXT3(2), 0); // output
		gpio_direction_output(TCC_GPEXT2(4), 0);
#if defined(CONFIG_TCC_CSR_BC0406_MODULE_SUPPORT) && defined(CONFIG_TCC_CSR_HOST_WAKE_UP)
		tcc_gpio_config(TCC_GPB(31), GPIO_FN(0));
		gpio_request(TCC_GPEXT3(3), "bt_hwake");
		gpio_direction_input(TCC_GPEXT3(3));
#endif // for CSR Bluetooth host wake up
	}
	else if(machine_is_m801_88() || machine_is_m803())
	{
		#if defined(CONFIG_TCC_RDA_587X_MODULE_SUPPORT)
		gpio_request(TCC_GPA(13), "LDO_ON");
		tcc_gpio_config(TCC_GPA(13), GPIO_FN(0));
		gpio_direction_output(TCC_GPA(13), 0);
		#else
		gpio_request(TCC_GPA(13), "bt_reset");
		gpio_request(TCC_GPB(22), "BT WAKE");
		gpio_direction_output(TCC_GPA(13), 0); // output
		gpio_direction_output(TCC_GPB(22), 0); // output
		#endif

	}
	else if(machine_is_tcc8800st())
	{
		gpio_request(TCC_GPC(31), "bt_power");
		gpio_request(TCC_GPD(12), "bt_reset");
		gpio_direction_output(TCC_GPC(31), 0); // output
		gpio_direction_output(TCC_GPD(12), 0); // output
	}
#endif

    if(ret < 0){
        printk("[## BT ##] [%d]fail to register the character device\n", ret);
        return ret;
    }

    return 0;
}

void cleanup_module(void)
{
	printk("[## BT ##] cleanup_module\n");
    unregister_chrdev(BT_DEV_MAJOR_NUM, DEV_NAME);
}


late_initcall(init_module);
module_exit(cleanup_module);


MODULE_AUTHOR("Telechips Inc. linux@telechips.com");
MODULE_DESCRIPTION("TCC_BT_DEV");
MODULE_LICENSE("GPL");

