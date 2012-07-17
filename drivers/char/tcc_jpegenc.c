/*
 * drivers/video/Tcc_jpeg_enc.c
 *
 * Copyright (C) 2004 Telechips, Inc. 
 *
 * jpeg encoder driver
 *
 * 
 * This package is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
 * 
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED 
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. 
 *
 */

 
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
#include <asm/processor.h>
#include <linux/dma-mapping.h>
#include <linux/fb.h>
#include <linux/clk.h>
#include <linux/cpufreq.h>
#include <linux/syscalls.h>

#include <linux/poll.h>

#include <asm/io.h>
#include <asm/byteorder.h>
#include <asm/irq.h>
#include <mach/bsp.h>

#include "tcc_jpeg.h"
#include "tcc_jpegenc_app.h"
#include <plat/pmap.h>
#include <mach/tcc_scaler_ioctrl.h>
#include <asm/mach-types.h>


#define TCC_JPEGENC_MAJOR_ID 211
#define TCC_JPEGENC_MINOR_ID 1
#define TCC_JPEGENC_DEVICE_NAME "jpegenc"

#define dprintk(msg...)	if (0) { printk( "TCC_JPEGENC: " msg); }

typedef struct _int_data_t {
	wait_queue_head_t wq;
	spinlock_t lock;
	unsigned int count;
	
	struct mutex io_mutex;

	unsigned char irq_reged;
	unsigned char dev_opened;
} int_data_t;
static int_data_t jpgenc_data;

JPEG_ENC_INFO  gJPEG_ENC_Info;
JPEG_BUFFER_CTRL gJPEGEnc_Buffer_Info;

static TCCXXX_JPEG_ENC_DATA enc_data;
static unsigned int gJpegEncodeDone = 0;

void *pJpegenc_remapped_rawdata, *pJpegenc_remapped_streamdata, *pJpegenc_remapped_header;

static struct clk *vcore_clk;
#ifdef USE_VCACHE		
static struct clk *vcache_clk;
#endif
static struct clk *jpege_clk;

#ifdef CONFIG_CPU_FREQ
extern struct tcc_freq_table_t gtJpegClockLimitTable[];
extern struct tcc_freq_table_t gtJpegMaxClockLimitTable;
extern struct tcc_freq_table_t gtVpuNormalClockLimitTable[];
#endif

#ifdef CONFIG_VIDEO_TCCXX_CAMERA
extern int cif_get_capturedone(void);
extern int Get_Index_sizeTable(unsigned int Image_Size);
extern int tccxxx_isalive_camera(void);
#else
void cif_set_frameskip(unsigned char skip_count, unsigned char locked)
{
}
EXPORT_SYMBOL(cif_set_frameskip);

int tccxxx_isalive_camera(void)
{
	return 0;
}
EXPORT_SYMBOL(tccxxx_isalive_camera);
#endif

static void tccxx_jpgenc_set_clock(unsigned char init)
{
	if(init)
	{
		clk_enable(vcore_clk);
#ifdef USE_VCACHE				
		clk_enable(vcache_clk);
#endif
		clk_enable(jpege_clk);
		clk_disable(vcore_clk); 
	}
	else
	{
#ifdef CONFIG_VIDEO_TCCXX_CAMERA	
		int index = 0;
		unsigned int capture_done = 0;
		unsigned int cam_alive_size= 0;
#endif
		clk_enable(vcore_clk);

#ifdef CONFIG_VIDEO_TCCXX_CAMERA
		cam_alive_size = tccxxx_isalive_camera();
		capture_done = cif_get_capturedone();

		if(cam_alive_size && !capture_done)
		{
			index = Get_Index_sizeTable(cam_alive_size);
	#ifdef CONFIG_CPU_FREQ
			tcc_cpufreq_set_limit_table(&gtJpegClockLimitTable[index], TCC_FREQ_LIMIT_JPEG, 1);		
	#endif
		}
		else
#endif
		{
#if defined(CONFIG_CPU_FREQ) 
	#if defined(CONFIG_VIDEO_TCCXX_CAMERA)
			if(cam_alive_size && capture_done)
				tcc_cpufreq_set_limit_table(&gtJpegMaxClockLimitTable, TCC_FREQ_LIMIT_CAMERA, 1);
			else
	#endif				
				tcc_cpufreq_set_limit_table(&gtJpegMaxClockLimitTable, TCC_FREQ_LIMIT_JPEG, 1);
			dprintk("JPEG Enc Clock Setting!!\n");
#endif
		}
	}
}

