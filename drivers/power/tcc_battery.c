#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/power_supply.h>
#include <linux/platform_device.h>
#include <linux/debugfs.h>
#include <linux/wakelock.h>
#include <linux/device.h>
#include <asm/gpio.h>
#include <linux/kthread.h>
#include <linux/time.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <mach/bsp.h>
#include <linux/clk.h>
#include <linux/notifier.h>
#include <linux/usb.h>
#include <linux/spinlock.h>
#include <linux/clk.h>
#include <mach/bsp.h>
#include <asm/io.h>
#include <mach/tcc_adc.h>
#include <asm/mach-types.h>
#include <linux/earlysuspend.h>

#if defined(CONFIG_REGULATOR_AXP192)
#include <linux/regulator/axp192.h>
#endif
#if defined(CONFIG_REGULATOR_AXP202)
#include <linux/regulator/axp202.h>
#endif

#if defined(CONFIG_BATTERY_COBY)
#define BATT_SPECIFIC_CUSTOMER 1
#endif

#if defined(CONFIG_REGULATOR_AXP192) || defined(CONFIG_REGULATOR_AXP202)
#define OVERCHARGE 105
#define ADJUSTCHARGE1 195
#define ADJUSTCHARGE2 185
#define ADJUSTCHARGE3 155 
#define MAX_CHARGING_INCREASE 200
#else
#define OVERCHARGE 30
#define ADJUSTCHARGE1 112
#define ADJUSTCHARGE2 130
#define ADJUSTCHARGE3 120 
#define MAX_CHARGING_INCREASE 200
#endif


#define ENABLE_START_EN (1<<0)
#define ECFLG_END (1<<15)

#define ESTIMATE_RATE 1000
#if defined(BATT_SPECIFIC_CUSTOMER)
#define EMPTLIMIT 400 //
#else
#define EMPTLIMIT 20 //
#endif
#define INTR 1
#define SAMPLE 100

#if BATT_SPECIFIC_CUSTOMER
#define LEDSAMPLE 4
#define BATTVOLSAMPLE 16
#define BATTWINDOW BATTVOLSAMPLE
#define BATT_CHECK_CNT_MAX      30
#define BATT_CHECK_PERIOD_MIN   10
#define BATT_CHECK_PERIOD_MAX   60
unsigned long   gAvrVoltage[BATTVOLSAMPLE];
unsigned long   LEDIndex;
static int battery_percentage = 100;
static int battery_adc = 0;
#define BATTERY_FULL 3840
#else
#define BATTSAMPLE 16
#define BATTWINDOW 16
unsigned long   gAvrVoltage[BATTWINDOW];
#endif
unsigned long   gIndex = 0;

#define BATT_CHECK_LOOP 1500 //ms
#define BATT_READ_WINDOW 1

#define COMPENSATION_LOAD 1
#define COMPENSATION_AC 1
#define COMPENSATION_RESUME 1

#if defined(CONFIG_REGULATOR_AXP192)
#define Charger_Current_100mA	0
#define Charger_Current_190mA	1
#define Charger_Current_280mA	2
#define Charger_Current_360mA	3
#define Charger_Current_450mA	4
#define Charger_Current_550mA	5
#define Charger_Current_630mA	6
#define Charger_Current_700mA	7
#define Charger_Current_780mA	8
#define Charger_Current_880mA	9
#define Charger_Current_960mA	10
#define Charger_Current_1000mA	11
#define Charger_Current_1080mA	12
#define Charger_Current_1160mA	13

#define Charger_Current_Normal Charger_Current_450mA
#define Charger_Current_High   Charger_Current_780mA
#elif defined(CONFIG_REGULATOR_AXP202)
#define Charger_Current_400mA	0
#define Charger_Current_500mA	1
#define Charger_Current_600mA	2
#define Charger_Current_700mA	3
#define Charger_Current_800mA	4
#define Charger_Current_900mA	5
#define Charger_Current_1000mA	6
#define Charger_Current_1100mA	7
#define Charger_Current_1200mA	8
#define Charger_Current_1300mA	9

#define Charger_Current_Normal Charger_Current_500mA
#define Charger_Current_High   Charger_Current_800mA
#endif

#define CHG_OK		0x02
#define CHG_ING		0x01
#define CHG_NONE	0x00



#define TRACE_BATT 0 

static struct wake_lock vbus_wake_lock;
static struct work_struct batt_work;

#if TRACE_BATT
#define BATT(x...) printk(KERN_INFO "[BATT] " x)
#else
#define BATT(x...) do {} while (0)
#endif

typedef enum {
	CHARGER_BATTERY = 0,
	CHARGER_USB,
	CHARGER_AC
} charger_type_t;

typedef enum {
	DISABLE_CHG = 0,
	ENABLE_SLOW_CHG,
	ENABLE_FAST_CHG
} batt_ctl_t;

typedef struct {
	int voltage_low;
	int voltage_high;
	int percentage;
} tcc_batt_vol;

const char *charger_tags[] = {"none", "USB", "AC"};

struct battery_info_reply {
	u32 batt_id;			/* Battery ID from ADC */
	u32 batt_vol;			/* Battery voltage from ADC */
	u32 batt_temp;			/* Battery Temperature (C) from formula and ADC */
	u32 batt_current;		/* Battery current from ADC */
	u32 level;			/* formula */
	u32 charging_source;		/* 0: no cable, 1:usb, 2:AC */
	u32 charging_enabled;		/* 0: Disable, 1: Enable */
	u32 full_bat;			/* Full capacity of battery (mAh) */
};

struct tcc_battery_info {
	/* lock to protect the battery info */
	struct tcc_adc_client *client;
	struct platform_device *pdev;
	struct mutex lock;
	struct battery_info_reply rep;
	struct work_struct changed_work;
	int present;
	unsigned long update_time;
};

struct mutex batt_lock;
struct task_struct	*batt_thread_task;
struct early_suspend batt_early_suspend;

#if 0
static int batt_charging_increase = 0;
#endif
static int batt_suspend_status = 0;
static int batt_charging_finish = 0;
static int batt_check_times = 6;
static int old_batt_adcValue = 0;
static int in_suspend = 0;
static int adc_channel;
static int ac_channel;
static int once = 1;

extern int android_system_booting_finished;
#if defined(CONFIG_REGULATOR_AXP192)
extern int axp192_get_batt_discharge_current(void);
extern int axp192_get_batt_charge_current(void);
#endif

#if defined(CONFIG_REGULATOR_AXP202)
extern int axp202_get_batt_discharge_current(void);
extern int axp202_get_batt_charge_current(void);
#endif
// XXX

tcc_battery_charging_status(void);
void tcc_battery_process(void) ;

void usb_register_notifier(struct notifier_block *nb);


static int tcc_power_get_property(struct power_supply *psy, 
				  enum power_supply_property psp,
				  union power_supply_propval *val);

static int tcc_battery_get_property(struct power_supply *psy, 
				    enum power_supply_property psp,
				    union power_supply_propval *val);

static ssize_t tcc_battery_show_property(struct device *dev,
					 struct device_attribute *attr,
					 char *buf);

static struct tcc_battery_info tcc_batt_info;

static unsigned int cache_time = 1000;

static int tcc_battery_initial = 0;



static enum power_supply_property tcc_battery_properties[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_TECHNOLOGY,
	POWER_SUPPLY_PROP_CAPACITY,
};

static enum power_supply_property tcc_power_properties[] = {
	POWER_SUPPLY_PROP_ONLINE,
};

static char *supply_list[] = {
	"battery",
};

