/****************************************************************************
 *   FileName    : tca_sc.c
 *   Description :
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/

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
#include <linux/cpufreq.h>
#ifdef CONFIG_PM
#include <linux/pm.h>
#endif
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif
#include <linux/timer.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>
#include <asm/mach/map.h>

#include <mach/bsp.h>
#include <mach/tca_ckc.h>
#include <mach/globals.h>

#include <mach/tcc_sc_ioctl.h>
#include <mach/tcc_pca953x.h>
#include <mach/gpio.h>

#define HwINT1_UART 		Hw15		// R/W, UART interrupt enable

#define HwUART_CH_BASE(X)	(HwUART0_BASE+(X)*0x10000)
#define SC_UART_ADDR(x)		HwUART_CH_BASE(x)

static sDRV_UART_BUF gDRV_UART_BUF[DRV_UART_PORTMAX];
static sTCC_SC_PARAMS g_SCParams;

static int debug = 0;
#define dprintk(msg...)	if(debug) { printk( "tca_sc: " msg); }

static struct clk *gSC_clk;		// uart clock

/* gpio to interface ic card */
static struct _sc_io {
	int	rst;
	int	detect;
	int	pwren;
	int	set_volt;
	int	clk;
	int	data;
} sc_io;

#define	SC_DETECT_FILTER_MAX	5
#define	SC_DETECT_TIME	10
struct _sc_detect {
	int	state;
	int	filter[SC_DETECT_FILTER_MAX];
	int	filter_idx;
	struct timer_list sc_timer;
} sc_detect;

void sc_detect_timer_handler (unsigned long data);
int sc_detect_get_state (void);
int tca_sc_enable(void);
int tca_sc_disable(void);
int	sc_enable_flag=0;

sDRV_UART gDRV_UART_SC = 
{
	0									// UART Baud Rate
	, 0									// RX Interrupt Enable
	| DRV_UART_IER_EnableRXI
	, 0									// FCR Options
	| DRV_UART_FCR_TxWindowLvl_01
	| DRV_UART_FCR_TxFifoReset 
	| DRV_UART_FCR_RxFifoReset	
	| DRV_UART_FCR_EnableFifo
	, 0									// LCR Options
	| DRV_UART_LCR_EnableEvenParity	
	| DRV_UART_LCR_EnableParity	
	| DRV_UART_LCR_OneStopBit 
	| DRV_UART_LCR_WordLength_8Bit
	, 0 								// AFT Options
	, 0 								// UART Frequency (Fuart) - unit: 100Hz
	, DRV_UART_SCCTRL_ON 				// Smart Card Control
	, 0 								// Smart Card Clock - unit: 100Hz
	, 0									// UART Status
	, DRV_UART_PORT4					// UART Port
};

static unsigned gSCClkFactor[16] = 
{
	DRV_UART_SCCLK_FACTOR_0, DRV_UART_SCCLK_FACTOR_1, DRV_UART_SCCLK_FACTOR_2, DRV_UART_SCCLK_FACTOR_3,
	DRV_UART_SCCLK_FACTOR_4, DRV_UART_SCCLK_FACTOR_5, DRV_UART_SCCLK_FACTOR_6, DRV_UART_SCCLK_FACTOR_7,
	DRV_UART_SCCLK_FACTOR_8, DRV_UART_SCCLK_FACTOR_9, DRV_UART_SCCLK_FACTOR_A, DRV_UART_SCCLK_FACTOR_B,
	DRV_UART_SCCLK_FACTOR_C, DRV_UART_SCCLK_FACTOR_D, DRV_UART_SCCLK_FACTOR_E, DRV_UART_SCCLK_FACTOR_F
};

static int uart_port_map[24][2] = {
	{TCC_GPA(7), GPIO_FN(7)},     // UT_TXD(0)
	{TCC_GPA(13), GPIO_FN(7)},   // UT_TXD(1)
	{TCC_GPD(4), GPIO_FN(7)},     // UT_TXD(2)
	{TCC_GPD(11), GPIO_FN(7)},   // UT_TXD(3)
	{TCC_GPD(17), GPIO_FN(7)},   // UT_TXD(4)
	{TCC_GPB(0), GPIO_FN(10)},   // UT_TXD(5)
	{TCC_GPB(11), GPIO_FN(10)},	// UT_TXD(6)
	{TCC_GPB(19), GPIO_FN(10)}, // UT_TXD(7)
	{TCC_GPB(25), GPIO_FN(10)},       // UT_TXD(8)
	{TCC_GPC(0), GPIO_FN(6)},     // UT_TXD(9)
	{TCC_GPC(10), GPIO_FN(6)},     // UT_TXD(10)
	{TCC_GPC(23), GPIO_FN(6)},     // UT_TXD(11)
	{TCC_GPE(12), GPIO_FN(5)},     // UT_TXD(12)
	{TCC_GPE(28), GPIO_FN(5)},     // UT_TXD(13)
	{TCC_GPF(13), GPIO_FN(9)},     // UT_TXD(14) *
	{TCC_GPF(17), GPIO_FN(9)},     // UT_TXD(15)
	{TCC_GPF(25), GPIO_FN(9)},     // UT_TXD(16)
	{TCC_GPG(0), GPIO_FN(3)},     // UT_TXD(17)
	{TCC_GPG(4), GPIO_FN(3)},     // UT_TXD(18)
	{TCC_GPG(8), GPIO_FN(3)},     // UT_TXD(19)
	{TCC_GPG(14), GPIO_FN(3)},     // UT_TXD(20) *
	{TCC_GPG(18), GPIO_FN(3)},  	     // UT_TXD(21) *
	{TCC_GPHDMI(0), GPIO_FN(3)},     // UT_TXD(22)		
	{TCC_GPADC(2), GPIO_FN(4)},     // UT_TXD(23)
};

static unsigned short gCWT; //microsecond
static unsigned short gBWT; //microsecond
static int iIrqData;

void tca_sc_pwren(int on)
{
	if (sc_io.pwren != -1) {
#if defined(CONFIG_MACH_TCC8920) || defined(CONFIG_MACH_TCC8920ST)
		//control CMDVCC# for TDA8024
		if (on) {
			gpio_direction_output (sc_io.pwren, 0);
			gpio_set_value (sc_io.pwren, 0);
		}
		else	{
			gpio_direction_output (sc_io.pwren, 1);
			gpio_set_value (sc_io.pwren, 1);
		}
#elif defined(CONFIG_MACH_M805_892X)
		//control LDO
		if (on) {
			gpio_direction_output (sc_io.pwren, 1);
			gpio_set_value (sc_io.pwren, 1);
		}
		else	{
			gpio_direction_output (sc_io.pwren, 0);
			gpio_set_value (sc_io.pwren, 0);
		}
#endif
	}
}

