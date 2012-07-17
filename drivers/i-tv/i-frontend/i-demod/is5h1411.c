#include <i-tv/itv_common.h>
#include <i-tv/itv_frontend.h>

#include "is5h1411.h"

static int itv_s5h1411_module_debug;
static int itv_s5h1411_module_debug_callstack;


//20110414 koo : snr & ber & status debug option
//#define DEMOD_DEBUG

module_param_named(debug, itv_s5h1411_module_debug, int, 0644);
module_param_named(debug_cs, itv_s5h1411_module_debug_callstack, int, 0644);

MODULE_PARM_DESC(debug, "Turn on/off s5h1411 debugging (default:off).");
MODULE_PARM_DESC(debug_cs, "Turn on/off s5h1411 callstack debugging (default:off).");

#ifdef dprintk
#undef dprintk
#endif

#define dprintk(args...) \
	do { if(itv_s5h1411_module_debug) { printk("[%s] ", __func__); printk(args); } } while (0)

#define DEBUG_CALLSTACK		\
	do { if(itv_s5h1411_module_debug_callstack) { printk("[Call] ----> %s()\n", __func__); } } while (0);

#define ENABLE_AUTO_SLEEP_CONTROL

/* Register values to initialise the demod, defaults to VSB */
static struct init_tab {
	unsigned char	addr;
	unsigned char	reg;
	unsigned short	data;
} init_tab[] = {
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x00, 0x0071, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x08, 0x0047, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x1c, 0x0400, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x1e, 0x0370, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x1f, 0x342c, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x24, 0x0231, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x25, 0x1011, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x26, 0x0f07, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x27, 0x0f04, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x28, 0x070f, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x29, 0x2820, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x2a, 0x102e, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x2b, 0x0220, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x2e, 0x0d0e, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x2f, 0x1013, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x31, 0x171b, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x32, 0x0e0f, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x33, 0x0f10, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x34, 0x170e, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x35, 0x4b10, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x36, 0x0f17, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x3c, 0x1577, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x3d, 0x081a, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x3e, 0x77ee, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x40, 0x1e09, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x41, 0x0f0c, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x42, 0x1f10, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x4d, 0x0509, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x4e, 0x0a00, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x50, 0x0000, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x5b, 0x0000, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x5c, 0x0008, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x57, 0x1101, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x65, 0x007c, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x68, 0x0512, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x69, 0x0258, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x70, 0x0004, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x71, 0x0007, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x76, 0x00a9, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x78, 0x3141, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0x7a, 0x3141, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0xb3, 0x8003, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0xb5, 0xa6bb, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0xb6, 0x0609, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0xb7, 0x2f06, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0xb8, 0x003f, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0xb9, 0x2700, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0xba, 0xfac8, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0xbe, 0x1003, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0xbf, 0x103f, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0xce, 0x2000, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0xcf, 0x0800, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0xd0, 0x0800, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0xd1, 0x0400, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0xd2, 0x0800, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0xd3, 0x2000, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0xd4, 0x3000, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0xdb, 0x4a9b, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0xdc, 0x1000, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0xde, 0x0001, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0xdf, 0x0000, },
	{ ITV_S5H1411_I2C_TOP_ADDR, 0xe3, 0x0301, },
	{ ITV_S5H1411_I2C_QAM_ADDR, 0xf3, 0x0000, },
	{ ITV_S5H1411_I2C_QAM_ADDR, 0xf3, 0x0001, },
	{ ITV_S5H1411_I2C_QAM_ADDR, 0x08, 0x0600, },
	{ ITV_S5H1411_I2C_QAM_ADDR, 0x18, 0x4201, },
	{ ITV_S5H1411_I2C_QAM_ADDR, 0x1e, 0x6476, },
	{ ITV_S5H1411_I2C_QAM_ADDR, 0x21, 0x0830, },
	{ ITV_S5H1411_I2C_QAM_ADDR, 0x0c, 0x5679, },
	{ ITV_S5H1411_I2C_QAM_ADDR, 0x0d, 0x579b, },
	{ ITV_S5H1411_I2C_QAM_ADDR, 0x24, 0x0102, },
	{ ITV_S5H1411_I2C_QAM_ADDR, 0x31, 0x7488, },
	{ ITV_S5H1411_I2C_QAM_ADDR, 0x32, 0x0a08, },
	{ ITV_S5H1411_I2C_QAM_ADDR, 0x3d, 0x8689, },
	{ ITV_S5H1411_I2C_QAM_ADDR, 0x49, 0x0048, },
	{ ITV_S5H1411_I2C_QAM_ADDR, 0x57, 0x2012, },
	{ ITV_S5H1411_I2C_QAM_ADDR, 0x5d, 0x7676, },
	{ ITV_S5H1411_I2C_QAM_ADDR, 0x04, 0x0400, },
	{ ITV_S5H1411_I2C_QAM_ADDR, 0x58, 0x00c0, },
	{ ITV_S5H1411_I2C_QAM_ADDR, 0x5b, 0x0100, },
};

