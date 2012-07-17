#include <i-tv/itv_common.h>
#include <i-tv/itv_adapter.h>
#include <i-tv/itv_frontend.h>

#include "ifrontend.h"

static int itv_frontend_debug;
static int itv_frontend_debug_callstack;
static int itv_frontend_shutdown_timeout;
static int itv_frontend_force_auto_inversion;
static int itv_frontend_override_tune_delay;
static int itv_frontend_powerdown_on_sleep = 1;

module_param_named(ifrontend_debug, itv_frontend_debug, int, 0644);
module_param_named(ifrontend_debug_cs, itv_frontend_debug_callstack, int, 0644);
module_param_named(ifrontend_shutdown_timeout, itv_frontend_shutdown_timeout, int, 0644);
module_param_named(ifrontend_force_auto_inversion, itv_frontend_force_auto_inversion, int, 0644);
module_param_named(ifrontend_override_tune_delay, itv_frontend_override_tune_delay, int, 0644);
module_param_named(ifrontend_powerdown_on_sleep, itv_frontend_powerdown_on_sleep, int, 0644);

MODULE_PARM_DESC(ifrontend_debug, "Turn on/off frontend debugging (default:off).");
MODULE_PARM_DESC(ifrontend_debug_cs, "Turn on/off frontend callstack debugging (default:off).");
MODULE_PARM_DESC(ifrontend_shutdown_timeout, "wait <shutdown_timeout> seconds after close() before suspending hardware");
MODULE_PARM_DESC(ifrontend_force_auto_inversion, "0: normal (default), 1: INVERSION_AUTO forced always");
MODULE_PARM_DESC(ifrontend_override_tune_delay, "0: normal (default), >0 => delay in milliseconds to wait for lock after a tune attempt");
MODULE_PARM_DESC(ifrontend_powerdown_on_sleep, "0: do not power down, 1: turn LNB voltage off on sleep (default)");

#ifdef dprintk
#undef dprintk
#endif

#define dprintk(args...) \
	do { if(itv_frontend_debug) { printk("[%s] ", __func__); printk(args); } } while (0)

#define DEBUG_CALLSTACK		\
	do { if(itv_frontend_debug_callstack) { printk("[Call] ----> %s()\n", __func__); } } while (0);

char *itv_atsc_demod_name[] = {
	"s5h1411",
	"lgdt3305",
	NULL
	};

char *itv_atsc_tuner_name[] = {
	"xc5000",
	"xc5000c",
	"fqd1136",
	"fj2148",
	NULL
	};

itv_dtv_cmds_h_t dtv_cmds[] = {
	[ITV_DTV_TUNE] = {
		.name	= "DTV_TUNE",
		.cmd	= ITV_DTV_TUNE,
		.set	= 1,
	},
	[ITV_DTV_CLEAR] = {
		.name	= "DTV_CLEAR",
		.cmd	= ITV_DTV_CLEAR,
		.set	= 1,
	},

	/* Set */
	[ITV_DTV_FREQUENCY] = {
		.name	= "DTV_FREQUENCY",
		.cmd	= ITV_DTV_FREQUENCY,
		.set	= 1,
	},
	[ITV_DTV_BANDWIDTH_HZ] = {
		.name	= "DTV_BANDWIDTH_HZ",
		.cmd	= ITV_DTV_BANDWIDTH_HZ,
		.set	= 1,
	},
	[ITV_DTV_MODULATION] = {
		.name	= "DTV_MODULATION",
		.cmd	= ITV_DTV_MODULATION,
		.set	= 1,
	},
	[ITV_DTV_INVERSION] = {
		.name	= "DTV_INVERSION",
		.cmd	= ITV_DTV_INVERSION,
		.set	= 1,
	},
	[ITV_DTV_DISEQC_MASTER] = {
		.name	= "DTV_DISEQC_MASTER",
		.cmd	= ITV_DTV_DISEQC_MASTER,
		.set	= 1,
		.buffer	= 1,
	},
	[ITV_DTV_SYMBOL_RATE] = {
		.name	= "DTV_SYMBOL_RATE",
		.cmd	= ITV_DTV_SYMBOL_RATE,
		.set	= 1,
	},
	[ITV_DTV_INNER_FEC] = {
		.name	= "DTV_INNER_FEC",
		.cmd	= ITV_DTV_INNER_FEC,
		.set	= 1,
	},
	[ITV_DTV_VOLTAGE] = {
		.name	= "DTV_VOLTAGE",
		.cmd	= ITV_DTV_VOLTAGE,
		.set	= 1,
	},
	[ITV_DTV_TONE] = {
		.name	= "DTV_TONE",
		.cmd	= ITV_DTV_TONE,
		.set	= 1,
	},
	[ITV_DTV_PILOT] = {
		.name	= "DTV_PILOT",
		.cmd	= ITV_DTV_PILOT,
		.set	= 1,
	},
	[ITV_DTV_ROLLOFF] = {
		.name	= "DTV_ROLLOFF",
		.cmd	= ITV_DTV_ROLLOFF,
		.set	= 1,
	},
	[ITV_DTV_DELIVERY_SYSTEM] = {
		.name	= "DTV_DELIVERY_SYSTEM",
		.cmd	= ITV_DTV_DELIVERY_SYSTEM,
		.set	= 1,
	},
	[ITV_DTV_HIERARCHY] = {
		.name	= "DTV_HIERARCHY",
		.cmd	= ITV_DTV_HIERARCHY,
		.set	= 1,
	},
	[ITV_DTV_CODE_RATE_HP] = {
		.name	= "DTV_CODE_RATE_HP",
		.cmd	= ITV_DTV_CODE_RATE_HP,
		.set	= 1,
	},
	[ITV_DTV_CODE_RATE_LP] = {
		.name	= "DTV_CODE_RATE_LP",
		.cmd	= ITV_DTV_CODE_RATE_LP,
		.set	= 1,
	},
	[ITV_DTV_GUARD_INTERVAL] = {
		.name	= "DTV_GUARD_INTERVAL",
		.cmd	= ITV_DTV_GUARD_INTERVAL,
		.set	= 1,
	},
	[ITV_DTV_TRANSMISSION_MODE] = {
		.name	= "DTV_TRANSMISSION_MODE",
		.cmd	= ITV_DTV_TRANSMISSION_MODE,
		.set	= 1,
	},
	/* Get */
	[ITV_DTV_DISEQC_SLAVE_REPLY] = {
		.name	= "DTV_DISEQC_SLAVE_REPLY",
		.cmd	= ITV_DTV_DISEQC_SLAVE_REPLY,
		.set	= 0,
		.buffer	= 1,
	},
	[ITV_DTV_API_VERSION] = {
		.name	= "DTV_API_VERSION",
		.cmd	= ITV_DTV_API_VERSION,
		.set	= 0,
	},
	[ITV_DTV_CODE_RATE_HP] = {
		.name	= "DTV_CODE_RATE_HP",
		.cmd	= ITV_DTV_CODE_RATE_HP,
		.set	= 0,
	},
	[ITV_DTV_CODE_RATE_LP] = {
		.name	= "DTV_CODE_RATE_LP",
		.cmd	= ITV_DTV_CODE_RATE_LP,
		.set	= 0,
	},
	[ITV_DTV_GUARD_INTERVAL] = {
		.name	= "DTV_GUARD_INTERVAL",
		.cmd	= ITV_DTV_GUARD_INTERVAL,
		.set	= 0,
	},
	[ITV_DTV_TRANSMISSION_MODE] = {
		.name	= "DTV_TRANSMISSION_MODE",
		.cmd	= ITV_DTV_TRANSMISSION_MODE,
		.set	= 0,
	},
	[ITV_DTV_HIERARCHY] = {
		.name	= "DTV_HIERARCHY",
		.cmd	= ITV_DTV_HIERARCHY,
		.set	= 0,
	},
};

static int itv_frontend_ioctl_legacy(struct file *file, unsigned int cmd, void *parg);

//#define FE_LOCK_TIME_CHECK
#ifdef FE_LOCK_TIME_CHECK
typedef int64_t mtime_t;

mtime_t imdate(void)
{
	struct timeval tv_date;
	struct timespec tv_set;

	do_gettimeofday(&tv_date);

	// exception
	if(tv_date.tv_sec < 0)
	{
		tv_set.tv_sec = tv_date.tv_sec * (-1);
		tv_set.tv_nsec = tv_date.tv_usec * 1000;
		do_settimeofday(&tv_set);
		do_gettimeofday(&tv_date);
	}

    return (mtime_t)tv_date.tv_sec * 1000000 + (mtime_t)tv_date.tv_usec;
}
#endif

