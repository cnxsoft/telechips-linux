/*
 * linux/drivers/video/tcc/tccfb.c
 *
 * Based on:    Based on s3c2410fb.c, sa1100fb.c and others
 * Author:  <linux@telechips.com>
 * Created: June 10, 2008
 * Description: TCC LCD Controller Frame Buffer Driver
 *
 * Copyright (C) 2008-2009 Telechips
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see the file COPYING, or write
 * to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/err.h>
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

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>
#include <asm/mach/map.h>
#include <asm/mach-types.h>
#ifdef CONFIG_PM
#include <linux/pm.h>
#endif
#include <asm/tlbflush.h>
#include <asm/cacheflush.h>

#include <mach/bsp.h>
#include <mach/tca_ckc.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif
#include "tccfb.h"
#include <plat/pmap.h>
#include <mach/tcc_fb.h>
#include <mach/tcc_scaler_ctrl.h>
#include <mach/tccfb_ioctrl.h>
#include <mach/tcc_composite_ioctl.h>
#include <mach/tcc_component_ioctl.h>
#include <mach/TCC_LCD_Interface.h>

#include <mach/tca_fb_hdmi.h>
#include <mach/tca_fb_output.h>
#include <mach/tca_lcdc.h>
#include <mach/globals.h>
#include <linux/console.h>
#include <mach/vioc_rdma.h>
#include <mach/vioc_disp.h>
#ifdef TCC_VIDEO_DISPLAY_DEINTERLACE_MODE
#include "tcc_vioc_viqe_interface.h"
#include "viqe.h"
#endif


static unsigned int EX_OUT_LCDC;
static unsigned int LCD_LCDC_NUM;

#define TCC_FB_DOUBLE

#define VIQE_DUPLICATE_ROUTINE

/* Debugging stuff */
static int debug = 0;
#define dprintk(msg...)	if (debug) { printk( "tccfb: " msg); }

static int screen_debug = 0;
#define sprintk(msg...)	if (screen_debug) { printk( "tcc92fb scr: " msg); }

extern int tca_fb_pan_display(struct fb_var_screeninfo *var, struct fb_info *info);
extern void tca_fb_activate_var(struct tccfb_info *fbi,  struct fb_var_screeninfo *var);
#ifdef CONFIG_HAS_EARLYSUSPEND
extern void tca_fb_early_suspend(struct early_suspend *h);
extern void tca_fb_earlier_suspend(struct early_suspend *h);
extern void tca_fb_late_resume(struct early_suspend *h);
extern void tca_fb_later_resume(struct early_suspend *h);
#endif
extern int tca_fb_suspend(struct platform_device *dev, pm_message_t state);
extern int tca_fb_resume(struct platform_device *dev);
extern int tca_fb_init(void);
extern void tca_fb_exit(void);
extern int tcc_lcd_interrupt_reg(char SetClear);

#if defined(CONFIG_FB_TCC_COMPOSITE)
extern int tcc_composite_detect(void);
extern void tcc_composite_update(struct tcc_lcdc_image_update *update);
#endif

#if defined(CONFIG_FB_TCC_COMPONENT)
extern int tcc_component_detect(void);
extern void tcc_component_update(struct tcc_lcdc_image_update *update);
#endif

extern unsigned int tca_get_lcd_lcdc_num(viod);
extern unsigned int tca_get_output_lcdc_num(viod);

static pmap_t pmap_fb_video;
#define FB_VIDEO_MEM_BASE	pmap_fb_video.base
#define FB_VIDEO_MEM_SIZE	pmap_fb_video.size


#define CONFIG_FB_TCC_DEVS_MAX	3	// do not change!
#define CONFIG_FB_TCC_DEVS		1

#if (CONFIG_FB_TCC_DEVS > CONFIG_FB_TCC_DEVS_MAX)
	#undef CONFIG_FB_TCC_DEVS
	#define CONFIG_FB_TCC_DEVS	CONFIG_FB_TCC_DEVS_MAX
#endif


#define SCREEN_DEPTH_MAX	32	// 32 or 16
//								 : 32 - 32bpp(alpah+rgb888)
//								 : 16 - 16bpp(rgb565)


const unsigned int default_scn_depth[CONFIG_FB_TCC_DEVS_MAX] =
{
/* fb0, Layer0: bottom */  (16), // 32 or 16
/* fb1, Layer1: middle */  (16), //  "
/* fb2, Layer2: top    */  (16)  //  "
};


#define LCD_OUT_LCDC 	1
static struct lcd_panel *lcd_panel;


TCC_OUTPUT_TYPE	Output_SelectMode =  TCC_OUTPUT_NONE;
static unsigned int Output_BaseAddr;


static char  HDMI_pause = 0;
static char HDMI_video_mode = 0;

static unsigned int HDMI_video_width = 0;
static unsigned int HDMI_video_height = 0;
static unsigned int HDMI_video_hz = 0;


char fb_power_state;

// for frame buffer clear
static u_char *fb_mem_vaddr[CONFIG_FB_TCC_DEVS]= {0,};
static u_int   fb_mem_size [CONFIG_FB_TCC_DEVS]= {0,};

#ifdef TCC_VIDEO_DISPLAY_BY_VSYNC_INT

static int debug_v = 0;
#define vprintk(msg...)	if (debug_v) { printk( "tccfb_vsync: " msg); }
static int testToggleBit1=0;

static int testToggleBit=0;
inline static int tcc_vsync_get_time(void);
inline static void tcc_vsync_set_time(int time);
#define USE_VSYNC_TIMER
//#define USE_SOFT_IRQ_FOR_VSYNC

#define TIME_BUFFER_COUNT			30
// if time gab of video frame over base time is bigger than this value, that frame is skipped.
#define VIDEO_SYNC_MARGIN_LATE		50
// if time gap of video frame over base time is less than this value, that frame is displayed immediately
#define VIDEO_SYNC_MARGIN_EARLY		50

#define TIME_MARK_SKIP				(-1)

typedef struct{
	int readIdx ;
	int writeIdx ;
	int clearIdx;

	int valid_buff_count;
	int readable_buff_count;

	int max_buff_num ;
	int last_cleared_buff_id;
	int available_buffer_id_on_vpu;
	int cur_lcdc_buff_addr;
	struct tcc_lcdc_image_update stImage[6] ;
}tcc_vsync_buffer_t;

typedef struct {
	tcc_vsync_buffer_t vsync_buffer;

	int isVsyncRunning;
	// for time sync
	unsigned int unVsyncCnt ;
	int baseTime;
	unsigned int timeGapIdx ;
	unsigned int timeGapBufferFullFlag;
	int timeGap[TIME_BUFFER_COUNT];
	int timeGapTotal;
	int updateGapTime;
	int vsync_interval;
	int perfect_vsync_flag;

	int skipFrameStatus;
	int overlayUsedFlag;
	int outputMode;
	int video_frame_rate;

	//for deinterlace mode
	int deinterlace_mode;
	int firstFrameFlag;
	int frameInfo_interlace;
	int m2m_mode;
	int output_toMemory;
	int nDeinterProcCount;
	int nTimeGapToNextField;
	int interlace_output;
	int interlace_bypass_lcdc;
}tcc_video_disp ;

typedef struct _tcc_viqe_param_t
{
	int mode;
	int region;
	int strength1;
	int strength2;
	int modeparam;
} tcc_viqe_param_t;

tcc_viqe_param_t viqe_param;

spinlock_t vsync_lock ;
spinlock_t vsync_lockDisp ;

tcc_video_disp tccvid_vsync ;
static int vsync_started = 0;

DECLARE_WAIT_QUEUE_HEAD( wq_consume ) ;
DECLARE_WAIT_QUEUE_HEAD( wq_consume1 ) ;

#ifdef USE_SOFT_IRQ_FOR_VSYNC
static struct work_struct vsync_work_q;
#endif

inline static int tcc_vsync_set_max_buffer(tcc_vsync_buffer_t * buffer_t, int buffer_count)
{
	buffer_t->max_buff_num = buffer_count;
	return buffer_count;
}

inline static int tcc_vsync_push_buffer(tcc_vsync_buffer_t * buffer_t, struct tcc_lcdc_image_update* inputData)
{

	if(buffer_t->valid_buff_count >= buffer_t->max_buff_num || buffer_t->readable_buff_count >= buffer_t->max_buff_num)
	{
		printk("error: buffer full %d, max %d %d ts %d \n", buffer_t->valid_buff_count, buffer_t->max_buff_num,buffer_t->readable_buff_count,inputData->time_stamp);
		return -1;
	}

	memcpy(&(buffer_t->stImage[buffer_t->writeIdx]),(void*)inputData, sizeof(struct tcc_lcdc_image_update) );
	
	//printk("tcc_vsync_push_buffer writeIdx(%d) valid_buff_count %d buffer_unique_id %d addr0 %x \n", 
	//	buffer_t->writeIdx,buffer_t->valid_buff_count, buffer_t->stImage[buffer_t->writeIdx].buffer_unique_id,buffer_t->stImage[buffer_t->writeIdx].addr0);
		
	if(++buffer_t->writeIdx >= buffer_t->max_buff_num)
		buffer_t->writeIdx = 0;
	

	buffer_t->valid_buff_count++;
	buffer_t->readable_buff_count++;
	
	return 0;
}

inline static int tcc_vsync_pop_buffer(tcc_vsync_buffer_t * buffer_t)
{
	if(buffer_t->readable_buff_count == 0)
	{
		printk("error: buffer empty \n");
		return -1;
	}

	if(++buffer_t->readIdx >= buffer_t->max_buff_num)
		buffer_t->readIdx = 0;

	buffer_t->readable_buff_count--;

	//printk("tcc_vsync_pop_buffer readIdxbuffer_t(%d) writeIdx(%d) valid_ valid_count %d readble %d\n", 
	//	buffer_t->readIdx,buffer_t->writeIdx,buffer_t->valid_buff_count,buffer_t->readable_buff_count);

	return buffer_t->readable_buff_count;
}

inline static void* tcc_vsync_get_buffer(tcc_vsync_buffer_t * buffer_t, int offset)
{
	int readIdx;

	if((buffer_t->readable_buff_count-offset) > 0)
	{
		readIdx = buffer_t->readIdx;

		while(offset)
		{
			if(++readIdx >= buffer_t->max_buff_num)
				readIdx = 0;

			offset--;
		}

		return (void*)&(buffer_t->stImage[readIdx]);
		
	}
	else
	{
		return 0;
	}
}

inline static int tcc_vsync_clean_buffer(tcc_vsync_buffer_t * buffer_t)
{
	if(buffer_t->readIdx == buffer_t->clearIdx || buffer_t->valid_buff_count == 0)
	{
		vprintk("error: no clean buffer clearIdx(%d) valid_buff_count(%d) \n",  buffer_t->clearIdx,buffer_t->valid_buff_count );
		return -1;
	}

	vprintk("tcc_vsync_clean_buffer start clearIdx(%d) readIdx(%d) writeIdx(%d) valid_buff_count %d  \n", buffer_t->clearIdx,buffer_t->readIdx,buffer_t->writeIdx,buffer_t->valid_buff_count);
	do
	{
		if(++buffer_t->clearIdx >= buffer_t->max_buff_num)
			buffer_t->clearIdx = 0;
		
		if(buffer_t->last_cleared_buff_id < buffer_t->stImage[buffer_t->clearIdx].buffer_unique_id)
			buffer_t->last_cleared_buff_id = buffer_t->stImage[buffer_t->clearIdx].buffer_unique_id;

		buffer_t->valid_buff_count--;

	}while(buffer_t->readIdx != buffer_t->clearIdx);
	wake_up_interruptible( &wq_consume ) ;
	vprintk("tcc_vsync_clean_buffer valid_buff_count %d  \n", buffer_t->valid_buff_count);
	//printk("tcc_vsync_clean_buffer clearIdx(%d) readIdx(%d) writeIdx(%d) buff_id %d valid_count %d readble %d \n",
	//	buffer_t->clearIdx,buffer_t->readIdx,buffer_t->writeIdx,buffer_t->last_cleared_buff_id,buffer_t->valid_buff_count,buffer_t->readable_buff_count);
	return buffer_t->valid_buff_count;
}


inline static int tcc_vsync_pop_all_buffer(tcc_vsync_buffer_t * buffer_t)
{

	if(buffer_t->valid_buff_count == 0)
	{
		vprintk("error: buffer empty \n");
		return -1;
	}

	while(buffer_t->valid_buff_count)
	{
		if(buffer_t->last_cleared_buff_id < buffer_t->stImage[buffer_t->clearIdx].buffer_unique_id)
			buffer_t->last_cleared_buff_id = buffer_t->stImage[buffer_t->clearIdx].buffer_unique_id;

		if(++buffer_t->clearIdx >= buffer_t->max_buff_num)
			buffer_t->clearIdx = 0;

		buffer_t->valid_buff_count--;
	}

	buffer_t->readIdx = buffer_t->clearIdx;
	buffer_t->readable_buff_count = 0;
	printk("tcc_vsync_pop_all_buffer valid_buff_count %d last_cleared_buff_id %d  \n", buffer_t->valid_buff_count,buffer_t->last_cleared_buff_id);
	return 0;
}


inline static int tcc_vsync_is_full_buffer(tcc_vsync_buffer_t * buffer_t)
{
	if(buffer_t->valid_buff_count >= buffer_t->max_buff_num)
		return 1;
	else
		return 0;
}