/* VSB SNR lookup table */
static struct vsb_snr_tab {
	unsigned short val;
	unsigned short data;
} vsb_snr_tab[] = {
	{  0x39f, 300, },
	{  0x39b, 295, },
	{  0x397, 290, },
	{  0x394, 285, },
	{  0x38f, 280, },
	{  0x38b, 275, },
	{  0x387, 270, },
	{  0x382, 265, },
	{  0x37d, 260, },
	{  0x377, 255, },
	{  0x370, 250, },
	{  0x36a, 245, },
	{  0x364, 240, },
	{  0x35b, 235, },
	{  0x353, 230, },
	{  0x349, 225, },
	{  0x340, 220, },
	{  0x337, 215, },
	{  0x327, 210, },
	{  0x31b, 205, },
	{  0x310, 200, },
	{  0x302, 195, },
	{  0x2f3, 190, },
	{  0x2e4, 185, },
	{  0x2d7, 180, },
	{  0x2cd, 175, },
	{  0x2bb, 170, },
	{  0x2a9, 165, },
	{  0x29e, 160, },
	{  0x284, 155, },
	{  0x27a, 150, },
	{  0x260, 145, },
	{  0x23a, 140, },
	{  0x224, 135, },
	{  0x213, 130, },
	{  0x204, 125, },
	{  0x1fe, 120, },
	{      0,   0, },
};

/* QAM64 SNR lookup table */
static struct qam64_snr_tab {
	unsigned short val;
	unsigned short data;
} qam64_snr_tab[] = {
	{  0x0001,   0, },
	{  0x0af0, 300, },
	{  0x0d80, 290, },
	{  0x10a0, 280, },
	{  0x14b5, 270, },
	{  0x1590, 268, },
	{  0x1680, 266, },
	{  0x17b0, 264, },
	{  0x18c0, 262, },
	{  0x19b0, 260, },
	{  0x1ad0, 258, },
	{  0x1d00, 256, },
	{  0x1da0, 254, },
	{  0x1ef0, 252, },
	{  0x2050, 250, },
	{  0x20f0, 249, },
	{  0x21d0, 248, },
	{  0x22b0, 247, },
	{  0x23a0, 246, },
	{  0x2470, 245, },
	{  0x24f0, 244, },
	{  0x25a0, 243, },
	{  0x26c0, 242, },
	{  0x27b0, 241, },
	{  0x28d0, 240, },
	{  0x29b0, 239, },
	{  0x2ad0, 238, },
	{  0x2ba0, 237, },
	{  0x2c80, 236, },
	{  0x2d20, 235, },
	{  0x2e00, 234, },
	{  0x2f10, 233, },
	{  0x3050, 232, },
	{  0x3190, 231, },
	{  0x3300, 230, },
	{  0x3340, 229, },
	{  0x3200, 228, },
	{  0x3550, 227, },
	{  0x3610, 226, },
	{  0x3600, 225, },
	{  0x3700, 224, },
	{  0x3800, 223, },
	{  0x3920, 222, },
	{  0x3a20, 221, },
	{  0x3b30, 220, },
	{  0x3d00, 219, },
	{  0x3e00, 218, },
	{  0x4000, 217, },
	{  0x4100, 216, },
	{  0x4300, 215, },
	{  0x4400, 214, },
	{  0x4600, 213, },
	{  0x4700, 212, },
	{  0x4800, 211, },
	{  0x4a00, 210, },
	{  0x4b00, 209, },
	{  0x4d00, 208, },
	{  0x4f00, 207, },
	{  0x5050, 206, },
	{  0x5200, 205, },
	{  0x53c0, 204, },
	{  0x5450, 203, },
	{  0x5650, 202, },
	{  0x5820, 201, },
	{  0x6000, 200, },
	{  0xffff,   0, },
};

/* QAM256 SNR lookup table */
static struct qam256_snr_tab {
	unsigned short val;
	unsigned short data;
} qam256_snr_tab[] = {
	{  0x0001,   0, },
	{  0x0970, 400, },
	{  0x0a90, 390, },
	{  0x0b90, 380, },
	{  0x0d90, 370, },
	{  0x0ff0, 360, },
	{  0x1240, 350, },
	{  0x1345, 348, },
	{  0x13c0, 346, },
	{  0x14c0, 344, },
	{  0x1500, 342, },
	{  0x1610, 340, },
	{  0x1700, 338, },
	{  0x1800, 336, },
	{  0x18b0, 334, },
	{  0x1900, 332, },
	{  0x1ab0, 330, },
	{  0x1bc0, 328, },
	{  0x1cb0, 326, },
	{  0x1db0, 324, },
	{  0x1eb0, 322, },
	{  0x2030, 320, },
	{  0x2200, 318, },
	{  0x2280, 316, },
	{  0x2410, 314, },
	{  0x25b0, 312, },
	{  0x27a0, 310, },
	{  0x2840, 308, },
	{  0x29d0, 306, },
	{  0x2b10, 304, },
	{  0x2d30, 302, },
	{  0x2f20, 300, },
	{  0x30c0, 298, },
	{  0x3260, 297, },
	{  0x32c0, 296, },
	{  0x3300, 295, },
	{  0x33b0, 294, },
	{  0x34b0, 293, },
	{  0x35a0, 292, },
	{  0x3650, 291, },
	{  0x3800, 290, },
	{  0x3900, 289, },
	{  0x3a50, 288, },
	{  0x3b30, 287, },
	{  0x3cb0, 286, },
	{  0x3e20, 285, },
	{  0x3fa0, 284, },
	{  0x40a0, 283, },
	{  0x41c0, 282, },
	{  0x42f0, 281, },
	{  0x44a0, 280, },
	{  0x4600, 279, },
	{  0x47b0, 278, },
	{  0x4900, 277, },
	{  0x4a00, 276, },
	{  0x4ba0, 275, },
	{  0x4d00, 274, },
	{  0x4f00, 273, },
	{  0x5000, 272, },
	{  0x51f0, 272, },
	{  0x53a0, 270, },
	{  0x5520, 269, },
	{  0x5700, 268, },
	{  0x5800, 267, },
	{  0x5a00, 266, },
	{  0x5c00, 265, },
	{  0x5d00, 264, },
	{  0x5f00, 263, },
	{  0x6000, 262, },
	{  0x6200, 261, },
	{  0x6400, 260, },
	{  0xffff,   0, },
};

