#include <i-tv/itv_common.h>
#include <i-tv/itv_frontend.h>

#include "ituner_i2c.h"
//20110824 koo : lgdt3305 demod & xc500c tuner porting
#if defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000C) || defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000C_MODULE)
#include "ixc5000c_fw.h"
#else
#include "ixc5000_fw.h"
#endif
#include "ixc5000.h"

static int itv_xc5000_module_debug;
static int itv_xc5000_module_debug_callstack;
static int itv_xc5000_module_load_fw_on_attach;

module_param_named(debug, itv_xc5000_module_debug, int, 0644);
module_param_named(debug_cs, itv_xc5000_module_debug_callstack, int, 0644);
module_param_named(init_fw, itv_xc5000_module_load_fw_on_attach, int, 0644);

MODULE_PARM_DESC(debug, "Turn on/off xc5000 debugging (default:off).");
MODULE_PARM_DESC(debug_cs, "Turn on/off xc5000 callstack debugging (default:off).");
MODULE_PARM_DESC(init_fw, "Load firmware during driver initialization.");

#ifdef dprintk
#undef dprintk
#endif

#define dprintk(args...) \
	do { if(itv_xc5000_module_debug) { printk("[%s] ", __func__); printk(args); } } while (0)

#define DEBUG_CALLSTACK		\
	do { if(itv_xc5000_module_debug_callstack) { printk("[Call] ----> %s()\n", __func__); } } while (0);

static itv_xc5000_tv_standard_t XC5000_Standard[MAX_TV_STANDARD] = {
	{"M/N-NTSC/PAL-BTSC", 0x0400, 0x8020},
	{"M/N-NTSC/PAL-A2",   0x0600, 0x8020},
	{"M/N-NTSC/PAL-EIAJ", 0x0440, 0x8020},
	{"M/N-NTSC/PAL-Mono", 0x0478, 0x8020},
	{"B/G-PAL-A2",        0x0A00, 0x8049},
	{"B/G-PAL-NICAM",     0x0C04, 0x8049},
	{"B/G-PAL-MONO",      0x0878, 0x8059},
	{"I-PAL-NICAM",       0x1080, 0x8009},
	{"I-PAL-NICAM-MONO",  0x0E78, 0x8009},
	{"D/K-PAL-A2",        0x1600, 0x8009},
	{"D/K-PAL-NICAM",     0x0E80, 0x8009},
	{"D/K-PAL-MONO",      0x1478, 0x8009},
	{"D/K-SECAM-A2 DK1",  0x1200, 0x8009},
	{"D/K-SECAM-A2 L/DK3", 0x0E00, 0x8009},
	{"D/K-SECAM-A2 MONO", 0x1478, 0x8009},
	{"L-SECAM-NICAM",     0x8E82, 0x0009},
	{"L'-SECAM-NICAM",    0x8E82, 0x4009},
	{"DTV6",              0x00C0, 0x8002},
	{"DTV8",              0x00C0, 0x800B},
	{"DTV7/8",            0x00C0, 0x801B},
	{"DTV7",              0x00C0, 0x8007},
	{"FM Radio-INPUT2",   0x9802, 0x9002},
	{"FM Radio-INPUT1",   0x0208, 0x9002}
};

struct itv_tuner_priv_t {
	itv_tuner_i2c_props_t i2c_props;
	
	unsigned int if_khz, prev_if_khz;
	unsigned int freq_hz;
	unsigned int bandwidth;
	unsigned char video_standard, prev_video_standard;
	unsigned char rf_mode, prev_rf_mode;
};

static int itv_xc5000_read_reg(itv_tuner_priv_t *p_priv, unsigned short reg_addr, unsigned short *i2c_data);

static int itv_xc5000_get_ADC_Envelope(itv_tuner_priv_t *p_priv, unsigned short *adc_envelope)
{
//	DEBUG_CALLSTACK
		
	return itv_xc5000_read_reg(p_priv, XREG_ADC_ENV, adc_envelope);
}

static int itv_xc5000_get_frequency_error(itv_tuner_priv_t *p_priv, unsigned int *freq_error_hz)
{
	int result;
	unsigned int tmp;
	unsigned short regData;	

//	DEBUG_CALLSTACK

	result = itv_xc5000_read_reg(p_priv, XREG_FREQ_ERROR, &regData);
	if(result)
		return result;

	tmp = (unsigned int)regData;
	(*freq_error_hz) = (tmp * 15625) / 1000;
	return result;
}

