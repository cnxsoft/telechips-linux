
/****************************************************************************
 *   FileName    : TCC_LCD_interfface.c
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
/*****************************************************************************
*
* Header Files Include
*
******************************************************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/clk.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>
#include <asm/mach/map.h>
#ifdef CONFIG_PM
#include <linux/pm.h>
#endif

#include <mach/bsp.h>
#include <mach/tca_ckc.h>
#include <mach/tcc_fb.h>
#include <mach/TCC_LCD_Interface.h>


#if 1
#define dprintk(msg...)	 { printk( "LCD_interface: " msg); }
#else
#define dprintk(msg...)	 
#endif




void DEV_Lcd_PortSet(unsigned int bit_per_pixel)
{	

	PGPION	pGPIO_C = (volatile PGPION)tcc_p2v(HwGPIOC_BASE);

	BITCSET(pGPIO_C->GPEN, Hw24|Hw25|Hw26|Hw27|Hw28|Hw29|Hw30|Hw31, Hw24|Hw25|Hw26|Hw27|Hw28|Hw29|Hw30|Hw31);
	BITCSET(pGPIO_C->GPDAT, Hw24|Hw25|Hw26|Hw27|Hw28|Hw29|Hw30|Hw31, Hw24|Hw25|Hw26|Hw27|Hw28|Hw29|Hw30|Hw31);


	switch (bit_per_pixel) 
	{
		case 24:
			BITCSET(pGPIO_C->GPEN, Hw23|Hw22|Hw21|Hw20|Hw19|Hw18, Hw23|Hw22|Hw21|Hw20|Hw19|Hw18);
		case 18:
			BITCSET(pGPIO_C->GPEN, Hw17|Hw16, Hw17|Hw16);


		case 16:
			BITCSET(pGPIO_C->GPEN, 0x0000FFFF, 0x0000FFFF);
				break;
			
		default:
			// do nothing
			break;
	}
}

void LCDC_IO_Set (char lcdctrl_num, unsigned bit_per_pixel)
{
	char LCD_PORT_ENABLE;
	PGPION	pGPIO_C = (volatile PGPION)tcc_p2v(HwGPIOC_BASE);

	dprintk("%s lcdctrl_num:%d bit_per_pixel:%d ~ \n", __func__, lcdctrl_num, bit_per_pixel);
	if(lcdctrl_num)
		LCD_PORT_ENABLE = 5;
	else
		LCD_PORT_ENABLE = 2;

#ifdef CONFIG_LCD_CPU_INTERFACE
	DEV_Lcd_PortSet(bit_per_pixel);
	LCD_PORT_ENABLE = 1;
#endif//_CONFIG_LCD_CPU_INTERFACE_	

	BITCSET(pGPIO_C->GPFN3, HwPORTCFG_GPFN0_MASK | HwPORTCFG_GPFN1_MASK | HwPORTCFG_GPFN2_MASK | HwPORTCFG_GPFN3_MASK,
							HwPORTCFG_GPFN0(LCD_PORT_ENABLE) | HwPORTCFG_GPFN1(LCD_PORT_ENABLE) | HwPORTCFG_GPFN2(LCD_PORT_ENABLE) | 
							HwPORTCFG_GPFN3(LCD_PORT_ENABLE));

	switch (bit_per_pixel) {
		case 24:
			BITCSET(pGPIO_C->GPFN2, HwPORTCFG_GPFN2_MASK | HwPORTCFG_GPFN3_MASK | HwPORTCFG_GPFN4_MASK | HwPORTCFG_GPFN5_MASK |
									HwPORTCFG_GPFN6_MASK | HwPORTCFG_GPFN7_MASK,
									HwPORTCFG_GPFN2(LCD_PORT_ENABLE) | HwPORTCFG_GPFN3(LCD_PORT_ENABLE) | HwPORTCFG_GPFN4(LCD_PORT_ENABLE) | 
									HwPORTCFG_GPFN5(LCD_PORT_ENABLE) | HwPORTCFG_GPFN6(LCD_PORT_ENABLE) | HwPORTCFG_GPFN7(LCD_PORT_ENABLE));
		case 18:
			BITCSET(pGPIO_C->GPFN2, HwPORTCFG_GPFN0_MASK | HwPORTCFG_GPFN1_MASK,
									HwPORTCFG_GPFN0(LCD_PORT_ENABLE) | HwPORTCFG_GPFN1(LCD_PORT_ENABLE));

		case 16:
			BITCSET(pGPIO_C->GPFN1, HwPORTCFG_GPFN0_MASK | HwPORTCFG_GPFN1_MASK | HwPORTCFG_GPFN2_MASK | HwPORTCFG_GPFN3_MASK | 
									HwPORTCFG_GPFN4_MASK | HwPORTCFG_GPFN5_MASK | HwPORTCFG_GPFN6_MASK | HwPORTCFG_GPFN7_MASK,
									HwPORTCFG_GPFN0(LCD_PORT_ENABLE) | HwPORTCFG_GPFN1(LCD_PORT_ENABLE) | HwPORTCFG_GPFN2(LCD_PORT_ENABLE) | 
									HwPORTCFG_GPFN3(LCD_PORT_ENABLE) | HwPORTCFG_GPFN4(LCD_PORT_ENABLE) | HwPORTCFG_GPFN5(LCD_PORT_ENABLE) | 
									HwPORTCFG_GPFN6(LCD_PORT_ENABLE) | HwPORTCFG_GPFN7(LCD_PORT_ENABLE));
		case 8:
			BITCSET(pGPIO_C->GPFN0, HwPORTCFG_GPFN0_MASK | HwPORTCFG_GPFN1_MASK | HwPORTCFG_GPFN2_MASK | HwPORTCFG_GPFN3_MASK | 
									HwPORTCFG_GPFN4_MASK | HwPORTCFG_GPFN5_MASK | HwPORTCFG_GPFN6_MASK | HwPORTCFG_GPFN7_MASK,
									HwPORTCFG_GPFN0(LCD_PORT_ENABLE) | HwPORTCFG_GPFN1(LCD_PORT_ENABLE) | HwPORTCFG_GPFN2(LCD_PORT_ENABLE) | 
									HwPORTCFG_GPFN3(LCD_PORT_ENABLE) | HwPORTCFG_GPFN4(LCD_PORT_ENABLE) | HwPORTCFG_GPFN5(LCD_PORT_ENABLE) | 
									HwPORTCFG_GPFN6(LCD_PORT_ENABLE) | HwPORTCFG_GPFN7(LCD_PORT_ENABLE));
			break;
			
		default:
			// do nothing
			break;
	}

}

#define LCD_PORT_DISABLE	0	
void LCDC_IO_Disable (char lcdctrl_num, unsigned bit_per_pixel)
{
	PGPION	pGPIO_C = (volatile PGPION)tcc_p2v(HwGPIOC_BASE);

	BITCSET(pGPIO_C->GPFN3, HwPORTCFG_GPFN0_MASK | HwPORTCFG_GPFN1_MASK | HwPORTCFG_GPFN2_MASK | HwPORTCFG_GPFN3_MASK,
							HwPORTCFG_GPFN0(LCD_PORT_DISABLE) | HwPORTCFG_GPFN1(LCD_PORT_DISABLE) | HwPORTCFG_GPFN2(LCD_PORT_DISABLE) | 
							HwPORTCFG_GPFN3(LCD_PORT_DISABLE));

	BITCSET(pGPIO_C->GPEN, Hw24|Hw25|Hw26|Hw27, Hw24|Hw25|Hw26|Hw27);
	BITCLR(pGPIO_C->GPDAT, Hw24|Hw25|Hw26|Hw27);


	switch (bit_per_pixel) {
		case 24:
			BITCSET(pGPIO_C->GPFN2, HwPORTCFG_GPFN2_MASK | HwPORTCFG_GPFN3_MASK | HwPORTCFG_GPFN4_MASK | HwPORTCFG_GPFN5_MASK |
									HwPORTCFG_GPFN6_MASK | HwPORTCFG_GPFN7_MASK,
									HwPORTCFG_GPFN2(LCD_PORT_DISABLE) | HwPORTCFG_GPFN3(LCD_PORT_DISABLE) | HwPORTCFG_GPFN4(LCD_PORT_DISABLE) | 
									HwPORTCFG_GPFN5(LCD_PORT_DISABLE) | HwPORTCFG_GPFN6(LCD_PORT_DISABLE) | HwPORTCFG_GPFN7(LCD_PORT_DISABLE));

			BITCSET(pGPIO_C->GPEN, Hw23|Hw22|Hw21|Hw20|Hw19|Hw18, Hw23|Hw22|Hw21|Hw20|Hw19|Hw18);
			BITCLR(pGPIO_C->GPDAT, Hw23|Hw22|Hw21|Hw20|Hw19|Hw18);

		case 18:
			BITCSET(pGPIO_C->GPFN2, HwPORTCFG_GPFN0_MASK | HwPORTCFG_GPFN1_MASK,
									HwPORTCFG_GPFN0(LCD_PORT_DISABLE) | HwPORTCFG_GPFN1(LCD_PORT_DISABLE));

			BITCSET(pGPIO_C->GPEN, Hw17|Hw16, Hw17|Hw16);
			BITCLR(pGPIO_C->GPDAT, Hw17|Hw16);
	
		case 16:
			BITCSET(pGPIO_C->GPFN1, HwPORTCFG_GPFN0_MASK | HwPORTCFG_GPFN1_MASK | HwPORTCFG_GPFN2_MASK | HwPORTCFG_GPFN3_MASK | 
									HwPORTCFG_GPFN4_MASK | HwPORTCFG_GPFN5_MASK | HwPORTCFG_GPFN6_MASK | HwPORTCFG_GPFN7_MASK,
									HwPORTCFG_GPFN0(LCD_PORT_DISABLE) | HwPORTCFG_GPFN1(LCD_PORT_DISABLE) | HwPORTCFG_GPFN2(LCD_PORT_DISABLE) | 
									HwPORTCFG_GPFN3(LCD_PORT_DISABLE) | HwPORTCFG_GPFN4(LCD_PORT_DISABLE) | HwPORTCFG_GPFN5(LCD_PORT_DISABLE) | 
									HwPORTCFG_GPFN6(LCD_PORT_DISABLE) | HwPORTCFG_GPFN7(LCD_PORT_DISABLE));
			BITCSET(pGPIO_C->GPFN0, HwPORTCFG_GPFN0_MASK | HwPORTCFG_GPFN1_MASK | HwPORTCFG_GPFN2_MASK | HwPORTCFG_GPFN3_MASK | 
									HwPORTCFG_GPFN4_MASK | HwPORTCFG_GPFN5_MASK | HwPORTCFG_GPFN6_MASK | HwPORTCFG_GPFN7_MASK,
									HwPORTCFG_GPFN0(LCD_PORT_DISABLE) | HwPORTCFG_GPFN1(LCD_PORT_DISABLE) | HwPORTCFG_GPFN2(LCD_PORT_DISABLE) | 
									HwPORTCFG_GPFN3(LCD_PORT_DISABLE) | HwPORTCFG_GPFN4(LCD_PORT_DISABLE) | HwPORTCFG_GPFN5(LCD_PORT_DISABLE) | 
									HwPORTCFG_GPFN6(LCD_PORT_DISABLE) | HwPORTCFG_GPFN7(LCD_PORT_DISABLE));

			BITCSET(pGPIO_C->GPEN, 0x0000FFFF, 0x0000FFFF);
			BITCLR(pGPIO_C->GPDAT, 0x0000FFFF);
			break;

		default:
			// do nothing
			break;
	}
	
	
}




#include <mach/dev_gpsb.h>
static sDRV_GPSB gDRV_SPI_LCD;


void GPSB_SETConfig(sDRV_GPSB * pDRV_GPSB, int iCH, int iPort)
{
	GPSB*	pHwGPSB;		// Base Address

	U32	uMode, uSCKFreq, uSwCtrl, uSDOSDI, uDIV;
			// Get Channel

	pDRV_GPSB->CHSTS		= DRV_GPSB_STATUS_IDLE;
	pDRV_GPSB->CHANNEL		= iCH;
	pDRV_GPSB->PORT			= iPort;
	pDRV_GPSB->pHwGPSB		= IO_GPSB_GetBaseAddr(iCH);

	pHwGPSB		= pDRV_GPSB->pHwGPSB;

	uMode		= pDRV_GPSB->MODE;
	uSCKFreq	= pDRV_GPSB->SCKFREQ;
	uSwCtrl		= pDRV_GPSB->SWCTRL;
	uSDOSDI		= pDRV_GPSB->SDOSDI;

		//Get and Set the Divider value
	if(uSCKFreq)
	{
		uDIV = 900000 / (2*uSCKFreq);
		if (uDIV > 0)
			uDIV--;
		uMode 				|= uDIV*Hw24;
		pDRV_GPSB->MODE 	= uMode;
	}

	//Initialize GPSB Channel
	IO_GPSB_InitChannel(pDRV_GPSB->CHANNEL, pDRV_GPSB->PORT, uMode, uSwCtrl, uSDOSDI);

	//Set the MODE Register
	pHwGPSB->MODE = uMode;

	//Set Handle Status
	pDRV_GPSB->CHSTS = DRV_GPSB_STATUS_ACTIVE;
}


/* -------------------------------------------------
clock : GPSB clock  //100hz ´ÜÀ§.

---------------------------------------------------*/
void DEV_LCD_SPIO_Config(DEV_LCD_SPIO_Config_type *Lcd_gpsg)
{
	dprintk(KERN_INFO "%s :  \n", __FUNCTION__);

	gDRV_SPI_LCD.MODE = Lcd_gpsg->mode;
	
	gDRV_SPI_LCD.SCKFREQ = Lcd_gpsg->clock;
	gDRV_SPI_LCD.SWCTRL = DRV_GPSB_CTRL_HW;
	gDRV_SPI_LCD.SDOSDI = DRV_GPSB_SDOSDI_ALLUSE;

	GPSB_SETConfig(&gDRV_SPI_LCD, DRV_GPSB_CH1, Lcd_gpsg->port);

}
EXPORT_SYMBOL(DEV_LCD_SPIO_Config);

unsigned int DEV_LCD_SPIO_TxRxData(void *pTxBuf, void *pRxBuf, unsigned uLength)
{
	unsigned int ret;


	ret= IO_GPSB_TxRxData(DRV_GPSB_CH1, pTxBuf, pRxBuf, uLength);
	return ret;
}
EXPORT_SYMBOL(DEV_LCD_SPIO_TxRxData);