void tccxx_jpgenc_reset_clock(unsigned char deinit)
{
#ifdef CONFIG_CPU_FREQ
	tcc_cpufreq_set_limit_table(&gtJpegClockLimitTable[0], TCC_FREQ_LIMIT_JPEG, 0);
#endif

	if(deinit)
	{
		clk_disable(jpege_clk);
#ifdef USE_VCACHE		
		clk_disable(vcache_clk);
#endif
	}
	else
	{
		clk_disable(vcore_clk);			
	}
}

static int tccxx_enc_init(void *phy_src, int ImageWidth, int ImageHeight, enum jpeg_quantisation_val val, EncodeInputType type)
{
#if !defined(CONFIG_ARCH_TCC892X)
	unsigned int uiOutputBufferSize, tmp_width;
	void *BufferY, *BufferU, *BufferV;

	JPEG_Exif_Header_Info_Init();

	if((ImageWidth%16) == 0)
		tmp_width = ImageWidth;
	else
		tmp_width = ((ImageWidth+15)>>4)<<4;

	BufferY = (void*)phy_src;
	BufferU = (void*)GET_ADDR_YUV42X_spU(BufferY, tmp_width, ImageHeight);
	if(type == ENC_INPUT_422)
		BufferV = (void*)GET_ADDR_YUV422_spV(BufferU, tmp_width, ImageHeight);
	else	
		BufferV = (void*)GET_ADDR_YUV420_spV(BufferU, tmp_width, ImageHeight);
	
	JPEG_SW_Reset(JPEG_ENCODE_MODE);

	uiOutputBufferSize = (gJPEGEnc_Buffer_Info.pBaseBitstreamDataSize+1023)/1024;

	gJPEG_ENC_Info.is_rolling = FALSE;
	gJPEG_ENC_Info.y_addr = (unsigned int)BufferY;
	gJPEG_ENC_Info.u_addr = (unsigned int)BufferU;
	gJPEG_ENC_Info.v_addr = (unsigned int)BufferV;
	gJPEG_ENC_Info.ifrm_hsize = tmp_width;
	gJPEG_ENC_Info.ibuf_vsize = ImageHeight;
	gJPEG_ENC_Info.q_value = val;
	gJPEG_ENC_Info.img_hsize = ImageWidth;
	gJPEG_ENC_Info.img_vsize = ImageHeight;
	gJPEG_ENC_Info.cbuf_addr = (unsigned int)gJPEGEnc_Buffer_Info.pBaseBitstreamDataAddr;
	gJPEG_ENC_Info.cbuf_size = uiOutputBufferSize;
	gJPEG_ENC_Info.frame_cnt = FRAME_LINE_CNT;

	printk("rolling capture -> %d. \n", gJPEG_ENC_Info.is_rolling);
	printk("y_addr -> 0x%08x, u_addr -> 0x%08x, v_addr -> 0x%08x. \n", gJPEG_ENC_Info.y_addr, \
																		gJPEG_ENC_Info.u_addr, \
																		gJPEG_ENC_Info.v_addr);
	printk("frm_hsize -> %d, buf_vsize -> %d. \n", gJPEG_ENC_Info.ifrm_hsize, gJPEG_ENC_Info.ibuf_vsize);
	printk("img_width -> %d, img_height -> %d. \n", gJPEG_ENC_Info.img_hsize, gJPEG_ENC_Info.img_vsize);
	printk("cbuf_addr -> 0x%08x, cbuf_size -> 0x%08x. \n", gJPEG_ENC_Info.cbuf_addr, gJPEG_ENC_Info.cbuf_size);
	printk("frame_cnt -> %d. \n", gJPEG_ENC_Info.frame_cnt);

	if(type == ENC_INPUT_422)
		gJPEG_ENC_Info.chroma = HwJP_CHROMA_422;
	else
		gJPEG_ENC_Info.chroma = Hw31|HwJP_CHROMA_422;  //TCC92XX Encoding Input = YUV420/422 :: Output => Only YUV422 Format

	JPEG_SET_Encode_Config(&gJPEG_ENC_Info);

	JPEG_INTERRUPT_Enable(JPEG_ENCODE_MODE);
#endif // CONFIG_ARCH_TCC892X
	return 0;
}

