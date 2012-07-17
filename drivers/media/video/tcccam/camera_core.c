/*
 * drivers/media/video/tcccam/camera_core.c
 *
 * Copyright (C) 2008 Telechips, Inc. 
 *
 * Video-for-Linux (Version 2) camera capture driver for
 * the OMAP H2 and H3 camera controller.
 *
 * Adapted from omap24xx driver written by Andy Lowe (source@mvista.com)
 * Copyright (C) 2003-2004 MontaVista Software, Inc.
 * 
 * This package is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
 * 
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED 
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. 
 *
 * History:
 *   
 */
 
#include <generated/autoconf.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/ctype.h>
#include <linux/pagemap.h>
#include <linux/mm.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/videodev2.h>
#include <linux/pci.h>
#include <linux/version.h>
#include <linux/semaphore.h>
#include <asm/processor.h>
#include <linux/dma-mapping.h>
#include <linux/fb.h>
#include <media/v4l2-common.h>
#include <media/v4l2-ioctl.h>

#include <asm/io.h>
#include <asm/byteorder.h>
#include <asm/irq.h>

#include "sensor_if.h"
#include "camera_hw_if.h"
#include "camera_core.h"
#include "tcc_cam.h"
#include "tdd_cif.h"

#include <linux/clk.h>
#include <linux/cpufreq.h>

#include <linux/jiffies.h>


extern unsigned long volatile __jiffy_data jiffies;
extern int tcc_is_camera_enable;

static int debug	   = 0;
#define dprintk(msg...)	if(debug) { printk( "Camera_core: " msg); }


#ifdef JPEG_ENCODE_WITH_CAPTURE
#include "tcc83xx_jpeg.h"
#endif

#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
#include <mach/bsp.h>
#elif defined(CONFIG_ARCH_TCC79X)
#include <mach/tcc79x.h>
#endif

#define DRIVER_VERSION "v1.0"
#define DRIVER_AUTHOR  "Telechips.Co.Ltd"
#define DRIVER_DESC    "TCC CAMERA driver"

#if defined(CONFIG_USE_ISP)
static struct clk *isp_clk;
#elif	defined(CONFIG_ARCH_TCC892X)
static struct clk *vioc_clk;
#else
static struct clk *cif_clk;
#endif

extern struct TCCxxxCIF hardware_data;
struct v4l2_framebuffer fbuf;
static struct video_device *vfd;
static struct v4l2_pix_format pix;
static unsigned long xclk;
#ifdef CONFIG_HAS_EARLYSUSPEND
    static struct early_suspend early_suspend;
#endif	

//int tmp_i2c_add(void);
//void tmp_i2c_del(void);

//static void camera_core_sgdma_process(struct tcc78xx_camera_device *cam);

/* module parameters */
static int video_nr = -1;	/* video device minor (-1 ==> auto assign) */

/* Maximum amount of memory to use for capture buffers.
 * Default is 4800KB, enough to double-buffer SXGA.
 */
static int capture_mem = 1280*960*2*2;

/*Size of video overlay framebuffer. This determines the maximum image size
 *that can be previewed. Default is 600KB, enough for sxga.
 */
//static int overlay_mem = 320*240*2*2;

#define NUM_FREQS 3
static unsigned int out_width, out_height;
extern int cam_no_connect_cnt; //20120404 ysseung   if the camera is not connected, modify to an infinite loop issue of camera stop.

static unsigned int gtCamSizeTable[NUM_FREQS] = {
	 720 *  480, // D1
	1280 *  720, // HD - 720P
	1920 * 1080  // FHD - 1080p
};

#if defined(CONFIG_CPU_FREQ)
extern struct tcc_freq_table_t gtCameraClockLimitTable[];
extern struct tcc_freq_table_t gtISPCameraClockLimitTable[];
#endif

static int Get_Index_sizeTable(unsigned int Image_Size)
{
	int i;

	for(i=0; i < NUM_FREQS; i++) {
		if(gtCamSizeTable[i] >= Image_Size) {
			return i;
		}
	}

	return (NUM_FREQS -1);
}

#if 0
/* -------------------overlay routines ------------------------------*/
/* callback routine for overlay DMA completion. We just start another DMA
 * transfer unless overlay has been turned off
 */

static void
camera_core_overlay_callback(void *arg1, void *arg)
{
	struct camera_device *cam = (struct camera_device *)arg1;
	int err;
	unsigned long irqflags;
	int i, j;
	int count, index;
	unsigned char *fb_buf = phys_to_virt((unsigned long)camera_dev->fbuf.base);

	spin_lock_irqsave(&cam->overlay_lock, irqflags);

	if (!cam->previewing || cam->overlay_cnt == 0) {
		spin_unlock_irqrestore(&cam->overlay_lock, irqflags);
		return;
	}

	--cam->overlay_cnt;
	sg_dma_address(&cam->overlay_sglist) = cam->overlay_base_phys;
	sg_dma_len(&cam->overlay_sglist) = cam->pix.sizeimage;

	count = 0;
	j = ((cam->pix.width - 1) * cam->fbuf.fmt.bytesperline);
	for (i = 0 ; i < cam->pix.sizeimage; i += cam->pix.bytesperline) {
		for (index = 0; index < cam->pix.bytesperline; index++) {
			fb_buf[j] = *(((unsigned char *) cam->overlay_base) +
								 i + index);
			index++;
			fb_buf[j + 1] = *(((unsigned char *) cam->overlay_base) + i + index);
			j = j - cam->fbuf.fmt.bytesperline;
		}
		count += 2;
		j = ((cam->pix.width - 1) * cam->fbuf.fmt.bytesperline) + count;
	}

	while (cam->overlay_cnt < 2) {
		err = camera_core_sgdma_queue(cam, &cam->overlay_sglist, 1,
			camera_core_overlay_callback, NULL);
		if (err)
			break;
		++cam->overlay_cnt;
	}

	spin_unlock_irqrestore(&cam->overlay_lock, irqflags);

}

 
static void
camera_core_start_overlay(struct camera_device *cam)
{
	int err;
	unsigned long irqflags;

	if (!cam->previewing) 
		return;

	spin_lock_irqsave(&cam->overlay_lock, irqflags);

	sg_dma_address(&cam->overlay_sglist) = cam->overlay_base_phys;
	sg_dma_len(&cam->overlay_sglist)= cam->pix.sizeimage;
	while (cam->overlay_cnt < 2) {
		err = camera_core_sgdma_queue(cam, &cam->overlay_sglist, 1,
				camera_core_overlay_callback, NULL);
		if (err)
			break;
		++cam->overlay_cnt;
	}

	spin_unlock_irqrestore(&cam->overlay_lock, irqflags);
}
#endif

