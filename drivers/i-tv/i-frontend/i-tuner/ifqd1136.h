#ifndef __IFQD1136_H__
#define __IFQD1136_H__

#define ITV_FQD1136_TUNER_RESET		0

#define FQD_FREQ_LOW_BAND			1
#define FQD_FREQ_MID_BAND			2
#define FQD_FREQ_HIGH_BAND			4

#define FQD_AGC_115dBuV				1
#define FQD_AGC_112dBuV				2
#define FQD_AGC_109dBuV				3
#define FQD_AGC_106dBuV				4
#define FQD_AGC_103dBuV				5
#define FQD_AGC_EXTERNAL			6
#define FQD_AGC_DISABLE				7

#define FQD_PLL_CP_50uA				6
#define FQD_PLL_CP_125uA			7
#define FQD_PLL_CP_250uA			0x0e
#define FQD_PLL_CP_650uA			0x0f

#define FQD_PLLref_DIVIDER_64		3
#define FQD_PLLref_DIVIDER_24		2

/* Result codes */
#define FQD_RESULT_SUCCESS				0
#define FQD_RESULT_RESET_FAILURE		1
#define FQD_RESULT_I2C_WRITE_FAILURE	2
#define FQD_RESULT_I2C_READ_FAILURE		3
#define FQD_RESULT_OUT_OF_RANGE			5

/* Product id */
#define FQD_INIT_STATE		0xC940

/* Tuner standards */
#define DVBT_6MHz			2
#define ATSC				1
#define NTSC				0

typedef struct itv_fqd1136_config_t {
	unsigned char	i2c_address;
	unsigned int	if_freq;
} itv_fqd1136_config_t;

#endif	/* __IFQD1136_H__ */
