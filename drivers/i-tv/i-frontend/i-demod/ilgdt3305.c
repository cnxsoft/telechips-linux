/*
 *    Support for LGDT3305 - VSB/QAM
 *
 *    Copyright (C) 2008, 2009 Michael Krufky <mkrufky@linuxtv.org>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */



#include <i-tv/itv_common.h>
#include <i-tv/itv_frontend.h>

#include "../../i-core/imath.h"
#include "ilgdt3305.h"

static int itv_lgdt3305_module_debug;
static int itv_lgdt3305_module_debug_callstack;

module_param_named(debug, itv_lgdt3305_module_debug, int, 0644);
module_param_named(debug_cs, itv_lgdt3305_module_debug_callstack, int, 0644);

MODULE_PARM_DESC(debug, "Turn on/off lgdt3305 debugging (default:off).");
MODULE_PARM_DESC(debug_cs, "Turn on/off lgdt3305 callstack debugging (default:off).");

#ifdef dprintk
#undef dprintk
#endif

#define dprintk(args...) \
	do { if(itv_lgdt3305_module_debug) { printk("[%s] ", __func__); printk(args); } } while (0)

#define DEBUG_CALLSTACK		\
	do { if(itv_lgdt3305_module_debug_callstack) { printk("[Call] ----> %s()\n", __func__); } } while (0);

#define lg_fail(ret)							\
({									\
	int __ret;							\
	__ret = (ret < 0);						\
	if (__ret)							\
		eprintk("error %d on line %d\n",	ret, __LINE__);		\
	__ret;								\
})




struct itv_demod_priv_t {
	struct i2c_adapter *i2c_adap;
	const struct lgdt3305_config *cfg;

	itv_frontend_modulation_e current_modulation;

	u32 current_frequency;
	u32 snr;
};


#define LGDT3305_GEN_CTRL_1                   0x0000
#define LGDT3305_GEN_CTRL_2                   0x0001
#define LGDT3305_GEN_CTRL_3                   0x0002
#define LGDT3305_GEN_STATUS                   0x0003
#define LGDT3305_GEN_CONTROL                  0x0007
#define LGDT3305_GEN_CTRL_4                   0x000a
#define LGDT3305_DGTL_AGC_REF_1               0x0012
#define LGDT3305_DGTL_AGC_REF_2               0x0013
#define LGDT3305_CR_CTR_FREQ_1                0x0106
#define LGDT3305_CR_CTR_FREQ_2                0x0107
#define LGDT3305_CR_CTR_FREQ_3                0x0108
#define LGDT3305_CR_CTR_FREQ_4                0x0109
#define LGDT3305_CR_MSE_1                     0x011b
#define LGDT3305_CR_MSE_2                     0x011c
#define LGDT3305_CR_LOCK_STATUS               0x011d
#define LGDT3305_CR_CTRL_7                    0x0126
#define LGDT3305_AGC_POWER_REF_1              0x0300
#define LGDT3305_AGC_POWER_REF_2              0x0301
#define LGDT3305_AGC_DELAY_PT_1               0x0302
#define LGDT3305_AGC_DELAY_PT_2               0x0303
#define LGDT3305_RFAGC_LOOP_FLTR_BW_1         0x0306
#define LGDT3305_RFAGC_LOOP_FLTR_BW_2         0x0307
#define LGDT3305_IFBW_1                       0x0308
#define LGDT3305_IFBW_2                       0x0309
#define LGDT3305_AGC_CTRL_1                   0x030c
#define LGDT3305_AGC_CTRL_4                   0x0314
#define LGDT3305_EQ_MSE_1                     0x0413
#define LGDT3305_EQ_MSE_2                     0x0414
#define LGDT3305_EQ_MSE_3                     0x0415
#define LGDT3305_PT_MSE_1                     0x0417
#define LGDT3305_PT_MSE_2                     0x0418
#define LGDT3305_PT_MSE_3                     0x0419
#define LGDT3305_FEC_BLOCK_CTRL               0x0504
#define LGDT3305_FEC_LOCK_STATUS              0x050a
#define LGDT3305_FEC_PKT_ERR_1                0x050c
#define LGDT3305_FEC_PKT_ERR_2                0x050d
#define LGDT3305_TP_CTRL_1                    0x050e
#define LGDT3305_BERT_PERIOD                  0x0801
#define LGDT3305_BERT_ERROR_COUNT_1           0x080a
#define LGDT3305_BERT_ERROR_COUNT_2           0x080b
#define LGDT3305_BERT_ERROR_COUNT_3           0x080c
#define LGDT3305_BERT_ERROR_COUNT_4           0x080d





static int lgdt3305_write_reg(itv_demod_priv_t *state, u16 reg, u8 val)
{
	int ret;
	u8 buf[] = { reg >> 8, reg & 0xff, val };
	struct i2c_msg msg = {
		.addr = state->cfg->i2c_addr, .flags = 0,
		.buf = buf, .len = 3,
	};

	ret = i2c_transfer(state->i2c_adap, &msg, 1);

	if (ret != 1) {
		eprintk("error (addr %02x %02x <- %02x, err = %i)\n",
		       msg.buf[0], msg.buf[1], msg.buf[2], ret);
		if (ret < 0)
			return ret;
		else
			return -EREMOTEIO;
	}
	return 0;
}

