#include <i-tv/itv_common.h>
#include <i-tv/itv_frontend.h>

#include "ituner_i2c.h"
#include "ifqd1136.h"

static int itv_fqd1136_module_debug;
static int itv_fqd1136_module_debug_callstack;

module_param_named(debug, itv_fqd1136_module_debug, int, 0644);
module_param_named(debug_cs, itv_fqd1136_module_debug_callstack, int, 0644);

MODULE_PARM_DESC(debug, "Turn on/off fqd1136 debugging (default:off).");
MODULE_PARM_DESC(debug_cs, "Turn on/off fqd1136 callstack debugging (default:off).");
MODULE_PARM_DESC(init_fw, "Load firmware during driver initialization.");

#ifdef dprintk
#undef dprintk
#endif

#define dprintk(args...) \
	do { if(itv_fqd1136_module_debug) { printk("[%s] ", __func__); printk(args); } } while (0)

#define DEBUG_CALLSTACK		\
	do { if(itv_fqd1136_module_debug_callstack) { printk("[Call] ----> %s()\n", __func__); } } while (0);

struct itv_tuner_priv_t {
	itv_tuner_i2c_props_t i2c_props;
	
	unsigned int if_freq;
	unsigned int freq_hz;
	unsigned int bandwidth;
	unsigned char freq_band;
	unsigned char agc_mode;
	unsigned char pll_step;
	unsigned char pll_step_val;
	unsigned char pll_charge_pump;
	unsigned char tuner_standard;
};

static int itv_fqd1136_readregs(itv_tuner_priv_t *p_priv, unsigned char *buf, unsigned char len);
static int itv_fqd1136_writeregs(itv_tuner_priv_t *p_priv, unsigned char *buf, unsigned char len);

//#define TUNER_FQD1136_TEST
#ifdef TUNER_FQD1136_TEST

static unsigned int FreqTable_Cable[] = {
	 75000000,  57000000,  63000000,  69000000,  79000000, 
	 85000000, 177000000, 183000000, 189000000, 195000000,
	201000000, 207000000, 213000000, 123000000, 129000000,
	135000000, 141000000, 147000000, 153000000, 159000000,
	165000000, 171000000, 219000000, 225000000, 231000000,
	237000000, 243000000, 249000000, 255000000, 261000000,
	267000000, 273000000, 279000000, 285000000, 291000000,
	297000000, 303000000, 309000000, 315000000, 321000000,
	327000000, 333000000, 339000000, 345000000, 351000000,
	357000000, 363000000, 369000000, 375000000, 381000000,
	387000000, 393000000, 399000000, 405000000, 411000000,
	417000000, 423000000, 429000000, 435000000, 441000000,
	447000000, 453000000, 459000000, 465000000, 471000000,
	477000000, 483000000, 489000000, 495000000, 501000000,
	507000000, 513000000, 519000000, 525000000, 531000000,
	537000000, 543000000, 549000000, 555000000, 561000000,
	567000000, 573000000, 579000000, 585000000, 591000000,
	597000000, 603000000, 609000000, 615000000, 621000000,
	627000000, 633000000, 639000000, 645000000, 651000000,
	657000000, 663000000, 669000000, 675000000, 681000000,
	687000000, 693000000, 699000000, 705000000, 711000000,
	717000000, 723000000, 729000000, 735000000, 741000000,
	747000000, 753000000, 759000000, 765000000, 771000000,
	777000000, 783000000, 789000000, 795000000, 801000000,
	807000000, 813000000, 819000000, 825000000, 831000000,
	837000000, 843000000, 849000000, 855000000, 861000000,
	0
};