/* ---------------------------------------------------------------------------- */

int tcc_videobuf_inputenum(struct v4l2_input *input)
{
	/* default handler assumes 1 video input (the camera) */
	int index = input->index;

	memset(input, 0, sizeof(*input));
	input->index = index;

	if (index > 0)
		return -EINVAL;

	strlcpy(input->name, "camera", sizeof(input->name));
	input->type = V4L2_INPUT_TYPE_CAMERA;

	return 0;
}

int tcc_videobuf_g_input(unsigned int *input)
{
	*input = 0;
	
	return 0;
}

int tcc_videobuf_s_input(unsigned int *input)
{
	if (*input > 0)
		return -EINVAL;

	return 0;
}

int tcc_videobuf_s_caminfo(unsigned int *input)
{
	if (*input < 0)
		return -EINVAL;

	hardware_data.cam_info = *input;
	dprintk("[cam device] = %d \n", hardware_data.cam_info);

	/* Init the camera IF */
	#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	tcc_get_sensor_info(hardware_data.cam_info);
	#endif

	return 0;
}

int tcc_videobuf_g_param(struct v4l2_streamparm *gparam)
{
	memset(gparam,0x00,sizeof(*gparam));
	gparam->parm.capture.capturemode=hardware_data.cif_cfg.oper_mode;
	
	return 0;
}

int tcc_videobuf_s_param(struct v4l2_streamparm *sparam)
{
	return 0;
}

int tcc_videobuf_enum_fmt(struct v4l2_fmtdesc *fmt)
{	
	return sensor_if_enum_pixformat(fmt);
}

int tcc_videobuf_try_fmt(struct v4l2_format *fmt)
{	
	//return sensor_try_format(&fmt->fmt.pix);

	return 0;
}

int tcc_videobuf_g_fmt(struct v4l2_format *fmt)
{
	/* get the current format */
	memset(&fmt->fmt.pix, 0, sizeof (fmt->fmt.pix));
	fmt->fmt.pix = pix;
	
	return 0;
}

int tcc_videobuf_s_fmt(struct v4l2_format *fmt)
{
	unsigned int temp_sizeimage = 0;
	temp_sizeimage = pix.sizeimage;
	
	out_width  = pix.width	= fmt->fmt.pix.width;
	out_height = pix.height	= fmt->fmt.pix.height;

	return tccxxx_cif_set_resolution(fmt->fmt.pix.pixelformat, fmt->fmt.pix.width, fmt->fmt.pix.height);
}

int tcc_videobuf_querycap(struct v4l2_capability *cap)
{
	memset(cap, 0, sizeof(struct v4l2_capability));
	
	strlcpy(cap->driver, CAM_NAME, sizeof(cap->driver));
	strlcpy(cap->card, vfd->name, sizeof(cap->card));

	cap->bus_info[0] = '\0';
	cap->version = KERNEL_VERSION(2, 6, 24);
	cap->capabilities = V4L2_CAP_VIDEO_CAPTURE |				
						V4L2_CAP_VIDEO_OVERLAY |
						V4L2_CAP_READWRITE | 
						V4L2_CAP_STREAMING;
	
	return 0;
}

int tcc_videobuf_g_fbuf(struct v4l2_framebuffer *fargbuf)
{
	memcpy(fargbuf,&fbuf,sizeof(struct v4l2_framebuffer));

	return 0;
}

int tcc_videobuf_s_fbuf(struct v4l2_framebuffer *fargbuf)
{
	fbuf.base = fargbuf->base;
	fbuf.fmt = fargbuf->fmt;				

	return 0;
}

int tcc_videobuf_reqbufs(struct v4l2_requestbuffers *req)
{	
	#if (0) //20111209 ysseung   test...
	if(req->count < 1) {
		printk("reqbufs: count invalid (%d)\n",req->count);
		return -EINVAL;
	}
	#endif

	if(req->memory != V4L2_MEMORY_MMAP && req->memory != V4L2_MEMORY_USERPTR \
		&& req->memory != V4L2_MEMORY_OVERLAY) {
		printk("reqbufs: memory type invalid\n");
		return -EINVAL;
	}
	
	if(req->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) {
		printk("reqbufs: video type invalid\n");
		return -EINVAL;
	}

	#if (0) //20111209 ysseung   test...
	if(req->count > TCC_CAMERA_MAX_BUFNBRS)
		req->count = TCC_CAMERA_MAX_BUFNBRS;
	#endif

	return tccxxx_cif_buffer_set(req);
}

int tcc_videobuf_set_camera_addr(struct v4l2_requestbuffers *req)
{
	tccxxx_set_camera_addr(req->count, req->reserved[0], req->reserved[1]);
	return 0;
}