static int lgdt3305_read_reg(itv_demod_priv_t *state, u16 reg, u8 *val)
{
	int ret;
	u8 reg_buf[] = { reg >> 8, reg & 0xff };
	struct i2c_msg msg[] = {
		{ .addr = state->cfg->i2c_addr,
		  .flags = 0, .buf = reg_buf, .len = 2 },
		{ .addr = state->cfg->i2c_addr,
		  .flags = I2C_M_RD, .buf = val, .len = 1 },
	};

	ret = i2c_transfer(state->i2c_adap, msg, 2);

	//printk("\n\tlgdt3305 [%s][0x%x] 0x%x => 0x%x\n\n", state->i2c_adap->name, state->cfg->i2c_addr, reg, *val);

	if(ret <= 0) {
		eprintk("error (addr %02x reg %04x error (ret == %i)\n",
		       state->cfg->i2c_addr, reg, ret);
		if (ret < 0)
			return ret;
		else
			return -EREMOTEIO;
	}
	return 0;
}

#define read_reg(state, reg)						\
({									\
	u8 __val;							\
	int ret = lgdt3305_read_reg(state, reg, &__val);		\
	if (lg_fail(ret))						\
		__val = 0;						\
	__val;								\
})

static int lgdt3305_set_reg_bit(itv_demod_priv_t *state,
				u16 reg, int bit, int onoff)
{
	u8 val;
	int ret;

	ret = lgdt3305_read_reg(state, reg, &val);
	if (lg_fail(ret))
		goto fail;

	val &= ~(1 << bit);
	val |= (onoff & 1) << bit;

	ret = lgdt3305_write_reg(state, reg, val);
fail:
	return ret;
}

struct lgdt3305_reg {
	u16 reg;
	u8 val;
};

static int lgdt3305_write_regs(itv_demod_priv_t *state,
			       struct lgdt3305_reg *regs, int len)
{
	int i, ret;

	for (i = 0; i < len - 1; i++) {
		ret = lgdt3305_write_reg(state, regs[i].reg, regs[i].val);
		if (lg_fail(ret))
			return ret;
	}
	return 0;
}

static int lgdt3305_soft_reset(itv_demod_priv_t *state)
{
	int ret;

	dprintk("\n");

	ret = lgdt3305_set_reg_bit(state, LGDT3305_GEN_CTRL_3, 0, 0);
	if (lg_fail(ret))
		goto fail;

	msleep(20);
	ret = lgdt3305_set_reg_bit(state, LGDT3305_GEN_CTRL_3, 0, 1);

fail:
	return ret;
}

static inline int lgdt3305_mpeg_mode(itv_demod_priv_t *state,
				     enum lgdt3305_mpeg_mode mode)
{
	dprintk("(%d)\n", mode);
	return lgdt3305_set_reg_bit(state, LGDT3305_TP_CTRL_1, 5, mode);
}

static int lgdt3305_mpeg_mode_polarity(itv_demod_priv_t *state,
				       enum lgdt3305_tp_clock_edge edge,
				       enum lgdt3305_tp_valid_polarity valid)
{
	u8 val;
	int ret;

	dprintk("edge = %d, valid = %d\n", edge, valid);

	ret = lgdt3305_read_reg(state, LGDT3305_TP_CTRL_1, &val);
	if (lg_fail(ret))
		goto fail;

	val &= ~0x09;

	if (edge)
		val |= 0x08;
	if (valid)
		val |= 0x01;

	ret = lgdt3305_write_reg(state, LGDT3305_TP_CTRL_1, val);
	if (lg_fail(ret))
		goto fail;

	ret = lgdt3305_soft_reset(state);
fail:
	return ret;
}

static int lgdt3305_set_modulation(itv_demod_priv_t *state,
				   struct itv_frontend_parameters_t *param)
{
	u8 opermode;
	int ret;

	dprintk("\n");

	ret = lgdt3305_read_reg(state, LGDT3305_GEN_CTRL_1, &opermode);
	if (lg_fail(ret))
		goto fail;

	opermode &= ~0x03;

	switch (param->u.vsb.modulation) {
	case VSB_8:
		opermode |= 0x03;
		break;
	case QAM_64:
		opermode |= 0x00;
		break;
	case QAM_256:
		opermode |= 0x01;
		break;
	default:
		return -EINVAL;
	}
	ret = lgdt3305_write_reg(state, LGDT3305_GEN_CTRL_1, opermode);

fail:
	return ret;
}

static int lgdt3305_set_filter_extension(itv_demod_priv_t *state,
					 struct itv_frontend_parameters_t *param)
{
	int val;

	switch (param->u.vsb.modulation) {
	case VSB_8:
		val = 0;
		break;
	case QAM_64:
	case QAM_256:
		val = 1;
		break;
	default:
		return -EINVAL;
	}
	dprintk("val = %d\n", val);

	return lgdt3305_set_reg_bit(state, 0x043f, 2, val);
}

static int lgdt3305_passband_digital_agc(itv_demod_priv_t *state,
					 struct itv_frontend_parameters_t *param)
{
	u16 agc_ref;

	switch (param->u.vsb.modulation) {
	case VSB_8:
		agc_ref = 0x32c4;
		break;
	case QAM_64:
		agc_ref = 0x2a00;
		break;
	case QAM_256:
		agc_ref = 0x2a80;
		break;
	default:
		return -EINVAL;
	}

	dprintk("agc ref: 0x%04x\n", agc_ref);

	lgdt3305_write_reg(state, LGDT3305_DGTL_AGC_REF_1, agc_ref >> 8);
	lgdt3305_write_reg(state, LGDT3305_DGTL_AGC_REF_2, agc_ref & 0xff);