static int test_analog_tune(itv_frontend_t *p_ifrontend)
{
	int					result;
	unsigned int		uc_IF, uc_Mhz, cnt = 0;
	unsigned char		buf[5];
	unsigned char       buf_aux[2];
	itv_tuner_priv_t *priv = p_ifrontend->ituner.priv;
	unsigned char status;

	
	while( FreqTable_Cable[cnt] != 0)
	{
		uc_Mhz	= FreqTable_Cable[cnt] / 1000000;
		uc_IF	= ((uc_Mhz + 44) * 1000000) / 62500;

		buf[0]	= (uc_IF & 0x7f00) >> 8;
		buf[1]	= uc_IF & 0x00ff;

		// PLL charge pump current setting 
		buf[2] = 0xb6;

		//Band Selection
		if(uc_Mhz <= 159/*135*/)			// Low Band
			buf[3] = 0x01;
		else if(uc_Mhz <= 444/*369*/)		// Mid Band
			buf[3] = 0x02;
		else 								// High Band
			buf[3] = 0x04;

		result = itv_fqd1136_writeregs(priv, &buf[0], 4);

		//msleep(10);
		buf_aux[0] = 0x9E;
		buf_aux[1] = 0x80 | 0x40;
		result = itv_fqd1136_writeregs(priv, &buf_aux[0], 2);

		msleep(200);

		result = itv_fqd1136_readregs(priv, &status, 1);

		dprintk("No#%03d Freq %3dMhz, CCAB[%02X %02X %02X %02X] BB[%02X %02X], status[%02X] POR %d, FL %d, AGC %d, AFC %d\n"
			, cnt, uc_Mhz
			, buf[0], buf[1], buf[2], buf[3], buf_aux[0], buf_aux[1]
			, status, !!(status&0x80), !!(status&0x40), !!(status&0x08), status&0x07 );

		cnt++;
	}

	return 0;
}
#endif

static int itv_fqd1136_get_lock_status(itv_tuner_priv_t *p_priv, unsigned short *lock_status)
{
	int ret;
	unsigned char lock=0;
//	DEBUG_CALLSTACK
	*lock_status = 0;

	ret = itv_fqd1136_readregs(p_priv, &lock, 1);
	*lock_status = ((lock & 0x47)==0x42);

	dprintk("lock=0x%02X, lock_status=0x%X\n", lock, *lock_status);

	return ret;
}

static void itv_fqd1136_debug_dump(itv_tuner_priv_t *p_priv)
{
	unsigned short lock_status = 0;

//	DEBUG_CALLSTACK

	msleep(100);

	itv_fqd1136_get_lock_status(p_priv,  &lock_status);
	dprintk("*** Lock status (0-No-signal, 1-Locked) = %d\n", lock_status);
}

static int itv_fqd1136_readregs(itv_tuner_priv_t *p_priv, unsigned char *buf, unsigned char len)
{
	struct i2c_msg msg = { .addr = p_priv->i2c_props.addr,
		.flags = I2C_M_RD, .buf = buf, .len = len };

//	DEBUG_CALLSTACK
	buf[0] = 0;

	if (i2c_transfer(p_priv->i2c_props.i2c, &msg, 1) <= 0) {
		eprintk("fqd1136 I2C read failed (len=%i)\n", (int)len);
		return FQD_RESULT_I2C_READ_FAILURE;
	}
	return FQD_RESULT_SUCCESS;
}

static int itv_fqd1136_writeregs(itv_tuner_priv_t *p_priv, unsigned char *buf, unsigned char len)
{
	struct i2c_msg msg = { .addr = p_priv->i2c_props.addr,
		.flags = 0, .buf = buf, .len = len };

//	DEBUG_CALLSTACK

	if(i2c_transfer(p_priv->i2c_props.i2c, &msg, 1) <= 0) {
		eprintk("fqd1136: I2C write failed (len=%i)\n", (int)len);
		return FQD_RESULT_I2C_WRITE_FAILURE;
	}
	return FQD_RESULT_SUCCESS;
}

static int itv_fqd1136_reset(itv_frontend_t *p_ifrontend)
{
	int ret;
	
//	DEBUG_CALLSTACK

	if(p_ifrontend->idemod.reset_tuner) {
		ret = p_ifrontend->idemod.reset_tuner(p_ifrontend, ITV_FRONTEND_COMPONENT_TUNER, ITV_FQD1136_TUNER_RESET, 0);
		if(ret)
			eprintk("fqd1136: reset failed\n");
	}
	else {
		eprintk("fqd1136: no tuner reset callback function, fatal\n");
		ret = FQD_RESULT_RESET_FAILURE;
	}

	return ret;
}