static int itv_xc5000_get_lock_status(itv_tuner_priv_t *p_priv, unsigned short *lock_status)
{
//	DEBUG_CALLSTACK

	return itv_xc5000_read_reg(p_priv, XREG_LOCK, lock_status);
}

static int itv_xc5000_get_version(itv_tuner_priv_t *p_priv, 
	unsigned char *hw_majorversion, unsigned char *hw_minorversion, 
	unsigned char *fw_majorversion, unsigned char *fw_minorversion)
{	
	int result;
	unsigned short data;

//	DEBUG_CALLSTACK

	result = itv_xc5000_read_reg(p_priv, XREG_VERSION, &data);
	if(result)
		return result;

	(*hw_majorversion) = (data >> 12) & 0x0F;
	(*hw_minorversion) = (data >>  8) & 0x0F;
	(*fw_majorversion) = (data >>  4) & 0x0F;
	(*fw_minorversion) = data & 0x0F;

	return 0;
}

static int itv_xc5000_get_hsync_freq(itv_tuner_priv_t *p_priv, unsigned int *hsync_freq_hz)
{	
	int result;
	unsigned short regData;

//	DEBUG_CALLSTACK

	result = itv_xc5000_read_reg(p_priv, XREG_HSYNC_FREQ, &regData);
	if(result)
		return result;

	(*hsync_freq_hz) = ((regData & 0x0fff) * 763)/100;
	return result;
}

static int itv_xc5000_get_frame_lines(itv_tuner_priv_t *p_priv, unsigned short *frame_lines)
{
//	DEBUG_CALLSTACK
		
	return itv_xc5000_read_reg(p_priv, XREG_FRAME_LINES, frame_lines);
}

static int itv_xc5000_get_quality(itv_tuner_priv_t *p_priv, unsigned short *quality)
{
//	DEBUG_CALLSTACK
		
	return itv_xc5000_read_reg(p_priv, XREG_QUALITY, quality);
}

static void itv_xc5000_debug_dump(itv_tuner_priv_t *p_priv)
{
	unsigned short adc_envelope;
	unsigned int freq_error_hz = 0;
	unsigned short lock_status;
	unsigned int hsync_freq_hz = 0;
	unsigned short frame_lines;
	unsigned short quality;
	unsigned char hw_majorversion = 0, hw_minorversion = 0;
	unsigned char fw_majorversion = 0, fw_minorversion = 0;

//	DEBUG_CALLSTACK

	msleep(100);

	itv_xc5000_get_ADC_Envelope(p_priv,  &adc_envelope);
	dprintk("*** ADC envelope (0-1023) = %d\n", adc_envelope);

	itv_xc5000_get_frequency_error(p_priv, &freq_error_hz);
	dprintk("*** Frequency error = %d Hz\n", freq_error_hz);

	itv_xc5000_get_lock_status(p_priv,  &lock_status);
	dprintk("*** Lock status (0-Wait, 1-Locked, 2-No-signal) = %d\n", lock_status);

	itv_xc5000_get_version(p_priv,  &hw_majorversion, &hw_minorversion, &fw_majorversion, &fw_minorversion);
	dprintk("*** HW: V%02x.%02x, FW: V%02x.%02x\n", hw_majorversion, hw_minorversion, fw_majorversion, fw_minorversion);

	itv_xc5000_get_hsync_freq(p_priv,  &hsync_freq_hz);
	dprintk("*** Horizontal sync frequency = %d Hz\n", hsync_freq_hz);

	itv_xc5000_get_frame_lines(p_priv,  &frame_lines);
	dprintk("*** Frame lines = %d\n", frame_lines);

	itv_xc5000_get_quality(p_priv,  &quality);
	dprintk("*** Quality (0:<8dB, 7:>56dB) = %d\n", quality);
}

static int itv_xc5000_readreg(itv_tuner_priv_t *p_priv, unsigned short reg, unsigned short *val)
{
	unsigned char buf[2] = { reg >> 8, reg & 0xff };
	unsigned char bval[2] = { 0, 0 };
	
	struct i2c_msg msg[2] = {
		{ .addr = p_priv->i2c_props.addr,
			.flags = 0, .buf = &buf[0], .len = 2 },
		{ .addr = p_priv->i2c_props.addr,
			.flags = I2C_M_RD, .buf = &bval[0], .len = 2 },
	};

//	DEBUG_CALLSTACK

	if(i2c_transfer(p_priv->i2c_props.i2c, msg, 2) <= 0) {
		wprintk("xc5000: I2C read failed\n");
		return XC_RESULT_I2C_READ_FAILURE;
	}
	*val = (bval[0] << 8) | bval[1];
	return XC_RESULT_SUCCESS;
}