inline static int tcc_vsync_is_empty_buffer(tcc_vsync_buffer_t * buffer_t)
{
	if(buffer_t->valid_buff_count > 0)
		return 0;
	else
		return 1;
}

static void tcc_check_interlace_output(int output_mode)
{
	VIOC_DISP * pDISPBase;
	char output_lcdc = EX_OUT_LCDC;

	if(output_lcdc)
		pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP1);
	else
		pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP0);
	
	if(pDISPBase->uCTRL.nREG & HwDISP_NI)
		tccvid_vsync.interlace_output = 0;
	else
		tccvid_vsync.interlace_output = 1;
}

static void DisplayUpdate(void)
{
	int current_time;
	int time_gap;
	static int time_gap_sign = 1;
	int valid_buff_count;
	int i;
	
	
	struct tcc_lcdc_image_update *pNextImage;

	if(tccvid_vsync.vsync_buffer.readable_buff_count < 2)
	{
		vprintk("There is no output data\n");
		//return;
	}

	current_time = tcc_vsync_get_time();
	tcc_vsync_clean_buffer(&tccvid_vsync.vsync_buffer);

	valid_buff_count = tccvid_vsync.vsync_buffer.readable_buff_count;
	valid_buff_count--;
	//printk("DisplayUpdate valid_buff_count : %d readIdx %d \n", valid_buff_count,tccvid_vsync.vsync_buffer.readIdx) ;
	//vprintk("DisplayUpdate valid_buff_count : %d\n", valid_buff_count) ;
	
	for(i=0; i < valid_buff_count; i++)
	{
		pNextImage = tcc_vsync_get_buffer(&tccvid_vsync.vsync_buffer, 0);
		vprintk("DisplayUpdate pNextImage->time_stamp : %d %d\n", pNextImage->time_stamp,current_time) ;
		//printk("DisplayUpdateI time_stamp(%d), current_time(%d) sync_time(%d)\n", pNextImage->time_stamp, current_time,pNextImage->sync_time) ;

		time_gap = (current_time+VIDEO_SYNC_MARGIN_EARLY) - pNextImage->time_stamp;
		if(time_gap >= 0)
		{
			tcc_vsync_pop_buffer(&tccvid_vsync.vsync_buffer);

			if(tccvid_vsync.perfect_vsync_flag == 1 && time_gap < 4 )
			{
				tcc_vsync_set_time(current_time + (tccvid_vsync.vsync_interval>>1)*time_gap_sign);
				if(time_gap_sign > 0)
					time_gap_sign = -1;
				else
					time_gap_sign = 1;
			}
		}
		else
		{
			break;
		}
	}

	pNextImage = &tccvid_vsync.vsync_buffer.stImage[tccvid_vsync.vsync_buffer.readIdx];
	
	if(tccvid_vsync.outputMode == Output_SelectMode && tccvid_vsync.vsync_buffer.cur_lcdc_buff_addr != pNextImage->addr0  )
	{
		//printk("mt(%d), remain_buff(%d) readIdx(%d) write(%d) readaddr(%x) cleared_buff_id(%d) buffer_unique_id %d\n", pNextImage->time_stamp, tccvid_vsync.vsync_buffer.readable_buff_count,
		//	tccvid_vsync.vsync_buffer.readIdx,tccvid_vsync.vsync_buffer.writeIdx,pNextImage->addr0,tccvid_vsync.vsync_buffer.last_cleared_buff_id,tccvid_vsync.vsync_buffer.stImage[tccvid_vsync.vsync_buffer.readIdx].buffer_unique_id) ;
		tccvid_vsync.vsync_buffer.cur_lcdc_buff_addr = pNextImage->addr0;
#if 0
	    if(testToggleBit)
	    {
			PGPION hwGPIOC;
	        testToggleBit = 0; 

			hwGPIOC = (volatile PGPION)tcc_p2v(HwGPIOC_BASE);
			//hwGPIOC->GPEN.nREG|= (unsigned int)(0x00004000);
			//hwGPIOC->GPDAT.nREG |= (unsigned int)(0x00004000);
			hwGPIOC->GPFN1.bREG.GPFN14 = 0;
			hwGPIOC->GPEN.bREG.GP14 = 1;
			hwGPIOC->GPDAT.bREG.GP14 = 1;

			
	    }
	    else
	    {
			PGPION hwGPIOC;
	        testToggleBit = 1;

			hwGPIOC = (volatile PGPION)tcc_p2v(HwGPIOC_BASE);
			//hwGPIOC->GPEN.nREG |= (unsigned int)(0x00004000);
			//hwGPIOC->GPDAT.nREG &= (unsigned int)(~0x00004000);
			hwGPIOC->GPFN1.bREG.GPFN14 = 0;
			hwGPIOC->GPEN.bREG.GP14 = 1;
			hwGPIOC->GPDAT.bREG.GP14 = 0;

	    }
#endif
		switch(Output_SelectMode)
		{
			case TCC_OUTPUT_NONE:
				TCC_HDMI_DISPLAY_UPDATE(LCD_OUT_LCDC, pNextImage);
				break;
			case TCC_OUTPUT_HDMI:
				TCC_HDMI_DISPLAY_UPDATE(EX_OUT_LCDC, pNextImage);
				break;
			case TCC_OUTPUT_COMPONENT:
				#if defined(CONFIG_FB_TCC_COMPONENT)
					tcc_component_update(pNextImage);
				#endif
				break;
			case TCC_OUTPUT_COMPOSITE:
				#if defined(CONFIG_FB_TCC_COMPOSITE)
					tcc_composite_update(pNextImage);
				#endif
				break;
				
			default:
				break;
		}
	}
	return;
}

#ifdef TCC_VIDEO_DISPLAY_DEINTERLACE_MODE
static int SavedOddfirst = -1;

static int byPassImageToLCDC(struct tcc_lcdc_image_update *pImage, int ref_cnt, int lcdc_num)
{
	VIOC_DISP * pDISPBase;
	unsigned int lstatus = 0;
	int ret = 0;
	
	if(lcdc_num)
		pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP1);
	else
		pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP0);
	
	lstatus = pDISPBase->uLSTATUS.nREG;
	
	SavedOddfirst = pImage->odd_first_flag;
		
	#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
		/* check output path flag */
		if(pImage->output_path)
		{
			if(output_path_addr != pImage->addr0)
			{
				/* bottom field first */
				if(SavedOddfirst == 1)
				{
					/* current even field : top field displaying now */
					if(ISSET(lstatus, HwLSTATUS_EF))
					{
						output_path_addr = pImage->addr0;
						//printk("bf, 0x%08x update\n", output_path_addr);
					}
					/* current odd field : bottom field displaying now */
					else
					{
						return ret;
					}
				}
				/* top field first */
				else
				{
					/* current even field : top field displaying now */
					if(ISSET(lstatus, HwLSTATUS_EF))
					{
						return ret;
					}
					/* current odd field : bottom field displaying now */
					else
					{
						output_path_addr = pImage->addr0;
						//printk("ef, 0x%08x update\n", output_path_addr);
					}
				}
			}
			else
			{
				//printk("@@@-%c-%c-0x%08x-%d\n", SavedOddfirst? 'o':'e', ISSET(lstatus, HwLSTATUS_EF)? 'e':'o', pImage->addr0, pImage->output_path);
				return ret;
			}
		}
		else
		{
			/* clear ouput path address */
			output_path_addr = 0;
		}
	#endif

	if(SavedOddfirst==0){
		if(tccvid_vsync.nDeinterProcCount ==0){
			if(ISSET(lstatus, HwLSTATUS_EF)){
				ret = 1;
			}
		}
		else{
			if(!ISSET(lstatus, HwLSTATUS_EF)){
				ret = 1;
			}
		}
	}
	else{

		if(tccvid_vsync.nDeinterProcCount ==1){
			if(ISSET(lstatus, HwLSTATUS_EF)){
				ret = 1;
			}
		}
		else{
			if(!ISSET(lstatus, HwLSTATUS_EF)){
				ret = 1;
			}
		}
	}
	
	if(ret == 1){
		printk("nDeinterProcCount(%d), SavedOddfirst(%d)\n", tccvid_vsync.nDeinterProcCount, SavedOddfirst) ;
		return ret;
	}
	
	switch(Output_SelectMode)
	{
		case TCC_OUTPUT_NONE:
			TCC_HDMI_DISPLAY_UPDATE(LCD_OUT_LCDC, pImage);
			break;
		case TCC_OUTPUT_HDMI:
			TCC_HDMI_DISPLAY_UPDATE(EX_OUT_LCDC, pImage);
			break;
		case TCC_OUTPUT_COMPONENT:
			#if defined(CONFIG_FB_TCC_COMPONENT)
				tcc_component_update(pImage);
			#endif
			break;
		case TCC_OUTPUT_COMPOSITE:
			#if defined(CONFIG_FB_TCC_COMPOSITE)
				tcc_composite_update(pImage);
			#endif
			break;
			
		default:
			break;
	}

	return ret;

}

static void DisplayUpdateWithDeinterlace(void)
{
	int current_time;
	int time_gap,time_diff;
	static int time_gap_sign = 1;
	int valid_buff_count;
	int i;
	static struct tcc_lcdc_image_update *pNextImage;

	if(tccvid_vsync.vsync_buffer.readable_buff_count < 3)
	{
		vprintk("There is no output data.\n");
		//return;
	}

	current_time = tcc_vsync_get_time();

	#ifndef VIQE_DUPLICATE_ROUTINE
	if(tccvid_vsync.nDeinterProcCount == 0)
	{
		tcc_vsync_clean_buffer(&tccvid_vsync.vsync_buffer);

		valid_buff_count = tccvid_vsync.vsync_buffer.readable_buff_count;
		valid_buff_count--;
		for(i=0; i < valid_buff_count; i++)
		{
			pNextImage = tcc_vsync_get_buffer(&tccvid_vsync.vsync_buffer, 0);

			time_gap = (current_time+VIDEO_SYNC_MARGIN_EARLY) - pNextImage->time_stamp;
			if(time_gap >= 0)
			{
				tcc_vsync_pop_buffer(&tccvid_vsync.vsync_buffer);

				if(tccvid_vsync.perfect_vsync_flag == 1 && time_gap < 4 )
				{
					tcc_vsync_set_time(current_time + (tccvid_vsync.vsync_interval>>1)*time_gap_sign);
					if(time_gap_sign > 0)
						time_gap_sign = -1;
					else
						time_gap_sign = 1;
				}
			}
			else
			{
				break;
			}
		}


		pNextImage = &tccvid_vsync.vsync_buffer.stImage[tccvid_vsync.vsync_buffer.readIdx];
//		printk("mt(%d), st(%d)\n", pNextImage->time_stamp, current_time) ;
		time_diff = abs (( pNextImage->time_stamp - (current_time+VIDEO_SYNC_MARGIN_EARLY) ));
		
		if(tccvid_vsync.interlace_bypass_lcdc ||(tccvid_vsync.vsync_buffer.cur_lcdc_buff_addr != pNextImage->addr0))// &&  time_diff < 100) )
		{
			tccvid_vsync.vsync_buffer.cur_lcdc_buff_addr = pNextImage->addr0;

			if(tccvid_vsync.outputMode == Output_SelectMode)
			{
				if(!tccvid_vsync.interlace_bypass_lcdc)
				{
					TCC_VIQE_DI_Run60Hz(pNextImage->on_the_fly, pNextImage->addr0, pNextImage->addr1, pNextImage->addr2,
										pNextImage->Frame_width, pNextImage->Frame_height,
										pNextImage->crop_top,pNextImage->crop_bottom, pNextImage->crop_left, pNextImage->crop_right, 
										pNextImage->Image_width, pNextImage->Image_height, 
										pNextImage->offset_x, pNextImage->offset_y, pNextImage->odd_first_flag, pNextImage->frameInfo_interlace);
				}
				else
				{
					
					if(byPassImageToLCDC(pNextImage, 0, EX_OUT_LCDC) == 1){
						tccvid_vsync.nDeinterProcCount =0;
						return;
					}
				}
			}	
			tccvid_vsync.nDeinterProcCount ++;
		}
		
	}
	else
	{
		if(tccvid_vsync.outputMode == Output_SelectMode)
		{
			if(!tccvid_vsync.interlace_bypass_lcdc)
			{
				TCC_VIQE_DI_Run60Hz(pNextImage->on_the_fly, pNextImage->addr0, pNextImage->addr1, pNextImage->addr2,
									pNextImage->Frame_width, pNextImage->Frame_height,
									pNextImage->crop_top,pNextImage->crop_bottom, pNextImage->crop_left, pNextImage->crop_right, 
									pNextImage->Image_width, pNextImage->Image_height,
									pNextImage->offset_x, pNextImage->offset_y, pNextImage->odd_first_flag^0x01, pNextImage->frameInfo_interlace);
			}
		}
		tccvid_vsync.nDeinterProcCount = 0;

	}
	#else
	
	if(tccvid_vsync.nDeinterProcCount == 0)
	{
		tcc_vsync_clean_buffer(&tccvid_vsync.vsync_buffer);

		valid_buff_count = tccvid_vsync.vsync_buffer.readable_buff_count;
		valid_buff_count--;
		for(i=0; i < valid_buff_count; i++)
		{
			pNextImage = tcc_vsync_get_buffer(&tccvid_vsync.vsync_buffer, 0);

			time_gap = (current_time+VIDEO_SYNC_MARGIN_EARLY) - pNextImage->time_stamp;
			if(time_gap >= 0)
			{
				tcc_vsync_pop_buffer(&tccvid_vsync.vsync_buffer);

				if(tccvid_vsync.perfect_vsync_flag == 1 && time_gap < 4 )
				{
					tcc_vsync_set_time(current_time + (tccvid_vsync.vsync_interval>>1)*time_gap_sign);
					if(time_gap_sign > 0)
						time_gap_sign = -1;
					else
						time_gap_sign = 1;
				}
			}
			else
			{
				break;
			}
		}
		//printk("mt(%d), st(%d)\n", pNextImage->time_stamp, current_time) ;
		
		//printk("C(%d), pass(%d)\n", tccvid_vsync.nDeinterProcCount,tccvid_vsync.interlace_bypass_lcdc) ;

		if(tccvid_vsync.interlace_bypass_lcdc )
		{
			pNextImage = &tccvid_vsync.vsync_buffer.stImage[tccvid_vsync.vsync_buffer.readIdx];
			tccvid_vsync.vsync_buffer.cur_lcdc_buff_addr = pNextImage->addr0;

			if(tccvid_vsync.outputMode == Output_SelectMode)
			{
				if(byPassImageToLCDC(pNextImage, 0, EX_OUT_LCDC) == 1){
					tccvid_vsync.nDeinterProcCount =0;
					return;
				}
			}	
		}
		tccvid_vsync.nDeinterProcCount ++;
	}
	else{
		tccvid_vsync.nDeinterProcCount = 0;
	}
	
	if(!tccvid_vsync.interlace_bypass_lcdc )
	viqe_render_field(current_time);
	
	#endif
	return;
}
#endif // TCC_VIDEO_DISPLAY_DEINTERLACE_MODE