	return 0;
}

static int lgdt3305_rfagc_loop(itv_demod_priv_t *state,
			       struct itv_frontend_parameters_t *param)
{
	u16 ifbw, rfbw, agcdelay;

	switch (param->u.vsb.modulation) {
	case VSB_8:
		agcdelay = 0x04c0;
		rfbw     = 0x8000;
		ifbw     = 0x8000;
		break;
	case QAM_64:
	case QAM_256:
		agcdelay = 0x046b;
		rfbw     = 0x8889;
		ifbw     = 0x8888;
		break;
	default:
		return -EINVAL;
	}

	if (state->cfg->rf_agc_loop) {
		dprintk("agcdelay: 0x%04x, rfbw: 0x%04x\n", agcdelay, rfbw);

		/* rf agc loop filter bandwidth */
		lgdt3305_write_reg(state, LGDT3305_AGC_DELAY_PT_1,
				   agcdelay >> 8);
		lgdt3305_write_reg(state, LGDT3305_AGC_DELAY_PT_2,
				   agcdelay & 0xff);

		lgdt3305_write_reg(state, LGDT3305_RFAGC_LOOP_FLTR_BW_1,
				   rfbw >> 8);
		lgdt3305_write_reg(state, LGDT3305_RFAGC_LOOP_FLTR_BW_2,
				   rfbw & 0xff);
	} else {
		dprintk("ifbw: 0x%04x\n", ifbw);

		/* if agc loop filter bandwidth */
		lgdt3305_write_reg(state, LGDT3305_IFBW_1, ifbw >> 8);
		lgdt3305_write_reg(state, LGDT3305_IFBW_2, ifbw & 0xff);
	}

	return 0;
}

static int lgdt3305_agc_setup(itv_demod_priv_t *state,
			      struct itv_frontend_parameters_t *param)
{
	int lockdten, acqen;

	switch (param->u.vsb.modulation) {
	case VSB_8:
		lockdten = 0;
		acqen = 0;
		break;
	case QAM_64:
	case QAM_256:
		lockdten = 1;
		acqen = 1;
		break;
	default:
		return -EINVAL;
	}

	dprintk("lockdten = %d, acqen = %d\n", lockdten, acqen);

	/* control agc function */
	lgdt3305_write_reg(state, LGDT3305_AGC_CTRL_4, 0xe1 | lockdten << 1);
	lgdt3305_set_reg_bit(state, LGDT3305_AGC_CTRL_1, 2, acqen);

	return lgdt3305_rfagc_loop(state, param);
}

static int lgdt3305_set_agc_power_ref(itv_demod_priv_t *state,
				      struct itv_frontend_parameters_t *param)
{
	u16 usref = 0;

	switch (param->u.vsb.modulation) {
	case VSB_8:
		if (state->cfg->usref_8vsb)
			usref = state->cfg->usref_8vsb;
		break;
	case QAM_64:
		if (state->cfg->usref_qam64)
			usref = state->cfg->usref_qam64;
		break;
	case QAM_256:
		if (state->cfg->usref_qam256)
			usref = state->cfg->usref_qam256;
		break;
	default:
		return -EINVAL;
	}

	if (usref) {
		dprintk("set manual mode: 0x%04x\n", usref);

		lgdt3305_set_reg_bit(state, LGDT3305_AGC_CTRL_1, 3, 1);

		lgdt3305_write_reg(state, LGDT3305_AGC_POWER_REF_1,
				   0xff & (usref >> 8));
		lgdt3305_write_reg(state, LGDT3305_AGC_POWER_REF_2,
				   0xff & (usref >> 0));
	}

	return 0;
}

static int lgdt3305_spectral_inversion(itv_demod_priv_t *state,
				       struct itv_frontend_parameters_t *param,
				       int inversion)
{
	int ret;

	dprintk("(%d)\n", inversion);

	switch (param->u.vsb.modulation) {
	case VSB_8:
		ret = lgdt3305_write_reg(state, LGDT3305_CR_CTRL_7,
					 inversion ? 0xf9 : 0x79);
		break;
	case QAM_64:
	case QAM_256:
		ret = lgdt3305_write_reg(state, LGDT3305_FEC_BLOCK_CTRL,
					 inversion ? 0xfd : 0xff);
		break;
	default:
		ret = -EINVAL;
	}
	
	return ret;
}

static int lgdt3305_set_if(itv_demod_priv_t *state,
			   struct itv_frontend_parameters_t *param)
{
	u16 if_freq_khz;
	u8 nco1, nco2, nco3, nco4;
	u64 nco;

	switch (param->u.vsb.modulation) {
	case VSB_8:
		if_freq_khz = state->cfg->vsb_if_khz;
		break;
	case QAM_64:
	case QAM_256:
		if_freq_khz = state->cfg->qam_if_khz;
		break;
	default:
		return -EINVAL;
	}

	nco = if_freq_khz / 10;

	switch (param->u.vsb.modulation) {
	case VSB_8:
		nco <<= 24;
		do_div(nco, 625);
		break;
	case QAM_64:
	case QAM_256:
		nco <<= 28;
		do_div(nco, 625);
		break;
	default:
		return -EINVAL;
	}

	nco1 = (nco >> 24) & 0x3f;
	nco1 |= 0x40;
	nco2 = (nco >> 16) & 0xff;
	nco3 = (nco >> 8) & 0xff;
	nco4 = nco & 0xff;

