#include <i-tv/itv_common.h>
#include <i-tv/itv_ringbuffer.h>
#include <i-tv/itv_platform.h>
#include <i-tv/itv_stream.h>
#include <i-tv/itv_demux.h>
#include <i-tv/itv_frontend.h>
#include <i-tv/itv_adapter.h>

static int itv_direct_debug;
static int itv_direct_debug_callstack;
static int itv_direct_std_type = ITV_STD_TYPE_ATSC;
static int itv_direct_if_type = ITV_IF_TYPE_TS_PARALLEL;
static int itv_direct_tsif_mode = ITV_TSIF_MODE_PARALLEL;

module_param_named(debug, itv_direct_debug, int, 0644);
module_param_named(debug_cs, itv_direct_debug_callstack, int, 0644);
module_param_named(std_type, itv_direct_std_type, int, 0644);
module_param_named(if_type, itv_direct_if_type, int, 0644);
module_param_named(tsif_mode, itv_direct_tsif_mode, int, 0644);

MODULE_PARM_DESC(debug, "Turn on/off direct debugging (default:off).");
MODULE_PARM_DESC(debug_cs, "Turn on/off direct callstack debugging (default:off).");

#ifdef dprintk
#undef dprintk
#endif

#define dprintk(args...) \
	do { if(itv_direct_debug) { printk("[%s] ", __func__); printk(args); } } while (0)

#define DEBUG_CALLSTACK		\
	do { if(itv_direct_debug_callstack) { printk("[Call] ----> %s()\n", __func__); } } while (0);

struct itv_adapter_priv_t {
	int fe_synced;

	// hook functions
	int (*read_fe_status)(itv_frontend_t *, itv_frontend_status_e *);
	int (*set_frontend)(itv_frontend_t *, itv_frontend_parameters_t *);
	int (*set_frontend_analog)(itv_frontend_t *, itv_frontend_parameters_t *);
};
		
itv_adapter_t *p_idirect = NULL;

static void itv_direct_destructor(itv_object_t *p_this)
{
	itv_adapter_t *iadapter = (itv_adapter_t *)p_this;
	itv_adapter_priv_t *priv = iadapter->priv;

	DEBUG_CALLSTACK

	if(!priv)
		return;

	kfree(priv);
}

static int itv_direct_read_fe_status(itv_frontend_t *p_ifrontend, itv_frontend_status_e *status)
{
	int ret;
	int synced;

	itv_adapter_t *idirect;
	itv_adapter_priv_t *priv;
	itv_demux_t *idemux;

//	DEBUG_CALLSTACK

	idirect = (itv_adapter_t *)itv_object_find(p_ifrontend, ITV_OBJECT_ADAPTER, FIND_PARENT);
	priv = idirect->priv;

	if(p_ifrontend->idemod.read_status)
		ret = priv->read_fe_status(p_ifrontend, status);
	else
		ret = -EINVAL;

	if(!ret)
	{
		synced = (*status & FE_HAS_LOCK);

		if(synced != priv->fe_synced)
		{
			dprintk(" synced %d\n", synced);
			
			priv->fe_synced = synced;
			idemux = (itv_demux_t *)idirect->idemux;
			if(synced)
			{
				if(p_ifrontend->tune_digital)
					idemux->demux_start(idemux);
			}
			else
				idemux->demux_stop(idemux);
		}
	}
	itv_object_release(idirect);
	
	return ret;
}

static int itv_direct_set_frontend(itv_frontend_t *p_ifrontend, itv_frontend_parameters_t *params)
{
	itv_adapter_t *idirect;
	itv_adapter_priv_t *priv;
	itv_demux_t *idemux;

//	DEBUG_CALLSTACK

	idirect = (itv_adapter_t *)itv_object_find(p_ifrontend, ITV_OBJECT_ADAPTER, FIND_PARENT);
	priv = idirect->priv;

	if(priv->fe_synced)
	{
		priv->fe_synced = 0;
		idemux = (itv_demux_t *)idirect->idemux;
		idemux->demux_stop(idemux);
	}

	itv_object_release(idirect);

	return priv->set_frontend(p_ifrontend, params);
}