void tca_video_vsync_interrupt_onoff(int onoff, int lcdc_num)
{
	VIOC_RDMA *pRDMABase;
	
	if(lcdc_num)
		pRDMABase = (volatile VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA04);
	else
		pRDMABase = (volatile VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA00);

	VIOC_RDMA_SetIreqMask(pRDMABase, VIOC_RDMA_IREQ_ALL_MASK, 1);

	if(onoff)
		VIOC_RDMA_SetIreqMask(pRDMABase, VIOC_RDMA_IREQ_IEOFF_MASK, 0);
	else
		VIOC_RDMA_SetIreqMask(pRDMABase, VIOC_RDMA_IREQ_IEOFF_MASK, 0);
}

void tca_video_vsync_interrupt_maskset(int lcdc_num)
{
	VIOC_RDMA *pRDMABase;
	
	if(lcdc_num)
		pRDMABase = (volatile VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA04);
	else
		pRDMABase = (volatile VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA00);

	VIOC_RDMA_SetStatus(pRDMABase, VIOC_RDMA_STAT_IEOFF);
}

static irqreturn_t tcc_lcd_handler0_for_video(int irq, void *dev_id)
{
	tca_video_vsync_interrupt_maskset(0);
	tca_video_vsync_interrupt_onoff(1, 0);

	#ifdef USE_SOFT_IRQ_FOR_VSYNC
		if (schedule_work(&vsync_work_q) == 0 ) {
			printk("vsync error:cannot schedule work !!!\n");
		}
#else

#ifndef USE_VSYNC_TIMER
		if((++tccvid_vsync.unVsyncCnt) &0x01)
			tccvid_vsync.baseTime += 16;
		else
			tccvid_vsync.baseTime += 17;

		if(!(tccvid_vsync.unVsyncCnt%6))
			tccvid_vsync.baseTime++;
#endif

#ifndef VIQE_DUPLICATE_ROUTINE
		if( tcc_vsync_is_empty_buffer(&tccvid_vsync.vsync_buffer) == 0 )
		{
			#ifdef TCC_VIDEO_DISPLAY_DEINTERLACE_MODE
				if(tccvid_vsync.deinterlace_mode && !tccvid_vsync.output_toMemory)
					DisplayUpdateWithDeinterlace();
				else
			#endif
					DisplayUpdate();				

			#if defined(CONFIG_TCC_OUTPUT_ATTACH)
				TCC_OUTPUT_FB_AttachUpdateFlag(0);
			#endif
		}
#else
		#ifdef TCC_VIDEO_DISPLAY_DEINTERLACE_MODE
		if(tccvid_vsync.deinterlace_mode && !tccvid_vsync.output_toMemory){
			if(tccvid_vsync.interlace_bypass_lcdc){
				if( tcc_vsync_is_empty_buffer(&tccvid_vsync.vsync_buffer) == 0 )
					DisplayUpdateWithDeinterlace();
			}
			else{
			DisplayUpdateWithDeinterlace();
			}
			
			#if defined(CONFIG_TCC_OUTPUT_ATTACH)
				TCC_OUTPUT_FB_AttachUpdateFlag(0);
			#endif
		}
		else
		#endif
		if( tcc_vsync_is_empty_buffer(&tccvid_vsync.vsync_buffer) == 0 )
		{
			DisplayUpdate();
			#if defined(CONFIG_TCC_OUTPUT_ATTACH)
				TCC_OUTPUT_FB_AttachUpdateFlag(0);
			#endif
		}
#endif
#endif	

	return IRQ_HANDLED;	
}

static irqreturn_t tcc_lcd_handler1_for_video(int irq, void *dev_id)
{
	tca_video_vsync_interrupt_maskset(1);
	tca_video_vsync_interrupt_onoff(1, 1);
	
#ifdef USE_SOFT_IRQ_FOR_VSYNC
		if (schedule_work(&vsync_work_q) == 0 ) {
            printk("vsync error:cannot schedule work !!!\n");
        }
#else
	
#ifndef USE_VSYNC_TIMER
	   if((++tccvid_vsync.unVsyncCnt) &0x01)
	           tccvid_vsync.baseTime += 16;
	   else
	           tccvid_vsync.baseTime += 17;

	   if(!(tccvid_vsync.unVsyncCnt%6))
	           tccvid_vsync.baseTime++;
#endif

	#ifndef VIQE_DUPLICATE_ROUTINE
		if( tcc_vsync_is_empty_buffer(&tccvid_vsync.vsync_buffer) == 0)
		{
			#ifdef TCC_VIDEO_DISPLAY_DEINTERLACE_MODE
				if(tccvid_vsync.deinterlace_mode && !tccvid_vsync.output_toMemory)
					DisplayUpdateWithDeinterlace();
				else
			#endif
					DisplayUpdate();

			#if defined(CONFIG_TCC_OUTPUT_ATTACH)
				TCC_OUTPUT_FB_AttachUpdateFlag(1);
			#endif
		}
	#else
		#ifdef TCC_VIDEO_DISPLAY_DEINTERLACE_MODE
			if(tccvid_vsync.deinterlace_mode && !tccvid_vsync.output_toMemory){
				if(tccvid_vsync.interlace_bypass_lcdc){
					if( tcc_vsync_is_empty_buffer(&tccvid_vsync.vsync_buffer) == 0 )
						DisplayUpdateWithDeinterlace();
				}
				else{
					DisplayUpdateWithDeinterlace();
				}
				#if defined(CONFIG_TCC_OUTPUT_ATTACH)
					TCC_OUTPUT_FB_AttachUpdateFlag(0);
				#endif
			}
			else
		#endif
			if( tcc_vsync_is_empty_buffer(&tccvid_vsync.vsync_buffer) == 0 )
			{
				DisplayUpdate();
				#if defined(CONFIG_TCC_OUTPUT_ATTACH)
					TCC_OUTPUT_FB_AttachUpdateFlag(0);
				#endif
			}
	#endif
#endif	

	return IRQ_HANDLED;	
}

#ifdef USE_VSYNC_TIMER
static unsigned int tcc_vsync_timer_count = 0;
static int tcc_vsync_timer_last_get_time = 0;