	lgdt3305_write_reg(state, LGDT3305_CR_CTR_FREQ_1, nco1);
	lgdt3305_write_reg(state, LGDT3305_CR_CTR_FREQ_2, nco2);
	lgdt3305_write_reg(state, LGDT3305_CR_CTR_FREQ_3, nco3);
	lgdt3305_write_reg(state, LGDT3305_CR_CTR_FREQ_4, nco4);

	dprintk("%d KHz -> [%02x%02x%02x%02x]\n",
	       if_freq_khz, nco1, nco2, nco3, nco4);

	return 0;
}

static int lgdt3305_i2c_gate_ctrl(struct itv_frontend_t *fe, int enable)
{
	itv_demod_priv_t *state = fe->idemod.priv;

	if (state->cfg->deny_i2c_rptr)
		return 0;

	dprintk("(%d)\n", enable);
	return lgdt3305_set_reg_bit(state, LGDT3305_GEN_CTRL_2, 5,
				    enable ? 0 : 1);
}

static int lgdt3305_sleep(struct itv_frontend_t *fe)
{
	itv_demod_priv_t *state = fe->idemod.priv;
	u8 gen_ctrl_3, gen_ctrl_4;

	dprintk("\n");

	gen_ctrl_3 = read_reg(state, LGDT3305_GEN_CTRL_3);
	gen_ctrl_4 = read_reg(state, LGDT3305_GEN_CTRL_4);

	/* hold in software reset while sleeping */
	gen_ctrl_3 &= ~0x01;
	/* tristate the IF-AGC pin */
	gen_ctrl_3 |=  0x02;
	/* tristate the RF-AGC pin */
	gen_ctrl_3 |=  0x04;

	/* disable vsb/qam module */
	gen_ctrl_4 &= ~0x01;
	/* disable adc module */
	gen_ctrl_4 &= ~0x02;

	lgdt3305_write_reg(state, LGDT3305_GEN_CTRL_3, gen_ctrl_3);
	lgdt3305_write_reg(state, LGDT3305_GEN_CTRL_4, gen_ctrl_4);

	return 0;
}

static int lgdt3305_init(struct itv_frontend_t *fe)
{
	itv_demod_priv_t *state = fe->idemod.priv;
	int ret;

	static struct lgdt3305_reg lgdt3305_init_data[] = {
		{ .reg = LGDT3305_GEN_CTRL_1,
		  .val = 0x03, },
		{ .reg = LGDT3305_GEN_CTRL_2,
		  .val = 0xb0, },
		{ .reg = LGDT3305_GEN_CTRL_3,
		  .val = 0x01, },
		{ .reg = LGDT3305_GEN_CONTROL,
		  .val = 0x6f, },
		{ .reg = LGDT3305_GEN_CTRL_4,
		  .val = 0x03, },
		{ .reg = LGDT3305_DGTL_AGC_REF_1,
		  .val = 0x32, },
		{ .reg = LGDT3305_DGTL_AGC_REF_2,
		  .val = 0xc4, },
		{ .reg = LGDT3305_CR_CTR_FREQ_1,
		  .val = 0x00, },
		{ .reg = LGDT3305_CR_CTR_FREQ_2,
		  .val = 0x00, },
		{ .reg = LGDT3305_CR_CTR_FREQ_3,
		  .val = 0x00, },
		{ .reg = LGDT3305_CR_CTR_FREQ_4,
		  .val = 0x00, },
		{ .reg = LGDT3305_CR_CTRL_7,
		  .val = 0x79, },
		{ .reg = LGDT3305_AGC_POWER_REF_1,
		  .val = 0x32, },
		{ .reg = LGDT3305_AGC_POWER_REF_2,
		  .val = 0xc4, },
		{ .reg = LGDT3305_AGC_DELAY_PT_1,
		  .val = 0x0d, },
		{ .reg = LGDT3305_AGC_DELAY_PT_2,
		  .val = 0x30, },
		{ .reg = LGDT3305_RFAGC_LOOP_FLTR_BW_1,
		  .val = 0x80, },
		{ .reg = LGDT3305_RFAGC_LOOP_FLTR_BW_2,
		  .val = 0x00, },
		{ .reg = LGDT3305_IFBW_1,
		  .val = 0x80, },
		{ .reg = LGDT3305_IFBW_2,
		  .val = 0x00, },
		{ .reg = LGDT3305_AGC_CTRL_1,
		  .val = 0x30, },
		{ .reg = LGDT3305_AGC_CTRL_4,
		  .val = 0x61, },
		{ .reg = LGDT3305_FEC_BLOCK_CTRL,
		  .val = 0xff, },
		{ .reg = LGDT3305_TP_CTRL_1,
		  .val = 0x1b, },
	};

	dprintk("\n");

	ret = lgdt3305_write_regs(state, lgdt3305_init_data,
				  ARRAY_SIZE(lgdt3305_init_data));
	if (lg_fail(ret))
		goto fail;

	ret = lgdt3305_soft_reset(state);

fail:
	return ret;
}

