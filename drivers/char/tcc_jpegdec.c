/*
 * drivers/video/Tcc_jpeg_dec.c
 *
 * Copyright (C) 2004 Telechips, Inc. 
 *
 * jpeg decoder driver
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
#include <linux/videodev.h>
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
#include "tcc_jpegdec_app.h"
#include <mach/tcc_scaler_ioctrl.h>


#define TCC_JPEGDEC_MAJOR_ID 209
#define TCC_JPEGDEC_MINOR_ID 1
#define TCC_JPEGDEC_DEVICE_NAME "jpegdec"

#define dprintk(msg...)	if (0) { printk( "TCC_JPEGDEC: " msg); }

typedef struct _int_data_t {
	wait_queue_head_t wq;
	spinlock_t lock;
	unsigned int count;
	
	struct mutex io_mutex;
	
	unsigned char irq_reged;
	unsigned char dev_opened;
} int_data_t;
static int_data_t jpgdec_data;

JPEG_DEC_INFO  gJPEG_DEC_Info;
JPEG_BUFFER_CTRL gJPEGDec_Buffer_Info;
JPEG_DISPLAY_INFO JPEG_Info;
jpeg_parser_rsp gJpegInfo;

static TCCXXX_JPEG_DEC_DATA dec_data;
static unsigned int gJpegDecodeError = 0;

void *pJpegdec_remapped_rawdata, *pJpegdec_remapped_streamdata, *pJpegdec_remapped_header;

static struct clk *vcore_clk;
#ifdef USE_VCACHE		
static struct clk *vcache_clk;
#endif
static struct clk *jpegd_clk;

#ifdef CONFIG_CPU_FREQ
extern struct tcc_freq_table_t gtJpegClockLimitTable[];
extern struct tcc_freq_table_t gtJpegMaxClockLimitTable;
extern struct tcc_freq_table_t gtVpuNormalClockLimitTable[];
#endif

#ifdef CONFIG_VIDEO_TCCXX_CAMERA
extern int cif_get_capturedone(void);
extern int Get_Index_sizeTable(unsigned int Image_Size);
extern int tccxxx_isalive_camera(void);
#endif

extern void JPEG_SW_Reset(JpegOperationType mode);
extern void JPEG_INTERRUPT_Enable(JpegOperationType mode);
extern void JPEG_INTERRUPT_Disable(JpegOperationType mode);
extern void JPEG_VIDEOCACHE_Enable(unsigned char on, unsigned int read_min_addr, unsigned int read_max_addr, 
									unsigned int wrtie_min_addr, unsigned int wrtie_max_addr);
extern void JPEG_VIDEOCACHE_Drain(void);

static void tccxx_jpgdec_set_clock(unsigned char init)
{
	if(init)
	{
		clk_enable(vcore_clk);
#ifdef USE_VCACHE		
		clk_enable(vcache_clk);
#endif
		clk_enable(jpegd_clk);
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
#ifdef CONFIG_CPU_FREQ
			tcc_cpufreq_set_limit_table(&gtJpegMaxClockLimitTable, TCC_FREQ_LIMIT_JPEG, 1);
#endif
		}
	}
}

void tccxx_jpgdec_reset_clock(unsigned char deinit)
{
#ifdef CONFIG_CPU_FREQ
	tcc_cpufreq_set_limit_table(&gtJpegClockLimitTable[0], TCC_FREQ_LIMIT_JPEG, 0);
#endif

	if(deinit)
	{
		clk_disable(jpegd_clk);
#ifdef USE_VCACHE				
		clk_disable(vcache_clk);
#endif
	}
	else
	{
		clk_disable(vcore_clk);			
	}
}

int tccxx_decode_init(uint32 file_length, Decode_Scale_Type ratio, 
								uint32* ImageWidth, uint32* ImageHeight, 
								uint32* ImagePadWidth, uint32* ImagePadHeight, 
								uint32* ImageFormat)
{
	JPEG_PARSER_STATUS ucJpegParserStatus;
	unsigned int uiIFrmWidth, uiIBufHeight;
	unsigned int uiImageWidth, uiImageHeight;
	unsigned int ucIsPadding, uiDecodeScale;
	unsigned int usPaddingWidth, usPaddingHeight;

	void *virtHeader_addr, *virtStream_addr, *virtRaw_addr;
	void *phyStream_addr, *phyRaw_addr, *phyStream_RealAddr;
	char *temp_debug;
	
	phyStream_addr = gJPEGDec_Buffer_Info.pBaseBitstreamDataAddr;
	phyRaw_addr = gJPEGDec_Buffer_Info.pBaseRawDataAddr;	

	virtHeader_addr = pJpegdec_remapped_header;
	virtStream_addr = pJpegdec_remapped_streamdata;
	virtRaw_addr = pJpegdec_remapped_rawdata;	

	
	dprintk("filelen[0x%x], virtHeader_addr = 0x%x, virtStream_addr = 0x%x \n", file_length, (unsigned int)virtHeader_addr, (unsigned int)virtStream_addr);

	if(virtHeader_addr)
	{
		temp_debug = (char*)virtHeader_addr;
		dprintk("Header[0x%x, 0x%x, 0x%x  ~ 0x%x, 0x%x] \n", temp_debug[0], temp_debug[1], temp_debug[2], temp_debug[file_length-1], temp_debug[file_length]);
		temp_debug = (char*)virtStream_addr;
		dprintk("Stream[0x%x, 0x%x, 0x%x  ~ ] \n", temp_debug[0], temp_debug[1], temp_debug[2]);		
	}
	
	JPEG_SW_Reset(JPEG_DECODE_MODE);
	
	JPEG_Info.JpegFileLength = file_length;
	uiDecodeScale = ratio;

	memset(&gJpegInfo, 0x00, sizeof(gJpegInfo));

	if(virtHeader_addr)
	{
		JPEG_Info.JpegFileLength += 2;
		ucJpegParserStatus = JPEG_IMAGE_Parser(JPEG_Info.JpegFileLength, virtHeader_addr, TRUE, &gJpegInfo);
	}
	else
	{
		ucJpegParserStatus = JPEG_IMAGE_Parser(JPEG_Info.JpegFileLength, virtStream_addr, TRUE, &gJpegInfo);
	}

	dprintk("Parser_status[%d], source_virtual = 0x%x, HeaderSize = 0x%x \n", ucJpegParserStatus, (unsigned int)virtStream_addr,  gJpegInfo.HeaderSize);

	if (ucJpegParserStatus == JPEG_PARSER_SUCCESS) {
		if (gJpegInfo.ImgChromaType == IMAGE_CHROMA_422S)  //YUV422S output format is YUV420 at TCC92XX.
			gJpegInfo.ImgChromaType = IMAGE_CHROMA_420;

		phyStream_RealAddr = phyStream_addr;		
		if(virtHeader_addr == NULL)
		{
			// to increase performance and to remove needless memory-copy.
			phyStream_RealAddr = (void*)((unsigned int)phyStream_addr + gJpegInfo.HeaderSize);

			if((unsigned int)phyStream_RealAddr%8 != 0)
			{
				memcpy(virtStream_addr, (void*)((unsigned int)virtStream_addr+gJpegInfo.HeaderSize), gJpegInfo.BitStreamSize);
				phyStream_RealAddr = phyStream_addr;
			}
		}
	}
	else {
		printk("error[%d]: jpeg parser \n", ucJpegParserStatus);
		return -1;
	}

	if (gJpegInfo.IsNeedsPadding == TRUE) {
		uiImageWidth = gJpegInfo.PaddingWidth;
		uiImageHeight = gJpegInfo.PaddingHeight;
	}
	else {
		uiImageWidth = gJpegInfo.ImageWidth;
		uiImageHeight = gJpegInfo.ImageHeight;
	}

	//uiDecodeScale = JPEG_Dec_Scale_Set(uiImageWidth, uiImageHeight, gJpegInfo.ImgChromaType);
	uiIFrmWidth = JPEG_Dec_IFrame_Width(uiDecodeScale, uiImageWidth);
	uiIBufHeight = JPEG_Dec_IBuf_Height(uiDecodeScale, uiImageHeight);
	ImagePaddingForDecode(gJpegInfo.ImgChromaType, uiIFrmWidth, uiIBufHeight, &ucIsPadding, &usPaddingWidth, &usPaddingHeight);

	if (ucIsPadding) {
		gJpegInfo.IsNeedsPadding = TRUE;
		uiIFrmWidth = usPaddingWidth;
		uiIBufHeight = usPaddingHeight;
	}

	if(uiDecodeScale == 1){
		*ImageWidth = gJpegInfo.ImageWidth/2;
		*ImageHeight = gJpegInfo.ImageHeight/2;
	}
	else if(uiDecodeScale == 2){
		*ImageWidth = gJpegInfo.ImageWidth/4;
		*ImageHeight = gJpegInfo.ImageHeight/4;
	}
	else{
		*ImageWidth = gJpegInfo.ImageWidth;
		*ImageHeight = gJpegInfo.ImageHeight;
	}

	*ImagePadWidth = uiIFrmWidth;
	*ImagePadHeight = uiIBufHeight;

	dprintk("Tar = %d x %d, Chroma = %d \n", gJpegInfo.ImageWidth, gJpegInfo.ImageHeight, gJpegInfo.ImgChromaType);
	dprintk("Pad = %d x %d \n", uiIFrmWidth, uiIBufHeight);
	
	if (gJpegInfo.ImgChromaType == HwJP_CHROMA_420)
		*ImageFormat  = HwJP_CHROMA_420;
	else
		*ImageFormat  = HwJP_CHROMA_422;

	gJPEG_DEC_Info.lcd_buffer = phyRaw_addr;
	gJPEG_DEC_Info.cbuf_addr = (unsigned int)phyStream_RealAddr; //phyStream_addr;

	gJPEG_DEC_Info.cbuf_size = (gJPEGDec_Buffer_Info.pBaseBitstreamDataSize + 1023)/1024;
	gJPEG_DEC_Info.scale = uiDecodeScale;
	gJPEG_DEC_Info.ifrm_width = uiIFrmWidth;
	gJPEG_DEC_Info.ibuf_height = uiIBufHeight;
	gJPEG_DEC_Info.ImageFormat = gJpegInfo.ImgChromaType;

	JPEG_SET_Decode_Config(&gJPEG_DEC_Info);

	if(gJPEG_DEC_Info.ImageFormat == IMAGE_CHROMA_YONLY)
	{
		unsigned int Ydec, Udec, Vdec;
		
		Ydec = (unsigned int)virtRaw_addr;
		Udec = GET_ADDR_YUV42X_spU(Ydec , uiIFrmWidth, uiIBufHeight );
		Vdec = GET_ADDR_YUV422_spV(Udec , uiIFrmWidth, uiIBufHeight );

		memset((char*)Udec, 0x80, uiIFrmWidth*uiIBufHeight/2);
		memset((char*)Vdec, 0x80, uiIFrmWidth*uiIBufHeight/2);
	}
	
	JPEG_INTERRUPT_Enable(JPEG_DECODE_MODE);

	return 0;
}

int tccxx_meminit(void *data)
{
	TCCXXX_JPEG_DEC_DATA *Dec_data;
	unsigned int raw_phy_addr, raw_size;
	unsigned int stream_phy_addr, stream_size;
	unsigned int header_phy_addr, header_size;

	if(pJpegdec_remapped_rawdata != NULL)
		iounmap(pJpegdec_remapped_rawdata);

	if(pJpegdec_remapped_streamdata != NULL)
		iounmap(pJpegdec_remapped_streamdata);

	if(pJpegdec_remapped_header != NULL)
		iounmap(pJpegdec_remapped_header);
	
	Dec_data = (TCCXXX_JPEG_DEC_DATA *)data;

	//Target is Raw-data in case of Decode.
	raw_phy_addr = Dec_data->target_addr;
	raw_size = Dec_data->target_size;

	pJpegdec_remapped_rawdata = (void *)ioremap_nocache(raw_phy_addr, PAGE_ALIGN(raw_size/*-PAGE_SIZE*/));
	if (pJpegdec_remapped_rawdata == NULL) {
		printk(KERN_ALERT "[raw] can not remap for jpeg\n");
		return -EFAULT;
	}	

	if(Dec_data->split_body)
	{
		//Source is Stream-data in case of Decode.
		stream_phy_addr = Dec_data->stream_src_addr;
		stream_size = Dec_data->stream_src_len;

		//Source is Stream-data in case of Decode.
		header_phy_addr = Dec_data->header_src_addr;
		header_size = Dec_data->header_src_len + 2; //plus 2 for parser!!
		
		pJpegdec_remapped_header = (void *)ioremap_nocache(header_phy_addr, PAGE_ALIGN(header_size/*-PAGE_SIZE*/));
		if (pJpegdec_remapped_header == NULL) {
			printk(KERN_ALERT "[stream] can not remap for jpeg\n");
			return -EFAULT;
		}
	}
	else
	{
		//Source is Stream-data in case of Decode.
		stream_phy_addr = Dec_data->source_addr;
		stream_size = Dec_data->file_length;
		header_phy_addr = header_size = 0;
	
		pJpegdec_remapped_header = NULL;
	}
	
	pJpegdec_remapped_streamdata = (void *)ioremap_nocache(stream_phy_addr, PAGE_ALIGN(stream_size/*-PAGE_SIZE*/));
	if (pJpegdec_remapped_streamdata == NULL) {
		printk(KERN_ALERT "[stream] can not remap for jpeg\n");
		return -EFAULT;
	}

	//for parser in case of split header!!
	if(Dec_data->split_body)
	{
		char *virtHeader, *virtStream;

		virtHeader = (char*)pJpegdec_remapped_header;
		virtStream = (char*)pJpegdec_remapped_streamdata;
		virtHeader[header_size-2] = virtStream[0];
		virtHeader[header_size-1] = virtStream[1];		
	}

	dprintk("raw 	:: phy[0x%x ~ 0x%x], virt[0x%x ~ 0x%x], size = 0x%x \n",raw_phy_addr, raw_phy_addr+raw_size, (unsigned int)pJpegdec_remapped_rawdata, (unsigned int)pJpegdec_remapped_rawdata+raw_size, raw_size);
	dprintk("stream  :: phy[0x%x ~ 0x%x], virt[0x%x ~ 0x%x], size = 0x%x \n",stream_phy_addr, stream_phy_addr+stream_size, (unsigned int)pJpegdec_remapped_streamdata, (unsigned int)pJpegdec_remapped_streamdata+stream_size, stream_size);
	dprintk("header  :: phy[0x%x ~ 0x%x], virt[0x%x ~ 0x%x], size = 0x%x \n",header_phy_addr, header_phy_addr + header_size, (unsigned int)pJpegdec_remapped_header, (unsigned int)pJpegdec_remapped_header + header_size, header_size);

	gJPEGDec_Buffer_Info.pBaseRawDataAddr 			= (void*)raw_phy_addr;
	gJPEGDec_Buffer_Info.pBaseRawDataSize 			= raw_size;
	gJPEGDec_Buffer_Info.pBaseBitstreamDataAddr 	= (void*)stream_phy_addr;
	gJPEGDec_Buffer_Info.pBaseBitstreamDataSize 	= stream_size;
	gJPEGDec_Buffer_Info.pBaseHeaderDataAddr 		= (void*)header_phy_addr;
	gJPEGDec_Buffer_Info.pBaseHeaderDataSize 		= header_size;

	return 0;
}