static struct power_supply tcc_power_supplies[] = {
	{
		.name = "battery",
		.type = POWER_SUPPLY_TYPE_BATTERY,
		.properties = tcc_battery_properties,
		.num_properties = ARRAY_SIZE(tcc_battery_properties),
		.get_property = tcc_battery_get_property,
	},
	{
		.name = "usb",
		.type = POWER_SUPPLY_TYPE_USB,
		.supplied_to = supply_list,
		.num_supplicants = ARRAY_SIZE(supply_list),
		.properties = tcc_power_properties,
		.num_properties = ARRAY_SIZE(tcc_power_properties),
		.get_property = tcc_power_get_property,
	},
	{
		.name = "ac",
		.type = POWER_SUPPLY_TYPE_MAINS,
		.supplied_to = supply_list,
		.num_supplicants = ARRAY_SIZE(supply_list),
		.properties = tcc_power_properties,
		.num_properties = ARRAY_SIZE(tcc_power_properties),
		.get_property = tcc_power_get_property,
	},
};

static void usb_status_notifier_func(struct notifier_block *self, unsigned long action, void *dev);
static int g_usb_online;
int g_ac_plugin;

static struct notifier_block usb_status_notifier = {
	.notifier_call = usb_status_notifier_func,
};



//extern struct tcc_udc *the_controller;
// extern int USB_FLAG; // not use

/* not yet */
#define GPIO_BATTERY_DETECTION
#define GPIO_BATTERY_CHARGER_EN
#define GPIO_BATTERY_CHARGER_CURRENT
#define TIME_STEP (HZ)

/* SAMPLE Value */

#if defined(CONFIG_REGULATOR_RN5T614)
extern int rn5t614_battery_voltage(void);
extern int rn5t614_acin_detect(void);
extern int rn5t614_charge_status(void);
#endif

const int tcc_battery_vol[] = {
	100,  // 4.2v
	45,  // 4.1v
	40,  // 4.0v
	35,  // 3.9v
	30,  // 3.8v
	25,  // 3.7v
	20,  // 3.6v
	15,  // 3.5v
	10,  // 3.45v
}; // must check adc value 


/* backup
const int m805_892x_vol1[] = { // for m805 8923 system_rev 2001
	3740,  // 4.2v
	3663,  // 4.1v
	3576,  // 4.0v
	3475,  // 3.9v
	3383,  // 3.8v
	3290,  // 3.7v
	3194,  // 3.6v
	3100,  // 3.5v
	3080,  // 3.48v
};
*/

#if BATT_SPECIFIC_CUSTOMER
const tcc_batt_vol m801_88_battery_levels[] = {
    // low, high, persent
    { 3751, 4096,  100},
    { 3701, 3750,  90},
    { 3651, 3700,  80},
    { 3611, 3650,  70},
    { 3571, 3610,  60},     // 4.05
    { 3531, 3570,  50},     // 3.95
    { 3491, 3530,  45},     //
    { 3451, 3490,  40},     //
    { 3401, 3450,  35},	    //
    { 3351, 3400,  30},	    //
    { 3301, 3350,  20},	    //    
    { 3261, 3300,  15},	    //
    { 3181, 3260,  10},	    //
    { 3131, 3180,  5},	    //
    { 3121, 3130,  1},	    //
    { 500, 3120,  0},     
};

const tcc_batt_vol tcc_battery_levels[] = {
    	{ 0, 4095, 100},
};
#else

#if defined(CONFIG_REGULATOR_AXP192)
const int m805_892x_vol[] = {
	3700,  // 4.2v
	3613,  // 4.1v
	3522,  // 4.0v
	3425,  // 3.9v
	3330,  // 3.8v
	3235,  // 3.7v
	3144,  // 3.6v
	3043,  // 3.5v
	3010,  // 3.45v
};


const int m801_88_vol[] = {
	3707,  // 4.2v
	3613,  // 4.1v
	3522,  // 4.0v
	3425,  // 3.9v
	3330,  // 3.8v
	3235,  // 3.7v
	3144,  // 3.6v
	3043,  // 3.5v
	2997,  // 3.45v
};

#elif defined(CONFIG_REGULATOR_RN5T614)

const int m805_892x_vol[] = {
	3015,  // 4.2v
	2974,  // 4.1v
	2900,   // 4.0v
	2827,   // 3.9v
	2752,   // 3.8v
	2680,  // 3.7v
	2606,  // 3.6v
	2533,  // 3.5v
	2459,  // 3.4v
};

const int m801_88_vol[] = {
	2612,  // 4.2v
	2548,  // 4.1v
	2479,  // 4.0v
	2421,  // 3.9v
	2357,  // 3.8v
	2293,  // 3.7v
	2227,  // 3.6v
	2165,  // 3.5v
	2135,  // 3.45v
};
#else
const int m805_892x_vol[] = {
	3015,  // 4.2v
	2974,  // 4.1v
	2900,   // 4.0v
	2827,   // 3.9v
	2752,   // 3.8v
	2680,  // 3.7v
	2606,  // 3.6v
	2533,  // 3.5v
	2459,  // 3.4v
};

const int m801_88_vol[] = {
	2612,  // 4.2v
	2548,  // 4.1v
	2479,  // 4.0v
	2421,  // 3.9v
	2357,  // 3.8v
	2293,  // 3.7v
	2227,  // 3.6v
	2165,  // 3.5v
	2135,  // 3.45v
};
#endif

#endif


int battery_voltage(void)
{
#if defined(CONFIG_REGULATOR_AXP192)
	return axp192_battery_voltage();
#elif defined(CONFIG_REGULATOR_AXP202)
	return axp202_battery_voltage();
#elif defined(CONFIG_REGULATOR_RN5T614)
	if(batt_suspend_status == 0)
	    return tcc_adc_start(tcc_batt_info.client, adc_channel, 0);// Ricoh pmic doen't support battery voltage level.
	else
	    return old_batt_adcValue;
	
#else
	return tcc_adc_start(tcc_batt_info.client, adc_channel, 0);// using internal ADC
#endif
}

int acin_detect(void)
{
#if defined(CONFIG_REGULATOR_AXP192)
	return axp192_acin_detect();
#elif defined(CONFIG_REGULATOR_AXP202)
	return axp202_acin_detect();
#elif defined(CONFIG_REGULATOR_RN5T614)
	//return (tcc_adc_start(tcc_batt_info.client, ac_channel, 0) > 1500) ? 1:0; // Ricoh pmic doen't support ac detection..
	return rn5t614_acin_detect();
#else
	return (tcc_adc_start(tcc_batt_info.client, ac_channel, 0) > 1500) ? 1:0;// using internal ADC
#endif
}

void charge_current(unsigned char val)
{
#if defined(CONFIG_REGULATOR_AXP192)
	axp192_charge_current(val);
#elif defined(CONFIG_REGULATOR_AXP202)
	axp202_charge_current(val);
#elif defined(CONFIG_REGULATOR_RN5T614)
	
#else

#endif
}

int charge_status(void)
{
#if defined(CONFIG_REGULATOR_AXP192)
	return axp192_charge_status();
#elif defined(CONFIG_REGULATOR_AXP202)
	return axp202_charge_status();
#elif defined(CONFIG_REGULATOR_RN5T614)
        return rn5t614_charge_status();	
#else
	return 0;
#endif
}

int get_charge_current(void)
{
#if defined(CONFIG_REGULATOR_AXP192)
	return axp192_get_batt_charge_current();
#elif defined(CONFIG_REGULATOR_AXP202)
	return axp202_get_batt_charge_current();
#else
	return SAMPLE;
#endif
}

int get_discharge_current(void)
{
#if defined(CONFIG_REGULATOR_AXP192)
	return axp192_get_batt_discharge_current();
#elif defined(CONFIG_REGULATOR_AXP202)
	return axp202_get_batt_discharge_current();
#else
	return SAMPLE;
#endif
}
static void tcc_set_fast_charge_mode(int onoff) {
    if( onoff ) {
        if( machine_is_m57te() ) {
	     gpio_set_value(TCC_GPE(2), 1);		
	     printk("set fast charge mode\n");
        }
    } else {
        if( machine_is_m57te() ) {
	     gpio_set_value(TCC_GPE(2), 0);		
           printk("clear fast charge mode\n");
        }
    }
}

static void tcc_set_charge_enable(int onoff) {
    if( onoff ) {
        if( machine_is_m57te() ) {
	     gpio_set_value(TCC_GPE(3), 1);		
           printk("set charge enable\n");
        }
    } else {
        if( machine_is_m57te() ) {
	      gpio_set_value(TCC_GPE(3), 0);		
	      printk("set charge disable\n");
        }
    }
}

