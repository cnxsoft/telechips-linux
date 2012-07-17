
/*
 * drivers/media/video/tcccam/tcc83x_cif.c
 *
 * Copyright (C) 2008 Telechips, Inc. 
 * 
 * Video-for-Linux (Version 2) camera capture driver for
 * the TCC78xx camera controller.
 *
 * leverage some code from CEE distribution 
 * Copyright (C) 2003-2004 MontaVista Software, Inc.
 * 
 * This package is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
 * 
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED 
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. 
 */
 
#include <generated/autoconf.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/ctype.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <linux/sched.h>
#include <linux/irq.h>

#include <mach/irqs.h>
//#include <asm/arch/dma.h>
#include <mach/hardware.h>
#include <asm/io.h>

#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
#include <mach/bsp.h>
#include <mach/reg_physical.h>
#elif defined(CONFIG_ARCH_TCC79X)
#include <mach/tcc79x.h>
#include <asm/plat-tcc/tcc_ckc.h>
#endif

#ifdef CONFIG_ARCH_TCC892X
//#include <mach/vioc_outcfg.h>
//#include <mach/vioc_rdma.h>
#include <mach/vioc_ireq.h>
#include <mach/vioc_config.h>
#include <mach/vioc_wdma.h>
#include <mach/vioc_wmix.h>
#include <mach/vioc_scaler.h>
#include <mach/vioc_vin.h>
#include <mach/vioc_viqe.h>
#include <mach/vioc_global.h>
#include <mach/vioc_plugin_tcc892x.h>
#endif

#include <mach/memory.h>
#include <asm/scatterlist.h>
#include <asm/mach-types.h>

#include "sensor_if.h"
#include "tcc_cam.h"
#include "camera_hw_if.h"
#include "camera_core.h"
#include "tdd_cif.h"
#include "cam.h"
#include "cam_reg.h"
#include <plat/pmap.h>
#include <mach/tcc_cam_ioctrl.h>
#include "tcc_cam_i2c.h"
#if defined(CONFIG_USE_ISP)
#include "tccisp/isp_interface.h"
#endif


#define FEATURE_ANDROID_ICS

static int debug	   = 0;
#define dprintk(msg...)	if (debug) { printk( "Tcc_cam: " msg); }

#if defined(CONFIG_VIDEO_CAMERA_SENSOR_AIT848_ISP)
extern unsigned int gnSizeOfAITALLJPEG;
extern unsigned int gnSizeOfAITMainJPEG;
extern unsigned int gnSizeOfAITThumbnail;
extern unsigned int gnoffsetOfAITThumbnail;
extern unsigned short gnWidthOfAITMainJPEG;
extern unsigned short gnHeightOfAITMainJPEG;
#endif

#ifdef JPEG_ENCODE_WITH_CAPTURE
#include "tcc83xx/tcc83xx_jpeg.h"
#include "tcc83xx/jpeg_param.h"

JPEG_ENC_INFO  gJPEG_ENC_Info;
JPEG_DEC_INFO  gJPEG_DEC_Info;
JPEG_BUFFER_CTRL gJPEG_Buffer_Info;
JPEG_DISPLAY_INFO JPEG_Info;
jpeg_parser_rsp gJpegInfo;
extern volatile char gJpegDecodeDone;
extern volatile char gJpegEncodeDone;
unsigned char gJpegCodecMode = JPEG_UNKNOWN_MODE;
extern uint8 gIsExifThumbnailProcess;
extern jpeg_encode_option_type gEncodeOption;

unsigned char gIsRolling;

void *jpeg_header_remapped_address;   // Header Buffer
void *jpeg_remapped_base_address;

//#define JPEG_STREAM      	0x180000 //1600*1024
//#define JPEG_HEADER      	0x020000 //96*1024
#define JPEG_STREAM      	0x1D0000 //1856*1024
#define JPEG_HEADER      	0x030000 //192*1024

#define CAPTURE_PHY_ADDRESS	PA_VIDEO_BASE
#define JPEG_PHY_ADDRESS	PA_VIDEO_BASE + CAPTURE_MEM
#endif

static pmap_t pmap_ump_reserved;
#define PA_PREVIEW_BASE_ADDR 		pmap_ump_reserved.base
#define PREVIEW_MEM_SIZE 			pmap_ump_reserved.size

static pmap_t pmap_jpeg_raw;
#define PA_JPEG_RAW_BASE_ADDR 		pmap_jpeg_raw.base
#define JPEG_RAW_MEM_SIZE 			pmap_jpeg_raw.size
#define PA_TEMP 					(PA_JPEG_RAW_BASE_ADDR + JPEG_RAW_MEM_SIZE / 2)

u8 current_effect_mode = 0;
struct TCCxxxCIF hardware_data;
#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
TCC_SENSOR_INFO_TYPE tcc_sensor_info;
int CameraID;
#endif

#define NUM_FREQS 2
static unsigned int gtCamSizeTable[NUM_FREQS] =
{
	720 * 480 , //D1,	
	1280 * 720, //HD720P,
};

#define CAMERA_LOOP_LIMIT_COUNT 	3000 //20120425 ysseung   infinite loop limit is 3sec.
int cam_loop_lmt_cnt = 0; //20120404 ysseung   if the camera is not connected, modify to an infinite loop issue of camera stop.
int cam_no_connect_cnt = 0; //20120404 ysseung   if the camera is not connected, modify to an infinite loop issue of camera stop.

void cif_timer_register(void* priv, unsigned long timeover);
void cif_dma_hw_reg(unsigned char frame_num);

static unsigned char cam_irq = 0;
static unsigned char skip_frm = 0, skipped_frm = 0;
static unsigned char frame_lock = 0;
static unsigned char register_timer = 0;
static unsigned char cam_open = 0, cam_close = 1;
static unsigned char *reg_buf = NULL;
#if defined(CONFIG_USE_ISP)
#ifdef TCCISP_GEN_CFG_UPD
static 	struct tccxxx_cif_buffer * next_buf = 0;
static unsigned next_num = 0;
static unsigned chkpnt = 0;
#endif
struct tccxxx_cif_buffer *prev_buf;
static unsigned int		prev_num;

#endif

#ifdef CONFIG_ARCH_TCC892X
static VIOC_VIN_DEMUX * pVINDemuxBase;
static VIOC_WMIX* 		pWMIXBase;
static VIOC_WDMA*		pWDMABase;
static VIOC_VIN* 		pVINBase;
static VIOC_SC* 		pSCBase;
static DDICONFIG*		pDDIConfig;
static VIQE*			pVIQEBase;
static VIOC_IREQ_CONFIG* 		pVIOCConfig;
static unsigned int old_zoom_step;
struct tccxxx_cif_buffer *prev_buf;
static unsigned int		prev_num;
#ifdef	CONFIG_VIDEO_ATV_SENSOR_TVP5150
static uint				bfield;
static uint				frm_cnt;
static uint				field_cnt;
#endif
#endif

void cif_set_frameskip(unsigned char skip_count, unsigned char locked)
{
	skip_frm = skip_count;
	frame_lock = locked;
}
EXPORT_SYMBOL(cif_set_frameskip);

int cif_get_capturedone(void)
{
	if(hardware_data.cif_cfg.cap_status == CAPTURE_DONE) {
		mdelay(5);
		return 1;
	}
	return 0;
}
EXPORT_SYMBOL(cif_get_capturedone);

int Get_Index_sizeTable(unsigned int Image_Size)
{
	int i;
	for(i=0; i < NUM_FREQS; i++) {
		if(gtCamSizeTable[i] >= Image_Size) return i;
	}
	return (NUM_FREQS - 1);
}
EXPORT_SYMBOL(Get_Index_sizeTable);

int tccxxx_isalive_camera(void)
{
	unsigned short HSize, VSize;
#if	defined(CONFIG_ARCH_TCC892X)
	// In case of 892X, we have to add.
#else
	volatile PCKC pCKC = (PCKC)tcc_p2v(HwCKC_BASE);
	volatile PCIF pCIF = (PCIF)tcc_p2v(HwCIF_BASE);
#endif

#if defined(CONFIG_USE_ISP)
	struct TCCxxxCIF *data = (struct TCCxxxCIF *)&hardware_data;

	if(pCKC->PCLK_ISPS & Hw29)
	{
		dprintk("JPEG - Cam Alive = %d x %d \n", data->cif_cfg.main_set.target_x, data->cif_cfg.main_set.target_y);
		return (data->cif_cfg.main_set.target_x * data->cif_cfg.main_set.target_y);
	}
#elif defined(CONFIG_ARCH_TCC892X)
	// In case of 892X, we have to add.	
#else
	#if defined(CONFIG_ARCH_TCC92XX)
	if(pCKC->PCLK_CIFMC & Hw28)
	#else // CONFIG_ARCH_TCC93XX or CONFIG_ARCH_TCC88XX
	if(pCKC->PCLK_CIFMC & Hw29)
	#endif
	{
		if(pCIF->ICPCR1 & Hw31) {
			HSize = (pCIF->IIS >> 16) - (pCIF->IIW1 >> 16) - (pCIF->IIW1 & 0xFFFF);
			VSize = (pCIF->IIS & 0xFFFF) - (pCIF->IIW2 >> 16) - (pCIF->IIW2 & 0xFFFF);
			dprintk("JPEG - Cam Alive = %d x %d \n", HSize, VSize);
			return HSize * VSize;
		}
	}
#endif

	return 0;
}
EXPORT_SYMBOL(tccxxx_isalive_camera);


static void cif_data_init(void *priv)
{
	// 카메라 초기화 
	struct TCCxxxCIF *data = (struct TCCxxxCIF *)priv;

#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	// Default Setting
	data->cif_cfg.polarity_pclk 	= tcc_sensor_info.p_clock_pol;
	data->cif_cfg.polarity_vsync 	= tcc_sensor_info.v_sync_pol;
	data->cif_cfg.polarity_href 	= tcc_sensor_info.h_sync_pol;
#ifdef CONFIG_ARCH_TCC892X
	data->cif_cfg.polarity_href 	= tcc_sensor_info.de_pol;
#endif
	data->cif_cfg.zoom_step			= 0;
	data->cif_cfg.base_buf			= hardware_data.cif_buf.addr;
	data->cif_cfg.pp_num			= 0; // TCC_CAMERA_MAX_BUFNBRS;
	data->cif_cfg.fmt				= tcc_sensor_info.format;
	data->cif_cfg.order422 			= CIF_YCBYCR;
	data->cif_cfg.oper_mode 		= OPER_PREVIEW;
	data->cif_cfg.main_set.target_x = 0;
	data->cif_cfg.main_set.target_y = 0;
	data->cif_cfg.esd_restart 		= OFF;
	data->cif_cfg.cap_status 		= CAPTURE_NONE;
	data->cif_cfg.retry_cnt			= 0;
	current_effect_mode = 0;
#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	//Default Setting
	data->cif_cfg.order422 			= CIF_YCBYCR;

#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_AIT848_ISP) \
	|| defined(CONFIG_VIDEO_CAMERA_SENSOR_ISX006) || defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150) || defined(CONFIG_VIDEO_ATV_SENSOR_RDA5888) \
	|| defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9D112) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T113)
	data->cif_cfg.polarity_pclk 	= POSITIVE_EDGE;
#else
	data->cif_cfg.polarity_pclk 	= NEGATIVE_EDGE;
#endif

#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MV9317) || defined(CONFIG_VIDEO_CAMERA_SENSOR_OV7690) || defined(CONFIG_VIDEO_CAMERA_SENSOR_SIV100B)
	data->cif_cfg.polarity_vsync 	= ACT_LOW;
#else
	data->cif_cfg.polarity_vsync 	= ACT_HIGH;
#endif
	data->cif_cfg.polarity_href 	= ACT_HIGH;
#ifdef CONFIG_ARCH_TCC892X
	data->cif_cfg.polarity_de 		= ACT_LOW;
#endif
	data->cif_cfg.oper_mode 		= OPER_PREVIEW;
	data->cif_cfg.zoom_step			= 0;
	
	#if (1) //20111209 ysseung   test...
	data->cif_cfg.base_buf			= NULL;
	#else
	data->cif_cfg.base_buf			= hardware_data.cif_buf.addr;
	#endif

	data->cif_cfg.pp_num			= 0; // TCC_CAMERA_MAX_BUFNBRS;
#if defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150) || defined(CONFIG_VIDEO_ATV_SENSOR_RDA5888)
	data->cif_cfg.fmt				= M420_EVEN;
#else
	data->cif_cfg.fmt				= M420_ZERO;
#endif

	data->cif_cfg.main_set.target_x = 0;
	data->cif_cfg.main_set.target_y = 0;

	data->cif_cfg.esd_restart 		= OFF;

	data->cif_cfg.cap_status 		= CAPTURE_NONE;
	data->cif_cfg.retry_cnt			= 0;
	
	current_effect_mode = 0;
#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT

#if defined(CONFIG_USE_ISP)
	ISP_Init();
#endif
}

#if defined(CONFIG_USE_ISP)
static irqreturn_t isp_cam_isr0(int irq, void *client_data/*, struct pt_regs *regs*/)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *)&hardware_data;
	struct tccxxx_cif_buffer *curr_buf, *next_buf;

	unsigned int curr_num, next_num;
	volatile PCIF pCIF;

	pCIF = (PCIF)tcc_p2v(HwISPBASE);
	client_data = client_data;
}

//20101126 ysseung   test code.
typedef struct {
	unsigned int *y_addr;
	unsigned int *u_addr;
	unsigned int *v_addr;	
} isp_tmp;
isp_tmp isp[5];
extern void cif_interrupt_disable(void);
static irqreturn_t isp_cam_isr1(int irq, void *client_data/*, struct pt_regs *regs*/)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *)&hardware_data;
#ifdef TCCISP_GEN_CFG_UPD
	struct tccxxx_cif_buffer *curr_buf;
	unsigned int curr_num;
#else
	struct tccxxx_cif_buffer *curr_buf, *next_buf;
	unsigned int curr_num, next_num;
#endif
	volatile PCIF pCIF;

	pCIF = (PCIF)tcc_p2v(HwISPBASE);
	client_data = client_data;

#ifndef TCCISP_GEN_CFG_UPD
	if(ISP_CheckInterrupt()) {
		ISP_ClearInterrupt();
		printk ("isp_cam_isr1: Collision Detected!!!! \n");
		return IRQ_HANDLED;
	}
#endif

	ISP_EventHandler2();
	if(ISP_EventHandler2_getEvent(1)) {
		if(data->stream_state == STREAM_ON) {
			if(skip_frm == 0 && !frame_lock) {
				if(prev_buf != NULL) 	list_move_tail(&prev_buf->buf_list, &data->done_list);
				prev_buf = data->buf + data->cif_cfg.now_frame_num;
				prev_num = data->cif_cfg.now_frame_num;
			#ifndef TCCISP_GEN_CFG_UPD
				next_buf = list_entry(data->list.next->next, struct tccxxx_cif_buffer, buf_list);
				next_num = next_buf->v4lbuf.index;

				if(next_buf != &data->list && prev_buf != next_buf) {
					//exception process!!
			#else // TCCISP_GEN_CFG_UPD
				if(next_buf != &data->list && curr_buf != next_buf && (chkpnt==1)) {
					#ifndef TCCISP_ONE_IRQ
					//exception process!!
					//next_buf = 0; 
					chkpnt = 2;
					#endif // TCCISP_ONE_IRQ
			#endif // TCCISP_GEN_CFG_UPD

				#ifndef TCCISP_ONE_IRQ
					if(prev_num != prev_buf->v4lbuf.index) {
						printk("Frame num mismatch :: true num  :: %d \n", prev_num);
						printk("Frame num mismatch :: false num :: %d \n", prev_buf->v4lbuf.index);
						prev_buf->v4lbuf.index = prev_num ;
					}

					#if (0) //20101122 ysseung   test code.
					{
						//isp_tmp = ioremap(data->cif_cfg.preview_buf[curr_num].p_Cr, 4096);
						*isp[curr_num].y_addr = 0x80808080;
						*(isp[curr_num].y_addr + data->cif_cfg.main_set.target_x/4) = 0x80808080;
						*(isp[curr_num].y_addr + data->cif_cfg.main_set.target_x/4*2) = 0x80808080;
						*(isp[curr_num].y_addr + data->cif_cfg.main_set.target_x/4*3) = 0x80808080;

						*isp[curr_num].u_addr = 0x80808080;
						*(isp[curr_num].u_addr + data->cif_cfg.main_set.target_x/8) = 0x80808080;

						*isp[curr_num].v_addr = 0x80808080;
						*(isp[curr_num].v_addr + data->cif_cfg.main_set.target_x/8) = 0x80808080;
					}
					#endif
					#if (0)
					{
						static unsigned int d_old = 0;
						unsigned int d = 0;
						//volatile unsigned int *temp;

						d = *(volatile unsigned int *)isp[curr_num].y_addr;
						if (d_old)
						{
							if (d != d_old)
							{
								printk ("Check point....(isp) \n");
							}
						}
						d_old = d;
					}
					#endif
					//cif_dma_hw_reg(next_num);
					{	
						struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;
						data->cif_cfg.now_frame_num = next_num;

						#ifndef TCCISP_GEN_CFG_UPD
						ISP_Zoom_Apply (1);
						#if defined(FEATURE_ANDROID_ICS)
						ISP_SetPreviewH_StartAddress((unsigned int)data->cif_cfg.preview_buf[next_num].p_Y, 	\
													 (unsigned int)data->cif_cfg.preview_buf[next_num].p_Cr, 	\
													 (unsigned int)data->cif_cfg.preview_buf[next_num].p_Cb);
						#else // FEATURE_ANDROID_ICS
						ISP_SetPreviewH_StartAddress((unsigned int)data->cif_cfg.preview_buf[next_num].p_Y, 	\
													 (unsigned int)data->cif_cfg.preview_buf[next_num].p_Cb, 	\
													 (unsigned int)data->cif_cfg.preview_buf[next_num].p_Cr);
						#endif // FEATURE_ANDROID_ICS
						#else // TCCISP_GEN_CFG_UPD
							#ifdef TCCISP_UPDATE
							// todo : 
							#endif
						#endif // TCCISP_GEN_CFG_UPD
					}

					#if defined(FEATURE_ANDROID_ICS)
					{
						unsigned int stride = ALIGNED_BUFF(data->cif_cfg.main_set.target_x, 16);
						prev_buf->v4lbuf.bytesused = ALIGNED_BUFF((stride/2), 16) * (data->cif_cfg.main_set.target_y/2);
					}
					#else // FEATURE_ANDROID_ICS
					if(data->cif_cfg.fmt == M420_ZERO)
						prev_buf->v4lbuf.bytesused = data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y*2;
					else
						prev_buf->v4lbuf.bytesused = (data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y*3)/2;
					#endif // FEATURE_ANDROID_ICS

				    //cif_buf->v4lbuf.sequence = cam->buf_seq[bufno];
					prev_buf->v4lbuf.flags &= ~V4L2_BUF_FLAG_QUEUED;
					prev_buf->v4lbuf.flags |= V4L2_BUF_FLAG_DONE;
					//spin_lock_irqsave(&data->dev_lock, flags);
					//cif_buf->v4lbuf.timestamp = gettimeofday(.., ..);

					//list_move_tail(&curr_buf->buf_list, &data->done_list);
				}
				else
				{
					prev_buf = NULL;
					dprintk("no-buf change... wakeup!! \n");
					skipped_frm++;
				}

				data->wakeup_int = 1;
				wake_up_interruptible(&data->frame_wait);
			}
			else
			{
				if(skip_frm > 0)
					skip_frm--;
				else
					skip_frm = 0;
			}
	#endif   // TCCISP_ONE_IRQ	

		}
		else if((data->stream_state == STREAM_OFF) && (data->cif_cfg.oper_mode == OPER_CAPTURE)) // capture mode.
		{
			// turn off Camera Flash
			sensor_turn_off_camera_flash();
			
			ISP_Stop_Irq();
/*
			data->cif_cfg.now_frame_num = 0;
			
			curr_buf = data->buf + data->cif_cfg.now_frame_num;
			
			curr_buf->v4lbuf.bytesused 	= data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y*2;
			curr_buf->v4lbuf.flags 		&= ~V4L2_BUF_FLAG_QUEUED;
			curr_buf->v4lbuf.flags 		&= ~V4L2_BUF_FLAG_DONE;

			list_move_tail(&curr_buf->buf_list, &data->done_list);
*/
			data->cif_cfg.cap_status = CAPTURE_DONE;

			wake_up_interruptible(&data->frame_wait); // POLL
		}
	}

	return IRQ_HANDLED;
}