int tcc_videobuf_querybuf(struct v4l2_buffer *buf)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *)(&hardware_data);
	struct tccxxx_cif_buffer *cif_buf = data->buf + buf->index;
	int index = buf->index;	

	if (index < 0 || index > data->cif_cfg.pp_num) {
		printk(KERN_WARNING "querybuf error : index : %d / %d",index, data->cif_cfg.pp_num);
		return -EINVAL;
	}
	
	memset(buf, 0, sizeof(*buf));
	buf->type 					= V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf->index 					= index;
	buf->flags 					= V4L2_BUF_FLAG_MAPPED;

	buf->flags					|= cif_buf->v4lbuf.flags;
	buf->field 					= V4L2_FIELD_NONE;
	buf->timestamp 				= cif_buf->v4lbuf.timestamp;
	buf->sequence 				= cif_buf->v4lbuf.sequence;
	buf->memory 				= V4L2_MEMORY_MMAP;

	if(data->cif_cfg.fmt == 0) //yuv420
		buf->length 				= PAGE_ALIGN(pix.width*pix.height*2);	//사이즈에 맞게 조정		
	else
		buf->length 				= PAGE_ALIGN(pix.width*pix.height*3/2);	//사이즈에 맞게 조정		
	cif_buf->v4lbuf.length		= buf->length;
	cif_buf->v4lbuf.index		= index;
	buf->m.offset 				= buf->index * buf->length;//data->cif_cfg.preview_buf[index].p_Y;
	cif_buf->v4lbuf.m.offset	= buf->index * buf->length;//data->cif_cfg.preview_buf[index].p_Y;

	dprintk("<%d :: [PA]0x%x / flag: 0x%x  >\n", index, (unsigned int)(cif_buf->v4lbuf.m.offset), (unsigned int)(buf->flags));	
	
	return 0;
}

int tcc_videobuf_qbuf(struct v4l2_buffer *buf)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *)(&hardware_data);
	struct tccxxx_cif_buffer *cif_buf = data->buf + buf->index;

	if(buf->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EAGAIN;

	if((buf->index < 0) || (buf->index > data->cif_cfg.pp_num))
		return -EAGAIN;

	if(cif_buf->v4lbuf.flags & V4L2_BUF_FLAG_QUEUED)
		cif_buf->v4lbuf.flags |= ~V4L2_BUF_FLAG_QUEUED;

	if(cif_buf->v4lbuf.flags & V4L2_BUF_FLAG_DONE)
		cif_buf->v4lbuf.flags |= ~V4L2_BUF_FLAG_DONE;

	cif_buf->v4lbuf.flags |= V4L2_BUF_FLAG_QUEUED;

	list_add_tail(&cif_buf->buf_list, &data->list);

	return 0;			
}

int tcc_videobuf_dqbuf(struct v4l2_buffer *buf, struct file *file )
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *)(&hardware_data);
	struct tccxxx_cif_buffer *cif_buf;

	if(data->cif_cfg.esd_restart) tccxxx_cif_cam_restart(&pix,xclk);

	if(list_empty(&data->done_list)) {
		if(file->f_flags & O_NONBLOCK){
			printk("file->f_flags Fail!!\n");
			return -EAGAIN;
		}
			
		data->wakeup_int = 0;
		if(wait_event_interruptible_timeout(data->frame_wait, data->wakeup_int == 1, msecs_to_jiffies(500)) <= 0) {
			cam_no_connect_cnt++;
			printk("wait_event_interruptible_timeout 500ms!!\n");
			return -EFAULT;
		}

		/* Should probably recheck !list_empty() here */
		if(list_empty(&data->done_list)){
			printk("It needs list_empty!!\n");
			return -ENOMEM;
		}
	}

	cif_buf = list_entry(data->done_list.next, struct tccxxx_cif_buffer, buf_list);
	list_del(data->done_list.next);

	cif_buf->v4lbuf.flags &= ~V4L2_BUF_FLAG_DONE;
	memcpy(buf, &(cif_buf->v4lbuf),sizeof(struct v4l2_buffer));
	
	return 0;
}

static void tcc_set_clock(unsigned char cpu_max)
{
#if defined(CONFIG_CPU_FREQ)
	int index;
	struct tcc_freq_table_t clk_table;

	dprintk(" tcc_set_clock() in \n");

	index = Get_Index_sizeTable(out_width*out_height);
	printk("clock index = %d \n", index);
	
	#if defined(CONFIG_USE_ISP)
		memcpy(&clk_table, &gtISPCameraClockLimitTable[index], sizeof(struct tcc_freq_table_t));
	#else
		memcpy(&clk_table, &gtCameraClockLimitTable[index], sizeof(struct tcc_freq_table_t));
	#endif

	#if defined(CONFIG_VIDEO_ENABLE_CAMERA_WITH_MAX_CLK)
	#if defined(CONFIG_USE_ISP)
		clk_table.mem_freq = gtISPCameraClockLimitTable[(NUM_FREQS -1)].mem_freq;
	#else
		clk_table.mem_freq = gtCameraClockLimitTable[(NUM_FREQS -1)].mem_freq;
	#endif
	#endif

	if(cpu_max)
		clk_table.cpu_freq = 600000;

	printk("Camera Clk_idx[%d]-[%d/%d]-[%d/%d] :: res = %d x %d\n", index, clk_table.cpu_freq/1000, clk_table.mem_freq/1000, 
						clk_table.vbus_freq/10000, clk_table.vcod_freq/10000, out_width, out_height);

	tcc_cpufreq_set_limit_table(&clk_table, TCC_FREQ_LIMIT_CAMERA, 1);

	dprintk(" tcc_set_clock() out \n");
#endif
}

static void tcc_reset_clock(void)
{
#if defined(CONFIG_CPU_FREQ)
	dprintk("tcc_reset_clock in \n");
	#if defined(CONFIG_USE_ISP)
		tcc_cpufreq_set_limit_table(&gtISPCameraClockLimitTable[0], TCC_FREQ_LIMIT_CAMERA, 0);
	#else
		tcc_cpufreq_set_limit_table(&gtCameraClockLimitTable[0], TCC_FREQ_LIMIT_CAMERA, 0);
	#endif
	dprintk("tcc_reset_clock out \n");	
#endif
}