static void tcc_is_full_charge(int onoff) {
    if( machine_is_m57te() ) {
        return !(gpio_get_value(TCC_GPE(5)));
    }
}

void bubblesort(unsigned int Number[],unsigned int num)
{
	int i,j;
	unsigned int temp;
	for(i=0 ; i<(int)(num-1) ; i++)
	{
		for(j=i+1;j<(int)num;j++)
		{
			if(Number[j] != 0 &&  Number[i]>Number[j])
			{
				temp   = Number[i];
				Number[i] = Number[j];
				Number[j] = temp;
			}
		}
	}
}

static unsigned long tcc_read_adc(void)
{
	unsigned long adcValue = 0;
	int i=0;
	int *pbattery_vols;
	int temp_adc, adjustResume;
	struct clk *cpu_clk;
	struct clk *ddi_clk;
	
	struct tcc_adc_client *client = tcc_batt_info.client;
	
#if BATT_SPECIFIC_CUSTOMER
	adcValue = battery_voltage();	

	if(adcValue < EMPTLIMIT)
		adcValue = battery_voltage();

	if(adcValue < EMPTLIMIT)
		adcValue = tcc_battery_levels[ARRAY_SIZE(tcc_battery_levels)-1].voltage_high;
	if(!g_ac_plugin && !in_suspend){			
		cpu_clk = clk_get(0, "cpu");
		ddi_clk = clk_get(0, "ddi");
		
		if(clk_get_rate(cpu_clk) > 800000000)
			adcValue += 30;
		else if (clk_get_rate(cpu_clk) > 700000000 || clk_get_rate(ddi_clk) > 360000000)
			adcValue += 25;
		else if (clk_get_rate(cpu_clk) > 600000000 || clk_get_rate(ddi_clk) > 340000000)
			adcValue += 20;
		else if (clk_get_rate(cpu_clk) > 500000000 || clk_get_rate(ddi_clk) > 310000000)
			adcValue += 10;
		else if (clk_get_rate(cpu_clk) > 400000000 || clk_get_rate(ddi_clk) > 260000000)
			adcValue += 5;
	} //COMPENSATION by load


#if defined(CONFIG_REGULATOR_AXP192) || defined(CONFIG_REGULATOR_AXP202)
	if(get_discharge_current() < 20)
	{
		adcValue -= get_charge_current()*7/50;

		if((old_batt_adcValue >0) && (old_batt_adcValue > adcValue))
			adcValue = old_batt_adcValue;
	}

	old_batt_adcValue = adcValue;
#endif

	gAvrVoltage[gIndex%BATTVOLSAMPLE] = adcValue;
	gIndex++;
#else
	if (client) { 
		int i;
		int adc_samples[BATTSAMPLE];

		for(i=0;i<BATTSAMPLE;i++)
			adc_samples[i] = battery_voltage();

		bubblesort(adc_samples, BATTSAMPLE);

		for(i=(BATTSAMPLE/2 - 4);i< (BATTSAMPLE/2 + 4) ; i++)
			adcValue += adc_samples[i];

		adcValue /= 8;

		//printk("### BATT adc value = %d\n", adcValue);		

		if(machine_is_m805_892x() || machine_is_tcc8920()){
			pbattery_vols = m805_892x_vol;		
		}
		else if(machine_is_m801_88())
			pbattery_vols = m801_88_vol;		
		else 
		{
			pbattery_vols = tcc_battery_vol;
			adcValue = 4000;
		}


		BATT("ADC Value = %x\n", adcValue);

		// temporary compensation by cpu, ddi clock
		//########################################################
#if COMPENSATION_LOAD
		cpu_clk = clk_get(0, "cpu");
		ddi_clk = clk_get(0, "ddi");

		//printk("cpu = %d\n",clk_get_rate(cpu_clk));
		//printk("ddi = %d\n",clk_get_rate(ddi_clk));
		if(!g_ac_plugin && !in_suspend){				
			//batt_check_times = 6;
			if(1) { //adcValue < pbattery_vols[6]){
				if(clk_get_rate(cpu_clk) > 800000000)
					adcValue += 30;
				else if (clk_get_rate(cpu_clk) > 700000000 || clk_get_rate(ddi_clk) > 360000000)
					adcValue += 25;
				else if (clk_get_rate(cpu_clk) > 600000000 || clk_get_rate(ddi_clk) > 340000000)
					adcValue += 20;
				else if (clk_get_rate(cpu_clk) > 500000000 || clk_get_rate(ddi_clk) > 310000000)
					adcValue += 10;
				else if (clk_get_rate(cpu_clk) > 400000000 || clk_get_rate(ddi_clk) > 260000000)
					adcValue += 5;
			}		
/* // TODO:
			else
			{
				if(clk_get_rate(cpu_clk) > 800000000)
					adcValue += 68;
				else if (clk_get_rate(cpu_clk) > 700000000 || clk_get_rate(ddi_clk) > 360000000)
					adcValue += 58;
				else if (clk_get_rate(cpu_clk) > 600000000 || clk_get_rate(ddi_clk) > 340000000)
					adcValue += 38;
			}
*/
		}
#endif
		//########################################################

		// temporary compensation by ac	
		//########################################################


#if COMPENSATION_AC
		if(acin_detect()){
			//batt_check_times = 12;
			temp_adc = adcValue;
			//printk(" original adc = %d \n", temp_adc);

#if defined(CONFIG_REGULATOR_AXP192)
		  	if(charge_status() == CHG_OK){
#endif
			if(adcValue < pbattery_vols[5])
				adcValue -= ADJUSTCHARGE1;
			else if(adcValue >= pbattery_vols[5] && adcValue < pbattery_vols[0])
				adcValue -= ADJUSTCHARGE2;
			else if(adcValue >= pbattery_vols[0])
				adcValue -= (ADJUSTCHARGE3);
#if defined(CONFIG_REGULATOR_AXP192)			
			}
#endif

			if(old_batt_adcValue > 0 && (old_batt_adcValue - 40) > adcValue)
				adcValue = old_batt_adcValue;
//	if(temp_adc >= (pbattery_vols[0] + OVERCHARGE) ){
			if(temp_adc >= (pbattery_vols[0]) ){				
				adcValue = temp_adc;
				//batt_check_times = 24;				
			}
			else
			{
				//TODO : 
			}
		}
		else
		{
//			if(adcValue >= pbattery_vols[1])
				//batt_check_times =  12;				
//			else
				//batt_check_times =  6;				
		}

		old_batt_adcValue = adcValue;

#endif
		//########################################################

		// temporary compensation by resume
		//########################################################
#if COMPENSATION_RESUME
	
		if(!g_ac_plugin){
			adjustResume = 60;
			if(g_usb_online)
				adjustResume = 40;
		}
		else
			adjustResume = 10;		
		
		if(in_suspend)
			adcValue -= adjustResume;

#endif
		//########################################################

		//msleep(30);
		gAvrVoltage[gIndex++%BATTWINDOW] = adcValue;
	}	

#endif
	return 0;
}

static int test = 25;

struct battery_info_reply tcc_cur_battery_info =
{
	.batt_id = 0,			/* Battery ID from ADC */
	.batt_vol = 1100,			/* Battery voltage from ADC */
	.batt_temp = SAMPLE,			/* Battery Temperature (C) from formula and ADC */
	.batt_current = SAMPLE,		/* Battery current from ADC */
	.level = 100,				/* formula */
	.charging_source = 0,	/* 0: no cable, 1:usb, 2:AC */
	.charging_enabled  = 0,	/* 0: Disable, 1: Enable */
	.full_bat = SAMPLE			/* Full capacity of battery (mAh) */
};

