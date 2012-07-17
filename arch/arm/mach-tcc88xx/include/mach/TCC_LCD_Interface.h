#ifndef	_TCC_LCD_INTERFACE_H_
#define	_TCC_LCD_INTERFACE_H_



extern void LCDC_IO_Set (char lcdctrl_num, unsigned bit_per_pixel);

extern void LCDC_IO_Disable (char lcdctrl_num, unsigned bit_per_pixel);


typedef enum{
	LCD_GPSB_PORT0 = 0,
	LCD_GPSB_PORT1,
	LCD_GPSB_PORT2,
	LCD_GPSB_PORT3,
	LCD_GPSB_PORT4,
	LCD_GPSB_PORT5,
	LCD_GPSB_PORT6,
	LCD_GPSB_PORT7,
	LCD_GPSB_PORT8,
	LCD_GPSB_PORT9,
	LCD_GPSB_PORT10,
	LCD_GPSB_PORT11,
	LCD_GPSB_PORT12,
	LCD_GPSB_PORTMAX
}DEV_LCD_GPSB_PORT;


typedef struct{
	unsigned int clock;
	unsigned int mode;
	DEV_LCD_GPSB_PORT port;
	unsigned int data_length;
}DEV_LCD_SPIO_Config_type;


/* -------------------------------------------------
GPSB Interface 사용
clock : GPSB clock  //100hz 단위.

---------------------------------------------------*/
extern void DEV_LCD_SPIO_Config(DEV_LCD_SPIO_Config_type *Lcd_gpsg);


extern unsigned int DEV_LCD_SPIO_TxRxData(void *pTxBuf, void *pRxBuf, unsigned uLength);

#endif //_TCC_LCD_INTERFACE_H_