static int itv_fqd1136_set_freq_band(itv_tuner_priv_t *p_priv, int freq_hz)
{
	unsigned char freq_band;
//	DEBUG_CALLSTACK
	
	dprintk("frequency = %d\n", freq_hz);

	if((freq_hz >= 55000000) && (freq_hz <= 159000000))
		freq_band = FQD_FREQ_LOW_BAND;
	else if((freq_hz >= 160000000) && (freq_hz <= 442000000))
		freq_band = FQD_FREQ_MID_BAND;
	else if((freq_hz >= 443000000) && (freq_hz <= 863000000))
		freq_band = FQD_FREQ_HIGH_BAND;
	else
		return FQD_RESULT_OUT_OF_RANGE;

	p_priv->freq_band = freq_band;
	
	return FQD_RESULT_SUCCESS;
}

static int itv_fqd1136_set_pll(itv_tuner_priv_t *p_priv, int tuner_standard, int freq_hz)
{
	unsigned char pll_charge_pump, pll_step, pll_step_val, agc_mode;

	dprintk("(%d) tuner_standard = %s\n", tuner_standard, tuner_standard == NTSC ? "ANALOG" : "DIGITAL");

	if(tuner_standard == NTSC)
	{
		pll_charge_pump = FQD_PLL_CP_50uA;
		pll_step = FQD_PLLref_DIVIDER_64;
		pll_step_val = 16;

		agc_mode = FQD_AGC_106dBuV;
	}
	else if(tuner_standard == ATSC)
	{
		if((freq_hz >= 445000000) && (freq_hz <= 863000000))
			pll_charge_pump = FQD_PLL_CP_125uA;
		else
			pll_charge_pump = FQD_PLL_CP_50uA;	// 55MHz ~ 440MHz

		pll_step = FQD_PLLref_DIVIDER_64;
		pll_step_val = 16;

		agc_mode = FQD_AGC_115dBuV;
	}
	else if(tuner_standard == DVBT_6MHz)
	{
		if((freq_hz >= 750000000) && (freq_hz <= 863000000))
			pll_charge_pump = FQD_PLL_CP_650uA;
		else if((freq_hz >= 160000000) && (freq_hz < 750000000))
			pll_charge_pump = FQD_PLL_CP_250uA;
		else if((freq_hz > 120000000) && (freq_hz <= 159000000))
			pll_charge_pump = FQD_PLL_CP_650uA;
		else
			pll_charge_pump = FQD_PLL_CP_250uA;	// 55MHz ~ 120MHz

		pll_step = FQD_PLLref_DIVIDER_24;
		pll_step_val = 6;

		agc_mode = FQD_AGC_112dBuV;
	}
	else
		return FQD_RESULT_OUT_OF_RANGE;
	
	p_priv->pll_charge_pump = pll_charge_pump;
	p_priv->pll_step = pll_step;
	p_priv->pll_step_val = pll_step_val;
	p_priv->agc_mode = agc_mode;
	
	return FQD_RESULT_SUCCESS;
}

static int itv_fqd1136_set_if_frequency(itv_tuner_priv_t *p_priv, unsigned int freq_hz)
{
	dprintk("intermediate freq_hz = %d)\n", freq_hz);

	if(		(freq_hz != 44000000) 
		&&	(freq_hz != 45750000)
		&&	(freq_hz != 42170000)
		&&	(freq_hz != 41250000)
	)
		return FQD_RESULT_OUT_OF_RANGE;
	
	p_priv->if_freq = freq_hz;

	return FQD_RESULT_SUCCESS;
}