int tccxx_decode(void)
{
	unsigned char ratio;
	int ret = 0;

	if(dec_data.normal_op && jpgdec_data.dev_opened > 1)
	{
		dprintk("JpegDevice(Dec) is already using \n");
		return -EFAULT;
	}

	ret = tccxx_meminit(&dec_data);

	if(ret >= 0)
	{
		dec_data.image_format=0;
		ratio = dec_data.ratio;
		
		if(0 > tccxx_decode_init(dec_data.file_length, ratio, 
									&(dec_data.width), &(dec_data.height), 
									&(dec_data.pad_width), &(dec_data.pad_height),
									&(dec_data.image_format))
		)
		{
			printk("not supported format \n");
			ret = -1;
		}
		else
		{
			dprintk(" Result: %dx%d [%dx%d], fmt: %d \n", dec_data.width, dec_data.height, dec_data.pad_width, dec_data.pad_height, dec_data.image_format);

			gJpegDecodeError = 0;
			JPEG_DECODE_Start(JPEG_DECODE_MODE);
		}

	}
		
	return ret;
}

int tccxx_get_headerInfo(TCCXXX_JPEG_DEC_DATA *arg)
{
	TCCXXX_JPEG_DEC_DATA dec_data;
	JPEG_PARSER_STATUS ucJpegParserStatus;
	void *virtStream_addr;
	int ret = 0;

	if(copy_from_user(&dec_data, arg, sizeof(TCCXXX_JPEG_DEC_DATA)))
		return -EFAULT;

	ret = tccxx_meminit(&dec_data);

	if(ret >= 0)
	{
		virtStream_addr = pJpegdec_remapped_streamdata;

		dprintk(" Check JPEG [addr: 0x%x, len: %d]===========> \n", (uint32)pJpegdec_remapped_streamdata, dec_data.file_length);

		JPEG_SW_Reset(JPEG_DECODE_MODE);

		ucJpegParserStatus = JPEG_IMAGE_Parser(dec_data.file_length, virtStream_addr, TRUE, &gJpegInfo);

		if (ucJpegParserStatus == JPEG_PARSER_SUCCESS)
		{
			dec_data.width 	 = gJpegInfo.ImageWidth;
			dec_data.height	 = gJpegInfo.ImageHeight;
			dec_data.pad_width  = gJpegInfo.PaddingWidth;
			dec_data.pad_height = gJpegInfo.PaddingHeight;
			if (gJpegInfo.ImgChromaType == HwJP_CHROMA_420)
				dec_data.image_format  = HwJP_CHROMA_420;
			else
				dec_data.image_format  = HwJP_CHROMA_422;

			if(dec_data.pad_width > 4095 || dec_data.pad_height > 4095)
			{
				printk("[size over]:: Impossible TCC H/W -Decode !!\n");
				ret = -1;
			}

			if(dec_data.width == 0 || dec_data.height == 0)
			{
				printk("[size 0x0]:: Impossible TCC H/W -Decode !!\n");
				ret = -1;
			}
			
			dprintk("Possible TCC-Decode - size: %dx%d !!\n", gJpegInfo.ImageWidth, gJpegInfo.ImageHeight);
		}	
		else
		{
			printk("[%d]:: Impossible TCC H/W -Decode !!\n", ucJpegParserStatus);
			ret = -1;
		}
	}
	if(copy_to_user(arg, &dec_data, sizeof(TCCXXX_JPEG_DEC_DATA)))
		return -EFAULT;
	
	return ret;
}