static int itv_frontend_timeval_usec_diff(struct timeval lasttime, struct timeval curtime)
{
//	DEBUG_CALLSTACK
		
	return ((curtime.tv_usec < lasttime.tv_usec) ?
		1000000 - lasttime.tv_usec + curtime.tv_usec :
		curtime.tv_usec - lasttime.tv_usec);
}

static inline void itv_frontend_timeval_usec_add(struct timeval *curtime, unsigned int add_usec)
{
//	DEBUG_CALLSTACK
		
	curtime->tv_usec += add_usec;
	if(curtime->tv_usec >= 1000000) {
		curtime->tv_usec -= 1000000;
		curtime->tv_sec++;
	}
}

static void itv_frontend_dtv_property_dump(itv_dtv_property_t *tvp)
{
	int i;

//	DEBUG_CALLSTACK

	if(tvp->cmd <= 0 || tvp->cmd > ITV_DTV_MAX_COMMAND) {
		wprintk("tvp.cmd = 0x%08x undefined\n", tvp->cmd);
		return;
	}

	dprintk("tvp.cmd    = 0x%08x (%s)\n", tvp->cmd, dtv_cmds[tvp->cmd].name);

	if(dtv_cmds[tvp->cmd].buffer) {
		dprintk("tvp.u.buffer.len = 0x%02x\n", tvp->u.buffer.len);
		for(i = 0; i < tvp->u.buffer.len; i++) {
			dprintk("tvp.u.buffer.data[0x%02x] = 0x%02x\n", i, tvp->u.buffer.data[i]);
		}

	}
	else
		dprintk("tvp.u.data = 0x%08x\n", tvp->u.data);
}

static int itv_frontend_is_legacy_delivery_system(itv_frontend_delivery_system_e s)
{
//	DEBUG_CALLSTACK
		
	if((s == SYS_UNDEFINED) || (s == SYS_DVBC_ANNEX_AC) ||
	   (s == SYS_DVBC_ANNEX_B) || (s == SYS_DVBT) || (s == SYS_DVBS) ||
	   (s == SYS_ATSC))
		return 1;

	return 0;
}

void itv_frontend_dtv_property_cache_sync(itv_frontend_t *p_ifrontend, itv_frontend_parameters_t *params)
{
	itv_dtv_frontend_properties_t *props = &p_ifrontend->dtv_property_cache;

//	DEBUG_CALLSTACK

	props->frequency = params->frequency;
	props->inversion = params->inversion;

	switch(p_ifrontend->idemod.info.type) {
		case FE_QPSK:
			props->modulation = QPSK;
			props->rolloff = ROLLOFF_35;
			props->symbol_rate = params->u.qpsk.symbol_rate;
			props->fec_inner = params->u.qpsk.fec_inner;
			props->delivery_system = SYS_DVBS;
			break;
		case FE_QAM:
			props->symbol_rate = params->u.qam.symbol_rate;
			props->fec_inner = params->u.qam.fec_inner;
			props->modulation = params->u.qam.modulation;
			props->delivery_system = SYS_DVBC_ANNEX_AC;
			break;
		case FE_OFDM:
			if(params->u.ofdm.bandwidth == BANDWIDTH_6_MHZ)
				props->bandwidth_hz = 6000000;
			else if(params->u.ofdm.bandwidth == BANDWIDTH_7_MHZ)
				props->bandwidth_hz = 7000000;
			else if(params->u.ofdm.bandwidth == BANDWIDTH_8_MHZ)
				props->bandwidth_hz = 8000000;
			else
				props->bandwidth_hz = 0;
			props->code_rate_HP = params->u.ofdm.code_rate_HP;
			props->code_rate_LP = params->u.ofdm.code_rate_LP;
			props->modulation = params->u.ofdm.constellation;
			props->transmission_mode = params->u.ofdm.transmission_mode;
			props->guard_interval = params->u.ofdm.guard_interval;
			props->hierarchy = params->u.ofdm.hierarchy_information;
			props->delivery_system = SYS_DVBT;
			break;
		case FE_ATSC:
			props->modulation = params->u.vsb.modulation;
			if((props->modulation == VSB_8) || (props->modulation == VSB_16))
				props->delivery_system = SYS_ATSC;
			else
				props->delivery_system = SYS_DVBC_ANNEX_B;
			break;
	}
}

static void itv_frontend_dtv_property_legacy_params_sync(itv_frontend_t *p_ifrontend)
{	
	itv_dtv_frontend_properties_t *props = &p_ifrontend->dtv_property_cache;
	itv_frontend_priv_t *priv = p_ifrontend->priv;
	itv_frontend_parameters_t *params = &priv->params;

//	DEBUG_CALLSTACK

	params->frequency = props->frequency;
	params->inversion = props->inversion;

	switch(p_ifrontend->idemod.info.type) {
		case FE_QPSK:
			dprintk("Preparing QPSK req\n");
			params->u.qpsk.symbol_rate = props->symbol_rate;
			params->u.qpsk.fec_inner = props->fec_inner;
			props->delivery_system = SYS_DVBS;
			break;
		case FE_QAM:
			dprintk("Preparing QAM req\n");
			params->u.qam.symbol_rate = props->symbol_rate;
			params->u.qam.fec_inner = props->fec_inner;
			params->u.qam.modulation = props->modulation;
			props->delivery_system = SYS_DVBC_ANNEX_AC;
			break;
		case FE_OFDM:
			dprintk("Preparing OFDM req\n");
			if(props->bandwidth_hz == 6000000)
				params->u.ofdm.bandwidth = BANDWIDTH_6_MHZ;
			else if(props->bandwidth_hz == 7000000)
				params->u.ofdm.bandwidth = BANDWIDTH_7_MHZ;
			else if(props->bandwidth_hz == 8000000)
				params->u.ofdm.bandwidth = BANDWIDTH_8_MHZ;
			else
				params->u.ofdm.bandwidth = BANDWIDTH_AUTO;
			params->u.ofdm.code_rate_HP = props->code_rate_HP;
			params->u.ofdm.code_rate_LP = props->code_rate_LP;
			params->u.ofdm.constellation = props->modulation;
			params->u.ofdm.transmission_mode = props->transmission_mode;
			params->u.ofdm.guard_interval = props->guard_interval;
			params->u.ofdm.hierarchy_information = props->hierarchy;
			props->delivery_system = SYS_DVBT;
			break;
		case FE_ATSC:
			dprintk("Preparing VSB req\n");
			params->u.vsb.modulation = props->modulation;
			if((props->modulation == VSB_8) || (props->modulation == VSB_16))
				props->delivery_system = SYS_ATSC;
			else
				props->delivery_system = SYS_DVBC_ANNEX_B;
			break;
	}
}

void itv_frontend_dtv_property_adv_params_sync(itv_frontend_t *p_ifrontend)
{
	itv_dtv_frontend_properties_t *props = &p_ifrontend->dtv_property_cache;
	itv_frontend_priv_t *priv = p_ifrontend->priv;
	itv_frontend_parameters_t *params = &priv->params;

//	DEBUG_CALLSTACK

	params->frequency = props->frequency;
	params->inversion = props->inversion;

	switch(props->modulation) {
		case PSK_8:
		case APSK_16:
		case APSK_32:
		case QPSK:
			params->u.qpsk.symbol_rate = props->symbol_rate;
			params->u.qpsk.fec_inner = props->fec_inner;
			break;
		default:
			break;
	}

	if(props->delivery_system == SYS_ISDBT) {
		params->frequency = props->frequency;
		params->inversion = INVERSION_AUTO;
		params->u.ofdm.constellation = QAM_AUTO;
		params->u.ofdm.code_rate_HP = FEC_AUTO;
		params->u.ofdm.code_rate_LP = FEC_AUTO;
		params->u.ofdm.bandwidth = BANDWIDTH_AUTO;
		params->u.ofdm.transmission_mode = TRANSMISSION_MODE_AUTO;
		params->u.ofdm.guard_interval = GUARD_INTERVAL_AUTO;
		params->u.ofdm.hierarchy_information = HIERARCHY_AUTO;
	}
}

static void itv_frontend_dtv_property_cache_submit(itv_frontend_t *p_ifrontend)
{
	itv_dtv_frontend_properties_t *props = &p_ifrontend->dtv_property_cache;

//	DEBUG_CALLSTACK

	if(itv_frontend_is_legacy_delivery_system(props->delivery_system)) {
		dprintk("legacy, modulation = %d\n", props->modulation);
		itv_frontend_dtv_property_legacy_params_sync(p_ifrontend);
	}
	else {
		dprintk("adv, modulation = %d\n", props->modulation);
		itv_frontend_dtv_property_adv_params_sync(p_ifrontend);
	}
}