static int itv_xc5000_readregs(itv_tuner_priv_t *p_priv, unsigned char *buf, unsigned char len)
{
	struct i2c_msg msg = { .addr = p_priv->i2c_props.addr,
		.flags = I2C_M_RD, .buf = buf, .len = len };

//	DEBUG_CALLSTACK

	if (i2c_transfer(p_priv->i2c_props.i2c, &msg, 1) != 1) {
		eprintk("xc5000 I2C read failed (len=%i)\n", (int)len);
		return XC_RESULT_I2C_WRITE_FAILURE;
	}
	return XC_RESULT_SUCCESS;
}

static int itv_xc5000_writeregs(itv_tuner_priv_t *p_priv, unsigned char *buf, unsigned char len)
{
	struct i2c_msg msg = { .addr = p_priv->i2c_props.addr,
		.flags = 0, .buf = buf, .len = len };

//	DEBUG_CALLSTACK

	if(i2c_transfer(p_priv->i2c_props.i2c, &msg, 1) != 1) {
		eprintk("xc5000: I2C write failed (len=%i)\n", (int)len);
		return XC_RESULT_I2C_WRITE_FAILURE;
	}
	return XC_RESULT_SUCCESS;
}

static int itv_xc5000_read_reg(itv_tuner_priv_t *p_priv, unsigned short reg_addr, unsigned short *i2c_data)
{	
	int result;
	unsigned char buf[2];

//	DEBUG_CALLSTACK

	buf[0] = (reg_addr >> 8) & 0xFF;
	buf[1] = reg_addr & 0xFF;
	result = itv_xc5000_writeregs(p_priv, buf, 2);
	if(result != XC_RESULT_SUCCESS)
		return result;

	result = itv_xc5000_readregs(p_priv, buf, 2);
	if(result != XC_RESULT_SUCCESS)
		return result;

	*i2c_data = buf[0] * 256 + buf[1];
	return result;
}

static int itv_xc5000_write_reg(itv_tuner_priv_t *p_priv, unsigned short reg_addr, unsigned short i2c_data)
{
	int result;
	int WatchDogTimer = 5;
	unsigned char buf[4];

//	DEBUG_CALLSTACK

	buf[0] = (reg_addr >> 8) & 0xFF;
	buf[1] = reg_addr & 0xFF;
	buf[2] = (i2c_data >> 8) & 0xFF;
	buf[3] = i2c_data & 0xFF;
	result = itv_xc5000_writeregs(p_priv, buf, 4);
	if(result == XC_RESULT_SUCCESS) {
		/* wait for busy flag to clear */
		while((WatchDogTimer > 0) && (result == XC_RESULT_SUCCESS)) {
			buf[0] = 0;
			buf[1] = XREG_BUSY;

			result = itv_xc5000_writeregs(p_priv, buf, 2);
			if(result == XC_RESULT_SUCCESS) {
				result = itv_xc5000_readregs(p_priv, buf, 2);
				if(result == XC_RESULT_SUCCESS) {
					if((buf[0] == 0) && (buf[1] == 0)) {
						/* busy flag cleared */
						break;
					}
					else {
						msleep(100); /* wait 5 ms */
						WatchDogTimer--;
					}
				}
			}
		}
	}
	if(WatchDogTimer < 0)
		result = XC_RESULT_I2C_WRITE_FAILURE;

	return result;
}

static int itv_xc5000_reset(itv_frontend_t *p_ifrontend)
{
	int ret;
	
//	DEBUG_CALLSTACK

//20110824 koo : lgdt3305 demod & xc500c tuner porting
#if defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000C) || defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000C_MODULE)
	itv_platform_frontend_operations_t *fe_ops = p_ifrontend->fe_ops;

	//20111206 koo : lgdt3305의 경우 s5h1411처럼 gpio가 나와있지 않아 ts_rst#이 그대로 xc5000c에 연결되어 있음.
	fe_ops->demod_reset();

	return XC_RESULT_SUCCESS;
#endif

	if(p_ifrontend->idemod.reset_tuner) {
		ret = p_ifrontend->idemod.reset_tuner(p_ifrontend, ITV_FRONTEND_COMPONENT_TUNER, ITV_XC5000_TUNER_RESET, 0);
		if(ret)
			eprintk("xc5000: reset failed\n");
	}
	else {
		eprintk("xc5000: no tuner reset callback function, fatal\n");
		ret = XC_RESULT_RESET_FAILURE;
	}

	return ret;
}

