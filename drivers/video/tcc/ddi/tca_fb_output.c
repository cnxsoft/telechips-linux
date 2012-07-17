
/****************************************************************************
 *   FileName    : tca_backlight.c
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
#include <asm/mach-types.h>
#ifdef CONFIG_PM
#include <linux/pm.h>
#endif

#include <mach/bsp.h>
#include <mach/tca_ckc.h>
#include <mach/tca_lcdc.h>

#include <linux/cpufreq.h>
#include <linux/wait.h>
#include <linux/kthread.h>

#include <plat/pmap.h>
#include <mach/tccfb_ioctrl.h>
#include <mach/tcc_grp_ioctrl.h>
#include <mach/tcc_scaler_ctrl.h>
#include <mach/tca_fb_output.h>
#include <mach/tcc_composite_ioctl.h>
#include <mach/tcc_component_ioctl.h>
#include <mach/tca_fb_hdmi.h>

#if (defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC93XX)) && defined(CONFIG_DRAM_DDR2)
#define FB_SCALE_MAX_WIDTH		1920
#define FB_SCALE_MAX_HEIGHT		1080
#else
#define FB_SCALE_MAX_WIDTH		1280
#define FB_SCALE_MAX_HEIGHT		720
#endif


#if 0
#define dprintk(msg...)	 { printk( "tca_fb_output: " msg); }
#else
#define dprintk(msg...)	 
#endif

//#define FB_OUTPUT_TEST

char M2M_Scaler1_Ctrl_Detail(SCALER_TYPE *scale_img);
char M2M_Scaler_Ctrl_Detail(SCALER_TYPE *scale_img);
void grp_rotate_ctrl(G2D_BITBLIT_TYPE *g2d_p);

static char *fb_scaler_pbuf0;
static char *fb_scaler_pbuf1;
static char *fb_g2d_pbuf0;
static char *fb_g2d_pbuf1;

#if defined(CONFIG_TCC_OUTPUT_STARTER) && defined(CONFIG_TCC_OUTPUT_DUAL_UI)
static char *fb_scaler_pbuf2;
static char *fb_scaler_pbuf3;
static char *fb_g2d_pbuf2;
static char *fb_g2d_pbuf3;
#endif

#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
static char *exclusive_ui_src_pbuf;
static char *exclusive_ui_dst_pbuf;
static char *exclusive_ui_scale_pbuf;
static char *exclusive_ui_g2d_pbuf;
static char *exclusive_ui_viqe_pbuf;
#endif

#if defined(CONFIG_ARCH_TCC88XX)
static char *pMouseBuffer1;
static char *pMouseBuffer2;
static char MouseBufIdx = 0;
static unsigned int mouse_cursor_width = 0;
static unsigned int mouse_cursor_height = 0;

dma_addr_t		Gmap_dma;	/* physical */
u_char *		Gmap_cpu;	/* virtual */
#endif /* CONFIG_ARCH_TCC88XX */


/*****************************************************************************
*
* Defines
*
******************************************************************************/
//#define TCC_FB_UPSCALE

/*****************************************************************************
*
* structures
*
******************************************************************************/


typedef struct {
	unsigned int output;
	unsigned int ctrl;
	unsigned int width;
	unsigned int height;
	unsigned int interlace;
} output_video_img_info;

struct output_struct {
	spinlock_t lock;
	wait_queue_head_t waitq;
	char state;
};
static struct output_struct Output_struct;
static struct output_struct Output_lcdc0_struct;
static struct output_struct Output_lcdc1_struct;

/*****************************************************************************
*
* Variables
*
******************************************************************************/

/*****************************************************************************
*
* Functions
*
******************************************************************************/

static PLCDC pLCDC_OUTPUT[TCC_OUTPUT_MAX] = {NULL, NULL};

static struct clk *scaler1_clk;	 	// video image scale
static struct clk *scaler0_clk;		// fb image scale
static struct clk *lcdc0_output_clk;
static struct clk *lcdc1_output_clk;
static struct clk *g2d_clk;

static char output_lcdc[TCC_OUTPUT_MAX];
static char buf_index = 0;
static unsigned int FBimg_buf_addr;
static unsigned int output_layer_ctrl[TCC_OUTPUT_MAX] = {0, 0};

static unsigned char OutputType = 0;
static unsigned char UseVSyncInterrupt = 0;
static tcc_display_resize uiOutputResizeMode;
static lcdc_chroma_params output_chroma;
static output_video_img_info output_video_img;

static char output_3d_ui_flag = 0;
static char output_3d_ui_mode = 0;
static char output_ui_resize_count = 0;


#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
#define TCC_EXCLUSIVE_UI_BD_MIN_WD		1280
#define TCC_EXCLUSIVE_UI_BD_MIN_HT		720
#define TCC_EXCLUSIVE_UI_BUFFER_WD		1920
#define TCC_EXCLUSIVE_UI_BUFFER_HT		1080
#define TCC_EXCLUSIVE_UI_BUFFER_SIZE	1920*1080*4
#define TCC_EXCLUSIVE_UI_ADDR_OFFSET	TCC_EXCLUSIVE_UI_BUFFER_SIZE
#define TCC_EXCLUSIVE_UI_UPDATE_MAX		8
static int exclusive_ui_lcdc = -1;
static unsigned int exclusive_ui_index = 0;
static unsigned int exclusive_ui_priority = 0;
static exclusive_ui_frame exclusive_ui_img;
static exclusive_ui_params exclusive_ui_info;
static char exclusive_ui_onthefly = 0;
static char exclusive_ui_update_flag = 0;
static char exclusive_ui_clear_force = 1;
static char exclusive_ui_interlace_bypass = 0;
static unsigned int exclusive_ui_mixer_count = 0;
static unsigned int exclusive_ui_viqe_count = 0;
static exclusive_ui_update exclusive_ui_mixer;
static exclusive_ui_update exclusive_ui_viqe[TCC_EXCLUSIVE_UI_UPDATE_MAX];
static exclusive_ui_ar_params exclusive_ui_aspect_ratio;
static struct task_struct *exclusive_ui_task = NULL;

extern int grp_overlay_ctrl(G2D_OVERY_FUNC *overlay);
#if defined(CONFIG_FB_TCC_COMPONENT)
extern void tcc_component_process(struct tcc_lcdc_image_update *update, char force_update);
extern void tcc_component_display_image(exclusive_ui_update UpdateImage);
#endif
#if defined(CONFIG_FB_TCC_COMPOSITE)
extern void tcc_composite_process(struct tcc_lcdc_image_update *update, char force_update);
extern void tcc_composite_display_image(exclusive_ui_update UpdateImage);
#endif
#endif

extern unsigned int tcc_output_fb_get_disable(void);

#define MOUSE_CURSOR_MAX_WIDTH			200
#define MOUSE_CURSOR_MAX_HEIGHT			200

#define MOUSE_CURSOR_BUFF_SIZE		(MOUSE_CURSOR_MAX_WIDTH*MOUSE_CURSOR_MAX_HEIGHT*4)

#if defined(CONFIG_ARCH_TCC92XX)
#define FB_UPSCALE_SCALER0
#define FB_SCALER_ON_THE_FLY
#endif//

#ifndef FB_SCALER_ON_THE_FLY
#define TCC_SCALER_TEST

#ifdef TCC_SCALER_TEST

struct inode scaler_inode;
struct file scaler_filp;

int (*scaler_ioctl) (struct file *, unsigned int, unsigned long);
int (*scaler_open) (struct inode *, struct file *);
int (*scaler_release) (struct inode *, struct file *);

	#ifdef FB_UPSCALE_SCALER0
	int tccxxx_scaler_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
	int tccxxx_scaler_release(struct inode *inode, struct file *filp);
	int tccxxx_scaler_open(struct inode *inode, struct file *filp);
	#else
	int tccxxx_scaler1_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
	int tccxxx_scaler1_release(struct inode *inode, struct file *filp);
	int tccxxx_scaler1_open(struct inode *inode, struct file *filp);
	#endif//FB_UPSCALE_SCALER0
#endif//TCC_SCALER_TEST

#endif//FB_SCALER_ON_THE_FLY

struct inode g2d_inode;
struct file g2d_filp;

int tccxxx_grp_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
int tccxxx_grp_release(struct inode *inode, struct file *filp);
int tccxxx_grp_open(struct inode *inode, struct file *filp);

int (*g2d_ioctl) (struct file *, unsigned int, unsigned long);
int (*g2d_open) (struct inode *, struct file *);
int (*g2d_release) (struct inode *, struct file *);

char M2M_Scaler_OnTheFly(char OnOff)
{
	volatile PDDICONFIG pDDICONFIG;
	volatile PM2MSCALER pM2MSCALER;

	pDDICONFIG = (volatile PDDICONFIG)tcc_p2v(HwDDI_CONFIG_BASE);
	
	#ifdef FB_UPSCALE_SCALER0
	pM2MSCALER = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER0_BASE);
	#else
	pM2MSCALER = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER1_BASE);
	#endif//FB_UPSCALE_SCALER0

	if(OnOff)	{
		#ifdef FB_UPSCALE_SCALER0
		M2M_Scaler_SW_Reset(M2M_SCALER0);
		#else
		M2M_Scaler_SW_Reset(M2M_SCALER1);
		#endif//
		#ifdef CONFIG_ARCH_TCC92XX
			// lcdc 0
			#ifdef FB_UPSCALE_SCALER0
			BITCSET(pDDICONFIG->ON_THE_FLY, Hw2+Hw3, Hw2);
			#else
			BITCSET(pDDICONFIG->ON_THE_FLY, Hw4+Hw5, Hw4);
			#endif//
		#else
			// lcdc 1
			#ifdef FB_UPSCALE_SCALER0
			BITCSET(pDDICONFIG->ON_THE_FLY, Hw2+Hw3, 0);
			#else
			BITCSET(pDDICONFIG->ON_THE_FLY, Hw4+Hw5, 0);
			#endif//
		#endif//
	}
	else	{
		BITCSET(pM2MSCALER->MSCCTR, 0xffffffff, 0x00000000);
		
		#ifdef FB_UPSCALE_SCALER0		
		BITCSET(pDDICONFIG->ON_THE_FLY, Hw2+Hw3, Hw3);
		M2M_Scaler_SW_Reset(M2M_SCALER0);
		#else
		BITCSET(pDDICONFIG->ON_THE_FLY, Hw4+Hw5, Hw4+Hw5);
		M2M_Scaler_SW_Reset(M2M_SCALER1);
		#endif//
	}
	return 1;
}

static irqreturn_t TCC_OUTPUT_LCDC_Handler(int irq, void *dev_id)
{
	unsigned int LCDCstatus;

	if(pLCDC_OUTPUT[OutputType] == NULL)
	{
		printk("%s - Err: Output LCDC is not valid, OutputType=%d\n", __func__, OutputType);
		return IRQ_HANDLED;
	}

	LCDCstatus = pLCDC_OUTPUT[OutputType]->LSTATUS;

 	dprintk("%s lcdc_struct.state:%d STATUS:0x%x OutputType:%d \n",__func__, Output_struct.state, LCDCstatus, OutputType);

	pLCDC_OUTPUT[OutputType]->LSTATUS = 0xFFFFFFFF;

	if(Output_struct.state == 0){
		Output_struct.state = 1;
		wake_up_interruptible(&Output_struct.waitq);
	}

	return IRQ_HANDLED;
}

static irqreturn_t TCC_OUTPUT_LCDC0_Handler(int irq, void *dev_id)
{
	unsigned int LCDCstatus;
	PLCDC pLCDC = (PLCDC)tcc_p2v(HwLCDC0_BASE);

	LCDCstatus = pLCDC->LSTATUS;

 	dprintk("%s lcdc_struct.state:%d STATUS:0x%x\n", __func__, Output_lcdc0_struct.state, LCDCstatus);

	pLCDC->LSTATUS = 0xFFFFFFFF;

	if(Output_lcdc0_struct.state == 0){
		Output_lcdc0_struct.state = 1;
		wake_up_interruptible(&Output_lcdc0_struct.waitq);
	}

	return IRQ_HANDLED;
}

static irqreturn_t TCC_OUTPUT_LCDC1_Handler(int irq, void *dev_id)
{
	unsigned int LCDCstatus;
	PLCDC pLCDC = (PLCDC)tcc_p2v(HwLCDC1_BASE);

	LCDCstatus = pLCDC->LSTATUS;

 	dprintk("%s lcdc_struct.state:%d STATUS:0x%x\n", __func__, Output_lcdc1_struct.state, LCDCstatus);

	pLCDC->LSTATUS = 0xFFFFFFFF;

	if(Output_lcdc1_struct.state == 0){
		Output_lcdc1_struct.state = 1;
		wake_up_interruptible(&Output_lcdc1_struct.waitq);
	}

	return IRQ_HANDLED;
}

void TCC_OUTPUT_LCDC_Init(void)
{
	pmap_t pmap;

	lcdc0_output_clk = clk_get(0, "lcdc0");
	BUG_ON(lcdc0_output_clk == NULL);

	lcdc1_output_clk = clk_get(0, "lcdc1");
	BUG_ON(lcdc1_output_clk == NULL);

	scaler1_clk = clk_get(0, "m2m1");
	BUG_ON(scaler1_clk == NULL);	

	scaler0_clk = clk_get(0, "m2m0");
	BUG_ON(scaler0_clk == NULL);	

	g2d_clk = clk_get(NULL, "overlay");
	BUG_ON(g2d_clk == NULL);

	pmap_get_info("fb_scale0", &pmap);
	fb_scaler_pbuf0 = (char *) pmap.base;
	pmap_get_info("fb_scale1", &pmap);
	fb_scaler_pbuf1 = (char *) pmap.base;

	pmap_get_info("fb_g2d0", &pmap);
	fb_g2d_pbuf0 = (char *) pmap.base;
	pmap_get_info("fb_g2d1", &pmap);
	fb_g2d_pbuf1 = (char *) pmap.base;

	#if defined(CONFIG_TCC_OUTPUT_STARTER) && defined(CONFIG_TCC_OUTPUT_DUAL_UI)
	pmap_get_info("fb_scale2", &pmap);
	fb_scaler_pbuf2 = (char *) pmap.base;
	pmap_get_info("fb_scale3", &pmap);
	fb_scaler_pbuf3 = (char *) pmap.base;
	pmap_get_info("fb_g2d2", &pmap);
	fb_g2d_pbuf2 = (char *) pmap.base;
	pmap_get_info("fb_g2d3", &pmap);
	fb_g2d_pbuf3 = (char *) pmap.base;
	#endif

	pLCDC_OUTPUT[TCC_OUTPUT_NONE] = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);


#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
	pmap_get_info("exclusive_src", &pmap);
	exclusive_ui_src_pbuf = (char *) pmap.base; 
	pmap_get_info("exclusive_dst", &pmap);
	exclusive_ui_dst_pbuf = (char *) pmap.base; 
	pmap_get_info("exclusive_scale", &pmap);
	exclusive_ui_scale_pbuf = (char *) pmap.base; 
	pmap_get_info("exclusive_g2d", &pmap);
	exclusive_ui_g2d_pbuf = (char *) pmap.base; 
	pmap_get_info("exclusive_viqe", &pmap);
	exclusive_ui_viqe_pbuf = (char *) pmap.base; 

	TCC_OUTPUT_EXCLUSIVE_UI_Init();
#endif

	memset((void *)&output_chroma, 0x00, sizeof((void *)&output_chroma));

    init_waitqueue_head(&Output_struct.waitq);
	Output_struct.state = 1;

    init_waitqueue_head(&Output_lcdc0_struct.waitq);
	Output_lcdc0_struct.state = 1;
    init_waitqueue_head(&Output_lcdc1_struct.waitq);
	Output_lcdc1_struct.state = 1;

	#ifdef TCC_SCALER_TEST
		#ifdef FB_UPSCALE_SCALER0
		scaler_ioctl = tccxxx_scaler_ioctl;
		scaler_open = tccxxx_scaler_open;
		scaler_release = tccxxx_scaler_release;
		#else
		scaler_ioctl = tccxxx_scaler1_ioctl;
		scaler_open = tccxxx_scaler1_open;
		scaler_release = tccxxx_scaler1_release;
		#endif//
	#endif//

	g2d_ioctl = tccxxx_grp_ioctl;
	g2d_open = tccxxx_grp_open;
	g2d_release = tccxxx_grp_release;

