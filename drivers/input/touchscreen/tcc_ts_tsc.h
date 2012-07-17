

//===========================================================
// tca_tchhwctl.h
#ifdef CONFIG_TOUCHSCREEN_TCC_AK4183
#define _TOUCH_AK4183_
#endif
#ifdef CONFIG_TOUCHSCREEN_TCC_TSC2003
#define _TOUCH_TSC2003__
#endif

void tsc2003_enableirq(void);
void tsc2003_disableirq(void);
unsigned int tsc2003_getirqnum(void);

//===========================================================




//===========================================================
// tca_tsc2003tchctl.h
#if !defined(_TCA_TSC2003CTRL_)
#define _TCA_TSC2003CTRL_

void tsc2003_port_init(void);
void tsc2003_poweroff(void);
void tsc2003_setreadcmd(int channel, unsigned char *nBfrTXD);

#endif
//===========================================================




//===========================================================
// tca_tchcontrol.h
#define READ_X	0
#define READ_Y	1
#define READ_Z1 2
#define READ_Z2 3


void tca_rawtoscreen(int *, int *, unsigned int, unsigned int);
int tsc2003_getrawdata(int * x, int *y);
int tsc2003_getrawdata2(int * x, int *y);
void tsc2003_touchbubblesort(unsigned int Number[],unsigned int num) ;
extern void tea_tch_openi2c(void);
extern short tsc2003_read_i2c(int channel);
extern void Serial_Printf(int x, int y, int z1, int z2, int tmp);
//===========================================================