static irqreturn_t isp_cam_isr2(int irq, void *client_data/*, struct pt_regs *regs*/)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *)&hardware_data;
	struct tccxxx_cif_buffer *curr_buf; //, *next_buf;
	unsigned int curr_num; //, next_num;
	volatile PCIF pCIF;

	pCIF = (PCIF)tcc_p2v(HwISPBASE);
	
	//client_data = client_data;
#ifndef TCCISP_GEN_CFG_UPD	
	if ( ISP_CheckInterrupt() )
	{
		printk ("isp_cam_isr2: Collision Detected!!!!======================================== \n");
	}
#endif
	ISP_EventHandler3();

	ISP_EventHandler3_getEvent(8);	

	if(ISP_EventHandler3_getEvent(0x40))  //DRVISPINT_VSTART
	{
		sensor_if_check_control();
#ifdef TCCISP_GEN_CFG_UPD		
		if(data->stream_state == STREAM_ON)
		{
			if(skip_frm == 0 && !frame_lock)
			{
				curr_buf = data->buf + data->cif_cfg.now_frame_num;
				curr_num = data->cif_cfg.now_frame_num;

				next_buf = list_entry(data->list.next->next, struct tccxxx_cif_buffer, buf_list);	
				next_num = next_buf->v4lbuf.index;

				if(next_buf != &data->list && curr_buf != next_buf)
				{
					#if defined(FEATURE_ANDROID_ICS)
					ISP_SetPreviewH_StartAddress((unsigned int)data->cif_cfg.preview_buf[next_num].p_Y,
												 (unsigned int)data->cif_cfg.preview_buf[next_num].p_Cr,
												 (unsigned int)data->cif_cfg.preview_buf[next_num].p_Cb);
					#else
					ISP_SetPreviewH_StartAddress((unsigned int)data->cif_cfg.preview_buf[next_num].p_Y,
												 (unsigned int)data->cif_cfg.preview_buf[next_num].p_Cb,
												 (unsigned int)data->cif_cfg.preview_buf[next_num].p_Cr);
					#endif
				}
				chkpnt =1;
			}
		}
#endif
	}
	
	return IRQ_HANDLED;
}

static irqreturn_t isp_cam_isr3(int irq, void *client_data/*, struct pt_regs *regs*/)
{
}
#endif

#ifndef CONFIG_ARCH_TCC892X
static irqreturn_t cif_cam_isr(int irq, void *client_data/*, struct pt_regs *regs*/)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *)&hardware_data;
	struct tccxxx_cif_buffer *curr_buf, *next_buf;
	unsigned int curr_num, next_num;
#if defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150) || defined(CONFIG_VIDEO_ATV_SENSOR_RDA5888)
	static int	isr_cnt = 0;
#endif
	
#ifdef JPEG_ENCODE_WITH_CAPTURE
	unsigned int uCLineCnt, uTempVCNT;
#endif

#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIF pCIF;

	pCIF = (PCIF)tcc_p2v(HwCIF_BASE);
#endif

	client_data = client_data;

#if defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150)
	if (isr_cnt >= 2)
		BITSET(pCIF->ICPCR1, 1<<26);
	isr_cnt++;	
#endif

#ifdef JPEG_ENCODE_WITH_CAPTURE
	if (data->cif_cfg.oper_mode == OPER_CAPTURE) 
	{
		// turn off Camera Flash
		sensor_turn_off_camera_flash();
		
		if(TDD_CIF_GetIntStatus(GET_CIF_INT_VCNT_GEN))
		{
			if (gIsRolling)
			{
				uCLineCnt = pCIF->HwCCLC;
				
				BITSET(pCIF->HwCIRQ, HwCIRQ_VIT);
				
				uTempVCNT = pCIF->HwCCLC;
				while(uTempVCNT%FRAME_LINE_CNT == 0 && uTempVCNT != data->cif_cfg.main_set.target_y)
					uTempVCNT = pCIF->HwCCLC;

				BITSET(pCIF->HwCIRQ, HwCIRQ_VIT);

				if(gJpegCodecMode != JPEG_ENCODE_MODE)
				{
					gJpegCodecMode = JPEG_ENCODE_MODE;
					BITCSET(HwJPEGENC->JP_START, 0x0000000F, HwJP_START_RUN);
					BITCSET(HwJPEGENC->HwJP_SBUF_WCNT, 0x0000FFFF, uCLineCnt);
				}
				else if(((uCLineCnt%FRAME_LINE_CNT==0)||(uCLineCnt >= data->cif_cfg.main_set.target_y - FRAME_LINE_CNT)))
				{
					if(uCLineCnt >= (data->cif_cfg.main_set.target_y - FRAME_LINE_CNT))
					{
						BITCSET(HwJPEGENC->HwJP_SBUF_WCNT, 0x0000FFFF, data->cif_cfg.main_set.target_y);
					}
					else
						BITCSET(HwJPEGENC->HwJP_SBUF_WCNT, 0x0000FFFF, uCLineCnt);
				}
			}

			return IRQ_HANDLED;
		}
	
		if(TDD_CIF_GetIntStatus(GET_CIF_INT_ENC_STRT))
		{
			BITSET(pCIF->HwCIRQ, HwCIRQ_ENS);
		
			gJpegCodecMode = JPEG_ENCODE_MODE;
			BITCSET(HwJPEGENC->JP_START, 0x0000000F, HwJP_START_RUN);
			return IRQ_HANDLED;
		}
	}
#else
	if(TDD_CIF_GetIntStatus(GET_CIF_INT_ENC_STRT))
	{
		data->cif_cfg.now_frame_num = 0;
		curr_buf = data->buf + data->cif_cfg.now_frame_num;		
		
		curr_buf->v4lbuf.bytesused =	data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y*2;
		curr_buf->v4lbuf.flags &= ~V4L2_BUF_FLAG_QUEUED;
		curr_buf->v4lbuf.flags |= V4L2_BUF_FLAG_DONE;

		list_move_tail(&curr_buf->buf_list, &data->done_list);		

		data->cif_cfg.cap_status = CAPTURE_DONE;

		wake_up_interruptible(&data->frame_wait); //POLL
		
	#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
		BITSET(pCIF->CIRQ, HwCIRQ_ENS);
		BITSET(pCIF->CIRQ, HwCIRQ_ENS);
	#endif

		return IRQ_HANDLED;
	}
#endif

	//preview : Stored-One-Frame in DMA
	if(TDD_CIF_GetIntStatus(GET_CIF_INT_ONEFRAME_STORE))
	{
		if(data->stream_state == STREAM_ON)
		{
			#if !defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150)
			TDD_CIF_SetInterrupt(SET_CIF_UPDATE_WITHOUT_VSYNC);
			sensor_if_check_control();
			#endif
			if(skip_frm == 0 && !frame_lock)
			{				
				//dprintk("[Camera Preview] Interrupt Rising Up!!\n");
				curr_buf = data->buf + data->cif_cfg.now_frame_num;
				curr_num = data->cif_cfg.now_frame_num;
	
				next_buf = list_entry(data->list.next->next, struct tccxxx_cif_buffer, buf_list);	
				next_num = next_buf->v4lbuf.index;

				if(next_buf != &data->list && curr_buf != next_buf)
				{
					//exception process!!
					if(curr_num != curr_buf->v4lbuf.index)
					{
						printk("Frame num mismatch :: true num  :: %d \n", curr_num);
						printk("Frame num mismatch :: false num :: %d \n", curr_buf->v4lbuf.index);
						curr_buf->v4lbuf.index = curr_num ;
					}
		
					cif_dma_hw_reg(next_num);

					#if defined(FEATURE_ANDROID_ICS)
					{
						unsigned int stride = ALIGNED_BUFF(data->cif_cfg.main_set.target_x, 16);
						curr_buf->v4lbuf.bytesused = ALIGNED_BUFF((stride/2), 16) * (data->cif_cfg.main_set.target_y/2);
					}
					#else // FEATURE_ANDROID_ICS
					if(data->cif_cfg.fmt == M420_ZERO)
						curr_buf->v4lbuf.bytesused = data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y*2;
					else
						curr_buf->v4lbuf.bytesused = (data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y*3)/2;
					#endif // FEATURE_ANDROID_ICS

					//cif_buf->v4lbuf.sequence = cam->buf_seq[bufno];
					curr_buf->v4lbuf.flags &= ~V4L2_BUF_FLAG_QUEUED;
					curr_buf->v4lbuf.flags |= V4L2_BUF_FLAG_DONE;
					//spin_lock_irqsave(&data->dev_lock, flags);
					//cif_buf->v4lbuf.timestamp = gettimeofday(.., ..);

					list_move_tail(&curr_buf->buf_list, &data->done_list);					
				}
				else
				{
					dprintk("no-buf change... wakeup!! \n");
					skipped_frm++;
				}

				data->wakeup_int = 1;
				wake_up_interruptible(&data->frame_wait);
			}
			else
			{
				if(skip_frm > 0)
					skip_frm--;
				else
					skip_frm = 0;
			}
			
			#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
			BITSET(pCIF->CIRQ, HwCIRQ_SOF);
			BITSET(pCIF->CIRQ, HwCIRQ_MSOF);
			BITCLR(pCIF->CIRQ, HwCIRQ_MSOF);
			#elif	defined(CONFIG_ARCH_TCC892X)
				//	In case of 892X, we have to add.
				
			#else // (CONFIG_ARCH_TCC79X)
			BITSET(HwCIRQ, HwCIRQ_SOF);
			BITSET(HwCIRQ, HwCIRQ_MSOF);
			BITCLR(HwCIRQ, HwCIRQ_MSOF);
			#endif

			#if defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150)
			// Do nothing.
			#else
			TDD_CIF_SetInterrupt(SET_CIF_UPDATE_IN_VSYNC);
			#endif
		}		
	}

	return IRQ_HANDLED;
}
#endif

#ifdef CONFIG_ARCH_TCC892X
static irqreturn_t cif_cam_isr_in8920(int irq, void *client_data /*, struct pt_regs *regs*/)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *)&hardware_data;
	struct tccxxx_cif_buffer *curr_buf, *next_buf;
	unsigned int curr_num, next_num, nCnt;
#if defined(JPEG_ENCODE_WITH_CAPTURE)
	unsigned int uCLineCnt, uTempVCNT;
#endif
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIF pCIF;
	pCIF = (PCIF)tcc_p2v(HwCIF_BASE);
#endif

	if(data->cif_cfg.oper_mode == OPER_PREVIEW) { // preview operation.
		if(pWDMABase->uIRQSTS.nREG & VIOC_WDMA_IREQ_EOFF_MASK) {
			sensor_if_check_control();

			if(skip_frm == 0 && !frame_lock) {
				if(prev_buf != NULL) list_move_tail(&prev_buf->buf_list, &data->done_list);

				prev_buf = data->buf + data->cif_cfg.now_frame_num;
				prev_num = data->cif_cfg.now_frame_num;

				next_buf = list_entry(data->list.next->next, struct tccxxx_cif_buffer, buf_list);
				next_num = next_buf->v4lbuf.index;

				if(next_buf != &data->list && prev_buf != next_buf) { // exception process!!
					if(prev_num != prev_buf->v4lbuf.index) {
						printk("Frame num mismatch :: true num	:: %d \n", prev_num);
						printk("Frame num mismatch :: false num :: %d \n", prev_buf->v4lbuf.index);
						prev_buf->v4lbuf.index = prev_num ;
					}

					cif_dma_hw_reg(next_num);

				#if defined(FEATURE_ANDROID_ICS)
					{
						unsigned int stride = ALIGNED_BUFF(data->cif_cfg.main_set.target_x, 16);
						prev_buf->v4lbuf.bytesused = ALIGNED_BUFF((stride/2), 16) * (data->cif_cfg.main_set.target_y / 2);
					}
				#else // FEATURE_ANDROID_ICS
					if(data->cif_cfg.fmt == M420_ZERO)
						prev_buf->v4lbuf.bytesused = data->cif_cfg.main_set.target_x * data->cif_cfg.main_set.target_y * 2;
					else
						prev_buf->v4lbuf.bytesused = data->cif_cfg.main_set.target_x * data->cif_cfg.main_set.target_y * 3 / 2;
				#endif // FEATURE_ANDROID_ICS

					prev_buf->v4lbuf.flags &= ~V4L2_BUF_FLAG_QUEUED;
					prev_buf->v4lbuf.flags |= V4L2_BUF_FLAG_DONE;

				#if defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150)
					if((frm_cnt == 1) && (bfield == 0)) {
						dprintk("Deintl Initialization\n");
						/**********  VERY IMPORTANT                  *************************************/
						/* if we want to operate temporal deinterlacer mode, initial 3 field are operated by spatial,
						 	then change the temporal mode in next fields. */

						// VIOC_VIQE_InitDeintlCoreNormal((VIQE *)pVIQEBase);
						//VIOC_VIQE_InitDeintlCoreTemporal((VIQE *)pVIQEBase);
						VIOC_VIQE_SetDeintlMode((VIQE *)pVIQEBase, 2);
					} else {
						field_cnt++;
					}

					if(bfield == 1) { // end fied of bottom field
						bfield = 0;
						frm_cnt++;
					} else {
						bfield = 1;
					}
				#endif // CONFIG_VIDEO_ATV_SENSOR_TVP5150
				}
				else {
					prev_buf = NULL;
					dprintk("no-buf change... wakeup!! \n");
					skipped_frm++;
				}

				data->wakeup_int = 1;
				wake_up_interruptible(&data->frame_wait);
			}
			else {
				if(skip_frm > 0) {
					skip_frm--;
					printk("decrease frm!!\n");
				} else {
					skip_frm = 0;
				}
			}

			BITCSET(pWDMABase->uCTRL.nREG, 1<<16, (1<<16));
			//BITCSET(pWDMABase->uIRQSTS.nREG, 1<<6, 1<<6); // clear EOFF
			BITCSET(pWDMABase->uIRQSTS.nREG, VIOC_WDMA_IREQ_ALL_MASK, VIOC_WDMA_IREQ_ALL_MASK);
			VIOC_WDMA_SetIreqMask(pWDMABase, VIOC_WDMA_IREQ_EOFF_MASK, 0x0);
		}

		return IRQ_HANDLED;
	}

	if(data->cif_cfg.oper_mode == OPER_CAPTURE) { // capture operation.
		if(pWDMABase->uIRQSTS.nREG & VIOC_WDMA_IREQ_EOFR_MASK) {
			if(skip_frm == 0 && !frame_lock) {
				VIOC_WDMA_SetIreqMask(pWDMABase, VIOC_WDMA_IREQ_ALL_MASK, 0x1);

				//BITCSET(pWDMABase->uCTRL.nREG, 1<<28, 0<<28); // disable WDMA
				//BITCSET(pWDMABase->uCTRL.nREG, 1<<16, 1<<16); // update WDMA
				//BITCSET(pWDMABase->uIRQSTS.nREG, 1<<5, 1<<5); // clear EORF bit

				VIOC_VIN_SetEnable(pVINBase, OFF); // disable VIN

				data->cif_cfg.now_frame_num = 0;
				curr_buf = data->buf + data->cif_cfg.now_frame_num;

				curr_buf->v4lbuf.bytesused = data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y*2;
				curr_buf->v4lbuf.flags &= ~V4L2_BUF_FLAG_QUEUED;
				curr_buf->v4lbuf.flags |= V4L2_BUF_FLAG_DONE;

				list_move_tail(&curr_buf->buf_list, &data->done_list);
				data->cif_cfg.cap_status = CAPTURE_DONE;
				wake_up_interruptible(&data->frame_wait); //POLL

				BITCSET(pWDMABase->uIRQSTS.nREG, VIOC_WDMA_IREQ_ALL_MASK, VIOC_WDMA_IREQ_ALL_MASK);
			}
			else {
				if(skip_frm > 0) {
					printk("capture frame skip. skip count is %d. \n", skip_frm);
					skip_frm--;
				} else {
					skip_frm = 0;
				}

				VIOC_WDMA_SetImageEnable(pWDMABase, OFF);
				BITCSET(pWDMABase->uIRQSTS.nREG, 1<<5, 1<<5); // clear EOFR
				VIOC_WDMA_SetIreqMask(pWDMABase, VIOC_WDMA_IREQ_EOFR_MASK, 0x0);
			}
		}

		return IRQ_HANDLED;
	}
}
#endif

