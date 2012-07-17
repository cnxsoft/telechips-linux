#include <mach/bsp.h>
#include <mach/io.h>

#include <i-tv/itv_common.h>
#include <i-tv/itv_platform.h>

#include "itccxxxx_tsif_p.h"

#if defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
struct tcc_gpio_regs {
    volatile unsigned long GPDAT, GPEN, GPSET, GPCLR, GPXOR, GPCD0, GPCD1, GPPD0, GPPD1, GPFN0, GPFN1, GPFN2,GPFN3;
};
#endif

//20110321 koo : S5H1411&XC5000_ATSC_SV6.0 brd에서는 cam1을 사용하므로 cam1_on on/off
#if defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
#include <mach/gpio.h>
#endif

/* frontend operations */
static void itv_platform_demod_reset(void)
{
#if defined(CONFIG_MACH_TCC8800)		//TCC93&88_DEMO_V6.1에서 xc5000을 사용할 때는 cam1 interface를 이용하고, fqd1136을 사용할 때는 ts parallel interface를 이용.
#if defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000) || defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000_MODULE)
	volatile PGPION regs;

	gpio_request(TCC_GPEXT1(13), NULL);
	gpio_request(TCC_GPEXT3(6), NULL);
	gpio_direction_output(TCC_GPEXT1(13), 1);

	regs = (volatile PGPION)tcc_p2v(HwGPIOE_BASE);
	
	BITCSET(regs->GPFN0, (Hw12-Hw8), 0);
	BITSET(regs->GPEN, Hw2);
	BITSET(regs->GPDAT, Hw2);				//TS_PWDN

	msleep(10);

	gpio_direction_output(TCC_GPEXT3(6), 1);
	msleep(50);
	gpio_direction_output(TCC_GPEXT3(6), 0);
	msleep(50);
	gpio_direction_output(TCC_GPEXT3(6), 1);
#elif defined(CONFIG_iTV_FE_TUNER_MODULE_FQD1136) || defined(CONFIG_iTV_FE_TUNER_MODULE_FQD1136_MODULE)
	PGPION regs;

	gpio_request(TCC_GPEXT3(4), NULL);
	gpio_direction_output(TCC_GPEXT3(4), 1);	

	regs = (volatile PGPION)tcc_p2v(HwGPIOF_BASE);

	BITCSET(regs->GPFN2, (Hw28-Hw24), 0);
	BITSET(regs->GPEN, Hw22);
	BITSET(regs->GPDAT, Hw22);				//TS_PWDN

	msleep(10);
	
	BITCSET(regs->GPFN1, (Hw24-Hw20), 0);
	BITSET(regs->GPEN, Hw13);
	BITSET(regs->GPDAT, Hw13);				//TS_RST
	msleep(50);
	BITCLR(regs->GPDAT, Hw13);				
	msleep(50);
	BITSET(regs->GPDAT, Hw13);				
#endif	//#if defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000) || defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000_MODULE)
#elif defined(CONFIG_MACH_TCC8800ST)
	PGPION regs;

	//20110706 koo : i2c0-scl gpio_a0 conf가 clear 되어 해당 conf set
	regs = (volatile PGPION)tcc_p2v(HwGPIOA_BASE);
	if((regs->GPFN0 & 0x00000001) != 0x1)		BITCSET(regs->GPFN0, (Hw4-Hw0), Hw0);
	
	regs = (volatile PGPION)tcc_p2v(HwGPIOF_BASE);

	BITCSET(regs->GPFN3, (Hw8-Hw4), 0);
	BITSET(regs->GPEN, Hw25);
	BITSET(regs->GPDAT, Hw25);				//TS_PWDN	

	msleep(10);

	BITCSET(regs->GPFN1, (Hw16-Hw12), 0);
	BITSET(regs->GPEN, Hw11);
	BITSET(regs->GPDAT, Hw11);				//TS_RST
	msleep(50);
	BITCLR(regs->GPDAT, Hw11);				
	msleep(50);
	BITSET(regs->GPDAT, Hw11);				
#elif defined(CONFIG_MACH_TCC8920ST)
	PGPION regs;

	regs = (volatile PGPION)tcc_p2v(HwGPIOE_BASE);

	BITCSET(regs->GPFN3.nREG, (Hw20-Hw16), 0);
	BITSET(regs->GPEN.nREG, Hw28);
	BITSET(regs->GPDAT.nREG, Hw28);				//TS_PWDN	

	msleep(10);

	BITCSET(regs->GPFN3.nREG, (Hw24-Hw20), 0);
	BITSET(regs->GPEN.nREG, Hw29);
	BITSET(regs->GPDAT.nREG, Hw29);				//TS_RST
	msleep(50);
	BITCLR(regs->GPDAT.nREG, Hw29);
	msleep(50);
	BITSET(regs->GPDAT.nREG, Hw29);
#else
	BITCSET(HwGPIOE->GPFN1, (Hw16-Hw12), 0);
	BITSET(HwGPIOE->GPEN, Hw11);
	BITSET(HwGPIOE->GPDAT, Hw11);
	msleep(50);
	BITCLR(HwGPIOE->GPDAT, Hw11);
	msleep(50);
	BITSET(HwGPIOE->GPDAT, Hw11);
#endif

	//20110224 koo : reset 후 바로 i2c로 read 할 경우 demod reset이 완료되기 전에 read하여 잘못된 data를 가져와서 reset 후 delay add
	msleep(1);
}