/****************************************************************************
	IRQ_JPEG_DEC_Handler()

	Description : 
	Parameter : NONE
	Return : NONE
****************************************************************************/
static irqreturn_t tccxx_dec_handler(int irq, void *client_data)
{
	unsigned long IFlag, IAck;
	int_data_t *jpg_intr = client_data;

	spin_lock_irq(&(jpg_intr->lock));
	jpg_intr->count++;
	spin_unlock_irq(&(jpg_intr->lock));

	IFlag = HwJPEGDEC->JP_INT_FLAG;

	if (IFlag & HwJP_INT_FLAG_DECODING_ERR)
	{
		gJpegDecodeError = 1;
		dprintk(KERN_ALERT "JPEG DECODING ERROR!! \n");
	}
	else if (IFlag & HwJP_INT_FLAG_JOB_FINISHED)
	{		
		dprintk(KERN_ALERT "JPEG DECODING DONE. \n");
	}

	IAck = HwJPEGDEC->JP_INT_ACK;
	wake_up_interruptible(&(jpg_intr->wq));

	return IRQ_HANDLED;
}

extern int range_is_allowed(unsigned long pfn, unsigned long size);
static int tcc_jpegdec_mmap(struct file *file, struct vm_area_struct *vma)
{
	if(range_is_allowed(vma->vm_pgoff, vma->vm_end - vma->vm_start) < 0){
		printk(KERN_ERR  "jpegdec: this address is not allowed \n");
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

static long tcc_jpegdec_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int_data_t *jpg_data = (int_data_t *)file->private_data;
	int wait_ms = 10000;	
	int ret = 0;


	switch (cmd)
	{
		case TCC_JPEGD_IOCTL_DEC:
			{
				mutex_lock(&jpg_data->io_mutex);
				
				if(copy_from_user(&dec_data, (TCCXXX_JPEG_DEC_DATA*)arg, sizeof(TCCXXX_JPEG_DEC_DATA)))
				{
					mutex_unlock(&jpg_data->io_mutex);
					return -EFAULT;
				}

				jpg_data->count = 0;
				
				tccxx_jpgdec_set_clock(0);

				if((ret = tccxx_decode()) >= 0) 
				{		
					if(gJpegInfo.ImageWidth <= 640 && gJpegInfo.ImageHeight <= 640)
						wait_ms = 500;
					
					ret = wait_event_interruptible_timeout(jpg_data->wq, jpg_data->count > 0, msecs_to_jiffies(wait_ms));
					if(ret <= 0)
					{
						printk("[%d]: jpegdec timed_out!! \n", ret);
						ret = -EFAULT;
					}
					else
					{
						if(gJpegDecodeError)
						{
							msleep(2);
							ret = -EINVAL;
						}
						else
						{
							if (copy_to_user((TCCXXX_JPEG_DEC_DATA*)arg, &dec_data, sizeof(TCCXXX_JPEG_DEC_DATA)))
								ret = -EFAULT;
						}
					}		
					
					JPEG_INTERRUPT_Disable(JPEG_DECODE_MODE);
					JPEG_VIDEOCACHE_Drain();					
				}

				tccxx_jpgdec_reset_clock(0);
				mutex_unlock(&jpg_data->io_mutex);
			}
			break;			

		case TCC_JPEGD_IOCTL_HEADER_INFO:
			ret = tccxx_get_headerInfo((TCCXXX_JPEG_DEC_DATA *)arg);
			break;

		case TCC_JPEGD_IOCTL_USABLE:
			{
				unsigned int usable = 0;
				
				if(jpgdec_data.dev_opened == 1)
					usable = 1;
					
				if (copy_to_user((unsigned int*)arg, &usable, sizeof(unsigned int)))
					ret = -EFAULT;
			}
			break;
			
		default:
			printk(KERN_ALERT "Not Supported JPEG_DEC_IOCTL(%d)\n", cmd);
			break;
	}


	return ret;
}

static int tcc_jpegdec_release(struct inode *inode, struct file *filp)
{
	jpgdec_data.dev_opened--;

	dprintk("tcc_jpegdec_release(%d) \n", jpgdec_data.dev_opened);

	if(!jpgdec_data.dev_opened)
	{
		if(jpgdec_data.irq_reged)
		{
			jpgdec_data.irq_reged = 0;
			
			free_irq(IRQ_JPGD, &jpgdec_data);
		}

		if(pJpegdec_remapped_rawdata != NULL)
			iounmap(pJpegdec_remapped_rawdata);
		
		if(pJpegdec_remapped_streamdata != NULL)
			iounmap(pJpegdec_remapped_streamdata);

		if(pJpegdec_remapped_header != NULL)
			iounmap(pJpegdec_remapped_header);

		pJpegdec_remapped_rawdata = NULL;
		pJpegdec_remapped_streamdata = NULL;
		pJpegdec_remapped_header = NULL;
	}

	JPEG_VIDEOCACHE_Enable(0,0,0,0,0);
	tccxx_jpgdec_reset_clock(1);
	dprintk("tcc_jpegdec_release(%d) \n", jpgdec_data.dev_opened);
	
	return 0;
}

static int tcc_jpegdec_open(struct inode *inode, struct file *filp)
{
	int ret = 0;

	dprintk("tcc_jpegdec_open!!\n");
	
	if(!jpgdec_data.irq_reged)
	{			
		if((ret = request_irq(IRQ_JPGD, tccxx_dec_handler, IRQF_DISABLED, "jpeg_dec", &jpgdec_data)) < 0)
		{
			printk("FAILED to aquire jpeg_dec-irq\n");
			return -EFAULT;
		}

		jpgdec_data.irq_reged = 1;
	}
	tccxx_jpgdec_set_clock(1);
	
	filp->private_data = &jpgdec_data;
	jpgdec_data.dev_opened++;
	dprintk("tcc_jpegdec_open(%d) \n", jpgdec_data.dev_opened);

	return 0;	
	
}

static struct file_operations tcc_jpegdec_fops = 
{
	.owner			= THIS_MODULE,
	.unlocked_ioctl	= tcc_jpegdec_ioctl,
	.mmap			= tcc_jpegdec_mmap,
	.open			= tcc_jpegdec_open,
	.release		= tcc_jpegdec_release,
};

void __exit
tcc_jpegdec_cleanup(void)
{
	unregister_chrdev(TCC_JPEGDEC_MAJOR_ID, TCC_JPEGDEC_DEVICE_NAME);

	clk_put(vcore_clk);
#ifdef USE_VCACHE			
	clk_put(vcache_clk);
#endif
	clk_put(jpegd_clk);
	
	dprintk(" ===========> tcc_jpegdec_cleanup \n");

	return;
}


static char banner[] __initdata = KERN_INFO "TCCXX JPEG driver initializing\n";
static struct class *jpegdec_class;

int __init 
tcc_jpegdec_init(void)
{
	printk(banner);

	memset(&jpgdec_data, 0, sizeof(int_data_t));
	spin_lock_init(&(jpgdec_data.lock));
	init_waitqueue_head(&(jpgdec_data.wq));

	mutex_init(&jpgdec_data.io_mutex);
	
	vcore_clk = clk_get(NULL, "vcore");
	BUG_ON(vcore_clk == NULL);

#ifdef USE_VCACHE			
	vcache_clk = clk_get(NULL, "vcache");
	BUG_ON(vcache_clk == NULL);
#endif

	jpegd_clk = clk_get(NULL, "jpegd");
	BUG_ON(jpegd_clk == NULL);
		
	register_chrdev(TCC_JPEGDEC_MAJOR_ID, TCC_JPEGDEC_DEVICE_NAME, &tcc_jpegdec_fops);
	
	jpegdec_class = class_create(THIS_MODULE, TCC_JPEGDEC_DEVICE_NAME);
	device_create(jpegdec_class,NULL,MKDEV(TCC_JPEGDEC_MAJOR_ID,TCC_JPEGDEC_MINOR_ID),NULL,TCC_JPEGDEC_DEVICE_NAME);

	pJpegdec_remapped_rawdata = NULL;
	pJpegdec_remapped_streamdata = NULL;
	pJpegdec_remapped_header = NULL;

	return 0;
}


MODULE_AUTHOR("Telechips.");
MODULE_DESCRIPTION("TCCXX JPEG Decoder driver");
MODULE_LICENSE("GPL");


module_init(tcc_jpegdec_init);
module_exit(tcc_jpegdec_cleanup);