void tca_sc_init_port(void)
{
	//init all variables at first.
	sc_io.rst = sc_io.detect = sc_io.pwren = sc_io.set_volt = sc_io.clk = sc_io.data = -1;

#ifdef CONFIG_MACH_TCC8920
	sc_io.rst = TCC_GPEXT2(6);		//active-low
	sc_io.detect = TCC_GPB(16);		//active-high
	sc_io.pwren = TCC_GPEXT2(7);		//active-low
	sc_io.set_volt = -1;
	sc_io.clk = TCC_GPC(24);
	sc_io.data = TCC_GPC(23);
#elif defined (CONFIG_MACH_TCC8920ST)
	sc_io.rst = TCC_GPB(27);
	sc_io.detect = TCC_GPG(16);
	sc_io.pwren = TCC_GPB(20);
	sc_io.set_volt = -1;
	sc_io.clk = TCC_GPB(26);
	sc_io.data = TCC_GPB(25);
#elif defined (CONFIG_MACH_M805_892X)
	if (system_rev == 0x2005) {
		sc_io.rst = TCC_GPC(12);	//active-low
		sc_io.detect = TCC_GPG(4);	//active-high
		sc_io.pwren = TCC_GPG(5);	//active-high
		sc_io.set_volt = -1;
		sc_io.clk = TCC_GPC(11);
		sc_io.data = TCC_GPC(10);
	}
#endif

	if (sc_io.clk != -1) gpio_request (sc_io.clk, NULL);
	if (sc_io.data != -1) gpio_request (sc_io.data, NULL);
	if (sc_io.pwren != -1) gpio_request (sc_io.pwren, NULL);
	if (sc_io.rst != -1) gpio_request (sc_io.rst, NULL);
	if (sc_io.detect != -1) gpio_request (sc_io.detect, NULL);

	if (sc_io.pwren != -1) {
	#if defined(CONFIG_MACH_TCC8920) || defined(CONFIG_MACH_TCC8920ST)
		tcc_gpio_config(sc_io.pwren, GPIO_FN(0)|GPIO_OUTPUT|GPIO_HIGH);
		gpio_direction_output(sc_io.pwren, 1);
	#elif defined(CONFIG_MACH_M805_892X)
		tcc_gpio_config(sc_io.pwren, GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);
	#endif
	}

	if (sc_io.rst != -1) {
		tcc_gpio_config(sc_io.rst, GPIO_FN(0)|GPIO_INPUT|GPIO_PULLDOWN);
	#if defined(CONFIG_MACH_TCC8920) || defined(CONFIG_MACH_TCC8920ST)
		gpio_direction_input (sc_io.rst);
	#endif
	}
	if (sc_io.detect != -1) tcc_gpio_config(sc_io.detect, GPIO_FN(0)|GPIO_INPUT); 
	if (sc_io.clk != -1) tcc_gpio_config(sc_io.clk, GPIO_FN(0)|GPIO_INPUT|GPIO_PULLDOWN);
	if (sc_io.data != -1) tcc_gpio_config(sc_io.data, GPIO_FN(0)|GPIO_INPUT|GPIO_PULLDOWN);

	// VCC Voltage Selection - High:5V, Low:3V
}

void tca_sc_init_buffer(unsigned uPort, unsigned uTxRx)
{
	sDRV_UART_BUF	*pUartBuf;

	dprintk("%s\n", __func__);

	//Get the Pointer of Buffer
	pUartBuf = &gDRV_UART_BUF[uPort];

	//Initialize the Starting Point
	if((uTxRx == DRV_UART_BUFFER_TX)||(uTxRx == DRV_UART_BUFFER_TXRX))
	{
		pUartBuf->usTXHead = pUartBuf->usTXTail = 0;
		memset(pUartBuf->sTX_Buffer, 0x00, IO_UART_TX_BUFFER_SIZE);
	}
	if((uTxRx == DRV_UART_BUFFER_RX)||(uTxRx == DRV_UART_BUFFER_TXRX))
	{
		pUartBuf->usRXHead = pUartBuf->usRXTail = 0;
		memset(pUartBuf->sRX_Buffer, 0x00, IO_UART_RX_BUFFER_SIZE);
	}
}

void tca_sc_rx_push_char(unsigned uPort, char cChar)
{
	sDRV_UART_BUF *pUartBuf = &gDRV_UART_BUF[uPort];

	pUartBuf->sRX_Buffer[pUartBuf->usRXHead++] = cChar;
	pUartBuf->usRXHead %= IO_UART_RX_BUFFER_SIZE;

	//dprintk("p\n");
}

unsigned tca_sc_rx_pop_char(unsigned uPort)
{
	sDRV_UART_BUF 	*pUartBuf = &gDRV_UART_BUF[uPort];
	unsigned 		uBufData;

	if (pUartBuf->usRXHead != pUartBuf->usRXTail)
	{
		uBufData = pUartBuf->sRX_Buffer[pUartBuf->usRXTail++];
		pUartBuf->usRXTail %= IO_UART_RX_BUFFER_SIZE;
		return uBufData;
	}

	return 0x100;
}

void tca_sc_make_reset(unsigned uReset)
{
	dprintk("%s uReset= %d \n", __func__, uReset);

	if(uReset) {
		if (sc_io.rst != -1) {
			gpio_direction_output (sc_io.rst, 1);
			gpio_set_value (sc_io.rst, 1);
		}
	} else {
		if (sc_io.rst != -1) {
			gpio_direction_output (sc_io.rst, 0);
			gpio_set_value (sc_io.rst, 0);
		}
	}
}

void tca_sc_make_activate(unsigned uActivate)
{
	dprintk("%s, uActivate=%d\n", __func__, uActivate);
	if(uActivate)
	{
		tca_sc_pwren(1);
		if (sc_io.rst != -1) gpio_direction_output (sc_io.rst, 1);
	}
	else
	{
		if (sc_io.rst != -1) gpio_direction_input (sc_io.rst);
		tca_sc_pwren(0);
	}
}

void tca_sc_select_voltage(unsigned uVoloate_3V)
{
	dprintk("%s\n", __func__);

	if(uVoloate_3V)
	{
	}
	else
	{
	}
}

unsigned tca_sc_detect_card(void)
{
	int	ret;
#if defined(CONFIG_MACH_TCC8920) || defined(CONFIG_MACH_TCC8920ST)
	ret = gpio_get_value (sc_io.detect);
	if (ret) {
		sc_detect.state = 1;
		tca_sc_pwren(1);
		tca_sc_make_reset(1);
		tca_sc_enable();
	} else {
		sc_detect.state = 0;
		tca_sc_disable();
		tca_sc_make_activate(0);
		tca_sc_pwren(0);
	}
#elif defined(CONFIG_MACH_M805_892X)
	ret = sc_detect.state;
#endif
	return ret;
}