#if defined(CONFIG_ARCH_TCC88XX)
	{
		unsigned int size = MOUSE_CURSOR_BUFF_SIZE;
		Gmap_cpu = dma_alloc_writecombine(0, size, &Gmap_dma, GFP_KERNEL);
		memset(Gmap_cpu, 0x00, MOUSE_CURSOR_BUFF_SIZE);

		pMouseBuffer1 = (char *)Gmap_dma;
		pMouseBuffer2 = (char *)(Gmap_dma + MOUSE_CURSOR_BUFF_SIZE/2);
	}
#endif /* CONFIG_ARCH_TCC88XX */
}



void TCC_OUTPUT_UPDATE_OnOff(char onoff)
{
	if(onoff)
	{
		clk_enable(scaler0_clk);
		clk_enable(scaler1_clk);
		clk_enable(g2d_clk);
		g2d_open((struct inode *)&g2d_inode, (struct file *)&g2d_filp);

		#ifdef FB_SCALER_ON_THE_FLY
			M2M_Scaler_OnTheFly(1);
		#elif defined(TCC_SCALER_TEST)
			scaler_open((struct inode *)&scaler_inode, (struct file *)&scaler_filp);
		#endif//

	}
	else
	{
		#ifdef FB_SCALER_ON_THE_FLY
			M2M_Scaler_OnTheFly(0);
		#elif defined(TCC_SCALER_TEST)
			scaler_release((struct inode *)&scaler_inode, (struct file *)&scaler_filp);
		#endif//

		clk_disable(scaler1_clk);
		clk_disable(scaler0_clk);
		clk_disable(g2d_clk);
		
		g2d_release((struct inode *)&g2d_inode, (struct file *)&g2d_filp);
	}
}

#ifdef CONFIG_CPU_FREQ
extern struct tcc_freq_table_t gtHdmiClockLimitTable;
#endif//CONFIG_CPU_FREQ
void TCC_OUTPUT_LCDC_OnOff(char output_type, char output_lcdc_num, char onoff)
{
	int i;
	dprintk(" %s output_type:%d lcdc_reg:0x%08x output_lcdc_num:%d onoff:%d  \n", __func__, output_type, pLCDC_OUTPUT[output_type], output_lcdc_num, onoff);

	if(onoff)
	{
		OutputType = output_type;

		if(output_lcdc_num)
		{						
			clk_enable(lcdc1_output_clk);

			pLCDC_OUTPUT[output_type] = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
			pLCDC_OUTPUT[output_type]->LCTRL = 0;
		#ifdef CONFIG_ARCH_TCC92XX
			pLCDC_OUTPUT[output_type]->LI0C = 0;
			pLCDC_OUTPUT[output_type]->LI2C = 0;
		#else
			pLCDC_OUTPUT[output_type]->LI0C = HwLCT_RU;
			pLCDC_OUTPUT[output_type]->LI2C = HwLCT_RU;
		#endif//
		}
		else
		{
			clk_enable(lcdc0_output_clk);

			pLCDC_OUTPUT[output_type] = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
			pLCDC_OUTPUT[output_type]->LCTRL = 0;
		#ifdef CONFIG_ARCH_TCC92XX
			pLCDC_OUTPUT[output_type]->LI0C = 0;
			pLCDC_OUTPUT[output_type]->LI2C = 0;
		#else
			pLCDC_OUTPUT[output_type]->LI0C = HwLCT_RU;
			pLCDC_OUTPUT[output_type]->LI2C = HwLCT_RU;
		#endif//
		}

		output_lcdc[output_type] = output_lcdc_num;
	#ifdef CONFIG_CPU_FREQ
		tcc_cpufreq_set_limit_table(&gtHdmiClockLimitTable, TCC_FREQ_LIMIT_HDMI, 1);
	#endif//CONFIG_CPU_FREQ


		TCC_OUTPUT_UPDATE_OnOff(1);

		tca_lcdc_interrupt_onoff(0, output_lcdc_num);
		
	#if defined(CONFIG_TCC_OUTPUT_DUAL_UI)
		if(output_lcdc_num)
		{
			request_irq(IRQ_LCD1, TCC_OUTPUT_LCDC1_Handler,	IRQF_SHARED,
			"TCC_LCD1",	TCC_OUTPUT_LCDC1_Handler);
		}
		else
		{
			request_irq(IRQ_LCD0, TCC_OUTPUT_LCDC0_Handler,	IRQF_SHARED,
			"TCC_LCD0",	TCC_OUTPUT_LCDC0_Handler);
		}
	#else
		if(output_lcdc_num)
		{
			request_irq(IRQ_LCD1, TCC_OUTPUT_LCDC_Handler,	IRQF_SHARED,
			"TCC_LCD",	TCC_OUTPUT_LCDC_Handler);
		}
		else
		{
			request_irq(IRQ_LCD0, TCC_OUTPUT_LCDC_Handler,	IRQF_SHARED,
			"TCC_LCD",	TCC_OUTPUT_LCDC_Handler);
		}
	#endif	
	}
	else
	{
		if(pLCDC_OUTPUT[output_type] != NULL)
		{
		#ifdef CONFIG_ARCH_TCC92XX
			pLCDC_OUTPUT[output_type]->LI0C = 0;
		#else
			pLCDC_OUTPUT[output_type]->LI0C = HwLCT_RU;
			pLCDC_OUTPUT[output_type]->LI2C = HwLCT_RU;
		#endif//

			pLCDC_OUTPUT[output_type]->LCTRL &= ~(HwLCTRL_LEN | HwLCTRL_MSEL);
			i = 0;
			while(i < 0xF0000)
			{
				volatile unsigned int status;

				status = pLCDC_OUTPUT[output_type]->LSTATUS;
				
				if(status & HwLSTATUS_DD)
				{
					dprintk(" lcdc disabled ! \n");
					break;
				}
				else
				{
					i++;
				}
			}
			pLCDC_OUTPUT[output_type] = NULL;

			dprintk("lcd disable time %d \n", i);

		#if defined(CONFIG_CPU_FREQ) && !defined(CONFIG_TCC_OUTPUT_DUAL_UI)
			tcc_cpufreq_set_limit_table(&gtHdmiClockLimitTable, TCC_FREQ_LIMIT_HDMI, 0);
		#endif//CONFIG_CPU_FREQ

			TCC_OUTPUT_UPDATE_OnOff(0);
			dprintk("lcd disable time %d \n", i);
			
			if(output_lcdc_num)	{						
				clk_disable(lcdc1_output_clk);
			}
			else {
				clk_disable(lcdc0_output_clk);
			}

		#if defined(CONFIG_TCC_OUTPUT_DUAL_UI)
			if(!output_lcdc_num)
				free_irq(IRQ_LCD0, TCC_OUTPUT_LCDC0_Handler);
			else
				free_irq(IRQ_LCD1, TCC_OUTPUT_LCDC1_Handler);
		#else
			if(!output_lcdc_num)
				free_irq(IRQ_LCD0, TCC_OUTPUT_LCDC_Handler);
			else
				free_irq(IRQ_LCD1, TCC_OUTPUT_LCDC_Handler);
		#endif
		}
	}

	memset((void *)output_layer_ctrl, 0x00, sizeof(output_layer_ctrl));
}

void TCC_OUTPUT_LCDC_CtrlLayer(char output_type, char interlace, char format)
{
	dprintk(" %s interlace:%d format:%d  \n", __func__, interlace, format);
	
	output_layer_ctrl[output_type] = HwLIC_IEN | HwLIC_CEN | format;

	if(interlace)
		output_layer_ctrl[output_type] |= HwLIC_INTL;
}

void TCC_OUTPUT_LCDC_CtrlChroma(lcdc_chroma_params lcdc_chroma)
{
	dprintk("%s\n", __func__);
	
	output_chroma.enable = lcdc_chroma.enable;
	output_chroma.color = lcdc_chroma.color;
}

char TCC_FB_G2D_FmtConvert(unsigned int width, unsigned int height, unsigned int g2d_rotate, unsigned int Sfmt, unsigned int Tfmt, unsigned int Saddr, unsigned int Taddr)
{
	G2D_BITBLIT_TYPE g2d_p;
	
	memset(&g2d_p, 0x00, sizeof(G2D_BITBLIT_TYPE));

	g2d_p.responsetype = G2D_POLLING;
	g2d_p.src0 = (unsigned int)Saddr;
	
	if(Sfmt == TCC_LCDC_IMG_FMT_RGB888)
		g2d_p.srcfm.format = GE_RGB888;
	else if(Sfmt == TCC_LCDC_IMG_FMT_YUV422SQ)
		g2d_p.srcfm.format = GE_YUV422_sq;		
	else
		g2d_p.srcfm.format = GE_RGB565;

	g2d_p.srcfm.data_swap = 0;
	g2d_p.src_imgx = width;
	g2d_p.src_imgy = height;

	g2d_p.ch_mode = g2d_rotate;

	g2d_p.crop_offx = 0;
	g2d_p.crop_offy = 0;
	g2d_p.crop_imgx = width;
	g2d_p.crop_imgy = height;

	g2d_p.tgt0 = (unsigned int)Taddr;	// destination image address

	if(Tfmt == TCC_LCDC_IMG_FMT_RGB888)
		g2d_p.tgtfm.format = GE_RGB888;
	else if(Tfmt == TCC_LCDC_IMG_FMT_YUV422SQ)
		g2d_p.tgtfm.format = GE_YUV422_sq;		
	else
		g2d_p.tgtfm.format = GE_RGB565;

	// destinaion f
	g2d_p.tgtfm.data_swap = 0;
	if((g2d_rotate == ROTATE_270) || (g2d_rotate == ROTATE_90)) 	{
		g2d_p.dst_imgx = height;
		g2d_p.dst_imgy = width;
	}
	else		{
		g2d_p.dst_imgx = width;
		g2d_p.dst_imgy = height;
	}
	
	g2d_p.dst_off_x = 0;
	g2d_p.dst_off_y = 0;
	g2d_p.alpha_value = 255;

	g2d_p.alpha_type = G2d_ALPHA_NONE;

	#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
		grp_rotate_ctrl(&g2d_p);
	#else
		g2d_ioctl((struct file *)&g2d_filp, TCC_GRP_ROTATE_IOCTRL_KERNEL, &g2d_p);
	#endif

	return 1;
}

void tcc_disable_external_display(unsigned int type, unsigned int disable)
{
	if((type == TCC_OUTPUT_NONE) || (pLCDC_OUTPUT[type] == NULL)){
		return 0;
}

	if(disable){
		/* Disable : CH2 of LCDC0 */
		pLCDC_OUTPUT[type]->LI2C &= ~HwLIC_IEN;
	#ifndef CONFIG_ARCH_TCC92XX
		pLCDC_OUTPUT[type]->LI2C |= HwLCT_RU;
	#endif	
		return 0;
	}
}