static int itv_xc5000_is_firmware_loaded(itv_frontend_t *p_ifrontend)
{
	int ret;
	unsigned short id;

	itv_tuner_priv_t *priv = p_ifrontend->ituner.priv;

//	DEBUG_CALLSTACK

	ret = itv_xc5000_readreg(priv, XREG_PRODUCT_ID, &id);
	if(ret == XC_RESULT_SUCCESS) {
		if(id == XC_PRODUCT_ID_FW_NOT_LOADED)
			ret = XC_RESULT_RESET_FAILURE;
		else
			ret = XC_RESULT_SUCCESS;
	}
//	dprintk("returns %s id = 0x%x\n", ret == XC_RESULT_SUCCESS ? "True" : "False", id);
	
	return ret;
}

static int itv_xc5000_load_i2c_sequence(itv_frontend_t *p_ifrontend, const unsigned char *i2c_sequence)
{
	int i, nbytes_to_send, result;
	unsigned int len, pos, index;
	unsigned char buf[XC_MAX_I2C_WRITE_LENGTH];

	itv_tuner_priv_t *priv = p_ifrontend->ituner.priv;

//	DEBUG_CALLSTACK

	index = 0;
	while((i2c_sequence[index] != 0xFF) || (i2c_sequence[index + 1] != 0xFF)) {
		len = i2c_sequence[index] * 256 + i2c_sequence[index + 1];
		if(len == 0x0000) {
			/* RESET command */
			result = itv_xc5000_reset(p_ifrontend);
			index += 2;
			if(result != XC_RESULT_SUCCESS)
				return result;
		}
		else if(len & 0x8000) {
			/* WAIT command */
			msleep(len & 0x7FFF);
			index += 2;
		}
		else {
			/* Send i2c data whilst ensuring individual transactions
			 * do not exceed XC_MAX_I2C_WRITE_LENGTH bytes.
			 */
			index += 2;
			buf[0] = i2c_sequence[index];
			buf[1] = i2c_sequence[index + 1];
			pos = 2;

			while(pos < len) {
				if((len - pos) > XC_MAX_I2C_WRITE_LENGTH - 2)
					nbytes_to_send = XC_MAX_I2C_WRITE_LENGTH;
				else
					nbytes_to_send = (len - pos + 2);
				
				for(i = 2; i < nbytes_to_send; i++) {
					buf[i] = i2c_sequence[index + pos + i - 2];
				}
				
				result = itv_xc5000_writeregs(priv, buf, nbytes_to_send);

				if(result != XC_RESULT_SUCCESS)
					return result;
				pos += nbytes_to_send - 2;
			}
			index += len;
		}
	}
	return XC_RESULT_SUCCESS;
}

static int itv_xc5000_fwupload(itv_frontend_t *p_ifrontend)
{
	int ret = 0;
	int fw_size;

	struct firmware *fw;

//	DEBUG_CALLSTACK

	dprintk("xc5000: waiting for firmware upload...\n");
	
	fw = kzalloc(sizeof(struct firmware), GFP_KERNEL);
	if(!fw) {
		eprintk("out of memory\n");
		ret = XC_RESULT_RESET_FAILURE;
		return ret;
	}

//20110824 koo : lgdt3305 demod & xc500c tuner porting
#if defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000C) || defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000C_MODULE)
	fw->size = sizeof(XC5000C_firmware_SEQUENCE);
	fw_size = ALIGN(fw->size, PAGE_SIZE);
	fw->data = vmalloc(fw_size);
	memcpy((unsigned char *)fw->data, (unsigned char *)XC5000C_firmware_SEQUENCE, fw->size);

	if(fw->size != ITV_XC5000C_DEFAULT_FIRMWARE_SIZE) {
		eprintk("xc5000: firmware incorrect size\n");
		ret = XC_RESULT_RESET_FAILURE;
	}
	else {
		dprintk("xc5000: firmware upload\n");
		ret = itv_xc5000_load_i2c_sequence(p_ifrontend, fw->data);

		//20111206 koo : xc5000c의 경우 firmware upload 중 가끔 이상동작이 발생되어 정상적인 firmware upload가 되지 않았을 때 다시 upload 시킴.
		if(ret != XC_RESULT_SUCCESS) {
			int i;
			for(i=1; i<5; i++) {
				if(ret != XC_RESULT_SUCCESS) {
					eprintk("xc5000: firmware upload again!!! ==> %d\n\n", i);
					ret = itv_xc5000_load_i2c_sequence(p_ifrontend, fw->data);
				} else {
					eprintk("xc5000: firmware upload complete!!! ==> %d\n\n", i);
					break;
				}
			}
		}
	}
#else
	fw->size = sizeof(XC5000_firmware_SEQUENCE);
	fw_size = ALIGN(fw->size, PAGE_SIZE);
	fw->data = vmalloc(fw_size);
	memcpy((unsigned char *)fw->data, (unsigned char *)XC5000_firmware_SEQUENCE, fw->size);

	if(fw->size != ITV_XC5000_DEFAULT_FIRMWARE_SIZE) {
		eprintk("xc5000: firmware incorrect size\n");
		ret = XC_RESULT_RESET_FAILURE;
	}
	else {
		dprintk("xc5000: firmware upload\n");
		ret = itv_xc5000_load_i2c_sequence(p_ifrontend, fw->data);
	}
#endif

	release_firmware(fw);

	return ret;
}

