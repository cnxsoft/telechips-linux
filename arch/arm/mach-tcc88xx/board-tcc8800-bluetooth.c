/*
 * Bluetooth Broadcomm  and low power control via GPIO
 *
 *  Copyright (C) 2011 Google, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/hrtimer.h>
#include <linux/irq.h>
#include <linux/rfkill.h>

#include <mach/bsp.h>
#include <asm/io.h>
#include <linux/delay.h>

#include <linux/platform_device.h>
#include <linux/wakelock.h>
#include <mach/gpio.h>
#include <asm/mach-types.h>
//#include <plat/serial.h>
//#include <plat/board-tuna-bluetooth.h>
//#include <linux/regulator/driver.h>

#define BT_REG_GPIO 103
#define BT_RESET_GPIO 42

#define BT_WAKE_GPIO 27
#define BT_HOST_WAKE_GPIO 177


static struct rfkill *bt_rfkill;
static struct regulator *clk32kaudio_reg;
static bool bt_enabled;
static bool host_wake_uart_enabled;
static bool wake_uart_enabled;

struct bcm_bt_lpm {
	int wake;
	int host_wake;

	struct hrtimer enter_lpm_timer;
	ktime_t enter_lpm_delay;

	struct uart_port *uport;

	struct wake_lock wake_lock;
	char wake_lock_name[100];
} bt_lpm;

static int bt_enable_flag = 0;

#ifdef CONFIG_TCC_RDA_587X_MODULE_SUPPORT
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


	if(machine_is_tcc8800() || machine_is_tcc8920()) { 	 // #elif defined (CONFIG_MACH_TCC9300)
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

	if (machine_is_tcc8800()|| machine_is_tcc8920()) {	   // #elif defined (CONFIG_MACH_TCC9300)
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


static int tcc_bt_rfkill_set_power(void *data, bool blocked)
{
	// rfkill_ops callback. Turn transmitter on when blocked is false

	printk("[## BT ##] tcc_bt_rfkill_set_power [%d]\n", blocked);
	
	if (!blocked && (bt_enable_flag == 0)) {
	      bt_enable_flag = 1;
              if (machine_is_tcc8800() || machine_is_tcc8920()) {      // #elif defined (CONFIG_MACH_TCC9300)
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
	} else if(blocked && (bt_enable_flag == 1)) {
		bt_enable_flag = 0;
		if (machine_is_tcc8800() || machine_is_tcc8920()) {      // #elif defined (CONFIG_MACH_TCC9300)
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

	}

	bt_enabled = !blocked;

	return 0;
}

static const struct rfkill_ops tcc_bt_rfkill_ops = {
	.set_block = tcc_bt_rfkill_set_power,
};

static void set_wake_locked(int wake)
{
/*
	bt_lpm.wake = wake;

	if (!wake)
		wake_unlock(&bt_lpm.wake_lock);

	if (!wake_uart_enabled && wake)
		omap_uart_enable(2);

	gpio_set_value(BT_WAKE_GPIO, wake);

	if (wake_uart_enabled && !wake)
		omap_uart_disable(2);

	wake_uart_enabled = wake;
	*/
}

static enum hrtimer_restart enter_lpm(struct hrtimer *timer) {
/*	unsigned long flags;
	spin_lock_irqsave(&bt_lpm.uport->lock, flags);
	set_wake_locked(0);
	spin_unlock_irqrestore(&bt_lpm.uport->lock, flags);
*/
	return HRTIMER_NORESTART;
}

void bcm_bt_lpm_exit_lpm_locked(struct uart_port *uport) {
/*	bt_lpm.uport = uport;

	hrtimer_try_to_cancel(&bt_lpm.enter_lpm_timer);

	set_wake_locked(1);

	hrtimer_start(&bt_lpm.enter_lpm_timer, bt_lpm.enter_lpm_delay,
		HRTIMER_MODE_REL);
		*/
}
EXPORT_SYMBOL(bcm_bt_lpm_exit_lpm_locked);