char TCC_OUTPUT_FB_Update(unsigned int width, unsigned int height, unsigned int bits_per_pixel, unsigned int addr, unsigned int type)
{
	unsigned int regl = 0, g2d_rotate_need = 0, g2d_format_need = 0, scaler_need= 0, ret = 0, m2m_onthefly_use = 0, interlace_output = 0;
	unsigned int lcd_width = 0, lcd_height = 0, lcd_h_pos = 0, lcd_w_pos = 0, scale_x = 0, scale_y = 0;
	unsigned int img_width = 0, img_height = 0, ifmt = 0;
	unsigned int chromaR = 0, chromaG = 0, chromaB = 0, alpha_ctrl = 0;
	PLCDC_CHANNEL pLCDC_OUTPUT_FB_CH;
	SCALER_TYPE fbscaler;
	char mode_3d = 0;
	
	memset(&fbscaler, 0x00, sizeof(SCALER_TYPE));

	if(pLCDC_OUTPUT[type] == NULL)
	{
		dprintk("%s - Err: Output LCDC is not valid, type=%d\n", __func__, type);
		return 0;
	}

	if(tcc_output_fb_get_disable()){
		pLCDC_OUTPUT[type]->LI2C &= ~HwLIC_IEN;
	#ifndef CONFIG_ARCH_TCC92XX
		pLCDC_OUTPUT[type]->LI2C |= HwLCT_RU;
	#endif	
		return 0;
	}else{
		pLCDC_OUTPUT[type]->LI2C |= HwLIC_IEN;
	}

	#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
		if(exclusive_ui_clear_force == 0)
		{
			dprintk("%s, force exclusive_ui not clear!!\n", __func__); 
			return 0;
		}
	#endif

	dprintk(" %s width=%d, height=%d, bpp=%d, type=%d\n", __func__, width, height, bits_per_pixel, type);

	regl = pLCDC_OUTPUT[type]->LDS; // get LCD size 
	lcd_width = (regl & 0xFFFF);
	lcd_height = ((regl>>16) & 0xFFFF);

	if(((!lcd_width) || (!lcd_height)) || ((lcd_width > TCC_FB_OUT_MAX_WIDTH) || (lcd_height > TCC_FB_OUT_MAX_HEIGHT)))
	{
		dprintk(" %s ERROR width=%d, height=%d, bpp=%d, type=%d LCD W:%d H:%d \n", __func__, width, height, bits_per_pixel, type, lcd_width, lcd_height);
		return 0;
	}

	#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
		if(output_chroma.enable)
		{
			chromaR = (output_chroma.color & 0x00FF0000) >> 16;
			chromaG = (output_chroma.color & 0x0000F000) >> 8;
			chromaB = (output_chroma.color & 0x000000FF) >> 0;
		}
		else
			chromaR = chromaG = chromaB = 0x00;
	#else
		chromaR = chromaG = chromaB = 0x00;
	#endif
	
	pLCDC_OUTPUT_FB_CH = (volatile PLCDC_CHANNEL)&pLCDC_OUTPUT[type]->LI2C;

	if(width < height)
		g2d_rotate_need = 1;

	if(lcd_width != width || lcd_height != height)
		scaler_need = 1;
		
	if(machine_is_tcc9300st() || machine_is_tcc8800st())
	{
		if(uiOutputResizeMode.resize_x != 0 || uiOutputResizeMode.resize_y != 0)
			scaler_need = 1;
	}
		
	if(TCC_OUTPUT_FB_Get3DMode(&mode_3d))
	{
		scaler_need = 1;
	}
	
	if(bits_per_pixel == 32 )
	{
		if(scaler_need)
		{
			g2d_format_need = 1;
		}
		ifmt = TCC_LCDC_IMG_FMT_RGB888;		
	}
	else
	{
		ifmt = TCC_LCDC_IMG_FMT_RGB565;
	}

	if(ISZERO(pLCDC_OUTPUT[type]->LCTRL, HwLCTRL_NI))//interlace mode
		interlace_output = 1;
	else
		interlace_output = 0;

	m2m_onthefly_use = 0;		

	#ifdef FB_SCALER_ON_THE_FLY
	if(scaler_need && (type == TCC_OUTPUT_HDMI) && (!interlace_output))
		m2m_onthefly_use = 1;
	#endif

	if(!scaler_need && !(g2d_rotate_need || g2d_format_need) )
		UseVSyncInterrupt = 1;
	else
		UseVSyncInterrupt = 0;

	img_width = width;
	img_height = height;
	FBimg_buf_addr = addr;

	dprintk(" %s width=%d, height=%d, bpp=%d, lcd_width=%d, lcd_height=%d, rotate=%d, format=%d, scale=%d, type=%d\n", 
			__func__, width, height, bits_per_pixel, lcd_width, lcd_height, g2d_rotate_need, g2d_format_need, scaler_need, type);
	
	if(uiOutputResizeMode.resize_x && uiOutputResizeMode.resize_y && TCC_OUTPUT_FB_Get3DMode(&mode_3d) && output_ui_resize_count)
	{
		char *pmap_cpu;
		unsigned int pmap_size = FB_SCALE_MAX_WIDTH*FB_SCALE_MAX_HEIGHT*2;
		unsigned int src_addr, dst_addr; 

		pmap_cpu = (unsigned)ioremap_nocache(fb_g2d_pbuf0, pmap_size);
		memset((char *)pmap_cpu, 0x00, pmap_size);
		iounmap(pmap_cpu);

		src_addr = fb_g2d_pbuf0;

		if(buf_index)
			dst_addr = fb_scaler_pbuf0;
		else
			dst_addr = fb_scaler_pbuf1;
				
		TCC_FB_G2D_FmtConvert(FB_SCALE_MAX_WIDTH, FB_SCALE_MAX_HEIGHT, NOOP, TCC_LCDC_IMG_FMT_RGB565, TCC_LCDC_IMG_FMT_YUV422SQ, src_addr, dst_addr);
		
		if(output_ui_resize_count)
			output_ui_resize_count--;
	}
	

	if(g2d_rotate_need || g2d_format_need)
	{
		unsigned int rotate, taddr; 
		
		if(g2d_rotate_need)		{
			img_width = height;
			img_height= width;
			rotate = ROTATE_270;
		}
		else		{
			img_width = width;
			img_height = height;
			rotate = NOOP;
		}

		#if defined(CONFIG_TCC_OUTPUT_STARTER) && defined(CONFIG_TCC_OUTPUT_DUAL_UI)
		if(output_lcdc[type])
		{
			if(buf_index)
				taddr = fb_g2d_pbuf2;	// destination image address
			else
				taddr = fb_g2d_pbuf3;	// destination image address
		}
		else
		#endif
		{
			if(buf_index)
				taddr = fb_g2d_pbuf0;	// destination image address
			else
				taddr = fb_g2d_pbuf1;	// destination image address
		}

		TCC_FB_G2D_FmtConvert(width, height, rotate, ifmt, TCC_LCDC_IMG_FMT_YUV422SQ, addr, taddr );
	
		ifmt = TCC_LCDC_IMG_FMT_YUV422SQ;
	
		FBimg_buf_addr = taddr;
	}

	if(scaler_need)
	{
		#ifdef TCC_SCALER_TEST
		fbscaler.responsetype = SCALER_POLLING;
		#else
		fbscaler.responsetype = SCALER_NOWAIT;
		#endif//
		
		fbscaler.src_Yaddr = (char *)FBimg_buf_addr;

		if(ifmt == TCC_LCDC_IMG_FMT_YUV422SQ)
			fbscaler.src_fmt = SCALER_YUV422_sq0;
		else
			fbscaler.src_fmt = SCALER_RGB565;

		fbscaler.src_ImgWidth = img_width;
		fbscaler.src_ImgHeight = img_height;

		fbscaler.src_winLeft = 0;
		fbscaler.src_winTop = 0;
		fbscaler.src_winRight = img_width;
		fbscaler.src_winBottom = img_height;

		#if defined(CONFIG_TCC_OUTPUT_STARTER) && defined(CONFIG_TCC_OUTPUT_DUAL_UI)
		if(output_lcdc[type])
		{
			if(buf_index)
				fbscaler.dest_Yaddr = fb_scaler_pbuf2;	// destination image address
			else
				fbscaler.dest_Yaddr = fb_scaler_pbuf3;	// destination image address
		}
		else
		#endif
		{
			if(buf_index)
				fbscaler.dest_Yaddr = fb_scaler_pbuf0;	// destination image address
			else
				fbscaler.dest_Yaddr = fb_scaler_pbuf1;	// destination image address
		}

		buf_index = !buf_index;

		if((lcd_width > FB_SCALE_MAX_WIDTH) && (!m2m_onthefly_use))
		{
			m2m_onthefly_use = 0;
			img_width = lcd_width / 2;
		}
		else	{
			img_width = lcd_width;
		}

		if((lcd_height > FB_SCALE_MAX_HEIGHT) && (!m2m_onthefly_use)) 	{
			m2m_onthefly_use = 0;
			img_height = lcd_height/2;
		}
		else	{
			img_height = lcd_height;
		}

		if(TCC_OUTPUT_FB_Get3DMode(&mode_3d))
		{
			int iCount = 0;
			int x_offset, y_offset, img_wd, img_ht;

			ifmt = TCC_LCDC_IMG_FMT_YUV422SQ;
			fbscaler.dest_fmt = SCALER_YUV422_sq0;	// destination image format
			fbscaler.dest_ImgWidth = img_width;		// destination image width
			fbscaler.dest_ImgHeight = img_height; 	// destination image height
			fbscaler.viqe_onthefly = 0;

			x_offset = (uiOutputResizeMode.resize_x << 4);
			y_offset = (uiOutputResizeMode.resize_y << 3);
			img_wd = img_width - x_offset;
			img_ht = img_height - y_offset;

			for(iCount=0; iCount<2; iCount++)
			{
				/* 3D MKV : SBS(SideBySide) Mode */
				if(mode_3d == 0)	
				{
					fbscaler.dest_winTop = (y_offset>>1);
					fbscaler.dest_winBottom = fbscaler.dest_winTop + img_ht;

					if(iCount == 0)
					{
						fbscaler.dest_winLeft = (x_offset>>2);
						fbscaler.dest_winRight = fbscaler.dest_winLeft + (img_wd>>1);
					}
					else
					{
						fbscaler.dest_winLeft = (img_width>>1) + (x_offset>>2);
						fbscaler.dest_winRight = fbscaler.dest_winLeft + (img_wd>>1);
					}
				}
				else
				/* 3D MKV : TNB(Top&Bottom) Mode */
				{
					fbscaler.dest_winLeft = (x_offset>>1);
					fbscaler.dest_winRight = fbscaler.dest_winLeft + img_wd;

					if(iCount == 0)
					{
						fbscaler.dest_winTop = (y_offset>>2);
						fbscaler.dest_winBottom = fbscaler.dest_winTop + (img_ht>>1);
					}
					else
					{
						fbscaler.dest_winTop = (img_height>>1) + (y_offset>>2);
						fbscaler.dest_winBottom = fbscaler.dest_winTop + (img_ht>>1);
					}
				}

				#ifdef FB_UPSCALE_SCALER0
					ret = DEV_M2M_Wait_signal_disable(0);
				#else
					ret = DEV_M2M_Wait_signal_disable(1);
				#endif//

				if((ret == 0) || (ret >= M2M_WAIT_MAX))		{
					printk("ERROR : %s M2M not ready to run %d \n",__func__, ret);
					return 0;
				}
				
				#ifdef FB_UPSCALE_SCALER0
				M2M_Scaler_Ctrl_Detail(&fbscaler);
				#else
				M2M_Scaler1_Ctrl_Detail(&fbscaler);
				#endif//
			}
		}
		else
		{
			if(machine_is_tcc9300st() || machine_is_tcc8800st())
			{
				img_width -= uiOutputResizeMode.resize_x << 4;
				img_height -= uiOutputResizeMode.resize_y << 3;

	            lcd_w_pos = uiOutputResizeMode.resize_x << 3;
				if( interlace_output )
					lcd_h_pos = uiOutputResizeMode.resize_y << 1;
				else
					lcd_h_pos = uiOutputResizeMode.resize_y << 2;
			}

			ifmt = TCC_LCDC_IMG_FMT_YUV422SQ;
			fbscaler.dest_fmt = SCALER_YUV422_sq0;		// destination image format
			fbscaler.dest_ImgWidth = img_width;		// destination image width
			fbscaler.dest_ImgHeight = img_height; 	// destination image height
			fbscaler.dest_winLeft = 0;
			fbscaler.dest_winTop = 0;
			fbscaler.dest_winRight = img_width;
			fbscaler.dest_winBottom = img_height;

			#ifdef FB_SCALER_ON_THE_FLY
			if(!m2m_onthefly_use)
			{
				fbscaler.viqe_onthefly = 0;
				
				#ifdef FB_UPSCALE_SCALER0
				ret = DEV_M2M_Wait_signal_disable(0);
				#else
				ret = DEV_M2M_Wait_signal_disable(1);
				#endif//
				
				if((ret == 0) || (ret >= M2M_WAIT_MAX))		{
					printk("ERROR : %s M2M not ready to run %d \n",__func__, ret);
					return 0;
				}
			}
			else
			{
				fbscaler.viqe_onthefly = 0x2;
			}
			#else
			fbscaler.viqe_onthefly = 0;

			#ifdef TCC_SCALER_TEST
				ret =  1;
			#else
				#ifdef FB_UPSCALE_SCALER0
					ret = DEV_M2M_Wait_signal_disable(0);
				#else
					ret = DEV_M2M_Wait_signal_disable(1);
				#endif//
			#endif//

			if((ret == 0) || (ret >= M2M_WAIT_MAX))		{
				printk("ERROR : %s M2M not ready to run %d \n",__func__, ret);
				return 0;
			}
			#endif//
			
			#ifdef TCC_SCALER_TEST
				scaler_ioctl((struct file *)&scaler_filp, TCC_SCALER_IOCTRL_KERENL, &fbscaler);
			#else
				#ifdef FB_UPSCALE_SCALER0
				M2M_Scaler_Ctrl_Detail(&fbscaler);
				#else
				M2M_Scaler1_Ctrl_Detail(&fbscaler);
				#endif//
			#endif//
		}

		FBimg_buf_addr = fbscaler.dest_Yaddr;

			
	}

// lcd image offset
	pLCDC_OUTPUT_FB_CH->LIO = img_width * 2;

// position
	pLCDC_OUTPUT_FB_CH->LIP = ((lcd_h_pos << 16) | (lcd_w_pos));

	if(machine_is_tcc9300st() || machine_is_tcc8800st())
	{
		scale_x = 0;
		scale_y = 0;
	}
	else
	{
		// scale 2น่
		if(lcd_width > img_width)
			scale_x = 9;
		else
			scale_x = 0;

		if(lcd_height > img_height)
			scale_y = 9;
		else
			scale_y = 0;
	}

	pLCDC_OUTPUT_FB_CH->LISR =((scale_y<<4) | scale_x);
// size
	pLCDC_OUTPUT_FB_CH->LIS =((img_height << 16) | (img_width));

// image address
//	pLCDC_OUTPUT_FB_CH->LIBA0 = fbscaler.dest_Yaddr;

//channel control
	BITCSET (pLCDC_OUTPUT_FB_CH->LIKR, 0xff <<  0, (chromaR)  <<  0); // key
	BITCSET (pLCDC_OUTPUT_FB_CH->LIKR, 0xff << 16, (0xF8)	<< 16); // keymask
	BITCSET (pLCDC_OUTPUT_FB_CH->LIKG, 0xff <<  0, (chromaG)  <<  0); // key
	BITCSET (pLCDC_OUTPUT_FB_CH->LIKG, 0xff << 16, (0xFC)	<< 16); // keymask
	BITCSET (pLCDC_OUTPUT_FB_CH->LIKB, 0xff <<  0, (chromaB)  <<  0); // key
	BITCSET (pLCDC_OUTPUT_FB_CH->LIKB, 0xff << 16, (0xF8)	<< 16); // keymask

	if(output_layer_ctrl[type])
	{
		BITCSET(output_layer_ctrl[type], 0x1F, ifmt);

		if(ifmt >= TCC_LCDC_IMG_FMT_YUV420SP)
			pLCDC_OUTPUT_FB_CH->LIC = output_layer_ctrl[type] | HwLIC_Y2R;
		else
		pLCDC_OUTPUT_FB_CH->LIC = output_layer_ctrl[type];
	}
	else
	{
		#ifdef FB_SCALER_ON_THE_FLY
		if(m2m_onthefly_use)
			ifmt |=  HwLIC_SRC;
		#endif//

		if(ifmt >= TCC_LCDC_IMG_FMT_YUV420SP)
			ifmt |= HwLIC_Y2R;

		#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
			if(ifmt == TCC_LCDC_IMG_FMT_RGB888)
				alpha_ctrl = HwLIC_AEN | HwLIC_ASEL;
		#endif

		if(interlace_output)//interlace mode
			pLCDC_OUTPUT_FB_CH->LIC = HwLIC_INTL | HwLIC_IEN | ifmt | HwLIC_CEN | alpha_ctrl;
		else
			pLCDC_OUTPUT_FB_CH->LIC = HwLIC_IEN | ifmt | HwLIC_CEN | alpha_ctrl;
	}

	switch(pLCDC_OUTPUT_FB_CH->LIC & 0x0000001F)
	{
		case TCC_LCDC_IMG_FMT_RGB444:
		case TCC_LCDC_IMG_FMT_RGB565:
		case TCC_LCDC_IMG_FMT_RGB555:
			pLCDC_OUTPUT_FB_CH->LIO = img_width * 2;
			break;

		case TCC_LCDC_IMG_FMT_RGB888:
		case TCC_LCDC_IMG_FMT_RGB666:
			pLCDC_OUTPUT_FB_CH->LIO = img_width * 4;
			break;
		
		case TCC_LCDC_IMG_FMT_YUV420SP:
		case TCC_LCDC_IMG_FMT_YUV422SP:
			pLCDC_OUTPUT_FB_CH->LIO = ((img_width / 2)<<16) | (img_width);
			break;

		case TCC_LCDC_IMG_FMT_YUV422SQ:
			pLCDC_OUTPUT_FB_CH->LIO = img_width * 2;
			break;

		case TCC_LCDC_IMG_FMT_YUV420ITL0:
		case TCC_LCDC_IMG_FMT_YUV420ITL1:
		case TCC_LCDC_IMG_FMT_YUV422ITL0:
		case TCC_LCDC_IMG_FMT_YUV422ITL1:
			pLCDC_OUTPUT_FB_CH->LIO = (img_width << 16) | (img_width);
			break;
	}
	
	#ifndef CONFIG_ARCH_TCC92XX
	pLCDC_OUTPUT_FB_CH->LIC |= HwLCT_RU;
	#endif//
	
	#ifdef FB_SCALER_ON_THE_FLY
	if(m2m_onthefly_use)
		pLCDC_OUTPUT[type]->LCTRL	|= (HwLCTRL_LEN | /* HwLCTRL_MSEL|*/ (Hw3+Hw1));
	else
		pLCDC_OUTPUT[type]->LCTRL	|= (HwLCTRL_LEN | HwLCTRL_MSEL | (Hw3+Hw1));
	#else
	pLCDC_OUTPUT[type]->LCTRL	|= (HwLCTRL_LEN | /* HwLCTRL_MSEL|*/ (Hw3+Hw1));
	#endif//FB_SCALER_ON_THE_FLY
	
	pLCDC_OUTPUT[type]->LSTATUS	|= HwLSTATUS_RU;

	dprintk(" end g2d_rotate_need=%d g2d_format_need=%d, scaler_need=%d end\n", g2d_rotate_need, g2d_format_need, scaler_need);

	return 1;
}


void TCC_OUTPUT_FB_UpdateSync(unsigned int type)
{
	PLCDC_CHANNEL pLCDC_OUTPUT_FB_CH;

	if(pLCDC_OUTPUT[type] == NULL)
	{
		dprintk("%s - Err: Output LCDC is not valid, type=%d\n", __func__, type);
		return;
	}

	#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
		if(exclusive_ui_clear_force == 0)
		{
			dprintk("%s, force exclusive_ui not clear!!\n", __func__); 
			return;
		}
	#endif

	dprintk("%s type=%d UseVSyncInterrupt=%d\n", __func__, type, UseVSyncInterrupt);

	pLCDC_OUTPUT_FB_CH = (volatile PLCDC_CHANNEL)&pLCDC_OUTPUT[type]->LI2C;

	pLCDC_OUTPUT_FB_CH->LIBA0 = FBimg_buf_addr;
#ifndef CONFIG_ARCH_TCC92XX
	pLCDC_OUTPUT_FB_CH->LIC |= HwLCT_RU;
#endif//
	//pjj
	FBimg_buf_addr = 0;

	if(UseVSyncInterrupt)
	{
		TCC_OUTPUT_FB_WaitVsyncInterrupt(type);
	}
}