void tca_sc_ctrl_txport(unsigned uPort, unsigned uEnable)
{
	UART *pHwUART = (volatile PUART)tcc_p2v(SC_UART_ADDR(uPort));
	
	dprintk("%s\n", __func__);
	
	//Control Direction of SmartCard TX Port
	if(uEnable)
	{
		BITSET(pHwUART->SCCR.nREG, HwUART_SCCR_STEN_EN);
	}
	else
	{
		BITCLR(pHwUART->SCCR.nREG, HwUART_SCCR_STEN_EN);
	}
}

unsigned tca_sc_receive_byte(unsigned int uPort)
{
	UART *pHwUART = (volatile PUART)tcc_p2v(SC_UART_ADDR(uPort));
	unsigned uByte;

	//Wait until Data is Ready
	while(ISZERO(pHwUART->LSR.nREG, HwUART_LSR_DR_ON));
	
	uByte = pHwUART->REG1.UTRXD;

	return uByte;
}

void tca_sc_send_byte(unsigned int uPort, unsigned uChar)
{ 
	UART *pHwUART = (volatile PUART)tcc_p2v(SC_UART_ADDR(uPort));

	//Wait util THR Empty
	while (ISZERO(pHwUART->LSR.nREG, HwUART_LSR_THRE_ON));

	//Send a Byte
	pHwUART->REG1.UTTXD = uChar;
}

void tca_sc_get_atr(unsigned char *pATRData, unsigned *pATRLength)
{
	unsigned char 	*pucBuffer 	= pATRData;
	unsigned 		uLength 	= 0;
	unsigned		uTimeOut 	= 10;//30;
	unsigned		uTimeCount 	= 10;//30;
	unsigned 		uData 		= 0;
	unsigned char	uPort			= gDRV_UART_SC.PORT;

	dprintk("%s uPort =%d\n", __func__, uPort);
	
	//Init Tx/Rx buffer
	tca_sc_init_buffer(uPort, DRV_UART_BUFFER_TXRX);

	tca_sc_make_reset(0);
	msleep(2);
	tca_sc_make_reset(1);

	//Disable SmartCard TX
	tca_sc_ctrl_txport(uPort, DISABLE);
	
	//Get ATR Response
	while (1)
	{
		uData = tca_sc_rx_pop_char(uPort);
		if(uData < 0x100)
		{
			*pucBuffer++ = uData;
			uLength++;
			uTimeOut = uTimeCount;
		}
		else
		{
			msleep(5);
			uTimeOut--;
			if(uTimeOut == 0)
				break;
		}
//		dprintk("%s uData = %x \n", __func__, uData);
	}

	//Set the ATR Length
	*pATRLength = uLength;
	
	//Set the Last Byte
	*(pATRData+uLength) 	= 0;

	dprintk("%s uLength = %d \n", __func__, uLength);
	
}

int tca_sc_parse_atr(unsigned char *pATRData, unsigned uATRLength, sTCC_SC_ATR_CHARS *pstATRParse)
{
	int i;
	int iProtocol = 0;
	unsigned char uCount = 0;
	unsigned char uHCharNum = 0;

	dprintk("%s\n", __func__);
	
	memset(pstATRParse, 0xFF, sizeof(sTCC_SC_ATR_CHARS));

	pstATRParse->ucTS = *(pATRData + uCount++);
	pstATRParse->ucT0 = *(pATRData + uCount++);

	uHCharNum = pstATRParse->ucT0 & 0x0F;

	//Get TA1, TB1, TC1, TD1 Characters
	if(pstATRParse->ucT0 & TA_CHECK_BIT)
		pstATRParse->usTA[0] = *(pATRData + uCount++);
	if(pstATRParse->ucT0 & TB_CHECK_BIT)
		pstATRParse->usTB[0] = *(pATRData + uCount++);
	if(pstATRParse->ucT0 & TC_CHECK_BIT)
		pstATRParse->usTC[0] = *(pATRData + uCount++);
	if(pstATRParse->ucT0 & TD_CHECK_BIT)
		pstATRParse->usTD[0] = *(pATRData + uCount++);

	//Get TA2, TB2, TC2, TD2 Characters
	if(pstATRParse->usTD[0] != 0xFFFF)
	{
		if(pstATRParse->usTD[0] & TA_CHECK_BIT)
			pstATRParse->usTA[1] = *(pATRData + uCount++);
		if(pstATRParse->usTD[0] & TB_CHECK_BIT)
			pstATRParse->usTB[1] = *(pATRData + uCount++);
		if(pstATRParse->usTD[0] & TC_CHECK_BIT)
			pstATRParse->usTC[1] = *(pATRData + uCount++);
		if(pstATRParse->usTD[0] & TD_CHECK_BIT)
			pstATRParse->usTD[1] = *(pATRData + uCount++);

		iProtocol = pstATRParse->usTD[0] & 0x0F;
	}
		
	//Get TA3, TB3, TC3, TD3 Characters
	if(pstATRParse->usTD[1] != 0xFFFF)
	{
		if(pstATRParse->usTD[1] & TA_CHECK_BIT)
			pstATRParse->usTA[2] = *(pATRData + uCount++);
		if(pstATRParse->usTD[1] & TB_CHECK_BIT)
			pstATRParse->usTB[2] = *(pATRData + uCount++);
		if(pstATRParse->usTD[1] & TC_CHECK_BIT)
			pstATRParse->usTC[2] = *(pATRData + uCount++);
		if(pstATRParse->usTD[1] & TD_CHECK_BIT)
			pstATRParse->usTD[2] = *(pATRData + uCount++);

		iProtocol = pstATRParse->usTD[1] & 0x0F;
	}
		
	//Get TA4, TB4, TC4, TD4 Characters
	if(pstATRParse->usTD[2] != 0xFFFF)
	{
		if(pstATRParse->usTD[2] & TA_CHECK_BIT)
			pstATRParse->usTA[3] = *(pATRData + uCount++);
		if(pstATRParse->usTD[2] & TB_CHECK_BIT)
			pstATRParse->usTB[3] = *(pATRData + uCount++);
		if(pstATRParse->usTD[2] & TC_CHECK_BIT)
			pstATRParse->usTC[3] = *(pATRData + uCount++);
		if(pstATRParse->usTD[2] & TD_CHECK_BIT)
			pstATRParse->usTD[3] = *(pATRData + uCount++);
	}

	//Get Historical Characters
	for(i=0; i<uHCharNum; i++)
	{
		pstATRParse->ucHC[i] = *(pATRData + uCount++);
	}

	//Get TCK Characters
	if(iProtocol != 0)
	{
		pstATRParse->ucTCK = *(pATRData + uCount++);
	}

	#if 0 // Test Log
	printk("TS:0x%02x\n", pstATRParse->ucTS);
	printk("TO:0x%02x\n", pstATRParse->ucT0);
	printk("TA1:0x%04x, TB1:0x%04x, TC1:0x%04x, TD1:0x%04x\n", pstATRParse->usTA[0], pstATRParse->usTB[0], pstATRParse->usTC[0], pstATRParse->usTD[0]);
	printk("TA2:0x%04x, TB2:0x%04x, TC2:0x%04x, TD2:0x%04x\n", pstATRParse->usTA[1], pstATRParse->usTB[1], pstATRParse->usTC[1], pstATRParse->usTD[1]);
	printk("TA3:0x%04x, TB3:0x%04x, TC3:0x%04x, TD3:0x%04x\n", pstATRParse->usTA[2], pstATRParse->usTB[2], pstATRParse->usTC[2], pstATRParse->usTD[2]);
	printk("TA4:0x%04x, TB4:0x%04x, TC4:0x%04x, TD4:0x%04x\n", pstATRParse->usTA[3], pstATRParse->usTB[3], pstATRParse->usTC[3], pstATRParse->usTD[3]);
	printk("Historical Charaters : %s\n", pstATRParse->ucHC);
	printk("TCK:0x%02x\n", pstATRParse->ucTCK);
	printk("ATRLenth=%d, uCount=%d\n", uATRLength, uCount);
	#endif

	//Check the ATR length
	if(uATRLength != uCount)
		return -1;

	return iProtocol;
}

