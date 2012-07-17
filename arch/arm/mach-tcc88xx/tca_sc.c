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

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>
#include <asm/mach/map.h>

#include <mach/bsp.h>
#include <mach/tca_ckc.h>
#include <mach/globals.h>

#if defined(CONFIG_ARCH_TCC88XX)
#include <mach/structures.h>
#endif

#include <mach/tcc_sc_ioctl.h>
#include <mach/tcc_pca953x.h>
#include <mach/gpio.h>


#if defined(CONFIG_MACH_TCC8800)
#define GPIO_CAS_DATA		TCC_GPD(13)

#define GPIO_TDA8024_VCC_RST		TCC_GPEXT2(7)

#define TDA8024_VCC_INPUT		gpio_direction_input(GPIO_TDA8024_VCC_RST)
#define TDA8024_VCC_OUTPUT		gpio_direction_output(GPIO_TDA8024_VCC_RST, 1)
#define TDA8024_VCC_LOW		gpio_set_value(GPIO_TDA8024_VCC_RST, 0)
#define TDA8024_VCC_HIGH		gpio_set_value(GPIO_TDA8024_VCC_RST, 1)

#define GPIO_CAS_RST		TCC_GPEXT2(6)

#define SC_RESET_INPUT		gpio_direction_input(GPIO_CAS_RST)
#define SC_RESET_OUTPUT		gpio_direction_output(GPIO_CAS_RST, 1)
#define SC_RESET_LOW		gpio_set_value(GPIO_CAS_RST, 0)
#define SC_RESET_HIGH		gpio_set_value(GPIO_CAS_RST, 1)

#elif defined(CONFIG_MACH_TCC8800ST)
#define GPIO_CAS_DATA		TCC_GPD(13)

#define GPIO_TDA8024_VCC_RST		TCC_GPB(28)

#define TDA8024_VCC_INPUT		gpio_direction_input(GPIO_TDA8024_VCC_RST)
#define TDA8024_VCC_OUTPUT		gpio_direction_output(GPIO_TDA8024_VCC_RST, 1)
#define TDA8024_VCC_LOW		gpio_set_value(GPIO_TDA8024_VCC_RST, 0)
#define TDA8024_VCC_HIGH		gpio_set_value(GPIO_TDA8024_VCC_RST, 1)

#define GPIO_CAS_RST		TCC_GPD(15)

#define SC_RESET_INPUT		gpio_direction_input(GPIO_CAS_RST)
#define SC_RESET_OUTPUT		gpio_direction_output(GPIO_CAS_RST, 1)
#define SC_RESET_LOW		gpio_set_value(GPIO_CAS_RST, 0)
#define SC_RESET_HIGH		gpio_set_value(GPIO_CAS_RST, 1)

 #else
#define SC_RESET_INPUT		NULL
#define SC_RESET_OUTPUT		NULL
#define SC_RESET_LOW		NULL
#define SC_RESET_HIGH		NULL

#endif

#if defined(CONFIG_MACH_TCC8800)
#define GPIO_SC_DETECT		TCC_GPF(27)

#define SC_DETECT_INPUT	gpio_direction_input(GPIO_SC_DETECT)
#define SC_DETECT_OUTPUT	gpio_direction_output(GPIO_SC_DETECT, 1)
#define SC_DETECT_PORT		NULL
#define SC_DETECT_BIT		NULL
#elif defined(CONFIG_MACH_TCC8800ST)

#define SC_DETECT_INPUT		NULL
#define SC_DETECT_OUTPUT	NULL
#define SC_DETECT_PORT		NULL
#define SC_DETECT_BIT		NULL

#else
#define SC_DETECT_INPUT		NULL
#define SC_DETECT_OUTPUT	NULL
#define SC_DETECT_PORT		NULL
#define SC_DETECT_BIT		NULL
#endif

#define SC_VCCN_INPUT		NULL
#define SC_VCCN_OUTPUT		NULL
#define SC_VCCN_LOW			NULL
#define SC_VCCN_HIGH		NULL

#define SC_VCCSEL_INPUT		NULL
#define SC_VCCSEL_OUTPUT	NULL
#define SC_VCCSEL_LOW		NULL
#define SC_VCCSEL_HIGH		NULL


#define SC_UART_ADDR(x)		HwUART_CH_BASE(x)

static sDRV_UART_BUF gDRV_UART_BUF[DRV_UART_CHMAX];
static sTCC_SC_PARAMS g_SCParams;

static int debug = 0;
#define dprintk(msg...)	if(debug) { printk( "tca_sc: " msg); }