static int itv_fqd1136_calculate_rf_frequency(itv_frontend_t *p_ifrontend, unsigned int freq_hz)
{
	int freq_code;

	itv_tuner_priv_t *priv = p_ifrontend->ituner.priv;
	
//	DEBUG_CALLSTACK
		
	if((freq_hz > p_ifrontend->ituner.info.frequency_max) || (freq_hz < p_ifrontend->ituner.info.frequency_min))
		return FQD_RESULT_OUT_OF_RANGE;

	//20101001 koo : analog frequency 설정 시 기존 수식으로 동작시키면 값의 차이가 있어 divider ratio에 의해 설정된 reference frequency로 직접 나누도록 변경.
	//freq_code = (unsigned short)((freq_hz + priv->if_freq) / 1000000 * priv->pll_step_val);
	if(priv->pll_step == FQD_PLLref_DIVIDER_64) {
		freq_code = (unsigned short)((freq_hz + priv->if_freq) / 62500);
	} else {
		freq_code = (unsigned short)((freq_hz + priv->if_freq) / 166670);
	}

	return freq_code;
}

static unsigned short itv_fqd1136_wait_for_lock(itv_tuner_priv_t *p_priv)
{	
	int watchDogCount = 10;
	unsigned short lockState = 0;

//	DEBUG_CALLSTACK

	while((lockState == 0) && (watchDogCount > 0)) {
		itv_fqd1136_get_lock_status(p_priv, &lockState);
		if(lockState != 1) {
			msleep(5);
			watchDogCount--;
		}
	}
	return lockState;
}

static int itv_fqd1136_tune_channel(itv_frontend_t *p_ifrontend, unsigned int freq_hz)
{
	int found = 0;
	int freq_code;
	unsigned char buf[4];
	
	itv_tuner_priv_t *priv = p_ifrontend->ituner.priv;

//	DEBUG_CALLSTACK
		
	freq_code = itv_fqd1136_calculate_rf_frequency(p_ifrontend, freq_hz);

	dprintk("(%u -> %d)\n", freq_hz, freq_code);

	buf[0] = (freq_code >> 8) & 0xFF;
	buf[1] = freq_code & 0xFF;
	buf[2] = (0x80 | (priv->pll_charge_pump << 3) | (priv->pll_step << 1));
	buf[3] = priv->freq_band;

	//dprintk("CCAB[%02X %02X %02X %02X]\n", buf[0], buf[1], buf[2], buf[3] );
	if(itv_fqd1136_writeregs(priv, buf, 4) != FQD_RESULT_SUCCESS)
		return found;

	buf[0] = (0x80 | (0x3 << 3) | (priv->pll_step << 1));
	buf[1] = (0x80 | (priv->agc_mode << 4));

	//dprintk("BB[%02X %02X]\n", buf[0], buf[1] );
	if(itv_fqd1136_writeregs(priv, buf, 2) != FQD_RESULT_SUCCESS)
		return found;

	//analog ch scan 일경우에만 read 한 status 중 lock & afc 를 check하여 analog ch lock status check 함.
	if(priv->tuner_standard == NTSC)
	{
		if(itv_fqd1136_wait_for_lock(priv) == 1)
			found = 1;
	}

	return found;
}

static int itv_fqd1136_init(itv_frontend_t *p_ifrontend)
{
	itv_tuner_priv_t *priv = p_ifrontend->ituner.priv;
	int ret, result;
	unsigned char val;

	DEBUG_CALLSTACK

	ret = itv_fqd1136_readregs(priv, &val, 1);

	if(ret == FQD_RESULT_SUCCESS)
	{
		printk("FQD1136 READ %x\n", val);
		
		if(val & 0x80)
			ret = FQD_RESULT_SUCCESS;
		else
		{
			result = itv_fqd1136_reset(p_ifrontend);
			if(result != FQD_RESULT_SUCCESS)
				ret = FQD_RESULT_RESET_FAILURE;
			else
			{
				ret = itv_fqd1136_readregs(priv, &val, 1);
				printk("FQD1136 READ after reset %x\n", val);
				ret = FQD_RESULT_SUCCESS;
			}
		}
	}

#ifdef TUNER_FQD1136_TEST
	test_analog_tune(p_ifrontend);
#endif
	
	if(itv_fqd1136_module_debug)
		itv_fqd1136_debug_dump(priv);

	return 0;
}