static int itv_xc5000_load_fw_and_init_tuner(itv_frontend_t *p_ifrontend)
{
	int ret = 0;
	
	itv_tuner_priv_t *priv = p_ifrontend->ituner.priv;

//	DEBUG_CALLSTACK

//	if(itv_xc5000_is_firmware_loaded(p_ifrontend) != XC_RESULT_SUCCESS)
	{
		ret = itv_xc5000_fwupload(p_ifrontend);
		if(ret != XC_RESULT_SUCCESS)
			return ret;
	}

	ret |= itv_xc5000_write_reg(priv, XREG_INIT, 0);
	msleep(100);

	ret |= itv_xc5000_write_reg(priv, XREG_SIGNALSOURCE, XC_RF_MODE_CABLE);

	priv->prev_if_khz = (unsigned int)(-1);
	priv->prev_rf_mode = priv->prev_video_standard = (unsigned char)(-1);	

	return ret;
}

static int itv_xc5000_set_signal_source(itv_tuner_priv_t *p_priv, unsigned char rf_mode)
{
//	DEBUG_CALLSTACK
	
	if(p_priv->prev_rf_mode != rf_mode)
	{
		p_priv->prev_rf_mode = rf_mode;

		dprintk("(%d) Source = %s\n", rf_mode, rf_mode == XC_RF_MODE_AIR ? "ANTENNA" : "CABLE");
		
		if((rf_mode != XC_RF_MODE_AIR) && (rf_mode != XC_RF_MODE_CABLE))
		{
			rf_mode = XC_RF_MODE_CABLE;
			eprintk("Invalid mode, defaulting to CABLE");
		}
		return itv_xc5000_write_reg(p_priv, XREG_SIGNALSOURCE, rf_mode);
	}
	else
		return XC_RESULT_SUCCESS;
}

static int itv_xc5000_set_tv_standard(itv_tuner_priv_t *p_priv, unsigned short video_mode, unsigned short audio_mode)
{
	int ret;
//	DEBUG_CALLSTACK
	
//	dprintk("(0x%04x,0x%04x)\n", video_mode, audio_mode);

	if(p_priv->prev_video_standard != p_priv->video_standard)
	{
		p_priv->prev_video_standard = p_priv->video_standard;
		
		dprintk("Standard = %s\n", XC5000_Standard[p_priv->video_standard].Name);

		ret = itv_xc5000_write_reg(p_priv, XREG_VIDEO_MODE, video_mode);
		if(ret == XC_RESULT_SUCCESS)
			ret = itv_xc5000_write_reg(p_priv, XREG_AUDIO_MODE, audio_mode);

		return ret;
	}
	else
		return XC_RESULT_SUCCESS;
}

#if 0
static int itv_xc5000_set_xtal_frequency(itv_tuner_priv_t *p_priv, unsigned int freq_khz)
{
	unsigned int freq_code = (32000 * 0x8000)/freq_khz;

//	DEBUG_CALLSTACK

	dprintk("(xtal_freq_khz = %d) freq_code = 0x%x\n", freq_khz, freq_code);

	return itv_xc5000_write_reg(p_priv, XREG_XTALFREQ, freq_code);
}
#endif

static int itv_xc5000_set_if_frequency(itv_tuner_priv_t *p_priv, unsigned int freq_khz)
{
	unsigned int freq_code = (freq_khz * 1024)/1000;
//	DEBUG_CALLSTACK

	if(p_priv->prev_if_khz != freq_khz)
	{
		dprintk("(freq_khz = %d) freq_code = 0x%x\n", freq_khz, freq_code);
		p_priv->prev_if_khz = freq_khz;
		return itv_xc5000_write_reg(p_priv, XREG_IF_OUT, freq_code);
	}
	else
		return XC_RESULT_SUCCESS;
}