/* ADC raw data Update to Android framework */
void tcc_read_battery(struct battery_info_reply *info)
{
	memcpy(info, &tcc_cur_battery_info, sizeof(struct battery_info_reply));

	#if BATT_SPECIFIC_CUSTOMER
	#else
	BATT("tcc_read_battery read batt id=%d,voltage=%d, temp=%d, current=%d, level=%d, charging source=%d, charging_enable=%d, full=%d\n", 
	     info->batt_id, info->batt_vol, info->batt_temp, info->batt_current, info->level, info->charging_source, info->charging_enabled, info->full_bat);
	#endif
	
	return 0;
}

int tcc_cable_status_update(int status)
{
	int rc = 0;
	unsigned last_source;
	//struct tcc_udc *pdev = the_controller;

	if (!tcc_battery_initial)
	     return 0;

	mutex_lock(&tcc_batt_info.lock);

	switch(status) {
	case CHARGER_BATTERY:
		BATT("cable NOT PRESENT\n");
 		tcc_batt_info.rep.charging_source = CHARGER_BATTERY;
		break;
	case CHARGER_USB:
		BATT("cable USB\n");
        if(tcc_batt_info.rep.charging_source != CHARGER_USB) {
    	    tcc_batt_info.rep.charging_source = CHARGER_USB;
        }
		break;
	case CHARGER_AC:
		BATT("cable AC\n");
        if(tcc_batt_info.rep.charging_source != CHARGER_AC) {
    		tcc_batt_info.rep.charging_source = CHARGER_AC;
        }
		break;
	default:
		BATT(KERN_ERR "%s: Not supported cable status received!\n", __FUNCTION__);
		rc = -EINVAL;
	}

	if ((status == CHARGER_USB) && (g_usb_online == 0)) {
		tcc_batt_info.rep.charging_source = CHARGER_AC;
	}

	if (machine_is_m801_88() || machine_is_m803() || machine_is_m805_892x()) {
		if ((status == CHARGER_BATTERY) && (g_usb_online == 1)) {
			g_usb_online = 0;
		}
	}

    if( g_ac_plugin || tcc_batt_info.rep.charging_source == CHARGER_AC ) {
        tcc_set_fast_charge_mode(1);
    } else {
        tcc_set_fast_charge_mode(0);
    }

    if( tcc_batt_info.rep.charging_source == CHARGER_AC
        || tcc_batt_info.rep.charging_source == CHARGER_USB ) {
        tcc_set_charge_enable(1);
    } else {
        tcc_set_charge_enable(0);
    }
    
	last_source = tcc_batt_info.rep.charging_source;
	mutex_unlock(&tcc_batt_info.lock);
	
/*
	//spin_lock(pdev->lock);
	if (USB_FLAG) {
		status = CHARGER_USB;
	} 
	//spin_unlock(pdev->lock);
*/ // not use

	if (last_source == CHARGER_USB) {
 		wake_lock(&vbus_wake_lock);
	} else {
		/* give userspace some time to see the uevent and update
	  	* LED state or whatnot...
	  	*/
	 	wake_lock_timeout(&vbus_wake_lock, HZ / 2);
	}

#if defined(CONFIG_BATTERY_COBY)
	if ((status == CHARGER_USB) && (g_ac_plugin == 1)) {
		tcc_batt_info.rep.charging_source = CHARGER_AC;
	}
#endif
	/* if the power source changes, all power supplies may change state */
	power_supply_changed(&tcc_power_supplies[CHARGER_BATTERY]);
	power_supply_changed(&tcc_power_supplies[CHARGER_USB]);
	power_supply_changed(&tcc_power_supplies[CHARGER_AC]);

	return rc;
}


/* A9 reports USB charging when helf AC cable in and China AC charger. */
/* Work arround: notify userspace AC charging first,
and notify USB charging again when receiving usb connected notificaiton from usb driver. */
static void usb_status_notifier_func(struct notifier_block *self, unsigned long action, void *dev)
{
	mutex_lock(&tcc_batt_info.lock);

	if (action == USB_BUS_ADD) {
	 	if (!g_usb_online) {
			g_usb_online = 1;
#if 1
			BATT("USB CHARGER IN\n");
			mutex_unlock(&tcc_batt_info.lock);
			tcc_cable_status_update(CHARGER_USB);
			mutex_lock(&tcc_batt_info.lock);
#else
			if (action && tcc_batt_info.rep.charging_source == CHARGER_AC) {
				mutex_unlock(&tcc_batt_info.lock);
				tcc_cable_status_update(CHARGER_USB);
				mutex_lock(&tcc_batt_info.lock);
			} else if (action) {
				BATT("warning: usb connected but charging source=%d\n", tcc_batt_info.rep.charging_source);
			}
#endif
		}     
	} else if( action == USB_BUS_REMOVE ) {
		g_usb_online = 0;
		BATT("USB CHARGER OUT\n");
		mutex_unlock(&tcc_batt_info.lock);
		// XXX

        if( g_ac_plugin )
    		tcc_cable_status_update(CHARGER_AC);
        else
            tcc_cable_status_update(CHARGER_BATTERY);
        
		mutex_lock(&tcc_batt_info.lock);
	}
	mutex_unlock(&tcc_batt_info.lock);

	return NOTIFY_OK;
}

static int tcc_get_battery_info(struct battery_info_reply *buffer)
{
	struct battery_info_reply info;
	int rc;

	if (buffer == NULL) 
		return -EINVAL;

	tcc_read_battery(&info);
                             
	//mutex_lock(&tcc_batt_info.lock);
	buffer->batt_id                 = (info.batt_id);
	buffer->batt_vol                = (info.batt_vol);
	buffer->batt_temp               = (info.batt_temp);
	buffer->batt_current            = (info.batt_current);
	buffer->level                   = (info.level);
	/* Move the rules of charging_source to cable_status_update. */
	/* buffer->charging_source      = be32_to_cpu(rep.info.charging_source); */
	buffer->charging_enabled        = (info.charging_enabled);
	buffer->full_bat                = (info.full_bat);
	//mutex_unlock(&tcc_batt_info.lock);
	
	return 0;
}

static int tcc_power_get_property(struct power_supply *psy, 
				  enum power_supply_property psp,
				  union power_supply_propval *val)
{
	charger_type_t charger;
	
	mutex_lock(&tcc_batt_info.lock);
	charger = tcc_batt_info.rep.charging_source;
	mutex_unlock(&tcc_batt_info.lock);

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		if (psy->type == POWER_SUPPLY_TYPE_MAINS)
			val->intval = (charger ==  CHARGER_AC ? 1 : 0);//1
		else if (psy->type == POWER_SUPPLY_TYPE_USB)
		{
//#ifdef CONFIG_MACH_TCC8800              // 101125 jckim temporary code for preventing to enter deep sleep
#if (0)
			val->intval = 1;
#else
			val->intval = (charger ==  CHARGER_USB ? 1 : 0);
#endif
		}
		else
			val->intval = 0;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}


int tcc_battery_get_charging_status(void)
{
	u32 level;
	charger_type_t charger;	
	int ret;

	mutex_lock(&tcc_batt_info.lock);
	charger = tcc_batt_info.rep.charging_source;
	
	switch (charger) {
	case CHARGER_BATTERY:
		ret = POWER_SUPPLY_STATUS_NOT_CHARGING;
		break;
	case CHARGER_USB:
	case CHARGER_AC:
		level = tcc_batt_info.rep.level;

        if(machine_is_m57te() ) {
            // check CHARG_RDY
            if( gpio_get_value(TCC_GPD(9))) {
    			ret = POWER_SUPPLY_STATUS_FULL;
            } else {
    			ret = POWER_SUPPLY_STATUS_CHARGING;
            }
        } else {
    		if (level == 100)
    			ret = POWER_SUPPLY_STATUS_FULL;
    		else
    			ret = POWER_SUPPLY_STATUS_CHARGING;
        }
		break;
	default:
		ret = POWER_SUPPLY_STATUS_UNKNOWN;
	}

	mutex_unlock(&tcc_batt_info.lock);
	return ret;
}
EXPORT_SYMBOL(tcc_battery_get_charging_status);