static int itv_fqd1136_set_params(itv_frontend_t *p_ifrontend, itv_frontend_parameters_t *params)
{
	int ret;

	itv_tuner_priv_t *priv = p_ifrontend->ituner.priv;

//	DEBUG_CALLSTACK

	dprintk("frequency=%d (Hz)\n", params->frequency);
	
	switch(params->u.vsb.modulation) {
		case VSB_8:
		case VSB_16:
			dprintk("VSB modulation\n");
			priv->freq_hz = params->frequency;
			priv->tuner_standard = ATSC;
			priv->bandwidth = BANDWIDTH_6_MHZ;
			priv->if_freq = 44000000;
			break;
		case QAM_64:
		case QAM_256:
		case QAM_AUTO:
			dprintk("QAM modulation\n");
			priv->freq_hz = params->frequency;
			priv->tuner_standard = ATSC;
			priv->bandwidth = BANDWIDTH_6_MHZ;
			priv->if_freq = 44000000;
			break;
		default:
			return -EINVAL;
	}

	dprintk("frequency=%d (compensated)\n", priv->freq_hz);

	ret = itv_fqd1136_set_freq_band(priv, priv->freq_hz);
	if(ret != FQD_RESULT_SUCCESS)
	{
		eprintk("fqd1136: frequency band (%d) failed\n", priv->freq_hz);
		return -EREMOTEIO;
	}

	ret = itv_fqd1136_set_pll(priv, priv->tuner_standard, priv->freq_hz);
	if(ret != FQD_RESULT_SUCCESS)
	{
		eprintk("fqd1136: pll (%d) failed\n", priv->freq_hz);
		return -EREMOTEIO;
	}

	itv_fqd1136_tune_channel(p_ifrontend, priv->freq_hz);

	if(itv_fqd1136_module_debug)
		itv_fqd1136_debug_dump(priv);

	return 0;
}

//20100413 koo : analog param set
static int itv_fqd1136_set_analog_params(itv_frontend_t *p_ifrontend, itv_frontend_parameters_t *params)
{
	int ret;

	itv_tuner_priv_t *priv = p_ifrontend->ituner.priv;

	dprintk("frequency=%d (Hz) analog\n", params->frequency);

	priv->freq_hz 			= params->frequency;
	priv->tuner_standard 	= NTSC;
	priv->if_freq			= 44000000;

	ret = itv_fqd1136_set_freq_band(priv, priv->freq_hz);
	if(ret != FQD_RESULT_SUCCESS)
	{
		eprintk("fqd1136: frequency band (%d) failed\n", priv->freq_hz);
		return -EREMOTEIO;
	}

	ret = itv_fqd1136_set_pll(priv, priv->tuner_standard, priv->freq_hz);
	if(ret != FQD_RESULT_SUCCESS)
	{
		eprintk("fqd1136: pll (%d) failed\n", priv->freq_hz);
		return -EREMOTEIO;
	}

	ret = itv_fqd1136_set_if_frequency(priv, priv->if_freq);
	if(ret != FQD_RESULT_SUCCESS)
	{
		eprintk("fqd1136: xc_Set_IF_frequency(%d) failed\n", priv->if_freq);
		return -EIO;
	}

	ret = itv_fqd1136_tune_channel(p_ifrontend, priv->freq_hz);

//	if(itv_fqd1136_module_debug)
//		itv_fqd1136_debug_dump(priv);

	return ret;
}

static int itv_fqd1136_get_frequency(itv_frontend_t *p_ifrontend, unsigned int *freq)
{
	itv_tuner_priv_t *priv = p_ifrontend->ituner.priv;

//	DEBUG_CALLSTACK
		
	*freq = priv->freq_hz;
	return 0;
}

static int itv_fqd1136_get_bandwidth(itv_frontend_t *p_ifrontend, unsigned int *bw)
{
	itv_tuner_priv_t *priv = p_ifrontend->ituner.priv;

//	DEBUG_CALLSTACK

	*bw = priv->bandwidth;
	return 0;
}

static int itv_fqd1136_get_status(itv_frontend_t *p_ifrontend, unsigned int *status)
{
	unsigned short lock_status = 0;

	itv_tuner_priv_t *priv = p_ifrontend->ituner.priv;

//	DEBUG_CALLSTACK

	itv_fqd1136_get_lock_status(priv, &lock_status);

	dprintk("lock_status = 0x%02x\n", lock_status);

	*status = lock_status;

	return 0;
}