/* ------------- below are interface functions ----------------- */
/* ------------- these functions are named tccxxxcif_<name> -- */

/* Enables the camera. Takes camera out of reset. Enables the clocks. */ 
static int cif_enable(void *priv)
{
#if 0	
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) priv;

	/* give clock to camera_module */
	data->camera_regs->mode = (FIFO_TRIGGER_LVL << THRESHOLD_BIT);
	data->camera_regs->ctrlclock = MCLK_EN | CAMEXCLK_EN;1394

	omap16xx_cam_clear_fifo(data);
#endif
	/* wait for camera to settle down */
	mdelay(5);

	return 0;
}
 
/* Disables all the camera clocks. Put the camera interface in reset. */
static int cif_disable(void)
{ 	
	sensor_if_cleanup();	
	return 0;
}

static int cif_cleanup(void)
{  
	cif_disable();
	return 0;
}

static void cif_check_handler(unsigned long arg)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) arg;
	unsigned long timeover = 0;
		
	if(data->cif_cfg.oper_mode == OPER_PREVIEW) {
		if(sensor_if_isESD()) { // Check Sensor-ESD register!!
			data->cif_cfg.esd_restart = ON;
			timeover = 0;
		} else {
			timeover = HZ;
		}

		cif_timer_register(data, timeover);
	}
}

void cif_timer_register(void* priv, unsigned long timeover)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) priv;

	init_timer(&data->cam_timer);
	data->cam_timer.function = cif_check_handler;
	data->cam_timer.data = (unsigned long)data;
	data->cam_timer.expires = get_jiffies_64()+ timeover;
	add_timer(&data->cam_timer);

	register_timer = 1;
}

void cif_timer_deregister(void)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;

	del_timer(&data->cam_timer);
	data->cam_timer.expires = 0;

	register_timer = 0;
}


void cif_scaler_set(void *priv, enum cifoper_mode mode)
{
	cif_main_set *data = (cif_main_set *) priv;
	unsigned int sc_hfactor, sc_vfactor;
	unsigned int crop_hori_start, crop_hori_end, crop_vert_start, crop_vert_end;
	unsigned int h_ratio, v_ratio, temp_x;	
#if defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150) || defined(CONFIG_VIDEO_ATV_SENSOR_RDA5888)
	unsigned int crop_hor_ofst, crop_ver_ofst;

	data->scaler_x = data->target_x;
	data->scaler_y = data->target_y;
#else
#if defined(CONFIG_VIDEO_CAMERA_SENSOR_AIT848_ISP)
	if(mode == OPER_CAPTURE)
	{
		data->source_x = data->scaler_x = 1536; // 1535fixed Value, the pClks of One HSync
		data->source_y = data->scaler_y = (gnSizeOfAITALLJPEG+1535)/1536;

		crop_hori_start = crop_hori_end = crop_vert_start = crop_vert_end = 0;
	}
	else
#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_ISX006)
	if(mode == OPER_CAPTURE)
	{
		data->source_x = data->scaler_x = CAP_W;
		data->source_y = data->scaler_y = CAP_H;

		crop_hori_start = crop_hori_end = crop_vert_start = crop_vert_end = 0;
	}
	else
#endif
	{
		if(((data->target_x * data->source_y) / data->source_x) == data->target_y)
		{
			data->scaler_x = data->target_x;
			data->scaler_y = data->target_y;
		}
		else
		{		
			h_ratio = (data->source_x * 100 / data->target_x);
			v_ratio = (data->source_y * 100 / data->target_y);
				
			if(h_ratio > v_ratio)
			{	
				data->scaler_y = data->target_y;
				data->scaler_x = (data->source_x * data->target_y)/data->source_y;
		
				temp_x = ((data->scaler_x+15) >> 4) << 4;
		
				if((temp_x - data->scaler_x) > 8)
					data->scaler_x = (data->scaler_x >> 4) << 4;
				else				
					data->scaler_x = temp_x;
			}
			else
			{
				data->scaler_x = data->target_x;
				data->scaler_y = (data->source_y * data->target_x)/data->source_x;
				
				data->scaler_y = (data->scaler_y >> 1) << 1;
			}
		}
		
		crop_hori_start = (data->scaler_x - data->target_x)/2;
		crop_hori_end 	= (data->scaler_x - data->target_x - crop_hori_start);
		crop_vert_start = (data->scaler_y - data->target_y)/2;
		crop_vert_end 	= (data->scaler_y - data->target_y - crop_vert_start);			
	}
#endif /*CONFIG_VIDEO_CAMERA_SENSOR_TVP5150*/

	#if defined(CONFIG_ARCH_TCC892X)


		
	#else
			sc_hfactor = (data->source_x*256/data->scaler_x);
			sc_vfactor = (data->source_y*256/data->scaler_y);

			TDD_CIF_SetFreeScale(SET_CIF_SCALER_SRC_SIZE|SET_CIF_SCALER_DST_SIZE,
									data->source_x, data->source_y, 0, 0, data->scaler_x, data->scaler_y, 0, 0);
			TDD_CIF_SetFreeScale(SET_CIF_SCALER_SRC_OFFSET,
									0, 0, data->win_hor_ofst, data->win_ver_ofst, 0, 0, 0, 0);
			TDD_CIF_SetFreeScale(SET_CIF_SCALER_FACTOR, 0, 0, 0, 0, 0, 0, sc_hfactor, sc_vfactor);
		#if defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150) || defined(CONFIG_VIDEO_ATV_SENSOR_RDA5888)
			crop_hor_ofst = (data->scaler_x - data->target_x)/2;
			crop_ver_ofst = (data->scaler_y - data->target_y)/2;
			//TDD_CIF_SetImage(INPUT_IMG, data->scaler_x, data->scaler_y, 
			TDD_CIF_SetImage(INPUT_IMG, data->source_x, data->source_y,
										crop_hor_ofst, (data->scaler_x - data->target_x - crop_hor_ofst), 
										crop_ver_ofst, (data->scaler_y - data->target_y - crop_ver_ofst));
			TDD_CIF_SetBaseAddr_offset(INPUT_IMG, data->scaler_x, data->scaler_x/2);
		#else
			//Crop
			TDD_CIF_SetImage(INPUT_IMG, data->scaler_x, data->scaler_y, crop_hori_start, crop_hori_end, crop_vert_start, crop_vert_end);

			#if defined(CONFIG_ARCH_TCC88XX)
				TDD_CIF_SetBaseAddr_offset(INPUT_IMG, data->target_x, data->target_x/2);
			#else
				TDD_CIF_SetBaseAddr_offset(INPUT_IMG, data->scaler_x, data->scaler_x/2);
			#endif
			
			dprintk("Crop Information -> crop_hori_start : %d, crop_hori_end : %d, crop_vert_start : %d, crop_vert_end : %d \n", crop_hori_start, crop_hori_end, crop_vert_start, crop_vert_end);
		#endif

		#if defined(CONFIG_VIDEO_CAMERA_SENSOR_AIT848_ISP) || defined(CONFIG_VIDEO_CAMERA_SENSOR_ISX006) || defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150) || defined(CONFIG_VIDEO_ATV_SENSOR_RDA5888)
			if(mode == OPER_CAPTURE)
			{
				TDD_CIF_SetScalerCtrl(SET_CIF_SCALER_DISABLE);
			}
			else
		#endif
			{
				TDD_CIF_SetScalerCtrl(SET_CIF_SCALER_ENABLE);
			}
	#endif
}

void cif_scaler_calc(void)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;
	enum cifoper_mode mode = data->cif_cfg.oper_mode;
	unsigned int off_x = 0, off_y = 0, width = 0, height = 0;

#if defined(CONFIG_ARCH_TCC892X) //20120425 ysseung   // Before camera quit, we have to wait WMDA's EOFR signal to LOW.
	VIOC_WDMA_SetIreqMask(pWDMABase, VIOC_WDMA_IREQ_ALL_MASK, 0x1); // disable WDMA interrupt

	BITCSET(pWDMABase->uCTRL.nREG, 1<<28, 0<<28); // disable WDMA
	BITCSET(pWDMABase->uCTRL.nREG, 1<<16, 1<<16); // update WDMA
	BITCSET(pWDMABase->uIRQSTS.nREG, 1<<5, 1<<5); // clear EORF bit

	mutex_lock(&data->lock);
	while(!(pWDMABase->uIRQSTS.nREG & VIOC_WDMA_IREQ_EOFR_MASK)) {
		msleep(1);
		if(cam_loop_lmt_cnt > CAMERA_LOOP_LIMIT_COUNT) {
			VIOC_WDMA_SWReset(pVIOCConfig, 0x5/* WDMA05 */);
			VIOC_WDMA_SetImageFormat(pWDMABase, VIOC_IMG_FMT_YUV420SEP);
			VIOC_WDMA_SetImageSize(pWDMABase, data->cif_cfg.main_set.target_x, data->cif_cfg.main_set.target_y);
			VIOC_WDMA_SetImageOffset(pWDMABase, VIOC_IMG_FMT_YUV420SEP, data->cif_cfg.main_set.target_x);
			printk("cam_loop_lmt_cnt = %d. \n", cam_loop_lmt_cnt);
			cam_loop_lmt_cnt = 0;
			break;
		}
	}
	mutex_unlock(&data->lock);

	VIOC_VIN_SetEnable(pVINBase, OFF);
#endif // CONFIG_ARCH_TCC892X

#if defined(CONFIG_VIDEO_CAMERA_SENSOR_AIT848_ISP)
	unsigned int nCntOfHsync;

	if(mode == OPER_CAPTURE)
	{
		nCntOfHsync = (gnSizeOfAITALLJPEG+1535)/1536;
		width = 1536; // 1535fixed Value, the pClks of One HSync
		height = nCntOfHsync;
		
		data->cif_cfg.zoom_step = 0;
	}
	else
	{
		off_x  = PRV_ZOFFX;
		off_y  = PRV_ZOFFY;
		width  = PRV_W;
		height = PRV_H;
	}
#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_ISX006)
	if(mode == OPER_CAPTURE)
	{
		width  = CAP_W; 
		height = CAP_H;
		
		data->cif_cfg.zoom_step = 0;
	}
	else
	{
		off_x  = PRV_ZOFFX;
		off_y  = PRV_ZOFFY;
		width  = PRV_W;
		height = PRV_H;
	}
#else
#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	if(mode == OPER_PREVIEW
		|| (mode == OPER_CAPTURE && data->cif_cfg.main_set.target_x < tcc_sensor_info.cam_capchg_width))
	{
		off_x  = tcc_sensor_info.preview_zoom_offset_x;
		off_y  = tcc_sensor_info.preview_zoom_offset_y;
		width  = tcc_sensor_info.preview_w;
		height = tcc_sensor_info.preview_h;
	}
	else
	{
		off_x  = tcc_sensor_info.capture_zoom_offset_x;
		off_y  = tcc_sensor_info.capture_zoom_offset_y;
		width  = tcc_sensor_info.capture_w;
		height = tcc_sensor_info.capture_h;
	}
#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	if(mode == OPER_PREVIEW
		|| (mode == OPER_CAPTURE && data->cif_cfg.main_set.target_x < CAM_CAPCHG_WIDTH) )
	{
		off_x  = PRV_ZOFFX;
		off_y  = PRV_ZOFFY;
		width  = PRV_W;
		height = PRV_H;
	}
	else
	{
		off_x  = CAP_ZOFFX;
		off_y  = CAP_ZOFFY;
		width  = CAP_W;
		height = CAP_H;
	}
#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
#endif

	data->cif_cfg.main_set.win_hor_ofst = off_x * data->cif_cfg.zoom_step;	
	data->cif_cfg.main_set.win_ver_ofst = off_y * data->cif_cfg.zoom_step;	
	data->cif_cfg.main_set.source_x 	= width - (off_x * data->cif_cfg.zoom_step)*2;
	data->cif_cfg.main_set.source_y 	= height - (off_y * data->cif_cfg.zoom_step)*2;

#if defined(CONFIG_USE_ISP)
	if(mode == OPER_PREVIEW)
	{
		ISP_SetPreview_Window(data->cif_cfg.main_set.win_hor_ofst, data->cif_cfg.main_set.win_ver_ofst,
								data->cif_cfg.main_set.source_x, data->cif_cfg.main_set.source_y );
	}
	else
	{
		ISP_SetCapture_Window(data->cif_cfg.main_set.win_hor_ofst, data->cif_cfg.main_set.win_ver_ofst,
								data->cif_cfg.main_set.source_x, data->cif_cfg.main_set.source_y );
	}
#else
	#if defined(CONFIG_ARCH_TCC892X)
	VIOC_VIN_SetImageSize(pVINBase, data->cif_cfg.main_set.source_x, data->cif_cfg.main_set.source_y);
	VIOC_VIN_SetImageOffset(pVINBase, data->cif_cfg.main_set.win_hor_ofst, data->cif_cfg.main_set.win_ver_ofst, 0);
	VIOC_VIN_SetEnable(pVINBase, ON);
	dprintk("VIN WxH[%dx%d] Crop Start XxY[%dx%d] \n", data->cif_cfg.main_set.source_x, data->cif_cfg.main_set.source_y, \
														data->cif_cfg.main_set.win_hor_ofst, data->cif_cfg.main_set.win_ver_ofst);
	VIOC_SC_SetUpdate(pSCBase);
	VIOC_WDMA_SetImageEnable(pWDMABase, ON);

	BITCSET(pWDMABase->uIRQSTS.nREG, 1<<6, 1<<6); // clear EOFF bit
	VIOC_WDMA_SetIreqMask(pWDMABase, VIOC_WDMA_IREQ_EOFF_MASK, 0x0);
	#else // CONFIG_ARCH_TCC892X
	TDD_CIF_SetSensorOutImgSize(width, height);
	cif_scaler_set(&data->cif_cfg.main_set, mode);
	#endif // CONFIG_ARCH_TCC892X
#endif	
}

void cif_dma_hw_reg(unsigned char frame_num)
{	
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;

	data->cif_cfg.now_frame_num = frame_num;

#if defined(CONFIG_USE_ISP)
	ISP_SetPreviewH_StartAddress((unsigned int)data->cif_cfg.preview_buf[frame_num].p_Y,
									(unsigned int)data->cif_cfg.preview_buf[frame_num].p_Cr,
									(unsigned int)data->cif_cfg.preview_buf[frame_num].p_Cb);
#elif	defined(CONFIG_ARCH_TCC892X)
	// For Making the format android(ICS) wants, we had to change between Cb and Cr.
	VIOC_WDMA_SetImageBase(pWDMABase, (unsigned int)data->cif_cfg.preview_buf[frame_num].p_Y, 
									(unsigned int)data->cif_cfg.preview_buf[frame_num].p_Cr,
									(unsigned int)data->cif_cfg.preview_buf[frame_num].p_Cb);	
#else
	TDD_CIF_SetBaseAddr(INPUT_IMG, (unsigned int)data->cif_cfg.preview_buf[frame_num].p_Y,
									(unsigned int)data->cif_cfg.preview_buf[frame_num].p_Cr,
									(unsigned int)data->cif_cfg.preview_buf[frame_num].p_Cb);
	#if defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150)
	//TDD_CIF_SetBaseAddr_offset(INPUT_IMG, data->cif_cfg.main_set.target_x, data->cif_cfg.main_set.target_x/2);
	#endif
#endif //CONFIG_USE_ISP
}

void cif_preview_dma_set(void)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;
	unsigned char i;
	dma_addr_t base_addr = data->cif_cfg.base_buf;
	unsigned int y_offset;
	unsigned int uv_offset = 0;
	unsigned int total_off = 0;
	#if defined(FEATURE_ANDROID_ICS)
	unsigned int stride;
	#endif

	data->cif_cfg.now_frame_num = 0;

	#if defined(FEATURE_ANDROID_ICS)
	stride 		= ALIGNED_BUFF(data->cif_cfg.main_set.target_x, 16);
	y_offset 	= stride * data->cif_cfg.main_set.target_y;
	uv_offset 	= ALIGNED_BUFF((stride/2), 16) * (data->cif_cfg.main_set.target_y/2);
	#else // FEATURE_ANDROID_ICS
	y_offset = data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y;
	if(data->cif_cfg.fmt == M420_ZERO)
		uv_offset = (data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y)/2;
	else
		uv_offset = (data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y)/4;
	#endif // FEATURE_ANDROID_ICS

	dprintk("RDA5888E Interrupt, Width=%d, Height=%d. \n", data->cif_cfg.main_set.target_x, data->cif_cfg.main_set.target_y);

	total_off = y_offset + uv_offset * 2;
  	total_off = PAGE_ALIGN(total_off);
	for(i=0; i < data->cif_cfg.pp_num; i++) {
		data->cif_cfg.preview_buf[i].p_Y  = (unsigned int)PAGE_ALIGN( base_addr + total_off*i);
		data->cif_cfg.preview_buf[i].p_Cb = (unsigned int)data->cif_cfg.preview_buf[i].p_Y + y_offset;
		data->cif_cfg.preview_buf[i].p_Cr = (unsigned int)data->cif_cfg.preview_buf[i].p_Cb + uv_offset;
	}
}