static irqreturn_t tcc_vsync_timer_handler(int irq, void *dev_id)
{
	PTIMER pTIMER_reg = (volatile PTIMER)tcc_p2v(HwTMR_BASE);

	#if defined(CONFIG_ARCH_TCC892X)
		if(system_rev == 0x1005 || system_rev == 0x1006 || system_rev == 0x1007 ||system_rev == 0x1008 || system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
		{
				pTIMER_reg->TIREQ.nREG = 0x00000202;
		}
		else
		{
				pTIMER_reg->TIREQ.nREG = 0x00000101;
		}
	#endif					
	
	vprintk("tcc_vsync_timer_handler \n");
	//tcc_vsync_timer_count +=55924;

	return IRQ_HANDLED;	
}

static int timer_interrupt_onoff = 0;
void tccfb_vsync_timer_onoff(int onOff)
{
	PPIC pHwPIC_reg = (volatile PPIC)tcc_p2v(HwPIC_BASE);
	PTIMER pTIMER_reg = (volatile PTIMER)tcc_p2v(HwTMR_BASE);

	tcc_vsync_timer_count = 0;
	tcc_vsync_timer_last_get_time = 0;
	printk("tccfb_vsync_timer_onoff onOff %d\n",onOff);

	if(onOff == 1)	{
		PCKC pCKC = (volatile PCKC)tcc_p2v(HwCKC_BASE);
		BITCSET(pCKC->PCLKCTRL01.nREG, 0xFFFFFFFF, 0x24000000);

		#if defined(CONFIG_ARCH_TCC892X)
			if(system_rev == 0x1005 || system_rev == 0x1006 || system_rev == 0x1007 ||system_rev == 0x1008 || system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
			{
				pTIMER_reg->TCFG1.bREG.EN = 1;
				pTIMER_reg->TCFG1.bREG.IEN = 1;
				pTIMER_reg->TCFG1.bREG.TCKSEL= 5;
				pTIMER_reg->TREF1.nREG= 0x0000FFFF;
				pTIMER_reg->TIREQ.bREG.TI1 = 1;
				pTIMER_reg->TIREQ.bREG.TF1 = 1;
				pTIMER_reg->TCNT1.bREG.TCNT = 0;
			}

			else
			{
				pTIMER_reg->TCFG0.bREG.EN = 1;
				pTIMER_reg->TCFG0.bREG.IEN = 1;
				pTIMER_reg->TCFG0.bREG.TCKSEL= 5;
				pTIMER_reg->TREF0.nREG= 0x0000FFFF;
				pTIMER_reg->TIREQ.bREG.TI0 = 1;
				pTIMER_reg->TIREQ.bREG.TF0 = 1;
				pTIMER_reg->TCNT0.bREG.TCNT = 0;
			}
		#endif
		
		pHwPIC_reg->SEL0.bREG.TC0= 1;
		pHwPIC_reg->IEN0.bREG.TC0 = 1;
		pHwPIC_reg->INTMSK0.bREG.TC0 = 1;
		pHwPIC_reg->MODEA0.bREG.TC0 = 1;
		

		if(timer_interrupt_onoff == 0)
		{

		#if defined(CONFIG_ARCH_TCC892X)
			if(system_rev == 0x1005 || system_rev == 0x1006 || system_rev == 0x1007 ||system_rev == 0x1008 || system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
			{
				request_irq(INT_TC_TI1, tcc_vsync_timer_handler,	IRQF_SHARED,
						"TCC_TC1",	tcc_vsync_timer_handler);
			}
			else
			{
				request_irq(INT_TC_TI0, tcc_vsync_timer_handler,	IRQF_SHARED,
						"TCC_TC0",	tcc_vsync_timer_handler);
			}
		#endif
			timer_interrupt_onoff = 1;
		}
	
	}
	else	{

		#if defined(CONFIG_ARCH_TCC892X)
			if(system_rev == 0x1005 || system_rev == 0x1006 || system_rev == 0x1007 ||system_rev == 0x1008 || system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
			{
				pTIMER_reg->TCFG1.bREG.EN = 0;
				pTIMER_reg->TCFG1.bREG.IEN = 0;
				pTIMER_reg->TCFG1.bREG.TCKSEL= 0;
				pTIMER_reg->TREF1.nREG= 0x0000FFFF;
				pTIMER_reg->TIREQ.bREG.TI1 = 0;
				pTIMER_reg->TIREQ.bREG.TF1 = 0;
				pTIMER_reg->TCNT1.bREG.TCNT = 0;
			}
			else
			{
				pTIMER_reg->TCFG0.bREG.EN = 0;
				pTIMER_reg->TCFG0.bREG.IEN = 0;
				pTIMER_reg->TCFG0.bREG.TCKSEL= 0;
				pTIMER_reg->TREF0.nREG= 0x0000FFFF;
				pTIMER_reg->TIREQ.bREG.TI0 = 0;
				pTIMER_reg->TIREQ.bREG.TF0 = 0;
				pTIMER_reg->TCNT0.bREG.TCNT = 0;

			}
		#endif

		// interrupt disable
		pHwPIC_reg->CLR0.bREG.TC0 = 1;
		pHwPIC_reg->IEN0.bREG.TC0 = 0;
		pHwPIC_reg->INTMSK0.bREG.TC0 = 0;

		if(timer_interrupt_onoff == 1)
		{
			#if defined(CONFIG_ARCH_TCC892X)
				if(system_rev == 0x1005 || system_rev == 0x1006 || system_rev == 0x1007 ||system_rev == 0x1008 || system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
				{
						free_irq(INT_TC_TI1, tcc_vsync_timer_handler);			
				}
				else
				{
						free_irq(INT_TC_TI0, tcc_vsync_timer_handler);
				}
			#endif					
			timer_interrupt_onoff = 0;
		}

	}
}

unsigned int tcc_vsync_get_timer_clock(void)
{
	int timer_tick;
	int msec_time;
	int add_count;
	
	PTIMER pTIMER_reg = (volatile PTIMER)tcc_p2v(HwTMR_BASE);


	#if defined(CONFIG_ARCH_TCC892X)
		if(system_rev == 0x1005 || system_rev == 0x1006 || system_rev == 0x1007 ||system_rev == 0x1008 || system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
		{
				timer_tick = pTIMER_reg->TCNT1.bREG.TCNT;
		}
		else
		{
				timer_tick = pTIMER_reg->TCNT0.bREG.TCNT;
		}
	#endif					

	msec_time = timer_tick*85/1000 + timer_tick/3000;
	

	msec_time += (tcc_vsync_timer_count/10);
	
	//printk(" timer_tick(%d) msec_time(%d), tcc_vsync_timer_last_get_time(%d) %d\n",timer_tick,msec_time,tcc_vsync_timer_last_get_time,tcc_vsync_timer_count);
	if(msec_time < tcc_vsync_timer_last_get_time)
	{
		//printk("msec_time(%d), tcc_vsync_timer_last_get_time(%d)\n", msec_time, tcc_vsync_timer_last_get_time);
		tcc_vsync_timer_count +=55924;
		msec_time += 5592;
	}

	//if(tcc_vsync_timer_last_get_time >= msec_time)
	//	printk(" same time msec_time(%d), tcc_vsync_timer_last_get_time(%d)\n",tcc_vsync_timer_last_get_time,msec_time);
	
	tcc_vsync_timer_last_get_time = msec_time;
	return msec_time;
}
#endif

inline static void tcc_vsync_set_time(int time)
{
#ifdef USE_VSYNC_TIMER
	tccvid_vsync.baseTime = tcc_vsync_get_timer_clock() - time;
	//printk("set base time %d kernel time %d time %d \n",tccvid_vsync.baseTime,tcc_vsync_get_timer_clock(),time);
#else
	tccvid_vsync.baseTime = time;
#endif
}

inline static int tcc_vsync_get_time(void)
{
#ifdef USE_VSYNC_TIMER
	return tcc_vsync_get_timer_clock() - tccvid_vsync.baseTime;
#else
	return tccvid_vsync.baseTime;
#endif

}


static int lcdc_interrupt_onoff = 0;
void tca_vsync_video_display_enable(void)
{
	int ret;
	char hdmi_lcdc = EX_OUT_LCDC;



	
	if(lcdc_interrupt_onoff == 0)
	{
		if(hdmi_lcdc){
			tca_video_vsync_interrupt_onoff(1, 1);

			ret = request_irq(INT_VIOC_RD4, tcc_lcd_handler1_for_video,	IRQF_SHARED,
					"INT_VIOC_RD4",	tcc_lcd_handler1_for_video);
			printk("tca_vsync_video_display_enable : request_irq 1 ret %d\n",ret);
		}
		else{
			tca_video_vsync_interrupt_onoff(1, 0);
				
			ret= request_irq(INT_VIOC_RD0, tcc_lcd_handler0_for_video,	IRQF_SHARED,
					"INT_VIOC_RD0",	tcc_lcd_handler0_for_video);
			
			printk("tca_vsync_video_display_enable : request_irq 0 ret %d\n",ret);
		}

#ifdef USE_SOFT_IRQ_FOR_VSYNC
	    INIT_WORK(&vsync_work_q, tcc_video_display_update_isr);
#endif
		lcdc_interrupt_onoff = 1;
	}
	else
	{
		printk("tccfb error: lcdc interrupt have been enabled already\n");
	}
}
void tca_vsync_video_display_disable(void)
{
	char hdmi_lcdc = EX_OUT_LCDC;
	printk("tca_vsync_video_display_disable \n");
	if(lcdc_interrupt_onoff == 1)
	{
		if(hdmi_lcdc){
			tca_video_vsync_interrupt_onoff(0, 1);
			free_irq(INT_VIOC_RD4, tcc_lcd_handler1_for_video);
		}
		else {
			tca_video_vsync_interrupt_onoff(0, 0);
			free_irq(INT_VIOC_RD0, tcc_lcd_handler0_for_video);
		}
		lcdc_interrupt_onoff = 0;
	}
	else
	{
		printk("tccfb error: lcdc interrupt have been disabled already\n");
	}
}

void tcc_vsync_set_firstFrameFlag(int firstFrameFlag)
{
	tccvid_vsync.firstFrameFlag = firstFrameFlag;
}

int tcc_vsync_get_isVsyncRunning(void)
{
	return tccvid_vsync.isVsyncRunning;
}

static int tccfb_calculateSyncTime(int currentTime)
{
	static int lastUdateTime = 0;
	int diffTime;
	int avgTime;
	int base_time;


	if(lastUdateTime == currentTime)
		return 0;

	lastUdateTime= currentTime;

	spin_lock_irq(&vsync_lock) ;
	base_time = tcc_vsync_get_time();
	spin_unlock_irq(&vsync_lock) ;
	
	diffTime = 	currentTime - base_time;

	tccvid_vsync.timeGapTotal -= tccvid_vsync.timeGap[tccvid_vsync.timeGapIdx];
	tccvid_vsync.timeGapTotal += diffTime;
	tccvid_vsync.timeGap[tccvid_vsync.timeGapIdx++] = diffTime;

	if(tccvid_vsync.timeGapIdx >= TIME_BUFFER_COUNT)
		tccvid_vsync.timeGapIdx = 0;

	if(tccvid_vsync.timeGapIdx == 0)
		tccvid_vsync.timeGapBufferFullFlag = 1;

	if(tccvid_vsync.timeGapBufferFullFlag)
		avgTime = tccvid_vsync.timeGapTotal / TIME_BUFFER_COUNT;
	else
		avgTime = tccvid_vsync.timeGapTotal / (int)(tccvid_vsync.timeGapIdx);


	vprintk("diffTime %d, avgTime %d, base : %d\n", diffTime, avgTime, base_time);

	if( (tccvid_vsync.timeGapBufferFullFlag || tccvid_vsync.unVsyncCnt < 100) 
		&& (avgTime > tccvid_vsync.updateGapTime || avgTime < -(tccvid_vsync.updateGapTime))	)
	{
		memset(tccvid_vsync.timeGap, 0x00, sizeof(tccvid_vsync.timeGap));
		tccvid_vsync.timeGapBufferFullFlag = 0;
		tccvid_vsync.timeGapIdx = 0;
		tccvid_vsync.timeGapTotal = 0;
		
		//printk("changed time base time %d kernel time %d time %d \n",tccvid_vsync.baseTime,tcc_vsync_get_timer_clock(),currentTime);
		spin_lock_irq(&vsync_lock) ;
		tcc_vsync_set_time(base_time+avgTime);
		spin_unlock_irq(&vsync_lock) ;

		printk("changed base time : %d, add time: %d diffTime %d \n",base_time+avgTime, avgTime,diffTime);
	}
	
	return 0;
}

static void tccfb_ResetSyncTime(int currentTime)
{
	memset(tccvid_vsync.timeGap, 0x00, sizeof(tccvid_vsync.timeGap));
	tccvid_vsync.timeGapBufferFullFlag = 0;
	tccvid_vsync.timeGapIdx = 0;
	tccvid_vsync.timeGapTotal = 0;
	
	spin_lock_irq(&vsync_lock) ;
	tcc_vsync_set_time(currentTime);
	tccvid_vsync.unVsyncCnt=0;
	spin_unlock_irq(&vsync_lock) ;

	printk("reset base time : %d\n",currentTime);
}
#endif

void tccfb_output_starter(char output_type, char lcdc_num, struct lcdc_timimg_parms_t *lcdc_timing)
{
	switch(output_type)
	{
		case TCC_OUTPUT_HDMI:
 		 	TCC_OUTPUT_LCDC_OnOff(TCC_OUTPUT_HDMI, lcdc_num, 1);
 			TCC_HDMI_LCDC_Timing(lcdc_num, lcdc_timing);
			TCC_OUTPUT_UPDATE_OnOff(1, TCC_OUTPUT_HDMI);
			Output_SelectMode = TCC_OUTPUT_HDMI;
			break;

		case TCC_OUTPUT_COMPOSITE:
 		 	TCC_OUTPUT_LCDC_OnOff(TCC_OUTPUT_COMPOSITE, lcdc_num, 1);
			Output_SelectMode = TCC_OUTPUT_COMPOSITE;
			break;

		case TCC_OUTPUT_COMPONENT:
 		 	TCC_OUTPUT_LCDC_OnOff(TCC_OUTPUT_COMPONENT, lcdc_num, 1);
			Output_SelectMode = TCC_OUTPUT_COMPONENT;
			break;
	}

	#if defined(TCC_VIDEO_DISPLAY_BY_VSYNC_INT)
		memset( &tccvid_vsync, 0, sizeof( tccvid_vsync ) );
		tccvid_vsync.overlayUsedFlag = -1;
		tccvid_vsync.outputMode = -1;
		tccvid_vsync.firstFrameFlag = 1;
		tccvid_vsync.deinterlace_mode= -1;
		tccvid_vsync.m2m_mode = -1;
		tccvid_vsync.output_toMemory = -1;
	#endif
}

static int tccfb_check_var(struct fb_var_screeninfo *var,
			       struct fb_info *info)
{
	/* validate bpp */
	if (var->bits_per_pixel > 32)
		var->bits_per_pixel = 32;
	else if (var->bits_per_pixel < 16)
		var->bits_per_pixel = 16;

	/* set r/g/b positions */
	if (var->bits_per_pixel == 16) {
		var->red.offset 	= 11;
		var->green.offset	= 5;
		var->blue.offset	= 0;
		var->red.length 	= 5;
		var->green.length	= 6;
		var->blue.length	= 5;
		var->transp.length	= 0;
	} else if (var->bits_per_pixel == 32) {
		var->red.offset 	= 16;
		var->green.offset	= 8;
		var->blue.offset	= 0;
		var->transp.offset	= 24;
		var->red.length 	= 8;
		var->green.length	= 8;
		var->blue.length	= 8;
		var->transp.length	= 8;
	} else {
		var->red.length 	= var->bits_per_pixel;
		var->red.offset 	= 0;
		var->green.length	= var->bits_per_pixel;
		var->green.offset	= 0;
		var->blue.length	= var->bits_per_pixel;
		var->blue.offset	= 0;
		var->transp.length	= 0;
	}
	if (var->yres_virtual < var->yoffset + var->yres)
		var->yres_virtual = var->yoffset + var->yres;

	return 0;
}




/* tccfb_pan_display
 *
 * pandisplay (set) the controller from the given framebuffer
 * information
*/
static int tccfb_pan_display(struct fb_var_screeninfo *var, struct fb_info *info)
{
	char output_type; 
	char tcc_output_ret = 0;
	unsigned int base_addr = 0;
	struct tccfb_info *fbi =(struct tccfb_info *) info->par;

	if(!fb_power_state)
		return 0;

 	base_addr = Output_BaseAddr = fbi->map_dma +fbi->fb->var.xres * (fbi->fb->var.bits_per_pixel/8) * var->yoffset;
	if(var->yoffset > var->yres)	{
		base_addr = Output_BaseAddr = PAGE_ALIGN(base_addr);
	}
	sprintk("%s addr:0x%x Yoffset:%d \n", __func__, Output_BaseAddr, var->yoffset);

	#if defined(CONFIG_TCC_OUTPUT_DUAL_UI)
		for(output_type = TCC_OUTPUT_COMPOSITE; output_type<TCC_OUTPUT_MAX; output_type++)
		{
			tcc_output_ret = TCC_OUTPUT_FB_Update(fbi->fb->var.xres, fbi->fb->var.yres, fbi->fb->var.bits_per_pixel, base_addr, output_type);
			if(tcc_output_ret)
				TCC_OUTPUT_FB_UpdateSync(output_type);
		}
	#else
		if(Output_SelectMode)
		{
			tcc_output_ret = TCC_OUTPUT_FB_Update(fbi->fb->var.xres, fbi->fb->var.yres, fbi->fb->var.bits_per_pixel, base_addr, Output_SelectMode);

			if(tcc_output_ret )
				TCC_OUTPUT_FB_UpdateSync(Output_SelectMode);
		}
	#endif

	tca_fb_pan_display(var, info);
	return 0;
}
/* tccfb_activate_var
 *
 * activate (set) the controller from the given framebuffer
 * information
*/
static void tccfb_activate_var(struct tccfb_info *fbi,  struct fb_var_screeninfo *var)
{
	unsigned int imgch = 0;

	sprintk("%s node:0x%x TCC_DEVS:%d \n", __func__, fbi->fb->node, CONFIG_FB_TCC_DEVS);

	if((0 <= fbi->fb->node) && (fbi->fb->node < CONFIG_FB_TCC_DEVS))
		imgch = fbi->fb->node;
	else
		return;

	tca_fb_activate_var(fbi, var);
}


/*
 *      tccfb_set_par - Optional function. Alters the hardware state.
 *      @info: frame buffer structure that represents a single frame buffer
 *
 */
static int tccfb_set_par(struct fb_info *info)
{
	struct tccfb_info *fbi = info->par;
	struct fb_var_screeninfo *var = &info->var;

	sprintk("- tccfb_set_par pwr:%d  output:%d \n",fb_power_state, Output_SelectMode);

	if (var->bits_per_pixel == 16)
		fbi->fb->fix.visual = FB_VISUAL_TRUECOLOR;
	else if (var->bits_per_pixel == 32)
		fbi->fb->fix.visual = FB_VISUAL_TRUECOLOR;
	else
		fbi->fb->fix.visual = FB_VISUAL_PSEUDOCOLOR;

	fbi->fb->fix.line_length = (var->xres*var->bits_per_pixel)/8;

	#ifndef CONFIG_TCC_OUTPUT_STARTER
	/* activate this new configuration */
   	if(fb_power_state && Output_SelectMode!=TCC_OUTPUT_COMPONENT) //&& Output_SelectMode!=TCC_OUTPUT_COMPOSITE
		tccfb_activate_var(fbi, var);
	#endif

	return 0;
}

static int tccfb_ioctl(struct fb_info *info, unsigned int cmd,unsigned long arg)
{
	struct tccfb_info *fb_info = info->par;
	unsigned int imgch=0;
	int screen_width, screen_height;

	screen_width = lcd_panel->xres;
	screen_height = lcd_panel->yres;

	if((0 <= info->node) && (info->node < CONFIG_FB_TCC_DEVS))	{
		imgch = info->node;
	}
	else	{
		dprintk("ioctl: Error - fix.id[%d]\n", info->node);
		return 0;
	}


	switch(cmd)
	{
		case TCC_LCDC_GET_NUM:
			{
				unsigned int LCD_lcdc_number;
				LCD_lcdc_number = tca_get_lcd_lcdc_num();
				dprintk("%s: TCC_LCDC_GET_NUM :: %d    \n", __func__ , LCD_lcdc_number);
				
				if (copy_to_user((unsigned int *)arg, &LCD_lcdc_number, sizeof(unsigned int))) {
					return -EFAULT;
				}
			}
			break;
			
		case TCC_LCDC_HDMI_START:
			TCC_OUTPUT_FB_DetachOutput(1);
			TCC_OUTPUT_LCDC_OnOff(TCC_OUTPUT_HDMI, EX_OUT_LCDC, 1);
			break;

		case TCC_LCDC_HDMI_TIMING:
			{
				struct lcdc_timimg_parms_t lcdc_timing;
				dprintk(" TCC_LCDC_HDMI_TIMING: \n");

				if (copy_from_user((void*)&lcdc_timing, (const void*)arg, sizeof(struct lcdc_timimg_parms_t)))
					return -EFAULT;

				VIOC_DISP_SWReset(EX_OUT_LCDC);

				TCC_HDMI_LCDC_Timing(EX_OUT_LCDC, &lcdc_timing);
				TCC_OUTPUT_UPDATE_OnOff(1, TCC_OUTPUT_HDMI);
				Output_SelectMode = TCC_OUTPUT_HDMI;
		
				TCC_OUTPUT_FB_Update(fb_info->fb->var.xres, fb_info->fb->var.yres, fb_info->fb->var.bits_per_pixel, Output_BaseAddr, Output_SelectMode);
				TCC_OUTPUT_FB_UpdateSync(Output_SelectMode);
				TCC_HDMI_LCDC_OutputEnable(EX_OUT_LCDC, 1);

				TCC_OUTPUT_FB_MouseShow(0, TCC_OUTPUT_HDMI);


				#ifdef TCC_VIDEO_DISPLAY_BY_VSYNC_INT

				if(tccvid_vsync.isVsyncRunning)
					tca_vsync_video_display_enable();

				spin_lock_irq(&vsync_lock);
				tccvid_vsync.outputMode = -1;
				spin_unlock_irq(&vsync_lock);
				#endif
			}
			break;

		case TCC_LCDC_HDMI_DISPLAY:
			{
				struct tcc_lcdc_image_update ImageInfo;
				if (copy_from_user((void *)&ImageInfo, (const void *)arg, sizeof(struct tcc_lcdc_image_update))){
					return -EFAULT;
				}

				dprintk("%s : TCC_LCDC_HDMI_DISPLAY\n", __func__);

				if(Output_SelectMode == TCC_OUTPUT_HDMI)	{
					#ifdef TCC_VIDEO_DISPLAY_BY_VSYNC_INT
 					// this code prevent a error of transparent on layer0 after stopping v
					if(ImageInfo.Lcdc_layer == 0 && ImageInfo.enable == 0)
					{
					       spin_lock_irq(&vsync_lock);
					       tccvid_vsync.outputMode = -1;
					       spin_unlock_irq(&vsync_lock);
					}
					 #endif
  					TCC_HDMI_DISPLAY_UPDATE(EX_OUT_LCDC, (struct tcc_lcdc_image_update *)&ImageInfo);
				}
			}
			break;

		case TCC_LCDC_HDMI_END:
			#if !defined(CONFIG_TCC_OUTPUT_DUAL_UI)
			if(Output_SelectMode == TCC_OUTPUT_HDMI) 
			#endif
			{
				struct tcc_lcdc_image_update lcdc_image;
				#if defined(CONFIG_ARCH_TCC93XX)
					TCC_OUTPUT_FB_BackupVideoImg(Output_SelectMode);
				#endif

				Output_SelectMode = TCC_OUTPUT_NONE;
				
				lcdc_image.enable = 0;
				lcdc_image.Lcdc_layer = 2;
				lcdc_image.fmt = TCC_LCDC_IMG_FMT_RGB565;

				TCC_HDMI_DISPLAY_UPDATE(EX_OUT_LCDC, &lcdc_image);

				TCC_OUTPUT_LCDC_OnOff(TCC_OUTPUT_HDMI, EX_OUT_LCDC, 0);

			}

			TCC_OUTPUT_FB_MouseShow(0, TCC_OUTPUT_HDMI);
			
			#ifdef TCC_VIDEO_DISPLAY_BY_VSYNC_INT
			if(tccvid_vsync.isVsyncRunning)
				tca_vsync_video_display_disable();
			tcc_vsync_set_firstFrameFlag(1);
			#endif
			#ifdef TCC_VIDEO_DISPLAY_DEINTERLACE_MODE
				if(tccvid_vsync.deinterlace_mode && !tccvid_vsync.output_toMemory &&!tccvid_vsync.interlace_bypass_lcdc)
					TCC_VIQE_DI_DeInit60Hz();
			#endif
			break;

		case TCC_LCDC_HDMI_CHECK:
			{
				unsigned int ret_mode = 0;
				if((!fb_power_state) || HDMI_pause || ((screen_width < screen_height)&& (!HDMI_video_mode)))
				{
					ret_mode = 1;
					dprintk("\n %d %d : %d %d  \n ", fb_power_state, HDMI_pause, screen_width, screen_height);
				}

				put_user(ret_mode, (unsigned int __user*)arg);
			}
			break;

		case TCC_LCDC_HDMI_MODE_SET:
 			{
				TCC_HDMI_M uiHdmi;

				if(get_user(uiHdmi, (int __user *) arg))
					return -EFAULT;

				dprintk("%s: TCC_LCDC_HDMI_MODE_SET [%d] video_M:%d Output_SelectMode:%d   \n", __func__ , uiHdmi , HDMI_video_mode, Output_SelectMode);

				switch(uiHdmi)
				{
					case TCC_HDMI_SUSEPNED:
						HDMI_pause = 1;
						break;
					case TCC_HDMI_RESUME:
						HDMI_pause = 0;
						break;
					case TCC_HDMI_VIDEO_START:
						HDMI_video_mode = 1;
						break;
					case TCC_HDMI_VIDEO_END:
						HDMI_video_mode = 0;
						break;
					default:
						break;
				}
 			}
			break;
			
		case TCC_LCDC_HDMI_GET_SIZE:
			{
				tcc_display_size HdmiSize;
				HdmiSize.width = HDMI_video_width;
				HdmiSize.height = HDMI_video_height;
  				HdmiSize.frame_hz= HDMI_video_hz;

				dprintk("%s: TCC_LCDC_HDMI_GET_SIZE -  HDMI_video_width:%d HDMI_video_height:%d   \n", __func__ , HDMI_video_width, HDMI_video_height);
				if (copy_to_user((tcc_display_size *)arg, &HdmiSize, sizeof(HdmiSize)))		{
					return -EFAULT;
				}
			}
			break;

		case TCC_LCDC_SET_OUTPUT_RESIZE_MODE:
			{
				struct tccfb_info *fbi =(struct tccfb_info *) info->par;
				tcc_display_resize mode;

				if(copy_from_user((void *)&mode, (const void *)arg, sizeof(tcc_display_resize)))
					return -EFAULT;

				//printk("%s : TCC_LCDC_SET_OUTPUT_RESIZE_MODE, mode=%d\n", __func__, mode);

				TCC_OUTPUT_SetOutputResizeMode(mode);

				#ifndef CONFIG_TCC_OUTPUT_AUTO_DETECTION
					//TCC_OUTPUT_FB_Update(fbi->fb->var.xres, fbi->fb->var.yres, fbi->fb->var.bits_per_pixel, Output_BaseAddr, Output_SelectMode);
					//TCC_OUTPUT_FB_UpdateSync(Output_SelectMode);
				#endif
			}
			break;

		case TCC_LCDC_HDMI_SET_SIZE:
			{
				tcc_display_size HdmiSize;
				if (copy_from_user((void *)&HdmiSize, (const void *)arg, sizeof(tcc_display_size)))
					return -EFAULT;

				HDMI_video_width = HdmiSize.width;
				HDMI_video_height = HdmiSize.height;
				HDMI_video_hz = HdmiSize.frame_hz;

				dprintk("%s: TCC_LCDC_HDMI_SET_SIZE -  HDMI_video_width:%d HDMI_video_height:%d   \n", __func__ , HDMI_video_width, HDMI_video_height);
			}
			break;

		case TCC_LCDC_COMPOSITE_CHECK:
			{
				unsigned int composite_detect = 1;

				#if defined(CONFIG_FB_TCC_COMPOSITE)
					composite_detect = tcc_composite_detect();
				#endif

				if (copy_to_user((void *)arg, &composite_detect, sizeof(unsigned int)))
					return -EFAULT;
			}
			break;
			
		case TCC_LCDC_COMPOSITE_MODE_SET:
			{
				LCDC_COMPOSITE_MODE composite_mode;
				if(copy_from_user((void *)&composite_mode, (const void *)arg, sizeof(LCDC_COMPOSITE_MODE))){
					return -EFAULT;
				}

				if(composite_mode == LCDC_COMPOSITE_UI_MODE)
				{
					Output_SelectMode = TCC_OUTPUT_COMPOSITE;
					#ifdef TCC_VIDEO_DISPLAY_BY_VSYNC_INT
					{
					       spin_lock_irq(&vsync_lock);
					       tccvid_vsync.outputMode = -1;
					       spin_unlock_irq(&vsync_lock);
					}
					#endif
					
 					printk("TCC_LCDC_COMPOSITE_MODE_SET : Output_SelectMode = %d , composite_mode = %d\n", composite_mode, Output_SelectMode);

					TCC_OUTPUT_FB_Update(fb_info->fb->var.xres, fb_info->fb->var.yres, fb_info->fb->var.bits_per_pixel, Output_BaseAddr, Output_SelectMode);
					TCC_OUTPUT_FB_UpdateSync(Output_SelectMode);
					TCC_OUTPUT_LCDC_OutputEnable(EX_OUT_LCDC, 1);

					TCC_OUTPUT_FB_MouseShow(1, TCC_OUTPUT_COMPOSITE);
				}
				else if(composite_mode == LCDC_COMPOSITE_NONE_MODE)
				{
					Output_SelectMode = TCC_OUTPUT_NONE;
				}
			}
			break;

		case TCC_LCDC_COMPONENT_CHECK:
			{
				unsigned int component_detect;

				#if defined(CONFIG_FB_TCC_COMPONENT)
					component_detect = tcc_component_detect();
				#endif

				if (copy_to_user((void *)arg, &component_detect, sizeof(unsigned int)))
					return -EFAULT;
			}
			break;
			
		case TCC_LCDC_COMPONENT_MODE_SET:
			{
				LCDC_COMPONENT_MODE component_mode;
				if(copy_from_user((void *)&component_mode, (const void *)arg, sizeof(LCDC_COMPONENT_MODE))){
					return -EFAULT;
				}

				if(component_mode == LCDC_COMPONENT_UI_MODE)
				{
					Output_SelectMode = TCC_OUTPUT_COMPONENT;

					#ifdef TCC_VIDEO_DISPLAY_BY_VSYNC_INT
				       spin_lock_irq(&vsync_lock);
				       tccvid_vsync.outputMode = -1;
				       spin_unlock_irq(&vsync_lock);
					#endif

					TCC_OUTPUT_FB_Update(fb_info->fb->var.xres, fb_info->fb->var.yres, fb_info->fb->var.bits_per_pixel, Output_BaseAddr, Output_SelectMode);
					TCC_OUTPUT_FB_UpdateSync(Output_SelectMode);
					TCC_OUTPUT_LCDC_OutputEnable(EX_OUT_LCDC, 1);

					TCC_OUTPUT_FB_MouseShow(1, TCC_OUTPUT_COMPONENT);
				}
				else if(component_mode == LCDC_COMPONENT_NONE_MODE)
				{
					Output_SelectMode = TCC_OUTPUT_NONE;
				}
			}
			break;

		case TCC_LCDC_MOUSE_SHOW:
			{
				unsigned int enable;

				if(copy_from_user((void *)&enable, (const void *)arg, sizeof(unsigned int)))
					return -EFAULT;
				TCC_OUTPUT_FB_MouseShow(enable, Output_SelectMode);
			}
			break;
		case TCC_LCDC_MOUSE_MOVE:
			{
				tcc_mouse mouse;
				if (copy_from_user((void *)&mouse, (const void *)arg, sizeof(tcc_mouse)))
					return -EFAULT;
				TCC_OUTPUT_FB_MouseMove(fb_info->fb->var.xres, fb_info->fb->var.yres, &mouse, Output_SelectMode);
			}
			break;
		case TCC_LCDC_MOUSE_ICON:
			{
				tcc_mouse_icon mouse_icon;
				if (copy_from_user((void *)&mouse_icon, (const void *)arg, sizeof(tcc_mouse_icon)))
					return -EFAULT;
				TCC_OUTPUT_FB_MouseSetIcon(&mouse_icon);
			}
			break;

		case TCC_LCDC_3D_UI_ENABLE:
			{
				unsigned int mode;

				if(copy_from_user((void *)&mode, (const void *)arg, sizeof(unsigned int)))
					return -EFAULT;

				TCC_OUTPUT_FB_Set3DMode(TRUE, mode);
				TCC_OUTPUT_FB_Update(fb_info->fb->var.xres, fb_info->fb->var.yres, fb_info->fb->var.bits_per_pixel, Output_BaseAddr, Output_SelectMode);
				TCC_OUTPUT_FB_UpdateSync(Output_SelectMode);
			}
			break;
			
		case TCC_LCDC_3D_UI_DISABLE:
			{
				TCC_OUTPUT_FB_Set3DMode(FALSE, 0);
				TCC_OUTPUT_FB_Update(fb_info->fb->var.xres, fb_info->fb->var.yres, fb_info->fb->var.bits_per_pixel, Output_BaseAddr, Output_SelectMode);
				TCC_OUTPUT_FB_UpdateSync(Output_SelectMode);
			}
			break;
			
		case TCC_LCDC_ATTACH_SET_STATE:
			{
				unsigned int attach_state;

				if(copy_from_user((void *)&attach_state, (const void *)arg, sizeof(unsigned int)))
					return -EFAULT;

				#if defined(CONFIG_TCC_OUTPUT_ATTACH)
					TCC_OUTPUT_FB_AttachSetSate(attach_state);
				#endif
			}
			break;
			
		case TCC_LCDC_ATTACH_GET_STATE:
			{
				unsigned int attach_state = 0;

				#if defined(CONFIG_TCC_OUTPUT_ATTACH)
					attach_state = TCC_OUTPUT_FB_AttachGetSate();
				#endif

				if (copy_to_user((void *)arg, &attach_state, sizeof(unsigned int)))
					return -EFAULT;
			}
			break;
			
	#ifdef TCC_VIDEO_DISPLAY_BY_VSYNC_INT
	case TCC_LCDC_VIDEO_START_VSYNC:
		{
			int backup_time;
			int backup_frame_rate;
			printk("\n### TCC_LCDC_VIDEO_START_VSYNC \n");

			if(!fb_power_state)
			{
				printk("##### Error ### vsync start\n");				
				return -1;
			}
			if(vsync_started == 0)
			{
				backup_time = tccvid_vsync.nTimeGapToNextField; 
				backup_frame_rate = tccvid_vsync.video_frame_rate;
				memset( &tccvid_vsync, 0, sizeof( tccvid_vsync ) ) ; 
				tccvid_vsync.isVsyncRunning = 1;
				tccvid_vsync.overlayUsedFlag = -1;
				tccvid_vsync.outputMode = -1;
				tccvid_vsync.firstFrameFlag = 1;
				tccvid_vsync.deinterlace_mode= -1;
				tccvid_vsync.m2m_mode = -1;
				tccvid_vsync.output_toMemory = -1;
				tccvid_vsync.nTimeGapToNextField = backup_time;
				tccvid_vsync.video_frame_rate = backup_frame_rate;
				viqe_render_init();
				if(backup_time)
					tccvid_vsync.updateGapTime = backup_time;
				else
					tccvid_vsync.updateGapTime = 16;

				if(Output_SelectMode == TCC_OUTPUT_HDMI && HDMI_video_hz != 0)
				{
					tccvid_vsync.vsync_interval = (1000/HDMI_video_hz);
				
					if( (tccvid_vsync.video_frame_rate > 0) 
						&& (HDMI_video_hz >= tccvid_vsync.video_frame_rate) 
						&& ((HDMI_video_hz % tccvid_vsync.video_frame_rate) == 0)
					)
						tccvid_vsync.perfect_vsync_flag = 1;
					else
						tccvid_vsync.perfect_vsync_flag = 0;						
				}
				else
				{
					tccvid_vsync.vsync_interval = (1000/60);
				}
				printk("vsync_interval (%d), perfect_flag(%d)\n", tccvid_vsync.vsync_interval, tccvid_vsync.perfect_vsync_flag);
				spin_lock_init(&vsync_lock) ;
				spin_lock_init(&vsync_lockDisp ) ;

#ifdef USE_VSYNC_TIMER
				tccfb_vsync_timer_onoff(1);
				msleep(0);
#endif

				spin_lock_irq(&vsync_lock) ;
				tcc_vsync_set_time(0);
				spin_unlock_irq(&vsync_lock) ;

				tca_vsync_video_display_enable();

				tcc_vsync_set_max_buffer(&tccvid_vsync.vsync_buffer, arg);
				vsync_started = 1;
			}
		}
		break ;

	case TCC_LCDC_VIDEO_END_VSYNC:
		{
			printk("\nTCC_LCDC_VIDEO_END_VSYNC fb_power_state:%d \n", fb_power_state);
			if(vsync_started == 1)
			{
#ifdef USE_VSYNC_TIMER
				tccfb_vsync_timer_onoff(0);
#endif

				tca_vsync_video_display_disable();			
#ifdef TCC_VIDEO_DISPLAY_DEINTERLACE_MODE
				if(tccvid_vsync.deinterlace_mode && !tccvid_vsync.output_toMemory &&!tccvid_vsync.interlace_bypass_lcdc)
					TCC_VIQE_DI_DeInit60Hz();
#endif
				tccvid_vsync.skipFrameStatus = 1;
				tccvid_vsync.nTimeGapToNextField = 0;
				tccvid_vsync.isVsyncRunning = 0;

				{
					struct tcc_lcdc_image_update ImageInfo;
					memset(&ImageInfo, 0x00, sizeof(struct tcc_lcdc_image_update));
					ImageInfo.Lcdc_layer = 2;
					ImageInfo.enable = 0;

					if(Output_SelectMode == TCC_OUTPUT_HDMI){
						TCC_HDMI_DISPLAY_UPDATE(EX_OUT_LCDC, (struct tcc_lcdc_image_update *)&ImageInfo);
					}
					else if(Output_SelectMode == TCC_OUTPUT_COMPOSITE){
		#if defined(CONFIG_FB_TCC_COMPOSITE)
						tcc_composite_update((struct tcc_lcdc_image_update *)&ImageInfo);
		#endif
					}
					else if(Output_SelectMode == TCC_OUTPUT_COMPONENT){
		#if defined(CONFIG_FB_TCC_COMPONENT)
						tcc_component_update((struct tcc_lcdc_image_update *)&ImageInfo);
		#endif
					}
				}
				/*
				if(Output_SelectMode == TCC_OUTPUT_NONE)
				{
					struct tcc_lcdc_image_update ImageInfo;
					memset(&ImageInfo, 0x00, sizeof(struct tcc_lcdc_image_update));
					TCC_HDMI_DISPLAY_UPDATE(EX_OUT_LCDC, &ImageInfo);
				}
				*/
				vsync_started = 0;
			}
		}
		break ;

	case TCC_LCDC_VIDEO_SET_SIZE_CHANGE:
		{
			printk("### Display size is changed, firstFrame(%d) deint_mode(%d) \n", tccvid_vsync.firstFrameFlag, tccvid_vsync.deinterlace_mode);

			//if(tccvid_vsync.firstFrameFlag == 0)
			{
				spin_lock_irq(&vsync_lock) ;
				// have to add code about pop all buffer.
				tcc_vsync_pop_all_buffer(&tccvid_vsync.vsync_buffer);
				printk("### vsync pop all buffer success!\n");
				
				spin_unlock_irq(&vsync_lock) ;
			}
		}
		break ;

	case TCC_LCDC_VIDEO_SET_FRAMERATE:
		{
			int media_time_gap;
			printk("### TCC_LCDC_VIDEO_SET_FRAMERATE %d\n", (int)arg);

			if(arg < 1000)
			{
				if(arg == 29)
				{
					arg = 30;
				}
				else if(arg == 23)
				{
					arg = 24;
				}
			}
			else
			{
				arg += 500;
				arg /= 1000;
			}

			tccvid_vsync.video_frame_rate = arg;

			if(tccvid_vsync.video_frame_rate > 0)
				media_time_gap = 1000/tccvid_vsync.video_frame_rate/2;
			else
				media_time_gap = 16;
			
			tccvid_vsync.nTimeGapToNextField= media_time_gap;

			printk("### video_frame_rate(%d), media_time_gap(%d)\n",tccvid_vsync.video_frame_rate, media_time_gap);
		}
		break ;

	case TCC_LCDC_VIDEO_PUSH_VSYNC:
		if(Output_SelectMode == TCC_OUTPUT_NONE || Output_SelectMode == TCC_OUTPUT_HDMI || Output_SelectMode == TCC_OUTPUT_COMPONENT || Output_SelectMode == TCC_OUTPUT_COMPOSITE)
		{
			struct tcc_lcdc_image_update input_image;
			int error_type = 0;
			int check_time;

			if (copy_from_user((void *)&input_image , (const void *)arg, sizeof(struct tcc_lcdc_image_update)))
			{
				printk("fatal error") ;
				return -EFAULT;
			}

			//printk("## Frame %d x %d , Image %d x %d, start x:%d y:%d, Addr %x\n", input_image.Frame_width, input_image.Frame_height, input_image.Image_width, input_image.Image_height, input_image.offset_x, input_image.offset_y, input_image.addr0);

			//printk("### push valid(%d), max(%d), vtime(%d), ctime(%d), buff_id(%d)\n",
			//				tccvid_vsync.vsync_buffer.valid_buff_count,
			//				tccvid_vsync.vsync_buffer.max_buff_num,
			//				input_image.time_stamp, input_image.sync_time, input_image.buffer_unique_id);
			if(!tccvid_vsync.isVsyncRunning )
			{
				printk("vsync already ended !! %d buffer_unique_id %d \n",input_image.time_stamp, input_image.buffer_unique_id);
				
				spin_lock_irq(&vsync_lockDisp) ;
				tccvid_vsync.vsync_buffer.last_cleared_buff_id = input_image.buffer_unique_id;
				spin_unlock_irq(&vsync_lockDisp) ;
				goto TCC_VSYNC_PUSH_ERROR;
				//input_image.time_stamp = 0;
				//return 0;
			}

			spin_lock_irq(&vsync_lock) ;
			check_time = abs(tcc_vsync_get_time() - input_image.sync_time);
			spin_unlock_irq(&vsync_lock) ;

			if(check_time> 200)
			{
				vprintk("reset time base time %d kernel time %d time %d \n",tccvid_vsync.baseTime,tcc_vsync_get_timer_clock(),input_image.sync_time);
				tccfb_ResetSyncTime(0);
			}
			
			tccfb_calculateSyncTime(input_image.sync_time);

			if(tccvid_vsync.outputMode < 0)
			{
				switch(input_image.outputMode)
				{
					case OUTPUT_NONE:
						tccvid_vsync.outputMode = TCC_OUTPUT_NONE;			
						break;
					case OUTPUT_HDMI:
						tccvid_vsync.outputMode = TCC_OUTPUT_HDMI;
						tcc_check_interlace_output(tccvid_vsync.outputMode);
						break;
					case OUTPUT_COMPOSITE:
						tccvid_vsync.outputMode = TCC_OUTPUT_COMPOSITE; 		
						tcc_check_interlace_output(tccvid_vsync.outputMode);
						break;
					case OUTPUT_COMPONENT:
						tccvid_vsync.outputMode = TCC_OUTPUT_COMPONENT; 		
						tcc_check_interlace_output(tccvid_vsync.outputMode);
						break;
					default:
						tccvid_vsync.outputMode = TCC_OUTPUT_NONE;									
				}
				
				vprintk("tccvid_vsync.outputMode %d input_image.outputMode %d \n", tccvid_vsync.outputMode ,input_image.outputMode);			
			}

			if(tccvid_vsync.deinterlace_mode < 0)
			{
				unsigned int lcdCtrlNum, lcd_width, lcd_height;
				VIOC_DISP * pDISPBase;
						
				if((tccvid_vsync.outputMode == TCC_OUTPUT_NONE))
					lcdCtrlNum = LCD_LCDC_NUM;
				else
					lcdCtrlNum = EX_OUT_LCDC; 
				
				/* Set the output resolution */
				if (lcdCtrlNum)
					pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP1);
				else
					pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP0);

				VIOC_DISP_GetSize(pDISPBase, &lcd_width, &lcd_height);
				
				tccvid_vsync.deinterlace_mode = input_image.deinterlace_mode;
				tccvid_vsync.frameInfo_interlace = input_image.frameInfo_interlace;
				tccvid_vsync.output_toMemory = input_image.output_toMemory;
				printk("### deinterlace_mode(%d), output_toMemory(%d)\n", tccvid_vsync.deinterlace_mode, tccvid_vsync.output_toMemory);
				
				tccvid_vsync.interlace_bypass_lcdc = 0;
			
				if( (tccvid_vsync.deinterlace_mode == 1) && 
					(tccvid_vsync.interlace_output == 1) && 
					(input_image.Frame_width == input_image.Image_width) && (input_image.Frame_height == input_image.Image_height) )
				{
					printk("### interlace_bypass_lcdc set !!\n");
					tccvid_vsync.interlace_bypass_lcdc = 1;
				}
			}

#ifdef TCC_VIDEO_DISPLAY_DEINTERLACE_MODE
			if(tccvid_vsync.deinterlace_mode && !tccvid_vsync.output_toMemory && tccvid_vsync.firstFrameFlag &&!tccvid_vsync.interlace_bypass_lcdc)
			{
				int lcdCtrlNum;
				printk("first TCC_excuteVIQE_60Hz \n") ;

				if((tccvid_vsync.outputMode == TCC_OUTPUT_NONE))
					lcdCtrlNum = LCD_OUT_LCDC;
				else
					lcdCtrlNum = EX_OUT_LCDC;	

				tccvid_vsync.nDeinterProcCount = 0;
				tccvid_vsync.m2m_mode =  input_image.m2m_mode;

				TCC_VIQE_DI_Init60Hz(lcdCtrlNum, input_image.Lcdc_layer, input_image.on_the_fly, input_image.fmt, 
										input_image.Frame_width, input_image.Frame_height,	// srcWidth, srcHeight
										input_image.Image_width, input_image.Image_height,
										input_image.offset_x, input_image.offset_y,
										input_image.odd_first_flag);
			}
#endif
			
			tccvid_vsync.firstFrameFlag = 0;

			if(input_image.output_path)
			{
			
			printk("### output_path buffer_unique_id %d \n",input_image.buffer_unique_id);
				spin_lock_irq(&vsync_lockDisp) ;
				tcc_vsync_pop_all_buffer(&tccvid_vsync.vsync_buffer);
				tccvid_vsync.vsync_buffer.last_cleared_buff_id = input_image.buffer_unique_id;
				spin_unlock_irq(&vsync_lockDisp) ;

				if(!tccvid_vsync.output_toMemory && !tccvid_vsync.interlace_bypass_lcdc)		//tccvid_vsync.deinterlace_mode && //
					tccvid_vsync.nDeinterProcCount = 0;
				else if(tccvid_vsync.interlace_bypass_lcdc)
					tccvid_vsync.nDeinterProcCount =0;
				else
				{
					/*
					switch(Output_SelectMode)
					{
						case TCC_OUTPUT_NONE:
							TCC_HDMI_DISPLAY_UPDATE(LCD_OUT_LCDC, &input_image);
							break;
						case TCC_OUTPUT_HDMI:
							TCC_HDMI_DISPLAY_UPDATE(EX_OUT_LCDC, &input_image);
							break;
						case TCC_OUTPUT_COMPONENT:
						#if defined(CONFIG_FB_TCC_COMPONENT)
								tcc_component_update(&input_image);
						#endif
							break;
						case TCC_OUTPUT_COMPOSITE:
						#if defined(CONFIG_FB_TCC_COMPOSITE)
								tcc_composite_update(&input_image);
						#endif
							break;
						default:
							break;
					}
					*/

				input_image.time_stamp = 0;
					return tccvid_vsync.vsync_buffer.writeIdx;

				}

				input_image.time_stamp = 0;
				goto PUSH_VIDEO_FORCE;
			}
			
			if(tccvid_vsync.skipFrameStatus)
			{
				error_type = 1;
			}
			else if(tccvid_vsync.outputMode != Output_SelectMode)
			{
				vprintk("vsync push error : output mode different %d %d \n", tccvid_vsync.outputMode ,Output_SelectMode);			
				error_type = 2;
			}
			else if(tccvid_vsync.vsync_buffer.available_buffer_id_on_vpu > input_image.buffer_unique_id)
			{
				vprintk("vsync push error : buffer index sync fail omx_buf_id: %d, cur_buff_id: %d \n", 
						tccvid_vsync.vsync_buffer.available_buffer_id_on_vpu, input_image.buffer_unique_id);
				error_type = 3;
			}
			else if(input_image.time_stamp < input_image.sync_time)
			{
				if(input_image.time_stamp < 2000)
				{
					vprintk("vsync push error 4: vtime: %d, sync_time: %d \n", input_image.time_stamp, input_image.sync_time);
					error_type = 4;
				}
				else
				{
					//printk("vsync push error : vtime: %d, writeIdx %d \n", input_image.time_stamp, tccvid_vsync.vsync_buffer.writeIdx);
					input_image.time_stamp = TIME_MARK_SKIP;
					
				}
			}

			if(error_type > 0)
			{
				vprintk("vsync push error : %d buffer_unique_id %d\n", error_type,input_image.buffer_unique_id);
				spin_lock_irq(&vsync_lockDisp) ;
				tccvid_vsync.vsync_buffer.last_cleared_buff_id = input_image.buffer_unique_id;
				spin_unlock_irq(&vsync_lockDisp) ;
				goto TCC_VSYNC_PUSH_ERROR;
			}

			int curTime = tcc_vsync_get_time();

PUSH_VIDEO_FORCE : 
			if(tcc_vsync_is_full_buffer(&tccvid_vsync.vsync_buffer))
			{
				vprintk("mio push wait start\n") ;
				wait_event_interruptible_timeout( wq_consume, 
												tcc_vsync_is_full_buffer(&tccvid_vsync.vsync_buffer) == 0, 
												msecs_to_jiffies(500)) ;
				vprintk("mio push wait end \n");
			}

			spin_lock_irq(&vsync_lock) ;
			if(tcc_vsync_push_buffer(&tccvid_vsync.vsync_buffer, &input_image) < 0)
			{
				printk("critical error: vsync buffer full by fault buffer controll\n");
			}

			if(tccvid_vsync.deinterlace_mode && !tccvid_vsync.output_toMemory && !tccvid_vsync.interlace_bypass_lcdc){
			viqe_render_frame(input_image.addr0, input_image.addr1, input_image.addr2, input_image.odd_first_flag, tccvid_vsync.vsync_interval, curTime,
							input_image.Frame_width, input_image.Frame_height,	// srcWidth, srcHeight
							input_image.crop_top, input_image.crop_bottom, input_image.crop_left, input_image.crop_right,
							input_image.Image_width, input_image.Image_height,
							input_image.offset_x, input_image.offset_y, input_image.frameInfo_interlace);
			}
			
			spin_unlock_irq(&vsync_lock) ;
			//printk("OddFirst = %d, interlaced %d\n", input_image.odd_first_flag, input_image.deinterlace_mode);
			vprintk("vtime : %d, curtime : %d\n", input_image.time_stamp, input_image.sync_time) ;

		}

TCC_VSYNC_PUSH_ERROR:
		return tccvid_vsync.vsync_buffer.writeIdx;
		break ;
		
	case TCC_LCDC_VIDEO_GET_DISPLAYED :
		
//		if(tccvid_vsync.outputMode == Output_SelectMode)
		{
			if(tccvid_vsync.skipFrameStatus || !vsync_started)
			{
				if(tccvid_vsync.skipFrameStatus)
					vprintk("frame skip mode return\n");
				return -1;
			}
			
			//printk("# last disp num(%d) \n", tccvid_vsync.vsync_buffer.last_cleared_buff_id ) ;
			return tccvid_vsync.vsync_buffer.last_cleared_buff_id ? (tccvid_vsync.vsync_buffer.last_cleared_buff_id-1):0 ; 
			
		}
		break ;
		
	case TCC_LCDC_VIDEO_GET_VALID_COUNT:
		return tccvid_vsync.vsync_buffer.readable_buff_count; 
		break ;
		
	case TCC_LCDC_VIDEO_CLEAR_FRAME:
		{
			int syncBufferIdx;
			syncBufferIdx = arg;
			printk("video fram clear %d\n", syncBufferIdx) ;

			spin_lock_irq(&vsync_lock) ;
			tcc_vsync_pop_all_buffer(&tccvid_vsync.vsync_buffer);
			spin_unlock_irq(&vsync_lock) ;
			printk("valid video fram count %d\n", tccvid_vsync.vsync_buffer.valid_buff_count) ;

			tccvid_vsync.vsync_buffer.available_buffer_id_on_vpu = syncBufferIdx+1;

		}
		break ;
		
	case TCC_LCDC_VIDEO_SKIP_FRAME_START:
		{
			vprintk("video fram skip start\n") ;
			printk("TCC_LCDC_VIDEO_SKIP_FRAME_START \n") ;
			tccvid_vsync.skipFrameStatus = 1;
		}
		break ;

	case TCC_LCDC_VIDEO_SKIP_FRAME_END:
		if(tccvid_vsync.skipFrameStatus)
		{
			vprintk("video fram skip end\n") ;
			spin_lock_irq(&vsync_lock) ;
			tcc_vsync_pop_all_buffer(&tccvid_vsync.vsync_buffer);
			spin_unlock_irq(&vsync_lock);
			printk("TCC_LCDC_VIDEO_SKIP_FRAME_END \n") ;

			tccvid_vsync.skipFrameStatus = 0;
			tccfb_ResetSyncTime(0);
		}
		break ;
		
	case TCC_LCDC_VIDEO_SKIP_ONE_FRAME:
		{
			spin_lock_irq(&vsync_lockDisp) ;
			printk("TCC_LCDC_VIDEO_SKIP_ONE_FRAME arg %d\n",arg) ;
			//tccvid_vsync.vsync_buffer.last_cleared_buff_id = arg;
			spin_unlock_irq(&vsync_lockDisp) ;
		}
		break ;
#endif
		default:
			dprintk("ioctl: Unknown [%d/0x%X]", cmd, cmd);
			break;
	}


	return 0;
}

static void schedule_palette_update(struct tccfb_info *fbi,
				    unsigned int regno, unsigned int val)
{
	unsigned long flags;

	local_irq_save(flags);

	local_irq_restore(flags);
}

/* from pxafb.c */
static inline unsigned int chan_to_field(unsigned int chan, struct fb_bitfield *bf)
{
	chan &= 0xffff;
	chan >>= 16 - bf->length;
	return chan << bf->offset;
}

static int tccfb_setcolreg(unsigned regno,
			       unsigned red, unsigned green, unsigned blue,
			       unsigned transp, struct fb_info *info)
{
	struct tccfb_info *fbi = info->par;
	unsigned int val;

	/* dprintk("setcol: regno=%d, rgb=%d,%d,%d\n", regno, red, green, blue); */

	switch (fbi->fb->fix.visual) {
		case FB_VISUAL_TRUECOLOR:
			/* true-colour, use pseuo-palette */

			if (regno < 16) {
				u32 *pal = fbi->fb->pseudo_palette;

				val  = chan_to_field(red,   &fbi->fb->var.red);
				val |= chan_to_field(green, &fbi->fb->var.green);
				val |= chan_to_field(blue,  &fbi->fb->var.blue);

				pal[regno] = val;
			}
			break;

		case FB_VISUAL_PSEUDOCOLOR:
			if (regno < 256) {
				/* currently assume RGB 5-6-5 mode */

				val  = ((red   >>  0) & 0xf800);
				val |= ((green >>  5) & 0x07e0);
				val |= ((blue  >> 11) & 0x001f);

				//writel(val, S3C2410_TFTPAL(regno));
				schedule_palette_update(fbi, regno, val);
			}
			break;

		default:
			return 1;   /* unknown type */
	}

	return 0;
}


/**
 *      tccfb_blank
 *	@blank_mode: the blank mode we want.
 *	@info: frame buffer structure that represents a single frame buffer
 *
 *	Blank the screen if blank_mode != 0, else unblank. Return 0 if
 *	blanking succeeded, != 0 if un-/blanking failed due to e.g. a
 *	video mode which doesn't support it. Implements VESA suspend
 *	and powerdown modes on hardware that supports disabling hsync/vsync:
 *	blank_mode == 2: suspend vsync
 *	blank_mode == 3: suspend hsync
 *	blank_mode == 4: powerdown
 *
 *	Returns negative errno on error, or zero on success.
 *
 */
static int tccfb_blank(int blank_mode, struct fb_info *info)
{
	dprintk("blank(mode=%d, info=%p)\n", blank_mode, info);
	return 0;
}


#ifdef CONFIG_HAS_EARLYSUSPEND
static void tcc_fb_earlier_suspend(struct early_suspend *h)
{
	printk("%s:  \n", __FUNCTION__);

#if 0
	if(Output_SelectMode == TCC_OUTPUT_HDMI)
	{
		Output_SelectMode = TCC_OUTPUT_NONE;
		TCC_OUTPUT_LCDC_OnOff(TCC_OUTPUT_HDMI, EX_OUT_LCDC, 0);
		TCC_FB_LCDC_NumSet(1, 0);
	}
#endif
	tca_fb_earlier_suspend(h);
}

static void tcc_fb_early_suspend(struct early_suspend *h)
{
	printk("%s:  \n", __FUNCTION__);
	console_lock();
	if((system_rev != 0x2004) && (system_rev != 0x2005))
	{
		fb_power_state = 0;

		if (lcd_panel->set_power)
			lcd_panel->set_power(lcd_panel, 0, LCD_LCDC_NUM);
	}
	tca_fb_early_suspend(h);
	
	console_unlock();
	printk("%s: finish \n", __FUNCTION__);

}

static void tcc_fb_late_resume(struct early_suspend *h)
{
	printk("%s:  \n", __FUNCTION__);
	
	console_lock();

	tca_fb_late_resume(h);
	if((system_rev != 0x2004) && (system_rev != 0x2005))
	{
		if (lcd_panel->set_power)
			lcd_panel->set_power(lcd_panel, 1, LCD_LCDC_NUM);
		
		fb_power_state = 1;
	}
	
	console_unlock();
	printk("%s: finish \n", __FUNCTION__);

}

static void tcc_fb_later_resume(struct early_suspend *h)
{
	printk("%s:  \n", __FUNCTION__);
	tca_fb_later_resume(h);
}
#endif

#ifdef CONFIG_PM
/* suspend and resume support for the lcd controller */
static int tccfb_suspend(struct platform_device *dev, pm_message_t state)
{

	if((system_rev == 0x2004) || (system_rev == 0x2005))
	{
		fb_power_state = 0;

		if (lcd_panel->set_power)
			lcd_panel->set_power(lcd_panel, 0, LCD_LCDC_NUM);
	}

	tca_fb_suspend(dev, state);
	return 0;
}

static int tccfb_resume(struct platform_device *dev)
{
	tca_fb_resume(dev);

	if((system_rev == 0x2004) || (system_rev == 0x2005))
	{
		if (lcd_panel->set_power)
			lcd_panel->set_power(lcd_panel, 1, LCD_LCDC_NUM);
		
		fb_power_state = 1;	
	}
	return 0;
}

static void tccfb_shutdown(struct platform_device *dev)
{
	pm_message_t state = {0};
	tccfb_suspend(dev, state);
}

#else
#define tccfb_suspend	NULL
#define tccfb_resume	NULL
#define tccfb_shutdown	NULL
#endif



static struct fb_ops tccfb_ops = {
	.owner			= THIS_MODULE,
	.fb_check_var	= tccfb_check_var,
	.fb_set_par		= tccfb_set_par,
	.fb_blank		= tccfb_blank,
	.fb_setcolreg	= tccfb_setcolreg,
	.fb_fillrect	= cfb_fillrect,
	.fb_copyarea	= cfb_copyarea,
	.fb_imageblit	= cfb_imageblit,
	.fb_ioctl		= tccfb_ioctl,
	.fb_pan_display = tccfb_pan_display,
};


/*
 * tccfb_map_video_memory():
 *	Allocates the DRAM memory for the frame buffer.  This buffer is
 *	remapped into a non-cached, non-buffered, memory region to
 *	allow palette and pixel writes to occur without flushing the
 *	cache.  Once this area is remapped, all virtual memory
 *	access to the video memory should occur at the new region.
 */

static int __init tccfb_map_video_memory(struct tccfb_info *fbi, int plane)
{

	fbi->map_size = PAGE_ALIGN(fbi->fb->fix.smem_len + PAGE_SIZE);


	if(FB_VIDEO_MEM_SIZE == 0)
	{
		fbi->map_cpu = dma_alloc_writecombine(fbi->dev, fbi->map_size, &fbi->map_dma, GFP_KERNEL);
		printk("map_video_memory (fbi=%p) kernel memory, dma:0x%x map_size:%08x\n", fbi,fbi->map_dma, fbi->map_size);
	}
	else
	{
		fbi->map_dma =  FB_VIDEO_MEM_BASE;
		fbi->map_cpu = ioremap_nocache(fbi->map_dma, fbi->map_size);
		printk("map_video_memory (fbi=%p) used map memory,map dma:0x%x size:%08x\n", fbi, fbi->map_dma ,fbi->map_size);
	}

	fbi->map_size = fbi->fb->fix.smem_len;

	if (fbi->map_cpu) {
		/* prevent initial garbage on screen */
		dprintk("map_video_memory: clear %p:%08x\n", fbi->map_cpu, fbi->map_size);

		memset(fbi->map_cpu, 0x00, fbi->map_size);

		fbi->screen_dma		= fbi->map_dma;
		fbi->fb->screen_base	= fbi->map_cpu;
		fbi->fb->fix.smem_start  = fbi->screen_dma;

		// Set the LCD frame buffer start address
		switch (plane)
		{
			case 2:	// IMG2
				fb_mem_vaddr[plane] = fbi->map_cpu;
				fb_mem_size [plane] = fbi->map_size;
				break;
			case 1:	// IMG1
				fb_mem_vaddr[plane] = fbi->map_cpu;
				fb_mem_size [plane] = fbi->map_size;
				break;
			case 0:	// IMG0
				fb_mem_vaddr[plane] = fbi->map_cpu;
				fb_mem_size [plane] = fbi->map_size;
				break;
		}
		dprintk("map_video_memory: dma=%08x cpu=%p size=%08x\n",
			fbi->map_dma, fbi->map_cpu, fbi->fb->fix.smem_len);
	}

	return fbi->map_cpu ? 0 : -ENOMEM;
}

static inline void tccfb_unmap_video_memory(struct tccfb_info *fbi)
{
	dma_free_writecombine(fbi->dev,fbi->map_size,fbi->map_cpu, fbi->map_dma);

	fb_mem_vaddr[fbi->imgch] = (u_char*)NULL;
	fb_mem_size [fbi->imgch] = (u_int)NULL;
}

static char tccfb_driver_name[]="tccfb";
static int __init tccfb_probe(struct platform_device *pdev)
{
	struct tccfb_info *info;
	struct fb_info *fbinfo;
	int ret;
	int plane = 0;
	unsigned int screen_width, screen_height;

	if (!lcd_panel) {
		pr_err("tccfb: no LCD panel data\n");
		return -EINVAL;
	}
	pr_info("LCD panel is %s %s %d x %d\n", lcd_panel->manufacturer, lcd_panel->name,
		lcd_panel->xres, lcd_panel->yres);

        screen_width      = lcd_panel->xres;
        screen_height     = lcd_panel->yres;

#if defined(CONFIG_TCC_HDMI_UI_SIZE_1280_720)
        if(tcc_display_data.resolution == 1)
        {
            screen_width      = 720;
            screen_height     = 576;
        }
        else if(tcc_display_data.resolution == 2)
        {
            screen_width 	  = 800;
            screen_height 	  = 480;
        }
#endif

	printk("%s, screen_width=%d, screen_height=%d\n", __func__, screen_width, screen_height);

	pmap_get_info("fb_video", &pmap_fb_video);

#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
	pmap_get_info("exclusive_viqe", &pmap_eui_viqe);
#endif
	tcc_lcd_interrupt_reg(TRUE);

	for (plane = 0; plane < CONFIG_FB_TCC_DEVS; plane++)
	{
		fbinfo = framebuffer_alloc(sizeof(struct tccfb_info), &pdev->dev);
		info = fbinfo->par;
		info->fb = fbinfo;
		//platform_set_drvdata(pdev, fbinfo);

		strcpy(fbinfo->fix.id, tccfb_driver_name);

		fbinfo->fix.type		= FB_TYPE_PACKED_PIXELS;
		fbinfo->fix.type_aux		= 0;
		fbinfo->fix.xpanstep		= 0;
		fbinfo->fix.ypanstep		= 1;
		fbinfo->fix.ywrapstep		= 0;
		fbinfo->fix.accel		= FB_ACCEL_NONE;

		fbinfo->var.nonstd		= 0;
		fbinfo->var.activate		= FB_ACTIVATE_NOW;

		fbinfo->var.accel_flags		= 0;
		fbinfo->var.vmode		= FB_VMODE_NONINTERLACED;

		fbinfo->fbops			= &tccfb_ops;
		fbinfo->flags			= FBINFO_FLAG_DEFAULT;

		fbinfo->var.xres		= screen_width;
		fbinfo->var.xres_virtual	= fbinfo->var.xres;
		fbinfo->var.yres		= screen_height;

#ifdef TCC_FB_DOUBLE
		fbinfo->var.yres_virtual	= fbinfo->var.yres * 2;
#else
		fbinfo->var.yres_virtual	= fbinfo->var.yres;
#endif//
		fbinfo->var.bits_per_pixel	= default_scn_depth[plane];
		fbinfo->fix.line_length 	= fbinfo->var.xres * fbinfo->var.bits_per_pixel/8;

		tccfb_check_var(&fbinfo->var, fbinfo);

		// the memory size that LCD should occupy
		fbinfo->fix.smem_len = fbinfo->var.xres * fbinfo->var.yres_virtual * SCREEN_DEPTH_MAX / 8;

		info->imgch = plane;

		/* Initialize video memory */
		ret = tccfb_map_video_memory(info, plane);
		if (ret) {
			dprintk( KERN_ERR "Failed to allocate video RAM: %d\n", ret);
			ret = -ENOMEM;
		}

		ret = register_framebuffer(fbinfo);
		if (ret < 0) {
			dprintk(KERN_ERR "Failed to register framebuffer device: %d\n", ret);
			goto free_video_memory;
		}

//		fbinfo->var.reserved[0] = 0x54445055;

		tccfb_set_par(fbinfo);

		if (plane == 0)	// top layer
			if (fb_prepare_logo(fbinfo,	FB_ROTATE_UR)) {
			/* Start display and show logo on boot */
			fb_set_cmap(&fbinfo->cmap, fbinfo);

			dprintk("fb_show_logo\n");
			fb_show_logo(fbinfo, FB_ROTATE_UR);
        	}
		printk(KERN_INFO "fb%d: %s frame buffer device\n",	fbinfo->node, fbinfo->fix.id);
	}



#ifdef CONFIG_HAS_EARLYSUSPEND
	info->early_suspend.suspend = tcc_fb_early_suspend;
	info->early_suspend.resume 	= tcc_fb_late_resume;
	info->early_suspend.level 	= EARLY_SUSPEND_LEVEL_DISABLE_FB - 2;
	register_early_suspend(&info->early_suspend);

	info->earlier_suspend.suspend = tcc_fb_earlier_suspend;
	info->earlier_suspend.resume 	= tcc_fb_later_resume;
	info->earlier_suspend.level 	= EARLY_SUSPEND_LEVEL_BLANK_SCREEN;
	register_early_suspend(&info->earlier_suspend);
#endif

	return 0;

free_video_memory:
	tccfb_unmap_video_memory(info);
	dprintk("TCC92xx fb init failed.\n");
	return ret;
}


/*
 *  Cleanup
 */
static int tccfb_remove(struct platform_device *pdev)
{
	struct fb_info	   *fbinfo = platform_get_drvdata(pdev);
	struct tccfb_info *info = fbinfo->par;

	dprintk(" %s  \n", __func__);

	tcc_lcd_interrupt_reg(FALSE);

	tccfb_unmap_video_memory(info);

	//release_mem_region((unsigned long)S3C24XX_VA_LCD, S3C24XX_SZ_LCD);
	unregister_framebuffer(fbinfo);

	return 0;
}

int tccfb_register_panel(struct lcd_panel *panel)
{
	dprintk(" %s  name:%s \n", __func__, panel->name);

	lcd_panel = panel;
	return 1;
}
EXPORT_SYMBOL(tccfb_register_panel);

struct lcd_panel *tccfb_get_panel(void)
{
	return lcd_panel;
}
EXPORT_SYMBOL(tccfb_get_panel);

static struct platform_driver tccfb_driver = {
	.probe		= tccfb_probe,
	.remove		= tccfb_remove,
	.suspend	= tccfb_suspend,
	.shutdown	= tccfb_shutdown,
	.resume		= tccfb_resume,
	.driver		= {
		.name	= "tccxxx-lcd",
		.owner	= THIS_MODULE,
	},
};

//int __devinit tccfb_init(void)
static int __init tccfb_init(void)
{
    printk(KERN_INFO " %s (built %s %s)\n", __func__, __DATE__, __TIME__);

	fb_power_state = 1;

	EX_OUT_LCDC = tca_get_output_lcdc_num();
	LCD_LCDC_NUM = tca_get_lcd_lcdc_num();

	tca_fb_init();

#ifdef TCC_VIDEO_DISPLAY_BY_VSYNC_INT
	spin_lock_init(&vsync_lock) ;
	spin_lock_init(&vsync_lockDisp ) ;
#endif

	printk(" %s  \n", __func__);

	return platform_driver_register(&tccfb_driver);
}

static void __exit tccfb_exit(void)
{
	dprintk(" %s \n", __func__);

	tca_fb_exit();
	platform_driver_unregister(&tccfb_driver);
}


module_init(tccfb_init);
module_exit(tccfb_exit);

MODULE_AUTHOR("linux <linux@telechips.com>");
MODULE_DESCRIPTION("Telechips TCC Framebuffer driver");
MODULE_LICENSE("GPL");
