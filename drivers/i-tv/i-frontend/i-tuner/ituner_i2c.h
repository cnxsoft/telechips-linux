#ifndef __ITV_TUNER_I2C_H__
#define __ITV_TUNER_I2C_H__

typedef struct itv_tuner_i2c_props_t {
	unsigned char addr;
	struct i2c_adapter *i2c;
} itv_tuner_i2c_props_t;

static inline int itv_tuner_i2c_xfer_send(itv_tuner_i2c_props_t *props, char *buf, int len)
{
	struct i2c_msg msg = { .addr = props->addr, .flags = 0,
			       .buf = buf, .len = len };
	int ret = i2c_transfer(props->i2c, &msg, 1);

	return (ret == 1) ? len : ret;
}

static inline int itv_tuner_i2c_xfer_recv(itv_tuner_i2c_props_t *props, char *buf, int len)
{
	struct i2c_msg msg = { .addr = props->addr, .flags = I2C_M_RD,
			       .buf = buf, .len = len };
	int ret = i2c_transfer(props->i2c, &msg, 1);

	return (ret == 1) ? len : ret;
}

static inline int itv_tuner_i2c_xfer_send_recv(itv_tuner_i2c_props_t *props,
					   char *obuf, int olen,
					   char *ibuf, int ilen)
{
	struct i2c_msg msg[2] = { { .addr = props->addr, .flags = 0,
				    .buf = obuf, .len = olen },
				  { .addr = props->addr, .flags = I2C_M_RD,
				    .buf = ibuf, .len = ilen } };
	int ret = i2c_transfer(props->i2c, msg, 2);

	return (ret == 2) ? ilen : ret;
}
#endif	/* __ITV_TUNER_I2C_H__ */
