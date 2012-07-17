/* This driver is designed to control the usage of the ADC block between
 * the touchscreen and any other drivers that may need to use it, such as
 * the hwmon driver.
 *
 * Priority will be given to the touchscreen driver, but as this itself is
 * rate limited it should not starve other requests which are processed in
 * order that they are received.
 *
 * Each user registers to get a client block which uniquely identifies it
 * and stores information such as the necessary functions to callback when
 * action is required.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/list.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/io.h>

#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/notifier.h>
#include <linux/spinlock.h>
#include <asm/io.h>
#include <mach/bsp.h>

#include <mach/tca_adconv.h>


#if defined(CONFIG_ARCH_TCC93XX)
#define PRESCALER_VAL(n)	(((n)&0xff)<<7)
#else
#define PRESCALER_VAL(n)	(((n)&0xff)<<6)
#endif

struct tcc_adc_client {
        struct platform_device  *pdev;
        struct list_head         pend;

        unsigned int			nr_samples;
        unsigned char        is_ts;
        unsigned char        channel;
        unsigned long       adc_data[16];
        unsigned long			raw;
        void    (*select_cb)(unsigned selected);
        void    (*convert_cb)(unsigned val1, unsigned val2);
};

struct adc_device {
        struct platform_device  *pdev;
        struct platform_device  *owner;
        struct clk              *clk;
        struct tcc_adc_client   *cur;
        struct tcc_adc_client   *ts_pend;
        void __iomem           *regs;
        unsigned int          prescale;
        int                     irq;
};

static struct mutex			lock;
static struct adc_device *adc_dev;

static LIST_HEAD(adc_pending);

#define adc_dbg(_adc, msg...) dev_dbg(&(_adc)->pdev->dev, msg)

static unsigned long adc_data[16];


//extern unsigned long tca_adc_read(unsigned int channel);
//
#if 0
#if 1
//#define dbg(x...) printk(KERN_DEBUG "tcc uart: ");
#define dbg printk
#else /* no debug */
#define dbg(x...) do {} while(0)
#endif
#endif


static inline void tcc_adc_convert(struct adc_device *adc)
{
    //printk("%s(%d)\n", __func__, __LINE__);
     if(adc->cur->channel < 16 && adc->cur->channel >= 0)
        adc_data[adc->cur->channel] = tca_adc_read(adc->cur->channel);


#if 0
    printk("ADC Ch:%d %d\n", adc->cur->channel, adc_data[adc->cur->channel]);
//	if (adc->cur->channel == 6)
 //       printk("Touch Key Ch:%d %x\n", adc->cur->channel, adc_data[adc->cur->channel]);
#endif
}

static inline void tcc_adc_select(struct adc_device *adc,
                                  struct tcc_adc_client *client)
{
     
}

static void tcc_adc_dbgshow(struct adc_device *adc)
{
               
}
 
void tcc_adc_try(struct adc_device *adc)
{
	struct tcc_adc_client *next = adc->ts_pend;

	if (!next && !list_empty(&adc_pending)) {
	     next = list_first_entry(&adc_pending,
	                             struct tcc_adc_client, pend);
	     list_del(&next->pend);
	} else
	     adc->ts_pend = NULL;
	if (next) {
	     adc_dbg(adc, "new client is %p\n", next);
	     adc->cur = next;
	     tcc_adc_select(adc, next);
	     tcc_adc_convert(adc);
	     tcc_adc_dbgshow(adc);
	}

}
 
unsigned long tcc_adc_start(struct tcc_adc_client *client,
               unsigned int channel, unsigned int nr_samples)
{
	struct adc_device *adc = adc_dev;
	unsigned long flags;
#if 0
	unsigned long key;
	unsigned long bat;
#endif //EYP

    //dbg("ADC Channel : %d\n", channel);
//   printk("ADC Channel : %d\n", channel);

	if (!adc) {
	     printk(KERN_ERR "%s: failed to find adc\n", __func__);
	     return -EINVAL;
	}

	if (client->is_ts && adc->ts_pend)
	     return -EAGAIN;

//	local_irq_save(flags);
    	mutex_lock(&lock);
	client->channel = channel;
	client->nr_samples = nr_samples;

	if (client->is_ts)
	     adc->ts_pend = client;
	else
	     list_add_tail(&client->pend, &adc_pending);

	//if (!adc->cur)
    tcc_adc_try(adc);	

	//ret = adc->cur->adc_data[channel];
	//adc->cur->adc_data[channel]= 0;	
	//printk("-->ch: %d value: %x\n",channel, ret);
	mutex_unlock(&lock);     
//	local_irq_restore(flags);



	return adc_data[channel];
}
EXPORT_SYMBOL_GPL(tcc_adc_start);

static void tcc_adc_default_select(unsigned select)
{
}