int tcc_videobuf_streamon(enum v4l2_buf_type *type)
{
	if (*type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EAGAIN;
	
	tcc_set_clock(0);
	return tccxxx_cif_start_stream();
}

//stop capture
int tcc_videobuf_streamoff(enum v4l2_buf_type *type )
{	
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;	
	struct tccxxx_cif_buffer *cif_buf;		

	int ret_val;

	if(*type != V4L2_BUF_TYPE_VIDEO_CAPTURE) return -EAGAIN;

	ret_val = tccxxx_cif_stop_stream();

	while(!list_empty(&data->done_list)) {
		cif_buf = list_entry(data->done_list.next, struct tccxxx_cif_buffer, buf_list);

		list_del(&cif_buf->buf_list);

		cif_buf->v4lbuf.flags &= ~V4L2_BUF_FLAG_DONE;
		cif_buf->v4lbuf.flags |= V4L2_BUF_FLAG_QUEUED;

		list_add_tail(&cif_buf->buf_list, &data->list);
	}
	data->done_list.next = &data->done_list;

	return ret_val;
}

int tcc_videobuf_cif_overlay(cif_SuperImpose *overlay)
{
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;

	memcpy(&(data->cif_cfg.si_overlay), overlay, sizeof(cif_SuperImpose));
	
	tccxxx_cif_set_overlay();	
	
	return 0;
}

int tcc_videobuf_user_jpeg_capture(int * Jpeg_quality )
{	
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;	
	struct tccxxx_cif_buffer *cif_buf;			
	int ret_val;

	while(!list_empty(&data->done_list))  {
		cif_buf = list_entry(data->done_list.next, struct tccxxx_cif_buffer, buf_list);		

		list_del(&cif_buf->buf_list);
	
		cif_buf->v4lbuf.flags &= ~V4L2_BUF_FLAG_DONE;
		cif_buf->v4lbuf.flags |= V4L2_BUF_FLAG_QUEUED;
		
		list_add_tail(&cif_buf->buf_list, &data->list);	
	}

	data->done_list.next = &data->done_list;
	data->cif_cfg.now_frame_num = 0;
	data->cif_cfg.retry_cnt 	= 0;

	tcc_set_clock(0);
	ret_val = tccxxx_cif_capture(*Jpeg_quality);

	return ret_val;	
}

int tcc_videobuf_user_get_capture_info(TCCXXX_JPEG_ENC_DATA * Jpeg_data )
{	
	struct TCCxxxCIF *data = (struct TCCxxxCIF *) &hardware_data;	
	unsigned char *pVirt_BaseAddr;

	Jpeg_data->width 			= data->cif_cfg.main_set.target_x;
	Jpeg_data->height 			= data->cif_cfg.main_set.target_y;
	Jpeg_data->target_addr 		= data->cif_cfg.jpg_info.start_phy_addr;
	Jpeg_data->target_size 		= data->cif_cfg.jpg_info.header_offset + data->cif_cfg.jpg_info.header_size;
	
	//header
 	Jpeg_data->header_offset 	= data->cif_cfg.jpg_info.header_offset;
	Jpeg_data->header_size 		= data->cif_cfg.jpg_info.header_size;
	//thumb
 	Jpeg_data->thumb_offset 	= data->cif_cfg.jpg_info.thumb_offset;
	Jpeg_data->thumb_size 		= data->cif_cfg.jpg_info.thumb_size;
	//stream
	Jpeg_data->bitstream_offset = data->cif_cfg.jpg_info.bitstream_offset;	
	Jpeg_data->bitstream_size	= data->cif_cfg.jpg_info.bitstream_size;


	if(Jpeg_data->target_size)
	{
		pVirt_BaseAddr = ioremap_nocache(Jpeg_data->target_addr, Jpeg_data->target_size);

		dprintk("CAM :: 0x%x, 0x%x /// 0x%x, 0x%x \n", *((unsigned char*)pVirt_BaseAddr), *((unsigned char*)pVirt_BaseAddr+1), 
													   *((unsigned char*)pVirt_BaseAddr+Jpeg_data->thumb_offset), *((unsigned char*)pVirt_BaseAddr+Jpeg_data->thumb_offset+1));
		iounmap(pVirt_BaseAddr);
	}
	
 	dprintk("%d x %d => stream: 0x%x - 0x%x, thumb: 0x%x - 0x%x, header: 0x%x - 0x%x \n",
					Jpeg_data->width, Jpeg_data->height,
					Jpeg_data->bitstream_offset, Jpeg_data->bitstream_size,
					Jpeg_data->thumb_offset, Jpeg_data->thumb_size,
					Jpeg_data->header_offset, Jpeg_data->header_size);
	
	return 0;
}

int tcc_videobuf_get_zoom_support(int cameraIndex)
{
	// cameraIndex는 나중에 사용할 수 있음.
	#if (defined(CONFIG_DRAM_16BIT_USED)&&(defined(CONFIG_ARCH_TCC88XX)||defined(CONFIG_ARCH_TCC93XX)||defined(CONFIG_ARCH_TCC89XX)))
	printk("Camera zoom not supported. \n");
	return 0;
	#else
	printk("Camera zoom support. \n");
	return 1;
	#endif
}

static int camera_core_sensor_open(void)
{
	int ret;

	/* initialize the sensor and define a default capture format cam->pix */
	dprintk("Sensor Device-Init \n");
	if ((ret = sensor_if_init(&pix)) < 0) 
	{
		printk(KERN_ERR CAM_NAME ": cannot initialize sensor\n");
		goto error;
	}	
	
	/* program the sensor for the capture format and rate */
	dprintk("Sensor Register-Init \n");
	if (sensor_if_configure(&pix,xclk))
	{
		printk (KERN_ERR CAM_NAME ": Camera sensor configuration failed\n");
		goto error;
	} 
	
	// ioremap시 probe에서 수행시 virtual address problem으로 이동.!!
	if (tccxxx_cif_init()) 
	{
		printk(KERN_ERR CAM_NAME ": cannot initialize interface hardware\n");
		goto error;
	}	

	if (tccxxx_cif_open())
	{
		printk (KERN_ERR CAM_NAME ": Camera IF configuration failed\n");
		goto error;
	}

	//dprintk("camera_open : %d ms \n", (jiffies - jiffies_prev)*10);

	return 0;

error :

	sensor_if_cleanup();
	sensor_if_deinit();
	return -ENODEV;	
}


/* ---------------------------------------------------------------------------- */

static long camera_core_do_ioctl(struct file *file, unsigned int cmd, void *arg)
{
	//int err;
	
	switch (cmd) {
		case VIDIOC_ENUMINPUT:
			return tcc_videobuf_inputenum((struct v4l2_input *)arg);
			
		case VIDIOC_G_INPUT:
			return tcc_videobuf_g_input((unsigned int*)arg);

		case VIDIOC_S_INPUT:
			return tcc_videobuf_s_input((unsigned int*)arg);
		
		case VIDIOC_G_PARM:
			return tcc_videobuf_g_param((struct v4l2_streamparm*)arg);

		case VIDIOC_S_PARM:
			return tcc_videobuf_s_param((struct v4l2_streamparm*)arg);
		
		case VIDIOC_ENUM_FMT:
			return tcc_videobuf_enum_fmt((struct v4l2_fmtdesc*)arg);

		case VIDIOC_TRY_FMT:
			return tcc_videobuf_try_fmt((struct v4l2_format*)arg);

		case VIDIOC_G_FMT:
			return tcc_videobuf_g_fmt((struct v4l2_format*)arg);

		case VIDIOC_S_FMT:
			return tcc_videobuf_s_fmt((struct v4l2_format*)arg);

		case VIDIOC_QUERYCTRL:
			return sensor_if_query_control((struct v4l2_queryctrl *)arg);

		case VIDIOC_G_CTRL:
			return sensor_if_get_control((struct v4l2_control *)arg);

		case VIDIOC_S_CTRL:
			return sensor_if_set_control((struct v4l2_control *)arg, 0);
		
		case VIDIOC_QUERYCAP:
			return tcc_videobuf_querycap((struct v4l2_capability *)arg);

		case VIDIOC_G_FBUF: /* Get the frame buffer parameters */
			return tcc_videobuf_g_fbuf((struct v4l2_framebuffer *)arg);

		case VIDIOC_S_FBUF: /* set the frame buffer parameters */
			return tcc_videobuf_s_fbuf((struct v4l2_framebuffer *)arg);

		case VIDIOC_REQBUFS:
			return tcc_videobuf_reqbufs((struct v4l2_requestbuffers *)arg);            

		case VIDIOC_QUERYBUF:
			return tcc_videobuf_querybuf((struct v4l2_buffer *)arg);			

		case VIDIOC_QBUF:
			return tcc_videobuf_qbuf((struct v4l2_buffer *)arg);						

		case VIDIOC_DQBUF:
			return tcc_videobuf_dqbuf((struct v4l2_buffer *)arg, file);						
			
		case VIDIOC_STREAMON:
			return tcc_videobuf_streamon((enum v4l2_buf_type *)arg);

		case VIDIOC_STREAMOFF:
			return tcc_videobuf_streamoff((enum v4l2_buf_type *)arg);

		case VIDIOC_OVERLAY:
			return -EINVAL;

		case VIDIOC_ENUMSTD:
		case VIDIOC_G_STD:
		case VIDIOC_S_STD:
		case VIDIOC_QUERYSTD:
		{
			/* Digital cameras don't have an analog video standard, 
			 * so we don't need to implement these ioctls.
			 */
			 return -EINVAL;
		}
		case VIDIOC_G_AUDIO:
		case VIDIOC_S_AUDIO:
		case VIDIOC_G_AUDOUT:
		case VIDIOC_S_AUDOUT:
		{
			/* we don't have any audio inputs or outputs */
			return -EINVAL;
		}

		case VIDIOC_G_JPEGCOMP:
		case VIDIOC_S_JPEGCOMP:
		{
			/* JPEG compression is not supported */
			return -EINVAL;
		}

		case VIDIOC_G_TUNER:
		case VIDIOC_S_TUNER:
		case VIDIOC_G_MODULATOR:
		case VIDIOC_S_MODULATOR:
		case VIDIOC_G_FREQUENCY:
		case VIDIOC_S_FREQUENCY:
		{
			/* we don't have a tuner or modulator */
			return -EINVAL;
		}

		case VIDIOC_ENUMOUTPUT:
		case VIDIOC_G_OUTPUT:
		case VIDIOC_S_OUTPUT:
		{
			/* we don't have any video outputs */
			return -EINVAL;
		}

		case VIDIOC_USER_CIF_OVERLAY:
			return tcc_videobuf_cif_overlay((cif_SuperImpose *)arg);
			
		case VIDIOC_USER_JPEG_CAPTURE:
			return tcc_videobuf_user_jpeg_capture((int *)arg);

		case VIDIOC_USER_GET_CAPTURE_INFO:
			return tcc_videobuf_user_get_capture_info((TCCXXX_JPEG_ENC_DATA *)arg);

		case VIDIOC_USER_PROC_AUTOFOCUS:
			return sensor_if_adjust_autofocus();

		case VIDIOC_USER_SET_CAMINFO_TOBEOPEN:
		{			
			tcc_videobuf_s_caminfo((unsigned int*)arg);
			return camera_core_sensor_open();
		}

		case VIDIOC_USER_GET_MAX_RESOLUTION:
			return sensor_if_get_max_resolution((int *)arg);

		case VIDIOC_USER_GET_SENSOR_FRAMERATE:
			return sensor_if_get_sensor_framerate((int *)arg);

		case VIDIOC_USER_GET_ZOOM_SUPPORT:
			return tcc_videobuf_get_zoom_support((int *)arg);

		case VIDIOC_USER_SET_CAMERA_ADDR:
			return tcc_videobuf_set_camera_addr((struct v4l2_requestbuffers *)arg);

		default:
		{
			/* unrecognized ioctl */
			//return -ENOIOCTLCMD;
		}
	}

	return 0;
}

/*
 *  file operations
 */
 
#if defined(CONFIG_VIDEO_CAMERA_SENSOR_ISX006)
//
// Decode Interleave Data(Spoof Free Runnng) and output Jpeg Data and YUV data seperately.
//
int DecodeInterleaveData(unsigned char *pInterleaveData,	// (IN) Pointer of Interleave Data
							 unsigned int interleaveDataSize,			// (IN) Data Size of Interleave Data
							 int yuvWidth,						// (IN) Width of YUV Thumbnail
							 int yuvHeight, 					// (IN) Height of YUV Thumbnail
							 unsigned char *pJpegData,			// (OUT) Pointer of Buffer for Receiving JPEG Data
							 unsigned int jpegBufSize,					// (IN) Buffer Size for JPEG Data
							 unsigned int *pJpegSize,					// (OUT) Pointer of JPEG Data Size
							 unsigned char *pYuvData,			// (OUT) Pointer of Buffer for Receiving YUV Data
							 unsigned int yuvBufSize)					// (IN) Buffer Size for YUV Data
{
	int ret;
	unsigned long *interleave_ptr;
	unsigned char *jpeg_ptr;
	unsigned char *yuv_ptr;
	unsigned char *p;
	unsigned int jpeg_size;
	unsigned int yuv_size;
	unsigned int i;

	unsigned int padding_cnt = 0, yuv_cnt = 0; 

	if(pInterleaveData == NULL)
	{
		return FALSE;
	}

	ret 			= TRUE;
	interleave_ptr	= (unsigned long*)pInterleaveData;
	jpeg_ptr		= pJpegData;
	yuv_ptr 		= pYuvData;
	jpeg_size		= 0;
	yuv_size		= 0;

	i = 0;
	while(i < interleaveDataSize)
	{
		if((*interleave_ptr == 0xFFFFFFFF) || (*interleave_ptr == 0x02FFFFFF) || (*interleave_ptr == 0xFF02FFFF)) // Need to modify here if Big-Endian system.
		{
			// Padding Data
			padding_cnt++;
			interleave_ptr++;
			i += 4;
		}
		else if((*interleave_ptr & 0xFFFF) == 0x05FF) // Need to modify here if Big-Endian system.
		{
			yuv_cnt++;
			// Start-code of YUV Data
			p = (unsigned char*)interleave_ptr;
			p += 2;
			i += 2;

			// Extract YUV Data
			if(pYuvData != NULL)
			{ 
		#if defined(CONFIG_VIDEO_CAMERA_SENSOR_ISX006)
				memcpy(yuv_ptr, p, (yuvWidth * 2));
		#else
				if(memcpy_s(yuv_ptr, (yuvBufSize-yuv_size), p, yuvWidth * 2) != 0)
				{
					ret = FALSE;
					break;
				}
		#endif
				yuv_ptr += yuvWidth * 2;
				yuv_size += yuvWidth * 2;
			}
			
			p += yuvWidth * 2;
			i += yuvWidth * 2;

			// Check End-code of YUV Data
			if((*p == 0xFF) && (*(p+1) == 0x06))
			{
				dprintk("YUVData [%d]-%d : 0x%x, 0x%x \n", i, yuv_cnt, *p, *(p+1));
				interleave_ptr = (unsigned long*)(p + 2);
				i += 2;
			}
			else
			{
				printk("YUVData error[%d]-%d : 0x%x, 0x%x \n", i, yuv_cnt, *p, *(p+1));
				ret = FALSE;
				break;
			}
		}
		else
		{
			// Extract JPEG Data
			if(pJpegData != NULL)
			{
		#if defined(CONFIG_VIDEO_CAMERA_SENSOR_ISX006)
				memcpy(jpeg_ptr, interleave_ptr, 4);
		#else
				if(memcpy_s(jpeg_ptr, (jpegBufSize-jpeg_size), interleave_ptr, 4) != 0)
				{
					ret = FALSE;
					break;
				}
		#endif
				jpeg_ptr += 4;
				jpeg_size += 4;
			}
			
			interleave_ptr++;
			i += 4;
		}
	}

	printk("count[%d]: padding(%d), yuv(%d), jpeg(%d) \n", i, padding_cnt*4, yuv_size, jpeg_size);
	
	if(ret)
	{
		if(pJpegData != NULL)
		{ 
			// Remove Padding after EOI
			for(i=0; i < 3; i++)
			{
				if(*(--jpeg_ptr) != 0xFF)
				{
					break;
				}
				jpeg_size--;
			}
			
			if(pJpegSize != NULL)
			{
				*pJpegSize = jpeg_size;
			}
		}
		
		// Check YUV Data Size
		if(pYuvData != NULL)
		{ 
			if(yuv_size != (yuvWidth * yuvHeight * 2))
			{
				printk("YUVData size mismatch \n");
			    ret = FALSE;
			}
		}
	}

	printk("JPEG SOI/EOI check: 0x%x, 0x%x, 0x%x, 0x%x ~~~ 0x%x, 0x%x, 0x%x, 0x%x \n", pJpegData[0], pJpegData[1], pJpegData[2], pJpegData[3],
				pJpegData[jpeg_size-4], pJpegData[jpeg_size-3], pJpegData[jpeg_size-2], pJpegData[jpeg_size-1]);

	return ret;
}
#endif

static unsigned int camera_core_poll(struct file *file, struct poll_table_struct *wait)
{
	poll_wait(file, &(hardware_data.frame_wait), wait);	

	if(hardware_data.cif_cfg.cap_status == CAPTURE_DONE)
	{
		dprintk("POLL IN ! \r\n");
		dprintk("CAM CLOCK :: initialized to 0 \n");
		//tcc_reset_clock();

#if defined(CONFIG_VIDEO_CAMERA_SENSOR_ISX006)
		{
			void *jpeg_source_remapped_data_baseaddr, *jpeg_target_remapped_data_baseaddr;
			
			tcc_set_clock(1);

			hardware_data.cif_cfg.jpg_info.start_phy_addr	= PA_JPEG_RAW_BASE_ADDR;
			hardware_data.cif_cfg.jpg_info.bitstream_offset = 2;
			hardware_data.cif_cfg.jpg_info.thumb_offset 	= (4*1024*1024);
			hardware_data.cif_cfg.jpg_info.header_offset	= (5*1024*1024);
			hardware_data.cif_cfg.jpg_info.bitstream_size	= 0;
			hardware_data.cif_cfg.jpg_info.thumb_size		= (320*240*2);
			hardware_data.cif_cfg.jpg_info.header_size		= 0;

			printk("DecodeInterleaveData Start!! \n");

			jpeg_source_remapped_data_baseaddr = (void *)ioremap_nocache(PA_PREVIEW_BASE_ADDR, PAGE_ALIGN(CAPTURE_MEM_SIZE/*-PAGE_SIZE*/));
			if (jpeg_source_remapped_data_baseaddr == NULL) {
				printk(KERN_ALERT "[raw] can not remap for jpeg\n");
				return -EFAULT;
			}

			jpeg_target_remapped_data_baseaddr = (void *)ioremap_nocache(PA_JPEG_RAW_BASE_ADDR, PAGE_ALIGN(JPEG_RAW_MEM_SIZE/*-PAGE_SIZE*/));
			if (jpeg_target_remapped_data_baseaddr == NULL) {
				printk(KERN_ALERT "[raw] can not remap for jpeg\n");
				return -EFAULT;
			}
			
			DecodeInterleaveData((unsigned char*)jpeg_source_remapped_data_baseaddr, (CAP_W*CAP_H),
									 320, 240, (unsigned char*)jpeg_target_remapped_data_baseaddr, (4096*1024),
									 &(hardware_data.cif_cfg.jpg_info.bitstream_size),
									 (unsigned char*)(jpeg_target_remapped_data_baseaddr+hardware_data.cif_cfg.jpg_info.thumb_offset), (320*240*2));
			printk("DecodeInterleaveData End!! stream_size = %d \n", hardware_data.cif_cfg.jpg_info.bitstream_size);

			if(jpeg_source_remapped_data_baseaddr != NULL)
				iounmap(jpeg_source_remapped_data_baseaddr);
			
			if(jpeg_target_remapped_data_baseaddr != NULL)
				iounmap(jpeg_target_remapped_data_baseaddr);

			//tcc_reset_clock();
		}	
#endif

		return POLLIN;
	}
	else if(hardware_data.cif_cfg.cap_status == CAPTURE_OVERFLOW)
	{
		dprintk("POLL ERR ! \r\n");		
		printk("CAM CLOCK :: initialized to 0 \n");
		//tcc_reset_clock();

		return POLLERR;
	}
	//else //cap_status = CAPTURE_NO_INT;
	//	return 0;

	//if (!list_empty(&(hardware_data.done_list)))
	//	return POLLIN | POLLRDNORM;

	dprintk("NO_INT ! \r\n"); 

	return 0;
}

extern int range_is_allowed(unsigned long pfn, unsigned long size);
static int camera_core_mmap(struct file *file, struct vm_area_struct *vma)
{
	if(range_is_allowed(vma->vm_pgoff, vma->vm_end - vma->vm_start) < 0){
		printk(KERN_ERR  "camera: this address is not allowed \n");
		return -EAGAIN;
	} 

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	if(remap_pfn_range(vma,vma->vm_start, vma->vm_pgoff , vma->vm_end - vma->vm_start, vma->vm_page_prot))
	{
		return -EAGAIN;
	}

	vma->vm_ops 	= NULL;
	vma->vm_flags 	|= VM_IO;
	vma->vm_flags 	|= VM_RESERVED;
	
	return 0;
}

static long camera_core_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	return video_usercopy(file, cmd, arg, camera_core_do_ioctl);
}