static int tcc_battery_get_property(struct power_supply *psy, 
				    enum power_supply_property psp,
				    union power_supply_propval *val)
{
	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		val->intval = tcc_battery_get_charging_status();
		break;
	case POWER_SUPPLY_PROP_HEALTH:
		val->intval = POWER_SUPPLY_HEALTH_GOOD;
		break;
	case POWER_SUPPLY_PROP_PRESENT:
		val->intval = tcc_batt_info.present;
		break;
	case POWER_SUPPLY_PROP_TECHNOLOGY:
		val->intval = POWER_SUPPLY_TECHNOLOGY_LION;
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		mutex_lock(&tcc_batt_info.lock);
		val->intval = tcc_batt_info.rep.level;
		mutex_unlock(&tcc_batt_info.lock);
		break;
	default:		
		return -EINVAL;
	}
	
	return 0;
}


#define TCC_BATTERY_ATTR(_name)						\
{									\
	.attr = { .name = #_name, .mode = S_IRUGO}, \
	.show = tcc_battery_show_property,				\
	.store = NULL,							\
}

static struct device_attribute tcc_battery_attrs[] = {
	TCC_BATTERY_ATTR(batt_id),
	TCC_BATTERY_ATTR(batt_vol),
	TCC_BATTERY_ATTR(batt_temp),
	TCC_BATTERY_ATTR(batt_current),
	TCC_BATTERY_ATTR(charging_source),
	TCC_BATTERY_ATTR(charging_enabled),
	TCC_BATTERY_ATTR(full_bat),
};

enum {
	BATT_ID = 0,
	BATT_VOL,
	BATT_TEMP,
	BATT_CURRENT,
	CHARGING_SOURCE,
	CHARGING_ENABLED,
	FULL_BAT,
};

static int tcc_battery_create_attrs(struct device * dev)
{
	int i, j, rc;
	for (i = 0; i < ARRAY_SIZE(tcc_battery_attrs); i++) {
		rc = device_create_file(dev, &tcc_battery_attrs[i]);
		if (rc)
			goto tcc_attrs_failed;
	}

	goto succeed;
     
tcc_attrs_failed:
	while (i--)
		device_remove_file(dev, &tcc_battery_attrs[i]);

succeed:        
	return rc;
}

static ssize_t tcc_battery_show_property(struct device *dev,
					 struct device_attribute *attr,
					 char *buf)
{
	int i = 0;
	const ptrdiff_t off = attr - tcc_battery_attrs;

	mutex_lock(&tcc_batt_info.lock);
	/* check cache time to decide if we need to update */
	if (tcc_batt_info.update_time &&
	    time_before(jiffies, tcc_batt_info.update_time + msecs_to_jiffies(cache_time)))
		goto dont_need_update;

	if (tcc_get_battery_info(&tcc_batt_info.rep) < 0) {
		printk(KERN_ERR "%s: rpc failed!!!\n", __FUNCTION__);
	} else {
		tcc_batt_info.update_time = jiffies;
	}
dont_need_update:
	mutex_unlock(&tcc_batt_info.lock);

	mutex_lock(&tcc_batt_info.lock);
	switch (off) {
	case BATT_ID:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			       tcc_batt_info.rep.batt_id);
		break;
	case BATT_VOL:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			       tcc_batt_info.rep.batt_vol);
		break;
	case BATT_TEMP:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			       tcc_batt_info.rep.batt_temp);
		break;
	case BATT_CURRENT:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			       tcc_batt_info.rep.batt_current);
		break;
	case CHARGING_SOURCE:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			       tcc_batt_info.rep.charging_source);
		break;
	case CHARGING_ENABLED:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			       tcc_batt_info.rep.charging_enabled);
		break;
	case FULL_BAT:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			       tcc_batt_info.rep.full_bat);
		break;
	default:
		i = -EINVAL;
	}
	mutex_unlock(&tcc_batt_info.lock);

	return i;
}
void tcc_ac_charger_detect_process(void) 
{
	if(machine_is_m801_88() || machine_is_m803()  || machine_is_m805_892x() || machine_is_tcc8920())
	{
		unsigned long adcValue = 0;
		  int i=0;
	
		struct tcc_adc_client *client = tcc_batt_info.client;
		if(acin_detect())
	            g_ac_plugin = 1;
        	else
            	    g_ac_plugin = 0;



	      if( g_ac_plugin ) {
#if defined(CONFIG_REGULATOR_AXP192) || defined(CONFIG_REGULATOR_AXP202)
		  if(charge_status() == CHG_OK){
		  	if( tcc_batt_info.rep.charging_source == CHARGER_AC ){
		            tcc_cable_status_update(CHARGER_BATTERY);
				batt_check_times = 1;
		  	}	
		  }
		  else
		  {
#endif		  	
	        if( tcc_batt_info.rep.charging_source == CHARGER_BATTERY ) {
	            tcc_cable_status_update(CHARGER_AC);
//			batt_check_times = 10;	
			#if 0 // not yet
			msleep(10000);
			gIndex = 0;
			for(i=0;i<BATTWINDOW;i++)
				tcc_read_adc();

		      tcc_ac_charger_detect_process();
		      tcc_battery_process();			
			#endif
				//if( machine_is_m801() )
				//	HwGPIOD->GPDAT |= Hw11;
	        }
#if defined(CONFIG_REGULATOR_AXP192) || defined(CONFIG_REGULATOR_AXP202)
		  }
#endif
	      } else {
	        if( tcc_batt_info.rep.charging_source == CHARGER_AC ) {
	            tcc_cable_status_update(CHARGER_BATTERY);
			batt_charging_finish = 0;
//			batt_check_times = 10;
			#if 0 // not yet
			msleep(10000);
			gIndex = 0;
			for(i=0;i<BATTWINDOW;i++)
				tcc_read_adc();

		      tcc_ac_charger_detect_process();
		      tcc_battery_process();
			#endif
				//if( machine_is_m801() )
				//	HwGPIOD->GPDAT &= ~Hw11;
	        	}
	      }
	}
	else if( machine_is_m57te() ) {
        static int CHRG_STBY = 0;
        static int CHRG = 0;


	  if( gpio_get_value(TCC_GPE(5)) != CHRG_STBY){
	  	CHRG_STBY = gpio_get_value(TCC_GPE(5));
            printk("CHRG_STBY# 0x%x ", gpio_get_value(TCC_GPE(5)));
	  }


	 if(gpio_get_value(TCC_GPD(9)) != CHRG){
            CHRG = gpio_get_value(TCC_GPD(9));
            printk("CHRG# 0x%x ", gpio_get_value(TCC_GPD(9)));        
        }
	 

        // check CHRG_STBY# PORT

	    if( (gpio_get_value(TCC_GPE(26))) && (g_ac_plugin == 0) ) {
            g_ac_plugin = 1;
            if(g_usb_online)
	            tcc_cable_status_update(CHARGER_USB);
            else
	            tcc_cable_status_update(CHARGER_AC);

	    } else if( !(gpio_get_value(TCC_GPE(26))) && (g_ac_plugin == 1) ) {
            g_ac_plugin = 0;
            if(g_usb_online)
	            tcc_cable_status_update(CHARGER_USB);
            else
	            tcc_cable_status_update(CHARGER_BATTERY);
	    }


    } else if( machine_is_m801() ) {
    
	    if(HwGPIOE->GPDAT & Hw26)
	        g_ac_plugin = 1;
        else 
            g_ac_plugin = 0;

	    if( g_ac_plugin ) {
	        if( tcc_batt_info.rep.charging_source == CHARGER_BATTERY ) {
	            tcc_cable_status_update(CHARGER_AC);
				if( machine_is_m801() )
					gpio_set_value(TCC_GPD(11), 1);
	        }
	    } else {
	        if( tcc_batt_info.rep.charging_source == CHARGER_AC ) {
	            tcc_cable_status_update(CHARGER_BATTERY);
				if( machine_is_m801() )
					gpio_set_value(TCC_GPD(11), 0);
	        }
	    }
	}

}

#if BATT_SPECIFIC_CUSTOMER
#else
static int battery_percentage = 100;
#endif
int tcc_battery_percentage(void)
{
	return battery_percentage;
}
EXPORT_SYMBOL(tcc_battery_percentage);