int tca_sc_set_factor(sDRV_UART *pstDrvSc, unsigned char ucF, unsigned char ucD)
{
	unsigned uDiv, uBaud, uSCClk, uSCClkDiv, uSCClkPDiv, uSCClkFactor, uBitrateFactor;
	UART *pHwUART = (volatile PUART)tcc_p2v(SC_UART_ADDR(pstDrvSc->PORT));
	uBitrateFactor =1;

	dprintk("%s, %d  pHwUART= %x, pstDrvSc->PORT = %d  \n", __func__, __LINE__, SC_UART_ADDR(pstDrvSc->PORT), pstDrvSc->PORT);

	//Get Divder Value
	uSCClkPDiv 	= (pHwUART->SCCR.nREG & HwUART_SCCR_P_EN)? 2:1;
	//Get SmartCard Clock Divisor
	uSCClkDiv = pHwUART->SCCR.nREG & HwUART_SCCR_DIV_MASK;
	//Get SmartCard Clock (Hz)
	uSCClk = (pstDrvSc->Fuart)/((uSCClkDiv+1)*uSCClkPDiv);
		//Set Start TX Count
	BITCSET(pHwUART->STC.nREG, HwUART_STC_SCNT_MASK, HwUART_STC_SCNT(0));

	//Get SmartCard Clock Factor
	uSCClkFactor = gSCClkFactor[ucF];

	dprintk("%s, %d uSCClkFactor = %d \n", __func__, __LINE__, uSCClkFactor);

	//Get Baud Rate
	if(ucD < 8)
	{
		if(ucD)
		{
			if(ucD == 1)
				uBitrateFactor = 1;				
			else
				uBitrateFactor = uBitrateFactor<<(ucD-1);
		}
		else
			uBitrateFactor = 1;
			
		uBaud = (uSCClk*uBitrateFactor) / uSCClkFactor;
	}
	else
	{
		if(ucD > 9)
			uBitrateFactor = uBitrateFactor<<(ucD-9);
		else
			uBitrateFactor = 1;
			
		uBaud = uSCClk / (uSCClkFactor*uBitrateFactor);
	}
			
	//Get the Divisor Latch Value
	uDiv = (pstDrvSc->Fuart)/(16*uBaud);
	//Set divisor register to generate desired baud rate clock
	pHwUART->LCR.nREG |= HwUART_LCR_DLAB_ON;	    // DLAB = 1 to enable access DLL/DLM
	pHwUART->REG1.nREG = uDiv;
	pHwUART->REG2.nREG = uDiv >> 8;
	pHwUART->LCR.nREG &= HwUART_LCR_DLAB_OFF;    // DLAB = 0 to disable access DLL/DLM

	dprintk("%s, SCClk=%d, Baudrate=%d, Div=%d\n", __func__, uSCClk, uBaud, uDiv);

	return uBaud;
}

void tca_sc_interrupt_enable(unsigned uPort, unsigned uTxEn, unsigned uRxEn)
{
	UART *pHwUART;
	dprintk("%s\n", __func__);
	
	pHwUART = (volatile PUART)tcc_p2v(SC_UART_ADDR(uPort));
	
	//Control Interrupt Enable Register
	if(uTxEn == DRV_UART_SCINT_ENABLE)
		BITSET(pHwUART->REG2.IER.ETXI, HwUART_IER_ETXI_EN);
	else if(uTxEn == DRV_UART_SCINT_DISABLE)
		BITCLR(pHwUART->REG2.IER.ETXI, HwUART_IER_ETXI_EN);

	if(uRxEn == DRV_UART_SCINT_ENABLE)
		BITSET(pHwUART->REG2.IER.ERXI, HwUART_IER_ERXI_EN);
	else if(uRxEn == DRV_UART_SCINT_DISABLE)
		BITCLR(pHwUART->REG2.IER.ERXI, HwUART_IER_ERXI_EN);
}

irqreturn_t tca_sc_interrupt_handler(int irq, void *dev_id)
{
	unsigned char ucRcvData;
	unsigned char uPort = gDRV_UART_SC.PORT;
	
	//Get a Received Byte
	ucRcvData = tca_sc_receive_byte(uPort);
	
	//Push Data to UART Buffer
	tca_sc_rx_push_char(uPort, ucRcvData);

    return IRQ_HANDLED;
}
void tca_sc_clock_ctrl(sDRV_UART *pstDrvSc, unsigned uEnable)
{
	unsigned char uPort = pstDrvSc->PORT;
	char name[10];

	dprintk("%s\n", __func__);

	if(uEnable)
	{		
		pstDrvSc->Fuart = 48*1000*1000; //480000;
		clk_set_rate(gSC_clk, pstDrvSc->Fuart);
		clk_enable(gSC_clk);
	}
	else
	{
		clk_disable(gSC_clk);
	}
}