static int camera_core_release(struct file *file)
{
	dprintk("camera_core_release \n");
	//printk("camera_core_release!! \n");
	tccxxx_cif_close();
	tcc_reset_clock();

	#if defined(CONFIG_USE_ISP)
	clk_disable(isp_clk);
	#elif	defined(CONFIG_ARCH_TCC892X)
		// VIOC Block Disable.
		clk_disable(vioc_clk);
	#else
	clk_disable(cif_clk);
	#endif
	
	sensor_if_deinit();
	dprintk("CAM CLOCK :: initialized to 0 \n");

	return 0;
}

static int camera_core_open(struct file *file)
{
	int minor = video_devdata(file)->minor;
	//int ret;
	//unsigned long jiffies_prev;
	//jiffies_prev = jiffies;

	dprintk("camera_core_open \n");	
	//printk("camera_core_open !!!\n");	
	if (!vfd || (vfd->minor != minor))
		return -ENODEV;

	#if defined(CONFIG_USE_ISP)
	clk_enable(isp_clk);
	#elif	defined(CONFIG_ARCH_TCC892X)
		// VIOC Block enable.
		clk_enable(vioc_clk);
	#else
	clk_enable(cif_clk);
	#endif

	tcc_set_clock(0);

#if (0)
	/* initialize the sensor and define a default capture format cam->pix */
	dprintk("Sensor Device-Init \n");
	if ((ret = sensor_if_init(&pix)) < 0) 
	{
		printk(KERN_ERR CAM_NAME ": cannot initialize sensor\n");
		goto error;
	}	
	
	/* program the sensor for the capture format and rate */
	dprintk("Sensor Register-Init \n");
	if (sensor_if_configure(&pix,xclk))
	{
		printk (KERN_ERR CAM_NAME ": Camera sensor configuration failed\n");
		goto error;
	} 
	
	// ioremap시 probe에서 수행시 virtual address problem으로 이동.!!
	if (tccxxx_cif_init()) 
	{
		printk(KERN_ERR CAM_NAME ": cannot initialize interface hardware\n");
		goto error;
	}	

	if (tccxxx_cif_open())
	{
		printk (KERN_ERR CAM_NAME ": Camera IF configuration failed\n");
		goto error;
	}

	//dprintk("camera_open : %d ms \n", (jiffies - jiffies_prev)*10);

	return 0;

error :

	sensor_if_cleanup();
	sensor_if_deinit();

	return -ENODEV;	
#endif

	return	0;
}