static void lgdt3305_regall_debug(itv_demod_priv_t *state)
{
	u8 val;
	int ret;
	unsigned short reg_addr = 0;

	printk("\nLGDT3305 #######################\n");
	for(reg_addr=0; reg_addr<=0x13; reg_addr++) {
		ret = lgdt3305_read_reg(state, reg_addr, &val);
		printk("addr : 0x%04x  =>  data : 0x%02x\n", reg_addr, val);
	}
	for(reg_addr=0x106; reg_addr<=0x11d; reg_addr++) {
		if(!((reg_addr == 0x10a) | (reg_addr == 0x10b) | (reg_addr == 0x10c) | (reg_addr == 0x10d) | (reg_addr == 0x10f) | (reg_addr == 0x110) |	\
			 (reg_addr == 0x111) | (reg_addr == 0x117))) {
			ret = lgdt3305_read_reg(state, reg_addr, &val);
			printk("addr : 0x%04x  =>  data : 0x%02x\n", reg_addr, val);
		}
	}
	reg_addr = 0x126;
	ret = lgdt3305_read_reg(state, reg_addr, &val);
	printk("addr : 0x%04x  =>  data : 0x%02x\n", reg_addr, val);
	for(reg_addr=0x200; reg_addr<=0x206; reg_addr++) {
		if(!((reg_addr == 0x201))) {
			ret = lgdt3305_read_reg(state, reg_addr, &val);
			printk("addr : 0x%04x  =>  data : 0x%02x\n", reg_addr, val);
		}
	}
	for(reg_addr=0x214; reg_addr<=0x21c; reg_addr++) {
		if(!((reg_addr == 0x215) | (reg_addr == 0x216))) {
			ret = lgdt3305_read_reg(state, reg_addr, &val);
			printk("addr : 0x%04x  =>  data : 0x%02x\n", reg_addr, val);
		}
	}
	for(reg_addr=0x300; reg_addr<=0x323; reg_addr++) {
		ret = lgdt3305_read_reg(state, reg_addr, &val);
		printk("addr : 0x%04x  =>  data : 0x%02x\n", reg_addr, val);
	}
	for(reg_addr=0x400; reg_addr<=0x41d; reg_addr++) {
		if(!((reg_addr == 0x402) | (reg_addr == 0x406) | (reg_addr == 0x408) | (reg_addr == 0x40a) | (reg_addr == 0x40b) | (reg_addr == 0x40c) | (reg_addr == 0x40d)	\
			 | (reg_addr == 0x416) | (reg_addr == 0x41a) | (reg_addr == 0x41b) | (reg_addr == 0x41c))) {
			ret = lgdt3305_read_reg(state, reg_addr, &val);
			printk("addr : 0x%04x  =>  data : 0x%02x\n", reg_addr, val);
		}
	}
	for(reg_addr=0x500; reg_addr<=0x523; reg_addr++) {
		if(!((reg_addr == 0x505))) {
			ret = lgdt3305_read_reg(state, reg_addr, &val);
			printk("addr : 0x%04x  =>  data : 0x%02x\n", reg_addr, val);
		}
	}
	for(reg_addr=0x800; reg_addr<=0x80d; reg_addr++) {
		if(!((reg_addr == 0x802))) {
			ret = lgdt3305_read_reg(state, reg_addr, &val);
			printk("addr : 0x%04x  =>  data : 0x%02x\n", reg_addr, val);
		}
	}
	for(reg_addr=0x900; reg_addr<=0x939; reg_addr++) {
		if(!((reg_addr == 0x908) | (reg_addr == 0x918) | (reg_addr == 0x92a) | (reg_addr == 0x932) | (reg_addr == 0x936))) {
			ret = lgdt3305_read_reg(state, reg_addr, &val);
			printk("addr : 0x%04x  =>  data : 0x%02x\n", reg_addr, val);
		}
	}
	printk("################################\n\n");
}

static int lgdt3305_set_parameters(struct itv_frontend_t *fe,
				   struct itv_frontend_parameters_t *param)
{
	itv_demod_priv_t *state = fe->idemod.priv;
	int ret;

	dprintk("(%d, %d)\n", param->frequency, param->u.vsb.modulation);

	//20110919 koo : swzigzag에서 digital/analog check
	fe->tune_digital = 1;
	
	if (fe->ituner.set_params) {
		ret = fe->ituner.set_params(fe, param);
		if (fe->idemod.i2c_gate_ctrl)
			fe->idemod.i2c_gate_ctrl(fe, 0);
		if (lg_fail(ret))
			goto fail;
		state->current_frequency = param->frequency;
	}

	ret = lgdt3305_set_modulation(state, param);
	if (lg_fail(ret))
		goto fail;

	ret = lgdt3305_passband_digital_agc(state, param);
	if (lg_fail(ret))
		goto fail;
	ret = lgdt3305_set_agc_power_ref(state, param);
	if (lg_fail(ret))
		goto fail;
	ret = lgdt3305_agc_setup(state, param);
	if (lg_fail(ret))
		goto fail;

	/* low if */
	ret = lgdt3305_write_reg(state, LGDT3305_GEN_CONTROL, 0x2f);
	if (lg_fail(ret))
		goto fail;
	ret = lgdt3305_set_reg_bit(state, LGDT3305_CR_CTR_FREQ_1, 6, 1);
	if (lg_fail(ret))
		goto fail;

	ret = lgdt3305_set_if(state, param);
	if (lg_fail(ret))
		goto fail;
	ret = lgdt3305_spectral_inversion(state, param,
					  state->cfg->spectral_inversion
					  ? 1 : 0);
	if (lg_fail(ret))
		goto fail;

	ret = lgdt3305_set_filter_extension(state, param);
	if (lg_fail(ret))
		goto fail;

	state->current_modulation = param->u.vsb.modulation;

	ret = lgdt3305_mpeg_mode(state, state->cfg->mpeg_mode);
	if (lg_fail(ret))
		goto fail;