#if 0 // not yet support
int tcc_battery_charging_status(void)
{
#if defined(CONFIG_REGULATOR_AXP192)
	int temp;
      axp192_discharging_current();
	if(axp192_charging_current() > 100)
		return 1;
	else
		return 0;
	
#elif defined(CONFIG_REGULATOR_RN5T614)
//TODO:
#else
//TODO:
#endif
}
#endif

void tcc_battery_process(void) 
{
	unsigned long adcValue;
	unsigned long BattVoltage;
	int temp;
	int i, source, level, count;
	struct battery_info_reply info;
	unsigned int battery_changed_flag = 0;
    int battery_level_count = 0;
    tcc_batt_vol *pbattery_levels;
	int *pbattery_vols;
	int info_temp;
	static old_batt_per = 0;
#if BATT_SPECIFIC_CUSTOMER
	unsigned long MaxBat,MinBat;	
	static int axp_full_flag;
	unsigned short axp_reg;
	unsigned char plugin;
	#define IRQ_CHR_ING 0x08
	#define IRQ_CHR_OK  0x04
#endif

	adcValue = 0;
	temp = 0;

#if BATT_SPECIFIC_CUSTOMER
	axp_reg = charge_status();
	
	if(axp_reg == CHG_OK){
		axp_full_flag = 1;
	}
	if(axp_reg == CHG_ING){
		axp_full_flag = 0;
	}
	
	if(gIndex < BATTVOLSAMPLE){
		if(axp_full_flag){
			if(gIndex){
				for (i=0; i<gIndex; i++){
					adcValue += gAvrVoltage[i];					
				}
				BattVoltage = adcValue/gIndex;
			}
			else{
				BattVoltage = 4000;
			}
			gIndex = 0;
			goto QUICK_RESPOND_FOR_FULL;//skip Sample cnt
		}
		return;
	}
	gIndex = 0;
	MaxBat = 0;
	MinBat = 4096;
	for (i=0; i<BATTVOLSAMPLE; i++){
		adcValue += gAvrVoltage[i];
		if(MaxBat < gAvrVoltage[i])
			MaxBat = gAvrVoltage[i];
		if(MinBat > gAvrVoltage[i])
			MinBat = gAvrVoltage[i];
		//printk("tcc_battery_process --- gAvrVoltage[%d]:%d\n", i, gAvrVoltage[i]);
	}
	
	if( adcValue ){
		adcValue = adcValue - MaxBat - MinBat;
		BattVoltage = adcValue/(BATTVOLSAMPLE-2);
	}
	else 
		adcValue = 0;

QUICK_RESPOND_FOR_FULL:		
	battery_adc = BattVoltage;	
#else
	for (i=0; i<BATTWINDOW; i++)
		adcValue += gAvrVoltage[i];

	if( adcValue )
		BattVoltage = adcValue/BATTWINDOW;
	else 
		adcValue = 0;
#endif

	//BATT("avr = %d\r\n", BattVoltage);
	//BATT("PERCENT = %d\n", (int)(100 - ( ( MAXCAPACITY -BattVoltage)*100)/(MAXCAPACITY- EMPTLIMIT) )); // 0~ 100 in the range
	//BATT("mV = %d\n", (BattVoltage * 1000) /ESTIMATE_RATE); // mV
	temp  = ((BattVoltage * 1000) /ESTIMATE_RATE);

	//mutex_lock(&tcc_batt_info.lock);
	info.batt_id                 	= 0;
	info.batt_vol                  	= temp;
	info.batt_temp					= SAMPLE;
	info.batt_current            	= get_discharge_current();
	info.charging_source			= tcc_batt_info.rep.charging_source;
	source							= tcc_batt_info.rep.charging_source;

	info.level = 100;
#if BATT_SPECIFIC_CUSTOMER
    if(machine_is_m801_88() || machine_is_m803() || machine_is_m805_892x()){
        battery_level_count = ARRAY_SIZE(m801_88_battery_levels);
        pbattery_levels = m801_88_battery_levels;
    	}	
	else {
        battery_level_count = ARRAY_SIZE(tcc_battery_levels);
        pbattery_levels = tcc_battery_levels;        
    }
    
	for (count = 0; count < battery_level_count; count++)
	{
		if ((temp >= pbattery_levels[count].voltage_low) && (temp <= pbattery_levels[count].voltage_high))
			info.level = pbattery_levels[count].percentage;
	}

	if( temp < pbattery_levels[battery_level_count-1].voltage_low ) {
	    info.level = pbattery_levels[battery_level_count-1].percentage;
	}
	
	if(axp_full_flag && info.level < 90){
		axp_full_flag = 0;
	}
	if(axp_full_flag){
		info.level = 100;
	}
	else{
		if(info.level == 100){
			info.level = 98;
		}
	}

#if 0 // Sam , why do you make default level of Ac charger?
	if(acin_detect() & 0x0080){ //if AC changrger default level is 15
		if(info.level < 15)
			info.level = 15;
	}
#endif

#else

	if(machine_is_m805_892x() || machine_is_tcc8920())
	{
		pbattery_vols = m805_892x_vol;		
	}
	else if(machine_is_m801_88())
		pbattery_vols = m801_88_vol;		
	else 
	{
		pbattery_vols = tcc_battery_vol;
		temp = 4000;
	}


	if( g_ac_plugin)
	{
		if(temp > pbattery_vols[0])
		{
			//printk("### 90~100 ###\n");
#if defined(CONFIG_REGULATOR_AXP192) || defined(CONFIG_REGULATOR_AXP202)
			if( temp >= (pbattery_vols[0] + OVERCHARGE)){
				if(charge_status() == CHG_OK)
					info.level = 100;

				batt_charging_finish++;
				if(batt_charging_finish >= 2000){
					info.level = 100;
					batt_charging_finish = 0;
				}
			}
			else
			{
					info.level = 95;
			}
#else
			if( temp >= (pbattery_vols[0] + OVERCHARGE) - 10)			
			{
				if(pbattery_vols[0] + OVERCHARGE <= temp){

					if(charge_status() == CHG_OK){
						printk("### charge complete ### \n");
						info_temp = 100;

					}
					else
						info_temp = old_batt_per;
				}
				else
				{
					//if(old_batt_per > 0)
						info_temp = old_batt_per;
					//else
					//	info_temp = 90;	
				}

			}
			else
				info_temp = old_batt_per;
			
			info.level = info_temp;
#endif
		}
		else if( temp > pbattery_vols[5] && temp <= pbattery_vols[0])
		{
			//printk("### 20~90 ###\n");
			info_temp = 20 + (temp - pbattery_vols[5])/((pbattery_vols[1] - pbattery_vols[5])/70);
			if(info_temp > 90) info_temp = 90;
			info.level = (int)info_temp;		
		}
		else if( temp >= pbattery_vols[6] && temp <= pbattery_vols[5])
		{
			info_temp = 10 + (temp - pbattery_vols[6])/((pbattery_vols[5] - pbattery_vols[6])/10);
			if(info_temp > 20) info_temp = 20;					
			info.level = (int)info_temp;		
		}
		else if( temp >= pbattery_vols[8] && temp <= pbattery_vols[6])
		{
			info_temp = (temp - pbattery_vols[8])/((pbattery_vols[6] - pbattery_vols[8])/10);
			if(info_temp > 10) info_temp = 10;					
			info.level = (int)info_temp;		
		}
		else
		{
			info_temp = 0;
			info.level = info_temp;
		}		
	}
	else
	{
		if(temp > pbattery_vols[0])
		{
			info_temp = 100;
			info.level = info_temp;
		}
		else if( temp > pbattery_vols[1] && temp <= pbattery_vols[0])
		{
			info_temp = 90 + (temp - pbattery_vols[1])/((pbattery_vols[0] - pbattery_vols[1])/10);
			info.level = (int)info_temp;
		}
		else if( temp > pbattery_vols[5] && temp <= pbattery_vols[1])
		{
			info_temp = 20 + (temp - pbattery_vols[5])/((pbattery_vols[1] - pbattery_vols[5])/70);
			info.level = (int)info_temp;		
		}
		else if( temp >= pbattery_vols[6] && temp <= pbattery_vols[5])
		{
			info_temp = 10 + (temp - pbattery_vols[6])/((pbattery_vols[5] - pbattery_vols[6])/10);
			info.level = (int)info_temp;		
		}
		else if( temp >= pbattery_vols[8] && temp <= pbattery_vols[6])
		{
			info_temp = (temp - pbattery_vols[8])/((pbattery_vols[6] - pbattery_vols[8])/10);
			info.level = (int)info_temp;		
		}
		else
		{
			info_temp = 0;
			info.level = info_temp;
		}
	}


	old_batt_per = info.level;

#endif

	if(machine_is_tcc8800())
		info.level = 100;

	//printk("batt_per  = %d \n" , (int)info.level);
		
	info.batt_current            	= get_discharge_current();
	//BATT("Voltage : %d, level: %d\r\n", temp, info->level);
	/* Move the rules of charging_source to cable_status_update. */
	//info->charging_source      		= CHARGER_BATTERY;
	info.charging_enabled        	= 0;
	info.full_bat                	= SAMPLE;

    if( (source == CHARGER_USB || source == CHARGER_AC) ) {
        if( machine_is_m57te() ) {
        	// check CHRG# port 
        	if( !(HwGPIOD->GPDAT & Hw9)  ) {
        	    info.charging_enabled = 1;
        	} else {
        	    info.charging_enabled = 0;
        	}
        } else {
        	// source 
        	if(info.level < 100 ){
        		info.charging_enabled = 1;
        	} else {
        		info.charging_enabled = 0;
        	}
        	//mutex_unlock(&tcc_batt_info.lock);	
        }
    } else {
		info.charging_enabled = 0;
    }

	if( info.batt_id != tcc_cur_battery_info.batt_id )			
	{/* Battery ID from ADC */

	}

	if( info.charging_source != tcc_cur_battery_info.charging_source )
	{
		battery_changed_flag = 1;
	}

	if( info.level != tcc_cur_battery_info.level 
	    //|| info.batt_vol != tcc_cur_battery_info.batt_vol 
		)
	{
		battery_changed_flag = 1;
	}

	if( info.charging_enabled != tcc_cur_battery_info.charging_enabled )
	{
		battery_changed_flag = 1;
	}

	if( info.full_bat != tcc_cur_battery_info.full_bat)
	{
		battery_changed_flag = 1;
	}

	if( info.level < 20 || battery_changed_flag )
	{
		memcpy(&tcc_cur_battery_info, &info, sizeof(struct battery_info_reply));
		power_supply_changed(&tcc_power_supplies[info.charging_source]);
	}

	battery_percentage = info.level;

	BATT("batt id=%d,voltage=%d, temp=%d, current=%d, level=%d%%, charging source=%d, charging_enable=%d, full=%d ==============================\n", 
	     info.batt_id, info.batt_vol, info.batt_temp, info.batt_current, info.level, info.charging_source, info.charging_enabled, info.full_bat);
}