struct tcc_adc_client *tcc_adc_register(struct platform_device *pdev,
                                     void (*select)(unsigned int selected),
                                     void (*conv)(unsigned d0, unsigned d1),
                                     unsigned int is_ts)
{
	struct tcc_adc_client *client;

	WARN_ON(!pdev);
	WARN_ON(!conv);

	if (!select)
	     select = tcc_adc_default_select;

	if (!conv || !pdev)
		return NULL;
	     //return ERR_PTR(-EINVAL);

	client = kzalloc(sizeof(struct tcc_adc_client), GFP_KERNEL);
	if (!client) {
	     dev_err(&pdev->dev, "no memory for adc client\n");
		 return NULL;
	     //return ERR_PTR(-ENOMEM);
	}

	client->pdev = pdev;
	client->is_ts = is_ts;
	client->select_cb = select;
	client->convert_cb = conv;

	return client;
}
EXPORT_SYMBOL_GPL(tcc_adc_register);

void tcc_adc_release(struct tcc_adc_client *client)
{
     /* We should really check that nothing is in progress. */
	 if (client)
     	kfree(client);
}
EXPORT_SYMBOL_GPL(tcc_adc_release);

static int tcc_adc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct adc_device *adc;
	struct resource *regs;

	int ret;

	volatile PTSADC pTSADC = (volatile PTSADC)tcc_p2v(HwTSADC_BASE);
//	volatile PGPIO  pGPIO  = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);
      volatile PPMU pPMU = (volatile PPMU)tcc_p2v(HwPMU_BASE);
	
	printk("%s\n", __func__);
	adc = kzalloc(sizeof(struct adc_device), GFP_KERNEL);
	if (adc == NULL) {
	 dev_err(dev, "failed to allocate adc_device\n");
	 return -ENOMEM;
	}
	mutex_init(&lock);
	adc->pdev = pdev;
	adc->prescale = PRESCALER_VAL(12);


	regs = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!regs) {
	 dev_err(dev, "failed to find registers\n");
	 ret = -ENXIO;
	 goto err_clk;
	}

	adc->regs = ioremap(regs->start, resource_size(regs));
	if (!adc->regs) {
	 dev_err(dev, "failed to map registers\n");
	 ret = -ENXIO;
	 goto err_clk;
	}

	//clk_enable(adc->clk);

	BITSET(pPMU->CONTROL, 1<<16/*HwPMU_CONTROL_APEN*/);
	BITCLR(pPMU->CONTROL, 1<<17/*HwPMU_CONTROL_ASTM*/);
	BITCLR(pPMU->CONTROL, 1<<18/*HwPMU_CONTROL_AISO*/);

	// 1. ADC Bus Clock Enable
//	tca_ckc_setiobus(RB_TSADCCONTROLLER,1);
	adc->clk = clk_get(NULL, "adc");
	if (!adc->clk) {
		printk("can't get ADC clock\n");
	} else {
		clk_enable(adc->clk);	
	}

	// 2. GPIO Setting as ADCIN
//	tca_adc_portinitialize((unsigned int)pGPIO, NULL);

	// 2. ADC Setting
	tca_adc_adcinitialize((unsigned int)pTSADC, NULL);

	printk("attached TCC adc driver\n");

	platform_set_drvdata(pdev, adc);
	adc_dev = adc;

	return 0;

err_clk:
	//clk_put(adc->clk);

//err_alloc:
//	kfree(adc);
	return ret;
}

static int tcc_adc_remove(struct platform_device *pdev)
{
	struct adc_device *adc = platform_get_drvdata(pdev);

	iounmap(adc->regs);
	//free_irq(adc->irq, adc);
	//clk_disable(adc->clk);
	//clk_put(adc->clk);
	kfree(adc);

	return 0;
}

#ifdef CONFIG_PM
static int tcc_adc_suspend(struct platform_device *pdev, pm_message_t state)
{
    tca_adc_powerdown();
	//tca_ckc_setiobus(RB_TSADCCONTROLLER,0);

	return 0;
}

static int tcc_adc_resume(struct platform_device *pdev)
{
    tca_adc_powerup();
	//tca_ckc_setiobus(RB_TSADCCONTROLLER,1);

	return 0;
}

#else
#define tcc_adc_suspend NULL
#define tcc_adc_resume NULL
#endif

static struct platform_driver tcc_adc_driver = {
	.driver         = {
	     .name   = "tcc-adc",
	     .owner  = THIS_MODULE,
	},
	.probe          = tcc_adc_probe,
	.remove         = __devexit_p(tcc_adc_remove),
	.suspend        = tcc_adc_suspend,
	.resume         = tcc_adc_resume,
};

static int __init adc_init(void)
{
	int ret;

	printk("%s\n", __func__);

	ret = platform_driver_register(&tcc_adc_driver);
	if (ret)
	     printk(KERN_ERR "%s: failed to add adc driver\n", __func__);

	return ret;
}

arch_initcall(adc_init);