static int tccxx_enc_meminit(void *data)
{
	TCCXXX_JPEG_ENC_DATA *Enc_data;
	unsigned int raw_phy_addr, raw_size;
	unsigned int stream_phy_addr, stream_size;
	unsigned int header_phy_addr, header_size;
	#if (1)
	unsigned int jpeg_header_size = 0x30000;
	#else
	pmap_t pmap_jpeg_header;
	pmap_get_info("jpeg_header", &pmap_jpeg_header);
	#endif

	if(pJpegenc_remapped_header != NULL) {
		iounmap(pJpegenc_remapped_header);
		pJpegenc_remapped_header = NULL;
	}
	
	if(pJpegenc_remapped_rawdata != NULL) {
		iounmap(pJpegenc_remapped_rawdata);
		pJpegenc_remapped_rawdata = NULL;
	}
	
	if(pJpegenc_remapped_streamdata != NULL) {
		iounmap(pJpegenc_remapped_streamdata);
		pJpegenc_remapped_streamdata = NULL;
	}
	
	Enc_data = (TCCXXX_JPEG_ENC_DATA *)data;

	//Target is Raw-data in case of Decode.
	raw_phy_addr = Enc_data->source_addr;
	if(Enc_data->src_format == ENC_INPUT_422)
		raw_size = Enc_data->width * Enc_data->height * 2;
	else
		raw_size = Enc_data->width * Enc_data->height * 3 / 2;

	//Source is Stream-data in case of Decode.
	stream_phy_addr = Enc_data->target_addr;
	stream_size 	= Enc_data->target_size - jpeg_header_size;
	header_phy_addr = stream_phy_addr + stream_size;
	header_size		= jpeg_header_size;

	if((raw_phy_addr && raw_size) > 0) {
		pJpegenc_remapped_rawdata = (void *)ioremap_nocache(raw_phy_addr, PAGE_ALIGN(raw_size/*-PAGE_SIZE*/));
		if(pJpegenc_remapped_rawdata == NULL) {
			printk(KERN_ALERT "[raw] can not remap for jpeg\n");
			return -EFAULT;
		}	
	} else {
		pJpegenc_remapped_rawdata = NULL;
	}

	if((stream_phy_addr && stream_size) > 0) {
		pJpegenc_remapped_streamdata = (void *)ioremap_nocache(stream_phy_addr, PAGE_ALIGN(stream_size/*-PAGE_SIZE*/));
		if(pJpegenc_remapped_streamdata == NULL) {
			printk(KERN_ALERT "[stream] can not remap for jpeg\n");
			return -EFAULT;
		}
	} else {
		pJpegenc_remapped_streamdata = NULL;
	}

	if((header_phy_addr && header_size) > 0) {
		pJpegenc_remapped_header = (void *)ioremap_nocache(header_phy_addr, PAGE_ALIGN(header_size/*-PAGE_SIZE*/));
		if(pJpegenc_remapped_header == NULL) {
			printk(KERN_ALERT "[header] can not remap for jpeg\n");
			return -EFAULT;
		}
	} else {
		pJpegenc_remapped_header = NULL;
	}

	dprintk("raw   :  phy[0x%x ~ 0x%x], virt[0x%x ~ 0x%x], size = 0x%x \n",raw_phy_addr, 					\
								raw_phy_addr+raw_size, (unsigned int)pJpegenc_remapped_rawdata, 			\
								(unsigned int)pJpegenc_remapped_rawdata+raw_size, raw_size);
	dprintk("stream:  phy[0x%x ~ 0x%x], virt[0x%x ~ 0x%x], size = 0x%x \n",stream_phy_addr, 				\
								stream_phy_addr+stream_size, (unsigned int)pJpegenc_remapped_streamdata, 	\
								(unsigned int)pJpegenc_remapped_streamdata+stream_size, stream_size);
	dprintk("header:  phy[0x%x ~ 0x%x], virt[0x%x ~ 0x%x], size = 0x%x \n",header_phy_addr, 				\
								header_phy_addr + header_size, (unsigned int)pJpegenc_remapped_header, 		\
								(unsigned int)pJpegenc_remapped_header + header_size, header_size);

	gJPEGEnc_Buffer_Info.pBaseRawDataAddr 			= (void*)raw_phy_addr;
	gJPEGEnc_Buffer_Info.pBaseRawDataSize 			= raw_size;
	gJPEGEnc_Buffer_Info.pBaseBitstreamDataAddr 	= (void*)stream_phy_addr;
	gJPEGEnc_Buffer_Info.pBaseBitstreamDataSize 	= stream_size;
	gJPEGEnc_Buffer_Info.pBaseHeaderDataAddr 		= (void*)(stream_phy_addr + stream_size - jpeg_header_size);
	gJPEGEnc_Buffer_Info.pBaseHeaderDataSize 		= jpeg_header_size;

	return 0;
}