static int tcc_battery_thread(void *v)
{
	struct task_struct *tsk = current;	
#if BATT_SPECIFIC_CUSTOMER
	//int adccnt = 0;
	int now_percentage = 0;
	//#define ADCCNT 600
	#define ADCCNT 60
#endif
	daemonize("tcc-battery");
	allow_signal(SIGKILL);

	while (!signal_pending(tsk)) {

	if(batt_suspend_status == 0){
#if BATT_SPECIFIC_CUSTOMER
		tcc_ac_charger_detect_process();
		tcc_read_adc();
		tcc_battery_process();
		//adccnt = 0;
		if(now_percentage!= battery_percentage){
			if(battery_percentage >= 95){
				charge_current(Charger_Current_Normal);
			}
			else if(battery_percentage >= 90){
				charge_current(Charger_Current_Normal);
			}
			else if(battery_percentage >= 70){
				charge_current(Charger_Current_Normal);
			}
			else if(battery_percentage >= 50){
				charge_current(Charger_Current_Normal);
			}
			else if(battery_percentage >= 30){
				charge_current(Charger_Current_Normal);
			}
			else{
				charge_current(Charger_Current_Normal);
			}
		}
		now_percentage = battery_percentage;
		msleep(1500);
		power_supply_changed(&tcc_power_supplies[CHARGER_BATTERY]);
#else
       int i;

	if(batt_check_times > 0) batt_check_times--;

	if(batt_check_times == 0){

#if BATT_READ_WINDOW
		if(gIndex >= (BATTWINDOW-1))
		{
		        gIndex = BATTWINDOW-1;
			for(i=0;i<BATTWINDOW-1;i++)
			{
				gAvrVoltage[i] = gAvrVoltage[i+1];
			}
			tcc_read_adc();
		}
		else
		{
			tcc_read_adc();
		}
#else
		gIndex = 0;
		for(i=0; i<BATTWINDOW; i++)
			tcc_read_adc();
#endif

		if(android_system_booting_finished == 1 && once)
		{
			gIndex = 0;
			for(i=0; i<BATTWINDOW; i++)
			tcc_read_adc();
			once = 0;
		}
   
  	     tcc_ac_charger_detect_process();
	     tcc_battery_process();
	}
	

	for( i=0; i< 10; i++ ) {
      	      msleep(10);
	      tcc_ac_charger_detect_process();
      }
		
		msleep(BATT_CHECK_LOOP);
		power_supply_changed(&tcc_power_supplies[CHARGER_BATTERY]);
#endif
	}
	else
	{
		msleep(BATT_CHECK_LOOP);
	}
    }

	return 0;
}

static void tcc_ts_select(unsigned int selected)
{
	/* dummy */
}

static int tcc_bat_standby_f = 0; // 0 : not standby, 1 : standby
int tcc_adc_battery_standby(void) // check battery driver loading
{
    return tcc_bat_standby_f;
}
EXPORT_SYMBOL(tcc_adc_battery_standby);

void batt_conv(void)
{
	/* dummy */
}

static tcc_battery_adc_channel(void)
{
	BATT("%s\n",__func__);

	if(machine_is_tcc8800() || machine_is_m801_88() || machine_is_m803() || machine_is_tcc8920()){
		adc_channel = 5;
		ac_channel = 4;
	}
	else if(machine_is_tcc8900() || machine_is_m57te() || machine_is_m801()){
		adc_channel = 1;
	}
	else if(machine_is_m805_892x()){
		if(system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004)
		{
			adc_channel = 3;
			ac_channel = 2;
		}
		else
		{
			adc_channel = 2;
			ac_channel = 3;
		}
	}


}


void tcc_battery_early_suspend(struct early_suspend *h)
{
#if defined(CONFIG_REGULATOR_AXP192) || defined(CONFIG_REGULATOR_AXP202)
	charge_current(Charger_Current_High);
#endif

	printk("%s in\n", __func__);
	in_suspend = 1;
	printk("%s out\n", __func__);
}

void tcc_battery_late_resume(struct early_suspend *h)
{
	int i;
	printk("%s in\n", __func__);
#if defined(CONFIG_REGULATOR_AXP192) || defined(CONFIG_REGULATOR_AXP202)
	charge_current(Charger_Current_Normal);
#endif

#if BATT_SPECIFIC_CUSTOMER
	gIndex = 0;
	for(i=0;i<BATTWINDOW;i++)
		tcc_read_adc();

	tcc_ac_charger_detect_process();
	tcc_battery_process();
#endif
  	in_suspend = 0;
	printk("%s out\n", __func__);	
}


static int tcc_battery_suspend(struct platform_device *dev, pm_message_t state)
{
	printk("%s in\n", __func__);	
	/* flush all pending status updates */


	if(machine_is_m801_88()) {
		// for LED
		gpio_set_value(TCC_GPF(15), 0);
		gpio_set_value(TCC_GPF(16), 0);			
	}

	batt_suspend_status = 1;

	printk("%s out\n", __func__);	
	return 0;
}