static int itv_frontend_dtv_property_process_get(itv_frontend_t *p_ifrontend, itv_dtv_property_t *tvp, struct file *file)
{
	int retval = 0;

//	DEBUG_CALLSTACK

	itv_frontend_dtv_property_dump(tvp);

	if(p_ifrontend->idemod.get_property) {
		retval = p_ifrontend->idemod.get_property(p_ifrontend, tvp);
		if(retval < 0) return retval;
	}

	switch(tvp->cmd) {
		case ITV_DTV_FREQUENCY:
			tvp->u.data = p_ifrontend->dtv_property_cache.frequency;
			break;
		case ITV_DTV_MODULATION:
			tvp->u.data = p_ifrontend->dtv_property_cache.modulation;
			break;
		case ITV_DTV_BANDWIDTH_HZ:
			tvp->u.data = p_ifrontend->dtv_property_cache.bandwidth_hz;
			break;
		case ITV_DTV_INVERSION:
			tvp->u.data = p_ifrontend->dtv_property_cache.inversion;
			break;
		case ITV_DTV_SYMBOL_RATE:
			tvp->u.data = p_ifrontend->dtv_property_cache.symbol_rate;
			break;
		case ITV_DTV_INNER_FEC:
			tvp->u.data = p_ifrontend->dtv_property_cache.fec_inner;
			break;
		case ITV_DTV_PILOT:
			tvp->u.data = p_ifrontend->dtv_property_cache.pilot;
			break;
		case ITV_DTV_ROLLOFF:
			tvp->u.data = p_ifrontend->dtv_property_cache.rolloff;
			break;
		case ITV_DTV_DELIVERY_SYSTEM:
			tvp->u.data = p_ifrontend->dtv_property_cache.delivery_system;
			break;
		case ITV_DTV_VOLTAGE:
			tvp->u.data = p_ifrontend->dtv_property_cache.voltage;
			break;
		case ITV_DTV_TONE:
			tvp->u.data = p_ifrontend->dtv_property_cache.sectone;
			break;
		case ITV_DTV_API_VERSION:
			tvp->u.data = (ITV_API_VERSION << 8) | ITV_API_VERSION_MINOR;
			break;
		case ITV_DTV_CODE_RATE_HP:
			tvp->u.data = p_ifrontend->dtv_property_cache.code_rate_HP;
			break;
		case ITV_DTV_CODE_RATE_LP:
			tvp->u.data = p_ifrontend->dtv_property_cache.code_rate_LP;
			break;
		case ITV_DTV_GUARD_INTERVAL:
			tvp->u.data = p_ifrontend->dtv_property_cache.guard_interval;
			break;
		case ITV_DTV_TRANSMISSION_MODE:
			tvp->u.data = p_ifrontend->dtv_property_cache.transmission_mode;
			break;
		case ITV_DTV_HIERARCHY:
			tvp->u.data = p_ifrontend->dtv_property_cache.hierarchy;
			break;
		default:
			retval = -1;
			break;
	}

	return retval;
}

static int itv_frontend_dtv_property_process_set(itv_frontend_t *p_ifrontend, itv_dtv_property_t *tvp, struct file *file)
{
	int retval = 0;

	itv_frontend_priv_t *priv = p_ifrontend->priv;

//	DEBUG_CALLSTACK
	
	itv_frontend_dtv_property_dump(tvp);

	if(p_ifrontend->idemod.set_property) {
		retval = p_ifrontend->idemod.set_property(p_ifrontend, tvp);
		if(retval < 0) return retval;
	}

	switch(tvp->cmd) {
		case ITV_DTV_CLEAR:
			dprintk("Flushing property cache\n");
			memset(&p_ifrontend->dtv_property_cache, 0, sizeof(itv_dtv_frontend_properties_t));
			p_ifrontend->dtv_property_cache.state = tvp->cmd;
			p_ifrontend->dtv_property_cache.delivery_system = SYS_UNDEFINED;
			break;
		case ITV_DTV_TUNE:
			p_ifrontend->dtv_property_cache.state = tvp->cmd;
			dprintk("Finalised property cache\n");
			itv_frontend_dtv_property_cache_submit(p_ifrontend);

			retval |= itv_frontend_ioctl_legacy(file, ITV_FE_SET_FRONTEND, &priv->params);
			break;
		case ITV_DTV_FREQUENCY:
			p_ifrontend->dtv_property_cache.frequency = tvp->u.data;
			break;
		case ITV_DTV_MODULATION:
			p_ifrontend->dtv_property_cache.modulation = tvp->u.data;
			break;
		case ITV_DTV_BANDWIDTH_HZ:
			p_ifrontend->dtv_property_cache.bandwidth_hz = tvp->u.data;
			break;
		case ITV_DTV_INVERSION:
			p_ifrontend->dtv_property_cache.inversion = tvp->u.data;
			break;
		case ITV_DTV_SYMBOL_RATE:
			p_ifrontend->dtv_property_cache.symbol_rate = tvp->u.data;
			break;
		case ITV_DTV_INNER_FEC:
			p_ifrontend->dtv_property_cache.fec_inner = tvp->u.data;
			break;
		case ITV_DTV_PILOT:
			p_ifrontend->dtv_property_cache.pilot = tvp->u.data;
			break;
		case ITV_DTV_ROLLOFF:
			p_ifrontend->dtv_property_cache.rolloff = tvp->u.data;
			break;
		case ITV_DTV_DELIVERY_SYSTEM:
			p_ifrontend->dtv_property_cache.delivery_system = tvp->u.data;
			break;
		case ITV_DTV_VOLTAGE:
			p_ifrontend->dtv_property_cache.voltage = tvp->u.data;
			retval = itv_frontend_ioctl_legacy(file, ITV_FE_SET_VOLTAGE, (void *)p_ifrontend->dtv_property_cache.voltage);
			break;
		case ITV_DTV_TONE:
			p_ifrontend->dtv_property_cache.sectone = tvp->u.data;
			retval = itv_frontend_ioctl_legacy(file, ITV_FE_SET_TONE, (void *)p_ifrontend->dtv_property_cache.sectone);
			break;
		case ITV_DTV_CODE_RATE_HP:
			p_ifrontend->dtv_property_cache.code_rate_HP = tvp->u.data;
			break;
		case ITV_DTV_CODE_RATE_LP:
			p_ifrontend->dtv_property_cache.code_rate_LP = tvp->u.data;
			break;
		case ITV_DTV_GUARD_INTERVAL:
			p_ifrontend->dtv_property_cache.guard_interval = tvp->u.data;
			break;
		case ITV_DTV_TRANSMISSION_MODE:
			p_ifrontend->dtv_property_cache.transmission_mode = tvp->u.data;
			break;
		case ITV_DTV_HIERARCHY:
			p_ifrontend->dtv_property_cache.hierarchy = tvp->u.data;
			break;
		default:
			retval = -1;
			break;
	}

	return retval;
}

static void itv_frontend_get_frequeny_limits(itv_frontend_t *p_ifrontend, unsigned int *freq_min, unsigned int *freq_max)
{
//	DEBUG_CALLSTACK
		
	*freq_min = max(p_ifrontend->idemod.info.frequency_min, p_ifrontend->ituner.info.frequency_min);

	if(p_ifrontend->idemod.info.frequency_max == 0)
		*freq_max = p_ifrontend->ituner.info.frequency_max;
	else if(p_ifrontend->ituner.info.frequency_max == 0)
		*freq_max = p_ifrontend->idemod.info.frequency_max;
	else
		*freq_max = min(p_ifrontend->idemod.info.frequency_max, p_ifrontend->ituner.info.frequency_max);

	if(*freq_min == 0 || *freq_max == 0) {
		wprintk("adapter %i frontend %u frequency limits undefined - fix the driver\n", 
			((itv_adapter_t *)p_ifrontend->iadapter)->id, p_ifrontend->id);
	}
}