int tca_sc_config(sDRV_UART *pstDrvSc)
{
	unsigned uDiv, uSCClk, uSCClkDiv, uSCClkPDiv, uBaud, uRcvData;
	unsigned char uPort;
	UART *pHwUART;

	dprintk("%s\n", __func__);
	
	//Get the channel number
	uPort	= pstDrvSc->PORT;

	//Set the Base Address
	pHwUART = (volatile PUART)tcc_p2v(SC_UART_ADDR(uPort));

	//Enable UART Peripheral Clock
	tca_sc_clock_ctrl(pstDrvSc, ENABLE);

	//Clear SmartCard TX : Input
	BITCLR(pHwUART->SCCR.nREG, HwUART_SCCR_STEN_EN);
	//Disable SmartCard TX Done Interrupt
	BITCLR(pHwUART->SCCR.nREG, HwUART_SCCR_STE_EN);
	//Enable SmartCard Clock Divider and Post Clock Divider
	BITSET(pHwUART->SCCR.nREG, HwUART_SCCR_DEN_EN|HwUART_SCCR_P_EN);
	//Get Post Divder Value
	uSCClkPDiv 	= (pHwUART->SCCR.nREG & HwUART_SCCR_P_EN)? 2:1;
	//Get SmartCard Clock Divisor
	uSCClkDiv = pstDrvSc->Fuart/(DRV_UART_SCCLK*uSCClkPDiv) - 1;
	//Set SmartCard Clock Divisor
	BITCSET(pHwUART->SCCR.nREG, HwUART_SCCR_DIV_MASK, HwUART_SCCR_DIV(uSCClkDiv));
	//Set Start TX Count
	//BITCSET(pHwUART->STC, HwUART_STC_SCNT_MASK, HwUART_STC_SCNT(0));
	//Set SmartCard Clock Correctly
	uSCClk = (pstDrvSc->Fuart)/((uSCClkDiv+1)*uSCClkPDiv);
	pstDrvSc->SCCLK = uSCClk;
	
	//Get Baud Rate
	uBaud	= uSCClk/DRV_UART_SCCLK_FACTOR_1;
	//Get the Divisor Latch Value
	uDiv 	= (pstDrvSc->Fuart)/(16*uBaud);

	pHwUART->LCR.nREG = HwUART_LCR_DLAB_ON;	// DLAB = 1
	pHwUART->REG1.nREG = uDiv;
	pHwUART->REG2.nREG = uDiv >> 8;
	pHwUART->LCR.nREG = pstDrvSc->LCR;		// DLAB = 0
	pHwUART->REG3.nREG = pstDrvSc->FCR;
	if (pstDrvSc->AFT)
	{
		pHwUART->MCR.nREG = HwUART_MCR_AFE_EN | HwUART_MCR_RTS;
		pHwUART->AFT.nREG = pstDrvSc->AFT;
	}

	pHwUART->REG2.nREG = pstDrvSc->IER;

	dprintk("SCClk:%d, uDiv:%d, LCR:%x, FCR:%x, AFT:%x, IER:%x\n", 
			pstDrvSc->SCCLK, uDiv, pstDrvSc->LCR, pstDrvSc->FCR, pstDrvSc->AFT, pstDrvSc->IER);

	//Get the dummy data?
	uRcvData = pHwUART->REG1.nREG;
	
	return 0;
}

int tca_sc_enable(void)
{
	unsigned uDiv, uCount, uSCClk, uSCClkDiv, uSCClkPDiv, uBaud;
	unsigned char uUartCh, uUartPort, uTempPort;
	UART *pHwUART;
	PIC *pHwPIC;
	GPION *pHwGPIO;
	
	volatile PUARTPORTCFG pPORT = (volatile PUARTPORTCFG)tcc_p2v(HwUART_PORTCFG_BASE);
	unsigned int phy_port;

	sDRV_UART *pstDrvSc = &gDRV_UART_SC;
	uUartCh = pstDrvSc->PORT;

	dprintk("%s uUartCh = %d\n", __func__, uUartCh);

	if (sc_enable_flag) return 0;
	sc_enable_flag = 1;

	//Set the Base Address
	pHwUART = (volatile PUART)tcc_p2v(SC_UART_ADDR(uUartCh));

	//Enable UART Peripheral Clock
	tca_sc_clock_ctrl(pstDrvSc, ENABLE);

	//Enable All UART Interrupts
	pHwPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);
	BITSET(pHwPIC->IEN1.nREG, HwINT1_UART);

	dprintk("%s SC_UART_ADDR(%d) = 0x%x\n", __func__, uUartCh, SC_UART_ADDR(uUartCh));
	
	if (uUartCh> 7)
		return -1;

#if defined(CONFIG_MACH_TCC8920)
	uUartPort = 11;
#elif defined(CONFIG_MACH_TCC8920ST)
	uUartPort = 8;
#elif defined(CONFIG_MACH_M805_892X)
	uUartPort = 10;