static int tccxx_exifheader_meminit(void *data)
{
	unsigned int header_phy_addr, header_size;
	unsigned int stream_phy_addr, stream_size;	
	TCCXXX_JPEG_ENC_EXIF_DATA *thumb_data;

	thumb_data = (TCCXXX_JPEG_ENC_EXIF_DATA*)data;
	
	if(pJpegenc_remapped_streamdata != NULL)
	{
		iounmap(pJpegenc_remapped_streamdata);
		pJpegenc_remapped_streamdata = NULL;
	}
	
	if(pJpegenc_remapped_header != NULL)
	{
		iounmap(pJpegenc_remapped_header);
		pJpegenc_remapped_header = NULL;
	}
	
	header_phy_addr = thumb_data->header_addr;
	header_size = thumb_data->header_size;

	if(header_phy_addr && header_size > 0)
	{
		pJpegenc_remapped_header = (void *)ioremap_nocache(header_phy_addr, PAGE_ALIGN(header_size));
		if (pJpegenc_remapped_header == NULL) {
			printk(KERN_ALERT "[header] can not remap for jpeg\n");
			return -EFAULT;
		}
	}
	else
	{
		pJpegenc_remapped_header = NULL;
	}

	stream_phy_addr = thumb_data->thumbjpg_addr;
	stream_size = thumb_data->thumbjpg_size;

	if(stream_phy_addr && stream_size > 0)
	{
		pJpegenc_remapped_streamdata = (void *)ioremap_nocache(stream_phy_addr, PAGE_ALIGN(stream_size));
		if (pJpegenc_remapped_streamdata == NULL) {
			printk(KERN_ALERT "[stream] can not remap for jpeg\n");
			return -EFAULT;
		}
	}
	else
	{
		pJpegenc_remapped_streamdata = NULL;
	}

	dprintk("ExifHeader	:: phy[0x%x ~ 0x%x], virt[0x%x ~ 0x%x], size = 0x%x \n",header_phy_addr, header_phy_addr+header_size, (unsigned int)pJpegenc_remapped_header, (unsigned int)pJpegenc_remapped_header+header_size, header_size);
	dprintk("ThumbJpeg	:: phy[0x%x ~ 0x%x], virt[0x%x ~ 0x%x], size = 0x%x \n",stream_phy_addr, stream_phy_addr+stream_size, (unsigned int)pJpegenc_remapped_streamdata, (unsigned int)pJpegenc_remapped_streamdata+stream_size, stream_size);

	return 0;
}

