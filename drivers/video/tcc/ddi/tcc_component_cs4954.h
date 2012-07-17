
#ifndef __TCC_COMPONENT_CS4954_H__
#define __TCC_COMPONENT_CS4954_H__


#define CS4954_I2C_ADDR			0x08
#define CS4954_I2C_DEVICE		(CS4954_I2C_ADDR>>1)

extern int cs4954_i2c_write(unsigned char reg, unsigned char val);
extern int cs4954_i2c_read(unsigned char reg, unsigned char *val);
extern void cs4954_reset(void);
extern int cs4954_set_ready(void);
extern void cs4954_set_end(void);
extern void cs4954_set_mode(int type);
extern void cs4954_enable(int type);

#endif //__TCC_COMPONENT_CS4954_H__