static struct v4l2_file_operations camera_core_fops = 
{
	.owner          = THIS_MODULE,
	.poll           = camera_core_poll,
	.unlocked_ioctl = camera_core_ioctl,
	.mmap           = camera_core_mmap,
	.open           = camera_core_open,
	.release        = camera_core_release
};

#ifdef CONFIG_HAS_EARLYSUSPEND
static void tcc92xx_camera_early_suspend(struct early_suspend *h);
static void tcc92xx_camera_late_resume(struct early_suspend *h);
#endif

static int __init camera_core_probe(struct platform_device *pdev)
{
	int	status;

	dprintk("camera_core_probe \n");

	/* initialize the video_device struct */
	vfd = video_device_alloc();
	if (!vfd) 
	{
		printk(KERN_ERR CAM_NAME": could not allocate video device struct\n");
		status = -ENOMEM;
		goto err0;
	}
	
 	vfd->release = video_device_release;

 	strlcpy(vfd->name, CAM_NAME, sizeof(vfd->name));
 	//vfd->type = VID_TYPE_CAPTURE | VID_TYPE_OVERLAY | VID_TYPE_CHROMAKEY;
 	
 	/* need to register for a VID_HARDWARE_* ID in videodev.h */
 	vfd->fops = &camera_core_fops;
	//video_set_drvdata(vfd, NULL);
 	vfd->minor = -1;


	/* initialize the camera interface */
	/* // ioremap시 probe에서 수행시 virtual address problem으로 이동.!!
	status = tccxxx_cif_init();	
	if (status != 0) 
	{
		printk(KERN_ERR CAM_NAME ": cannot initialize interface hardware\n");
		status = -ENODEV;
		goto err1;
	}
	*/

	//platform_set_drvdata(pdev, cam);
	
	if (video_register_device(vfd, VFL_TYPE_GRABBER, video_nr) < 0) 
	{
		printk(KERN_ERR CAM_NAME ": could not register Video for Linux device\n");
		status = -ENODEV;
		goto err1;
	}

	printk(KERN_INFO CAM_NAME ": registered device video%d [v4l2]\n", vfd->minor);
	#ifdef CONFIG_HAS_EARLYSUSPEND
	early_suspend.suspend = tcc92xx_camera_early_suspend;
	early_suspend.resume  = tcc92xx_camera_late_resume;
	early_suspend.level   = EARLY_SUSPEND_LEVEL_BLANK_SCREEN;
	register_early_suspend(&early_suspend);
	#endif

	//tmp_i2c_add();
	return 0;

 err1:
	video_device_release(vfd);
 err0:
	return status;
}