static int tccxx_enc_start(void)
{
	int ret = 0;

	if(enc_data.normal_op && jpgenc_data.dev_opened > 1) {
		dprintk("JpegDevice(Enc) is already using \n");
		return -EFAULT;
	}

	ret = tccxx_enc_meminit(&enc_data);
	dprintk("src: 0x%x, size: %d x %d !!\n", enc_data.source_addr, enc_data.width, enc_data.height);

	if(ret >= 0) {
		tccxx_enc_init((void*)enc_data.source_addr, enc_data.width, enc_data.height, enc_data.q_value, enc_data.src_format);

		gJpegEncodeDone = 0;
		JPEG_ENCODE_Start(enc_data.width, enc_data.height, &(enc_data.bitstream_size), &(enc_data.header_size));	
	}

	return ret;
}


static int tccxx_enc_finish(void)
{
	void *HeaderBuffer;
	int ret = 0;

	ret = JPEG_ENCODE_Get_Result(enc_data.width, enc_data.height, &(enc_data.bitstream_size), &(enc_data.header_size));

	dprintk("enc info :: %d = %d + %d \n", enc_data.header_size + enc_data.bitstream_size, enc_data.header_size, enc_data.bitstream_size);
	if(ret >= 0) {
		HeaderBuffer = pJpegenc_remapped_header;
		memcpy(pJpegenc_remapped_streamdata+enc_data.bitstream_size, HeaderBuffer, enc_data.header_size);

		enc_data.thumb_size = 0; //thumb is in header!!
		enc_data.bitstream_offset = 0;
		enc_data.header_offset = enc_data.bitstream_size;
	}

	return ret;
}


static int tccxx_make_exifheader(TCCXXX_JPEG_ENC_EXIF_DATA* arg)
{
	int ret = 0;
	TCCXXX_JPEG_ENC_EXIF_DATA thumb_data;

	if(copy_from_user(&thumb_data, arg, sizeof(TCCXXX_JPEG_ENC_EXIF_DATA)))
		return -EFAULT;

	ret = tccxx_exifheader_meminit(&thumb_data);
	
 	dprintk("ExifHeader stream: 0x%x [%d x %d], thumb: 0x%x - 0x%x[%d x %d] \n",
					thumb_data.bitstream_size, thumb_data.bitstream_width, thumb_data.bitstream_height,
					thumb_data.thumbjpg_addr, thumb_data.thumbjpg_size, thumb_data.thumb_width, thumb_data.thumb_height);

	if(ret >= 0)
	{	
		if(thumb_data.thumbjpg_size > 64*1024) //thumb is not jpeg but yuv. so we need to encode.
		{		
			printk("Err :: ExifHeader Max size over \n");
			return -EFAULT;
		}

		thumb_data.header_size = JPEG_Make_ExifHeader(&thumb_data, (unsigned int)pJpegenc_remapped_header);
	}
	dprintk("Header :: addr = 0x%x, size = 0x%x \n", thumb_data.header_addr, thumb_data.header_size);

	if(copy_to_user(arg, &thumb_data, sizeof(TCCXXX_JPEG_ENC_EXIF_DATA)))
		return -EFAULT;
	
	return ret;
}


/****************************************************************************
	IRQ_JPEG_ENC_Handler()

	Description : 
	Parameter : NONE
	Return : NONE
****************************************************************************/
static irqreturn_t tccxx_enc_handler(int irq, void *client_data)
{
#if !defined(CONFIG_ARCH_TCC892X)
	unsigned long IFlag, IAck;
	int_data_t *jpg_intr = client_data;
	volatile PJPEGENCODER pJPEGENC = (PJPEGENCODER)tcc_p2v(HwJPEGENCODER_BASE);

	spin_lock_irq(&(jpg_intr->lock));
	jpg_intr->count++;
	spin_unlock_irq(&(jpg_intr->lock));

	IFlag = pJPEGENC->JP_INT_FLAG;
	if(IFlag & HwJP_INT_FLAG_JOB_FINISHED)
	{				
		gJpegEncodeDone = 1;
		dprintk(KERN_ALERT "JPEG ENCODING DONE \n");
	}
	else if(IFlag & HwJP_INT_FLAG_CODED_BUF_STAT)
	{
		// Todo : 
	}

	IAck = pJPEGENC->JP_INT_ACK;
	wake_up_interruptible(&(jpg_intr->wq));
#endif // CONFIG_ARCH_TCC892X
	return IRQ_HANDLED;
}