static int itv_frontend_check_parameters(itv_frontend_t *p_ifrontend, itv_frontend_parameters_t *params)
{
	unsigned int freq_min;
	unsigned int freq_max;

//	DEBUG_CALLSTACK

	itv_frontend_get_frequeny_limits(p_ifrontend, &freq_min, &freq_max);
	if((freq_min && params->frequency < freq_min) || 
		(freq_max && params->frequency > freq_max)) {
		wprintk("adapter %i frontend %i frequency %u out of range (%u..%u)\n", 
			((itv_adapter_t *)p_ifrontend->iadapter)->id, p_ifrontend->id, params->frequency, freq_min, freq_max);
		return -EINVAL;
	}

	if(p_ifrontend->idemod.info.type == FE_QPSK) {
		if((p_ifrontend->idemod.info.symbol_rate_min && params->u.qpsk.symbol_rate < p_ifrontend->idemod.info.symbol_rate_min) || 
			(p_ifrontend->idemod.info.symbol_rate_max && params->u.qpsk.symbol_rate > p_ifrontend->idemod.info.symbol_rate_max)) {
			wprintk("adapter %i frontend %i symbol rate %u out of range (%u..%u)\n", 
				((itv_adapter_t *)p_ifrontend->iadapter)->id, p_ifrontend->id, params->u.qpsk.symbol_rate, 
				p_ifrontend->idemod.info.symbol_rate_min, p_ifrontend->idemod.info.symbol_rate_max);
			return -EINVAL;
		}
	}
	else if(p_ifrontend->idemod.info.type == FE_QAM) {
		if((p_ifrontend->idemod.info.symbol_rate_min && params->u.qam.symbol_rate < p_ifrontend->idemod.info.symbol_rate_min) || 
			(p_ifrontend->idemod.info.symbol_rate_max && params->u.qam.symbol_rate > p_ifrontend->idemod.info.symbol_rate_max)) {
			wprintk("adapter %i frontend %i symbol rate %u out of range (%u..%u)\n", 
				((itv_adapter_t *)p_ifrontend->iadapter)->id, p_ifrontend->id, params->u.qam.symbol_rate, 
				p_ifrontend->idemod.info.symbol_rate_min, p_ifrontend->idemod.info.symbol_rate_max);
			return -EINVAL;
		}
	}

	return 0;
}

static void itv_frontend_sleep_until(struct timeval *waketime, unsigned int add_usec)
{	
	unsigned int delta, newdelta;

	struct timeval lasttime;

//	DEBUG_CALLSTACK

	itv_frontend_timeval_usec_add(waketime, add_usec);

	do_gettimeofday(&lasttime);
	delta = itv_frontend_timeval_usec_diff(lasttime, *waketime);
	if(delta > 2500) {
		msleep((delta - 1500) / 1000);
		do_gettimeofday(&lasttime);
		newdelta = itv_frontend_timeval_usec_diff(lasttime, *waketime);
		delta = (newdelta > delta) ? 0 : newdelta;
	}
	if(delta > 0)
		udelay(delta);
}

static void itv_frontend_destructor(itv_object_t *p_this)
{
	itv_frontend_t *ifrontend = (itv_frontend_t *)p_this;
	itv_frontend_priv_t *priv = ifrontend->priv;

	DEBUG_CALLSTACK

	if(!priv)
		return;

	if(priv->ituner)
		itv_module_unneed(ITV_OBJECT(ifrontend), priv->ituner);

	if(priv->idemod)
		itv_module_unneed(ITV_OBJECT(ifrontend), priv->idemod);

	// put platform frontend operations
	if(ifrontend->fe_ops)
		itv_platform_put_frontend_operations(ifrontend->fe_ops);

	kfree(priv);
}

static int itv_frontend_is_exiting(itv_frontend_t *p_ifrontend)
{
	itv_frontend_priv_t *priv = p_ifrontend->priv;
	
//	DEBUG_CALLSTACK
		
	if(p_ifrontend->exit)
		return 1;

	if(p_ifrontend->writers == 1) {
		if(time_after(jiffies, priv->release_jiffies + itv_frontend_shutdown_timeout * HZ))
			return 1;
	}

	return 0;
}

static int itv_frontend_should_wakeup(itv_frontend_t *p_ifrontend)
{
	itv_frontend_priv_t *priv = p_ifrontend->priv;
	
//	DEBUG_CALLSTACK
		
	if(priv->wakeup) {
		priv->wakeup = 0;
		return 1;
	}
	return itv_frontend_is_exiting(p_ifrontend);
}

static void itv_frontend_wakeup(itv_frontend_t *p_ifrontend)
{
	itv_frontend_priv_t *priv = p_ifrontend->priv;

//	DEBUG_CALLSTACK

	priv->wakeup = 1;
	wake_up_interruptible(&priv->wait_queue);
}

static void itv_frontend_add_event(itv_frontend_t *p_ifrontend, itv_frontend_status_e status)
{
	int wp;

	itv_frontend_priv_t *priv = p_ifrontend->priv;
	itv_frontend_events_t *events = &priv->events;
	itv_frontend_event_t *event;

//	DEBUG_CALLSTACK

	if(mutex_lock_interruptible(&events->mutex))
		return;

	wp = (events->eventw + 1) % MAX_FE_EVENT;

	if(wp == events->eventr) {
		events->overflow = 1;
		events->eventr = (events->eventr + 1) % MAX_FE_EVENT;
	}

	event = &events->events[events->eventw];

	memcpy(&event->params, &priv->params, sizeof(itv_frontend_parameters_t));

	events->eventw = wp;

	mutex_unlock(&events->mutex);

	event->status = status;

	wake_up_interruptible(&events->wait_queue);
}

static int itv_frontend_get_event(itv_frontend_t *p_ifrontend, itv_frontend_event_t *event, int flags)
{
	itv_frontend_priv_t *priv = p_ifrontend->priv;
	itv_frontend_events_t *events = &priv->events;

//	DEBUG_CALLSTACK

	if(events->overflow) {
		events->overflow = 0;
		return -EOVERFLOW;
	}

	if(events->eventw == events->eventr) {
		int ret;

		if(flags & O_NONBLOCK)
			return -EWOULDBLOCK;

		up(&priv->sem);

		ret = wait_event_interruptible(events->wait_queue, events->eventw != events->eventr);

		if(down_interruptible(&priv->sem))
			return -ERESTARTSYS;

		if(ret < 0)
			return ret;
	}

	if(mutex_lock_interruptible(&events->mutex))
		return -ERESTARTSYS;

	memcpy(event, &events->events[events->eventr], sizeof(itv_frontend_event_t));

	events->eventr = (events->eventr + 1) % MAX_FE_EVENT;

	mutex_unlock(&events->mutex);

	return 0;
}

static void itv_frontend_swzigzag_update_delay(itv_frontend_priv_t *p_priv, int locked)
{
	int q2;

//	DEBUG_CALLSTACK

	if(locked)
		(p_priv->quality) = (p_priv->quality * 220 + 36 * 256) / 256;
	else
		(p_priv->quality) = (p_priv->quality * 220 + 0) / 256;

	q2 = p_priv->quality - 128;
	q2 *= q2;

	p_priv->delay = p_priv->min_delay + q2 * HZ / (128 * 128);
}

static int itv_frontend_swzigzag_autotune(itv_frontend_t *p_ifrontend, int check_wrapped)
{
	int ready = 0;
	int autoinversion;	
	int original_inversion;
	unsigned int original_frequency;

	itv_frontend_priv_t *priv = p_ifrontend->priv;

//	DEBUG_CALLSTACK

	original_inversion = priv->params.inversion;
	original_frequency = priv->params.frequency;

	autoinversion = ((!(p_ifrontend->idemod.info.caps & FE_CAN_INVERSION_AUTO)) && 
		(priv->params.inversion == INVERSION_AUTO));

	while(!ready) {
		priv->lnb_drift = priv->auto_step * priv->step_size;

		if(priv->lnb_drift > priv->max_drift) {
			priv->auto_step = 0;
			priv->auto_sub_step = 0;
			priv->lnb_drift = 0;
		}

		switch(priv->auto_sub_step) {
			case 0:
				ready = 1;
				break;
			case 1:
				if(!autoinversion) break;
				priv->inversion = (priv->inversion == INVERSION_OFF) ? INVERSION_ON : INVERSION_OFF;
				ready = 1;
				break;
			case 2:
				if(priv->lnb_drift == 0) break;
				priv->lnb_drift = -priv->lnb_drift;
				ready = 1;
				break;
			case 3:
				if(priv->lnb_drift == 0) break;
				if(!autoinversion) break;
				priv->inversion = (priv->inversion == INVERSION_OFF) ? INVERSION_ON : INVERSION_OFF;
				priv->lnb_drift = -priv->lnb_drift;
				ready = 1;
				break;
			default:
				priv->auto_step++;
				priv->auto_sub_step = -1;
				break;
		}

		if(!ready) priv->auto_sub_step++;
	}

	if((priv->auto_step == priv->started_auto_step) && 
		(priv->auto_sub_step == 0) && check_wrapped) {
		return 1;
	}

/*	dprintk("drift:%i inversion:%i auto_step:%i " "auto_sub_step:%i started_auto_step:%i\n", 
		priv->lnb_drift, priv->inversion, priv->auto_step, priv->auto_sub_step, priv->started_auto_step);*/

	priv->params.frequency += priv->lnb_drift;
	if(autoinversion)
		priv->params.inversion = priv->inversion;
	if(p_ifrontend->idemod.set_frontend)
		p_ifrontend->idemod.set_frontend(p_ifrontend, &priv->params);

	priv->params.frequency = original_frequency;
	priv->params.inversion = original_inversion;

	priv->auto_sub_step++;
	
	return 0;
}