	/* lgdt3305_mpeg_mode_polarity calls lgdt3305_soft_reset */
	ret = lgdt3305_mpeg_mode_polarity(state,
					  state->cfg->tpclk_edge,
					  state->cfg->tpvalid_polarity);

	//lgdt3305_regall_debug(state);
	
fail:
	return ret;
}

static int lgdt3305_get_frontend(struct itv_frontend_t *fe,
				 struct itv_frontend_parameters_t *param)
{
	itv_demod_priv_t *state = fe->idemod.priv;

	dprintk("\n");

	param->u.vsb.modulation = state->current_modulation;
	param->frequency = state->current_frequency;
	return 0;
}

static int lgdt3305_read_cr_lock_status(itv_demod_priv_t *state,
					int *locked)
{
	u8 val;
	int ret;
	char *cr_lock_state = "";

	*locked = 0;

	ret = lgdt3305_read_reg(state, LGDT3305_CR_LOCK_STATUS, &val);
	if (lg_fail(ret))
		goto fail;

	switch (state->current_modulation) {
	case QAM_256:
	case QAM_64:
		if (val & (1 << 1))
			*locked = 1;

		switch (val & 0x07) {
		case 0:
			cr_lock_state = "QAM UNLOCK";
			break;
		case 4:
			cr_lock_state = "QAM 1stLock";
			break;
		case 6:
			cr_lock_state = "QAM 2ndLock";
			break;
		case 7:
			cr_lock_state = "QAM FinalLock";
			break;
		default:
			cr_lock_state = "CLOCKQAM-INVALID!";
			break;
		}
		break;
	case VSB_8:
		if (val & (1 << 7)) {
			*locked = 1;
			cr_lock_state = "CLOCKVSB";
		}
		break;
	default:
		ret = -EINVAL;
	}
	dprintk("(%d) %s\n", *locked, cr_lock_state);
fail:
	return ret;
}

static int lgdt3305_read_fec_lock_status(itv_demod_priv_t *state,
					 int *locked)
{
	u8 val;
	int ret, mpeg_lock, fec_lock, viterbi_lock;

	*locked = 0;

	switch (state->current_modulation) {
	case QAM_256:
	case QAM_64:
		ret = lgdt3305_read_reg(state,
					LGDT3305_FEC_LOCK_STATUS, &val);
		if (lg_fail(ret))
			goto fail;

		mpeg_lock    = (val & (1 << 0)) ? 1 : 0;
		fec_lock     = (val & (1 << 2)) ? 1 : 0;
		viterbi_lock = (val & (1 << 3)) ? 1 : 0;

		*locked = mpeg_lock && fec_lock && viterbi_lock;
		dprintk("(%d) %s%s%s\n", *locked,
		       mpeg_lock    ? "mpeg lock  "  : "",
		       fec_lock     ? "fec lock  "   : "",
		       viterbi_lock ? "viterbi lock" : "");
		break;
	case VSB_8:
	default:
		ret = -EINVAL;
	}
fail:
	return ret;
}

static int lgdt3305_read_status(struct itv_frontend_t *fe, itv_frontend_status_e *status)
{
	itv_demod_priv_t *state = fe->idemod.priv;
	u8 val;
	int ret, signal, inlock, nofecerr, snrgood,
		cr_lock, fec_lock, sync_lock;

	*status = 0;

	ret = lgdt3305_read_reg(state, LGDT3305_GEN_STATUS, &val);
	if (lg_fail(ret))
		goto fail;

	signal    = (val & (1 << 4)) ? 1 : 0;
	inlock    = (val & (1 << 3)) ? 0 : 1;
	sync_lock = (val & (1 << 2)) ? 1 : 0;
	nofecerr  = (val & (1 << 1)) ? 1 : 0;
	snrgood   = (val & (1 << 0)) ? 1 : 0;

	dprintk("%s%s%s%s%s\n",
	       signal    ? "SIGNALEXIST " : "",
	       inlock    ? "INLOCK "      : "",
	       sync_lock ? "SYNCLOCK "    : "",
	       nofecerr  ? "NOFECERR "    : "",
	       snrgood   ? "SNRGOOD "     : "");

	ret = lgdt3305_read_cr_lock_status(state, &cr_lock);
	if (lg_fail(ret))
		goto fail;

	if (signal)
		*status |= FE_HAS_SIGNAL;
	if (cr_lock)
		*status |= FE_HAS_CARRIER;
	if (nofecerr)
		*status |= FE_HAS_VITERBI;
	if (sync_lock)
		*status |= FE_HAS_SYNC;

	switch (state->current_modulation) {
	case QAM_256:
	case QAM_64:
		ret = lgdt3305_read_fec_lock_status(state, &fec_lock);
		if (lg_fail(ret))
			goto fail;

		if (fec_lock)
			*status |= FE_HAS_LOCK;
		break;
	case VSB_8:
		if (inlock)
			*status |= FE_HAS_LOCK;
		break;
	default:
		ret = -EINVAL;
	}
fail:
	return ret;
}

/* borrowed from lgdt330x.c */
static u32 calculate_snr(u32 mse, u32 c)
{
	if (mse == 0) /* no signal */
		return 0;

	mse = intlog10(mse);
	if (mse > c) {
		/* Negative SNR, which is possible, but realisticly the
		demod will lose lock before the signal gets this bad.  The
		API only allows for unsigned values, so just return 0 */
		return 0;
	}
	return 10*(c - mse);
}