void cif_capture_dma_set(void *priv)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;
	dma_addr_t base_addr;
	unsigned int y_offset;
	unsigned int uv_offset = 0;
	unsigned int target_width, target_height;

#if defined(CONFIG_VIDEO_CAMERA_SENSOR_AIT848_ISP)
	base_addr = PA_JPEG_RAW_BASE_ADDR;
	target_width  = 1536;
	target_height = (gnSizeOfAITALLJPEG+1535)/1536;
#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_ISX006)
	base_addr = PA_PREVIEW_BASE_ADDR;
	target_width  = CAP_W;
	target_height = CAP_H;
#else
	base_addr = data->cif_cfg.base_buf;
	target_width  = data->cif_cfg.main_set.target_x;
	target_height = data->cif_cfg.main_set.target_y;
#endif

#if defined(JPEG_ENCODE_WITH_CAPTURE)
	if(gIsRolling) {
		y_offset = JPEG_Info.JpegCaptureBuffSize/2;
		if(data->cif_cfg.fmt == M420_ZERO)
		    uv_offset = JPEG_Info.JpegCaptureBuffSize/4;
		else
		    uv_offset = JPEG_Info.JpegCaptureBuffSize/4/2;
	}
	else
#endif
	{
		y_offset = target_width*target_height;
		if(data->cif_cfg.fmt == M420_ZERO)
		    uv_offset = (target_width * target_height) / 2;
		else
		    uv_offset = (target_width * target_height) / 4;
	} 

	#if (0) //20111212 ysseung   test...
	data->cif_cfg.capture_buf.p_Y  = (unsigned int)base_addr;
	data->cif_cfg.capture_buf.p_Cb = (unsigned int)data->cif_cfg.capture_buf.p_Y + y_offset;
	data->cif_cfg.capture_buf.p_Cr = (unsigned int)data->cif_cfg.capture_buf.p_Cb + uv_offset;
	#endif

#if defined(CONFIG_USE_ISP)
	//ISP_SetCapture_Window(data->cif_cfg.main_set.win_hor_ofst*2, data->cif_cfg.main_set.win_ver_ofst*2, 
	//						data->cif_cfg.main_set.source_x*2, data->cif_cfg.main_set.source_y*2);	// for capture
	//sISP_SetCapture_Window(0, 0, target_width, target_height);
	//						data->cif_cfg.main_set.source_x*2, data->cif_cfg.main_set.source_y*2);
	printk("cif_capture_dma_set:  yAddr=0x%08x. \n", (unsigned int)data->cif_cfg.capture_buf.p_Y);
	ISP_SetCapture_MainImageStartAddress((unsigned int)data->cif_cfg.capture_buf.p_Y,
									(unsigned int)data->cif_cfg.capture_buf.p_Cb,
									(unsigned int)data->cif_cfg.capture_buf.p_Cr);
	ISP_SetCapture_MainResolution(target_width, target_height);

	if(data->cif_cfg.fmt == M420_ZERO) {
		ISP_SetCapture_MainImageSize(target_width*target_height, target_width*target_height/2, target_width*target_height/2);
		ISP_SetCapture_MainFormat(ISP_FORMAT_YUV422);
	} else {
		ISP_SetCapture_MainImageSize(target_width*target_height, target_width*target_height/4, target_width*target_height/4);
		ISP_SetCapture_MainFormat(ISP_FORMAT_YUV420);
	}

	#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	if(data->cif_cfg.main_set.target_x < tcc_sensor_info.cam_capchg_width) {
		ISP_SetCapture_MakeZoomTable(tcc_sensor_info.preview_zoom_offset_x, tcc_sensor_info.preview_zoom_offset_y, tcc_sensor_info.preview_w, tcc_sensor_info.preview_h, tcc_sensor_info.max_zoom_step+1);
	} else {
		ISP_SetCapture_MakeZoomTable(tcc_sensor_info.capture_zoom_offset_x, tcc_sensor_info.capture_zoom_offset_y, tcc_sensor_info.capture_w, tcc_sensor_info.capture_h, tcc_sensor_info.max_zoom_step+1);
	}
	#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	if(data->cif_cfg.main_set.target_x < CAM_CAPCHG_WIDTH) {
		ISP_SetCapture_MakeZoomTable(PRV_ZOFFX, PRV_ZOFFY, PRV_W, PRV_H, CAM_MAX_ZOOM_STEP);
	} else {
		ISP_SetCapture_MakeZoomTable(CAP_ZOFFX, CAP_ZOFFY, CAP_W, CAP_H, CAM_MAX_ZOOM_STEP);
	}
	#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT 

	ISP_SetCapture_Zoom(data->cif_cfg.zoom_step);	
#else
	#if	defined(CONFIG_ARCH_TCC892X)
		VIOC_WDMA_SetImageBase(pWDMABase, (unsigned int)data->cif_cfg.capture_buf.p_Y, \
										  (unsigned int)data->cif_cfg.capture_buf.p_Cb, \
										  (unsigned int)data->cif_cfg.capture_buf.p_Cr);
#else // CONFIG_USE_ISP
	TDD_CIF_SetBaseAddr(INPUT_IMG,  (unsigned int)data->cif_cfg.capture_buf.p_Y,
									(unsigned int)data->cif_cfg.capture_buf.p_Cb,
									(unsigned int)data->cif_cfg.capture_buf.p_Cr);

	#if defined(CONFIG_VIDEO_CAMERA_SENSOR_AIT848_ISP) || defined(CONFIG_VIDEO_CAMERA_SENSOR_ISX006)
	TDD_CIF_SetBaseAddr_offset(INPUT_IMG, target_width*2, target_width/2);	
	TDD_CIF_SetEffectMode(SET_CIF_CEM_YCS);
	#elif defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150) || defined(CONFIG_VIDEO_ATV_SENSOR_RDA5888)
	TDD_CIF_SetBaseAddr_offset(INPUT_IMG, target_width, target_width/2);
	#else
	TDD_CIF_SetBaseAddr_offset(INPUT_IMG, target_width, target_width/2);
	#endif

	#if defined(CONFIG_VIDEO_CAMERA_SENSOR_OV7690)
	TDD_CIF_SetEffectMode(SET_CIF_CEM_YCS);
	#endif

	#ifdef JPEG_ENCODE_WITH_CAPTURE
	if(gIsRolling)
	{
		if(data->cif_cfg.fmt == M420_ZERO)
		{
			TDD_CIF_SetBaseAddr(IN_IMG_ROLLING,
										((unsigned int)data->cif_cfg.capture_buf.p_Y+(JPEG_Info.JpegCaptureBuffSize/2)-4),
										((unsigned int)data->cif_cfg.capture_buf.p_Cb+(JPEG_Info.JpegCaptureBuffSize/2/2)-4),
										((unsigned int)data->cif_cfg.capture_buf.p_Cr+(JPEG_Info.JpegCaptureBuffSize/2/2)-4));
		}
		else
		{
			TDD_CIF_SetBaseAddr(IN_IMG_ROLLING,
										((unsigned int)data->cif_cfg.capture_buf.p_Y+(JPEG_Info.JpegCaptureBuffSize/2)-4),
										((unsigned int)data->cif_cfg.capture_buf.p_Cb+(JPEG_Info.JpegCaptureBuffSize/2/4)-4),
										((unsigned int)data->cif_cfg.capture_buf.p_Cr+(JPEG_Info.JpegCaptureBuffSize/2/4)-4));
		}
	}
		#endif
	#endif
#endif // CONFIG_USE_ISP
}

void cif_interrupt_enable(enum cifoper_mode mode)
{
#if defined(CONFIG_USE_ISP)
	ISP_INTERRUPT_SET();
#else
	TDD_CIF_SetMaskIntStatus(SET_CIF_INT_ALL_MASK);

	if(mode == OPER_PREVIEW)
	{
		TDD_CIF_SetMaskIntStatus(SET_CIF_INT_STORE_1FRM_NOT_MASK);
	}
	else
	{
#ifdef JPEG_ENCODE_WITH_CAPTURE
		if(gIsRolling)
		{
			TDD_CIF_SetMaskIntStatus(SET_CIF_INT_VCNT_NOT_MASK);
		}
		else
#endif
		{
			TDD_CIF_SetMaskIntStatus(SET_CIF_INT_ENC_STRT_NOT_MASK);
			TDD_CIF_SetInterrupt(SET_CIF_UPDATE_WITHOUT_VSYNC);
#if defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150)
			TDD_CIF_SetInterrupt(SET_CIF_INT_TYPE_HOLDUP);
#endif
		}
	}

	TDD_CIF_SetInterrupt(SET_CIF_INT_ENABLE);
#endif //CONFIG_USE_ISP	
}

void cif_interrupt_disable(void)
{
#if defined(CONFIG_USE_ISP)
	ISP_INTERRUPT_CLEAR();
#else
	TDD_CIF_SetInterrupt(SET_CIF_INT_DISABLE);
#endif
}

void cif_global_reset(void)
{
	bool bEnableOfATV = FALSE;
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;

#if defined(CONFIG_USE_ISP)
	// todo : 
#else
#if defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150)	
	TDD_CIF_SetInterrupt(SET_CIF_UPDATE_WITHOUT_VSYNC);	//TDD_CIF_SetInterrupt(SET_CIF_INT_TYPE_HOLDUP);
	TDD_CIF_SetInterrupt(SET_CIF_INT_TYPE_HOLDUP);
#else	/* kilee@add_end */
	TDD_CIF_SetInterrupt(SET_CIF_INT_TYPE_HOLDUP); //should be '1'
	TDD_CIF_SetInterrupt(SET_CIF_UPDATE_IN_VSYNC);
#endif

#if defined(CONFIG_VIDEO_CAMERA_SENSOR_AIT848_ISP) || defined(CONFIG_VIDEO_CAMERA_SENSOR_ISX006)
	if(data->cif_cfg.oper_mode == OPER_CAPTURE)
		TDD_CIF_SetInfo(SET_CIF_ALL, NON_SEPARATE, MSB_FIRST, MODE_YUV, FMT422, BAYER_RGB, MODE16, data->cif_cfg.order422, SWICH_BUS);
	else
		TDD_CIF_SetInfo(SET_CIF_ALL, SEPARATE, MSB_FIRST, MODE_YUV, FMT422, BAYER_RGB, MODE16, data->cif_cfg.order422, SWICH_BUS);
#else
	TDD_CIF_SetInfo(SET_CIF_ALL, SEPARATE, MSB_FIRST, MODE_YUV, FMT422, BAYER_RGB, MODE16, data->cif_cfg.order422, SWICH_BUS);
#endif
	
	//DMA configure
	TDD_CIF_SetTransfer(SET_CIF_TRANSFER_ALL, BURST8, LOCK_TR, BURST_TRANS);
	TDD_CIF_SetSyncPol(data->cif_cfg.polarity_href, data->cif_cfg.polarity_vsync);

	//Effect all clear
	TDD_CIF_SetEffectMode(SET_CIF_CEM_NOR);

#if defined(CONFIG_VIDEO_CAMERA_SENSOR_AIT848_ISP) || defined(CONFIG_VIDEO_CAMERA_SENSOR_ISX006)
	if(data->cif_cfg.oper_mode == OPER_CAPTURE)
		TDD_CIF_SetCtrl(SET_CIF_II_ALL, CIF_PWDN_DISABLE, CIF_BYPASS_SCALER_ENABLE,
						data->cif_cfg.polarity_pclk, FRAME_0, data->cif_cfg.fmt, CIF_656CONVERT_DISABLE, bEnableOfATV);
	else
		TDD_CIF_SetCtrl(SET_CIF_II_ALL, CIF_PWDN_DISABLE, CIF_BYPASS_SCALER_DISABLE,
							data->cif_cfg.polarity_pclk, FRAME_0, data->cif_cfg.fmt, CIF_656CONVERT_DISABLE, bEnableOfATV);
#elif defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150) //|| defined(CONFIG_VIDEO_ATV_SENSOR_RDA5888)
	bEnableOfATV = TRUE;
	#ifdef	CONFIG_VIDEO_ATV_SENSOR_TVP5150
		TDD_CIF_Set656FormatConfig(SET_CIF_656_FIELD_INFO| SET_CIF_656_H_BLANK | SET_CIF_656_V_BLANK, 0, 0, 0, 0, 0x0D, 0x09, 0x0B);
		TDD_CIF_SetCtrl(SET_CIF_II_ALL, CIF_PWDN_DISABLE, CIF_BYPASS_SCALER_ENABLE,
			data->cif_cfg.polarity_pclk, FRAME_0, data->cif_cfg.fmt, CIF_656CONVERT_ENABLE, bEnableOfATV);
		//DMA configure
		TDD_CIF_SetTransfer(SET_CIF_TRANSFER_ALL, BURST8, NON_LOCK_TR, BURST_TRANS);
		//HwCIF->ICPCR1 = 0xB8802406;	//0xB8802406;	// 0xB8802406
		//HwCIF->CCIR656FCR2 = 0x352B;	//0x352B;	// 0x352B
		//HwCIF->CDCR1 = 0x03;
	#else
		bEnableOfATV = FALSE;
		TDD_CIF_SetCtrl(SET_CIF_II_ALL, CIF_PWDN_DISABLE, CIF_BYPASS_SCALER_ENABLE,
		data->cif_cfg.polarity_pclk, FRAME_0, data->cif_cfg.fmt, CIF_656CONVERT_DISABLE, bEnableOfATV);
	#endif	

#else
	TDD_CIF_SetCtrl(SET_CIF_II_ALL, CIF_PWDN_DISABLE, CIF_BYPASS_SCALER_DISABLE,
						data->cif_cfg.polarity_pclk, FRAME_0, data->cif_cfg.fmt, CIF_656CONVERT_DISABLE, bEnableOfATV);
#endif
#endif //CONFIG_USE_ISP
}


#if defined(CONFIG_ARCH_TCC892X)
void cif_set_port(unsigned int nUsingPort)
{
	// please note that every port mapping's number has to different!!
	// In now, we have to use Number 4 port.
	BITCSET(pDDIConfig->CIFPORT.nREG, 0x00077777, (0 << 16) | (3 << 12) | (2 << 8) | (1 << 4) | nUsingPort);
}

int tccxxx_vioc_vin_main(VIOC_VIN *pVIN)
{
	uint *pLUT;
	uint width, height, offs_width, offs_height, offs_height_intl;
	struct TCCxxxCIF *data = (struct TCCxxxCIF *)&hardware_data;

	offs_width = offs_height = offs_height_intl	= 0;

#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	if(data->cif_cfg.oper_mode == OPER_PREVIEW
	|| (data->cif_cfg.oper_mode == OPER_CAPTURE && data->cif_cfg.main_set.target_x < tcc_sensor_info.cam_capchg_width)) {
		offs_width 		= tcc_sensor_info.preview_zoom_offset_x;
		offs_height 	= tcc_sensor_info.preview_zoom_offset_y;
		width 			= tcc_sensor_info.preview_w;
		height 			= tcc_sensor_info.preview_h;
	} else {
		offs_width 		= tcc_sensor_info.capture_zoom_offset_x;
		offs_height 	= tcc_sensor_info.capture_zoom_offset_y;
		width 			= tcc_sensor_info.capture_w;
		height 			= tcc_sensor_info.capture_h;
	}
#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	if(data->cif_cfg.oper_mode == OPER_PREVIEW
		|| (data->cif_cfg.oper_mode == OPER_CAPTURE && data->cif_cfg.main_set.target_x < CAM_CAPCHG_WIDTH)) {
		offs_width 		= PRV_ZOFFX;
		offs_height 	= PRV_ZOFFY;
		width 			= PRV_W;
		height 			= PRV_H;
	} else {
		offs_width 		= CAP_ZOFFX;
		offs_height 	= CAP_ZOFFY;
		width 			= CAP_W;
		height 			= CAP_H;
	}
#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT

	data->cif_cfg.main_set.win_hor_ofst = offs_width  * data->cif_cfg.zoom_step;
	data->cif_cfg.main_set.win_ver_ofst = offs_height * data->cif_cfg.zoom_step;
	data->cif_cfg.main_set.source_x 	= width  - (offs_width  * data->cif_cfg.zoom_step) * 2;
	data->cif_cfg.main_set.source_y 	= height - (offs_height * data->cif_cfg.zoom_step) * 2;
	dprintk("%s():  width=%d, height=%d, offset_x=%d, offset_y=%d. \n", __FUNCTION__, data->cif_cfg.main_set.source_x, \
			data->cif_cfg.main_set.source_y, data->cif_cfg.main_set.win_hor_ofst, data->cif_cfg.main_set.win_ver_ofst);

	if((uint)pVIN == (uint)tcc_p2v(HwVIOC_VIN00)) { 	// VIN00 means VIN0 component
		pLUT = (unsigned int *)tcc_p2v(HwVIOC_VIN01); 	// VIN01 means LUT of VIN0 component
		VIOC_CONFIG_WMIXPath(WMIX50, OFF);
	} else {
		pLUT = (uint *)tcc_p2v(HwVIOC_VIN11); 			// VIN11 means LUT of VIN1 component
		VIOC_CONFIG_WMIXPath(WMIX60, OFF);
	}
	VIOC_WMIX_SetUpdate(pWMIXBase);

#if defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150)
	VIOC_VIN_SetSyncPolarity(pVIN, OFF, OFF, OFF, OFF, OFF, ON);
	VIOC_VIN_SetCtrl(pVIN, ON, OFF, OFF, FMT_YUV422_8BIT, ORDER_RGB);
	VIOC_VIN_SetInterlaceMode(pVIN, ON, OFF);