extern int range_is_allowed(unsigned long pfn, unsigned long size);
static int tcc_jpegenc_mmap(struct file *file, struct vm_area_struct *vma)
{
	if(range_is_allowed(vma->vm_pgoff, vma->vm_end - vma->vm_start) < 0){
		printk(KERN_ERR  "jpegenc: this address is not allowed \n");
		return -EAGAIN;
	}

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	if(remap_pfn_range(vma,vma->vm_start, vma->vm_pgoff , vma->vm_end - vma->vm_start, vma->vm_page_prot))
	{
		return -EAGAIN;
	}

	vma->vm_ops		= NULL;
	vma->vm_flags 	|= VM_IO;
	vma->vm_flags 	|= VM_RESERVED;

	return 0;
}

static long tcc_jpegenc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int_data_t *jpg_data = (int_data_t *)file->private_data;
	int wait_ms = 10000;
	int ret = 0;


	switch(cmd) {
		case TCC_JPEGE_IOCTL_ENC:
			{
				mutex_lock(&jpg_data->io_mutex);
								
				if(copy_from_user(&enc_data, (TCCXXX_JPEG_ENC_DATA*)arg, sizeof(TCCXXX_JPEG_ENC_DATA))) {
					mutex_unlock(&jpg_data->io_mutex);
					return -EFAULT;
				}

				jpg_data->count = 0;
				tccxx_jpgenc_set_clock(0);

				if((ret = tccxx_enc_start()) >= 0) {
					if(enc_data.width <= 640 && enc_data.height <= 640) //exception proc::  jpeg error in case of specific size!!
						wait_ms = 100;
					
					ret = wait_event_interruptible_timeout(jpg_data->wq, jpg_data->count > 0, msecs_to_jiffies(wait_ms));
					if(ret <= 0) {
						printk("[%d]: jpegenc[%d x %d] timed_out!! \n", ret, enc_data.width, enc_data.height);
						ret = -EFAULT;
					} else {
						if(!gJpegEncodeDone) {
							ret = -EINVAL;
						} else {
							if((ret = tccxx_enc_finish()) >= 0) {
								if (copy_to_user((TCCXXX_JPEG_ENC_DATA*)arg, &enc_data, sizeof(TCCXXX_JPEG_ENC_DATA)))
									ret = -EFAULT;
							}
						}
					}
					
					JPEG_INTERRUPT_Disable(JPEG_ENCODE_MODE);
					JPEG_VIDEOCACHE_Drain();
				}

				tccxx_jpgenc_reset_clock(0);
				mutex_unlock(&jpg_data->io_mutex);
			}
			break;

		case TCC_JPEGE_IOCTL_MAKE_EXIF:
			ret = tccxx_make_exifheader((TCCXXX_JPEG_ENC_EXIF_DATA*)arg);
			break;

		case TCC_JPEGE_IOCTL_USABLE:
			{
				unsigned int usable = 0;
				
				if(jpgenc_data.dev_opened == 1)
					usable = 1;
					
				if (copy_to_user((unsigned int*)arg, &usable, sizeof(unsigned int)))
					ret = -EFAULT;
			}
			break;
			
		default:
			printk(KERN_ALERT "Not Supported JPEG_ENC_IOCTL(%d) \n", cmd);
			break;
	}

	return ret;
}

static int tcc_jpegenc_release(struct inode *inode, struct file *filp)
{
	jpgenc_data.dev_opened--;

	dprintk("tcc_jpegenc_release(%d) \n", jpgenc_data.dev_opened);

	if(!jpgenc_data.dev_opened)
	{
		if(jpgenc_data.irq_reged)
		{
			jpgenc_data.irq_reged = 0;
			#if !defined(CONFIG_ARCH_TCC892X)
			free_irq(IRQ_JPGE, &jpgenc_data);
			#endif // CONFIG_ARCH_TCC892X
		}

		if(pJpegenc_remapped_header != NULL)
			iounmap(pJpegenc_remapped_header);

		if(pJpegenc_remapped_rawdata != NULL)
			iounmap(pJpegenc_remapped_rawdata);
		
		if(pJpegenc_remapped_streamdata != NULL)
			iounmap(pJpegenc_remapped_streamdata);

		pJpegenc_remapped_header = NULL;
		pJpegenc_remapped_rawdata = NULL;
		pJpegenc_remapped_streamdata = NULL;
	}

	JPEG_VIDEOCACHE_Enable(0,0,0,0,0);
	tccxx_jpgenc_reset_clock(1);
	dprintk("tcc_jpegenc_release(%d) \n", jpgenc_data.dev_opened);
	
	return 0;
}