#endif

	if(uUartCh< 4)
		BITCSET(pPORT->PCFG0.nREG, (0xFF<<(8*uUartPort)), uUartPort);
	else
		BITCSET(pPORT->PCFG1.nREG, (0xFF<<(8*(uUartCh-4))), uUartPort);

	switch(uUartCh){
		case 0: phy_port = pPORT->PCFG0.bREG.UART0; break;
		case 1: phy_port = (pPORT->PCFG0.nREG & 0xff00) >> 8; break;
		case 2: phy_port = pPORT->PCFG0.bREG.UART2; break;
		case 3: phy_port = pPORT->PCFG0.bREG.UART3; break;
		case 4: phy_port = pPORT->PCFG1.bREG.UART4; break;			
		case 5: phy_port = pPORT->PCFG1.bREG.UART5; break;
		case 6: phy_port = pPORT->PCFG1.bREG.UART6; break;
		case 7: phy_port = pPORT->PCFG1.bREG.UART7; break;			
	}

	dprintk("%s  phy_port = %x \n", __func__, phy_port);

	tcc_gpio_config(uart_port_map[phy_port][0], uart_port_map[phy_port][1]);	// TX
	tcc_gpio_config(uart_port_map[phy_port][0]+1, uart_port_map[phy_port][1]);	 // RX

	if (pstDrvSc->AFT)
	{
		if(phy_port!=0)
		{
			if(phy_port == 14 || phy_port == 20 || phy_port == 21){
			    tcc_gpio_config(uart_port_map[phy_port][0]-1, uart_port_map[phy_port][1]); //CTS	
			    tcc_gpio_config(uart_port_map[phy_port][0]-2, uart_port_map[phy_port][1]); //RTS	 
			}
			else{	
			    tcc_gpio_config(uart_port_map[phy_port][0]+2, uart_port_map[phy_port][1]); // CTS	
			    tcc_gpio_config(uart_port_map[phy_port][0]+3, uart_port_map[phy_port][1]); // RTS	
			}
		}
	}

	//Clear SmartCard TX : Input
	BITCLR(pHwUART->SCCR.nREG, HwUART_SCCR_STEN_EN);
	//Disable SmartCard TX Done Interrupt
	BITCLR(pHwUART->SCCR.nREG, HwUART_SCCR_STE_EN);
	//Enable SmartCard Clock Divider and Post Clock Divider
	BITSET(pHwUART->SCCR.nREG, HwUART_SCCR_DEN_EN|HwUART_SCCR_P_EN);
	//Get Post Divder Value
	uSCClkPDiv 	= (pHwUART->SCCR.nREG & HwUART_SCCR_P_EN)? 2:1;
	//Get SmartCard Clock Divisor
	uSCClkDiv = pstDrvSc->Fuart/(DRV_UART_SCCLK*uSCClkPDiv) - 1;
	//Set SmartCard Clock Divisor
	BITCSET(pHwUART->SCCR.nREG, HwUART_SCCR_DIV_MASK, HwUART_SCCR_DIV(uSCClkDiv));
	//Set Start TX Count
	BITCSET(pHwUART->STC.nREG, HwUART_STC_SCNT_MASK, HwUART_STC_SCNT(0));
	//Set SmartCard Clock Correctly
	uSCClk = (pstDrvSc->Fuart)/((uSCClkDiv+1)*uSCClkPDiv);
	pstDrvSc->SCCLK = uSCClk;

	//Get Baud Rate
	uBaud	= uSCClk/DRV_UART_SCCLK_FACTOR_1;
	//Get the Divisor Latch Value
	uDiv 	= (pstDrvSc->Fuart)/(16*uBaud);
	dprintk("%s, uSCClk=%d, uBaud=%d, uDiv=%d\n", __func__, uSCClk, uBaud, uDiv);
	pHwUART->LCR.nREG = HwUART_LCR_DLAB_ON;	// DLAB = 1
	pHwUART->REG1.nREG = uDiv;
	pHwUART->REG2.nREG = uDiv >> 8;
	pHwUART->LCR.nREG = pstDrvSc->LCR;		// DLAB = 0
	pHwUART->REG3.nREG = pstDrvSc->FCR;

	if (pstDrvSc->AFT)
	{
		pHwUART->MCR.nREG = HwUART_MCR_AFE_EN | HwUART_MCR_RTS;
		pHwUART->AFT.nREG = pstDrvSc->AFT;
	}
	else
	{
		pHwUART->MCR.nREG = 0;
		pHwUART->AFT.nREG = 0;
	}

	pHwUART->REG2.nREG = pstDrvSc->IER;

	dprintk("SCClk:%d, uDiv:%d, LCR:%x, FCR:%x, AFT:%x, IER:%x\n", 
			pstDrvSc->SCCLK, uDiv, pstDrvSc->LCR, pstDrvSc->FCR, pstDrvSc->AFT, pstDrvSc->IER);
	
	//Set the Interrupt Handler	
	//request_irq(INT_UART0+uUartCh, tca_sc_interrupt_handler, IRQF_SHARED, "tca_sc", &iIrqData);
	
	//Init UART Buffer
	tca_sc_init_buffer(uUartCh, DRV_UART_BUFFER_TXRX);
	
	//Enable SmartCard Interface
	BITSET(pHwUART->SCCR.nREG, HwUART_SCCR_SEN_EN);

	#if 0 /* For Debugging */
	{
		int i;
		unsigned int *pDataAddr;

		pDataAddr = (unsigned int *)pHwUART;
		printk("\n[ Register Setting ]\n");
		for(i=0; i<=(0x80/4); i+=4)
		{
			printk("0x%08x: 0x%08x 0x%08x 0x%08x 0x%08x\n", pDataAddr + i, *(pDataAddr+i+0), *(pDataAddr+i+1), *(pDataAddr+i+2), *(pDataAddr+i+3));
		}

	}	
	#endif
	
	return 0;
}

int tca_sc_disable(void)
{
	unsigned char uUartCh;
	UART *pHwUART;
	sDRV_UART *pstDrvSc = &gDRV_UART_SC;

	dprintk("%s\n", __func__);

	if (!sc_enable_flag)	return 0;
	sc_enable_flag = 0;

	uUartCh = pstDrvSc->PORT;
	pHwUART = (volatile PUART)tcc_p2v(SC_UART_ADDR(uUartCh));

	//Disable SmartCard Interface

	BITCLR(pHwUART->SCCR.nREG, HwUART_SCCR_SEN_EN);
	
	//Init UART Buffer
	tca_sc_init_buffer(uUartCh, DRV_UART_BUFFER_TXRX);

	//DRV_UART_FUNC_SC_CLOSE
	//free_irq(INT_UART0+uUartCh, &iIrqData);

	if (sc_io.data != -1) tcc_gpio_config(sc_io.data, GPIO_FN(0)|GPIO_INPUT|GPIO_PULLDOWN);
	if (sc_io.rst != -1) tcc_gpio_config(sc_io.clk, GPIO_FN(0)|GPIO_INPUT|GPIO_PULLDOWN);

	tca_sc_make_activate(0);

	//Disable All UART Interrupts
	//BITCLR(HwPIC->IEN1, HwINT1_UART);

	//Disable UART Peripheral Clock
	tca_sc_clock_ctrl(pstDrvSc, DISABLE);

	return 0;
}