#else
	#if defined(CONFIG_MACH_M805_892X) // Because of DE signal
		if((system_rev == 0x2002) || (system_rev == 0x2003) || (system_rev == 0x2004) || (system_rev == 0x2005)) //M805_8925 board. use DE signal
		{
			VIOC_VIN_SetSyncPolarity(pVIN, !(data->cif_cfg.polarity_href), !(data->cif_cfg.polarity_vsync), \
									 OFF, data->cif_cfg.polarity_de, OFF, !(data->cif_cfg.polarity_pclk));
			VIOC_VIN_SetCtrl(pVIN, OFF, OFF, OFF, FMT_YUV422_8BIT, ORDER_RGB);
		}
		else // M805_8923 board. not use DE signal.
		{
			VIOC_VIN_SetSyncPolarity(pVIN, !(data->cif_cfg.polarity_href), !(data->cif_cfg.polarity_vsync), \
									 OFF, data->cif_cfg.polarity_de, OFF, !(data->cif_cfg.polarity_pclk));
			VIOC_VIN_SetCtrl(pVIN, OFF, ON, ON, FMT_YUV422_8BIT, ORDER_RGB);
		}
	#else // CONFIG_MACH_M805_892X
		VIOC_VIN_SetSyncPolarity(pVIN, !(data->cif_cfg.polarity_href), !(data->cif_cfg.polarity_vsync), \
								 OFF, data->cif_cfg.polarity_de, OFF, !(data->cif_cfg.polarity_pclk));
		VIOC_VIN_SetCtrl(pVIN, OFF, OFF, OFF, FMT_YUV422_8BIT, ORDER_RGB);
	#endif // CONFIG_MACH_M805_892X
	VIOC_VIN_SetInterlaceMode(pVIN, OFF, OFF);
#endif

	VIOC_VIN_SetImageSize(pVIN, data->cif_cfg.main_set.source_x, data->cif_cfg.main_set.source_y);
	VIOC_VIN_SetImageOffset(pVIN, data->cif_cfg.main_set.win_hor_ofst, data->cif_cfg.main_set.win_ver_ofst, offs_height_intl);
	VIOC_VIN_SetY2RMode(pVIN, 2);
	VIOC_VIN_SetY2REnable(pVIN, OFF);
	VIOC_VIN_SetLUT(pVIN, pLUT);
	VIOC_VIN_SetLUTEnable(pVIN, OFF, OFF, OFF);
	VIOC_VIN_SetEnable(pVIN, ON);

	return 0;
}

int  tccxxx_vioc_vin_demux_main(VIOC_VIN_DEMUX *pVINDEMUX, uint enable)
{
    //VIOC_VIN_SetDemuxPort(VIOC_VIN_DEMUX *pVINDEMUX, uint p0, uint p1, uint p2, uint p3)
    VIOC_VIN_SetDemuxPort(pVINDEMUX, EXT_PORT0, EXT_PORT1, EXT_PORT2, EXT_PORT3);

    //VIOC_VIN_SetDemuxClock(VIOC_VIN_DEMUX *pVINDEMUX, uint mode)
    VIOC_VIN_SetDemuxClock(pVINDEMUX, CLK_DOUBLE_EDGE);

    //VIOC_VIN_SetDemuxEnable(VIOC_VIN_DEMUX *pVINDEMUX, uint enable)
    VIOC_VIN_SetDemuxEnable(pVINDEMUX, enable);

	return 0;
}


int tccxxx_vioc_vin_wdma_set(VIOC_WDMA *pDMA_CH)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;
	uint dw, dh; 	// destination image size
	uint fmt; 		// image format

	volatile PVIOC_IREQ_CONFIG pIREQConfig;
	pIREQConfig = (volatile PVIOC_IREQ_CONFIG)tcc_p2v(HwVIOC_IREQ);

	dw = data->cif_cfg.main_set.target_x;
	dh = data->cif_cfg.main_set.target_y;

	if(data->cif_cfg.oper_mode == OPER_CAPTURE)
		fmt = VIOC_IMG_FMT_YUV422SEP;	// 4:2:2 separate format
	else
		fmt = VIOC_IMG_FMT_YUV420SEP;	// 4:2:0 separate format


	if(data->cif_cfg.oper_mode == OPER_CAPTURE)
		cif_capture_dma_set(data);
	else
		cif_dma_hw_reg(0);

	dprintk("%s():  WDMA size[%dx%d], format[%d]. \n", __FUNCTION__, dw, dh, fmt);
	
	VIOC_WDMA_SetImageFormat(pDMA_CH, fmt);
	VIOC_WDMA_SetImageSize(pDMA_CH, dw, dh);
	VIOC_WDMA_SetImageOffset(pDMA_CH, fmt, dw);
						
	if(data->cif_cfg.oper_mode == OPER_CAPTURE) {
		printk("While capture, frame by frame mode. \n");
		VIOC_WDMA_SetImageEnable(pDMA_CH, OFF);	// operating start in 1 frame
		BITCSET(pWDMABase->uIRQSTS.nREG, 1<<5, 1<<5); // clear EORF bit
		VIOC_WDMA_SetIreqMask(pWDMABase, VIOC_WDMA_IREQ_EOFR_MASK, 0x0);
	} else {
		printk("While preview, continuous mode. \n");
		VIOC_WDMA_SetImageEnable(pDMA_CH, ON);	// operating start in 1 frame
		BITCSET(pWDMABase->uIRQSTS.nREG, 1<<6, 1<<6); // clear EOFF bit
		VIOC_WDMA_SetIreqMask(pWDMABase, VIOC_WDMA_IREQ_EOFF_MASK, 0x0);
	}

	return 0;
}

int tccxxx_vioc_scaler_set(VIOC_SC *pSC, VIOC_VIN *pVIN, VIOC_WMIX *pWMIX, uint plug_in)
{
	uint channel;
	uint dw, dh;			// destination size in SCALER
	uint width, height;
	uint mw, mh;			// image size in WMIX
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;

#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	if(data->cif_cfg.oper_mode == OPER_PREVIEW
	|| (data->cif_cfg.oper_mode == OPER_CAPTURE && data->cif_cfg.main_set.target_x < tcc_sensor_info.cam_capchg_width)) {
		width			= tcc_sensor_info.preview_w;
		height			= tcc_sensor_info.preview_h;
	} else {
		width			= tcc_sensor_info.capture_w;
		height			= tcc_sensor_info.capture_h;
	}
#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	if(data->cif_cfg.oper_mode == OPER_PREVIEW
		|| (data->cif_cfg.oper_mode == OPER_CAPTURE && data->cif_cfg.main_set.target_x < CAM_CAPCHG_WIDTH)) {
		width			= PRV_W;
		height			= PRV_H;
	} else {
		width			= CAP_W;
		height			= CAP_H;
	}
#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT

	dw = data->cif_cfg.main_set.target_x;
	dh = data->cif_cfg.main_set.target_y;

	if (plug_in == VIOC_SC_VIN_00) {			// PlugIn path : VIN0 - SCALE - WMIX
		mw = data->cif_cfg.main_set.target_x;
		mh = data->cif_cfg.main_set.target_y;
	} else if (plug_in == VIOC_SC_VIN_01) {		// PlugIn path : VIN1 - SCALE - WMIX
		mw = data->cif_cfg.main_set.target_x;
		mh = data->cif_cfg.main_set.target_y;
	} else if (plug_in == VIOC_SC_WDMA_06) {		// PlugIn path : VIN0 - WMIX - SCALE
		#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
		mw 	= tcc_sensor_info.preview_w;
		mh 	= tcc_sensor_info.preview_h;
		#else
		mw 	= PRV_W;
		mh 	= PRV_H;
		#endif
	} else if (plug_in == VIOC_SC_WDMA_05) {		// PlugIn path : VIN0 - WMIX - SCALE
		#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
		mw 	= tcc_sensor_info.preview_w;
		mh 	= tcc_sensor_info.preview_h;
		#else
		mw 	= PRV_W;
			#ifdef	CONFIG_VIDEO_ATV_SENSOR_TVP5150
				mh 	= PRV_H*2;
			#else
				mh 	= PRV_H;
			#endif
		#endif
	} else if (plug_in == VIOC_SC_WDMA_08) {		// PlugIn path : VIN1 - WMIX - SCALE
		#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
		mw 	= tcc_sensor_info.preview_w;
		mh 	= tcc_sensor_info.preview_h;
		#else
		mw 	= PRV_W;
		mh 	= PRV_H;
		#endif
	} else if (plug_in == VIOC_SC_WDMA_07) {		// PlugIn path : VIN1 - WMIX - SCALE
		#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
		mw 	= tcc_sensor_info.preview_w;
		mh 	= tcc_sensor_info.preview_h;
		#else
		mw 	= PRV_W;
		mh 	= PRV_H;
		#endif
	}

	if((uint)(pSC) == (uint)tcc_p2v(HwVIOC_SC0)) {
		channel = VIOC_SC0;
	} else if((uint)(pSC) == (uint)tcc_p2v(HwVIOC_SC1)) {
		channel = VIOC_SC1;
	} else if((uint)(pSC) == (uint)tcc_p2v(HwVIOC_SC2)) {
		channel = VIOC_SC2;
	} else if((uint)(pSC) == (uint)tcc_p2v(HwVIOC_SC3)) {
		channel = VIOC_SC3;
	}

#if !defined(CONFIG_MACH_M805_892X)
	if(dw == width && dh == height){
		dprintk("VIOC don't use SC, Because input size and output size is same\n");
		VIOC_CONFIG_PlugOut(channel);
		return 0;
	}
#endif

#if defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150) // 4:3 ratio.
	mw = dw + 16;
	mh = dh + 12;
#endif

	dprintk("%s():  channel=%d, Dst[%dx%d], WMIX[%dx%d]. \n", __FUNCTION__, channel, dw, dh, mw, mh);
	VIOC_CONFIG_PlugIn(channel, plug_in); 	// PlugIn position in SCALER
	VIOC_SC_SetBypass(pSC, OFF);
#if defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150)
	VIOC_SC_SetDstSize(pSC, mw, mh);		// set destination size in scaler
	VIOC_SC_SetOutPosition(pSC, (mw-dw)/2, (mh-dh)/2);
#else
	VIOC_SC_SetDstSize(pSC, dw, dh); 		// set destination size in scaler
#endif
	VIOC_SC_SetOutSize(pSC, dw, dh); 		// set output size in scaler
	VIOC_SC_SetUpdate(pSC); 				// Scaler update

	return 0;
}

#if	defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150)
int tccxxx_vioc_viqe_main(uint use_top_size)
{
	uint	width, height;
	uint	fmt;
	uint	bypass_deintl, bypass_d3d;
	uint	deintl_base0;
	uint	deintl_base1;
	uint	deintl_base2;
	uint	deintl_base3;
	uint	denoise_base0;
	uint	denoise_base1;
	uint	frmnum;
	int		cdf_lut_en;
	int		his_en;
	int		gamut_en;
	int		d3d_en;
	int		deintl_en;

	unsigned int	*pVIQE_DEINTL_BASE0;
	unsigned int	*pVIQE_DEINTL_BASE1;
	unsigned int	*pVIQE_DEINTL_BASE2;
	unsigned int	*pVIQE_DEINTL_BASE3;
	unsigned int	*pVIQE_D3D_BASE0;
	unsigned int	*pVIQE_D3D_BASE1;

	cdf_lut_en 		= OFF;				// color LUT (pixel_mapper)
	his_en 			= OFF;				// histogram
	gamut_en 		= OFF;				// gamut
	d3d_en 			= OFF;				// denoise 3d
	deintl_en 		= ON;				// deinterlacer
	
	fmt 			= FMT_FC_YUV420;
	bypass_deintl 	= ON; //20120508 ysseung   modify.
	bypass_d3d 		= ON;

	width 			= PRV_W;
	height 			= PRV_H;

	// if you use VIQE, you have to use physical address.
	pVIQE_DEINTL_BASE0 = PA_TEMP;
	pVIQE_DEINTL_BASE1 = PA_TEMP + (width * height * 2 * 2);
	pVIQE_DEINTL_BASE2 = PA_TEMP + (width * height * 2 * 2) * 2;
	pVIQE_DEINTL_BASE3 = PA_TEMP + (width * height * 2 * 2) * 3;

	deintl_base0	= (unsigned char *)(pVIQE_DEINTL_BASE0);
	deintl_base1	= (unsigned char *)(pVIQE_DEINTL_BASE1);
	deintl_base2	= (unsigned char *)(pVIQE_DEINTL_BASE2);
	deintl_base3	= (unsigned char *)(pVIQE_DEINTL_BASE3);
	denoise_base0	= (uint)(pVIQE_D3D_BASE0);
	denoise_base1	= (uint)(pVIQE_D3D_BASE1);

	frmnum = 0;

	if(use_top_size == ON) { // if (use_top_size == ON), size information of VIQE is gotten by VIOC modules.
		width 	= 0;
		height	= 0;
	}

	dprintk("%s():  width=%d, height=%d, format=%d. \n", __FUNCTION__, width, height, fmt);
	VIOC_VIQE_SetControlRegister((VIQE *)(pVIQEBase), width, height, fmt);
	VIOC_VIQE_SetDeintlRegister((VIQE *)(pVIQEBase), fmt, OFF, width, height, bypass_deintl, 	\
								deintl_base0, deintl_base1, deintl_base2, deintl_base3);
	VIOC_VIQE_SetDenoise((VIQE *)(pVIQEBase), fmt, width, height, bypass_d3d, frmnum, 			\
								denoise_base0, denoise_base1);
	VIOC_VIQE_SetControlEnable((VIQE *)(pVIQEBase), cdf_lut_en, his_en, gamut_en, d3d_en, 		\
								deintl_en);

	return 0;
}

int tccxxx_vioc_viqe_wmix_set(VIOC_WMIX *pWMIX, uint plug_in, unchar bUseSimpleDeIntl)
{
	uint channel;
	uint mw, mh;			

	mw = PRV_W;
	mh = PRV_H * 2;

	#if 0
	if(plug_in == VIOC_VIQE_VIN_00) {
		mw = PRV_W;
		mh = PRV_H * 2;				// set vertical size in WMIX 
	} else if(plug_in == VIOC_VIQE_VIN_01) {
		mw = PRV_W;
		mh = PRV_H * 2;				// set vertical size in WMIX
	}
	#endif

	if(!bUseSimpleDeIntl) 	channel = VIOC_VIQE;
	else 					channel = VIOC_DEINTLS;

	dprintk("%s():  width=%d, height=%d, channel=%d. \n", __FUNCTION__, mw, mh, channel);
	VIOC_CONFIG_PlugIn(channel, plug_in);

	return 0;
}
#endif // CONFIG_VIDEO_ATV_SENSOR_TVP5150
#endif // CONFIG_ARCH_TCC892X



#ifdef JPEG_ENCODE_WITH_CAPTURE
static irqreturn_t tccxxx_jpeg_handler(int irq, void *dev_id/*, struct pt_regs *reg*/)
{  	
	unsigned long lFlag, TempFlag;
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;
	struct tccxxx_cif_buffer *cif_buf;

	lFlag = HwJP_INT_FLAG;

	if (lFlag & HwJP_INT_FLAG_CODED_BUF_STAT)
	{
#ifdef JPEG_ENCODE_WITH_CAPTURE
	data->cif_cfg.cap_status = CAPTURE_OVERFLOW;
	#ifdef JPEG_TESTCODE
		JPEG_BUS_Disable(JPEG_ENCODE_MODE); // temp!! NO-thumbnail!!
		JPEG_INTERRUPT_Disable(JPEG_ENCODE_MODE);
	#endif
		wake_up_interruptible(&data->frame_wait);
#endif // JPEG_ENCODE_WITH_CAPTURE
		dprintk(KERN_ALERT "JPEG ENCODING BUFFER-ERROR! \r\n");

	}
	else if (lFlag & HwJP_INT_FLAG_DECODING_ERR)
	{

	}
	else if (lFlag & HwJP_INT_FLAG_JOB_FINISHED)
	{
		//Save to memory
		if (gJpegCodecMode == JPEG_ENCODE_MODE)
		{
			gJpegEncodeDone = 1;
			data->cif_cfg.cap_status = CAPTURE_DONE;		

#ifdef JPEG_ENCODE_WITH_CAPTURE
	#ifdef JPEG_TESTCODE
			tccxxx_jpeg_make_header(&(data->cif_cfg.jpg_info.jpg_len), &(data->cif_cfg.jpg_info.jpg_hdr_len));
			JPEG_BUS_Disable(JPEG_ENCODE_MODE); // temp!! NO-thumbnail!!
			JPEG_INTERRUPT_Disable(JPEG_ENCODE_MODE);
	#endif
			wake_up_interruptible(&data->frame_wait);
#endif // JPEG_ENCODE_WITH_CAPTURE
			
			dprintk(KERN_ALERT "JPEG ENCODING DONE! \r\n");
		}
		else if (gJpegCodecMode == JPEG_DECODE_MODE)
		{
			gJpegDecodeDone = 1;
			dprintk(KERN_ALERT "JPEG DECODING DONE \r\n");
		}
	}
#ifdef TCC83XX
	else if (lFlag & HwJP_INT_FLAG_YBUF_ROLLING)
	{
		BITSET(HwJP_INT_MASK, HwJP_INT_MASK_YBUF_ROLLING);
	}
#endif

	TempFlag = HwJP_INT_ACK;

	return IRQ_HANDLED;
}

static int
tccxxx_jpeg_encode_method(void)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;
	unsigned int  ucReturnVal = FALSE;
	unsigned int  temp_1, temp_2;

	JPEG_Info.JpegCaptureBuffSize = CAPTURE_MEM;

	if (JPEG_Info.JpegCaptureBuffSize < data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y*2)
		ucReturnVal = TRUE;

	if(ucReturnVal)
	{
		temp_1 = (JPEG_Info.JpegCaptureBuffSize/2)/data->cif_cfg.main_set.target_x;

		if (temp_1%FRAME_LINE_CNT != 0)
		{
			temp_2 = temp_1/FRAME_LINE_CNT;
			JPEG_Info.JpegCaptureBuffSize = (temp_2*FRAME_LINE_CNT)*data->cif_cfg.main_set.target_x*2;
		}
	}

	return ucReturnVal;
}