struct itv_demod_priv_t {
	struct i2c_adapter *i2c;

	itv_s5h1411_config_t *config;

	itv_frontend_modulation_e current_modulation;
	unsigned int first_tune:1;

	unsigned int current_frequency;
	int if_freq;

	unsigned char inversion;
#ifdef ENABLE_AUTO_SLEEP_CONTROL
	int f_demod_sleep;
#endif
};

static unsigned short itv_s5h1411_readreg(itv_demod_priv_t *p_priv, unsigned char addr, unsigned char reg)
{
	int ret;
	unsigned char b0[] = { reg };
	unsigned char b1[] = { 0, 0 };

	struct i2c_msg msg[] = {
		{ .addr = addr, .flags = 0, .buf = b0, .len = 1 },
		{ .addr = addr, .flags = I2C_M_RD, .buf = b1, .len = 2 } };

//	DEBUG_CALLSTACK

	ret = i2c_transfer(p_priv->i2c, msg, 2);
	if(ret <= 0)	eprintk("readreg error (ret == %i)\n", ret);
	
	return (b1[0] << 8) | b1[1];
}

static int itv_s5h1411_writereg(itv_demod_priv_t *p_priv, unsigned char addr, unsigned char reg, unsigned short data)
{
	int ret;
	unsigned char buf[] = { reg, data >> 8,  data & 0xff };

	struct i2c_msg msg = { .addr = addr, .flags = 0, .buf = buf, .len = 3 };

//	DEBUG_CALLSTACK

	ret = i2c_transfer(p_priv->i2c, &msg, 1);

	if(ret != 1) {
		eprintk("writereg error 0x%02x 0x%02x 0x%04x, " "ret == %i)\n", addr, reg, data, ret);
	}

	return (ret != 1) ? -1 : 0;
}

static int itv_s5h1411_softreset(itv_frontend_t *p_ifrontend)
{
	itv_demod_priv_t *priv = p_ifrontend->idemod.priv;

//	DEBUG_CALLSTACK

	itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xf7, 0);
	itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xf7, 1);
	
	return 0;
}

static int itv_s5h1411_set_if_freq(itv_frontend_t *p_ifrontend, int KHz)
{
	itv_demod_priv_t *priv = p_ifrontend->idemod.priv;

//	DEBUG_CALLSTACK
		
//	dprintk("(%d KHz)\n", KHz);

	switch(KHz) {
		case 3250:
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0x38, 0x10d5);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0x39, 0x5342);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_QAM_ADDR, 0x2c, 0x10d9);
			break;
		case 3500:
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0x38, 0x1225);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0x39, 0x1e96);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_QAM_ADDR, 0x2c, 0x1225);
			break;
		case 4000:
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0x38, 0x14bc);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0x39, 0xb53e);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_QAM_ADDR, 0x2c, 0x14bd);
			break;
		default:
			dprintk("(%d KHz) Invalid, defaulting to 5380\n", KHz);
			/* no break, need to continue */
		case 5380:
		case 44000:
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0x38, 0x1be4);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0x39, 0x3655);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_QAM_ADDR, 0x2c, 0x1be4);
			break;
	}
	priv->if_freq = KHz;

	return 0;
}

static int itv_s5h1411_set_mpeg_timing(itv_frontend_t *p_ifrontend, int mode)
{
	unsigned short val;
	
	itv_demod_priv_t *priv = p_ifrontend->idemod.priv;

//	DEBUG_CALLSTACK

	val = itv_s5h1411_readreg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xbe) & 0xcfff;
	switch(mode) {
		case ITV_S5H1411_MPEGTIMING_CONTINUOUS_INVERTING_CLOCK:
			val |= 0x0000;
			break;
		case ITV_S5H1411_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK:
			dprintk("(%d) Mode1 or Defaulting\n", mode);
			val |= 0x1000;
			break;
		case ITV_S5H1411_MPEGTIMING_NONCONTINUOUS_INVERTING_CLOCK:
			val |= 0x2000;
			break;
		case ITV_S5H1411_MPEGTIMING_NONCONTINUOUS_NONINVERTING_CLOCK:
			val |= 0x3000;
			break;
		default:
			return -EINVAL;
	}

	return itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xbe, val);
}

static int itv_s5h1411_set_inversion(itv_frontend_t *p_ifrontend, int inversion)
{
	unsigned short val;
	
	itv_demod_priv_t *priv = p_ifrontend->idemod.priv;

//	DEBUG_CALLSTACK

	val = itv_s5h1411_readreg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0x24) & ~0x1000;

	if(inversion == 1)
		val |= 0x1000; /* Inverted */

	priv->inversion = inversion;
	
	return itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0x24, val);
}