int tca_sc_reset(unsigned char *pATR, unsigned *pATRLength)
{
	int iRetry;
	int iRet = -1;
	int iProtocol;
	int iSCClkFactor, iBitrateFactor =1;
	sTCC_SC_ATR_CHARS stATRChar;
	int iClockMHz;
	int iETUwork;
	int iCount;
	int iCWIValue = 1;
	int iBWIValue = 1;

	dprintk("%s\n", __func__);

	tca_sc_set_factor(&gDRV_UART_SC, 1, 1);

	for(iRetry = 3; iRetry > 0; iRetry--)
	{
		tca_sc_get_atr(pATR, pATRLength);
		if(*pATRLength > 0)
		{
			iRet = 0;
			break;
		}
	}
	
	dprintk("ATR: %d Bytes (0x%02X:0x%02X:0x%02X:0x%02X ...)\n", *pATRLength, *(pATR+0), *(pATR+1), *(pATR+2), *(pATR+3));

	if(iRet == 0)
	{
		if((iProtocol = tca_sc_parse_atr(pATR, *pATRLength, &stATRChar)) < 0)
		{
			printk("err: ATR parsing error\n");
		}
		else
		{
			//Set the SmartCard Parameters
			g_SCParams.ucProtocol = iProtocol;

			if(stATRChar.usTA[0] != 0xFFFF) // TA1
			{
				g_SCParams.ucF = (stATRChar.usTA[0] & 0xF0) >> 4;
				g_SCParams.ucD = (stATRChar.usTA[0] & 0x0F);
			}
			else
			{
				g_SCParams.ucF = 1;
				g_SCParams.ucD = 1;
			}

			if(stATRChar.usTC[0] != 0xFFFF) // TC1
				g_SCParams.ucN = stATRChar.usTC[0];
			else
				g_SCParams.ucN = 0;

			if(stATRChar.usTB[2] != 0xFFFF) // TB3
			{
				g_SCParams.ucCWI = (stATRChar.usTB[2] & 0x0F);
				g_SCParams.ucBWI = (stATRChar.usTB[2] & 0xF0) >> 4;
			}
			else
			{
				g_SCParams.ucCWI = 13; // 5
				g_SCParams.ucBWI = 4;
			}
			
			g_SCParams.uiClock = gDRV_UART_SC.SCCLK;

			//Get SmartCard Clock Factor
			iSCClkFactor = gSCClkFactor[g_SCParams.ucF];

			//Get the baud rate
			if(g_SCParams.ucD < 8)
			{
				if(g_SCParams.ucD)
					iBitrateFactor = iBitrateFactor<<(g_SCParams.ucD-1);
				else
					iBitrateFactor = 1;

				g_SCParams.uiBaudrate = (g_SCParams.uiClock*iBitrateFactor) / iSCClkFactor;
			}
			else
			{
				if(g_SCParams.ucD > 9)
					iBitrateFactor = iBitrateFactor<<(g_SCParams.ucD-9);
				else
					iBitrateFactor = 1;

				g_SCParams.uiBaudrate = g_SCParams.uiClock / (iSCClkFactor*iBitrateFactor);
			}
			
			tca_sc_set_factor(&gDRV_UART_SC, g_SCParams.ucF, g_SCParams.ucD);

			iClockMHz	= g_SCParams.uiClock/1000000;					// MHz
			iETUwork	= iSCClkFactor/(g_SCParams.ucD*iClockMHz);		// usec

			for(iCount=0; iCount<g_SCParams.ucCWI; iCount++)
				iCWIValue *= 2;
			for(iCount=0; iCount<g_SCParams.ucBWI; iCount++)
				iBWIValue *= 2;
	
			gCWT		= iCWIValue*iETUwork + 11*iETUwork;				// usec
			gBWT		= iBWIValue*960*372/iClockMHz + 11*iETUwork;	// usec

			#if 0 // Test Log
			printk("Protocol: %d\n", g_SCParams.ucProtocol);
			printk("Factor_F: %d\n", g_SCParams.ucF);
			printk("Factor_D: %d\n", g_SCParams.ucD);
			printk("Factor_N: %d\n", g_SCParams.ucN);
			printk("CWI     : %d\n", g_SCParams.ucCWI);
			printk("BWI     : %d\n", g_SCParams.ucBWI);
			printk("SC_Clock: %d\n", g_SCParams.uiClock);
			printk("Baudrate: %d\n", g_SCParams.uiBaudrate);
			printk("CWT     : %d\n", gCWT);
			printk("BWT     : %d\n", gBWT);
			#endif
		}
	}

	return iRet;
}

void tca_sc_send_data(char *pData, unsigned uLength)
{
	UART 			*pHwUART;
	unsigned char 	*pucBuffer = pData;
	unsigned char 	uPort = gDRV_UART_SC.PORT;
	unsigned		uSendLength;
	unsigned		uCPSR;

	dprintk("%s, length=%d\n", __func__, uLength);
	
	//Set the Base Address
	pHwUART = (volatile PUART)tcc_p2v(SC_UART_ADDR(uPort));

	//Init UART Buffer
	tca_sc_init_buffer(uPort, DRV_UART_BUFFER_TXRX);

	//Check the Length of Data
	if(uLength > 0)
	{
		//Set SmartCard TX Count
		#if 0
		if(uLength == 1)
		{
			BITSET(pHwUART->SCR, Hw7);
			BITCSET(pHwUART->STC, HwUART_STC_SCNT_MASK, HwUART_STC_SCNT(2));
		}
		else
		#endif
		{
			BITCLR(pHwUART->SCR.nREG, Hw7);
			BITCSET(pHwUART->STC.nREG, HwUART_STC_SCNT_MASK, HwUART_STC_SCNT(uLength));
		}

		//Set SmartCard TX
		tca_sc_ctrl_txport(uPort, ENABLE);

		//Set the Length to Send		
		uSendLength = uLength;

		while(uSendLength)
		{
			//Disable the Interrupt
			#if 0
			if((uLength != 1) && (uSendLength == 2))
				uCPSR = IO_INT_DisableINT();
			#endif

			tca_sc_send_byte(uPort, *pucBuffer++);			
			uSendLength--;
		}

		//Restore the Interrupt
		#if 0
		if(uLength != 1) 
			IO_INT_RestoreINT(uCPSR);
		#endif
	}
}

unsigned tca_sc_receive_data(char *pData, unsigned uLength)
{
	unsigned char 	*pucBuffer 	= pData;
	unsigned 		uRcvLength 	= 0;
	unsigned		uTimeOut 	= 2000;
	unsigned		uTimeCount 	= 2000;
	unsigned 		uData 		= 0;
	unsigned char 	uPort 		= gDRV_UART_SC.PORT;

	dprintk("%s, length=%d\n", __func__, uLength);
	
	//Set the Value of Time-Out
	uTimeOut = uTimeCount = (uLength == 0xFFFFFFFF) ? 30 : 2000;

	//Receive Data from SmartCard 
	while (1)
	{
		uData = tca_sc_rx_pop_char(uPort);
		if(uData < 0x100)
		{
			*pucBuffer++ = uData;
			uRcvLength++;
			uTimeOut = uTimeCount;
 			if(uRcvLength >= uLength)
				break;
		}
		else
		{
			msleep(5);
			uTimeOut--;
			if(uTimeOut == 0)
				break;
		}
	}

	return uRcvLength;
}

unsigned tca_sc_receive_data_len(char *pData, unsigned *pLength)
{
	unsigned char 	*pucBuffer 	= pData;
	unsigned 		uRcvLength 	= 0;
	unsigned		uTimeOut 	= 200;
	unsigned		uTimeCount 	= 200;
	unsigned 		uData 		= 0;
	unsigned char 	uPort 		= gDRV_UART_SC.PORT;
	unsigned		iCWTmsec 	= (gCWT/1000) + 1;

	uTimeOut	= iCWTmsec;
	uTimeCount	= iCWTmsec;
#if 1
	tca_sc_ctrl_txport(uPort, DISABLE);
#endif

	//Receive Data and Length from SmartCard 
	while (1)
	{
		uData = tca_sc_rx_pop_char(uPort);
		if(uData < 0x100)
		{
			*pucBuffer++ = uData;
			uRcvLength++;
			uTimeOut = uTimeCount;
		}
		else
		{
			msleep(iCWTmsec);
			uTimeOut--;
			if(uTimeOut == 0)
				break;
		}
	}

	*pLength = uRcvLength;

	dprintk("%s, length=%d, CWT=%d[msec]\n", __func__, uRcvLength, iCWTmsec);
	
	return uRcvLength;
}