static int
tccxxx_jpeg_encode_init(enum jpeg_quantisation_val val)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;

	unsigned int uiOutputBufferSize, uiInputBufferSize;
	void *BufferY, *BufferU, *BufferV;

	gJpegCodecMode = JPEG_UNKNOWN_MODE;
	JPEG_Exif_Header_Info_Init();

	BufferY = (void*)data->cif_cfg.capture_buf.p_Y ;
	BufferU = (void*)data->cif_cfg.capture_buf.p_Cb;
	BufferV = (void*)data->cif_cfg.capture_buf.p_Cr;
	
	JPEG_BUS_Enable(JPEG_ENCODE_MODE);
	JPEG_SW_Reset(JPEG_ENCODE_MODE);
	gJpegEncodeDone = 0;
	
	uiInputBufferSize = (JPEG_Info.JpegCaptureBuffSize/2)/data->cif_cfg.main_set.target_x;
	uiOutputBufferSize = gJPEG_Buffer_Info.pBaseBitstreamDataSize/1024;

	gJPEG_ENC_Info.is_rolling = gIsRolling;
	gJPEG_ENC_Info.y_addr = (unsigned int)BufferY;
	gJPEG_ENC_Info.u_addr = (unsigned int)BufferU;
	gJPEG_ENC_Info.v_addr = (unsigned int)BufferV;
	gJPEG_ENC_Info.ifrm_hsize = data->cif_cfg.main_set.target_x;
	gJPEG_ENC_Info.ibuf_vsize = uiInputBufferSize;
	gJPEG_ENC_Info.q_value = val;
	gJPEG_ENC_Info.img_hsize = data->cif_cfg.main_set.target_x;
	gJPEG_ENC_Info.img_vsize = data->cif_cfg.main_set.target_y;
	gJPEG_ENC_Info.cbuf_addr = (unsigned int)gJPEG_Buffer_Info.pBaseBitstreamDataAddr;
	gJPEG_ENC_Info.cbuf_size = uiOutputBufferSize;
	gJPEG_ENC_Info.frame_cnt = FRAME_LINE_CNT;
	gJPEG_ENC_Info.chroma = HwJP_CHROMA_422;

#ifdef JPEG_TESTCODE
	data->cif_cfg.jpg_info.jpg_buf_addr = jpeg_remapped_base_address;
	data->cif_cfg.jpg_info.jpg_hdr_addr = jpeg_header_remapped_address;
#endif	

	JPEG_SET_Encode_Config(&gJPEG_ENC_Info);

	JPEG_INTERRUPT_Enable(JPEG_ENCODE_MODE);

	return 0;
}

int tccxxx_jpeg_make_header(unsigned int *uiBitStreamSize, unsigned int *uiHeaderSize)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;
	void *HeaderBuffer;

	//while (!gJpegEncodeDone)

	if (gIsExifThumbnailProcess)
	{
		gEncodeOption.ThumbnailInfo.pThumbnail_Buff = (void *)((uint32)jpeg_header_remapped_address + 2048);
		gEncodeOption.ThumbnailInfo.thumbnail_image_size = JPEG_Encoded_Data_Size();
	}
	else
	{
		*uiBitStreamSize = JPEG_Encoded_Data_Size();

		gEncodeOption.BitStreamSize = *uiBitStreamSize;
		gEncodeOption.ThumbnailInfo.thumbnail_image_size = 0;
		gEncodeOption.ImageWidth = data->cif_cfg.main_set.target_x;
		gEncodeOption.ImageHeight = data->cif_cfg.main_set.target_y;
		gEncodeOption.Q_FactorValue = JPEG_DEFAULT;
	}

	HeaderBuffer = jpeg_header_remapped_address;

	*uiHeaderSize = TCCXXX_JPEG_Make_Header((uint32)HeaderBuffer, &gEncodeOption, &gJpegHeaderExifRsp);
	*uiBitStreamSize = gEncodeOption.BitStreamSize;

	gJpegEncodeDone = 0;
	gIsExifThumbnailProcess = FALSE;

	return 0;
}
#endif

#if (1) //20111209 ysseung   test...
int tccxxx_cif_buffer_set(struct v4l2_requestbuffers *req)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;
	struct tccxxx_cif_buffer *buf = NULL;
	unsigned int y_offset = 0, uv_offset = 0, stride = 0;
	unsigned int buff_size;

	if(req->count == 0) {
		data->cif_cfg.now_frame_num = 0;

		#if defined(FEATURE_ANDROID_ICS)
		stride = ALIGNED_BUFF(data->cif_cfg.main_set.target_x, 16);
		y_offset = stride * data->cif_cfg.main_set.target_y;
		#else // FEATURE_ANDROID_ICS
		y_offset = data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y;
		#endif // FEATURE_ANDROID_ICS
		if(data->cif_cfg.fmt == M420_ZERO) 	buff_size = PAGE_ALIGN(y_offset*2);
		else 								buff_size = PAGE_ALIGN(y_offset + y_offset/2);

		data->buf->v4lbuf.length = buff_size;

		memset(data->static_buf, 0x00, req->count * sizeof(struct tccxxx_cif_buffer));

		data->done_list.prev = data->done_list.next = &data->done_list;
		data->list.prev 	 = data->list.next 		= &data->list;

		data->cif_cfg.base_buf = (unsigned int)req->reserved[0];
	}

	buf = &(data->static_buf[req->count]);

	INIT_LIST_HEAD(&buf->buf_list);

	buf->mapcount 		= 0;
	buf->cam 			= data;
	buf->v4lbuf.index 	= req->count;
	buf->v4lbuf.type 	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf->v4lbuf.field 	= V4L2_FIELD_NONE;
	buf->v4lbuf.memory 	= V4L2_MEMORY_MMAP;
	buf->v4lbuf.length 	= buff_size;

	#if defined(FEATURE_ANDROID_ICS)
	stride = ALIGNED_BUFF(data->cif_cfg.main_set.target_x, 16);
	y_offset = stride * data->cif_cfg.main_set.target_y;
	uv_offset = ALIGNED_BUFF((stride/2), 16) * (data->cif_cfg.main_set.target_y/2);
	#else // FEATURE_ANDROID_ICS
	if(data->cif_cfg.fmt == M420_ZERO)
		uv_offset = (data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y)/2;
	else
		uv_offset = (data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y)/4;
	#endif // FEATURE_ANDROID_ICS

	data->cif_cfg.preview_buf[req->count].p_Y  = (unsigned int)req->reserved[0];
	data->cif_cfg.preview_buf[req->count].p_Cb = (unsigned int)data->cif_cfg.preview_buf[req->count].p_Y + y_offset;
	data->cif_cfg.preview_buf[req->count].p_Cr = (unsigned int)data->cif_cfg.preview_buf[req->count].p_Cb + uv_offset;
	data->cif_cfg.pp_num = req->count;

	return 0;
}

void tccxxx_set_camera_addr(int index, unsigned int addr, unsigned int cameraStatus)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;
	unsigned int y_offset = 0, uv_offset = 0, stride = 0;

	#if defined(FEATURE_ANDROID_ICS)
	if(cameraStatus == 1 /* MODE_PREVIEW */) {
		stride = ALIGNED_BUFF(data->cif_cfg.main_set.target_x, 16);
		y_offset = stride * data->cif_cfg.main_set.target_y;
		uv_offset = ALIGNED_BUFF((stride/2), 16) * (data->cif_cfg.main_set.target_y/2);
	} else if(cameraStatus == 3 /* MODE_CAPTURE */) {
		y_offset = data->cif_cfg.main_set.target_x * data->cif_cfg.main_set.target_y;
		if(data->cif_cfg.fmt == M420_ZERO) {
			uv_offset = data->cif_cfg.main_set.target_x * data->cif_cfg.main_set.target_y / 2;
		} else {
			uv_offset = data->cif_cfg.main_set.target_x * data->cif_cfg.main_set.target_y / 4;
		}
	}
	#else // FEATURE_ANDROID_ICS
	y_offset = data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y;
	if(data->cif_cfg.fmt == M420_ZERO)
		uv_offset = (data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y)/2;
	else
		uv_offset = (data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y)/4;
	#endif // FEATURE_ANDROID_ICS

	if(cameraStatus == 1 /* MODE_PREVIEW */) {
		data->cif_cfg.preview_buf[index].p_Y  = addr;
		data->cif_cfg.preview_buf[index].p_Cb = (unsigned int)data->cif_cfg.preview_buf[index].p_Y + y_offset;
		data->cif_cfg.preview_buf[index].p_Cr = (unsigned int)data->cif_cfg.preview_buf[index].p_Cb + uv_offset;
	} else if(cameraStatus == 3 /* MODE_CAPTURE */) {
		data->cif_cfg.capture_buf.p_Y  = addr;
		data->cif_cfg.capture_buf.p_Cb = (unsigned int)data->cif_cfg.capture_buf.p_Y + y_offset;
		data->cif_cfg.capture_buf.p_Cr = (unsigned int)data->cif_cfg.capture_buf.p_Cb + uv_offset;
	}
}
#else
int tccxxx_cif_buffer_set(struct v4l2_requestbuffers *req)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;
	dma_addr_t base_addr = data->cif_cfg.base_buf;
	unsigned int y_offset = data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y;
	unsigned int uv_offset = 0;
	unsigned int total_off = 0;
	unsigned char i;
	unsigned long total_req_buf_size=0;
	unsigned char req_buf = req->count;
	unsigned int buff_size;
	
	data->cif_cfg.now_frame_num = 0;

	if (req->count > TCC_CAMERA_MAX_BUFNBRS) 
		req->count = TCC_CAMERA_MAX_BUFNBRS;

	if (req->count < 0) 
		req->count = 2;

	if(data->cif_cfg.fmt == M420_ZERO)
		buff_size = PAGE_ALIGN(y_offset*2);
	else
		buff_size = PAGE_ALIGN(y_offset + y_offset/2);

	data->buf->v4lbuf.length = buff_size;

retry:
	if(req_buf == 1)
	{
#if 0  // ZzaU :: Don't check Buffer in Rolling-Capture.
		if (data->buf->v4lbuf.length > CAPTURE_MEM_SIZE)
		{
			printk("reqbufs: count invalid\n");
			return -1;
		}
#endif
	}
	else
	{
		if (data->buf->v4lbuf.length*req->count > PREVIEW_MEM_SIZE) 
		{
			req->count--;
			if (req->count <= 0) 
			{
				printk("reqbufs: count invalid\n");
				return -1;
			}
			goto retry;
		}
	}

	memset(data->static_buf,0x00,req->count * sizeof(struct tccxxx_cif_buffer));
	
	data->done_list.prev = data->done_list.next = &data->done_list;
	data->list.prev 	 = data->list.next 		= &data->list;
		
	for (data->n_sbufs = 0; data->n_sbufs < req->count; (data->n_sbufs++)) 
	{
		struct tccxxx_cif_buffer *buf = &(data->static_buf[data->n_sbufs]);

		INIT_LIST_HEAD(&buf->buf_list);

		//buf->v4lbuf.length = PAGE_ALIGN(cam->pix_format.sizeimage);
		buf->v4lbuf.length = buff_size;
		
		total_req_buf_size += buf->v4lbuf.length;
		//dprintk(KERN_WARNING "<total size is 0x%x / 0x%x>\n", (unsigned int)(buf->v4lbuf.length), (unsigned int)(total_req_buf_size));	
	  		
		buf->mapcount = 0;
		buf->cam = data;
		buf->v4lbuf.index 	= data->n_sbufs;
		buf->v4lbuf.type 	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf->v4lbuf.field 	= V4L2_FIELD_NONE;
		buf->v4lbuf.memory 	= V4L2_MEMORY_MMAP;
		
		/*
		 * Offset: must be 32-bit even on a 64-bit system.	videobuf-dma-sg
		 * just uses the length times the index, but the spec warns
		 * against doing just that - vma merging problems.	So we
		 * leave a gap between each pair of buffers.
		 */
		//buf->v4lbuf.m.offset = 2*index*buf->v4lbuf.length;			
		
	}	

	//data->cif_cfg.pp_num = req->count;
	data->cif_cfg.pp_num = data->n_sbufs;
	req->count = data->cif_cfg.pp_num;


	if(data->cif_cfg.fmt == M420_ZERO)
		uv_offset = (data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y)/2;
	else
		uv_offset = (data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y)/4;
        
	total_off = y_offset + uv_offset*2;
  	total_off = PAGE_ALIGN(total_off);        
	for(i = 0; i<data->cif_cfg.pp_num; i++)
	{
		data->cif_cfg.preview_buf[i].p_Y  = (unsigned int)PAGE_ALIGN( base_addr + total_off*i);
		data->cif_cfg.preview_buf[i].p_Cb = (unsigned int)data->cif_cfg.preview_buf[i].p_Y + y_offset;
		data->cif_cfg.preview_buf[i].p_Cr = (unsigned int)data->cif_cfg.preview_buf[i].p_Cb + uv_offset;
	}	

	dprintk("buffer count = %d \n",data->cif_cfg.pp_num);
	
	return 0;	
}
#endif

int tccxxx_cif_cam_restart(struct v4l2_pix_format *pix, unsigned long xclk)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;
	struct tccxxx_cif_buffer *cif_buf;
	unsigned int stride = 0, y_offset = 0, uv_offset = 0;

	dprintk("%s Start!! \n", __FUNCTION__);

	tccxxx_cif_stop_stream();

	sensor_if_restart();
	sensor_if_change_mode(OPER_PREVIEW);

	while(!list_empty(&data->done_list)) {
		cif_buf = list_entry(data->done_list.next, struct tccxxx_cif_buffer, buf_list);
		list_del(&cif_buf->buf_list);
	
		cif_buf->v4lbuf.flags &= ~V4L2_BUF_FLAG_DONE;	
		cif_buf->v4lbuf.flags |= V4L2_BUF_FLAG_QUEUED;
		
		list_add_tail(&cif_buf->buf_list, &data->list);	
	}

	// Sensor setting Again!! 
	sensor_if_set_current_control();

	cif_dma_hw_reg(0);
	data->stream_state = STREAM_ON;
	cif_interrupt_enable(data->cif_cfg.oper_mode);

	data->cif_cfg.esd_restart = OFF;
	cif_timer_register(data, HZ);

	dprintk("%s End!! \n", __FUNCTION__);

#if defined(CONFIG_USE_ISP)
	#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	ISP_SetPreview_Window(0, 0, tcc_sensor_info.preview_w, tcc_sensor_info.preview_h);
	#else
	ISP_SetPreview_Window(0, 0, PRV_W, PRV_H);
	#endif

	#if defined(FEATURE_ANDROID_ICS)
	stride = ALIGNED_BUFF(data->cif_cfg.main_set.target_x, 16);
	y_offset = stride * data->cif_cfg.main_set.target_y;
	uv_offset = ALIGNED_BUFF((stride/2), 16) * (data->cif_cfg.main_set.target_y/2);
	ISP_SetPreviewH_Size(y_offset, uv_offset, uv_offset);
	if(data->cif_cfg.fmt == M420_ZERO) 	ISP_SetPreviewH_Format(ISP_FORMAT_YUV422);
	else 								ISP_SetPreviewH_Format(ISP_FORMAT_YUV420);
	#else // FEATURE_ANDROID_ICS
	if(data->cif_cfg.fmt == M420_ZERO) {
		ISP_SetPreviewH_Size(data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y, 	\
							 data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y/2, \
							 data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y/2);
		ISP_SetPreviewH_Format(ISP_FORMAT_YUV422);
	} else {
		ISP_SetPreviewH_Size(data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y, 	\
							 data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y/4, \
							 data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y/4);
		ISP_SetPreviewH_Format(ISP_FORMAT_YUV420);		
	}
	#endif // FEATURE_ANDROID_ICS
	printk ("prevSize:(%d,%d)=%x restart \n", data->cif_cfg.main_set.target_x, data->cif_cfg.main_set.target_y, \
											  data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y);

	ISP_SetPreviewH_Resolution(data->cif_cfg.main_set.target_x, data->cif_cfg.main_set.target_y);

	tccxxx_cif_set_effect(current_effect_mode);
	#ifdef TCCISP_GEN_CFG_UPD
	chkpnt = 0;
	#endif
	ISP_SetPreview_Control(ON);
#endif

	return 0;
}