static void itv_platform_demod_pwroff(int tsif_mod)
{
	//tsif stop
	if(tsif_mod)	tcc_tsif_p_stop();

#if defined(CONFIG_MACH_TCC8800)		//TCC93&88_DEMO_V6.1에서 xc5000을 사용할 때는 cam1 interface를 이용하고, fqd1136을 사용할 때는 ts parallel interface를 이용.
#if defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000) || defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000_MODULE)
	PGPION regs;

	regs = (volatile PGPION)tcc_p2v(HwGPIOE_BASE);
	BITCLR(regs->GPDAT, Hw2);																				//TS_PWDN
	gpio_request(TCC_GPEXT1(13), NULL);
	gpio_request(TCC_GPEXT3(6), NULL);
	gpio_direction_output(TCC_GPEXT1(13), 0);																	//TS_PWR
	gpio_direction_output(TCC_GPEXT3(6), 0);																//TS_RST
#elif defined(CONFIG_iTV_FE_TUNER_MODULE_FQD1136) || defined(CONFIG_iTV_FE_TUNER_MODULE_FQD1136_MODULE)
	PGPION regs;

	regs = (volatile PGPION)tcc_p2v(HwGPIOF_BASE);
	BITCLR(regs->GPDAT, Hw22);																				//TS_PWDN
	BITCLR(regs->GPDAT, Hw13);																				//TS_RST	
	gpio_request(TCC_GPEXT3(4), NULL);
	gpio_direction_output(TCC_GPEXT3(4), 0);																	//TS_PWR
#endif
#elif defined(CONFIG_MACH_TCC8800ST)
	PGPION regs;

	regs = (volatile PGPION)tcc_p2v(HwGPIOF_BASE);
	BITCLR(regs->GPDAT, Hw25);																				//TS_PWDN
#elif defined(CONFIG_MACH_TCC8920ST)
	PGPION regs;

	regs = (volatile PGPION)tcc_p2v(HwGPIOE_BASE);
	BITCLR(regs->GPDAT.nREG, Hw28);																		//TS_PWDN
#endif
}

//20110324 koo : i2c ch0를 사용할 경우 wm8731(addr : 0x1a)와 slave addr이 동일하여 i2c ch1를 사용하도록 board 시방.
#if defined(CONFIG_MACH_TCC8800)
#define ITV_TCCXXXX_I2C_CH		1
#elif defined(CONFIG_MACH_TCC8800ST)
#define ITV_TCCXXXX_I2C_CH		0
#elif defined(CONFIG_MACH_TCC8920ST)
#define ITV_TCCXXXX_I2C_CH		1
#else
#define ITV_TCCXXXX_I2C_CH		1
#endif



static struct i2c_adapter *itv_platform_i2c_get_adapter(void)
{
	return i2c_get_adapter(ITV_TCCXXXX_I2C_CH);
}

static void itv_platform_i2c_put_adapter(struct i2c_adapter *p_i2c_adap)
{
	if(!p_i2c_adap)
		return;

	i2c_put_adapter(p_i2c_adap);
}

itv_platform_frontend_operations_t *itv_platform_get_frontend_operations(void)
{
	itv_platform_frontend_operations_t *p_fe_ops;

	p_fe_ops = kzalloc(sizeof(*p_fe_ops), GFP_KERNEL);
	if(!p_fe_ops) {
		eprintk("out of memory\n");
		return NULL;
	}

	// demodulator operations
	p_fe_ops->demod_reset = itv_platform_demod_reset;
	p_fe_ops->demod_pwroff = itv_platform_demod_pwroff;

	// i2c operations
	p_fe_ops->i2c_get_adapter = itv_platform_i2c_get_adapter;
	p_fe_ops->i2c_put_adapter = itv_platform_i2c_put_adapter;

	return p_fe_ops;
}
EXPORT_SYMBOL(itv_platform_get_frontend_operations);

void itv_platform_put_frontend_operations(itv_platform_frontend_operations_t *p_fe_ops)
{
	kfree(p_fe_ops);
}
EXPORT_SYMBOL(itv_platform_put_frontend_operations);

/* ts interface operations */
itv_platform_tsif_operations_t *itv_platform_get_tsif_operations(void)
{
	itv_platform_tsif_operations_t *p_tsif_ops;

	p_tsif_ops = kzalloc(sizeof(*p_tsif_ops), GFP_KERNEL);
	if(!p_tsif_ops) {
		eprintk("out of memory\n");
		return NULL;
	}

	// TS parallel interface operations
	p_tsif_ops->tsif_p_init 	= tcc_tsif_p_init;
	p_tsif_ops->tsif_p_deinit 	= tcc_tsif_p_deinit;
	p_tsif_ops->tsif_p_get_pos 	= tcc_tsif_p_get_pos;
	p_tsif_ops->tsif_p_start 	= tcc_tsif_p_start;
	p_tsif_ops->tsif_p_stop 	= tcc_tsif_p_stop;
	p_tsif_ops->tsif_p_insert_pid 		= tcc_tsif_p_insert_pid;
	p_tsif_ops->tsif_p_remove_pid 		= tcc_tsif_p_remove_pid;
	p_tsif_ops->tsif_p_set_packetcnt 	= tcc_tsif_p_set_packetcnt;

	return p_tsif_ops;
}
EXPORT_SYMBOL(itv_platform_get_tsif_operations);

void itv_platform_put_tsif_operations(itv_platform_tsif_operations_t *p_tsif_ops)
{
	kfree(p_tsif_ops);
}
EXPORT_SYMBOL(itv_platform_put_tsif_operations);

MODULE_AUTHOR("JP");
MODULE_LICENSE("GPL");