static int tcc_battery_resume(struct platform_device *dev)
{
	printk("%s in\n", __func__);	
	/* things may have changed while we were away */
	/* wake thread */
	batt_suspend_status = 0;
	
	if(machine_is_m801_88()) {
		// for LED
		gpio_set_value(TCC_GPF(15), 1);
		gpio_set_value(TCC_GPF(16), 1);			
	}


	printk("%s out\n", __func__);	
	return 0;
}


static int tcc_battery_probe(struct platform_device *pdev)
{
	int i, err, ret;
	unsigned short adc_data;
   	struct tcc_adc_client *client;


	
	BATT("%s\n",__func__);
	//struct tcc_udc *usb_dev = the_controller;

#if 0 // kch, not support
	if (machine_is_tcc9200s()) {
		volatile PGPIO  pGPIO  = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);

		BITCSET(pGPIO->GPEFN3 , 0x00000F00, Hw8 );  // Function ADCIN
		BITCLR(pGPIO->GPEEN , Hw26); // Port as Input
	}
#endif
	tcc_battery_adc_channel();
	
	tcc_batt_info.update_time = jiffies;
	tcc_battery_initial = 1;

	/* init power supplier framework */
	for (i = 0; i < ARRAY_SIZE(tcc_power_supplies); i++) {
		err = power_supply_register(&pdev->dev, &tcc_power_supplies[i]);
		if (err)
			printk(KERN_ERR "Failed to register power supply (%d)\n", err);
	}

        #if defined(CONFIG_STB_BOARD_HDB892S) || defined(CONFIG_STB_BOARD_HDB892F)
        //Do't register usb notifier
        #else
	usb_register_notifier(&usb_status_notifier);
        #endif
	/* create tcc detail attributes */
	tcc_battery_create_attrs(tcc_power_supplies[CHARGER_BATTERY].dev);

	//spin_lock(usb_dev->lock);
	if( machine_is_m57te() ||machine_is_m801() ) {
	    if(gpio_get_value(TCC_GPE(26)))			
	        tcc_batt_info.rep.charging_source = CHARGER_AC;
	}



	tcc_battery_initial = 1;
	
	// Get current Battery info
	if (tcc_get_battery_info(&tcc_batt_info.rep) < 0)
		printk(KERN_ERR "%s: get info failed\n", __FUNCTION__);

	tcc_batt_info.present = 1;
	tcc_batt_info.update_time = jiffies;

	/* register adc client driver */
	client = tcc_adc_register(pdev, tcc_ts_select, batt_conv, 0);
	if (IS_ERR(client)) {
		printk("Battery client ADC fail%p", client);
		ret = PTR_ERR(client);
		return ret;
	}

#if defined(CONFIG_REGULATOR_AXP192) || defined(CONFIG_REGULATOR_AXP202)
	charge_current(Charger_Current_Normal);
#endif

	/* init */
	tcc_batt_info.client = client;
	tcc_batt_info.pdev   = pdev;
	tcc_batt_info.rep.level = 100;

	if( machine_is_m801_88() || machine_is_m803() || machine_is_m805_892x()){
		printk("dc input value = %d\n", tcc_adc_start(client, 4, 0));

		if(acin_detect())
		        tcc_batt_info.rep.charging_source = CHARGER_AC;
		}
	
	// /* get adc raw data */
	// tcc_adc_start(client, 1, 0);
	
	tcc_batt_info.update_time = jiffies;

	gIndex = 0;

#if 1
	batt_early_suspend.suspend = tcc_battery_early_suspend;
	batt_early_suspend.resume = tcc_battery_late_resume;
	batt_early_suspend.level = EARLY_SUSPEND_LEVEL_DISABLE_FB - 2;
	register_early_suspend(&batt_early_suspend);	
#endif	

      tcc_ac_charger_detect_process();

	for(i=0; i<BATTWINDOW; i++)
   		tcc_read_adc();	

	tcc_battery_process();

	kernel_thread(tcc_battery_thread, NULL, CLONE_KERNEL);

	tcc_bat_standby_f = 1; // battery driver stanby 

	printk("TCC Battery Driver Load\n");
	return 0;
	
fail:
	/* free adc client */
   	tcc_adc_release(client);

    return 0;
}

static int tcc_battery_remove(struct platform_device *pdev)
{
	/* dummy */
	return 0;
}

static struct platform_driver tcc_battery_driver = {
	.driver.name	= "tcc-battery",
	.driver.owner	= THIS_MODULE,
	.probe			= tcc_battery_probe,
	.suspend		= tcc_battery_suspend,
	.resume			= tcc_battery_resume,
};

static tcc_battery_port_init(void)
{
	if(machine_is_m801_88()) {
		gpio_request(TCC_GPF(15), "LED0");
		gpio_request(TCC_GPF(16), "LED1");
		tcc_gpio_config(TCC_GPF(15), GPIO_FN(0));
		tcc_gpio_config(TCC_GPF(16), GPIO_FN(0));
		gpio_direction_output(TCC_GPF(15),0);
		gpio_direction_output(TCC_GPF(16),0);		
		gpio_set_value(TCC_GPF(15), 1);
		gpio_set_value(TCC_GPF(16), 1);			
	}

	if( machine_is_m57te() ) {
		gpio_request(TCC_GPE(2), "CHRG_CTL");
		gpio_request(TCC_GPE(3), "CHRG_EN");	
		gpio_request(TCC_GPE(5), "CHRG_STBY");
		gpio_request(TCC_GPE(25), "BAT_DET");
		gpio_request(TCC_GPE(26), "DC INPUT");
		gpio_request(TCC_GPD(9), "CHRG#");
		tcc_gpio_config(TCC_GPE(2), GPIO_FN(0));
		tcc_gpio_config(TCC_GPE(3), GPIO_FN(0));
		tcc_gpio_config(TCC_GPE(5), GPIO_FN(0));
		tcc_gpio_config(TCC_GPE(25), GPIO_FN(0));
		tcc_gpio_config(TCC_GPE(26), GPIO_FN(0));
		tcc_gpio_config(TCC_GPD(9), GPIO_FN(0));
		gpio_direction_input(TCC_GPE(5));
		gpio_direction_input(TCC_GPE(25));	
		gpio_direction_input(TCC_GPE(26));
		gpio_direction_output(TCC_GPE(2), 0);
		gpio_direction_output(TCC_GPE(3), 0);
		gpio_direction_input(TCC_GPD(9));
	}

	if( machine_is_m801()) {
		//for AC cable
		gpio_request(TCC_GPE(26), "AC DET");
		tcc_gpio_config(TCC_GPE(26), GPIO_FN(0));
		gpio_direction_input(TCC_GPE(26));

		// for LED control
		gpio_request(TCC_GPD(11), "LED");
		tcc_gpio_config(TCC_GPD(11), GPIO_FN(0));
		gpio_direction_output(TCC_GPD(11), 1);
		gpio_set_value(TCC_GPD(11), 0);
	}

	if(machine_is_tcc8920()){
	//TODO:adc_gpio set config , led config, ricoh pmic config
	}


	if(machine_is_m805_892x()){
	//TODO:adc_gpio set config , led config, ricoh pmic config
#if defined(CONFIG_ARCH_TCC892X)

#endif
	}	

}

static int __init tcc_battery_init(void)
{
	struct clk *clk;

	printk("%s\n", __func__);

	wake_lock_init(&vbus_wake_lock, WAKE_LOCK_SUSPEND, "vbus_present");

	mutex_init(&tcc_batt_info.lock);

	tcc_battery_port_init();


	return platform_driver_register(&tcc_battery_driver);
}

static void __exit tcc_battery_exit(void)
{
	platform_driver_unregister(&tcc_battery_driver);
	usb_unregister_notify(&usb_status_notifier);
}

module_init(tcc_battery_init);
MODULE_DESCRIPTION("TCC Battery Driver");
MODULE_AUTHOR("Telechips, Inc");
MODULE_LICENSE("GPL");