int tccxxx_cif_start_stream(void)
{
	#if	defined(CONFIG_ARCH_TCC892X)
	uint    sc_plug_in0, sc_plug_in1, nCnt;
	unchar	bUseSimpleDeIntl;
	#endif
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;
	unsigned int stride = 0, y_offset = 0, uv_offset = 0;

	dprintk("%s Start!! \n", __FUNCTION__);
	dprintk("cif_cfg.main_set:  src_x = %d, src_y = %d. \n", data->cif_cfg.main_set.source_x, data->cif_cfg.main_set.source_y);
	dprintk("cif_cfg.main_set:  off_x = %d, off_y = %d. \n", data->cif_cfg.main_set.win_hor_ofst, data->cif_cfg.main_set.win_ver_ofst);
	dprintk("cif_cfg.main_set:  scale_x = %d, scale_y = %d. \n", data->cif_cfg.main_set.scaler_x, data->cif_cfg.main_set.scaler_y);
	dprintk("cif_cfg.main_set:  tgt_x = %d, tgt_y = %d. \n", data->cif_cfg.main_set.target_x, data->cif_cfg.main_set.target_y);

	data->cif_cfg.oper_mode  = OPER_PREVIEW;
	data->cif_cfg.cap_status = CAPTURE_NONE;

#if defined(CONFIG_USE_ISP)
	prev_buf = NULL;

	sensor_if_change_mode(OPER_PREVIEW);

	//cif_scaler_calc();
	//cif_scaler_set(NULL, data->cif_cfg.oper_mode); //20101124 ysseung   test code.
	#if (0) //20111209 ysseung   test...
	cif_preview_dma_set();
	#endif

	cif_dma_hw_reg(0);
	
	data->stream_state = STREAM_ON;
		
	cif_interrupt_enable(data->cif_cfg.oper_mode);
	cif_timer_register(data, HZ);

	cif_set_frameskip(0, 0);
	skipped_frm = 0;
	
	while(skip_frm) {
		msleep(1);
	}
	
	#if defined(FEATURE_ANDROID_ICS)
	stride = ALIGNED_BUFF(data->cif_cfg.main_set.target_x, 16);
	y_offset = stride * data->cif_cfg.main_set.target_y;
	uv_offset = ALIGNED_BUFF((stride/2), 16) * (data->cif_cfg.main_set.target_y/2);
	ISP_SetPreviewH_Size(y_offset, uv_offset, uv_offset);
	if(data->cif_cfg.fmt == M420_ZERO) 	ISP_SetPreviewH_Format(ISP_FORMAT_YUV422);
	else 								ISP_SetPreviewH_Format(ISP_FORMAT_YUV420);
	#else // FEATURE_ANDROID_ICS
	if(data->cif_cfg.fmt == M420_ZERO) {
		ISP_SetPreviewH_Size(data->cif_cfg.main_set.target_x * data->cif_cfg.main_set.target_y, 	\
 							 data->cif_cfg.main_set.target_x * data->cif_cfg.main_set.target_y / 2, \
 							 data->cif_cfg.main_set.target_x * data->cif_cfg.main_set.target_y / 2);
		ISP_SetPreviewH_Format(ISP_FORMAT_YUV422);
	} else {
		ISP_SetPreviewH_Size(data->cif_cfg.main_set.target_x * data->cif_cfg.main_set.target_y, 	\
							 data->cif_cfg.main_set.target_x * data->cif_cfg.main_set.target_y / 4, \
							 data->cif_cfg.main_set.target_x * data->cif_cfg.main_set.target_y / 4);
		ISP_SetPreviewH_Format(ISP_FORMAT_YUV420);
	}
	#endif // FEATURE_ANDROID_ICS
	printk("prevSize:(%d, %d) = %x. \n", data->cif_cfg.main_set.target_x, data->cif_cfg.main_set.target_y, \
										 data->cif_cfg.main_set.target_x*data->cif_cfg.main_set.target_y);
	
	ISP_SetPreviewH_Resolution( data->cif_cfg.main_set.target_x, data->cif_cfg.main_set.target_y);
	#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	ISP_SetPreview_MakeZoomTable(tcc_sensor_info.preview_zoom_offset_x, tcc_sensor_info.preview_zoom_offset_y, tcc_sensor_info.preview_w, tcc_sensor_info.preview_h, tcc_sensor_info.max_zoom_step+1);
	#else
	ISP_SetPreview_MakeZoomTable(PRV_ZOFFX, PRV_ZOFFY, PRV_W, PRV_H, CAM_MAX_ZOOM_STEP);
	#endif
	ISP_SetPreview_Zoom(data->cif_cfg.zoom_step);
	
	tccxxx_cif_set_effect(current_effect_mode);
	#ifdef TCCISP_GEN_CFG_UPD
	chkpnt = 0;
	#endif
	ISP_SetPreview_Control(ON);
#else // CONFIG_USE_ISP
	#if	defined(CONFIG_ARCH_TCC892X)
			data->stream_state = STREAM_ON;
			sc_plug_in0 = VIOC_SC_VIN_00; 	// VIOC_SC_WDMA_05;
		    sc_plug_in1 = VIOC_SC_WDMA_05; 	// VIOC_SC_WDMA_07;

			cif_set_port(4);

			skipped_frm 	= 0;
			old_zoom_step 	= 0;
			prev_buf 		= NULL;

		#if defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150)
			frm_cnt 	= 0;
			bfield 		= 0;
			field_cnt 	= 0;
			bUseSimpleDeIntl = OFF; // if you use DEINTL_S, you have to change below variable.

			dprintk("TVP5150 start!!\n");
			sensor_if_change_mode(OPER_PREVIEW);
			tccxxx_vioc_vin_main(pVINBase);
			if(!bUseSimpleDeIntl) 	tccxxx_vioc_viqe_main(ON);
			tccxxx_vioc_viqe_wmix_set(pWMIXBase, VIOC_VIQE_VIN_00, bUseSimpleDeIntl);
			tccxxx_vioc_scaler_set(pSCBase, pVINBase, pWMIXBase, sc_plug_in1);
			tccxxx_vioc_vin_wdma_set(pWDMABase);
		#else // CONFIG_VIDEO_ATV_SENSOR_TVP5150
			sensor_if_change_mode(OPER_PREVIEW);
			tccxxx_vioc_vin_main(pVINBase);
			tccxxx_vioc_scaler_set(pSCBase, pVINBase, pWMIXBase, sc_plug_in0);
			tccxxx_vioc_vin_wdma_set(pWDMABase);
		#endif // CONFIG_VIDEO_ATV_SENSOR_TVP5150
	#else // CONFIG_ARCH_TCC892X
		TDD_CIF_ONOFF(OFF);
		TDD_CIF_SWReset();
		TDD_CIF_SetCaptureCtrl(SET_CIF_SKIP_NUM, 0, 0, SET_CIF_CAP_DISABLE); // after capture!!
		
		sensor_if_change_mode(OPER_PREVIEW);
		#if !defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150)
			TDD_CIF_SetInterrupt(SET_CIF_UPDATE_IN_VSYNC);
		#endif
		cif_global_reset();

		tccxxx_cif_set_effect(current_effect_mode);
		
		cif_scaler_calc();
		cif_preview_dma_set();
		cif_dma_hw_reg(0);

		#if defined(CONFIG_VIDEO_CAMERA_SENSOR_OV7690)
			TDD_CIF_SetEffectMode(SET_CIF_CEM_YCS);
		#endif
		
		TDD_CIF_ONOFF(ON);

		data->stream_state = STREAM_ON;
			
		cif_interrupt_enable(data->cif_cfg.oper_mode);
		cif_timer_register(data, HZ);

		#if defined(CONFIG_VIDEO_CAMERA_SENSOR_GT2005)
			cif_set_frameskip(35, 0);		// Temporary Setting!! By Mun
		#else
			cif_set_frameskip(0, 0);
		#endif

		skipped_frm = 0;

		while(skip_frm) {
			msleep(1);
		}
	#endif // CONFIG_ARCH_TCC892X
#endif // CONFIG_USE_ISP

	dprintk("%s End!! \n", __FUNCTION__);
	return 0;
}

int tccxxx_cif_stop_stream(void)
{	
	int nCnt;
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;

	dprintk("%s Start!! \n", __FUNCTION__);

	//spin_lock_irq(&data->dev_lock);
	mutex_lock(&data->lock);
#if defined(CONFIG_USE_ISP)
	ISP_SetPreview_Control(OFF);
	cif_timer_deregister();
	cif_interrupt_disable();
#elif defined(CONFIG_ARCH_TCC892X)
	VIOC_WDMA_SetIreqMask(pWDMABase, VIOC_WDMA_IREQ_ALL_MASK, 0x1); // disable WDMA interrupt

	BITCSET(pWDMABase->uCTRL.nREG, 1<<28, 0<<28); // disable WDMA
	BITCSET(pWDMABase->uCTRL.nREG, 1<<16, 1<<16); // update WDMA
	BITCSET(pWDMABase->uIRQSTS.nREG, 1<<5, 1<<5); // clear EORF bit

	// Before camera quit, we have to wait WMDA's EOFR signal to LOW.
	while(!(pWDMABase->uIRQSTS.nREG & VIOC_WDMA_IREQ_EOFR_MASK)) {
		msleep(1);
		if((cam_loop_lmt_cnt > CAMERA_LOOP_LIMIT_COUNT) || (cam_no_connect_cnt > 3)) {
			VIOC_WDMA_SWReset(pVIOCConfig, 0x5/* WDMA05 */);
			printk("cam_loop_lmt_cnt = %d, cam_no_connect_cnt = %d. \n", cam_loop_lmt_cnt, cam_no_connect_cnt);
			cam_loop_lmt_cnt = cam_no_connect_cnt = 0;
			break;
		}
	}

	VIOC_VIN_SetEnable(pVINBase, OFF); // disable VIN

	#if defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150)
	VIOC_CONFIG_PlugOut(VIOC_VIQE);
	//VIOC_CONFIG_PlugOut(VIOC_DEINTLS);
	#endif
#else
	cif_timer_deregister();
	cif_interrupt_disable();
#endif
	//spin_unlock_irq(&data->dev_lock);
	mutex_unlock(&data->lock);

	data->stream_state = STREAM_OFF;	
	dprintk("\n\n SKIPPED FRAME = %d \n\n", skipped_frm);

	dprintk("%s End!! \n", __FUNCTION__);

	return 0;
}

void tccxxx_cif_set_overlay(void)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;
	cif_SuperImpose *SiData = (cif_SuperImpose *)&(data->cif_cfg.si_overlay);

	unsigned int frame_width = data->cif_cfg.main_set.scaler_x;  // Scaler_out width
	unsigned int frame_height = data->cif_cfg.main_set.scaler_y;  // Scaler_out width
	unsigned int siFrame_hoffset_s 	= 0;
	unsigned int siFrame_hoffset_e 		= 0;   
	unsigned int siFrame_voffset_s 	= 0;
	unsigned int siFrame_voffset_e 		= 0;
	unsigned int si_buffer_addr 	= data->cif_buf.addr + SiData->buff_offset;

	siFrame_hoffset_s = siFrame_hoffset_e = (frame_width - SiData->width)/2;
	siFrame_voffset_s = siFrame_voffset_e = (frame_height - SiData->height)/2;

#if defined(CONFIG_USE_ISP)
	// todo : 
#else
	TDD_CIF_OverlayCtrl(SET_CIF_OVERLAY_DISABLE, OL_RGB_565);

	if(SiData->buff_offset != 0)
	{
		TDD_CIF_SetImage(OVERLAY_IMG, frame_width, frame_height,
						siFrame_hoffset_s, siFrame_hoffset_e,	siFrame_voffset_s, siFrame_voffset_e);
		TDD_CIF_SetOverlay (SET_CIF_OVERLAY_ALL, 49, FULL_OVERLAY, OP_ALPHA, OP_ALPHA, ALPHA100, ALPHA100);
		TDD_CIF_SetOverlayKey(SET_CIF_OVERLAYKEY_ALL, SiData->chromakey_info.key_y, 
													  SiData->chromakey_info.key_u, 
													  SiData->chromakey_info.key_v, 
													  SiData->chromakey_info.mask_r, 
													  SiData->chromakey_info.mask_g, 
													  SiData->chromakey_info.mask_b);
		
		TDD_CIF_OverlayCtrl(SET_CIF_CHROMA_ENABLE|SET_CIF_OVERLAY_ENABLE|SET_CIF_COLOR_CONV_ENABLE|
						SET_CIF_COLOR_MODE_RGB,	OL_RGB_565);
		TDD_CIF_SetBaseAddr(OVERLAY_IMG, (unsigned int)si_buffer_addr, 0, 0);				
	}
#endif	
}

int tccxxx_cif_set_effect (u8 nCameraEffect)
{
	unsigned int uiSepiaU = 0x64;
	unsigned int uiSepiaV = 0x8C;
	unsigned int uiSKCoeff0 = 0x02;
	unsigned int uiSKCoeff1 = 0x0;
	unsigned int uiSKCoeff2 = 0xff;
	unsigned int uiSkecthThreshold = 0x80;

#ifdef CONFIG_USE_ISP
	ISP_SetImg_Effect(nCameraEffect);
#else
	if (nCameraEffect == TCC_EFFECT_NORMAL)
		TDD_CIF_SetEffectMode(SET_CIF_CEM_NOR);
	else if(nCameraEffect == TCC_EFFECT_GRAY)
		TDD_CIF_SetEffectMode(SET_CIF_CEM_GRAY);
	else if(nCameraEffect == TCC_EFFECT_NEGATIVE)
		TDD_CIF_SetEffectMode(SET_CIF_CEM_NEGA);
	else if(nCameraEffect == TCC_EFFECT_SKETCH)
	{
		uiSkecthThreshold = 0x89;
		TDD_CIF_SetEffectMode(SET_CIF_CEM_SKT);
		TDD_CIF_SetEffectHFilterCoeff(uiSKCoeff0, uiSKCoeff1, uiSKCoeff2);
		TDD_CIF_SetEffectSketchTh(uiSkecthThreshold);
	}
	else if(nCameraEffect == TCC_EFFECT_SEPHIA)
	{
		uiSepiaU =  0x64;
		uiSepiaV = 0x8C;
		TDD_CIF_SetEffectMode(SET_CIF_CEM_SEPI);
		TDD_CIF_SetEffectSepiaUV(uiSepiaU, uiSepiaV);
	}
	else if(nCameraEffect == TCC_EFFECT_GREEN)
	{		
		uiSepiaU =  100;
		uiSepiaV = 80;
		TDD_CIF_SetEffectMode(SET_CIF_CEM_SEPI);
		TDD_CIF_SetEffectSepiaUV(uiSepiaU, uiSepiaV);

	}
	else if(nCameraEffect == TCC_EFFECT_AQUA)
	{
		uiSepiaU =  192;
		uiSepiaV = 80;
		TDD_CIF_SetEffectMode(SET_CIF_CEM_SEPI);
		TDD_CIF_SetEffectSepiaUV(uiSepiaU, uiSepiaV);

	}
#endif	
	current_effect_mode = (u8) nCameraEffect;

	return 0;
}

int tccxxx_cif_capture(int quality)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;
	unsigned int target_width, target_height;
	unsigned int ens_addr;
	int skip_frame = 0, nCnt;
#if	defined(CONFIG_ARCH_TCC892X)
	uint sc_plug_in0;
#endif

	dprintk("%s Start!! \n", __FUNCTION__);
#if defined(CONFIG_USE_ISP)
	int mode=0;

	data->cif_cfg.oper_mode = OPER_CAPTURE;

	memset(&(data->cif_cfg.jpg_info), 0x00, sizeof(TCCXXX_JPEG_ENC_DATA));

	target_width  = data->cif_cfg.main_set.target_x;
	target_height = data->cif_cfg.main_set.target_y;

	#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	if(target_width >= tcc_sensor_info.cam_capchg_width && !(data->cif_cfg.retry_cnt))
	#else
	if(target_width >= CAM_CAPCHG_WIDTH && !(data->cif_cfg.retry_cnt))
	#endif
	{
		#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T113)
		unsigned char data_r[2];
		unsigned short data_read;
		unsigned char polling_reg[2] = {0x09, 0x8E};
		unsigned char polling_cmd[2] = {0x30, 0x00};
		unsigned short read_reg = 0x0990;

		do {
			DDI_I2C_Write(polling_reg, 2, 0);
			DDI_I2C_Write(polling_cmd, 2, 0);
			DDI_I2C_Read(read_reg, 2, data_r, 2);

			data_read = data_r[0];
			data_read = (data_read<<8)&0xff00 | data_r[1];
			printk("MT9T113 Autofocus 0[0x%x] 1[0x%x] data_read = 0x%x \n",data_r[0], data_r[1], data_read);

			if(data_read == 0x0001) 	break;
			else 						msleep(10);
		} while(data_read != 0x0001);
		#endif
		
		sensor_if_change_mode(OPER_CAPTURE);
		msleep(300); //20101125 ysseung   test code.
		mode = 1;
	}

	//cif_scaler_calc();
	//cif_scaler_set(NULL, data->cif_cfg.oper_mode); //20101124 ysseung   test code.
	cif_capture_dma_set(data);

	// for Rotate Capture
	data->cif_cfg.jpg_info.start_phy_addr = data->cif_cfg.base_buf;

	cif_interrupt_enable(data->cif_cfg.oper_mode);

	if(data->cif_cfg.si_overlay.buff_offset != 0)
		tccxxx_cif_set_overlay();

	data->cif_cfg.cap_status = CAPTURE_NONE;

	#ifdef TCCISP_GEN_CFG_UPD
	chkpnt = 0;
	#endif

	// Turn on Camera Flash
	sensor_turn_on_camera_flash();

	ISP_Capture_Shooting(mode);
	ISP_SetPreview_Control(OFF);