static void itv_frontend_swzigzag(itv_frontend_t *p_ifrontend)
{
	itv_frontend_priv_t *priv = p_ifrontend->priv;
	itv_frontend_status_e status = 0;
#ifdef FE_LOCK_TIME_CHECK
	static mtime_t start_time, elapsed_time;
#endif
	//DEBUG_CALLSTACK

	if(priv->state & FE_STATE_IDLE) {
		priv->delay = 3 * HZ;
		priv->quality = 0;
		return;
	}

	if(priv->tune_mode_flags & ITV_FE_TUNE_MODE_ONESHOT)
	{
		if(priv->state & FE_STATE_RETUNE)
		{
			if(p_ifrontend->idemod.set_frontend)
				p_ifrontend->idemod.set_frontend(p_ifrontend, &priv->params);
			
			priv->state = FE_STATE_TUNED;
		}
		priv->delay = 3 * HZ;
		priv->quality = 0;
		return;
	}

	if(priv->state & FE_STATE_RETUNE)
	{
		status = 0;
	}
	else
	{
		if(p_ifrontend->idemod.read_status)
			p_ifrontend->idemod.read_status(p_ifrontend, &status);

		if(status != priv->status)
		{
		#ifdef FE_LOCK_TIME_CHECK
			if(status & FE_HAS_LOCK)
			{
				elapsed_time = imdate() - start_time;
				printk("\n@@@@@@ tuner lock time %lld #######\n\n", elapsed_time);
			}
		#endif
			itv_frontend_add_event(p_ifrontend, status);
			priv->status = status;
		}
	}

	if((priv->state & FE_STATE_WAITFORLOCK) && (status & FE_HAS_LOCK)) {
		itv_frontend_swzigzag_update_delay(priv, status & FE_HAS_LOCK);
		priv->state = FE_STATE_TUNED;

		if((!(p_ifrontend->idemod.info.caps & FE_CAN_INVERSION_AUTO)) && 
			(priv->params.inversion == INVERSION_AUTO)) {
			priv->params.inversion = priv->inversion;
		}
		return;
	}

	if(priv->state & FE_STATE_TUNED)
	{
		itv_frontend_swzigzag_update_delay(priv, status & FE_HAS_LOCK);

		if(status & FE_HAS_LOCK)
		{
			return;
		}
		else {
			priv->state = FE_STATE_ZIGZAG_FAST;
			priv->started_auto_step = priv->auto_step;
			priv->check_wrapped = 0;
		}
	}

	if((priv->state & FE_STATE_LOSTLOCK) &&
	    (p_ifrontend->idemod.info.caps & FE_CAN_RECOVER) && (priv->max_drift == 0)) {
		itv_frontend_swzigzag_update_delay(priv, status & FE_HAS_LOCK);
		return;
	}

	if(priv->state & FE_STATE_DISEQC) {
		itv_frontend_swzigzag_update_delay(priv, status & FE_HAS_LOCK);
		return;
	}

	if(priv->state & FE_STATE_RETUNE) {
		priv->lnb_drift = 0;
		priv->auto_step = 0;
		priv->auto_sub_step = 0;
		priv->started_auto_step = 0;
		priv->check_wrapped = 0;
	}

#if 0
	if((priv->state & FE_STATE_SEARCHING_FAST) || (priv->state & FE_STATE_RETUNE)) {
		priv->delay = priv->min_delay;

		if(itv_frontend_swzigzag_autotune(p_ifrontend, priv->check_wrapped)) {
			priv->state = FE_STATE_SEARCHING_SLOW;
			priv->started_auto_step = priv->auto_step;
			return;
		}
		priv->check_wrapped = 1;

		if(priv->state & FE_STATE_RETUNE) {
			priv->state = FE_STATE_TUNING_FAST;
		}
	}
#else
	if(priv->state & FE_STATE_RETUNE)
	{
		priv->delay = priv->min_delay;
	#ifdef FE_LOCK_TIME_CHECK
		start_time = imdate();
	#endif
		p_ifrontend->idemod.set_frontend(p_ifrontend, &priv->params);

		if(priv->state & FE_STATE_RETUNE) {
			priv->state = FE_STATE_TUNING_FAST;
		}
	}
#endif

	if(priv->state & FE_STATE_SEARCHING_SLOW) {
		itv_frontend_swzigzag_update_delay(priv, status & FE_HAS_LOCK);

		itv_frontend_swzigzag_autotune(p_ifrontend, 0);
	}
}

static void itv_frontend_init(itv_frontend_t *p_ifrontend)
{
	DEBUG_CALLSTACK

	dprintk("initialising adapter %i frontend %i (%s)...\n", ((itv_adapter_t *)p_ifrontend->iadapter)->id, 
		p_ifrontend->id, p_ifrontend->idemod.info.name);

//20111206 koo : lgdt3305의 경우 s5h1411처럼 gpio가 나와있지 않아 ts_rst#이 그대로 xc5000c에 연결되어 있기 때문에 xc5000c firmware upload시 문제가 
//발생되어 다시 reset을 할 경우 demod까지 reset 되므로 xc500c 사용시 tuner init을 먼저 하도록 수정. 추후 firmware upload 문제 수정 시 원복하도록 함.
#if defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000C) || defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000C_MODULE)
	if(p_ifrontend->ituner.init) {
		if(p_ifrontend->idemod.i2c_gate_ctrl)
			p_ifrontend->idemod.i2c_gate_ctrl(p_ifrontend, 1);
		p_ifrontend->ituner.init(p_ifrontend);
		if(p_ifrontend->idemod.i2c_gate_ctrl)
			p_ifrontend->idemod.i2c_gate_ctrl(p_ifrontend, 0);
	}

	if(p_ifrontend->idemod.init)
		p_ifrontend->idemod.init(p_ifrontend);
#else
	if(p_ifrontend->idemod.init)
		p_ifrontend->idemod.init(p_ifrontend);
	
	if(p_ifrontend->ituner.init) {
		if(p_ifrontend->idemod.i2c_gate_ctrl)
			p_ifrontend->idemod.i2c_gate_ctrl(p_ifrontend, 1);
		p_ifrontend->ituner.init(p_ifrontend);
		if(p_ifrontend->idemod.i2c_gate_ctrl)
			p_ifrontend->idemod.i2c_gate_ctrl(p_ifrontend, 0);
	}	
#endif
}