static int camera_core_remove(struct platform_device *pdev)
{
	dprintk("camera_core_remove \n");

	//tmp_i2c_del();
	video_unregister_device(vfd);
	video_device_release(vfd);

	return 0;
}

int camera_core_suspend(struct platform_device *pdev, pm_message_t state)
{
	return 0;
}

int camera_core_resume(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver camera_core_driver = {
	.driver = {
		.name 	= CAM_NAME,
		.owner 	= THIS_MODULE,
	},
	.probe 		= camera_core_probe,
	.remove 		= camera_core_remove,
	.suspend 		= camera_core_suspend,
	.resume 		= camera_core_resume
};

#ifdef CONFIG_HAS_EARLYSUSPEND
static void tcc92xx_camera_early_suspend(struct early_suspend *h)
{
	camera_core_driver.suspend;
}

static void tcc92xx_camera_late_resume(struct early_suspend *h)
{
	camera_core_driver.resume;
}
#endif

static struct platform_device camera_core_device = {
	.name	= CAM_NAME,
	.dev	= {
		.release 	= NULL,
	},
	.id	= 0,
};

void __exit
camera_core_cleanup(void)
{
	dprintk("camera_core_cleanup \n");

	if(tcc_is_camera_enable){
		platform_driver_unregister(&camera_core_driver);
		platform_device_unregister(&camera_core_device);
	}

	#if defined(CONFIG_USE_ISP)
	clk_put(isp_clk);
	#elif	defined(CONFIG_ARCH_TCC892X)
		// VIOC Block Clock Put.
		clk_put(vioc_clk);
	#else
	clk_put(cif_clk);
	#endif
	TCC_CIF_CLOCK_Put();

	return;
}

static char banner[] __initdata = KERN_INFO "TCCXXX Camera driver initializing\n";

int __init 
camera_core_init(void)
{
	dprintk("camera_core_init \n");
		
	printk(banner);
	if(tcc_is_camera_enable){
		printk("Camera Feature is alive!\n");
		platform_device_register(&camera_core_device);
		platform_driver_register(&camera_core_driver);
	}
	else
		printk("Camera Feature is dead in bootloader! please, check!\n");

	#if defined(CONFIG_USE_ISP)
	isp_clk = clk_get(NULL, "isp");
	BUG_ON(isp_clk == NULL);
	#elif	defined(CONFIG_ARCH_TCC892X)
		// VIOC Block Clk Get.	vioc_clk
		vioc_clk = clk_get(NULL, "lcdc");
		BUG_ON(vioc_clk == NULL);
	#else
	cif_clk = clk_get(NULL, "cif");
	BUG_ON(cif_clk == NULL);
	#endif
	
	TCC_CIF_CLOCK_Get();

	return 0;
}

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERSION);
MODULE_LICENSE("GPL");
module_param(video_nr, int, 0);
MODULE_PARM_DESC(video_nr, "Minor number for video device (-1 ==> auto assign)");
module_param(capture_mem, int, 0);
MODULE_PARM_DESC(capture_mem, "Maximum amount of memory for capture buffers (default 4800KB)");
module_init(camera_core_init);
module_exit(camera_core_cleanup);