static int lgdt3305_read_snr(struct itv_frontend_t *fe, u16 *snr)
{
	itv_demod_priv_t *state = fe->idemod.priv;
	u32 noise;	/* noise value */
	u32 c;		/* per-modulation SNR calculation constant */

	switch (state->current_modulation) {
	case VSB_8:
#ifdef USE_PTMSE
		/* Use Phase Tracker Mean-Square Error Register */
		/* SNR for ranges from -13.11 to +44.08 */
		noise =	((read_reg(state, LGDT3305_PT_MSE_1) & 0x07) << 16) |
			(read_reg(state, LGDT3305_PT_MSE_2) << 8) |
			(read_reg(state, LGDT3305_PT_MSE_3) & 0xff);
		c = 73957994; /* log10(25*32^2)*2^24 */
#else
		/* Use Equalizer Mean-Square Error Register */
		/* SNR for ranges from -16.12 to +44.08 */
		noise =	((read_reg(state, LGDT3305_EQ_MSE_1) & 0x0f) << 16) |
			(read_reg(state, LGDT3305_EQ_MSE_2) << 8) |
			(read_reg(state, LGDT3305_EQ_MSE_3) & 0xff);
		c = 73957994; /* log10(25*32^2)*2^24 */
#endif
		break;
	case QAM_64:
	case QAM_256:
		noise = (read_reg(state, LGDT3305_CR_MSE_1) << 8) |
			(read_reg(state, LGDT3305_CR_MSE_2) & 0xff);

		c = (state->current_modulation == QAM_64) ?
			97939837 : 98026066;
		/* log10(688128)*2^24 and log10(696320)*2^24 */
		break;
	default:
		return -EINVAL;
	}
	state->snr = calculate_snr(noise, c);
	/* report SNR in dB * 10 */
	*snr = (state->snr / ((1 << 24) / 10));
	
	dprintk("noise = 0x%08x, snr = %d.%02d dB\n", noise,
	       state->snr >> 24, (((state->snr >> 8) & 0xffff) * 100) >> 16);
	return 0;
}

static int lgdt3305_read_signal_strength(struct itv_frontend_t *fe,
					 u16 *strength)
{
	/* borrowed from lgdt330x.c
	 *
	 * Calculate strength from SNR up to 35dB
	 * Even though the SNR can go higher than 35dB,
	 * there is some comfort factor in having a range of
	 * strong signals that can show at 100%
	 */
	itv_demod_priv_t *state = fe->idemod.priv;
	u16 snr;
	int ret;

	*strength = 0;

	ret = fe->idemod.read_snr(fe, &snr);
	if (lg_fail(ret))
		goto fail;
	/* Rather than use the 8.8 value snr, use state->snr which is 8.24 */
	/* scale the range 0 - 35*2^24 into 0 - 65535 */
	if (state->snr >= 8960 * 0x10000)
		*strength = 0xffff;
	else
		*strength = state->snr / 8960;

	//20111117 koo test debug : snr max 256
	#define APP_SNR_MAX_GUAGE		256
	*strength /= (65535 / APP_SNR_MAX_GUAGE);

fail:
	return ret;
}

static int lgdt3305_read_ber(struct itv_frontend_t *fe, u32 *ber)
{
	*ber = 0;
	return 0;
}

static int lgdt3305_read_ucblocks(struct itv_frontend_t *fe, u32 *ucblocks)
{
	itv_demod_priv_t *state = fe->idemod.priv;

	*ucblocks =
		(read_reg(state, LGDT3305_FEC_PKT_ERR_1) << 8) |
		(read_reg(state, LGDT3305_FEC_PKT_ERR_2) & 0xff);

	return 0;
}

static int lgdt3305_get_tune_settings(struct itv_frontend_t *fe,
				      struct itv_frontend_tune_settings_t
					*fe_tune_settings)
{
	fe_tune_settings->min_delay_ms = 500;
	dprintk("\n");
	return 0;
}

static struct lgdt3305_config hcw_lgdt3305_config = {
	.i2c_addr           = 0x0e,
	.mpeg_mode          = LGDT3305_MPEG_PARALLEL,
	.tpclk_edge         = LGDT3305_TPCLK_RISING_EDGE,
	.tpvalid_polarity   = LGDT3305_TP_VALID_HIGH,
	//20110825 koo :  lgdt3305 demod & xc500c tuner porting : tuner setting은 cpu에서 direct 하도록 되어 있음.
	.deny_i2c_rptr      = 1,
	.rf_agc_loop 	= 0,
#if defined(CONFIG_iTV_FE_TUNER_MODULE_FJ2148) || defined(CONFIG_iTV_FE_TUNER_MODULE_FJ2148_MODULE)
	.spectral_inversion = 1,
	.qam_if_khz         = 3600,
	.vsb_if_khz         = 3600,
#else
	.spectral_inversion = 0,
	.qam_if_khz         = 6000,
	.vsb_if_khz         = 6000,
#endif
	.usref_8vsb         = 0x0500,
}; 
 