static void update_host_wake_locked(int host_wake)
{
/*
	if (host_wake == bt_lpm.host_wake)
		return;

	bt_lpm.host_wake = host_wake;

	if (host_wake) {
		wake_lock(&bt_lpm.wake_lock);
		if (!host_wake_uart_enabled)
			omap_uart_enable(2);
	} else  {
		if (host_wake_uart_enabled)
			omap_uart_disable(2);
		// Take a timed wakelock, so that upper layers can take it.
		// The chipset deasserts the hostwake lock, when there is no
		// more data to send.
		wake_lock_timeout(&bt_lpm.wake_lock, HZ/2);
	}

	host_wake_uart_enabled = host_wake;
*/
}

static irqreturn_t host_wake_isr(int irq, void *dev)
{
/*
	int host_wake;
	unsigned long flags;

	host_wake = gpio_get_value(BT_HOST_WAKE_GPIO);
	irq_set_irq_type(irq, host_wake ? IRQF_TRIGGER_LOW : IRQF_TRIGGER_HIGH);

	if (!bt_lpm.uport) {
		bt_lpm.host_wake = host_wake;
		return IRQ_HANDLED;
	}

	spin_lock_irqsave(&bt_lpm.uport->lock, flags);
	update_host_wake_locked(host_wake);
	spin_unlock_irqrestore(&bt_lpm.uport->lock, flags);
*/
	return IRQ_HANDLED;
}