void TCC_OUTPUT_FB_WaitVsyncInterrupt(unsigned int type)
{
	int ret;
	
	if(type != OutputType)
		type = OutputType;

	if(pLCDC_OUTPUT[type] == NULL)
	{
		dprintk("%s - Err: Output LCDC is not valid, OutputType=%d\n", __func__, type);
		return;
	}

	pLCDC_OUTPUT[type]->LSTATUS = 0xFFFFFFFF; 
	tca_lcdc_interrupt_onoff(TRUE, output_lcdc[type]);

	#if defined(CONFIG_TCC_OUTPUT_DUAL_UI)
		if(output_lcdc[type] == 0)
		{
			Output_lcdc0_struct.state = 0;
			ret = wait_event_interruptible_timeout(Output_lcdc0_struct.waitq, Output_lcdc0_struct.state == 1, msecs_to_jiffies(50));
		}
		else
		{
			Output_lcdc1_struct.state = 0;
			ret = wait_event_interruptible_timeout(Output_lcdc1_struct.waitq, Output_lcdc1_struct.state == 1, msecs_to_jiffies(50));
		}
	#else
		Output_struct.state = 0;
		ret = wait_event_interruptible_timeout(Output_struct.waitq, Output_struct.state == 1, msecs_to_jiffies(50));
	#endif

	if(ret <= 0)	{
	 	printk("  [%d]: tcc_setup_interrupt timed_out!! \n", ret);
	}
}

int TCC_OUTPUT_SetOutputResizeMode(tcc_display_resize mode)
{
	printk("%s : mode = %d\n", __func__, mode);
	
	uiOutputResizeMode = mode;
}

int TCC_OUTPUT_FB_BackupVideoImg(char output_type)
{
	PLCDC_CHANNEL pLCDC_CH;
	
	if(pLCDC_OUTPUT[output_type] == NULL)
		return -1;

	pLCDC_CH = (volatile PLCDC_CHANNEL)&pLCDC_OUTPUT[output_type]->LI0C;

	if(pLCDC_CH->LIC & HwLIC_IEN)
	{
		output_video_img.output = output_type;
		output_video_img.ctrl = pLCDC_CH->LIC;
		output_video_img.width = ((pLCDC_OUTPUT[output_type]->LDS>>0) & 0xFFFF);
		output_video_img.height	= ((pLCDC_OUTPUT[output_type]->LDS>>16) & 0xFFFF);

		if(pLCDC_OUTPUT[output_type]->LCTRL & HwLCTRL_NI)
			output_video_img.interlace = 1;
		else
			output_video_img.interlace = 0;

		dprintk("%s, ctrl=0x%08x, width=%d, height=%d, interlace=%d\n", __func__, 
				output_video_img.ctrl, output_video_img.width, output_video_img.height, output_video_img.interlace);
	}

	return 0;
}

int TCC_OUTPUT_FB_RestoreVideoImg(char output_type)
{
	PLCDC_CHANNEL pLCDC_CH;
	unsigned int lcd_width, lcd_height, lcd_interlace;
	
	if(pLCDC_OUTPUT[output_type] == NULL)
		return -1;

	lcd_width = ((pLCDC_OUTPUT[output_type]->LDS>>0 ) & 0xFFFF);
	lcd_height = ((pLCDC_OUTPUT[output_type]->LDS>>16) & 0xFFFF);
		
	if(pLCDC_OUTPUT[output_type]->LCTRL & HwLCTRL_NI)
		lcd_interlace = 1;
	else
		lcd_interlace = 0;

	pLCDC_CH = (volatile PLCDC_CHANNEL)&pLCDC_OUTPUT[output_type]->LI0C;

	if(output_video_img.ctrl & HwLIC_IEN)
	{
		if( (output_video_img.output == output_type) &&
			(output_video_img.width == lcd_width) &&
			(output_video_img.height == lcd_height) &&
			(output_video_img.interlace == lcd_interlace) )
		{
			#if defined(CONFIG_ARCH_TCC92XX)
				pLCDC_CH->LIC = output_video_img.ctrl;
			#else
				pLCDC_CH->LIC = output_video_img.ctrl | HwLCT_RU;
			#endif
			dprintk("%s, ctrl=0x%08x\n", __func__, output_video_img.ctrl);
		}
	}

	memset((void *)&output_video_img, 0x00, sizeof((void *)&output_video_img));

	return 0;
}

int TCC_OUTPUT_FB_Set3DMode(char enable, char mode)
{
	dprintk("%s, 3D mode=%d\n", __func__, mode);

	output_3d_ui_flag = enable;
	output_3d_ui_mode = mode;

	output_ui_resize_count = 4;

	return 0;
}

int TCC_OUTPUT_FB_Get3DMode(char *mode)
{
	dprintk("%s, flag=%d, mode=%d\n", __func__, output_3d_ui_flag, output_3d_ui_mode);

	*mode = output_3d_ui_mode;

	return output_3d_ui_flag;
}

#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
int TCC_OUTPUT_EXCLUSIVE_SetBusPriority(unsigned int bus, unsigned int rw, unsigned int priority)
{
    bus &= 0xf;
    priority &= 0xff;

    // slot<<24 | priority<<8 | SI num<<0
    if ( rw == 1 ) {
        *(volatile unsigned int *)(0xF0300000 + 0x40C) = (bus<<24) | (priority<<8) | (0<<0);
    }
    else {
        *(volatile unsigned int *)(0xF0300000 + 0x408) = (bus<<24) | (priority<<8) | (0<<0);
    }
}

int TCC_OUTPUT_EXCLUSIVE_GetBusPriority(unsigned int bus, unsigned int rw)
{
    int priority;
    bus &= 0xf;

    // slot<<24 | priority<<8 | SI num<<0
    *(volatile unsigned int *)(0xF0300000 + 0x0000 + 0x0) = (0xff<<24) | (bus<<0);
    if ( rw == 1 ) {
        priority = *(volatile unsigned int *)(0xF0300000 + 0x40C);
    }
    else {
        priority = *(volatile unsigned int *)(0xF0300000 + 0x408);
    }
    priority = (priority>>8) & 0xff;

    return (priority);
}

int TCC_OUTPUT_EXCLUSIVE_CfgBusPriority(char bus, char config)
{
	int i;
	unsigned int data, priority;
	volatile unsigned long *pReg;
	volatile unsigned long *pIOMCFG = (volatile unsigned long *)tcc_p2v(*(volatile unsigned long*)0xB0080014);
	#if 0
	volatile unsigned long *pQT0 = (volatile unsigned long *)tcc_p2v(*(volatile unsigned long*)0xB0300400);
	volatile unsigned long *pQA0 = (volatile unsigned long *)tcc_p2v(*(volatile unsigned long*)0xB0300404);
	#endif
	//unsigned long priority[10] = { 9, 8, 2, 3, 4, 0, 5, 6, 7, 1 };

	printk("%s, bus=%d, config=%d\n", __func__, bus, config);
	
	//pReg = pIOMCFG;
	//*pReg = 0x00008000;
	
	#if 0
	pReg = pQT0;
	data = 0x3;
	*pReg = data;
	printk("[pQT0] address=0x%08x, data=0x%08x\n", pQT0, *pReg);

	pReg = pQA0;
	data = 0x200;
	*pReg = data;
	printk("[pQA0] address=0x%08x, data=0x%08x\n", pQA0, *pReg);
	#endif

	if(config)
		exclusive_ui_priority = TRUE;
	else
		exclusive_ui_priority = FALSE;

	dprintk("[AR0] address=0x%08x\n", 0xB0300408);
	for(i=0; i<=9; i++)
	{
		if(config)
		{
			if(i==bus)
				TCC_OUTPUT_EXCLUSIVE_SetBusPriority(i, 0, 0);
			else
				TCC_OUTPUT_EXCLUSIVE_SetBusPriority(i, 0, 1);
		}
		else
			TCC_OUTPUT_EXCLUSIVE_SetBusPriority(i, 0, 0);
	}
	
	for(i=0; i<=9; i++)
	{
		data = TCC_OUTPUT_EXCLUSIVE_GetBusPriority(i, 0);
		dprintk("[AR0:%d] = 0x%08x\n", i, data);
	}

	dprintk("[AW0] address=0x%08x\n", 0xB030040C);
	for(i=0; i<=9; i++)
	{
		if(config)
		{
			if(i==bus)
				TCC_OUTPUT_EXCLUSIVE_SetBusPriority(i, 1, 0);
			else
				TCC_OUTPUT_EXCLUSIVE_SetBusPriority(i, 1, 1);
		}
		else
			TCC_OUTPUT_EXCLUSIVE_SetBusPriority(i, 1, 0);
	}
	
	for(i=0; i<=9; i++)
	{
		data = TCC_OUTPUT_EXCLUSIVE_GetBusPriority(i, 1);
		dprintk("[AW0:%d] = 0x%08x\n", i, data);
	}
}