#else // CONFIG_USE_ISP
	#if	defined(CONFIG_ARCH_TCC892X)
	sc_plug_in0 = VIOC_SC_VIN_00; // VIOC_SC_WDMA_05;
	data->cif_cfg.oper_mode = OPER_CAPTURE;
	memset(&(data->cif_cfg.jpg_info), 0x00, sizeof(TCCXXX_JPEG_ENC_DATA));

	target_width  = data->cif_cfg.main_set.target_x;
	target_height = data->cif_cfg.main_set.target_y;
	#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	if(target_width >= tcc_sensor_info.cam_capchg_width && !(data->cif_cfg.retry_cnt)) {
		sensor_if_change_mode(OPER_CAPTURE);
		//msleep(300);
	}
	#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	if(target_width >= CAM_CAPCHG_WIDTH && !(data->cif_cfg.retry_cnt)) {
		dprintk("Capture Mode!!\n");
		sensor_if_change_mode(OPER_CAPTURE);
		//msleep(300);
	}
	#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT

	// for Rotate Capture
	data->cif_cfg.jpg_info.start_phy_addr = data->cif_cfg.base_buf;
	//capture config
	if(data->cif_cfg.retry_cnt) 	{
		skip_frame = 0;
	} else {
		#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
		skip_frame = tcc_sensor_info.capture_skip_frame;
		#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
		skip_frame = FRAMESKIP_COUNT_FOR_CAPTURE;
		#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	}

	cif_set_frameskip(skip_frame, 0);	
	tccxxx_vioc_vin_main(pVINBase);
	tccxxx_vioc_scaler_set(pSCBase, pVINBase, pWMIXBase, sc_plug_in0);
	tccxxx_vioc_vin_wdma_set(pWDMABase);
	data->cif_cfg.cap_status = CAPTURE_NONE;
	#else // CONFIG_ARCH_TCC892X
	cif_interrupt_disable();
	CIF_OpStop(1, 1);
	
	data->cif_cfg.oper_mode = OPER_CAPTURE;

	cif_global_reset();
	tccxxx_cif_set_effect(current_effect_mode);

	memset(&(data->cif_cfg.jpg_info), 0x00, sizeof(TCCXXX_JPEG_ENC_DATA));
	
#if defined(CONFIG_VIDEO_CAMERA_SENSOR_AIT848_ISP)
	sensor_if_capture_config(data->cif_cfg.main_set.target_x, data->cif_cfg.main_set.target_y);
	target_width  = 1536;
	target_height = (gnSizeOfAITALLJPEG+1535)/1536;

	data->cif_cfg.jpg_info.start_phy_addr	= PA_JPEG_RAW_BASE_ADDR;
	data->cif_cfg.jpg_info.bitstream_offset = 2;
	data->cif_cfg.jpg_info.thumb_offset 	= gnoffsetOfAITThumbnail;
	data->cif_cfg.jpg_info.header_offset 	= gnSizeOfAITALLJPEG;
	data->cif_cfg.jpg_info.bitstream_size 	= gnSizeOfAITMainJPEG;
	data->cif_cfg.jpg_info.thumb_size 		= gnSizeOfAITThumbnail;
	data->cif_cfg.jpg_info.header_size 		= 0;
#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_ISX006)
	sensor_if_capture_config(data->cif_cfg.main_set.target_x,data->cif_cfg.main_set.target_y);

	target_width  = CAP_W;
	target_height = CAP_H;

	data->cif_cfg.jpg_info.start_phy_addr	= PA_PREVIEW_BASE_ADDR;
	data->cif_cfg.jpg_info.bitstream_offset = 0;
	data->cif_cfg.jpg_info.thumb_offset 	= 0;
	data->cif_cfg.jpg_info.header_offset	= 0;
	data->cif_cfg.jpg_info.bitstream_size	= 0;
	data->cif_cfg.jpg_info.thumb_size		= 0;
	data->cif_cfg.jpg_info.header_size		= 0;
	#else
	target_width  = data->cif_cfg.main_set.target_x;
	target_height = data->cif_cfg.main_set.target_y;
	#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	if(target_width >= tcc_sensor_info.cam_capchg_width && !(data->cif_cfg.retry_cnt)){
		sensor_if_change_mode(OPER_CAPTURE);
		msleep(300); 
	}
	#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	if(target_width >= CAM_CAPCHG_WIDTH && !(data->cif_cfg.retry_cnt)){
		sensor_if_change_mode(OPER_CAPTURE);
		msleep(300); 
	}
	#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT	
#endif
	cif_scaler_calc();

	#ifdef JPEG_ENCODE_WITH_CAPTURE
	gIsRolling = tccxxx_jpeg_encode_method();
	#endif
	cif_capture_dma_set(data);

	// for Rotate Capture
	data->cif_cfg.jpg_info.start_phy_addr = data->cif_cfg.base_buf;
	//capture config
	if(data->cif_cfg.retry_cnt) {
		#if defined(CONFIG_VIDEO_CAMERA_SENSOR_ISX006)
		sensor_if_change_mode(OPER_PREVIEW);
		msleep(100);
		#endif
		skip_frame = 0;
	} else {
		#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
		skip_frame = tcc_sensor_info.capture_skip_frame;
		#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
		skip_frame = FRAMESKIP_COUNT_FOR_CAPTURE;
		#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	}
	
	#ifdef JPEG_ENCODE_WITH_CAPTURE
	if(gIsRolling) {
		TDD_CIF_SetInterrupt(SET_CIF_UPDATE_WITHOUT_VSYNC);
		TDD_CIF_SetCaptureCtrl(SET_CIF_SKIP_NUM|SET_CIF_VCNT_NUM, skip_frame, FRAME_LINE_CNT/16,
								SET_CIF_EIT_ENC_INT|SET_CIF_RLV_ENABLE|SET_CIF_RLU_ENABLE|SET_CIF_RLY_ENABLE|
								SET_CIF_CAP_ENABLE|SET_CIF_VEN_ENABLE);
	}
	else
	#endif
	{
		#if defined(CONFIG_VIDEO_CAMERA_SENSOR_AIT848_ISP) || defined(CONFIG_VIDEO_CAMERA_SENSOR_ISX006)
		ens_addr = (unsigned int)data->cif_cfg.capture_buf.p_Y + (target_width * (target_height - 100));
		#else
		ens_addr = (unsigned int)data->cif_cfg.capture_buf.p_Y + (target_width * (target_height - 2));
		#endif

		TDD_CIF_SetCaptureCtrl(SET_CIF_SKIP_NUM, skip_frame, 0,SET_CIF_EIT_ENC_INT|SET_CIF_CAP_ENABLE|SET_CIF_UES_ENABLE);		
		TDD_CIF_SetBaseAddr(IN_ENC_START_ADDR, ens_addr, 0, 0);
	}

	cif_interrupt_enable(data->cif_cfg.oper_mode);

	#ifdef JPEG_ENCODE_WITH_CAPTURE
	tccxxx_jpeg_encode_init(quality);
	#endif

	if(data->cif_cfg.si_overlay.buff_offset != 0) 	tccxxx_cif_set_overlay();

	data->cif_cfg.cap_status = CAPTURE_NONE;
	#if defined(CONFIG_VIDEO_CAMERA_SENSOR_AIT848_ISP) || defined(CONFIG_VIDEO_CAMERA_SENSOR_ISX006)
	sensor_if_change_mode(OPER_CAPTURE);
	#endif

	// Turn on Camera Flash
	sensor_turn_on_camera_flash();

	TDD_CIF_ONOFF(ON);
	msleep(100);
	#endif // CONFIG_ARCH_TCC892X
#endif // CONFIG_USE_ISP
	dprintk("%s End!! \n", __FUNCTION__);

	return 0;
}
		

int tccxxx_cif_set_zoom(unsigned char arg)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;

	data->cif_cfg.zoom_step = arg;

	#if defined(CONFIG_USE_ISP)
	ISP_SetPreview_Zoom(data->cif_cfg.zoom_step);
	#else // CONFIG_USE_ISP
	if(data->stream_state != STREAM_OFF) {
		dprintk("zoom level = %d. \n", data->cif_cfg.zoom_step);
		TDD_CIF_ONOFF(OFF);
		cif_scaler_calc();
		TDD_CIF_ONOFF(ON);
	}
	#endif // CONFIG_USE_ISP

	return 0;
}

int tccxxx_cif_set_resolution(unsigned int pixel_fmt, unsigned short width, unsigned short height)
{
	struct TCCxxxCIF *data = &hardware_data;

	if(pixel_fmt == V4L2_PIX_FMT_YUYV)  data->cif_cfg.fmt = M420_ZERO;  // yuv422
	else 								data->cif_cfg.fmt = M420_ODD; 	// yuv420

	data->cif_cfg.main_set.target_x = width;
	data->cif_cfg.main_set.target_y = height;

	if(data->stream_state != STREAM_OFF) {
		tccxxx_cif_stop_stream();
		tccxxx_cif_start_stream();
	}

	return 0;
}

int tccxxx_cif_open(void)
{
	int ret;
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;

	data->done_list.next = &data->done_list;
	data->list.next = &data->list;

#if defined(CONFIG_USE_ISP)
	if(!cam_irq) {
		if((ret = request_irq(IRQ_ISP1, isp_cam_isr1, IRQF_DISABLED, "isp2", NULL)) < 0) {
			printk("FAILED to aquire camera-irq\n");
			return ret;
		}

		if((ret = request_irq(IRQ_ISP2, isp_cam_isr2, IRQF_DISABLED, "isp3", NULL)) < 0) {
			printk("FAILED to aquire camera-irq\n");
			return ret;
		}
		cam_irq = 1;
	}
#else // CONFIG_USE_ISP
	if(!cam_irq) {
		#if defined(CONFIG_ARCH_TCC88XX)
			if((ret = request_irq(IRQ_CIF, cif_cam_isr, IRQF_DISABLED, "camera", NULL)) < 0)
		#elif defined(CONFIG_ARCH_TCC892X)
			if((ret = request_irq(INT_VIOC_WD5, cif_cam_isr_in8920, IRQF_DISABLED /*IRQF_SHARED*/, "camera", NULL /*cif_cam_isr_in8920*/)) < 0)
		#else
			if((ret = request_irq(IRQ_CAM, cif_cam_isr, IRQF_DISABLED, "camera", NULL)) < 0)
		#endif
		{
			printk("FAILED to aquire camera-irq\n");
			return ret;
		}
		cam_irq = 1;
	}
	
	#if defined(JPEG_ENCODE_WITH_CAPTURE) && !defined(NLY_ENCODE_JPEG_IN_ROLLING_CAPTURE)
	free_irq(IRQ_JPGE, NULL);
	if((ret = request_irq(IRQ_JPE, tccxxx_jpeg_handler, IRQF_DISABLED, "jpeg", NULL)) < 0) {
		printk("FAILED to aquire irq\n");
		return ret;
	}
	#endif // defined(JPEG_ENCODE_WITH_CAPTURE) && !defined(NLY_ENCODE_JPEG_IN_ROLLING_CAPTURE)
#endif //CONFIG_USE_ISP

	return 0;
}

int tccxxx_cif_close(void)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;	

/*
	int i;

	mutex_lock(&data->lock);	

	INIT_LIST_HEAD(&data->done_list);
	INIT_LIST_HEAD(&data->list);
	for(i=0; i < data->cif_cfg.pp_num; i++) {
		INIT_LIST_HEAD(&data->buf[i].buf_list);
		data->buf[i].v4lbuf.flags &= ~V4L2_BUF_FLAG_QUEUED;
		data->buf[i].v4lbuf.flags &= ~V4L2_BUF_FLAG_DONE;
	}

	mutex_unlock(&data->lock);	
*/

#if defined(CONFIG_USE_ISP)
	ISP_Exit();
#endif

	mutex_destroy(&data->lock);
	cif_interrupt_disable();
	cif_cleanup();
	dprintk("camera close reamp : [0x%x - 0x%x] -> [0x%x] \n",data->cif_buf.addr, data->cif_buf.bytes, (unsigned int)data->cif_buf.area);

	if((cam_open == 1) && (cam_close == 0)) {
		if(data->cif_buf.area != NULL) iounmap(data->cif_buf.area);
		reg_buf   = NULL;
		cam_open  = 0;
		cam_close = 1;
	}

	if(cam_irq) {
	#if defined(CONFIG_USE_ISP)
		//free_irq(IRQ_ISP0, NULL);
		free_irq(IRQ_ISP1, NULL);
		free_irq(IRQ_ISP2, NULL);
		//free_irq(IRQ_ISP3, NULL);
	#else // CONFIG_USE_ISP
		#if defined(CONFIG_ARCH_TCC88XX)
		free_irq(IRQ_CIF, NULL);
		#elif defined(CONFIG_ARCH_TCC892X)
		free_irq(INT_VIOC_WD5, NULL /*cif_cam_isr_in8920*/);
		#else
		free_irq(IRQ_CAM, NULL);
		#endif
	#endif // CONFIG_USE_ISP
	}
	cam_irq = 0;
	
#ifdef JPEG_ENCODE_WITH_CAPTURE
	free_irq(IRQ_JPGE, NULL);
#endif

	if(register_timer) {
		cif_timer_deregister();
	}

	return 0;
}

#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
int tcc_get_sensor_info(int index)
{
	CameraID = index;
	if(CameraID)
		sensor_if_set_facing_front();
	else
		sensor_if_set_facing_back();
}

int tccxxx_sensor_init(void * priv)
{
	 TCC_SENSOR_INFO_TYPE *data = (struct TCC_SENSOR_INFO_TYPE *) priv;
}
#endif

/* Initialise the OMAP camera interface */
int  tccxxx_cif_init(void)
{
	struct cif_dma_buffer *buf = &hardware_data.cif_buf;    

	pmap_get_info("ump_reserved", &pmap_ump_reserved);
	pmap_get_info("jpeg_raw", &pmap_jpeg_raw);

	memset(&hardware_data,0x00,sizeof(struct TCCxxxCIF));
	hardware_data.buf = hardware_data.static_buf;


#ifdef JPEG_ENCODE_WITH_CAPTURE
	buf->bytes = PAGE_ALIGN(CAPTURE_MEM+JPEG_MEM);
	buf->addr = PA_VIDEO_BASE;
	buf->area = ioremap_nocache(buf->addr,buf->bytes);

	jpeg_remapped_base_address = (void *)ioremap_nocache(JPEG_PHY_ADDRESS, PAGE_ALIGN(JPEG_MEM-PAGE_SIZE));    //JPEG Buffer
	jpeg_header_remapped_address = jpeg_remapped_base_address + JPEG_STREAM;

	gJPEG_Buffer_Info.pBaseRawDataAddr = (void*)CAPTURE_PHY_ADDRESS;
	gJPEG_Buffer_Info.pBaseRawDataSize = CAPTURE_MEM;
	gJPEG_Buffer_Info.pBaseBitstreamDataAddr = (void*)JPEG_PHY_ADDRESS;
	gJPEG_Buffer_Info.pBaseBitstreamDataSize = JPEG_STREAM;
	gJPEG_Buffer_Info.pBaseHeaderDataAddr = (void*)(JPEG_PHY_ADDRESS + JPEG_STREAM);
	gJPEG_Buffer_Info.pBaseHeaderDataSize = JPEG_HEADER;

	if (buf->area == NULL) 
	{
		printk(KERN_ERR CAM_NAME ": cannot remap buffer\n");
		return ENODEV;
	}
	
#else // JPEG_ENCODE_WITH_CAPTURE
	printk("tccxxx_cif_init cam_open = %d, cam_close = %d\n", cam_open, cam_close);

	if((cam_open == 1) && (cam_close ==0)) {
		if(reg_buf != NULL) 	iounmap(reg_buf);
		reg_buf   = NULL;
		cam_open  = 0;
		cam_close = 1;
	}

	if((cam_open == 0) && (cam_close == 1)) {
		buf->bytes = PAGE_ALIGN(PREVIEW_MEM_SIZE);
		buf->addr = PA_PREVIEW_BASE_ADDR;
		buf->area = ioremap_nocache(buf->addr, buf->bytes);
		reg_buf = buf->area;

		dprintk("reamp : [0x%x - 0x%x] -> [0x%x] \n",buf->addr, buf->bytes, (unsigned int)buf->area);

		if(buf->area == NULL) {
			printk(KERN_ERR CAM_NAME ": cannot remap buffer\n");
			return ENODEV;
		}

		cam_open  = 1;
		cam_close = 0;
	}
#endif // JPEG_ENCODE_WITH_CAPTURE

#if defined(CONFIG_ARCH_TCC892X)
	pVINDemuxBase = (VIOC_VIN_DEMUX*)tcc_p2v(HwVIOC_VINDEMUX);
	pWMIXBase = (VIOC_WMIX *)tcc_p2v(HwVIOC_WMIX5);
	pWDMABase = (VIOC_WDMA *)tcc_p2v(HwVIOC_WDMA05);
	pVINBase = (VIOC_VIN *)tcc_p2v(HwVIOC_VIN00);
	pSCBase = (VIOC_SC *)tcc_p2v(HwVIOC_SC0);
	pDDIConfig = (DDICONFIG *)tcc_p2v(HwDDI_CONFIG_BASE);
	pVIOCConfig = (VIOC_IREQ_CONFIG *)tcc_p2v(HwVIOC_IREQ);
	pVIQEBase = (VIQE *)tcc_p2v(HwVIOC_VIQE0);

	BITCSET(pVIOCConfig->uSOFTRESET.nREG[1], (1<<5), (1<<5)); 		// WDMA5 reset
	BITCSET(pVIOCConfig->uSOFTRESET.nREG[0], (1<<28), (1<<28)); 	// SCALER0 reset
	BITCSET(pVIOCConfig->uSOFTRESET.nREG[1], (1<<14), (1<<14)); 	// WMIX5 reset
	BITCSET(pVIOCConfig->uSOFTRESET.nREG[0], (1<<24), (1<<24)); 	// VIN0 reset
	BITCSET(pVIOCConfig->uSOFTRESET.nREG[1], (1<<16), (1<<16)); 	// VIQE0 reset

	BITCSET(pVIOCConfig->uSOFTRESET.nREG[1], (1<<16), (0<<16)); 	// VIQE0 reset clear
	BITCSET(pVIOCConfig->uSOFTRESET.nREG[0], (1<<24), (0<<24)); 	// VIN0 reset clear
	BITCSET(pVIOCConfig->uSOFTRESET.nREG[1], (1<<14), (0<<14)); 	// WMIX5 reset clear
	BITCSET(pVIOCConfig->uSOFTRESET.nREG[0], (1<<28), (0<<28)); 	// SCALER0 reset clear
	BITCSET(pVIOCConfig->uSOFTRESET.nREG[1], (1<<5), (0<<5)); 		// WDMA5 reset clear
#endif
  
	/* Init the camera IF */
	cif_data_init((void*)&hardware_data);

	/* enable it. This is needed for sensor detection */
	cif_enable((void*)&hardware_data);

	init_waitqueue_head(&hardware_data.frame_wait);
	spin_lock_init(&hardware_data.dev_lock);

	INIT_LIST_HEAD(&hardware_data.list);
	INIT_LIST_HEAD(&hardware_data.done_list);	
	mutex_init(&hardware_data.lock);

	return 0;
}