static int itv_s5h1411_set_serialmode(itv_frontend_t *p_ifrontend, int serial)
{
	unsigned short val;
	
	itv_demod_priv_t *priv = p_ifrontend->idemod.priv;

//	DEBUG_CALLSTACK

	val = itv_s5h1411_readreg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xbd) & ~0x100;

	if(serial == 1)
		val |= 0x100;

	return itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xbd, val);
}

static int itv_s5h1411_enable_modulation(itv_frontend_t *p_ifrontend, itv_frontend_modulation_e m)
{
	itv_demod_priv_t *priv = p_ifrontend->idemod.priv;

//	DEBUG_CALLSTACK

	if((priv->first_tune == 0) && (m == priv->current_modulation)) {
		dprintk("Already at desired modulation.  Skipping...\n");
		return 0;
	}

	switch(m) {
		case VSB_8:
			dprintk("VSB_8\n");
			itv_s5h1411_set_if_freq(p_ifrontend, priv->config->vsb_if);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0x00, 0x71);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xf6, 0x00);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xcd, 0xf1);
			break;
		case QAM_256:
			dprintk("256QAM\n");
			itv_s5h1411_set_if_freq(p_ifrontend, priv->config->qam_if);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0x00, 0x0171);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xf6, 0x0001);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_QAM_ADDR, 0x16, 0x1100);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_QAM_ADDR, 0x17, 0x0111);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xcd, 0x00f0);
			break;
		case QAM_64:
			dprintk("64QAM\n");
			itv_s5h1411_set_if_freq(p_ifrontend, priv->config->qam_if);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0x00, 0x0171);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xf6, 0x0001);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_QAM_ADDR, 0x16, 0x1100);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_QAM_ADDR, 0x17, 0x0101);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xcd, 0x00f0);
			break;
		case QAM_AUTO:
			dprintk("QAM_AUTO (64/256)\n");
			itv_s5h1411_set_if_freq(p_ifrontend, priv->config->qam_if);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0x00, 0x0171);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xf6, 0x0001);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_QAM_ADDR, 0x16, 0x1101);
			itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xcd, 0x00f0);
			break;
		default:
			dprintk("Invalid modulation\n");
			return -EINVAL;
	}

	priv->current_modulation = m;
	priv->first_tune = 0;
	itv_s5h1411_softreset(p_ifrontend);

	return 0;
}

static int itv_s5h1411_i2c_gate_ctrl(itv_frontend_t *p_ifrontend, int enable)
{
	itv_demod_priv_t *priv = p_ifrontend->idemod.priv;

//	DEBUG_CALLSTACK

	if(enable)
		return itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xf5, 1);
	else
		return itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xf5, 0);
}

static int itv_s5h1411_set_gpio(itv_frontend_t *p_ifrontend, int enable)
{
	unsigned short val;
	
	itv_demod_priv_t *priv = p_ifrontend->idemod.priv;

//	DEBUG_CALLSTACK

	val = itv_s5h1411_readreg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xe0) & ~0x02;

	if(enable)
		return itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xe0, val | 0x02);
	else
		return itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xe0, val);
}

static int itv_s5h1411_reset_tuner(itv_frontend_t *p_ifrontend, int component, int command, int arg)
{
	int ret = 0;
	unsigned short val;
	
	itv_demod_priv_t *priv = p_ifrontend->idemod.priv;

//	DEBUG_CALLSTACK

	val = itv_s5h1411_readreg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xe0) & ~0x02;

	itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xe0, val);
	msleep(330);

	itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xe0, val | 0x02);
	msleep(330);

	return ret;
}

static int itv_s5h1411_set_powerstate(itv_frontend_t *p_ifrontend, int enable)
{
	itv_demod_priv_t *priv = p_ifrontend->idemod.priv;

//	DEBUG_CALLSTACK

	if(enable)
	{
		itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xf4, 1);
#ifdef ENABLE_AUTO_SLEEP_CONTROL
		//printk("[%s] Sleep\n", __func__);
		priv->f_demod_sleep  = 1;
#endif
	}
	else {
#ifdef ENABLE_AUTO_SLEEP_CONTROL
		//printk("[%s] Wakeup\n", __func__);
		priv->f_demod_sleep  = 0;
#endif
		itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xf4, 0);
		itv_s5h1411_softreset(p_ifrontend);
	}

	return 0;
}

static int itv_s5h1411_sleep(itv_frontend_t *p_ifrontend)
{
//	DEBUG_CALLSTACK
		
	return itv_s5h1411_set_powerstate(p_ifrontend, 1);
}

static int itv_s5h1411_register_reset(itv_frontend_t *p_ifrontend)
{
	itv_demod_priv_t *priv = p_ifrontend->idemod.priv;

//	DEBUG_CALLSTACK

	return itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xf3, 0);
}

#ifdef DEMOD_DEBUG
struct task_struct *s5h1411_debug_thread = NULL;
itv_frontend_t *p_ifrontend_tmp= NULL;
static int itv_s5h1411_qam256_lookup_snr(itv_frontend_t *p_ifrontend, unsigned short *snr, unsigned short v);
static int itv_s5h1411_qam64_lookup_snr(itv_frontend_t *p_ifrontend, unsigned short *snr, unsigned short v);
static int itv_s5h1411_vsb_lookup_snr(itv_frontend_t *p_ifrontend, unsigned short *snr, unsigned short v);
static int itv_s5h1411_read_status(itv_frontend_t *p_ifrontend, itv_frontend_status_e *status);