int TCC_OUTPUT_EXCLUSIVE_UI_Thread(void *data)
{
	int count;
	
	while(1)
	{
		if(exclusive_ui_viqe_count)
		{
			/* Overlay Mixer */
			TCC_OUTPUT_EXCLUSIVE_UI_Mixer(&exclusive_ui_viqe[0].image, exclusive_ui_viqe[0].index, 0);
			/* M2M Scaler without On_the_fly */
			TCC_OUTPUT_EXCLUSIVE_UI_Scaler(&exclusive_ui_viqe[0].image, 1, exclusive_ui_viqe[0].wd, exclusive_ui_viqe[0].ht, exclusive_ui_viqe[0].index, 1);
			/* Update Screen */
			TCC_HDMI_DISPLAY_UPDATE(0, &exclusive_ui_viqe[0].image);

			if(exclusive_ui_viqe_count)
			{
				exclusive_ui_viqe_count--;
				if(exclusive_ui_viqe_count)
				{
					for(count=0; count<exclusive_ui_viqe_count; count++)
						memcpy(&exclusive_ui_viqe[count], &exclusive_ui_viqe[count+1], sizeof(exclusive_ui_update));
				}

				dprintk("%d\n", exclusive_ui_viqe_count);
			}
		}

		msleep(0);
	}

	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_Init(void)
{
	dprintk("%s\n", __func__);
	
	exclusive_ui_info.enable		= 0;
	exclusive_ui_info.bd			= 1;
	exclusive_ui_info.plane_width	= TCC_EXCLUSIVE_UI_BUFFER_WD;
	exclusive_ui_info.plane_height	= TCC_EXCLUSIVE_UI_BUFFER_HT;
	exclusive_ui_info.base_addr_dma	= 0;
	exclusive_ui_info.base_addr_cpu	= 0;
	exclusive_ui_info.interlace		= FALSE;
	exclusive_ui_info.process		= 0;
	exclusive_ui_info.update		= FALSE;
	exclusive_ui_info.clear 		= TRUE;

	exclusive_ui_img.buf_wd = TCC_EXCLUSIVE_UI_BUFFER_WD;
	exclusive_ui_img.buf_ht = TCC_EXCLUSIVE_UI_BUFFER_HT;
	exclusive_ui_img.src_x = 0;
	exclusive_ui_img.src_y = 0;
	exclusive_ui_img.src_wd = TCC_EXCLUSIVE_UI_BUFFER_WD;
	exclusive_ui_img.src_ht = TCC_EXCLUSIVE_UI_BUFFER_HT;
	exclusive_ui_img.dst_x = 0;
	exclusive_ui_img.dst_y = 0;
	exclusive_ui_img.pBaseAddr = NULL;

	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_SetParam(exclusive_ui_params exclusive_ui_param)
{
	unsigned int ui_size;
	unsigned int lcdc_width, lcdc_height;
	PLCDC pLCDC;
	PLCDC_CHANNEL pLCDC_CH;
  
	#if 0
	memcpy(&exclusive_ui_info, &exclusive_ui_param, sizeof(exclusive_ui_params));
	#else
	exclusive_ui_info.enable		= exclusive_ui_param.enable;
	exclusive_ui_info.plane_width	= exclusive_ui_param.plane_width;
	exclusive_ui_info.plane_height	= exclusive_ui_param.plane_height;
	#endif

	/* Get the base address */
	if(exclusive_ui_info.base_addr_dma != (unsigned)exclusive_ui_src_pbuf)
	{
		ui_size = TCC_EXCLUSIVE_UI_BUFFER_SIZE*2;

		exclusive_ui_img.pBaseAddr = exclusive_ui_info.base_addr_dma = (unsigned)exclusive_ui_src_pbuf;

	#if 0 //for Testing
		/* Unmap virtual address */
		if(exclusive_ui_info.base_addr_cpu)
		{
 			iounmap(exclusive_ui_info.base_addr_cpu);
			exclusive_ui_info.base_addr_cpu = 0;			
		}

		/* Remap virtual address */
 		exclusive_ui_info.base_addr_cpu = (unsigned)ioremap_nocache(exclusive_ui_info.base_addr_dma, ui_size);
 		if(exclusive_ui_info.base_addr_cpu == 0)
		{
			printk("%s, Err: Memory Allocation Error, size=%d\n", __func__, ui_size);
			return -1;
		}

		if(0)
		{
			unsigned int w, h;
			unsigned int alpha = 0;
			unsigned int *pBuffer = (unsigned int *)exclusive_ui_info.base_addr_cpu;
			
			memset((void*)exclusive_ui_info.base_addr_cpu, 0x00, ui_size);

			for(h=0; h<exclusive_ui_info.plane_height; h++)
			{
				for(w=0; w<exclusive_ui_info.plane_width; w++)
				{
					*(pBuffer + (h*exclusive_ui_info.plane_width) + w) = 0x00880000 | (alpha<<24);
					//*(pBuffer + (h*exclusive_ui_info.plane_width) + w) = 0x00000000;
				}

				alpha++;
				if(alpha >= 255)
					alpha = 0;
			}
		}
	#else
		exclusive_ui_info.base_addr_cpu = 0;
	#endif
	}

	/* Set the defaut value */
	exclusive_ui_info.bd			= 1;
	exclusive_ui_info.interlace 	= FALSE;
	exclusive_ui_info.process 		= 0;
	exclusive_ui_info.update 		= FALSE;
	exclusive_ui_info.clear			= TRUE;

	exclusive_ui_priority			= FALSE;
	exclusive_ui_onthefly			= FALSE;
	exclusive_ui_update_flag		= 0;
	exclusive_ui_clear_force		= 1;

	exclusive_ui_interlace_bypass	= 0;
	//output_3d_ui_flag			= 0;
	//output_3d_ui_mode			= 0;

	/* Clear information of aspect ratio */
	//memset((void *)&exclusive_ui_aspect_ratio, 0x00, sizeof(exclusive_ui_ar_params));
		
	/* Clear lcdc num */
	exclusive_ui_lcdc = -1;

	/* Update LCDC CH1 directly */
	if(pLCDC_OUTPUT[TCC_OUTPUT_HDMI] != NULL)
		exclusive_ui_lcdc = output_lcdc[TCC_OUTPUT_HDMI];
	else if(pLCDC_OUTPUT[TCC_OUTPUT_COMPONENT] != NULL)
		exclusive_ui_lcdc = output_lcdc[TCC_OUTPUT_COMPONENT];
	else if(pLCDC_OUTPUT[TCC_OUTPUT_COMPOSITE] != NULL)
		exclusive_ui_lcdc = output_lcdc[TCC_OUTPUT_COMPOSITE];

	if(exclusive_ui_lcdc >= 0)
	{
	 	if (exclusive_ui_lcdc)
			pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
		else
			pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
		lcdc_width	= ((pLCDC->LDS>>0 ) & 0xFFFF);
		lcdc_height	= ((pLCDC->LDS>>16) & 0xFFFF);
	
		if( (exclusive_ui_info.plane_width == TCC_FB_OUT_MAX_WIDTH) && (lcdc_width == TCC_FB_OUT_MAX_WIDTH) &&
			(exclusive_ui_info.plane_height == TCC_FB_OUT_MAX_HEIGHT) && (lcdc_height == TCC_FB_OUT_MAX_HEIGHT) )
		{
			//if(exclusive_ui_priority == FALSE)
			//	TCC_OUTPUT_EXCLUSIVE_CfgBusPriority(9, TRUE);
		}
	}

	printk("%s, width=%d, height=%d, phy_addr=0x%08x\n", __func__, exclusive_ui_info.plane_width, exclusive_ui_info.plane_height, exclusive_ui_info.base_addr_dma);

	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_GetParam(exclusive_ui_params *pExclusive_ui_param)
{
	dprintk("%s, enable=%d, interlace=%d\n", __func__, exclusive_ui_info.enable, exclusive_ui_info.interlace);
	
	memcpy(pExclusive_ui_param, &exclusive_ui_info, sizeof(exclusive_ui_params));

	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_SetEnable(char enable)
{
	exclusive_ui_info.enable = enable;

	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_GetEnable(void)
{
	return (int)exclusive_ui_info.enable;
}

int TCC_OUTPUT_EXCLUSIVE_UI_SetSize(exclusive_ui_size ui_size)
{
	printk("%s, width=%d, height=%d\n", __func__, ui_size.width, ui_size.height);
	
	/* Set the size information */
	exclusive_ui_info.plane_width	= ui_size.width;
	exclusive_ui_info.plane_height	= ui_size.height;

	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_SetInterlace(unsigned int interlace)
{
	printk("%s, interlace=%d, exclusive_ui_lcdc=%d\n", __func__, interlace, exclusive_ui_lcdc);

	if(interlace)
		exclusive_ui_info.interlace = 1;
	else
		exclusive_ui_info.interlace = 0;

	if(exclusive_ui_lcdc >= 0)
	{
		exclusive_ui_onthefly = FALSE;
		TCC_OUTPUT_EXCLUSIVE_UI_SetOnTheFly_Scaler(exclusive_ui_lcdc, 0, FALSE);
	}
	
	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_GetInterlace(void)
{
	return (int)exclusive_ui_info.interlace;
}

int TCC_OUTPUT_EXCLUSIVE_UI_SetImage(struct tcc_lcdc_image_update *ImageInfo)
{
	dprintk("%s\n", __func__);

	exclusive_ui_info.update = FALSE;
	memcpy(&exclusive_ui_info.update_img, ImageInfo, sizeof(struct tcc_lcdc_image_update));

	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_GetAddr(unsigned int *pBaseAddr)
{
	dprintk("%s, base_address=0x%08x\n", __func__, exclusive_ui_info.base_addr_dma);
	
	if(exclusive_ui_info.base_addr_dma == 0)
	{
		dprintk("%s, Err: Memory Allocation Error\n", __func__);
		return -1;
	}

	/* Set the base address */
	*pBaseAddr = exclusive_ui_info.base_addr_dma;

	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_SetUpdate(char value)
{
	exclusive_ui_update_flag = value;

	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_GetUpdate(void)
{
	return (int)exclusive_ui_update_flag;
}

int TCC_OUTPUT_EXCLUSIVE_UI_SetRatio(char aspect_ratio)
{
	printk("%s, aspect_ratio=%d\n", __func__, aspect_ratio);

	memset((void *)&exclusive_ui_aspect_ratio, 0x00, sizeof(exclusive_ui_ar_params));
	
	exclusive_ui_aspect_ratio.ratio = aspect_ratio;

	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_GetRatio(void)
{
	return (int)exclusive_ui_aspect_ratio.ratio;
}

int TCC_OUTPUT_EXCLUSIVE_UI_SetAR(unsigned int aspect_ratio_x, unsigned int aspect_ratio_y)
{
	printk("%s, aspect_ratio_x=%d, aspect_ratio_y=%d\n", __func__, aspect_ratio_x, aspect_ratio_y);

	memset((void *)&exclusive_ui_aspect_ratio, 0x00, sizeof(exclusive_ui_ar_params));
	
	exclusive_ui_aspect_ratio.ratio = 0xff;
	exclusive_ui_aspect_ratio.ratio_x = aspect_ratio_x;
	exclusive_ui_aspect_ratio.ratio_y = aspect_ratio_y;

	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_GetAR(exclusive_ui_ar_params *aspect_ratio)
{
	memcpy((void*)aspect_ratio, (void *)&exclusive_ui_aspect_ratio, sizeof(exclusive_ui_ar_params));

	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_CfgAR(char lcdc_num, unsigned int img_wd, unsigned int img_ht, exclusive_ui_ar_params *aspect_ratio)
{
	unsigned int lcd_width, lcd_height;
	unsigned int scale_limit_wd, scale_limit_ht, scale_limit_total;
	PLCDC pLCDC;
  
	/* Check the state */
	if(exclusive_ui_info.enable == FALSE)
	{
		dprintk("%s Err: EXCLUSIVE_UI is not enabled!!\n", __func__);
		memset((void *)&exclusive_ui_aspect_ratio, 0x00, sizeof(exclusive_ui_ar_params));
		memset((void *)aspect_ratio, 0x00, sizeof(exclusive_ui_ar_params));
		return -1;
	}

 	/* Set the output resolution */
 	if (lcdc_num)
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
	else
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);

	lcd_width = (pLCDC->LDS & 0xFFFF);
	lcd_height = ((pLCDC->LDS>>16) & 0xFFFF);
	
	if(exclusive_ui_aspect_ratio.config == TRUE)
	{
		memcpy((void*)aspect_ratio, (void *)&exclusive_ui_aspect_ratio, sizeof(exclusive_ui_ar_params));
		return -1;
	}

	if(exclusive_ui_aspect_ratio.ratio != 0xff)
	{
		switch(exclusive_ui_aspect_ratio.ratio)
		{
			/* Normal */
			case 0:
				exclusive_ui_aspect_ratio.ratio_x = img_wd;
				exclusive_ui_aspect_ratio.ratio_y = img_ht;
				break;

			/* 4:3 Aspect Ratio */
			case 1:
				exclusive_ui_aspect_ratio.ratio_x = 4;
				exclusive_ui_aspect_ratio.ratio_y = 3;
				break;

			/* 16:9 Aspect Ratio */
			case 2:
				exclusive_ui_aspect_ratio.ratio_x = 16;
				exclusive_ui_aspect_ratio.ratio_y = 9;
				break;

			default:
				exclusive_ui_aspect_ratio.ratio_x = 0;
				exclusive_ui_aspect_ratio.ratio_y = 0;
				break;
		}
	}

	if(exclusive_ui_aspect_ratio.ratio_x && exclusive_ui_aspect_ratio.ratio_y)
	{
		exclusive_ui_aspect_ratio.ar_ht = (lcd_width*exclusive_ui_aspect_ratio.ratio_y)/exclusive_ui_aspect_ratio.ratio_x;
		if(exclusive_ui_aspect_ratio.ar_ht <= lcd_height)
		{
			exclusive_ui_aspect_ratio.ar_wd = lcd_width;

			#if 0
			if((exclusive_ui_aspect_ratio.ar_wd < lcd_width) && (exclusive_ui_aspect_ratio.ar_ht < lcd_height))
			{
				exclusive_ui_aspect_ratio.ar_wd = (height*exclusive_ui_aspect_ratio.ratio_x)/exclusive_ui_aspect_ratio.ratio_y;
				exclusive_ui_aspect_ratio.ar_ht = height;
			}
			#endif
		}
		else
		{
			exclusive_ui_aspect_ratio.ar_wd = (lcd_height*exclusive_ui_aspect_ratio.ratio_x)/exclusive_ui_aspect_ratio.ratio_y;
			exclusive_ui_aspect_ratio.ar_ht = lcd_height;
		}

	 	/* Check the scale limitation */
		scale_limit_wd 		= (exclusive_ui_aspect_ratio.ar_wd*10) / img_wd;
		scale_limit_ht		= (exclusive_ui_aspect_ratio.ar_ht*10) / img_ht;
		scale_limit_total 	= scale_limit_wd * scale_limit_ht;
		
		if((scale_limit_wd >= 38 || scale_limit_ht >= 38))
		{
			if(scale_limit_wd >= 38)
			{
				exclusive_ui_aspect_ratio.ar_wd	= (img_wd * 38) / 10;
				exclusive_ui_aspect_ratio.ar_wd	= ((exclusive_ui_aspect_ratio.ar_wd+7)>>3)<<3;
				if(exclusive_ui_aspect_ratio.ar_wd > lcd_width)
					exclusive_ui_aspect_ratio.ar_wd = lcd_width;
			}

			if(scale_limit_ht >= 38)
			{
				exclusive_ui_aspect_ratio.ar_ht 	= (img_ht * 38) / 10;
				exclusive_ui_aspect_ratio.ar_ht 	= ((exclusive_ui_aspect_ratio.ar_ht+1)>>1)<<1;
				if(exclusive_ui_aspect_ratio.ar_ht > lcd_height)
					exclusive_ui_aspect_ratio.ar_ht = lcd_height;
			}

			dprintk("%s, scaler limitation, aspect_width=%d, aspect_height=%d\n", __func__, exclusive_ui_aspect_ratio.ar_wd, exclusive_ui_aspect_ratio.ar_ht);
		}

		exclusive_ui_aspect_ratio.ar_wd = ((exclusive_ui_aspect_ratio.ar_wd+7)>>3)<<3;
		exclusive_ui_aspect_ratio.ar_ht = ((exclusive_ui_aspect_ratio.ar_ht+1)>>1)<<1;

		exclusive_ui_aspect_ratio.enable = TRUE;
	}

	memcpy((void*)aspect_ratio, (void *)&exclusive_ui_aspect_ratio, sizeof(exclusive_ui_ar_params));
	
	exclusive_ui_aspect_ratio.config = TRUE;

	printk("%s enable=%d, ratio=%d, img_wd=%d, img_ht=%d, lcd_wd=%d, lcd_ht=%d, ar_wd=%d, ar_ht=%d\n", __func__, exclusive_ui_aspect_ratio.enable, 
			exclusive_ui_aspect_ratio.ratio, img_wd, img_ht, lcd_width, lcd_height, exclusive_ui_aspect_ratio.ar_wd, exclusive_ui_aspect_ratio.ar_ht);
	
	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_SetBypass(char bypass)
{
	dprintk("%s, bypasse=%d\n", __func__, bypass);

	exclusive_ui_interlace_bypass = bypass;

	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_GetBypass(void)
{
	dprintk("%s, bypasse=%d\n", __func__, exclusive_ui_interlace_bypass);

	return exclusive_ui_interlace_bypass;
}

int TCC_OUTPUT_EXCLUSIVE_UI_ViqeMtoM(char lcdc_num, unsigned int width, unsigned int height)
{
	unsigned int eui_width, eui_height;
	unsigned int lcd_width, lcd_height;
	PLCDC pLCDC;
  
 	/* Set the output resolution */
 	if (lcdc_num)
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
	else
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);

	lcd_width = (pLCDC->LDS & 0xFFFF);
	lcd_height = ((pLCDC->LDS>>16) & 0xFFFF);

	eui_width = exclusive_ui_info.plane_width;
	eui_height = exclusive_ui_info.plane_height;
	
	dprintk("%s width=%d,  height=%d, eui_width=%d, eui_height=%d\n", __func__, width, height, eui_width,eui_height);

	if(exclusive_ui_info.enable)
	{
	#if 0
		if((exclusive_ui_info.plane_width != TCC_FB_OUT_MAX_WIDTH) || (exclusive_ui_info.plane_height != TCC_FB_OUT_MAX_HEIGHT))
			return 1;
	#else
		if((eui_width >= TCC_FB_OUT_MAX_WIDTH && eui_height >= TCC_FB_OUT_MAX_HEIGHT) || (width >= TCC_FB_OUT_MAX_WIDTH && height >= TCC_FB_OUT_MAX_HEIGHT))
			return 0;
		else
			return 1;
	#endif
	}

	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_Restore_Viqe(struct tcc_lcdc_image_update *ImageInfo, char lcdc_num, unsigned int width, unsigned int height, char index)
{
	dprintk("%s\n", __func__);

	if(exclusive_ui_viqe_count >= TCC_EXCLUSIVE_UI_UPDATE_MAX)
	{
		printk("%s Error: update array was overflowed!!, count=%d\n", __func__, exclusive_ui_viqe_count);
		exclusive_ui_viqe_count = 0;
	}
	else
	{
		exclusive_ui_viqe[exclusive_ui_viqe_count].lcdc = lcdc_num;
		exclusive_ui_viqe[exclusive_ui_viqe_count].wd = width;
		exclusive_ui_viqe[exclusive_ui_viqe_count].ht = height;
		exclusive_ui_viqe[exclusive_ui_viqe_count].index = index;
		memcpy(&exclusive_ui_viqe[exclusive_ui_viqe_count].image, ImageInfo, sizeof(struct tcc_lcdc_image_update));

		exclusive_ui_viqe_count++;
	}
	
	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_Restore_Mixer(struct tcc_lcdc_image_update *ImageInfo, char lcdc_num, unsigned int width, unsigned int height, char index)
{
	dprintk("%s\n", __func__);

	exclusive_ui_mixer.lcdc = lcdc_num;
	exclusive_ui_mixer.wd = width;
	exclusive_ui_mixer.ht = height;
	exclusive_ui_mixer.index = index;
	memcpy(&exclusive_ui_mixer.image, ImageInfo, sizeof(struct tcc_lcdc_image_update));
	
	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_Ctrl(unsigned int enable)
{
	if(enable)
	{
		/* Set enable flag */
		exclusive_ui_info.enable = TRUE;
	}
	else
	{
		/* Clear information of aspect ratio */
		memset((void *)&exclusive_ui_aspect_ratio, 0x00, sizeof(exclusive_ui_ar_params));
		/* Clear enable flag */
		exclusive_ui_info.enable = FALSE;
		/* Clear UI information */
		//memset(&exclusive_ui_info, 0x00, sizeof(exclusive_ui_info));
		/* configure bus priority */
		//if(exclusive_ui_priority == TRUE)
		//	TCC_OUTPUT_EXCLUSIVE_CfgBusPriority(9, FALSE);

		#if 0 //shkim - on_the_fly
		/* Unmap virtual address */
		if(exclusive_ui_info.base_addr_cpu)
		{
 			iounmap(exclusive_ui_info.base_addr_cpu);
			exclusive_ui_info.base_addr_cpu = 0;			
		}
		#endif
 
		#if 0 //shkim - on_the_fly
		if(exclusive_ui_onthefly)
			TCC_OUTPUT_EXCLUSIVE_UI_SetOnthefly(0, 1, FALSE);
		#endif

		exclusive_ui_clear_force = 1;

		//output_3d_ui_flag = 0;
		//output_3d_ui_mode = 0;
 	}
	
	printk("%s, enblae=%d\n", __func__, exclusive_ui_info.enable);
	
	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_Update(exclusive_ui_frame ui_frame)
{
	unsigned int lcdc_width, lcdc_height;
	PLCDC pLCDC;
	PLCDC_CHANNEL pLCDC_CH;
  
	memcpy(&exclusive_ui_img, &ui_frame, sizeof(exclusive_ui_frame));
	
	if(ui_frame.index)
		exclusive_ui_img.pBaseAddr = (void *)exclusive_ui_info.base_addr_dma + TCC_EXCLUSIVE_UI_ADDR_OFFSET;
	else
		exclusive_ui_img.pBaseAddr = (void *)exclusive_ui_info.base_addr_dma;

	if(ui_frame.src_wd > ui_frame.buf_wd)
		exclusive_ui_img.src_wd = ui_frame.buf_wd;

	if(ui_frame.src_ht > ui_frame.buf_ht)
		exclusive_ui_img.src_ht = ui_frame.buf_ht;

	/* Check the position of frame is valid or not */
	if( (ui_frame.src_x >= ui_frame.buf_wd) || (ui_frame.src_y >= ui_frame.buf_ht) ||
		(ui_frame.dst_x >= ui_frame.buf_wd) || (ui_frame.dst_y >= ui_frame.buf_ht) )
	{
		dprintk("%s, Err: Position of frame is not valid!, sx=%d, sy=%d, dx=%d, dy=%d\n", 
				__func__, ui_frame.src_x, ui_frame.src_y, ui_frame.dst_x, ui_frame.dst_y);

		return -1;
	}

	/* Check the clear flag */
	if(exclusive_ui_clear_force)
	{
		dprintk("%s, force exclusive_ui clear!!\n", __func__); 
		return -1;
	}
	
	exclusive_ui_info.update = TRUE;
	exclusive_ui_info.clear = FALSE;

	if(exclusive_ui_lcdc >= 0)
	{
	 	if(exclusive_ui_lcdc)
			pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
		else
			pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
		lcdc_width	= ((pLCDC->LDS>>0 ) & 0xFFFF);
		lcdc_height	= ((pLCDC->LDS>>16) & 0xFFFF);
	
		if(exclusive_ui_info.interlace) 
		{
			/* Interlace BD Contents with 1920x1080 graphic plane */
			if(!TCC_OUTPUT_EXCLUSIVE_UI_ViqeMtoM(exclusive_ui_lcdc, exclusive_ui_info.plane_width, exclusive_ui_info.plane_height))
				TCC_OUTPUT_EXCLUSIVE_UI_Direct(exclusive_ui_lcdc, TRUE);	
			else
				TCC_OUTPUT_EXCLUSIVE_UI_Direct(exclusive_ui_lcdc, FALSE);
		}
		else
		{
			/* 1920x1080 Progressive/Interlace BD Contents */
			if( (exclusive_ui_info.plane_width == TCC_FB_OUT_MAX_WIDTH) && (lcdc_width == TCC_FB_OUT_MAX_WIDTH) &&
				(exclusive_ui_info.plane_height == TCC_FB_OUT_MAX_HEIGHT) && (lcdc_height == TCC_FB_OUT_MAX_HEIGHT) )
				TCC_OUTPUT_EXCLUSIVE_UI_Direct(exclusive_ui_lcdc, TRUE);	
			else
				TCC_OUTPUT_EXCLUSIVE_UI_Direct(exclusive_ui_lcdc, FALSE);	
		}
	}

	dprintk("%s, address=0x%08x, index=%d, \n", __func__, exclusive_ui_img.pBaseAddr, exclusive_ui_img.index);
	dprintk("%s, buf_wd=%d, buf_ht=%d, src_x=%d, src_y=%d, src_wd=%d, src_ht=%d, dst_x=%d, dst_y=%d\n", __func__, 
			ui_frame.buf_wd, ui_frame.buf_ht, ui_frame.src_x, ui_frame.src_y, ui_frame.src_wd, ui_frame.src_ht, ui_frame.dst_x, ui_frame.dst_y);
	dprintk("%s, buf_wd=%d, buf_ht=%d, src_x=%d, src_y=%d, src_wd=%d, src_ht=%d, dst_x=%d, dst_y=%d, addr=0x%08x\n", __func__, 
			exclusive_ui_img.buf_wd, exclusive_ui_img.buf_ht, exclusive_ui_img.src_x, exclusive_ui_img.src_y, 
			exclusive_ui_img.src_wd, exclusive_ui_img.src_ht, exclusive_ui_img.dst_x, exclusive_ui_img.dst_y, exclusive_ui_img.pBaseAddr);
	
	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_Update_Force(void)
{
	struct tcc_lcdc_image_update ImageInfo;

	if(exclusive_ui_info.clear == FALSE)
	{
		memcpy(&ImageInfo, &exclusive_ui_info.update_img, sizeof(struct tcc_lcdc_image_update));
		
		if(pLCDC_OUTPUT[TCC_OUTPUT_HDMI] != NULL)
			TCC_HDMI_DISPLAY_PROCESS(output_lcdc[TCC_OUTPUT_HDMI], (struct tcc_lcdc_image_update *)&ImageInfo, 1);	
		else if(pLCDC_OUTPUT[TCC_OUTPUT_COMPONENT] != NULL)
		{
		#if defined(CONFIG_FB_TCC_COMPONENT)
			tcc_component_process((struct tcc_lcdc_image_update *)&ImageInfo, 1);
		#endif
		}
		else if(pLCDC_OUTPUT[TCC_OUTPUT_COMPOSITE] != NULL)
		{
		#if defined(CONFIG_FB_TCC_COMPOSITE)
			tcc_composite_process((struct tcc_lcdc_image_update *)&ImageInfo, 1);
		#endif
		}
	 
		printk("%s, address=0x%08x\n", __func__, (unsigned int)exclusive_ui_img.pBaseAddr);
	}
	
	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_Update_Mixer(void)
{
	dprintk("%s\n", __func__);
	
	if(exclusive_ui_info.enable)
	{
		if(pLCDC_OUTPUT[TCC_OUTPUT_HDMI] != NULL)
			TCC_HDMI_DISPLAY_IMAGE(exclusive_ui_mixer);
		else if(pLCDC_OUTPUT[TCC_OUTPUT_COMPONENT] != NULL)
		{
		#if defined(CONFIG_FB_TCC_COMPONENT)
			tcc_component_display_image(exclusive_ui_mixer);
		#endif
		}
		else if(pLCDC_OUTPUT[TCC_OUTPUT_COMPOSITE] != NULL)
		{
		#if defined(CONFIG_FB_TCC_COMPOSITE)
			tcc_composite_display_image(exclusive_ui_mixer);
		#endif
		}
		
		/* M2M Scaler without On_the_fly */
		//TCC_OUTPUT_EXCLUSIVE_UI_Scaler(&exclusive_ui_mixer.image, 1, exclusive_ui_mixer.wd, exclusive_ui_mixer.ht, exclusive_ui_mixer.index, 0);
		/* Update Screen */
		//TCC_HDMI_DISPLAY_UPDATE(exclusive_ui_mixer.lcdc, &exclusive_ui_mixer.image);
	}

	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_Clear(void)
{
	dprintk("%s\n", __func__);
	
	//exclusive_ui_info.update = TRUE;

	/* Set the clear flag */
	exclusive_ui_info.clear = TRUE;

	/* Disable graphic plane channel of LCDC */
	if(pLCDC_OUTPUT[TCC_OUTPUT_HDMI] != NULL)
		TCC_OUTPUT_EXCLUSIVE_UI_Direct(output_lcdc[TCC_OUTPUT_HDMI], FALSE);
	else if(pLCDC_OUTPUT[TCC_OUTPUT_COMPONENT] != NULL)
		TCC_OUTPUT_EXCLUSIVE_UI_Direct(output_lcdc[TCC_OUTPUT_COMPONENT], FALSE);
	else if(pLCDC_OUTPUT[TCC_OUTPUT_COMPOSITE] != NULL)
		TCC_OUTPUT_EXCLUSIVE_UI_Direct(output_lcdc[TCC_OUTPUT_COMPOSITE], FALSE);
		
	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_Clear_Force(char clear_force)
{
	PLCDC pLCDC;
	PLCDC_CHANNEL pLCDC_CH;
  
	dprintk("%s, clear_force=%d\n", __func__, clear_force);

 	/* Set the output resolution */
 	if(exclusive_ui_lcdc)
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
	else
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
	
	if(clear_force)
	{
	 	/* Get layer1 of LCDC */
		pLCDC_CH = (volatile PLCDC_CHANNEL)&pLCDC->LI1C;
	}
	else
	{
	 	/* Get layer2 of LCDC */
		pLCDC_CH = (volatile PLCDC_CHANNEL)&pLCDC->LI2C;
	}

	/* Disalbe layer of LCDC */
	if(pLCDC_CH->LIC & HwLIC_IEN)
		pLCDC_CH->LIC = HwLCT_RU | 0x00;
		
	/* Set the flag */
	exclusive_ui_clear_force = clear_force;
	
	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_SetOnTheFly_Scaler(char lcdc_num, char scaler_num, char OnOff)
{
	volatile PDDICONFIG pDDICONFIG = (volatile PDDICONFIG)tcc_p2v(HwDDI_CONFIG_BASE);

	if(OnOff)
	{
		if(exclusive_ui_onthefly)
			return -1;
		
		exclusive_ui_onthefly = TRUE;
		
	 	if(scaler_num)
			M2M_Scaler_SW_Reset(M2M_SCALER1); 
		else
			M2M_Scaler_SW_Reset(M2M_SCALER0); 

		printk("%s, lcdc_num=%d, scaler_num=%d, onthefly=%d\n", __func__, lcdc_num, scaler_num, exclusive_ui_onthefly);
	}

	if(scaler_num)
	{
		if(lcdc_num)
		{
			BITCSET(pDDICONFIG->ON_THE_FLY , Hw0+Hw1, 0);			// VIQE + LCDC0
			BITCSET(pDDICONFIG->ON_THE_FLY, Hw2+Hw3, Hw3);			// SCALER0 Not Used

			if(OnOff)	
				BITCSET(pDDICONFIG->ON_THE_FLY, Hw4+Hw5, Hw4);		// SCALER1 + LCDC1
			else	
				BITCSET(pDDICONFIG->ON_THE_FLY, Hw4+Hw5, Hw4+Hw5);	// SCALER1 Not Used
		}
		else
		{
			BITCSET(pDDICONFIG->ON_THE_FLY, Hw0+Hw1, Hw0);			// VIQE + LCDC1
			BITCSET(pDDICONFIG->ON_THE_FLY, Hw2+Hw3, Hw3);			// SCALER0 Not Used

			if(OnOff)	
				BITCSET(pDDICONFIG->ON_THE_FLY, Hw4+Hw5, 0);		// SCALER1 + LCDC0
			else	
				BITCSET(pDDICONFIG->ON_THE_FLY, Hw4+Hw5, Hw4+Hw5);	// SCALER1 Not Used
		}
	}
	else
	{
		if(lcdc_num)
		{
			BITCSET(pDDICONFIG->ON_THE_FLY, Hw0+Hw1, 0);			// VIQE + LCDC0
			BITCSET(pDDICONFIG->ON_THE_FLY, Hw4+Hw5, Hw4+Hw5);		// SCALER1 Not Used

			if(OnOff)	
				BITCSET(pDDICONFIG->ON_THE_FLY, Hw2+Hw3, Hw2);		// SCALER0 + LCDC1
			else	
				BITCSET(pDDICONFIG->ON_THE_FLY, Hw2+Hw3, Hw3);		// SCALER0 Not Used
		}
		else
		{
			BITCSET(pDDICONFIG->ON_THE_FLY, Hw0+Hw1, Hw0);			// VIQE + LCDC1
			BITCSET(pDDICONFIG->ON_THE_FLY, Hw4+Hw5, Hw4+Hw5);		// SCALER1 Not Used

			if(OnOff)	
				BITCSET(pDDICONFIG->ON_THE_FLY, Hw2+Hw3, 0);		// SCALER0 + LCDC0
			else	
				BITCSET(pDDICONFIG->ON_THE_FLY, Hw2+Hw3, Hw3);		// SCALER0 Not Used
		}
	}

	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_SetOnTheFly_Viqe(char lcdc_num, char scaler_num, char OnOff)
{
	volatile PDDICONFIG pDDICONFIG = (volatile PDDICONFIG)tcc_p2v(HwDDI_CONFIG_BASE);

	if(OnOff)
	{
		if(exclusive_ui_onthefly)
			return -1;
		
		exclusive_ui_onthefly = TRUE;
		
	 	if(scaler_num)
			M2M_Scaler_SW_Reset(M2M_SCALER1); 
		else
			M2M_Scaler_SW_Reset(M2M_SCALER0); 

		dprintk("%s, lcdc_num=%d, scaler_num=%d, onthefly=%d\n", __func__, lcdc_num, scaler_num, exclusive_ui_onthefly);
	}

	if(scaler_num == 1)
	{
		BITCSET(pDDICONFIG->ON_THE_FLY, Hw2+Hw3, Hw3);			// SCALER0 Not Used
		BITCSET(pDDICONFIG->ON_THE_FLY, Hw4+Hw5, Hw4+Hw5);		// SCALER1 Not Used

		if(OnOff)
			BITCSET(pDDICONFIG->ON_THE_FLY, Hw0+Hw1, Hw0+Hw1);	// VIQE + MSCL1
		else
		{
			if(lcdc_num)
				BITCSET(pDDICONFIG->ON_THE_FLY, Hw0+Hw1, Hw0);	// VIQE + LCDC1
			else
				BITCSET(pDDICONFIG->ON_THE_FLY, Hw0+Hw1, 0);	// VIQE + LCDC0
		}
	}
	else
	{
		BITCSET(pDDICONFIG->ON_THE_FLY, Hw2+Hw3, Hw3);			// SCALER0 Not Used
		BITCSET(pDDICONFIG->ON_THE_FLY, Hw4+Hw5, Hw4+Hw5);		// SCALER1 Not Used

		if(OnOff)
			BITCSET(pDDICONFIG->ON_THE_FLY, Hw0+Hw1, Hw1);		// VIQE + MSCL0
		else
		{
			if(lcdc_num)
				BITCSET(pDDICONFIG->ON_THE_FLY, Hw0+Hw1, Hw0);	// VIQE + LCDC1
			else
				BITCSET(pDDICONFIG->ON_THE_FLY, Hw0+Hw1, 0);	// VIQE + LCDC0
		}
	}

	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_Direct(char lcdc_num, char enable)
{
	unsigned int output_width, output_height;
	PLCDC pLCDC;
	PLCDC_CHANNEL pLCDC_CH;
  
 	/* Set the output resolution */
 	if (lcdc_num)
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
	else
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
	output_width	= ((pLCDC->LDS>>0 ) & 0xFFFF);
	output_height	= ((pLCDC->LDS>>16) & 0xFFFF);
	
 	/* Update layer1 of LCDC */
	pLCDC_CH = (volatile PLCDC_CHANNEL)&pLCDC->LI1C;

	if(enable)
	{
		exclusive_ui_info.update = FALSE;
	
		pLCDC_CH->LIO = exclusive_ui_img.buf_wd * 4;
		pLCDC_CH->LISR = 0;
		pLCDC_CH->LIS =((exclusive_ui_img.src_ht << 16) | (exclusive_ui_img.src_wd));
		pLCDC_CH->LIBA0 = (unsigned int)(exclusive_ui_img.pBaseAddr + (exclusive_ui_img.src_y*exclusive_ui_img.buf_wd + exclusive_ui_img.src_x)*4);

		if(ISZERO(pLCDC->LCTRL, HwLCTRL_NI))
		{
			pLCDC_CH->LIP = ((exclusive_ui_img.dst_y/2) << 16) | (exclusive_ui_img.dst_x);
			pLCDC_CH->LIC = HwLIC_INTL | HwLCT_RU | HwLIC_ASEL | HwLIC_AEN | HwLIC_IEN | TCC_LCDC_IMG_FMT_RGB888;
		}
		else
		{
			pLCDC_CH->LIP = (exclusive_ui_img.dst_y << 16) | (exclusive_ui_img.dst_x);
			pLCDC_CH->LIC = HwLCT_RU | HwLIC_ASEL | HwLIC_AEN | HwLIC_IEN | TCC_LCDC_IMG_FMT_RGB888;
		}

		TCC_OUTPUT_FB_WaitVsyncInterrupt(OutputType);

 		dprintk("%s,  lcdc_num=%d, width=%d, height=%d, enable=%d, LIC=0x%08x, addr=0x%08x\n", __func__, lcdc_num, exclusive_ui_img.src_wd, exclusive_ui_img.src_ht, enable, pLCDC_CH->LIC, pLCDC_CH->LIBA0);
	}
	else
	{
		pLCDC_CH->LIC = HwLCT_RU | 0x00;
	}

	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_Mixer(struct tcc_lcdc_image_update *ImageInfo, char index, char lastflag)
{
	int ret;
	unsigned int img_width, img_height;
	G2D_OVERY_FUNC overlay;

	if(exclusive_ui_info.clear == TRUE)
	{
		dprintk("%s, Overlay Mixing is not needed!!\n", __func__);
		return -1;
	}

	img_width = ImageInfo->Image_width;
	img_height= ImageInfo->Image_height;

 	/* Set the overlay-mixer information */
	memset(&overlay, 0x00, sizeof(G2D_OVERY_FUNC));
		
	overlay.src0.add0 = ImageInfo->addr0;
	overlay.src0.add1 = ImageInfo->addr1;
	overlay.src0.add2 = ImageInfo->addr2;
	overlay.src0.frame_pix_sx = img_width;
	overlay.src0.frame_pix_sy = img_height;
	overlay.src0.src_off_sx = 0;
	overlay.src0.src_off_sy = 0;  
	overlay.src0.img_pix_sx = img_width;
	overlay.src0.img_pix_sy = img_height;  
	overlay.src0.win_off_sx = 0; // mandatory '0'
	overlay.src0.win_off_sy = 0; // mandatory '0'
	overlay.src0.op_mode = NOOP;

	if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV422SP)
		overlay.src0.src_form.format = GE_YUV422_sp;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV420SP)
		overlay.src0.src_form.format = GE_YUV420_sp;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV422SQ)
		overlay.src0.src_form.format = GE_YUV422_sq;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV420ITL0)
		overlay.src0.src_form.format = GE_YUV420_in;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV422ITL0)
		overlay.src0.src_form.format = GE_YUV422_in;
	else
		printk("%s, Err: Overlay Mixer Format(%d) is not supported!!\n", __func__, ImageInfo->fmt);
	overlay.src0.src_form.data_swap = G2D_ARGB;

	overlay.src1.add0 = (unsigned int)exclusive_ui_img.pBaseAddr;
	overlay.src1.frame_pix_sx = exclusive_ui_img.buf_wd;
	overlay.src1.frame_pix_sy = exclusive_ui_img.buf_ht;
	overlay.src1.src_off_sx = exclusive_ui_img.src_x;
	overlay.src1.src_off_sy = exclusive_ui_img.src_y;
	overlay.src1.img_pix_sx = exclusive_ui_img.src_wd;
	overlay.src1.img_pix_sy = exclusive_ui_img.src_ht;
	overlay.src1.win_off_sx = exclusive_ui_img.dst_x;
	overlay.src1.win_off_sy = exclusive_ui_img.dst_y;
	overlay.src1.op_mode = NOOP;
	overlay.src1.src_form.format = GE_ARGB8888;
	overlay.src1.src_form.data_swap = G2D_ARGB;

	overlay.op_ctrl_scel0 = CHROMA_OP0_NOOP; // Chroma-key Source Select
	overlay.chroma_RY = 0;  //chroma - key value
	overlay.chroma_GU = 0;  //chroma - key value
	overlay.chroma_BV = 0;  //chroma - key value

	overlay.alpha_en = G2d_ALAPH_RGB; // alpha-blending enable
	overlay.alpha_value = 0; // alpha-blending value

	overlay.dest.frame_pix_sx = img_width;
	overlay.dest.frame_pix_sy = img_height;
	overlay.dest.dest_off_sx = 0;
	overlay.dest.dest_off_sy = 0;
	overlay.dest.op_mode = NOOP;
	overlay.dest.dest_form.format = GE_YUV422_sq;
	overlay.dest.dest_form.data_swap = 0;
	overlay.responsetype = G2D_INTERRUPT; //G2D_ResponseType

#if defined(CONFIG_TCC_VIDEO_DISPLAY_BY_VSYNC_INT)
	if(lastflag)
	{
		if(ImageInfo->dst_addr0)
			overlay.dest.add0 = (unsigned int)ImageInfo->dst_addr0;
		else
		{
			if(index)
				overlay.dest.add0 = (unsigned int)(exclusive_ui_g2d_pbuf + 0x000000);
			else
				overlay.dest.add0 = (unsigned int)(exclusive_ui_g2d_pbuf + 0x400000);
		}
	}
	else
	{
		if(index)
			overlay.dest.add0 = (unsigned int)(exclusive_ui_g2d_pbuf + 0x000000);
		else
			overlay.dest.add0 = (unsigned int)(exclusive_ui_g2d_pbuf + 0x400000);
	}
#else
	if(index)
		overlay.dest.add0 = (unsigned int)(exclusive_ui_g2d_pbuf + 0x000000);
	else
		overlay.dest.add0 = (unsigned int)(exclusive_ui_g2d_pbuf + 0x400000);
#endif
	overlay.dest.add1 = (unsigned int)GET_ADDR_YUV42X_spU(overlay.dest.add0, overlay.dest.frame_pix_sx, overlay.dest.frame_pix_sy);
	overlay.dest.add2 = (unsigned int)GET_ADDR_YUV420_spV(overlay.dest.add1, overlay.dest.frame_pix_sx, overlay.dest.frame_pix_sy);

	dprintk("%s, Overlay Mixer : dst_format=%d, dst_w=%d, dst_h=%d, dst_addr=0x%08x\n", __func__, 
			overlay.dest.dest_form.format, overlay.dest.frame_pix_sx, overlay.dest.frame_pix_sy, overlay.dest.add0);
			
 	/* Execute the overlay-mixer */
	ret = grp_overlay_ctrl(&overlay);

 	/* Update new image information */
	if(ret >= 0)
	{
		ImageInfo->fmt = TCC_LCDC_IMG_FMT_YUV422SQ;
		ImageInfo->addr0 = overlay.dest.add0;
		ImageInfo->addr1 = overlay.dest.add1;
		ImageInfo->addr2 = overlay.dest.add2;
	}
	
	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_Scaler(struct tcc_lcdc_image_update *ImageInfo, char scaler_num, unsigned int width, unsigned int height, char index, char lastflag)
{
	int ret;
	unsigned int img_width, img_height, img_offset_x, img_offset_y;
	unsigned int output_width, output_height;
	unsigned int scale_limit_wd, scale_limit_ht, scale_limit_total;
	SCALER_TYPE fbscaler;

	img_width	= ImageInfo->Image_width;
	img_height	= ImageInfo->Image_height;

	img_offset_x = 0;
	img_offset_y = 0;

 	/* Set the output resolution */
	if(exclusive_ui_aspect_ratio.enable && lastflag)
	{
		if((exclusive_ui_aspect_ratio.ar_wd <= width) && (exclusive_ui_aspect_ratio.ar_ht <= height))
		{
			output_width	= exclusive_ui_aspect_ratio.ar_wd;
			output_height	= exclusive_ui_aspect_ratio.ar_ht;
			img_offset_x	= (width - output_width) >> 1;
			img_offset_y	= (height - output_height) >> 1;
		}
		else
		{
			output_width	= width;
			output_height	= height;
		}
	}
	else
	{
		output_width	= width;
		output_height	= height;
	}

 	/* Check the scale limitation */
	scale_limit_wd 		= (output_width*10) / img_width;
	scale_limit_ht		= (output_height*10) / img_height;
	scale_limit_total 	= scale_limit_wd * scale_limit_ht;
	
	if(scale_limit_wd >= 38 || scale_limit_ht >= 38)
	{
		if(scale_limit_wd >= 38)
		{
			output_width	= (img_width * 38) / 10;
			output_width	= ((output_width+7)>>3)<<3;
			if(output_width > width)
				output_width = width;
		}

		if(scale_limit_ht >= 38)
		{
			output_height 	= (img_height * 38) / 10;
			output_height 	= ((output_height+1)>>2)<<2;
			if(output_height > height)
				output_height = height;
		}

		img_offset_x	= (width - output_width) >> 1;
		img_offset_y	= (height - output_height) >> 1;

		dprintk("%s, scaler limitation, width=%d, height=%d\n", __func__, output_width, output_height);
	}

 	/* Check the resolution */
	if((img_width == output_width) && (img_height == output_height) && !lastflag)
	{
		dprintk("%s, Don't need scaler!!\n", __func__);
		return -1;
	}

 	/* Set the scaler information */
	memset(&fbscaler, 0x00, sizeof(SCALER_TYPE));

	fbscaler.responsetype = SCALER_INTERRUPT;

	if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV422SP)
		fbscaler.src_fmt = SCALER_YUV422_sp;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV420SP)
		fbscaler.src_fmt = SCALER_YUV420_sp;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV422SQ)
		fbscaler.src_fmt = SCALER_YUV422_sq0;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV420ITL0)
		fbscaler.src_fmt = SCALER_YUV420_inter;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV422ITL0)
		fbscaler.src_fmt = SCALER_YUV422_inter;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_RGB565)
		fbscaler.src_fmt = SCALER_RGB565;
	else
		printk("%s, Err: M2M Scaler Format(%d) is not supported!!\n", __func__, ImageInfo->fmt);
		
	fbscaler.src_Yaddr = (char *)ImageInfo->addr0;
	fbscaler.src_Uaddr = (char *)ImageInfo->addr1;
	fbscaler.src_Vaddr = (char *)ImageInfo->addr2;
	fbscaler.src_ImgWidth = img_width;
	fbscaler.src_ImgHeight = img_height;
	fbscaler.src_winLeft = 0;
	fbscaler.src_winTop = 0;
	fbscaler.src_winRight = img_width;
	fbscaler.src_winBottom = img_height;

	fbscaler.dest_fmt = SCALER_YUV422_sq0;		// destination image format
	fbscaler.dest_ImgWidth = output_width;		// destination image width
	fbscaler.dest_ImgHeight = output_height; 	// destination image height
	fbscaler.dest_winLeft = 0;
	fbscaler.dest_winTop = 0;
	fbscaler.dest_winRight = output_width;
	fbscaler.dest_winBottom = output_height;
	fbscaler.viqe_onthefly = 0;
	
#if defined(CONFIG_TCC_VIDEO_DISPLAY_BY_VSYNC_INT)
	if(lastflag)
	{
		if(ImageInfo->dst_addr0)
			fbscaler.dest_Yaddr = (char *)ImageInfo->dst_addr0;
		else
		{
			if(index)
				fbscaler.dest_Yaddr = (char *)(exclusive_ui_scale_pbuf + 0x000000);
			else
				fbscaler.dest_Yaddr = (char *)(exclusive_ui_scale_pbuf + 0x400000);
		}
	}
	else
	{
		if(index)
			fbscaler.dest_Yaddr = (char *)(exclusive_ui_scale_pbuf + 0x000000);
		else
			fbscaler.dest_Yaddr = (char *)(exclusive_ui_scale_pbuf + 0x400000);
	}
#else
	if(index)
		fbscaler.dest_Yaddr = (char *)(exclusive_ui_scale_pbuf + 0x000000);
	else
		fbscaler.dest_Yaddr = (char *)(exclusive_ui_scale_pbuf + 0x400000);
#endif
	fbscaler.dest_Uaddr = (char*)GET_ADDR_YUV42X_spU(fbscaler.dest_Yaddr, fbscaler.dest_ImgWidth, fbscaler.dest_ImgHeight);
	fbscaler.dest_Vaddr = (char*)GET_ADDR_YUV420_spV(fbscaler.dest_Uaddr, fbscaler.dest_ImgWidth, fbscaler.dest_ImgHeight);

	dprintk("%s, M2M Scaler : src_format=%d, src_w=%d, src_h=%d, src_addr=0x%08x, dst_format=%d, dst_w=%d, dst_h=%d, dst_addr=0x%08x\n", __func__, 
			fbscaler.src_fmt, fbscaler.src_ImgWidth, fbscaler.src_ImgHeight, fbscaler.src_Yaddr,
			fbscaler.dest_fmt, fbscaler.dest_ImgWidth, fbscaler.dest_ImgHeight, fbscaler.dest_Yaddr);
		
	ret = DEV_M2M_Wait_signal_disable(scaler_num);
	if((ret == 0) || (ret >= M2M_WAIT_MAX))		{
		printk("ERROR : %s Scaler%d not ready to run %d \n",__func__, scaler_num, ret);
		return 0;
	}

 	/* Execute the scaler */
	if(scaler_num)
		M2M_Scaler1_Ctrl_Detail(&fbscaler);
	else
		M2M_Scaler_Ctrl_Detail(&fbscaler);
		
	
 	/* Update new image information */
	ImageInfo->Frame_width = fbscaler.dest_ImgWidth;
	ImageInfo->Frame_height = fbscaler.dest_ImgHeight;
	ImageInfo->Image_width = fbscaler.dest_ImgWidth;
	ImageInfo->Image_height = fbscaler.dest_ImgHeight;
	ImageInfo->offset_x = img_offset_x;
	ImageInfo->offset_y = img_offset_y;
	ImageInfo->fmt = TCC_LCDC_IMG_FMT_YUV422SQ;
	ImageInfo->addr0 = (unsigned int)fbscaler.dest_Yaddr;
	ImageInfo->addr1 = (unsigned int)fbscaler.dest_Uaddr;
	ImageInfo->addr2 = (unsigned int)fbscaler.dest_Vaddr;

	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_Scaler_Viqe(struct tcc_lcdc_image_update *ImageInfo, unsigned int width, unsigned int height, char index, char lcdc_num)
{
	int ret;
	unsigned int img_width, img_height, img_offset_x, img_offset_y;
	unsigned int output_width, output_height;
	unsigned int scaler_num;
	unsigned int scale_limit_wd, scale_limit_ht, scale_limit_total;
	SCALER_TYPE fbscaler;

	/* M2M SCALER0 */
	scaler_num = 0;

	img_width	= ImageInfo->Image_width;
	img_height	= ImageInfo->Image_height;

	img_offset_x = 0;
	img_offset_y = 0;

 	/* Set the output resolution */
	output_width	= width;
	output_height	= height;

 	/* Check the scale limitation */
	scale_limit_wd 		= (output_width*10) / img_width;
	scale_limit_ht		= (output_height*10) / img_height;
	scale_limit_total 	= scale_limit_wd * scale_limit_ht;
	
	if(scale_limit_wd >= 38 || scale_limit_ht >= 38)
	{
		if(scale_limit_wd >= 38)
		{
			output_width	= (img_width * 38) / 10;
			output_width	= ((output_width+7)>>3)<<3;
			if(output_width > width)
				output_width = width;
		}

		if(scale_limit_ht >= 38)
		{
			output_height 	= (img_height * 38) / 10;
			output_height 	= ((output_height+1)>>2)<<2;
			if(output_height > height)
				output_height = height;
		}
		
		img_offset_x	= (width - output_width) >> 1;
		img_offset_y	= (height - output_height) >> 1;

		dprintk("%s, scaler limitation, width=%d, height=%d\n", __func__, output_width, output_height);
	}
			
	/* Set ON_THE_FLY path */
	TCC_OUTPUT_EXCLUSIVE_UI_SetOnTheFly_Viqe(lcdc_num, scaler_num, TRUE);

 	/* Set the scaler information */
	memset(&fbscaler, 0x00, sizeof(SCALER_TYPE));

	fbscaler.responsetype = SCALER_INTERRUPT;

	/* Set the viqe output format */
	ImageInfo->fmt = TCC_LCDC_IMG_FMT_YUV422SQ;

	if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV422SP)
		fbscaler.src_fmt = SCALER_YUV422_sp;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV420SP)
		fbscaler.src_fmt = SCALER_YUV420_sp;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV422SQ)
		fbscaler.src_fmt = SCALER_YUV422_sq0;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV420ITL0)
		fbscaler.src_fmt = SCALER_YUV420_inter;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV422ITL0)
		fbscaler.src_fmt = SCALER_YUV422_inter;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_RGB565)
		fbscaler.src_fmt = SCALER_RGB565;
	else
		printk("%s, Err: M2M Scaler Format(%d) is not supported!!\n", __func__, ImageInfo->fmt);
		
	fbscaler.src_Yaddr = (char *)ImageInfo->addr0;
	fbscaler.src_Uaddr = (char *)ImageInfo->addr1;
	fbscaler.src_Vaddr = (char *)ImageInfo->addr2;
	fbscaler.src_ImgWidth = img_width;
	fbscaler.src_ImgHeight = img_height;
	fbscaler.src_winLeft = 0;
	fbscaler.src_winTop = 0;
	fbscaler.src_winRight = img_width;
	fbscaler.src_winBottom = img_height;

	fbscaler.dest_fmt = SCALER_YUV422_sq0;		// destination image format
	fbscaler.dest_ImgWidth = output_width;		// destination image width
	fbscaler.dest_ImgHeight = output_height; 	// destination image height
	fbscaler.dest_winLeft = 0;
	fbscaler.dest_winTop = 0;
	fbscaler.dest_winRight = output_width;
	fbscaler.dest_winBottom = output_height;
	
 	/* Set the input path for ON_THE_FLY */
	fbscaler.viqe_onthefly = 0x01;

	if(index)
		fbscaler.dest_Yaddr = (char *)(exclusive_ui_dst_pbuf + 0x000000);
	else
		fbscaler.dest_Yaddr = (char *)(exclusive_ui_dst_pbuf + 0x400000);
	fbscaler.dest_Uaddr = (char*)GET_ADDR_YUV42X_spU(fbscaler.dest_Yaddr, fbscaler.dest_ImgWidth, fbscaler.dest_ImgHeight);
	fbscaler.dest_Vaddr = (char*)GET_ADDR_YUV420_spV(fbscaler.dest_Uaddr, fbscaler.dest_ImgWidth, fbscaler.dest_ImgHeight);

	dprintk("%s, M2M Scaler : src_format=%d, src_w=%d, src_h=%d, dst_format=%d, dst_w=%d, dst_h=%d, dst_addr=0x%08x\n", __func__, 
			fbscaler.src_fmt, fbscaler.src_ImgWidth, fbscaler.src_ImgHeight, 
			fbscaler.dest_fmt, fbscaler.dest_ImgWidth, fbscaler.dest_ImgHeight, fbscaler.dest_Yaddr);
		
	ret = DEV_M2M_Wait_signal_disable(scaler_num);
	if((ret == 0) || (ret >= M2M_WAIT_MAX))		{
		printk("ERROR : %s Scaler%d not ready to run %d \n",__func__, scaler_num, ret);
		return 0;
	}

 	/* Execute the scaler */
	if(scaler_num)
		M2M_Scaler1_Ctrl_Detail(&fbscaler);
	else
		M2M_Scaler_Ctrl_Detail(&fbscaler);
	
	/* Wait until the scaling is completed */
	if(exclusive_ui_info.clear == TRUE)
	{
		ret = DEV_M2M_Wait_signal_disable(scaler_num);
		if((ret == 0) || (ret >= M2M_WAIT_MAX))		{
			printk("ERROR : %s Scaler%d not ready to run %d \n",__func__, scaler_num, ret);
			return 0;
		}
	}

 	/* Update new image information */
	ImageInfo->Frame_width = fbscaler.dest_ImgWidth;
	ImageInfo->Frame_height = fbscaler.dest_ImgHeight;
	ImageInfo->Image_width = fbscaler.dest_ImgWidth;
	ImageInfo->Image_height = fbscaler.dest_ImgHeight;
	ImageInfo->offset_x = img_offset_x;
	ImageInfo->offset_y = img_offset_y;
	ImageInfo->fmt = TCC_LCDC_IMG_FMT_YUV422SQ;
	ImageInfo->addr0 = (unsigned int)fbscaler.dest_Yaddr;
	ImageInfo->addr1 = (unsigned int)fbscaler.dest_Uaddr;
	ImageInfo->addr2 = (unsigned int)fbscaler.dest_Vaddr;

	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_Scaler_OnTheFly(struct tcc_lcdc_image_update *ImageInfo, char lcdc_num)
{
	unsigned int lcd_width, lcd_height;
	unsigned int img_width, img_height, img_offset_x, img_offset_y;
	unsigned int output_width, output_height;
	unsigned int scaler_num;
	unsigned int scale_limit_wd, scale_limit_ht, scale_limit_total;
	SCALER_TYPE fbscaler;
	PLCDC pLCDC;
	PLCDC_CHANNEL pLCDC_channel;

	/* M2M SCALER0 */
	scaler_num = 0;

	img_width	= ImageInfo->Image_width;
	img_height	= ImageInfo->Image_height;

	img_offset_x = 0;
	img_offset_y = 0;

 	if (lcdc_num)
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
	else
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
	lcd_width	= ((pLCDC->LDS>>0 ) & 0xFFFF);
	lcd_height	= ((pLCDC->LDS>>16) & 0xFFFF);

	pLCDC_channel = (volatile PLCDC_CHANNEL)&pLCDC->LI0C;
			
 	/* Set the output resolution */
	if(exclusive_ui_aspect_ratio.enable)
	{
		if((exclusive_ui_aspect_ratio.ar_wd <= lcd_width) && (exclusive_ui_aspect_ratio.ar_ht <= lcd_height))
		{
			output_width	= exclusive_ui_aspect_ratio.ar_wd;
			output_height	= exclusive_ui_aspect_ratio.ar_ht;
			img_offset_x	= (lcd_width - output_width) >> 1;
			img_offset_y	= (lcd_height - output_height) >> 1;
		}
		else
		{
			output_width	= lcd_width;
			output_height	= lcd_height;
		}
	}
	else
	{
		output_width	= lcd_width;
		output_height	= lcd_height;
	}

	scale_limit_wd 		= (output_width*10) / img_width;
	scale_limit_ht		= (output_height*10) / img_height;
	scale_limit_total 	= scale_limit_wd * scale_limit_ht;
	
	if((scale_limit_wd >= 38 || scale_limit_ht >= 38))
	{
		if(scale_limit_wd >= 38)
		{
			output_width	= (img_width * 38) / 10;
			output_width	= ((output_width+7)>>3)<<3;
			if(output_width > lcd_width)
				output_width = lcd_width;
		}

		if(scale_limit_ht >= 38)
		{
			output_height 	= (img_height * 38) / 10;
			output_height 	= ((output_height+1)>>2)<<2;
			if(output_height > lcd_height)
				output_height = lcd_height;
		}
		
		img_offset_x	= (lcd_width - output_width) >> 1;
		img_offset_y	= (lcd_height - output_height) >> 1;

		dprintk("%s, scaler limitation, width=%d, height=%d\n", __func__, output_width, output_height);
	}

	/* Check the output format */
	if(exclusive_ui_info.interlace)
	{
		if(ISZERO(pLCDC->LCTRL, HwLCTRL_NI))
			output_height = output_height/2;
	}
		
 	/* Check the resolution */
	if((img_width != output_width) || (img_height != output_height))
	{
		dprintk("%s, need scaler!!\n", __func__);
		
		TCC_OUTPUT_EXCLUSIVE_UI_SetOnTheFly_Scaler(lcdc_num, scaler_num, TRUE);
	}
	else
	{
		dprintk("%s, Don't need scaler!!\n", __func__);

		TCC_OUTPUT_EXCLUSIVE_UI_SetOnTheFly_Scaler(lcdc_num, scaler_num, FALSE);
		return -1;
	}

 	/* Set the scaler information */
	memset(&fbscaler, 0x00, sizeof(SCALER_TYPE));

	fbscaler.responsetype = SCALER_INTERRUPT;

	if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV422SP)
		fbscaler.src_fmt = SCALER_YUV422_sp;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV420SP)
		fbscaler.src_fmt = SCALER_YUV420_sp;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV422SQ)
		fbscaler.src_fmt = SCALER_YUV422_sq0;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV420ITL0)
		fbscaler.src_fmt = SCALER_YUV420_inter;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV422ITL0)
		fbscaler.src_fmt = SCALER_YUV422_inter;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_RGB565)
		fbscaler.src_fmt = SCALER_RGB565;
	else
		printk("%s, Err: M2M Scaler Format(%d) is not supported!!\n", __func__, ImageInfo->fmt);
		
	fbscaler.src_Yaddr = (char *)ImageInfo->addr0;
	fbscaler.src_Uaddr = (char *)ImageInfo->addr1;
	fbscaler.src_Vaddr = (char *)ImageInfo->addr2;
	fbscaler.src_ImgWidth = img_width;
	fbscaler.src_ImgHeight = img_height;
	fbscaler.src_winLeft = 0;
	fbscaler.src_winTop = 0;
	fbscaler.src_winRight = img_width;
	fbscaler.src_winBottom = img_height;

	fbscaler.dest_fmt = SCALER_YUV422_sq0;		// destination image format
	fbscaler.dest_ImgWidth = output_width;		// destination image width
	fbscaler.dest_ImgHeight = output_height; 	// destination image height
	fbscaler.dest_winLeft = 0;
	fbscaler.dest_winTop = 0;
	fbscaler.dest_winRight = output_width;
	fbscaler.dest_winBottom = output_height;
	
 	/* Set the output path for ON_THE_FLY */
	fbscaler.viqe_onthefly = 0x02;

 	/* Execute the scaler */
	if(scaler_num)
		M2M_Scaler1_Ctrl_Detail(&fbscaler);
	else
		M2M_Scaler_Ctrl_Detail(&fbscaler);
	
	dprintk("%s, src_format=%d, src_w=%d, src_h=%d, src_addr=0x%08x, dst_format=%d, dst_w=%d, dst_h=%d, offset_x=%d, offset_y=%d\n", 
			__func__, fbscaler.src_fmt, fbscaler.src_ImgWidth, fbscaler.src_ImgHeight, fbscaler.src_Yaddr,
			fbscaler.dest_fmt, fbscaler.dest_ImgWidth, fbscaler.dest_ImgHeight, img_offset_x, img_offset_y);
		
 	/* Update new image information */
	ImageInfo->Frame_width = fbscaler.dest_ImgWidth;
	ImageInfo->Frame_height = fbscaler.dest_ImgHeight;
	ImageInfo->Image_width = fbscaler.dest_ImgWidth;
	ImageInfo->Image_height = fbscaler.dest_ImgHeight;
	ImageInfo->offset_x = img_offset_x;
	ImageInfo->offset_y = img_offset_y;
	ImageInfo->fmt = TCC_LCDC_IMG_FMT_YUV422SQ;
	ImageInfo->addr0 = (unsigned int)fbscaler.dest_Yaddr;
	ImageInfo->addr1 = (unsigned int)fbscaler.dest_Uaddr;
	ImageInfo->addr2 = (unsigned int)fbscaler.dest_Vaddr;

	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_Process(struct tcc_lcdc_image_update *ImageInfo, char lcdc_num)
{
	exclusive_ui_index = !exclusive_ui_index;

	return 0;
}

int TCC_OUTPUT_EXCLUSIVE_UI_Process_Deinterlace(struct tcc_lcdc_image_update *ImageInfo, char force_update)
{
	dprintk("%s\n", __func__);

	if(pLCDC_OUTPUT[TCC_OUTPUT_HDMI] != NULL)
		TCC_HDMI_DISPLAY_PROCESS(output_lcdc[TCC_OUTPUT_HDMI], ImageInfo, force_update);	
	else if(pLCDC_OUTPUT[TCC_OUTPUT_COMPONENT] != NULL)
	{
	#if defined(CONFIG_FB_TCC_COMPONENT)
		tcc_component_process(ImageInfo, force_update);
	#endif
	}
	else if(pLCDC_OUTPUT[TCC_OUTPUT_COMPOSITE] != NULL)
	{
	#if defined(CONFIG_FB_TCC_COMPOSITE)
		tcc_composite_process(ImageInfo, force_update);
	#endif
	}

	return 0;
}
#endif //CONFIG_TCC_EXCLUSIVE_UI_LAYER

#if defined(CONFIG_ARCH_TCC88XX)
int TCC_OUTPUT_FB_MouseShow(unsigned int enable, unsigned int type)
{
	if( enable )
	{
		//If image channel 3 cannot use LUT.EN3 bit should be '0'
		pLCDC_OUTPUT[type]->LUTIDX &= ~Hw7;		
	}
	else
	{
		pLCDC_OUTPUT[type]->LI3P = 0;
		pLCDC_OUTPUT[type]->LI3S = 0;
		pLCDC_OUTPUT[type]->LI3O = 0;
		
		//LCD CH 3 disable
		pLCDC_OUTPUT[type]->LI3C = 0;
		pLCDC_OUTPUT[type]->LI3C |= HwLCT_RU;
		msleep(30);
	}

	return 1;
}


int TCC_OUTPUT_FB_MouseMove(unsigned int width, unsigned int height, tcc_mouse *mouse, unsigned int type)
{
	unsigned int regl, lcd_width, lcd_height, mouse_x, mouse_y;
	
	regl = pLCDC_OUTPUT[type]->LDS; // get LCD size 
	lcd_width = (regl & 0xFFFF);
	lcd_height = ((regl>>16) & 0xFFFF);

	mouse_x = (unsigned int)(lcd_width * mouse->x / width);
	mouse_y = (unsigned int)(lcd_height *mouse->y / height);

	if( mouse_x > lcd_width - mouse_cursor_width )
		pLCDC_OUTPUT[type]->LI3S = (mouse_cursor_height << 16) | lcd_width - mouse_x;
	else
		pLCDC_OUTPUT[type]->LI3S = (mouse_cursor_height << 16) | mouse_cursor_width;

	if( MouseBufIdx )
		pLCDC_OUTPUT[type]->LI3BA0 = pMouseBuffer1;
	else
		pLCDC_OUTPUT[type]->LI3BA0 = pMouseBuffer2;

	pLCDC_OUTPUT[type]->LI3O = mouse_cursor_width * 4;
	
	if(ISZERO(pLCDC_OUTPUT[type]->LCTRL, HwLCTRL_NI))
	{
		pLCDC_OUTPUT[type]->LI3P = ((mouse_y/2) << 16) | mouse_x;
		pLCDC_OUTPUT[type]->LI3C = HwLIC_INTL | HwLCT_RU | HwLIC_ASEL | HwLIC_AEN | HwLIC_IEN | TCC_LCDC_IMG_FMT_RGB888;
	}
	else
	{
		pLCDC_OUTPUT[type]->LI3P = (mouse_y << 16) | mouse_x;
		pLCDC_OUTPUT[type]->LI3C = HwLCT_RU | HwLIC_ASEL | HwLIC_AEN | HwLIC_IEN | TCC_LCDC_IMG_FMT_RGB888;
	}

	return 1;
}

int TCC_OUTPUT_FB_MouseSetIcon(tcc_mouse_icon *mouse_icon)
{
	char *dst;
	char *src = mouse_icon->buf;
	int i, j , k;

	if( !MouseBufIdx )
		dst = (char *)Gmap_cpu;
	else
		dst = (char *)(Gmap_cpu + MOUSE_CURSOR_BUFF_SIZE/2);

	MouseBufIdx = !MouseBufIdx;

	mouse_cursor_width = ((mouse_icon->width + 3) >> 2) << 2;
	mouse_cursor_height = mouse_icon->height;

	for(i=0; i<mouse_icon->height; i++)
	{
		for(j=0; j<mouse_icon->width; j++)
		{
			*(dst+3) = *(src+3);
			*(dst+2) = *(src+0);
			*(dst+1) = *(src+1);
			*(dst+0) = *(src+2);
			dst+=4;
			src+=4;
		}
		for(k=0; k<(mouse_cursor_width-mouse_icon->width); k++)
		{
			*(dst+3) = 0;
			*(dst+2) = 0;
			*(dst+1) = 0;
			*(dst+0) = 0;
			dst+=4;
		}
	}
	return 1;
}
#endif