static itv_demod_t lgdt3305_demod = {
	.info = {
		.name = "LG Electronics LGDT3305 VSB/QAM Frontend",
		.type               = FE_ATSC,
		.frequency_min      = 54000000,
		.frequency_max      = 858000000,
		.frequency_stepsize = 62500,
		.caps = FE_CAN_QAM_64 | FE_CAN_QAM_256 | FE_CAN_8VSB
	},
	.i2c_gate_ctrl        = lgdt3305_i2c_gate_ctrl,
	.init                 = lgdt3305_init,
	.sleep                = lgdt3305_sleep,
	.set_frontend         = lgdt3305_set_parameters,
	.get_frontend         = lgdt3305_get_frontend,
	.get_tune_settings    = lgdt3305_get_tune_settings,
	.read_status          = lgdt3305_read_status,
	.read_ber             = lgdt3305_read_ber,
	.read_signal_strength = lgdt3305_read_signal_strength,
	.read_snr             = lgdt3305_read_snr,
	.read_ucblocks        = lgdt3305_read_ucblocks,
};

static int itv_lgdt3305_activate(itv_object_t *p_this)
{
	unsigned char val;
	unsigned int ret;

	itv_frontend_t *ifrontend = (itv_frontend_t *)p_this;
	itv_platform_frontend_operations_t *fe_ops = ifrontend->fe_ops;
	itv_demod_priv_t *priv = NULL;
	
	DEBUG_CALLSTACK

	priv = kmalloc(sizeof(*priv), GFP_KERNEL);
	if(priv == NULL) {
		eprintk("out of memory\n");
		return -1;
	}
	
	priv->cfg	 	= &hcw_lgdt3305_config;
	priv->i2c_adap	= fe_ops->i2c_get_adapter();
	
	if(!priv->i2c_adap) {
		eprintk("i2c_get_adapter is failed\n");
		kfree(priv);
		return -1;
	}

	fe_ops->demod_reset();

	//20120315 koo : demod reset 후 detect 시 delay 없이 동작시켰을 경우 read 시에 문제가 발생하는 경우가 나와 delay 추가.
	mdelay(10);
	
	ret = lgdt3305_read_reg(priv, LGDT3305_GEN_CTRL_2, &val);
	if ((lg_fail(ret)) | (val == 0))
		goto fail;
	ret = lgdt3305_write_reg(priv, 0x0808, 0x80);
	if (lg_fail(ret))
		goto fail;

	//20120315 koo : demod reset 후 detect 시 delay 없이 동작시켰을 경우 read 시에 문제가 발생하는 경우가 나와 delay 추가.
	mdelay(10);
	
	ret = lgdt3305_read_reg(priv, 0x0808, &val);
	if ((lg_fail(ret)) | (val != 0x80))
		goto fail;
	ret = lgdt3305_write_reg(priv, 0x0808, 0x00);
	if (lg_fail(ret))
		goto fail;

	printk("%s : lgdt3305 detect ok!!\n", __func__);

	priv->current_frequency = -1;
	priv->current_modulation = -1;
	
	memcpy(&ifrontend->idemod, &lgdt3305_demod, sizeof(itv_demod_t));
	ifrontend->idemod.priv = priv;

	return 0;
	
fail:
	eprintk("unable to detect LGDT3305 hardware\n");
	kfree(priv);
	return -1;
}

static void itv_lgdt3305_deactivate(itv_object_t *p_this)
{
	itv_frontend_t *ifrontend = (itv_frontend_t *)p_this;
	itv_platform_frontend_operations_t *fe_ops = ifrontend->fe_ops;
	itv_demod_priv_t *priv = ifrontend->idemod.priv;

	DEBUG_CALLSTACK

	ifrontend->idemod.priv = NULL;
	fe_ops->i2c_put_adapter(priv->i2c_adap);
	kfree(priv);
}


itv_module_t itv_lgdt3305_module = {
	.psz_module_name = "lgdt3305", 
	.psz_capability = "idemod", 
	.i_score = 20, 
	.pf_activate = itv_lgdt3305_activate,
	.pf_deactivate = itv_lgdt3305_deactivate,
	.next = NULL, 
	.parent = NULL, 
	.submodule = NULL, 
	.i_submodules = 0
};

static int __init itv_lgdt3305_module_init(void)
{
	int retval;

	DEBUG_CALLSTACK

	if((retval = itv_module_register(&itv_lgdt3305_module)) != 0) {
		eprintk("itv_module_register() is failed\n");
		goto out;
	}

	dprintk("iTV lgdt3305 module register success\n");

out:
	return retval;
}

#if defined(CONFIG_ARCH_TCC8800)
#include <mach/tcc_pca953x.h>
#include <mach/io.h>
extern int tcc_pca953x_setup(int slave, int name, int direction, int value, int mode);
#endif

static void __exit itv_lgdt3305_module_exit(void)
{
	DEBUG_CALLSTACK
		
	itv_module_unregister(&itv_lgdt3305_module);

#if defined(CONFIG_ARCH_TCC8800)
#if defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000C) || defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000C_MODULE)
	PGPION regs;

	#define TSPR_ON     			Hw4
	tcc_pca953x_setup(PCA9539_U5_SLAVE_ADDR, TSPR_ON, OUTPUT, OFF, (SET_DIRECTION|SET_VALUE));		//TS_PWR

	regs = (volatile PGPION)tcc_p2v(HwGPIOF_BASE);
	BITCLR(regs->GPDAT, Hw22);				//TS_PWDN
	BITCLR(regs->GPDAT, Hw13);				//TS_PWDN
#endif
#endif

	dprintk("iTV lgdt3305 module unregister success\n");
}

module_init(itv_lgdt3305_module_init);
module_exit(itv_lgdt3305_module_exit);

MODULE_DESCRIPTION("LG Electronics LGDT3305 ATSC/QAM-B Demodulator Driver");
MODULE_AUTHOR("Michael Krufky <mkrufky@linuxtv.org>");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