static struct clk *gSC_clk;		// uart clock

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
	, DRV_UART_CH4						// UART Channel
	, DRV_UART_PORT4					// UART Port
};

static unsigned gSCClkFactor[16] = 
{
	DRV_UART_SCCLK_FACTOR_0, DRV_UART_SCCLK_FACTOR_1, DRV_UART_SCCLK_FACTOR_2, DRV_UART_SCCLK_FACTOR_3,
	DRV_UART_SCCLK_FACTOR_4, DRV_UART_SCCLK_FACTOR_5, DRV_UART_SCCLK_FACTOR_6, DRV_UART_SCCLK_FACTOR_7,
	DRV_UART_SCCLK_FACTOR_8, DRV_UART_SCCLK_FACTOR_9, DRV_UART_SCCLK_FACTOR_A, DRV_UART_SCCLK_FACTOR_B,
	DRV_UART_SCCLK_FACTOR_C, DRV_UART_SCCLK_FACTOR_D, DRV_UART_SCCLK_FACTOR_E, DRV_UART_SCCLK_FACTOR_F
};

static unsigned short gCWT; //microsecond
static unsigned short gBWT; //microsecond
static int iIrqData;
	
void tca_sc_init_port(void)
{
	// RESET
	#if defined(CONFIG_MACH_TCC8800)
	gpio_request(GPIO_TDA8024_VCC_RST,NULL);
	gpio_request(GPIO_CAS_RST,NULL);
	gpio_request(GPIO_SC_DETECT,NULL);

 	tcc_gpio_config(GPIO_CAS_DATA, GPIO_FN(0));
	tcc_gpio_config(GPIO_TDA8024_VCC_RST, GPIO_FN(0));
	tcc_gpio_config(GPIO_CAS_RST, GPIO_FN(0));
	tcc_gpio_config(GPIO_SC_DETECT, GPIO_FN(0));
 
	TDA8024_VCC_OUTPUT;
	TDA8024_VCC_HIGH;
	
	#elif defined(CONFIG_MACH_TCC8800ST)
	gpio_request(GPIO_TDA8024_VCC_RST,NULL);
	gpio_request(GPIO_CAS_RST,NULL);
	gpio_request(GPIO_SC_DETECT,NULL);
	
	tcc_gpio_config(GPIO_CAS_DATA, GPIO_FN(0));	// CAS_DATA
	tcc_gpio_config(GPIO_CAS_RST, GPIO_FN(0));	// CAS_RST#
	/*******************************************************************/
	// Originaly "GPIO_B(28)" is "TCC8801F_STB" Test Pin . 
	// "GPIO_B(28)" use to in test.
	tcc_gpio_config(GPIO_TDA8024_VCC_RST, GPIO_FN(0));	// CAS_GP(SC_CDMVCC#)

	TDA8024_VCC_OUTPUT;
	TDA8024_VCC_HIGH;

 	#endif

	SC_RESET_OUTPUT;
//	SC_RESET_HIGH;	
	SC_RESET_LOW;	

	// Card Detection
	SC_DETECT_INPUT;

	// VCCN Selection - High
	SC_VCCN_OUTPUT;
	SC_VCCN_HIGH;

	// VCC Voltage Selection - High:5V, Low:3V
	SC_VCCSEL_OUTPUT;				
	SC_VCCSEL_HIGH;
}