static int itv_direct_set_frontend_analog(itv_frontend_t *p_ifrontend, itv_frontend_parameters_t *params)
{
	itv_adapter_t *idirect;
	itv_adapter_priv_t *priv;
	itv_demux_t *idemux;

//	DEBUG_CALLSTACK

	idirect = (itv_adapter_t *)itv_object_find(p_ifrontend, ITV_OBJECT_ADAPTER, FIND_PARENT);
	priv = idirect->priv;

	if(priv->fe_synced)
	{
		priv->fe_synced = 0;
		idemux = (itv_demux_t *)idirect->idemux;
		idemux->demux_stop(idemux);
	}

	itv_object_release(idirect);

	return priv->set_frontend_analog(p_ifrontend, params);
}


static int __init itv_direct_init(void)
{
	int retval;

	itv_adapter_t *iadapter;
	itv_adapter_priv_t *priv;
	itv_frontend_t *ifrontend;

	DEBUG_CALLSTACK

	if((iadapter = itv_adapter_create("idirect")) == NULL) {
		eprintk("itv_adapter_create() is failed\n");
		retval = -1;
		goto out;
	}

	if((iadapter->priv = kzalloc(sizeof(*priv), GFP_KERNEL)) == NULL) {
		eprintk("out of memory\n");
		goto error1;
	}
	priv = iadapter->priv;

	priv->fe_synced = 0;

	itv_object_set_destructor(iadapter, itv_direct_destructor);

	if((retval = itv_adapter_register(iadapter, THIS_MODULE, NULL)) != 0) {
		eprintk("itv_adapter_register() is failed\n");
		retval = -1;
		goto error1;
	}

	iadapter->std_type 	= (itv_std_type_e)itv_direct_std_type;
	iadapter->if_type 	= (itv_if_type_e)itv_direct_if_type;
	iadapter->tsif_mode = (itv_tsif_mode_e)itv_direct_tsif_mode;

	// demux
	if((iadapter->idemux = (itv_object_t *)itv_demux_create(ITV_OBJECT(iadapter))) == NULL) {
		eprintk("itv_demux_create() is failed\n");
		retval = -1;
		goto error2;
	}

	// stream
	if((iadapter->istream = (itv_object_t *)itv_stream_create(ITV_OBJECT(iadapter))) == NULL) {
		eprintk("itv_stream_create() is failed\n");
		retval = -1;
		goto error2;
	}

	// frontend
	if((iadapter->ifrontend = (itv_object_t *)itv_frontend_create(ITV_OBJECT(iadapter))) == NULL) {
		eprintk("itv_frontend_create() is failed\n");
		retval = -1;
		goto error2;
	}

	// hook functions
	ifrontend = (itv_frontend_t *)iadapter->ifrontend;
	if(ifrontend->idemod.read_status) {
		priv->read_fe_status = ifrontend->idemod.read_status;
		ifrontend->idemod.read_status = itv_direct_read_fe_status;
	}

	if(ifrontend->idemod.set_frontend) {
		priv->set_frontend = ifrontend->idemod.set_frontend;
		ifrontend->idemod.set_frontend = itv_direct_set_frontend;
	}

	if(ifrontend->idemod.set_frontend_analog) {
		priv->set_frontend_analog = ifrontend->idemod.set_frontend_analog;
		ifrontend->idemod.set_frontend_analog = itv_direct_set_frontend_analog;
	}

	p_idirect = iadapter;

	dprintk("iTV direct init success\n");

out:
	return retval;

error1:
	itv_adapter_destroy(iadapter);
	goto out;

error2:
	if(iadapter->ifrontend)
		itv_frontend_destroy((itv_frontend_t *)iadapter->ifrontend);

	if(iadapter->istream)
		itv_stream_destroy((itv_stream_t *)iadapter->istream);

	if(iadapter->idemux)
		itv_demux_destroy((itv_demux_t *)iadapter->idemux);

	itv_adapter_unregister(iadapter);
	goto error1;
}

static void __exit itv_direct_exit(void)
{
	itv_adapter_t *iadapter = p_idirect;

	DEBUG_CALLSTACK

	if(!iadapter) return;

	if(iadapter->ifrontend)
		itv_frontend_destroy((itv_frontend_t *)iadapter->ifrontend);

	if(iadapter->istream)
		itv_stream_destroy((itv_stream_t *)iadapter->istream);

	if(iadapter->idemux)
		itv_demux_destroy((itv_demux_t *)iadapter->idemux);
	
	itv_adapter_unregister(iadapter);
	itv_adapter_destroy(iadapter);
	p_idirect = NULL;

	dprintk("iTV direct eixt success\n");
}

module_init(itv_direct_init);
module_exit(itv_direct_exit);

MODULE_AUTHOR("JP");
MODULE_LICENSE("GPL");