static int itv_xc5000_set_rf_frequency(itv_frontend_t *p_ifrontend, unsigned int freq_hz)
{
	unsigned short freq_code;

	itv_tuner_priv_t *priv = p_ifrontend->ituner.priv;
	
//	DEBUG_CALLSTACK
		
	dprintk("(%u)\n", freq_hz);

	if((freq_hz > p_ifrontend->ituner.info.frequency_max) || (freq_hz < p_ifrontend->ituner.info.frequency_min))
		return XC_RESULT_OUT_OF_RANGE;

	freq_code = (unsigned short)(freq_hz / 15625);

	return itv_xc5000_write_reg(priv, XREG_RF_FREQ, freq_code);
}

static unsigned short itv_xc5000_wait_for_lock(itv_tuner_priv_t *p_priv)
{	
	int watchDogCount = 40;
	unsigned short lockState = 0;

//	DEBUG_CALLSTACK

	while((lockState == 0) && (watchDogCount > 0)) {
		itv_xc5000_get_lock_status(p_priv, &lockState);
		if(lockState == 0) {
			msleep(5);
			watchDogCount--;
		}
	}
	return lockState;
}

static int itv_xc5000_tune_channel(itv_frontend_t *p_ifrontend, unsigned int freq_hz)
{
	int found = 0;

	itv_tuner_priv_t *priv = p_ifrontend->ituner.priv;

//	DEBUG_CALLSTACK
		
	dprintk("(%u)\n", freq_hz);

	if(itv_xc5000_set_rf_frequency(p_ifrontend, freq_hz) != XC_RESULT_SUCCESS)
		return 0;

	if(itv_xc5000_wait_for_lock(priv) == 1)
		found = 1;

	return found;
}

static int itv_xc5000_init(itv_frontend_t *p_ifrontend)
{
	itv_tuner_priv_t *priv = p_ifrontend->ituner.priv;

	DEBUG_CALLSTACK

	if(itv_xc5000_load_fw_and_init_tuner(p_ifrontend) != XC_RESULT_SUCCESS) {
		eprintk("xc5000: Unable to initialise tuner\n");
		return -EREMOTEIO;
	}

	if(itv_xc5000_module_debug)
		itv_xc5000_debug_dump(priv);

	return 0;
}

static int itv_xc5000_force_init(itv_frontend_t *p_ifrontend)
{
	itv_tuner_priv_t *priv = p_ifrontend->ituner.priv;
	int ret = 0;

	DEBUG_CALLSTACK

//	if(itv_xc5000_load_fw_and_init_tuner(p_ifrontend) != XC_RESULT_SUCCESS) {
//		eprintk("xc5000: Unable to initialise tuner\n");
//		return -EREMOTEIO;
//	}

	ret = itv_xc5000_fwupload(p_ifrontend);
	if(ret != XC_RESULT_SUCCESS)
		return ret;

	ret |= itv_xc5000_write_reg(priv, XREG_INIT, 0);
	msleep(100);

	if(itv_xc5000_module_debug)
		itv_xc5000_debug_dump(priv);

	return 0;
}


static int itv_xc5000_set_params(itv_frontend_t *p_ifrontend, itv_frontend_parameters_t *params)
{
	int ret;

	itv_tuner_priv_t *priv = p_ifrontend->ituner.priv;

//	DEBUG_CALLSTACK

	dprintk("frequency=%d (Hz)\n", params->frequency);
	
	switch(params->u.vsb.modulation) {
		case VSB_8:
		case VSB_16:
			dprintk("VSB modulation\n");
			priv->rf_mode = XC_RF_MODE_AIR;
			priv->freq_hz = params->frequency - 1750000;
			priv->bandwidth = BANDWIDTH_6_MHZ;
			priv->video_standard = DTV6;
			break;
		case QAM_64:
		case QAM_256:
		case QAM_AUTO:
			dprintk("QAM modulation\n");
			priv->rf_mode = XC_RF_MODE_CABLE;
			priv->freq_hz = params->frequency - 1750000;
			priv->bandwidth = BANDWIDTH_6_MHZ;
			priv->video_standard = DTV6;
			break;
		default:
			return -EINVAL;
	}

	dprintk("frequency=%d (compensated)\n", priv->freq_hz);

	ret = itv_xc5000_set_signal_source(priv, priv->rf_mode);
	if(ret != XC_RESULT_SUCCESS) {
		eprintk("xc5000: xc_SetSignalSource(%d) failed\n", priv->rf_mode);
		return -EREMOTEIO;
	}

	ret = itv_xc5000_set_tv_standard(priv,
		XC5000_Standard[priv->video_standard].VideoMode,
		XC5000_Standard[priv->video_standard].AudioMode);
	if(ret != XC_RESULT_SUCCESS) {
		eprintk("xc5000: xc_SetTVStandard failed\n");
		return -EREMOTEIO;
	}

	ret = itv_xc5000_set_if_frequency(priv, priv->if_khz);
	if(ret != XC_RESULT_SUCCESS) {
		eprintk("xc5000: xc_Set_IF_frequency(%d) failed\n", priv->if_khz);
		return -EIO;
	}

#if 0
	ret = itv_xc5000_set_xtal_frequency(priv, 32000);
	if(ret != XC_RESULT_SUCCESS) {
		eprintk("xc5000: itv_xc5000_set_xtal_frequency(%d) failed\n", priv->if_khz);
		return -EIO;
	}
#endif

	ret = itv_xc5000_tune_channel(p_ifrontend, priv->freq_hz);

	if(itv_xc5000_module_debug)
		itv_xc5000_debug_dump(priv);

	return ret;
}