#define DEMOD_CHK_TIME	10

int itv_s5h1411_debug_thread(void *data)
{
	itv_demod_priv_t *priv;
	unsigned short reg;
	unsigned short snr;
	itv_frontend_status_e status;
	
	while (!kthread_should_stop()) {
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(DEMOD_CHK_TIME*HZ);

		if(p_ifrontend_tmp != NULL) {
			priv = p_ifrontend_tmp->idemod.priv;

			switch(priv->current_modulation) {
				case QAM_64:
						reg = itv_s5h1411_readreg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xf1);
						if(itv_s5h1411_qam64_lookup_snr(p_ifrontend_tmp, &snr, reg) == 0)		printk("snr    : %d\n", snr);	
						else																	printk("snr    : read fail...\n");
					break;
				case QAM_256:
						reg = itv_s5h1411_readreg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xf1);
						if(itv_s5h1411_qam256_lookup_snr(p_ifrontend_tmp, &snr, reg) == 0)	printk("snr    : %d\n", snr);	
						else																	printk("snr    : read fail...\n");
					break;
				case VSB_8:
						reg = itv_s5h1411_readreg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xf2) & 0x3ff;
						if(itv_s5h1411_vsb_lookup_snr(p_ifrontend_tmp, &snr, reg) == 0)	printk("snr    : %d\n", snr);	
						else																printk("snr    : read fail...\n");
					break;
				default:
					break;
			}

			printk("ber    : %d\n", itv_s5h1411_readreg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xc9));

			if(itv_s5h1411_read_status(p_ifrontend_tmp, &status) == 0) {
				printk("status : ");
				if(status & FE_HAS_SIGNAL)	printk("signal ");
				if(status & FE_HAS_CARRIER)	printk("carrier ");
				if(status & FE_HAS_VITERBI)	printk("viterbi ");
				if(status & FE_HAS_SYNC)	printk("sync ");
				if(status & FE_HAS_LOCK)	printk("lock ");
				if(status & FE_TIMEOUT)		printk("timeout ");
				if(status & FE_REINIT)			printk("reinit ");
				printk("\n\n");
			} else {
				printk("status read fail...\n\n");
			}
		}
	}

	return 0;
}
#endif	//#if DEMOD_DEBUG

static int itv_s5h1411_set_frontend(itv_frontend_t *p_ifrontend, itv_frontend_parameters_t *params)
{
	itv_demod_priv_t *priv = p_ifrontend->idemod.priv;

	DEBUG_CALLSTACK

	dprintk("(frequency=%d)\n", params->frequency);

	p_ifrontend->tune_digital = 1;

#ifdef ENABLE_AUTO_SLEEP_CONTROL
	if( priv->f_demod_sleep )
	{
		/* wake up */
	 	itv_s5h1411_set_powerstate(p_ifrontend, 0);
	}
	else
#endif
	{
		itv_s5h1411_softreset(p_ifrontend);
	}

	priv->current_frequency = params->frequency;

	itv_s5h1411_enable_modulation(p_ifrontend, params->u.vsb.modulation);

	if(p_ifrontend->ituner.set_params) {
		if(p_ifrontend->idemod.i2c_gate_ctrl)
			p_ifrontend->idemod.i2c_gate_ctrl(p_ifrontend, 1);

		p_ifrontend->ituner.set_params(p_ifrontend, params);

		if(p_ifrontend->idemod.i2c_gate_ctrl)
			p_ifrontend->idemod.i2c_gate_ctrl(p_ifrontend, 0);
	}

	itv_s5h1411_softreset(p_ifrontend);

#ifdef DEMOD_DEBUG
	if(s5h1411_debug_thread == NULL) {
		s5h1411_debug_thread = kthread_run(itv_s5h1411_debug_thread, NULL, "itv_s5h1411_debug_thread");
	}
	p_ifrontend_tmp = p_ifrontend;
#endif	//#ifdef DEMOD_DEBUG

	return 0;
}

//20100413 koo : frontend analog set func
static int itv_s5h1411_set_frontend_analog(itv_frontend_t *p_ifrontend, itv_frontend_parameters_t *params)
{
	itv_demod_priv_t *priv = p_ifrontend->idemod.priv;
	int ret = 0;

	dprintk("(frequency=%d)\n", params->frequency);

	p_ifrontend->tune_digital = 0;

	itv_s5h1411_softreset(p_ifrontend);
	
	priv->current_frequency = params->frequency;
	
	if(p_ifrontend->ituner.set_analog_params)
	{
		if(p_ifrontend->idemod.i2c_gate_ctrl)
		{
			p_ifrontend->idemod.i2c_gate_ctrl(p_ifrontend, 1);
		}

		ret = p_ifrontend->ituner.set_analog_params(p_ifrontend, params);

		if(p_ifrontend->idemod.i2c_gate_ctrl)
		{
			p_ifrontend->idemod.i2c_gate_ctrl(p_ifrontend, 0);
		}
	}

	itv_s5h1411_softreset(p_ifrontend);

#ifdef ENABLE_AUTO_SLEEP_CONTROL
	if( !priv->f_demod_sleep )
	{
		/* sleep demod */
	 	itv_s5h1411_set_powerstate(p_ifrontend, 1);
	}
#endif

	return ret;
}