void tca_sc_init_buffer(unsigned uCH, unsigned uTxRx)
{
	sDRV_UART_BUF	*pUartBuf;

	dprintk("%s\n", __func__);

	//Get the Pointer of Buffer
	pUartBuf = &gDRV_UART_BUF[uCH];

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

void tca_sc_rx_push_char(unsigned uCH, char cChar)
{
	sDRV_UART_BUF *pUartBuf = &gDRV_UART_BUF[uCH];

	pUartBuf->sRX_Buffer[pUartBuf->usRXHead++] = cChar;
	pUartBuf->usRXHead %= IO_UART_RX_BUFFER_SIZE;

	//dprintk("p\n");
}

unsigned tca_sc_rx_pop_char(unsigned uCH)
{
	sDRV_UART_BUF 	*pUartBuf = &gDRV_UART_BUF[uCH];
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

	if(uReset)
	{
#if defined(CONFIG_ARCH_TCC88XX)
		SC_RESET_OUTPUT;
		SC_RESET_HIGH;
#else
		SC_RESET_OUTPUT;
		SC_RESET_HIGH;
#endif
	}
	else
	{
#if defined(CONFIG_ARCH_TCC88XX)
		TDA8024_VCC_OUTPUT	;
		TDA8024_VCC_LOW;

		SC_RESET_OUTPUT;
		SC_RESET_LOW;
#elif defined(CONFIG_ARCH_TCC892X)
		TDA8024_VCC_OUTPUT	;
		TDA8024_VCC_LOW;

		SC_RESET_OUTPUT;
		SC_RESET_LOW;
#else
		SC_RESET_OUTPUT;
		SC_RESET_LOW;
#endif
	}
}

void tca_sc_make_activate(unsigned uActivate)
{
	dprintk("%s, uActivate=%d\n", __func__, uActivate);

	if(uActivate)
	{
		SC_VCCN_OUTPUT;
		SC_VCCN_LOW;
	}
	else
	{
		SC_VCCN_OUTPUT;
		SC_VCCN_HIGH;
	}
}

void tca_sc_select_voltage(unsigned uVoloate_3V)
{
	dprintk("%s\n", __func__);

	if(uVoloate_3V)
	{
		SC_VCCSEL_OUTPUT;
		SC_VCCSEL_LOW;
	}
	else
	{
		SC_VCCSEL_OUTPUT;
		SC_VCCSEL_HIGH;
	}
}

unsigned tca_sc_detect_card(void)
{
#if defined(CONFIG_MACH_TCC8800)
	int ret=0;
	ret =gpio_get_value(GPIO_SC_DETECT);

	dprintk("%s ret = %d\n", __func__, ret);

	if(ret)
		tca_sc_make_reset(0);	// Reset LOW
	else
		TDA8024_VCC_HIGH;

	return ret;
#else
	return 1;
#endif	
}

void tca_sc_ctrl_txport(unsigned uCH, unsigned uEnable)
{
	UART *pHwUART = (volatile PUART)tcc_p2v(SC_UART_ADDR(uCH));
	
	dprintk("%s\n", __func__);
	
	//Control Direction of SmartCard TX Port
	if(uEnable)
	{
		BITSET(pHwUART->SCCR, HwUART_SCCR_STEN_EN);
	}
	else
	{
		BITCLR(pHwUART->SCCR, HwUART_SCCR_STEN_EN);
	}
}

unsigned tca_sc_receive_byte(unsigned int uCH)
{
	UART *pHwUART = (volatile PUART)tcc_p2v(SC_UART_ADDR(uCH));
	unsigned uByte;

	//Wait until Data is Ready
	while(ISZERO(pHwUART->LSR, HwUART_LSR_DR_ON));
	
	uByte = pHwUART->REG1.UTRXD;

	return uByte;
}

void tca_sc_send_byte(unsigned int uCH, unsigned uChar)
{
	UART *pHwUART = (volatile PUART)tcc_p2v(SC_UART_ADDR(uCH));

	//Wait util THR Empty
	while (ISZERO(pHwUART->LSR, HwUART_LSR_THRE_ON));

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
	unsigned char	uCH			= gDRV_UART_SC.CHANNEL;

	dprintk("%s\n", __func__);
	
	//Init Tx/Rx buffer
	tca_sc_init_buffer(uCH, DRV_UART_BUFFER_TXRX);

	tca_sc_make_reset(1);	// Reset HIGH

	//Disable SmartCard TX
	tca_sc_ctrl_txport(uCH, DISABLE);
	
	//Get ATR Response
	while (1)
	{
		uData = tca_sc_rx_pop_char(uCH);
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
	}

	//Set the ATR Length
	*pATRLength = uLength;
	
	//Set the Last Byte
	*(pATRData+uLength) 	= 0;
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
	UART *pHwUART = (volatile PUART)tcc_p2v(SC_UART_ADDR(pstDrvSc->CHANNEL));
	uBitrateFactor =1;

	dprintk("%s\n", __func__);

	//Get Divder Value
	uSCClkPDiv 	= (pHwUART->SCCR & HwUART_SCCR_P_EN)? 2:1;
	//Get SmartCard Clock Divisor
	uSCClkDiv = pHwUART->SCCR & HwUART_SCCR_DIV_MASK;
	//Get SmartCard Clock (Hz)
	uSCClk = (pstDrvSc->Fuart)/((uSCClkDiv+1)*uSCClkPDiv);
		//Set Start TX Count
	BITCSET(pHwUART->STC, HwUART_STC_SCNT_MASK, HwUART_STC_SCNT(0));

	//Get SmartCard Clock Factor
	uSCClkFactor = gSCClkFactor[ucF];

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
	pHwUART->LCR |= HwUART_LCR_DLAB_ON;	    // DLAB = 1 to enable access DLL/DLM
#if defined(CONFIG_ARCH_TCC88XX)
	pHwUART->REG1.DLL = uDiv;
	pHwUART->REG2.DLM = uDiv >> 8;
#endif
	pHwUART->LCR &= HwUART_LCR_DLAB_OFF;    // DLAB = 0 to disable access DLL/DLM

	dprintk("%s, SCClk=%d, Baudrate=%d, Div=%d\n", __func__, uSCClk, uBaud, uDiv);

	return uBaud;
}

void tca_sc_interrupt_enable(unsigned uCH, unsigned uTxEn, unsigned uRxEn)
{
	UART *pHwUART;
	
	dprintk("%s\n", __func__);
	
	pHwUART = (volatile PUART)tcc_p2v(SC_UART_ADDR(uCH));
	
	//Control Interrupt Enable Register
	#if defined(CONFIG_ARCH_TCC88XX)
	if(uTxEn == DRV_UART_SCINT_ENABLE)
		BITSET(pHwUART->REG2.IER, HwUART_IER_ETXI_EN);
	else if(uTxEn == DRV_UART_SCINT_DISABLE)
		BITCLR(pHwUART->REG2.IER, HwUART_IER_ETXI_EN);

	if(uRxEn == DRV_UART_SCINT_ENABLE)
		BITSET(pHwUART->REG2.IER, HwUART_IER_ERXI_EN);
	else if(uRxEn == DRV_UART_SCINT_DISABLE)
		BITCLR(pHwUART->REG2.IER, HwUART_IER_ERXI_EN);
	#endif
}

irqreturn_t tca_sc_interrupt_handler(int irq, void *dev_id)
{
	unsigned char ucRcvData;
	unsigned char ucCH = gDRV_UART_SC.CHANNEL;
	
	//Get a Received Byte
	ucRcvData = tca_sc_receive_byte(ucCH);
	
	//Push Data to UART Buffer
	tca_sc_rx_push_char(ucCH, ucRcvData);

    return IRQ_HANDLED;
}

void tca_sc_clock_ctrl(sDRV_UART *pstDrvSc, unsigned uEnable)
{
	unsigned char uUartCH = pstDrvSc->CHANNEL;
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
	unsigned char uUartCH;
	UART *pHwUART;

	dprintk("%s\n", __func__);
	
	//Get the channel number
	uUartCH	= pstDrvSc->CHANNEL;

	//Set the Base Address
	pHwUART = (volatile PUART)tcc_p2v(SC_UART_ADDR(uUartCH));

	//Enable UART Peripheral Clock
	tca_sc_clock_ctrl(pstDrvSc, ENABLE);

	//Clear SmartCard TX : Input
	BITCLR(pHwUART->SCCR, HwUART_SCCR_STEN_EN);
	//Disable SmartCard TX Done Interrupt
	BITCLR(pHwUART->SCCR, HwUART_SCCR_STE_EN);
	//Enable SmartCard Clock Divider and Post Clock Divider
	BITSET(pHwUART->SCCR, HwUART_SCCR_DEN_EN|HwUART_SCCR_P_EN);
	//Get Post Divder Value
	uSCClkPDiv 	= (pHwUART->SCCR & HwUART_SCCR_P_EN)? 2:1;
	//Get SmartCard Clock Divisor
	uSCClkDiv = pstDrvSc->Fuart/(DRV_UART_SCCLK*uSCClkPDiv) - 1;
	//Set SmartCard Clock Divisor
	BITCSET(pHwUART->SCCR, HwUART_SCCR_DIV_MASK, HwUART_SCCR_DIV(uSCClkDiv));
	//Set Start TX Count
	//BITCSET(pHwUART->STC, HwUART_STC_SCNT_MASK, HwUART_STC_SCNT(0));
	//Set SmartCard Clock Correctly
	uSCClk = (pstDrvSc->Fuart)/((uSCClkDiv+1)*uSCClkPDiv);
	pstDrvSc->SCCLK = uSCClk;
	
	//Get Baud Rate
	uBaud	= uSCClk/DRV_UART_SCCLK_FACTOR_1;
	//Get the Divisor Latch Value
	uDiv 	= (pstDrvSc->Fuart)/(16*uBaud);

	#if defined(CONFIG_ARCH_TCC88XX)
		pHwUART->LCR = HwUART_LCR_DLAB_ON;	// DLAB = 1
		pHwUART->REG1.DLL = uDiv;
		pHwUART->REG2.DLM = uDiv >> 8;
		pHwUART->LCR = pstDrvSc->LCR;		// DLAB = 0
		pHwUART->REG3.FCR = pstDrvSc->FCR;
		if (pstDrvSc->AFT)
		{
			pHwUART->MCR = HwUART_MCR_AFE_EN | HwUART_MCR_RTS;
			pHwUART->AFT = pstDrvSc->AFT;
		}

		pHwUART->REG2.IER = pstDrvSc->IER;

		dprintk("SCClk:%d, uDiv:%d, LCR:%x, FCR:%x, AFT:%x, IER:%x\n", 
				pstDrvSc->SCCLK, uDiv, pstDrvSc->LCR, pstDrvSc->FCR, pstDrvSc->AFT, pstDrvSc->IER);

		//Get the dummy data?
		uRcvData = pHwUART->REG1.RBR;
	#endif
	
	return 0;
}

int tca_sc_enable(void)
{
	unsigned uDiv, uCount, uSCClk, uSCClkDiv, uSCClkPDiv, uBaud;
	unsigned char uUartCH, uUartPort, uTempPort;
	UART *pHwUART;
	PIC *pHwPIC;
	GPION *pHwGPIO;
	unsigned long *pCHSEL, uCHSEL;

	sDRV_UART *pstDrvSc = &gDRV_UART_SC;

	dprintk("%s\n", __func__);
	
	uUartCH		= pstDrvSc->CHANNEL;
	uUartPort	= pstDrvSc->PORT;

	//Set the Base Address
	pHwUART = (volatile PUART)tcc_p2v(SC_UART_ADDR(uUartCH));

	//Enable UART Peripheral Clock
	tca_sc_clock_ctrl(pstDrvSc, ENABLE);

	//Enable All UART Interrupts
	pHwPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);
	BITSET(pHwPIC->IEN1, HwINT1_UART);
		
	//Set PORT
	switch(uUartPort)
	{
		case DRV_UART_PORT0:
			pHwGPIO = (volatile PGPION)tcc_p2v(HwGPIOE_BASE);
			BITSET(pHwGPIO->GPEN, Hw0); // UTXD(0) Output
			BITCLR(pHwGPIO->GPEN, Hw1); // URXD(0) input
			BITCSET(pHwGPIO->GPFN0, 0x000000FF, Hw4|Hw0); // UTXD0, URXD0
			BITCSET(pHwGPIO->GPPD0, Hw3|Hw2|Hw1|Hw0, Hw2|Hw0); // pull-up enable
			break;
		case DRV_UART_PORT1:
			pHwGPIO = (volatile PGPION)tcc_p2v(HwGPIOE_BASE);
			BITSET(pHwGPIO->GPEN, Hw4); // UTXD(1) Output
			BITCLR(pHwGPIO->GPEN, Hw5); // URXD(1) input
			BITCSET(pHwGPIO->GPFN0, 0x00FF0000, Hw20|Hw16); // UTXD1, URXD1
			BITCSET(pHwGPIO->GPPD0, Hw11|Hw10|Hw9|Hw8, Hw10|Hw8); // pull-up enable
			break;
		case DRV_UART_PORT2:
			pHwGPIO = (volatile PGPION)tcc_p2v(HwGPIOE_BASE);
			BITSET(pHwGPIO->GPEN, Hw8); // UTXD(2) Output
			BITCLR(pHwGPIO->GPEN, Hw9); // URXD(2) input
			BITCSET(pHwGPIO->GPFN1, 0x000000FF, Hw4|Hw0); // UTXD2, URXD2
			BITCSET(pHwGPIO->GPPD0, Hw19|Hw18|Hw17|Hw16, Hw18|Hw16); // pull-up enable
			break;
		case DRV_UART_PORT3:
			pHwGPIO = (volatile PGPION)tcc_p2v(HwGPIOE_BASE);
			BITSET(pHwGPIO->GPEN, Hw10); // UTXD(3) Output
			BITCLR(pHwGPIO->GPEN, Hw11); // URXD(3) input
			BITCSET(pHwGPIO->GPFN1, 0x0000FF00, Hw12|Hw8); // UTXD3, URXD3
			BITCSET(pHwGPIO->GPPD0, Hw23|Hw22|Hw21|Hw20, Hw22|Hw20); // pull-up enable
			break;
		case DRV_UART_PORT4:
			pHwGPIO = (volatile PGPION)tcc_p2v(HwGPIOD_BASE);
			BITSET(pHwGPIO->GPEN, Hw13); // UTXD(4) Output
			BITCLR(pHwGPIO->GPEN, Hw14); // URXD(4) input
			BITCSET(pHwGPIO->GPFN1, 0x0FF00000, Hw24|Hw20); // UTXD4, URXD4
			BITCSET(pHwGPIO->GPPD0, Hw29|Hw28|Hw27|Hw26, Hw28|Hw26); // pull-up enable
			break;
		case DRV_UART_PORT5:
			pHwGPIO = (volatile PGPION)tcc_p2v(HwGPIOD_BASE);
			BITSET(pHwGPIO->GPEN, Hw17); // UTXD(5) Output
			BITCLR(pHwGPIO->GPEN, Hw18); // URXD(5) input
			BITCSET(pHwGPIO->GPFN2, 0x00000FF0, Hw4|Hw8); // UTXD5, URXD5
			BITCSET(pHwGPIO->GPPD1, Hw5|Hw4|Hw3|Hw2, Hw4|Hw2); // pull-up enable
			break;
	}

	pCHSEL = (unsigned long*)tcc_p2v(HwUART_CHSEL);
	uCHSEL = *pCHSEL;

	//Clear Same Port Number in CHSEL Register
	for(uCount=0; uCount<DRV_UART_CHMAX; uCount++)
	{
		if((uCHSEL & HwUART_CHSEL_MASK(uCount)) == (uUartPort<<(4*uCount)))
		{
			if(uCount != uUartCH)
			{
				uTempPort = (uCHSEL & HwUART_CHSEL_MASK(uUartCH)) >> (4*uUartCH);
				BITCSET(uCHSEL, HwUART_CHSEL_MASK(uCount), HwUART_CHSEL_SEL_PORT(uCount, uTempPort));			
			}
		}
	}
	//Select Port Number for Selected Channel
	BITCSET(uCHSEL, HwUART_CHSEL_MASK(uUartCH), HwUART_CHSEL_SEL_PORT(uUartCH, uUartPort));	
	//Update Channel Selection Register
	*pCHSEL = uCHSEL;

	//Clear SmartCard TX : Input
	BITCLR(pHwUART->SCCR, HwUART_SCCR_STEN_EN);
	//Disable SmartCard TX Done Interrupt
	BITCLR(pHwUART->SCCR, HwUART_SCCR_STE_EN);
	//Enable SmartCard Clock Divider and Post Clock Divider
	BITSET(pHwUART->SCCR, HwUART_SCCR_DEN_EN|HwUART_SCCR_P_EN);
	//Get Post Divder Value
	uSCClkPDiv 	= (pHwUART->SCCR & HwUART_SCCR_P_EN)? 2:1;
	//Get SmartCard Clock Divisor
	uSCClkDiv = pstDrvSc->Fuart/(DRV_UART_SCCLK*uSCClkPDiv) - 1;
	//Set SmartCard Clock Divisor
	BITCSET(pHwUART->SCCR, HwUART_SCCR_DIV_MASK, HwUART_SCCR_DIV(uSCClkDiv));
	//Set Start TX Count
	BITCSET(pHwUART->STC, HwUART_STC_SCNT_MASK, HwUART_STC_SCNT(0));
	//Set SmartCard Clock Correctly
	uSCClk = (pstDrvSc->Fuart)/((uSCClkDiv+1)*uSCClkPDiv);
	pstDrvSc->SCCLK = uSCClk;

	
	//Get Baud Rate
	uBaud	= uSCClk/DRV_UART_SCCLK_FACTOR_1;
	//Get the Divisor Latch Value
	uDiv 	= (pstDrvSc->Fuart)/(16*uBaud);
	dprintk("%s, uSCClk=%d, uBaud=%d, uDiv=%d\n", __func__, uSCClk, uBaud, uDiv);
	#if defined(CONFIG_ARCH_TCC88XX)
		pHwUART->LCR = HwUART_LCR_DLAB_ON;	// DLAB = 1
		pHwUART->REG1.DLL = uDiv;
		pHwUART->REG2.DLM = uDiv >> 8;
		pHwUART->LCR = pstDrvSc->LCR;		// DLAB = 0
		pHwUART->REG3.FCR = pstDrvSc->FCR;
		if (pstDrvSc->AFT)
		{
			pHwUART->MCR = HwUART_MCR_AFE_EN | HwUART_MCR_RTS;
			pHwUART->AFT = pstDrvSc->AFT;
		}
		else
		{
			pHwUART->MCR = 0;
			pHwUART->AFT = 0;
		}
		pHwUART->REG2.IER = pstDrvSc->IER;
	#endif

	dprintk("SCClk:%d, uDiv:%d, LCR:%x, FCR:%x, AFT:%x, IER:%x\n", 
			pstDrvSc->SCCLK, uDiv, pstDrvSc->LCR, pstDrvSc->FCR, pstDrvSc->AFT, pstDrvSc->IER);
	
	//Set the Interrupt Handler	
	request_irq(INT_UART0+uUartCH, tca_sc_interrupt_handler, IRQF_SHARED, "tca_sc", &iIrqData);
	
	//Init UART Buffer
	tca_sc_init_buffer(uUartCH, DRV_UART_BUFFER_TXRX);
	
	//Enable SmartCard Interface
	BITSET(pHwUART->SCCR, HwUART_SCCR_SEN_EN);

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
	unsigned char uUartCH;
	UART *pHwUART;
	sDRV_UART *pstDrvSc = &gDRV_UART_SC;
	
	dprintk("%s\n", __func__);
	
	uUartCH	= pstDrvSc->CHANNEL;
	pHwUART = (volatile PUART)tcc_p2v(SC_UART_ADDR(uUartCH));

	//Disable SmartCard Interface
#if 1
	{
		GPION *pHwGPIO;
		unsigned char uUartPort;
		uUartPort	= pstDrvSc->PORT;

		//Set PORT
		switch(uUartPort)
		{
			case DRV_UART_PORT0:
				pHwGPIO = (volatile PGPION)tcc_p2v(HwGPIOE_BASE);
				BITSET(pHwGPIO->GPEN, Hw0); // UTXD(0) Output
				BITCLR(pHwGPIO->GPEN, Hw1); // URXD(0) input
				BITSCLR(pHwGPIO->GPFN0, 0x000000FF, Hw4|Hw0); // UTXD0, URXD0
				BITSCLR(pHwGPIO->GPPD0, Hw3|Hw2|Hw1|Hw0, Hw2|Hw0); // pull-up enable
				break;
			case DRV_UART_PORT1:
				pHwGPIO = (volatile PGPION)tcc_p2v(HwGPIOE_BASE);
				BITSET(pHwGPIO->GPEN, Hw4); // UTXD(1) Output
				BITCLR(pHwGPIO->GPEN, Hw5); // URXD(1) input
				BITSCLR(pHwGPIO->GPFN0, 0x00FF0000, Hw20|Hw16); // UTXD1, URXD1
				BITSCLR(pHwGPIO->GPPD0, Hw11|Hw10|Hw9|Hw8, Hw10|Hw8); // pull-up enable
				break;
			case DRV_UART_PORT2:
				pHwGPIO = (volatile PGPION)tcc_p2v(HwGPIOE_BASE);
				BITSET(pHwGPIO->GPEN, Hw8); // UTXD(2) Output
				BITCLR(pHwGPIO->GPEN, Hw9); // URXD(2) input
				BITSCLR(pHwGPIO->GPFN1, 0x000000FF, Hw4|Hw0); // UTXD2, URXD2
				BITSCLR(pHwGPIO->GPPD0, Hw19|Hw18|Hw17|Hw16, Hw18|Hw16); // pull-up enable
				break;
			case DRV_UART_PORT3:
				pHwGPIO = (volatile PGPION)tcc_p2v(HwGPIOE_BASE);
				BITSET(pHwGPIO->GPEN, Hw10); // UTXD(3) Output
				BITCLR(pHwGPIO->GPEN, Hw11); // URXD(3) input
				BITSCLR(pHwGPIO->GPFN1, 0x0000FF00, Hw12|Hw8); // UTXD3, URXD3
				BITSCLR(pHwGPIO->GPPD0, Hw23|Hw22|Hw21|Hw20, Hw22|Hw20); // pull-up enable
				break;
			case DRV_UART_PORT4:
				pHwGPIO = (volatile PGPION)tcc_p2v(HwGPIOD_BASE);
				BITSET(pHwGPIO->GPEN, Hw13); // UTXD(4) Output
				BITCLR(pHwGPIO->GPEN, Hw14); // URXD(4) input
				BITSCLR(pHwGPIO->GPFN1, 0x0FF00000, Hw24|Hw20); // UTXD4, URXD4
				BITSCLR(pHwGPIO->GPPD0, Hw29|Hw28|Hw27|Hw26, Hw28|Hw26); // pull-up enable
				break;
			case DRV_UART_PORT5:
				pHwGPIO = (volatile PGPION)tcc_p2v(HwGPIOD_BASE);
				BITSET(pHwGPIO->GPEN, Hw17); // UTXD(5) Output
				BITCLR(pHwGPIO->GPEN, Hw18); // URXD(5) input
				BITSCLR(pHwGPIO->GPFN2, 0x00000FF0, Hw4|Hw8); // UTXD5, URXD5
				BITSCLR(pHwGPIO->GPPD1, Hw5|Hw4|Hw3|Hw2, Hw4|Hw2); // pull-up enable
				break;
		}
	}
	//DRV_UART_FUNC_SC_CLOSE
	free_irq(INT_UART0+uUartCH, &iIrqData);

	//Init UART Buffer
	tca_sc_init_buffer(uUartCH, DRV_UART_BUFFER_TXRX);
	BITCLR(pHwUART->SCCR, HwUART_SCCR_SEN_DIS);

	tcc_gpio_config(GPIO_CAS_DATA, GPIO_FN(0)|GPIO_PULL_DISABLE);

#else
	BITCLR(pHwUART->SCCR, HwUART_SCCR_SEN_EN);
	
	//Init UART Buffer
	tcc_sc_init_buffer(uUartCH, DRV_UART_BUFFER_TXRX);

	//DRV_UART_FUNC_SC_CLOSE
	free_irq(INT_UART0+uUartCH, &iIrqData);
#endif
	
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

	#if defined(CONFIG_MACH_TCC8800ST)
	TDA8024_VCC_HIGH;
	tca_sc_make_reset(0);	// Reset LOW
	#endif
		
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
	unsigned char 	uCH = gDRV_UART_SC.CHANNEL;
	unsigned		uSendLength;
	unsigned		uCPSR;

	dprintk("%s, length=%d\n", __func__, uLength);
	
	//Set the Base Address
	pHwUART = (volatile PUART)tcc_p2v(SC_UART_ADDR(uCH));

	//Init UART Buffer
	tca_sc_init_buffer(uCH, DRV_UART_BUFFER_TXRX);

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
			BITCLR(pHwUART->SCR, Hw7);
			BITCSET(pHwUART->STC, HwUART_STC_SCNT_MASK, HwUART_STC_SCNT(uLength));
		}

		//Set SmartCard TX
		tca_sc_ctrl_txport(uCH, ENABLE);

		//Set the Length to Send		
		uSendLength = uLength;

		while(uSendLength)
		{
			//Disable the Interrupt
			#if 0
			if((uLength != 1) && (uSendLength == 2))
				uCPSR = IO_INT_DisableINT();
			#endif

			tca_sc_send_byte(uCH, *pucBuffer++);			
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
	unsigned char 	uCH 		= gDRV_UART_SC.CHANNEL;

	dprintk("%s, length=%d\n", __func__, uLength);
	
	//Set the Value of Time-Out
	uTimeOut = uTimeCount = (uLength == 0xFFFFFFFF) ? 30 : 2000;

	//Receive Data from SmartCard 
	while (1)
	{
		uData = tca_sc_rx_pop_char(uCH);
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
	unsigned char 	uCH 		= gDRV_UART_SC.CHANNEL;
	unsigned		iCWTmsec 	= (gCWT/1000) + 1;

	uTimeOut	= iCWTmsec;
	uTimeCount	= iCWTmsec;
#if 1
	tca_sc_ctrl_txport(uCH, DISABLE);
#endif

	//Receive Data and Length from SmartCard 
	while (1)
	{
		uData = tca_sc_rx_pop_char(uCH);
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

int tca_sc_open(struct inode *inode, struct file *filp)
{
	dprintk("%s\n", __func__);
	
	tca_sc_init_port();

	//Init Reset Signal
	tca_sc_make_reset(0);	// Reset LOW

	//Enable Smartcard Interface
	tca_sc_enable();
	
	return 0;
}

int tca_sc_close(struct inode *inode, struct file *file)
{
	dprintk("%s\n", __func__);
	
	//Disable Smartcard Interface
	tca_sc_disable();
	
	//Init Reset Signal
	tca_sc_make_reset(0);	// Reset LOW

	return 0;
}

int tca_sc_init(void)
{
	unsigned char	uCH			= gDRV_UART_SC.CHANNEL;
	dprintk("%s\n", __func__);

	//Init GPIO
	tca_sc_init_port();

	switch(uCH)
	{
		case DRV_UART_CH0:
			gSC_clk = clk_get(0, "uart0");
			break;	
		case DRV_UART_CH1:
			gSC_clk = clk_get(0, "uart1");
			break;	
		case DRV_UART_CH2:
			gSC_clk = clk_get(0, "uart2");
			break;	
		case DRV_UART_CH3:
			gSC_clk = clk_get(0, "uart3");
			break;	
		case DRV_UART_CH4:
			gSC_clk = clk_get(0, "uart4");
			break;	
		case DRV_UART_CH5:
			gSC_clk = clk_get(0, "uart5");
			break;	
		default:
			return 0;
	}
		
	return 0;
}

void tca_sc_exit(void)
{
	dprintk("%s\n", __func__);
	clk_put(gSC_clk);
	return;
}