//20100413 koo : analog param set
static int itv_xc5000_set_analog_params(itv_frontend_t *p_ifrontend, itv_frontend_parameters_t *params)
{
	int ret;

	itv_tuner_priv_t *priv = p_ifrontend->ituner.priv;

	dprintk("frequency=%d (Hz) analog\n", params->frequency);

	priv->freq_hz 			= params->frequency;
	priv->rf_mode 			= XC_RF_MODE_CABLE; /* Fix me: it could be air. */
	priv->video_standard 	= MN_NTSC_PAL_Mono;

	ret = itv_xc5000_set_signal_source(priv, priv->rf_mode);
	if (ret != XC_RESULT_SUCCESS) {
		printk(KERN_ERR	"xc5000: xc_SetSignalSource(%d) failed\n", priv->rf_mode);
		return -EREMOTEIO;
	}

	ret = itv_xc5000_set_tv_standard(priv, XC5000_Standard[priv->video_standard].VideoMode, XC5000_Standard[priv->video_standard].AudioMode);	
	if (ret != XC_RESULT_SUCCESS) {
		printk(KERN_ERR "xc5000: xc_SetTVStandard failed\n");
		return -EREMOTEIO;
	}

	ret = itv_xc5000_tune_channel(p_ifrontend, priv->freq_hz);

	return ret;
}

static int itv_xc5000_get_frequency(itv_frontend_t *p_ifrontend, unsigned int *freq)
{
	itv_tuner_priv_t *priv = p_ifrontend->ituner.priv;

//	DEBUG_CALLSTACK
		
	*freq = priv->freq_hz;
	return 0;
}

static int itv_xc5000_get_bandwidth(itv_frontend_t *p_ifrontend, unsigned int *bw)
{
	itv_tuner_priv_t *priv = p_ifrontend->ituner.priv;

//	DEBUG_CALLSTACK

	*bw = priv->bandwidth;
	return 0;
}

static int itv_xc5000_get_status(itv_frontend_t *p_ifrontend, unsigned int *status)
{
	unsigned short lock_status = 0;

	itv_tuner_priv_t *priv = p_ifrontend->ituner.priv;

//	DEBUG_CALLSTACK

	itv_xc5000_get_lock_status(priv, &lock_status);

	dprintk("lock_status = 0x%08x\n", lock_status);

	*status = lock_status;

	return 0;
}

static itv_xc5000_config_t xc5000_config = {
	.i2c_address      = 0x61,

//20110919 koo : lgdt3305 & xc5000c에서는 if freq를 6MHz로 설정해야 demod lock이 잡힘.
#if defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000C) || defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000C_MODULE)
	.if_khz           = 6000, 
#else
	.if_khz           = 5380,
#endif
};

static const itv_tuner_t xc5000_tuner = {
	.info = {
		.name           = "Xceive XC5000",
		.frequency_min  =    1000000,
		.frequency_max  = 1023000000,
		.frequency_step =      50000,
	},

	.init				= itv_xc5000_init,
	.set_params			= itv_xc5000_set_params,

	//20100413 koo : set_analog_params define
	.set_analog_params	= itv_xc5000_set_analog_params,
		
	.get_frequency		= itv_xc5000_get_frequency,
	.get_bandwidth		= itv_xc5000_get_bandwidth,
	.get_status			= itv_xc5000_get_status
};