static int itv_s5h1411_init(itv_frontend_t *p_ifrontend)
{
	int i;
	
	itv_demod_priv_t *priv = p_ifrontend->idemod.priv;

	DEBUG_CALLSTACK

	itv_s5h1411_set_powerstate(p_ifrontend, 0);
	itv_s5h1411_register_reset(p_ifrontend);

	for(i = 0; i < ARRAY_SIZE(init_tab); i++) {
		itv_s5h1411_writereg(priv, init_tab[i].addr, init_tab[i].reg, init_tab[i].data);
	}

	priv->current_modulation = VSB_8;
	priv->first_tune = 1;
	priv->config->output_mode = p_ifrontend->tsif_mode;

	if(priv->config->output_mode == ITV_S5H1411_SERIAL_OUTPUT)
		itv_s5h1411_set_serialmode(p_ifrontend, 1);
	else
		itv_s5h1411_set_serialmode(p_ifrontend, 0);

	itv_s5h1411_set_inversion(p_ifrontend, priv->config->inversion);
	itv_s5h1411_set_if_freq(p_ifrontend, priv->config->vsb_if);
	itv_s5h1411_set_gpio(p_ifrontend, priv->config->gpio);
	itv_s5h1411_set_mpeg_timing(p_ifrontend, priv->config->mpeg_timing);
	itv_s5h1411_softreset(p_ifrontend);

	itv_s5h1411_i2c_gate_ctrl(p_ifrontend, 0);

#ifdef ENABLE_AUTO_SLEEP_CONTROL
 	itv_s5h1411_set_powerstate(p_ifrontend, 1);
#endif

	return 0;
}

static int itv_s5h1411_read_status(itv_frontend_t *p_ifrontend, itv_frontend_status_e *status)
{
	unsigned short reg;
	unsigned int tuner_status = 0;

	itv_demod_priv_t *priv = p_ifrontend->idemod.priv;

//	DEBUG_CALLSTACK

	*status = 0;

	switch(priv->current_modulation) {
		case QAM_64:
		case QAM_256:
			reg = itv_s5h1411_readreg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xf0);
			if(reg & 0x10) /* QAM FEC Lock */
				*status |= FE_HAS_SYNC | FE_HAS_LOCK;
			if(reg & 0x100) /* QAM EQ Lock */
				*status |= FE_HAS_VITERBI | FE_HAS_CARRIER | FE_HAS_SIGNAL;
			break;
		case VSB_8:
			reg = itv_s5h1411_readreg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xf2);
			if(reg & 0x1000) /* FEC Lock */
				*status |= FE_HAS_SYNC | FE_HAS_LOCK;
			if(reg & 0x2000) /* EQ Lock */
				*status |= FE_HAS_VITERBI | FE_HAS_CARRIER | FE_HAS_SIGNAL;

			reg = itv_s5h1411_readreg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0x53);
			if(reg & 0x1) /* AFC Lock */
				*status |= FE_HAS_SIGNAL;
			break;
		default:
			return -EINVAL;
	}

	switch(priv->config->status_mode) {
		case ITV_S5H1411_DEMODLOCKING:
			if(*status & FE_HAS_VITERBI)
				*status |= FE_HAS_CARRIER | FE_HAS_SIGNAL;
			break;
		case ITV_S5H1411_TUNERLOCKING:
			if(p_ifrontend->ituner.get_status) {
				if(p_ifrontend->idemod.i2c_gate_ctrl)
					p_ifrontend->idemod.i2c_gate_ctrl(p_ifrontend, 1);

				p_ifrontend->ituner.get_status(p_ifrontend, &tuner_status);

				if(p_ifrontend->idemod.i2c_gate_ctrl)
					p_ifrontend->idemod.i2c_gate_ctrl(p_ifrontend, 0);
			}
			if(tuner_status)
				*status |= FE_HAS_CARRIER | FE_HAS_SIGNAL;
			break;
	}

//	dprintk("status 0x%08x\n", *status);

	return 0;
}

static int itv_s5h1411_qam256_lookup_snr(itv_frontend_t *p_ifrontend, unsigned short *snr, unsigned short v)
{
	int i, ret = -EINVAL;
	
//	DEBUG_CALLSTACK

	for(i = 0; i < ARRAY_SIZE(qam256_snr_tab); i++) {
		if(v < qam256_snr_tab[i].val) {
			*snr = qam256_snr_tab[i].data;
			ret = 0;
			break;
		}
	}
	return ret;
}

static int itv_s5h1411_qam64_lookup_snr(itv_frontend_t *p_ifrontend, unsigned short *snr, unsigned short v)
{
	int i, ret = -EINVAL;

//	DEBUG_CALLSTACK

	for(i = 0; i < ARRAY_SIZE(qam64_snr_tab); i++) {
		if(v < qam64_snr_tab[i].val) {
			*snr = qam64_snr_tab[i].data;
			ret = 0;
			break;
		}
	}
	return ret;
}

static int itv_s5h1411_vsb_lookup_snr(itv_frontend_t *p_ifrontend, unsigned short *snr, unsigned short v)
{
	int i, ret = -EINVAL;

//	DEBUG_CALLSTACK

	for(i = 0; i < ARRAY_SIZE(vsb_snr_tab); i++) {
		if(v > vsb_snr_tab[i].val) {
			*snr = vsb_snr_tab[i].data;
			ret = 0;
			break;
		}
	}
//	dprintk("snr=%d\n", *snr);
	return ret;
}