static int bcm_bt_lpm_init(struct platform_device *pdev)
{
	int irq;
	int ret;
	int rc;
#if 0
	rc = gpio_request(BT_WAKE_GPIO, "bcm4330_wake_gpio");
	if (unlikely(rc)) {
		return rc;
	}

	rc = gpio_request(BT_HOST_WAKE_GPIO, "bcm4330_host_wake_gpio");
	if (unlikely(rc)) {
		gpio_free(BT_WAKE_GPIO);
		return rc;
	}

	hrtimer_init(&bt_lpm.enter_lpm_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	bt_lpm.enter_lpm_delay = ktime_set(1, 0);  /* 1 sec */
	bt_lpm.enter_lpm_timer.function = enter_lpm;

	bt_lpm.host_wake = 0;

	irq = gpio_to_irq(BT_HOST_WAKE_GPIO);
	ret = request_irq(irq, host_wake_isr, IRQF_TRIGGER_HIGH,
		"bt host_wake", NULL);
	if (ret) {
		gpio_free(BT_WAKE_GPIO);
		gpio_free(BT_HOST_WAKE_GPIO);
		return ret;
	}

	ret = irq_set_irq_wake(irq, 1);
	if (ret) {
		gpio_free(BT_WAKE_GPIO);
		gpio_free(BT_HOST_WAKE_GPIO);
		return ret;
	}

	gpio_direction_output(BT_WAKE_GPIO, 0);
	gpio_direction_input(BT_HOST_WAKE_GPIO);

	snprintf(bt_lpm.wake_lock_name, sizeof(bt_lpm.wake_lock_name),
			"BTLowPower");
	wake_lock_init(&bt_lpm.wake_lock, WAKE_LOCK_SUSPEND,
			 bt_lpm.wake_lock_name);
#endif
	return 0;
}

static int tcc_bluetooth_probe(struct platform_device *pdev)
{
	int rc = 0;
	int ret = 0;

	printk("[## BT ##] tcc_bluetooth_probe\n");
	if( machine_is_tcc8800()|| machine_is_tcc8920()) {      // #elif defined (CONFIG_MACH_TCC9300)
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
/*
	rc = gpio_request(BT_RESET_GPIO, "bcm4330_nreset_gpip");
	if (unlikely(rc)) {
		return rc;
	}

	rc = gpio_request(BT_REG_GPIO, "bcm4330_nshutdown_gpio");
	if (unlikely(rc)) {
		gpio_free(BT_RESET_GPIO);
		return rc;
	}
*/
	bt_rfkill = rfkill_alloc("Telechips Bluetooth", &pdev->dev,
				RFKILL_TYPE_BLUETOOTH, &tcc_bt_rfkill_ops,
				NULL);

	if (unlikely(!bt_rfkill)) {
		//gpio_free(BT_RESET_GPIO);
		//gpio_free(BT_REG_GPIO);
		printk("[## BT ##] rfkill_alloc failed \n");
		return -ENOMEM;
	}

	rc = rfkill_register(bt_rfkill);

	if (unlikely(rc)) {
		printk("[## BT ##] rfkill_register failed \n");
		rfkill_destroy(bt_rfkill);
		//gpio_free(BT_RESET_GPIO);
		//gpio_free(BT_REG_GPIO);
		return -1;
	}

	printk("[## BT ##] rfkill_register Telechips Bluetooth \n");

	rfkill_set_states(bt_rfkill, true, false);
	tcc_bt_rfkill_set_power(NULL, true);

#if defined (CONFIG_TCC_BRCM_BCM4330_MODULE_SUPPORT)
	ret = bcm_bt_lpm_init(pdev);
	if (ret) {
		rfkill_unregister(bt_rfkill);
		rfkill_destroy(bt_rfkill);

		//gpio_free(BT_RESET_GPIO);
		//gpio_free(BT_REG_GPIO);
	}
#endif

	return ret;
}

static int tcc_bluetooth_remove(struct platform_device *pdev)
{
	rfkill_unregister(bt_rfkill);
	rfkill_destroy(bt_rfkill);

	printk("[## BT ##] tcc_bluetooth_remove \n");
/*
	gpio_free(BT_REG_GPIO);
	gpio_free(BT_RESET_GPIO);
	gpio_free(BT_WAKE_GPIO);
	gpio_free(BT_HOST_WAKE_GPIO);
	regulator_put(clk32kaudio_reg);

	wake_lock_destroy(&bt_lpm.wake_lock);
	*/
	return 0;
}

int tcc_bluetooth_suspend(struct platform_device *pdev, pm_message_t state)
{
	printk("[## BT ##] tcc_bluetooth_suspend \n");
/*
	int irq = gpio_to_irq(BT_HOST_WAKE_GPIO);
	int host_wake;

	disable_irq(irq);
	host_wake = gpio_get_value(BT_HOST_WAKE_GPIO);

	if (host_wake) {
		enable_irq(irq);
		return -EBUSY;
	}
*/
	return 0;
}

int tcc_bluetooth_resume(struct platform_device *pdev)
{
	printk("[## BT ##] tcc_bluetooth_resume \n");
/*
	int irq = gpio_to_irq(BT_HOST_WAKE_GPIO);
	enable_irq(irq);
*/
	return 0;
}

static struct platform_driver tcc_bluetooth_platform_driver = {
	.probe = tcc_bluetooth_probe,
	.remove = tcc_bluetooth_remove,
	.suspend = tcc_bluetooth_suspend,
	.resume = tcc_bluetooth_resume,
	.driver = {
		   .name = "tcc_bluetooth_rfkill",
		   .owner = THIS_MODULE,
		   },
};

static int __init tcc_bluetooth_init(void)
{
	bt_enabled = false;

	printk("[## BT ##] tcc_bluetooth_init\n");
	return platform_driver_register(&tcc_bluetooth_platform_driver);
}

static void __exit tcc_bluetooth_exit(void)
{
	platform_driver_unregister(&tcc_bluetooth_platform_driver);
}


module_init(tcc_bluetooth_init);
module_exit(tcc_bluetooth_exit);

MODULE_DESCRIPTION("telechips_bluetooth");
MODULE_AUTHOR("Jaikumar Ganesh <jaikumar@google.com>");
MODULE_LICENSE("GPL");