static int itv_frontend_thread(void *data)
{
	unsigned long timeout;	
	
	itv_frontend_t *ifrontend = (itv_frontend_t *)data;
	itv_frontend_priv_t *priv = ifrontend->priv;
	itv_frontend_status_e status;
	itv_frontend_parameters_t *params;

#ifdef FE_POWER_SAVE_ENABLE	
	itv_platform_frontend_operations_t *fe_ops = ifrontend->fe_ops;
#endif

	DEBUG_CALLSTACK

	priv->check_wrapped = 0;
	priv->quality 		= 0;
	priv->delay 		= 3 * HZ;
	priv->status 		= 0;
	priv->wakeup 		= 0;
	priv->reinitialise 	= 0;

//20110523 koo : frontend driver open & thread create 시 demod & tuner init 전 atsc module pwdn off & reset send
#ifdef FE_POWER_SAVE_ENABLE	
	if(fe_ops->demod_reset)	fe_ops->demod_reset();
#endif

	itv_frontend_init(ifrontend);

	set_freezable();
	while(1)
	{
		up(&priv->sem);
restart:
		timeout = wait_event_interruptible_timeout(priv->wait_queue,
			itv_frontend_should_wakeup(ifrontend) || kthread_should_stop() || freezing(current),
			priv->delay);

		//if(kthread_should_stop() || itv_frontend_should_wakeup(ifrontend))
		if(kthread_should_stop() || itv_frontend_is_exiting(ifrontend))
		{
			/* got signal or quitting */
			break;
		}

		if(try_to_freeze())
			goto restart;

		if(down_interruptible(&priv->sem))
			break;

		if(priv->reinitialise)
		{
			itv_frontend_init(ifrontend);
			if(priv->tone != -1)
			{
				ifrontend->idemod.set_tone(ifrontend, priv->tone);
			}
			if(priv->voltage != -1)
			{
				ifrontend->idemod.set_voltage(ifrontend, priv->voltage);
			}
			priv->reinitialise = 0;
		}

		if(ifrontend->idemod.get_frontend_algo) 
		{
			if(ifrontend->idemod.get_frontend_algo(ifrontend) == FE_ALGO_HW)
			{
				params = NULL;
				if(priv->state & FE_STATE_RETUNE)
				{
					params = &priv->params;
					priv->state = FE_STATE_TUNED;
				}

				ifrontend->idemod.tune(ifrontend, params, priv->tune_mode_flags, &priv->delay, &status);
				if(status != priv->status)
				{
					itv_frontend_add_event(ifrontend, status);
					priv->status = status;
				}
			}
			else
				itv_frontend_swzigzag(ifrontend);
		}
		else
			itv_frontend_swzigzag(ifrontend);
	}

	if(itv_frontend_powerdown_on_sleep)
	{
//20110523 koo : frontend driver closd & thread destory 시 atsc module pwdn on
#ifdef FE_POWER_SAVE_ENABLE	
		if(fe_ops->demod_pwroff)		fe_ops->demod_pwroff(1);
#else		
		if(ifrontend->idemod.set_voltage)
			ifrontend->idemod.set_voltage(ifrontend, SEC_VOLTAGE_OFF);
		if(ifrontend->ituner.sleep)
		{
			if(ifrontend->idemod.i2c_gate_ctrl)
				ifrontend->idemod.i2c_gate_ctrl(ifrontend, 1);

			ifrontend->ituner.sleep(ifrontend);

			if(ifrontend->idemod.i2c_gate_ctrl)
				ifrontend->idemod.i2c_gate_ctrl(ifrontend, 0);
		}
		if(ifrontend->idemod.sleep){
			ifrontend->idemod.sleep(ifrontend);
	}
#endif
	}

	priv->thread = NULL;
	mb();

	itv_frontend_wakeup(ifrontend);

	return 0;
}

static void itv_frontend_stop(itv_frontend_t *p_ifrontend)
{
	itv_frontend_priv_t *priv = p_ifrontend->priv;
	
	DEBUG_CALLSTACK

	if(!priv->thread)
		return;

	kthread_stop(priv->thread);

	sema_init(&priv->sem, 1);
	
	priv->state = FE_STATE_IDLE;

	if(priv->thread)
		wprintk("thread %p won't exit\n", priv->thread);
}

static int itv_frontend_start(itv_frontend_t *p_ifrontend)
{
	int retval;

	itv_frontend_priv_t *priv = p_ifrontend->priv;
	struct task_struct *fe_thread;
	
	DEBUG_CALLSTACK

	if(priv->thread) {
		itv_frontend_stop(p_ifrontend);
	}

	if(signal_pending(current))
		return -EINTR;
	if(down_interruptible(&priv->sem))
		return -EINTR;

	priv->state = FE_STATE_IDLE;
	priv->thread = NULL;
	mb();

	fe_thread = kthread_run(itv_frontend_thread, p_ifrontend, "kitv-ad-%i-fe-%i", 
		((itv_adapter_t *)p_ifrontend->iadapter)->id, p_ifrontend->id);
	if(IS_ERR(fe_thread)) {
		retval = PTR_ERR(fe_thread);
		eprintk("failed to start kthread (%d)\n", retval);
		up(&priv->sem);
		return retval;
	}
	priv->thread = fe_thread;
	
	return 0;
}

static int itv_frontend_ioctl_properties(struct file *file, unsigned int cmd, void *parg)
{
	int i;
	int err = 0;

	itv_frontend_t *ifrontend = (itv_frontend_t *)file->private_data;
	itv_dtv_properties_t *tvps = NULL;
	itv_dtv_property_t *tvp = NULL;

	DEBUG_CALLSTACK

	if(cmd == ITV_FE_SET_PROPERTY) {
		tvps = (itv_dtv_properties_t __user *)parg;

		dprintk("properties.num = %d\n", tvps->num);
		dprintk("properties.props = %p\n", tvps->props);

		if((tvps->num == 0) || (tvps->num > ITV_DTV_IOCTL_MAX_MSGS))
			return -EINVAL;

		tvp = (itv_dtv_property_t *)kmalloc(tvps->num * sizeof(itv_dtv_property_t), GFP_KERNEL);
		if(!tvp) {
			err = -ENOMEM;
			goto out;
		}

		if(copy_from_user(tvp, tvps->props, tvps->num * sizeof(itv_dtv_property_t))) {
			err = -EFAULT;
			goto out;
		}

		for(i = 0; i < tvps->num; i++) {
			(tvp + i)->result = itv_frontend_dtv_property_process_set(ifrontend, tvp + i, file);
			err |= (tvp + i)->result;
		}

		if(ifrontend->dtv_property_cache.state == ITV_DTV_TUNE)
			dprintk("Property cache is full, tuning\n");

	}
	else if(cmd == ITV_FE_GET_PROPERTY) {
		tvps = (itv_dtv_properties_t __user *)parg;

		dprintk("properties.num = %d\n", tvps->num);
		dprintk("properties.props = %p\n", tvps->props);

		if((tvps->num == 0) || (tvps->num > ITV_DTV_IOCTL_MAX_MSGS))
			return -EINVAL;

		tvp = (itv_dtv_property_t *)kmalloc(tvps->num * sizeof(itv_dtv_property_t), GFP_KERNEL);
		if(!tvp) {
			err = -ENOMEM;
			goto out;
		}

		if(copy_from_user(tvp, tvps->props, tvps->num * sizeof(itv_dtv_property_t))) {
			err = -EFAULT;
			goto out;
		}

		for(i = 0; i < tvps->num; i++) {
			(tvp + i)->result = itv_frontend_dtv_property_process_get(ifrontend, tvp + i, file);
			err |= (tvp + i)->result;
		}

		if(copy_to_user(tvps->props, tvp, tvps->num * sizeof(itv_dtv_property_t))) {
			err = -EFAULT;
			goto out;
		}

	}
	else
		err = -EOPNOTSUPP;

out:
	kfree(tvp);
	return err;
}