static int itv_s5h1411_read_snr(itv_frontend_t *p_ifrontend, unsigned short *snr)
{
	unsigned short reg;
	
	itv_demod_priv_t *priv = p_ifrontend->idemod.priv;

//	DEBUG_CALLSTACK

	switch(priv->current_modulation) {
		case QAM_64:
			reg = itv_s5h1411_readreg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xf1);
			return itv_s5h1411_qam64_lookup_snr(p_ifrontend, snr, reg);
		case QAM_256:
			reg = itv_s5h1411_readreg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xf1);
			return itv_s5h1411_qam256_lookup_snr(p_ifrontend, snr, reg);
		case VSB_8:
			reg = itv_s5h1411_readreg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xf2) & 0x3ff;
			return itv_s5h1411_vsb_lookup_snr(p_ifrontend, snr, reg);
		default:
			break;
	}

	return -EINVAL;
}

static int itv_s5h1411_read_signal_strength(itv_frontend_t *p_ifrontend, unsigned short *signal_strength)
{
//	DEBUG_CALLSTACK
		
	return itv_s5h1411_read_snr(p_ifrontend, signal_strength);
}

static int itv_s5h1411_read_ucblocks(itv_frontend_t *p_ifrontend, unsigned int *ucblocks)
{
	itv_demod_priv_t *priv = p_ifrontend->idemod.priv;

//	DEBUG_CALLSTACK

	*ucblocks = itv_s5h1411_readreg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xc9);

	return 0;
}

static int itv_s5h1411_read_ber(itv_frontend_t *p_ifrontend, unsigned int *ber)
{
//	DEBUG_CALLSTACK
		
	return itv_s5h1411_read_ucblocks(p_ifrontend, ber);
}

static int itv_s5h1411_get_frontend(itv_frontend_t *p_ifrontend, itv_frontend_parameters_t *params)
{
	itv_demod_priv_t *priv = p_ifrontend->idemod.priv;

//	DEBUG_CALLSTACK

	params->frequency = priv->current_frequency;
	params->u.vsb.modulation = priv->current_modulation;

	return 0;
}

static int itv_s5h1411_get_tune_settings(itv_frontend_t *p_ifrontend, itv_frontend_tune_settings_t *tune)
{
//	DEBUG_CALLSTACK
	//tune->min_delay_ms = 1000;
	tune->min_delay_ms = 100;
	return 0;
}

static itv_s5h1411_config_t pinnacle_801e_config = {
	.output_mode   = ITV_S5H1411_SERIAL_OUTPUT, 
	.gpio          = ITV_S5H1411_GPIO_ON,
	.mpeg_timing   = ITV_S5H1411_MPEGTIMING_NONCONTINUOUS_NONINVERTING_CLOCK,
	.qam_if        = ITV_S5H1411_IF_44000,
	.vsb_if        = ITV_S5H1411_IF_44000,
	.inversion     = ITV_S5H1411_INVERSION_OFF,
//20100413 koo : locking mode select
	//.status_mode   = ITV_S5H1411_DEMODLOCKING
	.status_mode   = ITV_S5H1411_TUNERLOCKING
};

static itv_demod_t s5h1411_demod = {
	.info = {
		.name				= "Samsung S5H1411 QAM/8VSB Frontend",
		.type				= FE_ATSC,
		.frequency_min		= 54000000,
		.frequency_max		= 864000000,
		.frequency_stepsize	= 62500,
		.caps = FE_CAN_QAM_64 | FE_CAN_QAM_256 | FE_CAN_8VSB
	},

	.init					= itv_s5h1411_init,
	.sleep					= itv_s5h1411_sleep,
	.i2c_gate_ctrl			= itv_s5h1411_i2c_gate_ctrl,
	.set_frontend			= itv_s5h1411_set_frontend,

	//20100413 koo : frontend analog set func
	.set_frontend_analog	= itv_s5h1411_set_frontend_analog,
	
	.get_frontend			= itv_s5h1411_get_frontend,
	.get_tune_settings		= itv_s5h1411_get_tune_settings,
	.read_status			= itv_s5h1411_read_status,
	.read_ber				= itv_s5h1411_read_ber,
	.read_signal_strength	= itv_s5h1411_read_signal_strength,
	.read_snr				= itv_s5h1411_read_snr,
	.read_ucblocks			= itv_s5h1411_read_ucblocks,
	.reset_tuner			= itv_s5h1411_reset_tuner
};

static int itv_s5h1411_activate(itv_object_t *p_this)
{
	unsigned short reg;

	itv_frontend_t *ifrontend = (itv_frontend_t *)p_this;
	itv_platform_frontend_operations_t *fe_ops = ifrontend->fe_ops;
	itv_demod_priv_t *priv = NULL;
	int reset_retry_count = 5;
	
	DEBUG_CALLSTACK

	priv = kmalloc(sizeof(*priv), GFP_KERNEL);
	if(priv == NULL) {
		eprintk("out of memory\n");
		return -1;
	}

	priv->config 				= &pinnacle_801e_config;
	priv->i2c 					= fe_ops->i2c_get_adapter();
	priv->current_modulation 	= VSB_8;
	priv->inversion 			= priv->config->inversion;

	if(!priv->i2c) {
		eprintk("i2c_get_adapter is failed\n");
		kfree(priv);
		return -1;
	}

	/* reset */
	do
	{
	    fe_ops->demod_reset();

	    reg = itv_s5h1411_readreg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0x05);

		if(reg == 0x0066)
		{
			break;
		}
		dprintk("Fail to identify s5h1411. reg=%02X\n", reg);
		msleep(50);
	} while( --reset_retry_count > 0 );

	if( reset_retry_count == 0)
	{
		eprintk("Fail to identify s5h1411. reg=%02X\n", reg);
		kfree(priv);
		return -1;
	}

	if( reset_retry_count < 5 )
	{
		dprintk("Success identification\n" );
	}

	memcpy(&ifrontend->idemod, &s5h1411_demod, sizeof(itv_demod_t));