static int tcc_jpegenc_open(struct inode *inode, struct file *filp)
{
	int ret = 0;

	dprintk("tcc_jpegenc_open!!\n");
	
	if(!jpgenc_data.irq_reged)
	{
		#if !defined(CONFIG_ARCH_TCC892X)
		if((ret = request_irq(IRQ_JPGE, tccxx_enc_handler, IRQF_DISABLED, "jpeg_enc", &jpgenc_data)) < 0)
		{
			printk("FAILED to aquire jpeg_enc-irq\n");
			return -EFAULT;
		}
		#endif // CONFIG_ARCH_TCC892X
		jpgenc_data.irq_reged = 1;
	}
	tccxx_jpgenc_set_clock(1);
	
	filp->private_data = &jpgenc_data;
	jpgenc_data.dev_opened++;
	dprintk("tcc_jpegenc_open(%d) \n", jpgenc_data.dev_opened);

	return 0;	
	
}

static struct file_operations tcc_jpegenc_fops = 
{
	.owner			= THIS_MODULE,
	.unlocked_ioctl	= tcc_jpegenc_ioctl,
	.mmap			= tcc_jpegenc_mmap,
	.open			= tcc_jpegenc_open,
	.release		= tcc_jpegenc_release,
};

void __exit tcc_jpegenc_cleanup(void)
{
	unregister_chrdev(TCC_JPEGENC_MAJOR_ID, TCC_JPEGENC_DEVICE_NAME);

	clk_put(vcore_clk);
#ifdef USE_VCACHE		
	clk_put(vcache_clk);
#endif
	clk_put(jpege_clk);
	
	dprintk(" ===========> tcc_jpegenc_cleanup \n");

	return;
}


static char banner[] __initdata = KERN_INFO "TCCXX JPEG Encoder driver initializing\n";
static struct class *jpegenc_class;

int __init tcc_jpegenc_init(void)
{
	printk(banner);

	memset(&jpgenc_data, 0, sizeof(int_data_t));
	spin_lock_init(&(jpgenc_data.lock));
	init_waitqueue_head(&(jpgenc_data.wq));
	
	mutex_init(&jpgenc_data.io_mutex);
	
	vcore_clk = clk_get(NULL, "vcore");
	BUG_ON(vcore_clk == NULL);

#ifdef USE_VCACHE		
	vcache_clk = clk_get(NULL, "vcache");
	BUG_ON(vcache_clk == NULL);
#endif
	
	jpege_clk = clk_get(NULL, "jpege");
	BUG_ON(jpege_clk == NULL);
		
	register_chrdev(TCC_JPEGENC_MAJOR_ID, TCC_JPEGENC_DEVICE_NAME, &tcc_jpegenc_fops);
	
	jpegenc_class = class_create(THIS_MODULE, TCC_JPEGENC_DEVICE_NAME);
	device_create(jpegenc_class,NULL,MKDEV(TCC_JPEGENC_MAJOR_ID,TCC_JPEGENC_MINOR_ID),NULL,TCC_JPEGENC_DEVICE_NAME);

	pJpegenc_remapped_header = NULL;
	pJpegenc_remapped_rawdata = NULL;
	pJpegenc_remapped_streamdata = NULL;

	return 0;
}


MODULE_AUTHOR("Telechips.");
MODULE_DESCRIPTION("TCCXX JPEG Encoder driver");
MODULE_LICENSE("GPL");


module_init(tcc_jpegenc_init);
module_exit(tcc_jpegenc_cleanup);