static int itv_xc5000_activate(itv_object_t *p_this)
{	
	unsigned short id = 0;

	itv_frontend_t *ifrontend = (itv_frontend_t *)p_this;
	itv_platform_frontend_operations_t *fe_ops = ifrontend->fe_ops;
	itv_tuner_priv_t *priv = NULL;

	int retry_cnt = 0;
	
	DEBUG_CALLSTACK
	
	priv = kmalloc(sizeof(*priv), GFP_KERNEL);
	if(priv == NULL) {
		eprintk("out of memory\n");
		return -1;
	}

	priv->i2c_props.addr 	= xc5000_config.i2c_address;
	priv->i2c_props.i2c 	= fe_ops->i2c_get_adapter();

	if(!priv->i2c_props.i2c) {
		eprintk("i2c_get_adapter is failed\n");
		kfree(priv);
		return -1;
	}

	priv->bandwidth	= BANDWIDTH_6_MHZ;
	priv->if_khz	= xc5000_config.if_khz;

	priv->prev_if_khz = (unsigned int)(-1);
	priv->prev_rf_mode = priv->prev_video_standard = (unsigned char)(-1);	

Retry:
	if(itv_xc5000_readreg(priv, XREG_PRODUCT_ID, &id) != 0) {
		fe_ops->i2c_put_adapter(priv->i2c_props.i2c);
		ifrontend->ituner.priv = NULL;
		kfree(priv);
		return -1;
	}

	switch(id) { 
		case XC_PRODUCT_ID_FW_LOADED:
			dprintk("xc5000: Successfully identified at address 0x%02x\n", priv->i2c_props.addr);
			dprintk("xc5000: Firmware has been loaded previously\n");
			break;
		case XC_PRODUCT_ID_FW_NOT_LOADED:
			dprintk("xc5000: Successfully identified at address 0x%02x\n", priv->i2c_props.addr);
			dprintk("xc5000: Firmware has not been loaded previously\n");
			break;
		default:
			eprintk("xc5000: Device not found at addr 0x%02x (0x%x)\n", priv->i2c_props.addr, id);

			if(++retry_cnt > 3) {
				fe_ops->i2c_put_adapter(priv->i2c_props.i2c);
				ifrontend->ituner.priv = NULL;
				kfree(priv);
				return -1;
			} else {
				mdelay(10);
				goto Retry;
			}
	}

#if defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000C) || defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000C_MODULE)
	printk("%s : xc5000c detect ok!!\n", __func__);
#else
	printk("%s : xc5000 detect ok!!\n", __func__);
#endif

	memcpy(&ifrontend->ituner, &xc5000_tuner, sizeof(itv_tuner_t));
	ifrontend->ituner.priv = priv;

	if(itv_xc5000_module_load_fw_on_attach) {
		if(itv_xc5000_force_init(ifrontend) != 0) {
			eprintk("Failed to initialize correctly\n");
			fe_ops->i2c_put_adapter(priv->i2c_props.i2c);
			ifrontend->ituner.priv = NULL;
			kfree(priv);
			return -1;
		}
	}

	return 0;
}

static void itv_xc5000_deactivate(itv_object_t *p_this)
{
	itv_frontend_t *ifrontend = (itv_frontend_t *)p_this;
	itv_platform_frontend_operations_t *fe_ops = ifrontend->fe_ops;
	itv_tuner_priv_t *priv = ifrontend->ituner.priv;
	
	DEBUG_CALLSTACK

	ifrontend->ituner.priv = NULL;
	fe_ops->i2c_put_adapter(priv->i2c_props.i2c);
	kfree(priv);
}

itv_module_t ixc5000_module = {
//20110824 koo : lgdt3305 demod & xc500c tuner porting
#if defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000C) || defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000C_MODULE)
	.psz_module_name = "xc5000c", 
#else
	.psz_module_name = "xc5000", 
#endif
	.psz_capability = "ituner", 
	.i_score = 20, 
	.pf_activate = itv_xc5000_activate,
	.pf_deactivate = itv_xc5000_deactivate,
	.next = NULL, 
	.parent = NULL, 
	.submodule = NULL, 
	.i_submodules = 0
};

static int __init itv_xc5000_module_init(void)
{
	int retval;

	DEBUG_CALLSTACK

	if((retval = itv_module_register(&ixc5000_module)) != 0) {
		eprintk("itv_module_register() is failed\n");
		goto out;
	}

	dprintk("iTV xc5000 module register success\n");

out:
	return retval;
}

static void __exit itv_xc5000_module_exit(void)
{
	DEBUG_CALLSTACK
		
	itv_module_unregister(&ixc5000_module);

	dprintk("iTV xc5000 module unregister success\n");
}

module_init(itv_xc5000_module_init);
module_exit(itv_xc5000_module_exit);

MODULE_AUTHOR("JP");
MODULE_LICENSE("GPL");