#ifdef ENABLE_AUTO_SLEEP_CONTROL
	priv->f_demod_sleep  = 0;
#endif
	ifrontend->idemod.priv = priv;

//20110520 koo : s5h1411 init은 frontend open 시 수행하도록 함.
#if 0
	if(itv_s5h1411_init(ifrontend) != 0) {
		eprintk("Failed to initialize correctly\n");
		fe_ops->i2c_put_adapter(priv->i2c);
		ifrontend->idemod.priv = NULL;
		kfree(priv);
		return -1;
	}

	itv_s5h1411_writereg(priv, ITV_S5H1411_I2C_TOP_ADDR, 0xf5, 1);
#endif

	printk("%s : s5h1411 detect ok!!\n", __func__);

	return 0;
}

static void itv_s5h1411_deactivate(itv_object_t *p_this)
{
	itv_frontend_t *ifrontend = (itv_frontend_t *)p_this;
	itv_platform_frontend_operations_t *fe_ops = ifrontend->fe_ops;
	itv_demod_priv_t *priv = ifrontend->idemod.priv;

	DEBUG_CALLSTACK

#ifdef DEMOD_DEBUG
	if(s5h1411_debug_thread != NULL) {
		p_ifrontend_tmp = NULL;
		kthread_stop(s5h1411_debug_thread); 
		s5h1411_debug_thread = NULL;
	}
#endif

	ifrontend->idemod.priv = NULL;
	fe_ops->i2c_put_adapter(priv->i2c);
	kfree(priv);
}

itv_module_t itv_s5h1411_module = {
	.psz_module_name = "s5h1411", 
	.psz_capability = "idemod", 
	.i_score = 20, 
	.pf_activate = itv_s5h1411_activate,
	.pf_deactivate = itv_s5h1411_deactivate,
	.next = NULL, 
	.parent = NULL, 
	.submodule = NULL, 
	.i_submodules = 0
};

static int __init itv_s5h1411_module_init(void)
{
	int retval;

	DEBUG_CALLSTACK

	if((retval = itv_module_register(&itv_s5h1411_module)) != 0) {
		eprintk("itv_module_register() is failed\n");
		goto out;
	}

	dprintk("iTV s5h1411 module register success\n");

out:
	return retval;
}

#if defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
#include <mach/gpio.h>
#include <mach/bsp.h>
#include <mach/io.h>
#endif

static void __exit itv_s5h1411_module_exit(void)
{
	DEBUG_CALLSTACK
		
	itv_module_unregister(&itv_s5h1411_module);

#if defined(CONFIG_MACH_TCC8800)
#if defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000) || defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000_MODULE)
	//20110405 koo : S5H1411&XC5000_ATSC_SV6.0 brd에서는 cam1을 사용하므로 cam1_on on/off
	gpio_request(TCC_GPEXT1(13), NULL);
	gpio_request(TCC_GPEXT3(6), NULL);
	gpio_direction_output(TCC_GPEXT1(13), 0);
	gpio_direction_output(TCC_GPEXT3(6), 0);
#elif defined(CONFIG_iTV_FE_TUNER_MODULE_FQD1136) || defined(CONFIG_iTV_FE_TUNER_MODULE_FQD1136_MODULE)
	//20110405 koo : S5H1411&FQD1163_ATSC_SV6.0 brd에서는 ts parallel을 사용하므로 tspr_on on/off
	gpio_request(TCC_GPEXT3(4), NULL);
	gpio_direction_output(TCC_GPEXT3(4), 0);
#endif	//#if defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000) || defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000_MODULE)
#elif defined(CONFIG_MACH_TCC8800ST)
	PGPION regs;

	regs = (volatile PGPION)tcc_p2v(HwGPIOF_BASE);

	BITCSET(regs->GPFN3, (Hw8-Hw4), 0);
	BITSET(regs->GPEN, Hw25);
	BITCLR(regs->GPDAT, Hw25);				//TS_PWDN	
#elif defined(CONFIG_MACH_TCC8920ST)
	PGPION regs;

	regs = (volatile PGPION)tcc_p2v(HwGPIOE_BASE);

	BITCSET(regs->GPFN3.nREG, (Hw20-Hw16), 0);
	BITSET(regs->GPEN.nREG, Hw28);
	BITCLR(regs->GPDAT.nREG, Hw28);				//TS_PWDN	
#endif

	dprintk("iTV s5h1411 module unregister success\n");
}

module_init(itv_s5h1411_module_init);
module_exit(itv_s5h1411_module_exit);

MODULE_AUTHOR("JP");
MODULE_LICENSE("GPL");