static itv_fqd1136_config_t fqd1136_config = {
	.i2c_address      = 0x61,
	.if_freq          = 44000000,
};

static const itv_tuner_t fqd1136_tuner = {
	.info = {
		.name           = "NuTune FQD5000",
		.frequency_min  =  55000000,
		.frequency_max  = 863000000,
	},

	.init				= itv_fqd1136_init,
	.set_params			= itv_fqd1136_set_params,

	//20100413 koo : set_analog_params define
	.set_analog_params	= itv_fqd1136_set_analog_params,
		
	.get_frequency		= itv_fqd1136_get_frequency,
	.get_bandwidth		= itv_fqd1136_get_bandwidth,
	.get_status			= itv_fqd1136_get_status
};

static int itv_fqd1136_activate(itv_object_t *p_this)
{	
	itv_frontend_t *ifrontend = (itv_frontend_t *)p_this;
	itv_platform_frontend_operations_t *fe_ops = ifrontend->fe_ops;
	itv_tuner_priv_t *priv = NULL;
	
	DEBUG_CALLSTACK
	
	priv = kmalloc(sizeof(*priv), GFP_KERNEL);
	if(priv == NULL)
	{
		eprintk("out of memory\n");
		return -1;
	}

	priv->i2c_props.addr 	= fqd1136_config.i2c_address;
	priv->i2c_props.i2c 	= fe_ops->i2c_get_adapter();

	if(!priv->i2c_props.i2c) {
		eprintk("i2c_get_adapter is failed\n");
		kfree(priv);
		return -1;
	}

	priv->bandwidth	= BANDWIDTH_6_MHZ;
	priv->if_freq	= fqd1136_config.if_freq;

	memcpy(&ifrontend->ituner, &fqd1136_tuner, sizeof(itv_tuner_t));
	ifrontend->ituner.priv = priv;

	if(itv_fqd1136_init(ifrontend) != 0)
	{
		eprintk("Failed to initialize correctly\n");
		fe_ops->i2c_put_adapter(priv->i2c_props.i2c);
		ifrontend->ituner.priv = NULL;
		kfree(priv);
		return -1;
	}

	printk("%s : fqd1136 detect ok!!\n", __func__);

	return 0;
}

static void itv_fqd1136_deactivate(itv_object_t *p_this)
{
	itv_frontend_t *ifrontend = (itv_frontend_t *)p_this;
	itv_platform_frontend_operations_t *fe_ops = ifrontend->fe_ops;
	itv_tuner_priv_t *priv = ifrontend->ituner.priv;
	
	DEBUG_CALLSTACK

	ifrontend->ituner.priv = NULL;
	fe_ops->i2c_put_adapter(priv->i2c_props.i2c);
	kfree(priv);
}

itv_module_t ifqd1136_module = {
	.psz_module_name = "fqd1136", 
	.psz_capability = "ituner", 
	.i_score = 20, 
	.pf_activate = itv_fqd1136_activate,
	.pf_deactivate = itv_fqd1136_deactivate,
	.next = NULL, 
	.parent = NULL, 
	.submodule = NULL, 
	.i_submodules = 0
};

static int __init itv_fqd1136_module_init(void)
{
	int retval;

	DEBUG_CALLSTACK

	if((retval = itv_module_register(&ifqd1136_module)) != 0)
	{
		eprintk("itv_module_register() is failed\n");
		goto out;
	}

	dprintk("iTV fqd1136 module register success\n");

out:
	return retval;
}

static void __exit itv_fqd1136_module_exit(void)
{
	DEBUG_CALLSTACK
		
	itv_module_unregister(&ifqd1136_module);

	dprintk("iTV fqd1136 module unregister success\n");
}

module_init(itv_fqd1136_module_init);
module_exit(itv_fqd1136_module_exit);

MODULE_AUTHOR("RONALD");
MODULE_LICENSE("GPL");
