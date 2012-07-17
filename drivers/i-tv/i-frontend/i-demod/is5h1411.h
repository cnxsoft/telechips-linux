#ifndef __IS5H1411_H__
#define __IS5H1411_H__

#define ITV_S5H1411_I2C_TOP_ADDR (0x32 >> 1)
#define ITV_S5H1411_I2C_QAM_ADDR (0x34 >> 1)

/* serial/parallel output */
#define ITV_S5H1411_PARALLEL_OUTPUT 0
#define ITV_S5H1411_SERIAL_OUTPUT   1

/* GPIO Setting */
#define ITV_S5H1411_GPIO_OFF 0
#define ITV_S5H1411_GPIO_ON  1

/* MPEG signal timing */
#define ITV_S5H1411_MPEGTIMING_CONTINUOUS_INVERTING_CLOCK       0
#define ITV_S5H1411_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK    1
#define ITV_S5H1411_MPEGTIMING_NONCONTINUOUS_INVERTING_CLOCK    2
#define ITV_S5H1411_MPEGTIMING_NONCONTINUOUS_NONINVERTING_CLOCK 3

/* IF Freq for QAM and VSB in KHz */
#define ITV_S5H1411_IF_3250  		3250
#define ITV_S5H1411_IF_3500  		3500
#define ITV_S5H1411_IF_4000  		4000
#define ITV_S5H1411_IF_5380  		5380
#define ITV_S5H1411_IF_44000 		44000
#define ITV_S5H1411_VSB_IF_DEFAULT 	ITV_S5H1411_IF_44000
#define ITV_S5H1411_QAM_IF_DEFAULT 	ITV_S5H1411_IF_44000

/* Spectral Inversion */
#define ITV_S5H1411_INVERSION_OFF 0
#define ITV_S5H1411_INVERSION_ON  1

/* Return lock status based on tuner lock, or demod lock */
#define ITV_S5H1411_TUNERLOCKING 0
#define ITV_S5H1411_DEMODLOCKING 1

typedef struct itv_s5h1411_config_t {
	/* serial/parallel output */
	unsigned char output_mode;
	/* GPIO Setting */
	unsigned char gpio;
	/* MPEG signal timing */
	unsigned short mpeg_timing;
	/* IF Freq for QAM and VSB in KHz */
	unsigned short qam_if;
	unsigned short vsb_if;
	/* Spectral Inversion */
	unsigned char inversion;
	/* Return lock status based on tuner lock, or demod lock */
	unsigned char status_mode;
} itv_s5h1411_config_t;

#endif	/* __IS5H1411_H__ */