static int itv_frontend_ioctl_legacy(struct file *file, unsigned int cmd, void *parg)
{
	int err = -EOPNOTSUPP;
	
	itv_frontend_t *ifrontend = (itv_frontend_t *)file->private_data;
	itv_frontend_priv_t *priv = ifrontend->priv;	

//	DEBUG_CALLSTACK

	switch(cmd) {
		case ITV_FE_GET_INFO:
			{
				itv_frontend_info_t *info = parg;

//				dprintk("ITV_FE_GET_INFO\n");
				
				memcpy(info, &ifrontend->idemod.info, sizeof(itv_frontend_info_t));
				itv_frontend_get_frequeny_limits(ifrontend, &info->frequency_min, &info->frequency_max);

				info->caps |= FE_CAN_INVERSION_AUTO;
				err = 0;
				break;
			}
		case ITV_FE_READ_STATUS:
			{
				itv_frontend_status_e *status = parg;

//				dprintk("ITV_FE_READ_STATUS\n");

				if(priv->state == FE_STATE_RETUNE) {
					err = 0;
					*status = 0;
					break;
				}
				if(ifrontend->idemod.read_status)
					err = ifrontend->idemod.read_status(ifrontend, status);				
			}
			break;
		case ITV_FE_READ_BER:
//			dprintk("ITV_FE_READ_BER\n");
			if(ifrontend->idemod.read_ber)
				err = ifrontend->idemod.read_ber(ifrontend, (__u32 *)parg);
			break;
		case ITV_FE_READ_SIGNAL_STRENGTH:
//			dprintk("ITV_FE_READ_SIGNAL_STRENGTH\n");
			if(ifrontend->idemod.read_signal_strength)
				err = ifrontend->idemod.read_signal_strength(ifrontend, (__u16 *)parg);
			break;
		case ITV_FE_READ_SNR:
//			dprintk("ITV_FE_READ_SNR\n");
			if(ifrontend->idemod.read_snr)
				err = ifrontend->idemod.read_snr(ifrontend, (__u16 *)parg);
			break;
		case ITV_FE_READ_UNCORRECTED_BLOCKS:
//			dprintk("ITV_FE_READ_UNCORRECTED_BLOCKS\n");
			if(ifrontend->idemod.read_ucblocks)
				err = ifrontend->idemod.read_ucblocks(ifrontend, (__u32 *)parg);
			break;
		case ITV_FE_DISEQC_RESET_OVERLOAD:
//			dprintk("ITV_FE_DISEQC_RESET_OVERLOAD\n");
			if(ifrontend->idemod.diseqc_reset_overload) {
				err = ifrontend->idemod.diseqc_reset_overload(ifrontend);
				priv->state = FE_STATE_DISEQC;
				priv->status = 0;
			}
			break;
		case ITV_FE_DISEQC_SEND_MASTER_CMD:
//			dprintk("ITV_FE_DISEQC_SEND_MASTER_CMD\n");
			if(ifrontend->idemod.diseqc_send_master_cmd) {
				err = ifrontend->idemod.diseqc_send_master_cmd(ifrontend, (itv_diseqc_master_cmd_t *)parg);
				priv->state = FE_STATE_DISEQC;
				priv->status = 0;
			}
			break;
		case ITV_FE_DISEQC_SEND_BURST:
//			dprintk("ITV_FE_DISEQC_SEND_BURST\n");
			if(ifrontend->idemod.diseqc_send_burst) {
				err = ifrontend->idemod.diseqc_send_burst(ifrontend, (itv_frontend_sec_mini_cmd_e)parg);
				priv->state = FE_STATE_DISEQC;
				priv->status = 0;
			}
			break;
		case ITV_FE_SET_TONE:
//			dprintk("ITV_FE_SET_TONE\n");
			if(ifrontend->idemod.set_tone) {
				err = ifrontend->idemod.set_tone(ifrontend, (itv_frontend_sec_tone_mode_e)parg);
				priv->tone = (itv_frontend_sec_tone_mode_e)parg;
				priv->state = FE_STATE_DISEQC;
				priv->status = 0;
			}
			break;
		case ITV_FE_SET_VOLTAGE:
//			dprintk("ITV_FE_SET_VOLTAGE\n");
			if(ifrontend->idemod.set_voltage) {
				err = ifrontend->idemod.set_voltage(ifrontend, (itv_frontend_sec_voltage_e)parg);
				priv->voltage = (itv_frontend_sec_voltage_e)parg;
				priv->state = FE_STATE_DISEQC;
				priv->status = 0;
			}
			break;
		case ITV_FE_DISHNETWORK_SEND_LEGACY_CMD:
//			dprintk("ITV_FE_DISHNETWORK_SEND_LEGACY_CMD\n");
			if(ifrontend->idemod.dishnetwork_send_legacy_command) {
				err = ifrontend->idemod.dishnetwork_send_legacy_command(ifrontend, (unsigned long)parg);
				priv->state = FE_STATE_DISEQC;
				priv->status = 0;
			}
			else if(ifrontend->idemod.set_voltage) {
				int i;
				unsigned char last = 1;
				unsigned long swcmd = ((unsigned long)parg) << 1;
				
				struct timeval nexttime;
				struct timeval tv[10];				
				
				dprintk("switch command: 0x%04lx\n", swcmd);
				do_gettimeofday(&nexttime);
				if(itv_frontend_debug)
					memcpy(&tv[0], &nexttime, sizeof(struct timeval));

				ifrontend->idemod.set_voltage(ifrontend, SEC_VOLTAGE_18);
				itv_frontend_sleep_until(&nexttime, 32000);

				for(i = 0; i < 9; i++) {
					if(itv_frontend_debug)
						do_gettimeofday(&tv[i + 1]);
					if((swcmd & 0x01) != last) {
						ifrontend->idemod.set_voltage(ifrontend, (last) ? SEC_VOLTAGE_13 : SEC_VOLTAGE_18);
						last = (last) ? 0 : 1;
					}
					swcmd = swcmd >> 1;
					if(i != 8)
						itv_frontend_sleep_until(&nexttime, 8000);
				}

				dprintk("switch delay (should be 32k followed by all 8k - %d\n", ((itv_adapter_t *)ifrontend->iadapter)->id);
				if(itv_frontend_debug) {
					for(i = 1; i < 10; i++)
						dprintk("%d: %d\n", i, itv_frontend_timeval_usec_diff(tv[i-1] , tv[i]));
				}
				err = 0;
				priv->state = FE_STATE_DISEQC;
				priv->status = 0;
			}
			break;
		case ITV_FE_DISEQC_RECV_SLAVE_REPLY:
//			dprintk("ITV_FE_DISEQC_RECV_SLAVE_REPLY\n");
			if(ifrontend->idemod.diseqc_recv_slave_reply)
				err = ifrontend->idemod.diseqc_recv_slave_reply(ifrontend, (itv_diseqc_slave_reply_t *)parg);
			break;
		case ITV_FE_ENABLE_HIGH_LNB_VOLTAGE:
//			dprintk("ITV_FE_ENABLE_HIGH_LNB_VOLTAGE\n");
			if(ifrontend->idemod.enable_high_lnb_voltage)
				err = ifrontend->idemod.enable_high_lnb_voltage(ifrontend, (long)parg);
			break;
		case ITV_FE_SET_FRONTEND:
			{
				itv_frontend_tune_settings_t fetunesettings;

//				dprintk("ITV_FE_SET_FRONTEND\n");

				if(ifrontend->dtv_property_cache.state == ITV_DTV_TUNE) {
					if(itv_frontend_check_parameters(ifrontend, &priv->params) < 0) {
						err = -EINVAL;
						break;
					}
				}
				else {
					if(itv_frontend_check_parameters(ifrontend, parg) < 0) {
						err = -EINVAL;
						break;
					}
					memcpy(&priv->params, parg, sizeof(itv_frontend_parameters_t));
					itv_frontend_dtv_property_cache_sync(ifrontend, &priv->params);
				}
				memset(&fetunesettings, 0, sizeof(itv_frontend_tune_settings_t));
				memcpy(&fetunesettings.params, parg, sizeof(itv_frontend_parameters_t));
				
				if(itv_frontend_force_auto_inversion) {
					priv->params.inversion = INVERSION_AUTO;
					fetunesettings.params.inversion = INVERSION_AUTO;
				}
				if(ifrontend->idemod.info.type == FE_OFDM) {
					if(priv->params.u.ofdm.hierarchy_information == HIERARCHY_NONE &&
						priv->params.u.ofdm.code_rate_LP == FEC_NONE)
						priv->params.u.ofdm.code_rate_LP = FEC_AUTO;
				}

				if(ifrontend->idemod.get_tune_settings && (ifrontend->idemod.get_tune_settings(ifrontend, &fetunesettings) == 0)) {
					priv->min_delay = (fetunesettings.min_delay_ms * HZ) / 1000;
					priv->max_drift = fetunesettings.max_drift;
					priv->step_size = fetunesettings.step_size;
				}
				else {
					switch(ifrontend->idemod.info.type) {
						case FE_QPSK:
							priv->min_delay = HZ / 20;
							priv->step_size = priv->params.u.qpsk.symbol_rate / 16000;
							priv->max_drift = priv->params.u.qpsk.symbol_rate / 2000;
							break;
						case FE_QAM:
							priv->min_delay = HZ / 20;
							priv->step_size = 0;
							priv->max_drift = 0;
							break;
						case FE_OFDM:
							priv->min_delay = HZ / 20;
							priv->step_size = ifrontend->idemod.info.frequency_stepsize * 2;
							priv->max_drift = (ifrontend->idemod.info.frequency_stepsize * 2) + 1;
							break;
						case FE_ATSC:
							priv->min_delay = HZ / 20;
							priv->step_size = 0;
							priv->max_drift = 0;
							break;
					}
				}
				if(itv_frontend_override_tune_delay > 0)
					priv->min_delay = (itv_frontend_override_tune_delay * HZ) / 1000;

				priv->state = FE_STATE_RETUNE;
				itv_frontend_wakeup(ifrontend);
				itv_frontend_add_event(ifrontend, 0);
				priv->status = 0;
				err = 0;
			}
			break;
			
		//20100413 koo : ITV_FE_SET_FRONTEND_ANALOG   
		case ITV_FE_SET_FRONTEND_ANALOG:
		{
			itv_frontend_parameters_t* param;

			param = (itv_frontend_parameters_t*)parg;

			if(ifrontend->idemod.set_frontend_analog)
			{
				err = ifrontend->idemod.set_frontend_analog(ifrontend, param);
			}
			else
			{
				err = -EINVAL;
			}
		}
			break;
			
		case ITV_FE_GET_EVENT:
//			dprintk("ITV_FE_GET_EVENT\n");
			err = itv_frontend_get_event(ifrontend, parg, file->f_flags);
			break;
		case ITV_FE_GET_FRONTEND:
//			dprintk("ITV_FE_GET_FRONTEND\n");
			if(ifrontend->idemod.get_frontend) {
				memcpy(parg, &priv->params, sizeof(itv_frontend_parameters_t));
				err = ifrontend->idemod.get_frontend(ifrontend, (itv_frontend_parameters_t *)parg);
			}
			break;
		case ITV_FE_SET_FRONTEND_TUNE_MODE:
//			dprintk("ITV_FE_SET_FRONTEND_TUNE_MODE\n");
			priv->tune_mode_flags = (unsigned long)parg;
			err = 0;
			break;
	}

	return err;
}