int	tca_sc_send_receive(char *pSend, unsigned uSendLength, char *pRcv, unsigned uRcvLength)
{
	int	ret	= -1;

	dprintk("%s\n", __func__);
	
	// Send Command
	if(pSend && uSendLength)
		tca_sc_send_data(pSend, uSendLength);

	// Receive Response
	if(pRcv!=NULL && uRcvLength!=0)
	{
		if (uRcvLength == 0xFFFFFFFF)
		{
			ret	= tca_sc_receive_data(pRcv, uRcvLength);
			if(ret <= 0)
				ret	= -1;
		}
		else
		{
			ret	= tca_sc_receive_data(pRcv, uRcvLength);
			if(ret == uRcvLength)
				ret	= 0;
			else if(ret < 0x7FFFFFFF)
				ret	= Hw31|ret;
			else
				ret	= -1;
		}
	}

	return ret;
}

int	tca_sc_send_receive_len(char *pSend, unsigned uSendLength, char *pRcv, unsigned *pRcvLength)
{
	int	iRet = -1;
	int	iLength = 0;
	int	iBWTmsec = (gBWT/1000) + 1;

	dprintk("%s, BWT=%d[msec]\n", __func__, iBWTmsec);

	if (!sc_detect_get_state())
		return 0;

	// Send Command
	if(pSend && uSendLength)
		tca_sc_send_data(pSend, uSendLength);

	msleep(iBWTmsec);
					
	// Receive Response and Length
	if(pRcv!=NULL && pRcvLength!=NULL)
	{
		iLength	= tca_sc_receive_data_len(pRcv, pRcvLength);
		if(iLength > 0)
			iRet = iLength;
	}

	return iRet;
}

int sc_detect_get_state (void)
{
	return sc_detect.state;
}

#if defined(CONFIG_MACH_M805_892X)
void sc_detect_init (void)
{
	int	i;

	sc_detect.state = 0;
	sc_detect.filter_idx = 0;
	for (i=0; i < SC_DETECT_FILTER_MAX; i++)
		sc_detect.filter[i] = 0;
}
void sc_detect_update_filter(int detect_flag)
{
	if (sc_detect.filter_idx >= SC_DETECT_FILTER_MAX)
		sc_detect.filter_idx = 0;

	sc_detect.filter[sc_detect.filter_idx] = (detect_flag ? 1 : 0);
	sc_detect.filter_idx++;
}
int sc_detect_update_state (void)
{
	int prev_state, cur_state;
	int count,filter_sum;

	prev_state = sc_detect.state;

	filter_sum = 0;
	for (count=0; count < SC_DETECT_FILTER_MAX; count++)
		filter_sum += sc_detect.filter[count];

	if (filter_sum == SC_DETECT_FILTER_MAX || filter_sum == 0) {	//if all values of filter are same,
		cur_state = filter_sum ? 1 : 0;
		if (cur_state != prev_state) {
			sc_detect.state = cur_state;
			return 1;
		}
	} else {
		//sc detection signal is now in transition.
		//do nothing
	}
	return 0;
}

void sc_detect_timer_regist(struct timer_list *ptimer, unsigned int timeover)
{
	init_timer (ptimer);
	ptimer->expires = jiffies+msecs_to_jiffies(timeover);
	ptimer->data =  (unsigned long)0;
	ptimer->function = sc_detect_timer_handler;

	add_timer(ptimer);
}

void sc_detect_timer_handler (unsigned long data)
{
	int	detect_val = 0;

	if (sc_io.detect != -1) detect_val = gpio_get_value (sc_io.detect);
	sc_detect_update_filter(detect_val);
	if (sc_detect_update_state()) {	//if state changes
		if (sc_detect_get_state()) {
			tca_sc_pwren(1);		
			tca_sc_make_reset(1);
			tca_sc_enable();
		} else {
			tca_sc_disable();
			tca_sc_make_activate(0);
			tca_sc_pwren(0);
		}
	}

	sc_detect_timer_regist (&sc_detect.sc_timer, SC_DETECT_TIME);
}
#endif

int tca_sc_open(struct inode *inode, struct file *filp)
{
	dprintk("%s\n", __func__);
	
	tca_sc_init_port();

	sc_enable_flag = 0;

#if defined(CONFIG_MACH_M805_892X)
	sc_detect_init ();
	sc_detect_timer_regist (&sc_detect.sc_timer, SC_DETECT_TIME);
#endif

	return 0;
}
int tca_sc_close(struct inode *inode, struct file *file)
{
	dprintk("%s\n", __func__);

	del_timer_sync (&sc_detect.sc_timer);

	//Disable Smartcard Interface
	if (sc_enable_flag) {
		tca_sc_disable();
	}

	return 0;
}

int tca_sc_init(void)
{
	unsigned char	uPort			= gDRV_UART_SC.PORT;
	
	dprintk("%s\n", __func__);

	tca_sc_init_port();

	//set the interrupt handler
	request_irq(INT_UART0+uPort, tca_sc_interrupt_handler, IRQF_SHARED, "tca_sc", &iIrqData);

	switch(uPort)
	{
		case DRV_UART_PORT0:
			gSC_clk = clk_get(0, "uart0");
			break;	
		case DRV_UART_PORT1:
			gSC_clk = clk_get(0, "uart1");
			break;	
		case DRV_UART_PORT2:
			gSC_clk = clk_get(0, "uart2");
			break;	
		case DRV_UART_PORT3:
			gSC_clk = clk_get(0, "uart3");
			break;	
		case DRV_UART_PORT4:
			gSC_clk = clk_get(0, "uart4");
			break;	
		case DRV_UART_PORT5:
			gSC_clk = clk_get(0, "uart5");
			break;	
		case DRV_UART_PORT6:
			gSC_clk = clk_get(0, "uart6");
			break;	
		case DRV_UART_PORT7:
			gSC_clk = clk_get(0, "uart7");
			break;	

		default:
			return 0;
	}
		
	return 0;
}

void tca_sc_exit(void)
{
	unsigned char uPort = gDRV_UART_SC.PORT;
	dprintk("%s\n", __func__);

	free_irq(INT_UART0+uPort, &iIrqData);

	clk_put(gSC_clk);
	return;
}
