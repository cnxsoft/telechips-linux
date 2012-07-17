/*
 * tcc_scaler.c
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/clk.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>
#include <asm/mach/map.h>

#include <mach/bsp.h>

#include <linux/poll.h>

#include <mach/clock.h>
#include "tcc_scaler.h"
#include <mach/tcc_scaler_ctrl.h>

#if 0
static int debug	   = 1;
#else
static int debug	   = 0;
#endif

static unsigned int scaler_ended = 0;

#define dprintk(msg...)	if (debug) { printk( "tcc_scaler1: " msg); }

#define DEVICE_NAME			"scaler1"
#define MAJOR_ID			201
#define MINOR_ID			1
//#define SCALER_USE_INTERRUPT

typedef struct _intr_data_t {
	//wait for Poll!!  
	wait_queue_head_t poll_wq;
	spinlock_t poll_lock;
	unsigned int poll_count;

	struct mutex io_mutex;
	unsigned char irq_reged;
	unsigned int  dev_opened;
} intr_data_t;

static intr_data_t stScale;

static struct clk *scaler_clk;
static struct clk *ddicache_clk;


void tcc_scaler_SetInterruptInit(M2M_CHANNEL ch)
{
	volatile PPIC pPIC;

	pPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);

	if(ch == MSCL0)
	{
		BITCLR(pPIC->MODE0, HwINT0_SC0);	// edge-triggered
		BITCLR(pPIC->MODEA0, HwINT0_SC0);	// single-edge
		BITSET(pPIC->POL0, HwINT0_SC0);		// active-low
	}
	else
	{
		BITCLR(pPIC->MODE0, HwINT0_SC1);	// edge-triggered
		BITCLR(pPIC->MODEA0, HwINT0_SC1);	// single-edge
		BITSET(pPIC->POL0, HwINT0_SC1);		// active-low
	}
}

void tcc_scaler_SetInterrupt(M2M_CHANNEL ch, unsigned int uiFlag)
{
	volatile PPIC pPIC;

	pPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);

	if(ch == MSCL0)
	{
		if(uiFlag & SET_M2MSCL_INT_ENABLE)
		{
			BITSET(pPIC->CLR0, HwINT0_SC0);	
			BITSET(pPIC->INTMSK0, HwINT0_SC0);	
		}  
		if(uiFlag & SET_M2MSCL_INT_DISABLE)
		{
			BITSET(pPIC->CLR0, HwINT0_SC0);	
			BITCLR(pPIC->INTMSK0, HwINT0_SC0);
		}  
	}
	else
	{
		if(uiFlag & SET_M2MSCL_INT_ENABLE)
		{
			BITSET(pPIC->CLR0, HwINT0_SC1);	
			BITSET(pPIC->INTMSK0, HwINT0_SC1);	
		}  
		if(uiFlag & SET_M2MSCL_INT_DISABLE)
		{
			BITSET(pPIC->CLR0, HwINT0_SC1);	
			BITCLR(pPIC->INTMSK0, HwINT0_SC1);
		}  
	}
}

void tcc_scaler_Disable(M2M_CHANNEL ch)
{
#if 1
	volatile PM2MSCALER pM2MScaler;
	unsigned int uiMscStatus = 0, delay_cnt = 0;


	if(ch == MSCL0)
		pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER0_BASE);
	else
		pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER1_BASE);

	BITCLR(pM2MScaler->MSCCTR, HwMSC_CTRL_EN );
	do
	{
		uiMscStatus = pM2MScaler->MSCSTR;

		mdelay(1);
		if( uiMscStatus & HwMSC_STATUS_RDY)
			break;
		delay_cnt++;
		if( delay_cnt > 17 )
			break;

	}while(uiMscStatus & HwMSC_STATUS_BUSY );

#endif
	volatile PDDICONFIG pDDICONFIG;

	pDDICONFIG = (volatile PDDICONFIG)tcc_p2v(HwDDI_CONFIG_BASE);

	if(ch == MSCL0)
	{
		BITSET(pDDICONFIG->SWRESET, HwDDIC_SWRESET_MSCL0);
		BITSET(pDDICONFIG->PWDN, HwDDIC_PWDN_MSCL0);
		BITCLR(pDDICONFIG->SWRESET, HwDDIC_SWRESET_MSCL0);
	}
	else
	{
		BITSET(pDDICONFIG->SWRESET, HwDDIC_SWRESET_MSCL1);
		BITSET(pDDICONFIG->PWDN, HwDDIC_PWDN_MSCL1);
		BITCLR(pDDICONFIG->SWRESET, HwDDIC_SWRESET_MSCL1);
	}
}

void tcc_scaler_Enable(M2M_CHANNEL ch)
{
	volatile PDDICONFIG pDDICONFIG;
#if 1
	volatile PM2MSCALER pM2MScaler;

	if(ch == MSCL0)
		pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER0_BASE);
	else
		pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER1_BASE);

	BITSET(pM2MScaler->MSCCTR, HwMSC_CTRL_EN );

#endif
	pDDICONFIG = (volatile PDDICONFIG)tcc_p2v(HwDDI_CONFIG_BASE);

	if(ch == MSCL0)
	{
		BITSET(pDDICONFIG->SWRESET, HwDDIC_SWRESET_MSCL0);
		BITCLR(pDDICONFIG->PWDN, HwDDIC_PWDN_MSCL0);
		BITCLR(pDDICONFIG->SWRESET, HwDDIC_SWRESET_MSCL0);
	}
	else
	{
		BITSET(pDDICONFIG->SWRESET, HwDDIC_SWRESET_MSCL1);
		BITCLR(pDDICONFIG->PWDN, HwDDIC_PWDN_MSCL1);
		BITCLR(pDDICONFIG->SWRESET, HwDDIC_SWRESET_MSCL1);
	}
}

void tcc_scaler_SWReset(M2M_CHANNEL ch)
{
	volatile PDDICONFIG pDDICONFIG;

	pDDICONFIG = (volatile PDDICONFIG)tcc_p2v(HwDDI_CONFIG_BASE);
#if 1
	volatile PM2MSCALER pM2MScaler;
	unsigned int uiMscStatus = 0, delay_cnt = 0;


	if(ch == MSCL0)
		pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER0_BASE);
	else
		pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER1_BASE);


	uiMscStatus = pM2MScaler->MSCSTR;
	do
	{
		mdelay(1);

		BITCLR(pM2MScaler->MSCCTR, HwMSC_CTRL_EN );
		if( uiMscStatus & HwMSC_STATUS_RDY)
			break;
		delay_cnt++;
		if( delay_cnt > 17 )
			break;

	}while(uiMscStatus & HwMSC_STATUS_BUSY );

//	printk("%s : uiMscStatus = 0x%08x, delay_cnt = %d\n", __func__, uiMscStatus, delay_cnt );
#endif	

	if(ch == MSCL0)
	{
		BITSET(pDDICONFIG->SWRESET, HwDDIC_SWRESET_MSCL0);
		BITCLR(pDDICONFIG->SWRESET, HwDDIC_SWRESET_MSCL0);
	}
	else
	{
		BITSET(pDDICONFIG->SWRESET, HwDDIC_SWRESET_MSCL1);
		BITCLR(pDDICONFIG->SWRESET, HwDDIC_SWRESET_MSCL1);
	}
}


void tcc_scaler_ChangeBaseAddress(int ch, unsigned int pBase0, unsigned int pBase1, unsigned int pBase2)
{
	volatile PM2MSCALER pM2MScaler;
	
	if(ch == MSCL0)
		pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER0_BASE);
	else
		pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER1_BASE);

	BITCSET(pM2MScaler->SRCBASEY, HwMSC_SRC_Y_BASE_ADDR, pBase0);
	BITCSET(pM2MScaler->SRCBASEU, HwMSC_SRC_U_BASE_ADDR, pBase1);
	BITCSET(pM2MScaler->SRCBASEV, HwMSC_SRC_V_BASE_ADDR, pBase2);
}

void tcc_scaler_ChangeDstAddress(int ch, unsigned int pBase0, unsigned int pBase1, unsigned int pBase2)
{
	volatile PM2MSCALER pM2MScaler;
	
	if(ch == MSCL0)
		pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER0_BASE);
	else
		pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER1_BASE);

	BITCSET(pM2MScaler->DSTBASEY, HwMSC_DST_Y_BASE_ADDR, pBase0);
	BITCSET(pM2MScaler->DSTBASEU, HwMSC_DST_U_BASE_ADDR, pBase1);
	BITCSET(pM2MScaler->DSTBASEV, HwMSC_DST_V_BASE_ADDR, pBase2);
}

void tcc_scaler_ControlWithVIQE( M2M_CHANNEL ch,
							unsigned s_y, unsigned s_u, unsigned s_v, unsigned d_y, unsigned d_u, unsigned d_v,
							int s_hsize, int s_vsize, int d_hsize, int d_vsize,
							int crop_top, int crop_bottom, int crop_left, int crop_right)
{
	unsigned int write_val;
	unsigned int pBase0, pBase1, pBase2;
	unsigned int width, height;
	volatile PM2MSCALER pM2MScaler;

	if(ch == MSCL0)
		pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER0_BASE);
	else
		pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER1_BASE);

	BITCSET(pM2MScaler->SRCCFG, 0xffffffff, HwMSC_SRC_CFG_YUV422_SEQ0);

	BITCSET(pM2MScaler->DSTCFG, 0x0000000f, HwMSC_DST_CFG_YUV422_SEQ0);

	pBase0 = s_y + ( (s_hsize * crop_top) + crop_left ) * 2;
	pBase1 = s_u; pBase2 = s_v;
	tcc_scaler_ChangeBaseAddress(ch, pBase0, pBase1, pBase2);
	
	width = ((s_hsize - crop_left - crop_right) >> 3) << 3;		// 8bit align
	height = ((s_vsize - crop_top - crop_bottom) >> 1) << 1;	// 2bit align

	write_val = (height << 16) + width;							// src size
	BITCSET(pM2MScaler->SRCSIZE, 0xffffffff, write_val);

	write_val = ((s_hsize)<<16)+s_hsize;						// src frame offset(YUV420IL) : U/V = s_hsize, Y = s_hsize
	BITCSET(pM2MScaler->SRCOFF, 0xffffffff, write_val);

	tcc_scaler_ChangeDstAddress(ch, d_y, 0, 0);

	write_val = (d_vsize<<16) + d_hsize;						// dst size
	BITCSET(pM2MScaler->DSTSIZE, 0xffffffff, write_val);

	write_val = ((d_hsize)<<16) + d_hsize*2;					// dst frame offset : src offset : U/V = s_hsize, Y = s_hsize*2
	BITCSET(pM2MScaler->DSTOFF, 0xffffffff, write_val);

	write_val = ((256*s_vsize/d_vsize)<<16) + (256*s_hsize/d_hsize);// scale ratio
	BITCSET(pM2MScaler->MSCINF, 0xffffffff, write_val);

	BITCSET(pM2MScaler->MSCCTR, 0xffffffff, HwMSC_INPTH | (0x80000000));	// on the fly
	//BITCSET(pM2MScaler->DSTCFG, HwMSC_DST_CFG_RDY | HwMSC_DST_CFG_PATH, HwMSC_DST_CFG_RDY); // LCD directpath
	BITCSET(pM2MScaler->MSCCTR, 0xffffffff, HwMSC_INPTH | HwMSC_CTRL_RDY_EN | HwMSC_CTRL_EN);	// on the fly start
}

void tcc_scaler_Control60HzWithVIQE( M2M_CHANNEL ch,
							unsigned s_y, unsigned s_u, unsigned s_v,
							int s_hsize, int s_vsize, int d_hsize, int d_vsize,
							int crop_top, int crop_bottom, int crop_left, int crop_right)
{
	unsigned int write_val;
	unsigned int pBase0, pBase1, pBase2;
	unsigned int width, height;
	volatile PM2MSCALER pM2MScaler;
	
	if(ch == MSCL0)
		pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER0_BASE);
	else
		pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER1_BASE);

	BITCSET(pM2MScaler->SRCCFG, 0xffffffff, HwMSC_SRC_CFG_YUV422_SEQ0);	

	BITCSET(pM2MScaler->DSTCFG, 0x0000000f, HwMSC_DST_CFG_YUV422_SEQ0);

	pBase0 = s_y + ( (s_hsize * crop_top) + crop_left ) * 2;
	pBase1 = s_u; pBase2 = s_v;
	tcc_scaler_ChangeBaseAddress(ch, pBase0, pBase1, pBase2);
	
	width = ((s_hsize - crop_left - crop_right) >> 3) << 3;		// 8bit align
	height = ((s_vsize - crop_top - crop_bottom) >> 1) << 1;	// 2bit align

	write_val = (height << 16) + width;							// src size
	BITCSET(pM2MScaler->SRCSIZE, 0xffffffff, write_val);

	write_val = (width << 16) + width;						// src frame offset(YUV420IL) : U/V = s_hsize, Y = s_hsize
	BITCSET(pM2MScaler->SRCOFF, 0xffffffff, write_val);

	write_val = (d_vsize<<16) + d_hsize;						// dst size
	BITCSET(pM2MScaler->DSTSIZE, 0xffffffff, write_val);

	write_val = ((d_hsize)<<16) + d_hsize*2;					// dst frame offset : src offset : U/V = s_hsize, Y = s_hsize*2
	BITCSET(pM2MScaler->DSTOFF, 0xffffffff, write_val);

	write_val = ((256*height/d_vsize)<<16) + (256*width/d_hsize);// scale ratio
	BITCSET(pM2MScaler->MSCINF, 0xffffffff, write_val);

	BITCSET(pM2MScaler->MSCCTR, 0xffffffff, HwMSC_INPTH | (0x80000000));	// on the fly
	BITCSET(pM2MScaler->DSTCFG, HwMSC_DST_CFG_RDY | HwMSC_DST_CFG_PATH, HwMSC_DST_CFG_RDY | HwMSC_DST_CFG_PATH); // LCD directpath
	BITCSET(pM2MScaler->MSCCTR, 0xffffffff, HwMSC_INPTH | HwMSC_CTRL_CON_EN);	// on the fly start
}

void tcc_scaler_Control60HzScalerOnly( M2M_CHANNEL ch, int DI_use,
							unsigned s_y, unsigned s_u, unsigned s_v,
							int s_hsize, int s_vsize, int d_hsize, int d_vsize,
							int crop_top, int crop_bottom, int crop_left, int crop_right)
{
	unsigned int		write_val;
	unsigned int pBase0, pBase1, pBase2;
	unsigned int width, height;
	volatile PM2MSCALER pM2MScaler;

	if(ch == MSCL0)
		pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER0_BASE);
	else
		pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER1_BASE);
	
	BITCSET(pM2MScaler->SRCCFG, 0xffffffff, HwMSC_SRC_CFG_INTLV | HwMSC_SRC_CFG_YUV420_SEP);		

	BITCSET(pM2MScaler->DSTCFG, 0x0000000f, HwMSC_DST_CFG_YUV422_SEQ0);

	pBase0 = s_y + ( (s_hsize * crop_top) + crop_left ) * 2;
	pBase1 = s_u; pBase2 = s_v;
	tcc_scaler_ChangeBaseAddress(ch, pBase0, pBase1, pBase2);

// If we use scaler by de-interlacer, Input screens height should be set half of input height.
	width = ((s_hsize - crop_left - crop_right) >> 3) << 3;		// 8bit align
	if(DI_use == 1)
		height = ((s_vsize / 2 - crop_top - crop_bottom) >> 1) << 1;// 2bit align
	else
		height = ((s_vsize - crop_top - crop_bottom) >> 1) << 1;	// 2bit align

	write_val = (height << 16) + width;							// src size
	BITCSET(pM2MScaler->SRCSIZE, 0xffffffff, write_val);

// If we use scaler by de-interlacer, Input data should be read one line in two line data.
	if(DI_use == 1)
		write_val = ((s_hsize*2)<<16)+s_hsize*2;
	else	// In this case, we will use scaler for display and input video format is YUV420IL. Input data should be read every line.
		write_val = ((s_hsize)<<16)+s_hsize;
		
	BITCSET(pM2MScaler->SRCOFF, 0xffffffff, write_val);

	write_val = (d_vsize<<16) + d_hsize;						// dst size
	BITCSET(pM2MScaler->DSTSIZE, 0xffffffff, write_val);

	write_val = ((d_hsize)<<16) + d_hsize*2;					// dst frame offset : src offset : U/V = s_hsize, Y = s_hsize*2
	BITCSET(pM2MScaler->DSTOFF, 0xffffffff, write_val);

	if(DI_use == 1)
		write_val = ((256*height/d_vsize)<<16) + (256*width/d_hsize);// scale ratio
	else
		write_val = ((256*height/d_vsize)<<16) + (256*width/d_hsize);// scale ratio

	BITCSET(pM2MScaler->MSCINF, 0xffffffff, write_val);

	BITCSET(pM2MScaler->MSCCTR, 0xffffffff, 0x00000000);
	BITCSET(pM2MScaler->DSTCFG, HwMSC_DST_CFG_RDY | HwMSC_DST_CFG_PATH, HwMSC_DST_CFG_RDY | HwMSC_DST_CFG_PATH);	// LCD directpath
	BITCSET(pM2MScaler->MSCCTR, 0xffffffff, HwMSC_CTRL_CON_EN);	// on the fly start
}

void tcc_scaler_Start(M2M_CHANNEL ch)
{
	volatile PM2MSCALER pM2MScaler;
	
	if(ch == MSCL0)
		pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER0_BASE);
	else
		pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER1_BASE);

	BITCSET(pM2MScaler->MSCCTR, 0x00000003, HwMSC_CTRL_RDY_EN | HwMSC_CTRL_EN);
}

// interrupt handler
#ifdef SCALER_USE_INTERRUPT
static irqreturn_t tcc_scaler_handler(int irq, void *client_data)
{  	
	unsigned IFlag;
	volatile PM2MSCALER pM2MScaler;
//	intr_data_t *msc_data = client_data;

//	spin_lock_irq(&(msc_data->poll_lock));
//	msc_data->poll_count++;
//	spin_unlock_irq(&(msc_data->poll_lock));
	
	if(irq == INT_SC0)
		pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER0_BASE);
	else
		pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER1_BASE);
	
	IFlag = pM2MScaler->MSCSTR;

	if (IFlag & HwMSC_STATUS_IRDY)
	{
		scaler_ended = 1;
	}
	else if (IFlag & HwMSC_STATUS_IBUSY)
	{
		scaler_ended = 2;
		printk("interrupt:: scaler busy!!!\n");
	}
	else
	{
		printk("unknown interrupt:: 0x%x!!!\n", IFlag);
	}

	BITCSET(pM2MScaler->MSCCTR, 0xffffffff, 0x00000000);
	pM2MScaler->MSCSTR = IFlag; // clear interrupt flag
	
//	wake_up_interruptible(&(msc_data->poll_wq));

	return IRQ_HANDLED;
}

void tcc_scaler_finalizeWithPolling(void)
{
	intr_data_t *msc_data = &stScale;
	volatile PM2MSCALER pM2MScaler;
	volatile PLCDC pLCDC;
	unsigned int reg;
	
	while(!msc_data->poll_count)
	{
		if(msc_data->poll_count > 0)
		{
			break;
		}
		else
		{
			pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER1_BASE);
			pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
			reg = pLCDC->LSTATUS;
			printk("MSCSTR = 0x%x\n", pM2MScaler->MSCSTR);
			printk("LSTATUS = 0x%x\n", reg);

			pLCDC->LSTATUS = reg;
		}
	}

	spin_lock_irq(&(msc_data->poll_lock));
	msc_data->poll_count--;
	spin_unlock_irq(&(msc_data->poll_lock));

	printk("scaler Done...\n");
}
#endif

unsigned int tcc_scaler_finalize(M2M_CHANNEL ch)
{
	#if 0
	intr_data_t *msc_data = &stScale;
	unsigned int ret;
	
	ret = wait_event_interruptible_timeout(msc_data->poll_wq, msc_data->poll_count > 0, msecs_to_jiffies(200));

	spin_lock_irq(&(msc_data->poll_lock));
	msc_data->poll_count--;
	spin_unlock_irq(&(msc_data->poll_lock));
	
	if(!ret)
	{
		printk("[%d]: scaler timed_out!! \n", ret);
	}
	return ret;
	#else
	volatile unsigned int	IsM2MScalerEnd, Cnt = 0;
	unsigned int IFlag;
	volatile PM2MSCALER pM2MScaler;

	if(ch == MSCL0)
		pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER0_BASE);
	else
		pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER1_BASE);
	
	IsM2MScalerEnd =0;
	
	while(!IsM2MScalerEnd )
	{
		Cnt ++;
		
		IFlag = pM2MScaler->MSCSTR;

		if (IFlag & HwMSC_STATUS_IRDY)
		{
			IsM2MScalerEnd =1;
		}
		else if (IFlag & HwMSC_STATUS_IBUSY)
		{
			IsM2MScalerEnd =2;
		}		

		if(Cnt > 0xF0000)
		{
			IsM2MScalerEnd = 3;
		}
	}
	pM2MScaler->MSCSTR = IFlag; // clear interrupt flag	
	//printk("Done!!\n");
	return IsM2MScalerEnd;
	#endif
}

void tcc_scaler_clk_onoff(M2M_CHANNEL ch, int onOff )
{
	unsigned int bit;
	volatile PDDICONFIG pDDICONFIG;
	
	pDDICONFIG = (volatile PDDICONFIG)tcc_p2v(HwDDI_CONFIG_BASE);
	
	(ch == MSCL0) ? (bit = HwDDIC_PWDN_MSCL0) : (bit = HwDDIC_PWDN_MSCL1);
	
	if(onOff)
	{
		BITCLR(pDDICONFIG->PWDN, bit);
		BITCLR(pDDICONFIG->PWDN, HwDDIC_PWDN_DDIC);
	}
	else
	{
		BITSET(pDDICONFIG->PWDN, bit);
		BITSET(pDDICONFIG->PWDN, HwDDIC_PWDN_DDIC);
	}

	printk("%s : ch %d, onOff %d, DDIConfig PWDN : 0x%x, SWRESET : 0x%x\n", __func__, ch, onOff, pDDICONFIG->PWDN, pDDICONFIG->SWRESET);
}

#ifdef SCALER_USE_INTERRUPT
int tcc_scaler_interrupt_enable(M2M_CHANNEL ch)
{
	int ret;
	
	spin_lock_init(&(stScale.poll_lock));
	init_waitqueue_head(&(stScale.poll_wq));
	
	if(!stScale.irq_reged)
	{
		tcc_scaler_SetInterruptInit(ch);
		tcc_scaler_SetInterrupt(ch, SET_M2MSCL_INT_ENABLE);

		ret = request_irq(INT_SC1, tcc_scaler_handler, IRQF_DISABLED, "scaler_60hz", &stScale);
		if (ret)
		{
			tcc_scaler_clk_onoff(ch, 0);
			printk("FAILED to aquire scaler1-irq\n");
			return -EFAULT;
		}
		stScale.irq_reged = 1;
	}
	
	dprintk("scaler%d_open Out!! %d'th \n", ch, stScale.dev_opened);
}
#endif

int tcc_scaler_open(M2M_CHANNEL ch)
{	
	int ret = 0;

	memset(&stScale, 0, sizeof(intr_data_t));

	if(ch == MSCL0)
		scaler_clk = clk_get(NULL, "m2m0");
	else
		scaler_clk = clk_get(NULL, "m2m1");
	
	BUG_ON(scaler_clk == NULL);

	ddicache_clk = clk_get(NULL, "ddi_cache");
	BUG_ON(ddicache_clk == NULL);
	
	clk_enable(scaler_clk);
	clk_enable(ddicache_clk);
	
	tcc_scaler_Enable(ch);

	stScale.dev_opened++;
	return ret;	
}

int tcc_scaler_close(M2M_CHANNEL ch)
{
	printk("scaler%d_close In!! %d'th, poll(%d), irq(%d)  \n", ch, stScale.dev_opened, stScale.poll_count, stScale.irq_reged);

	if(stScale.dev_opened > 0)
		stScale.dev_opened--;
	else
		return 0;

#ifdef SCALER_USE_INTERRUPT
	if(stScale.dev_opened == 0)
	{
		if(stScale.irq_reged)
		{
			if(ch == 0)
				free_irq(INT_SC0, &stScale);
			else
				free_irq(INT_SC1, &stScale);
			
			stScale.irq_reged = 0;
		}
		stScale.poll_count = 0;
	}
#endif	

	clk_disable(scaler_clk);
	clk_disable(ddicache_clk);

	printk("scaler%d_close Out!! %d'th \n", ch, stScale.dev_opened);

	return 0;
}