static int itv_frontend_do_ioctl(struct file *file, unsigned int cmd, void *parg)
{
	int err = -EOPNOTSUPP;

	itv_frontend_t *ifrontend = (itv_frontend_t *)file->private_data;
	itv_frontend_priv_t *priv;

//	DEBUG_CALLSTACK

	if(!ifrontend || ifrontend->exit)
		return -ENODEV;

	priv = ifrontend->priv;

	if((file->f_flags & O_ACCMODE) == O_RDONLY && 
		(_IOC_DIR(cmd) != _IOC_READ || cmd == ITV_FE_GET_EVENT || 
		cmd == ITV_FE_DISEQC_RECV_SLAVE_REPLY))
		return -EPERM;

	if(down_interruptible (&priv->sem))
		return -ERESTARTSYS;

	if((cmd == ITV_FE_SET_PROPERTY) || (cmd == ITV_FE_GET_PROPERTY))
		err = itv_frontend_ioctl_properties(file, cmd, parg);
	else {
		ifrontend->dtv_property_cache.state = ITV_DTV_UNDEFINED;
		err = itv_frontend_ioctl_legacy(file, cmd, parg);
	}

	up(&priv->sem);

	return err;
}

static int itv_frontend_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{	
//	DEBUG_CALLSTACK

	return itv_usercopy(file, cmd, arg, itv_frontend_do_ioctl);
}

static unsigned int itv_frontend_poll(struct file *file, poll_table *wait)
{
	itv_frontend_t *ifrontend = (itv_frontend_t *)file->private_data;
	itv_frontend_priv_t *priv;
	
//	DEBUG_CALLSTACK

	if(!ifrontend)
		return -ENODEV;

	priv = ifrontend->priv;

	poll_wait(file, &priv->events.wait_queue, wait);

	if(priv->events.eventw != priv->events.eventr)
		return (POLLIN | POLLRDNORM | POLLPRI);

	return 0;
}

static int itv_frontend_open(struct inode *inode, struct file *file)
{
	int retval = 0;
	
	itv_frontend_t *ifrontend = (itv_frontend_t *)file->private_data;
	itv_frontend_priv_t *priv;
	
	DEBUG_CALLSTACK

	if(!ifrontend)
		return -ENODEV;

	priv = ifrontend->priv;

	if(ifrontend->users == 0 && ifrontend->idemod.ts_bus_ctrl) {
		if((retval = ifrontend->idemod.ts_bus_ctrl(ifrontend, 1)) < 0)
			return retval;
	}

	if((file->f_flags & O_ACCMODE) != O_RDONLY) {
		priv->tune_mode_flags &= ~ITV_FE_TUNE_MODE_ONESHOT;
		priv->tone = -1;
		priv->voltage = -1;
		
		retval = itv_frontend_start(ifrontend);
		if(retval) return retval;

		priv->events.eventr = priv->events.eventw = 0;
	}

	return itv_device_open(ifrontend);
}

static int itv_frontend_release(struct inode *inode, struct file *file)
{
	itv_frontend_t *ifrontend = (itv_frontend_t *)file->private_data;
	itv_frontend_priv_t *priv;
	
	DEBUG_CALLSTACK

	if(!ifrontend)
		return -ENODEV;

	priv = ifrontend->priv;

	if((file->f_flags & O_ACCMODE) != O_RDONLY)
		priv->release_jiffies = jiffies;

	if(ifrontend->users == 1) {
		if(ifrontend->idemod.ts_bus_ctrl)
			ifrontend->idemod.ts_bus_ctrl(ifrontend, 0);
	}

	itv_frontend_stop(ifrontend);
	
	return itv_device_release(ifrontend, file);
}

struct file_operations ifrontend_fops = {
	.owner 			= THIS_MODULE, 
	.poll 			= itv_frontend_poll, 
	.unlocked_ioctl	= itv_frontend_ioctl, 
	.open 			= itv_frontend_open, 
	.release 			= itv_frontend_release
};

itv_frontend_t *itv_frontend_create(itv_object_t *p_this)
{
	bool b_need_tuner;
	char **sz_demod_name = NULL;
	char **sz_tuner_name = NULL;

	itv_adapter_t *iadapter = (itv_adapter_t *)p_this;
	itv_frontend_t *ifrontend;
	itv_frontend_priv_t *priv;

	DEBUG_CALLSTACK

	if((ifrontend = (itv_frontend_t *)itv_device_create(ITV_OBJECT(iadapter), sizeof(*ifrontend), "ifrontend")) == NULL) {
		eprintk("itv_device_create() is failed\n");
		return NULL;
	}

	if((ifrontend->priv = kzalloc(sizeof(*priv), GFP_KERNEL)) == NULL) {
		eprintk("out of memory\n");
		itv_device_destroy(ifrontend);
		return NULL;
	}
	priv = ifrontend->priv;

	ifrontend->tsif_mode = iadapter->tsif_mode;

	// get platform frontend operations
	ifrontend->fe_ops = itv_platform_get_frontend_operations();
	if(!ifrontend->fe_ops) {
		eprintk("itv_platform_get_frontend_operations() is failed\n");
		itv_device_destroy(ifrontend);
		return NULL;
	}

	itv_object_set_destructor(ifrontend, itv_frontend_destructor);

	sema_init(&priv->sem, 1);
	init_waitqueue_head(&priv->wait_queue);
	init_waitqueue_head(&priv->events.wait_queue);
	mutex_init(&priv->events.mutex);

	priv->inversion = INVERSION_OFF;

	b_need_tuner = false;
	switch(iadapter->std_type)
	{
		case ITV_STD_TYPE_ATSC:
			{
				int i;
				
				sz_demod_name = itv_atsc_demod_name;
				for(i=0; ; i++) {
					if(sz_demod_name[i] == NULL)
					{
						eprintk("demod name reached end of list..\n");
						eprintk("idemod itv_module_need() is failed\n");
						itv_device_destroy(ifrontend);
						return NULL;
					}

					if((priv->ituner = itv_module_need(ITV_OBJECT(ifrontend), "idemod", sz_demod_name[i])) != NULL)
					{
						dprintk("demod found : %s\n", sz_demod_name[i]);
						break;
					}
				}
				
			// tuner
			b_need_tuner = true;
			sz_tuner_name = itv_atsc_tuner_name;
			}
			break;
		default:
			eprintk("Can not support standard type : 0x%x\n", iadapter->std_type);
			itv_device_destroy(ifrontend);
			return NULL;
	}

	if(b_need_tuner)
	{
		int i;
		for(i=0; ; i++)
		{
			if(sz_tuner_name[i] == NULL)
			{
				eprintk("tuner name reached end of list..\n");
				eprintk("ituner itv_module_need() is failed\n");
				itv_device_destroy(ifrontend);
				return NULL;
			}
			
			if((priv->ituner = itv_module_need(ITV_OBJECT(ifrontend), "ituner", sz_tuner_name[i])) != NULL)
			{
				dprintk("tuner found : %s\n", sz_tuner_name[i]);
				break;
			}
		}
	}
	
	ifrontend->psz_object_name = kstrdup("ifrontend", GFP_KERNEL);

	if(itv_device_register(ifrontend, ITV_DEV_TYPE_FRONTEND, 0, &ifrontend_fops) != 0) {
		eprintk("itv_device_register() is failed\n");
		itv_device_destroy(ifrontend);
		return NULL;
	}

//20110523 koo : booting 시 module create 완료 후 atsc module pwroff
#ifdef FE_POWER_SAVE_ENABLE	
	if(ifrontend->fe_ops->demod_pwroff)		ifrontend->fe_ops->demod_pwroff(0);
#endif

	dprintk("iTV frontend device create and register success\n");

	return ifrontend;
}
EXPORT_SYMBOL(itv_frontend_create);

void itv_frontend_destroy(itv_frontend_t *p_ifrontend)
{
	DEBUG_CALLSTACK
		
	if(!p_ifrontend) return;

	itv_device_destroy(p_ifrontend);

	dprintk("iTV frontend device unregister and destroy success\n");
}
EXPORT_SYMBOL(itv_frontend_destroy);
