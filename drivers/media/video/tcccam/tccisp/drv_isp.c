#define DRV_ISP_INTERNAL

typedef struct DRVISPProfile *HProfile; 
typedef struct DRVISP *HIsp; 
typedef struct DRVISPEvent *HEvent; 

#include "drv_isp.h"
//#include "tcIspProfile.h"
//#include "Io_address.h"
//#include "tcIspTimer.h"
#include "stdinc.h"
#include "reg_access.h"
#include "mrv_priv.h"

//#define HwMRVISP_BASE_PTR(X)    (volatile unsigned long *)(0xB0210000+(X))
//#define IO_OFFSET		0x00000000	/* Virtual IO = 0xB0000000 */
//#define HwMRVISP_BASE_PTR(X)    (volatile unsigned long *)(0xF0910000-IO_OFFSET+(X))
#define HwMRVISP_BASE_PTR(X) 	(volatile unsigned long *) ( (unsigned) (MEM_MRV_REG_BASE) + (X))

#define TCISP_PROFILE_MAX 		1000	

#define DRVISPPRS_DEVID_TYPE_ISP		0x0000
#define DRVISPPRS_DEVID_TYPE_I2C		0x4000
#define DRVISPPRS_DEVID_TYPE_CMD		0x8000
#define DRVISPPRS_DEVID_TYPE_ADDR		0xc000
#define DRVISPPRS_DEVID_TYPE			0xc000
#define DRVISPPRS_DEVID_DATA_8BIT		0x0000
#define DRVISPPRS_DEVID_DATA_16BIT		0x0100
#define DRVISPPRS_DEVID_DATA_32BIT		0x0200
#define DRVISPPRS_DEVID_DATA			0x0300
#define DRVISPPRS_DEVID_ADDR_8BIT		0x0000
#define DRVISPPRS_DEVID_ADDR_16BIT		0x0400
#define DRVISPPRS_DEVID_ADDR_32BIT		0x0800
#define DRVISPPRS_DEVID_ADDR			0x0c00
#define DRVISPPRS_DEVID_ID				0x00ff	// slave address

#define DRVISPFLAG_PROFILEUPDATE_ENDOFFRAME  0x1
#define DRVISPFLAG_PROFILEUPDATE_MODECHANGE  0x2

#define	DRVISPERRFLAG_MI_SP_CR_FIFO_FULL	0x00004000
#define	DRVISPERRFLAG_MI_SP_CB_FIFO_FULL	0x00002000
#define	DRVISPERRFLAG_MI_SP_Y_FIFO_FULL		0x00001000
#define	DRVISPERRFLAG_MI_MP_CR_FIFO_FULL	0x00000400
#define	DRVISPERRFLAG_MI_MP_CB_FIFO_FULL	0x00000200
#define	DRVISPERRFLAG_MI_MP_Y_FIFO_FULL		0x00000100
#define DRVISPERRFLAG_OUTFORM_SIZE_ERR		0x00000030
#define DRVISPERRFLAG_IS_SIZE_ERR			0x00000020
#define DRVISPERRFLAG_INFORM_SIZE_ERR		0x00000010
#define DRVISPERRFLAG_DATA_LOSS				0x00000002 
#define DRVISPERRFLAG_PIC_SIZE_ERR			0x00000001

#define DRVISPFLAG_IN_PROGRESS				0x80000000
#define DRVISPFLAG_WAIT_ONE_FRAME			0x40000000
#define DRVISPFLAG_START_CMD				0x08000000
#define DRVISPFLAG_FIRSTFRAME_END			0x04000000
#define DRVISPFLAG_STOP_CMD					0x02000000
#define DRVISPFLAG_ISP_OFF					0x01000000
#define DRVISPFLAG_MP_LASTFRAME_END			0x00800000
#define DRVISPFLAG_SP_LASTFRAME_END			0x00400000
#define DRVISPFLAG_UPDATE_REQ				0x00200000
#define DRVISPFLAG_ERROR_OCCURED			0x00100000

#define DRVISPFLAG_3A_ENABLE				0x00010000
#define DRVISPFLAG_ADFUNC_ENABLE			0x00020000

#define DRVISPFLAG_PARALLELYUV_MODE			0x00001000

#define DRVISPCTRL_SENSOR	0x08000000
#define DRVISPCTRL_READ		0x04000000
#define DRVISPCTRL_MAIN		0x02000000
#define DRVISPCTRL_SELF		0x01000000

#define DRVISPCTRL_BLS		0x00800000
#define DRVISPCTRL_LSC		0x00400000
#define DRVISPCTRL_AFM		0x00200000
#define DRVISPCTRL_EXP		0x00100000
#define DRVISPCTRL_DPF		0x00080000
#define DRVISPCTRL_DPCC		0x00040000
#define DRVISPCTRL_WDR		0x00020000
#define DRVISPCTRL_HIST 	0x00010000
#define DRVISPCTRL_FILT 	0x00008000
#define DRVISPCTRL_CAC  	0x00004000
#define DRVISPCTRL_IS   	0x00002000

#define DRVISPCTRL_MIPI 	0x00000800
#define DRVISPCTRL_SIMP 	0x00000200
#define DRVISPCTRL_IE   	0x00000100
#define DRVISPCTRL_ALL  	0x00000080
#define DRVISPCTRL_MI   	0x00000040
#define DRVISPCTRL_JPEG		0x00000020
#define DRVISPCTRL_SRSZ		0x00000010
#define DRVISPCTRL_MRSZ		0x00000008
#define DRVISPCTRL_CP		0x00000002
#define DRVISPCTRL_ISP		0x00000001

#define DRVISPPATH_SENSOR	0x00000080
#define DRVISPPATH_MIPI		0x00000040
#define DRVISPPATH_ISPIN	0x00000020
#define DRVISPPATH_ISPOUT	0x00000010
#define DRVISPPATH_READ		0x00000008
#define DRVISPPATH_MAIN		0x00000004
#define DRVISPPATH_SELF		0x00000002


typedef struct DRVISPEvent {
	unsigned 	event1;
	unsigned 	event2;
	unsigned 	event3;
	unsigned 	event4;
	unsigned 	eventmask1;
	unsigned 	eventmask2;
	unsigned 	eventmask3;
	unsigned 	eventmask4;
	
	HIsp            hisp;
	HEvent		next;
} DRVISPEvent;

	
typedef struct DRVISP{
	unsigned blockctrl[2];	// blockctrl[0] for activate control, blockctrl[1] for initialize control 
	unsigned pathctrl;
	unsigned status;
	unsigned oldstatus;
	unsigned properties;
	unsigned errflag;
	unsigned errcnt;
	
	DRVISPJpegInfo	jpeginfo;

	unsigned 	nevents;
	HEvent		hevent;
	DRVISPEvent	event; 
	
	unsigned nprofiles; 	
	DRVISPPreset **preset;
	DRVISPCBFunc cbfunc;
	HProfile    hprofile;
	HProfile 	hcurprofile;
	HProfile 	hnextprofile;
	HProfile 	hprevprofile;
} DRVISP;

typedef struct {
	unsigned ie;
	unsigned si;
	unsigned isp;
	unsigned demosaic;
	unsigned flash;
	unsigned shutter;
	unsigned cproc;
	unsigned mrsz;
	unsigned srsz;
	unsigned mi;
	unsigned miinit;
	unsigned midma;
	unsigned midmastart;
	unsigned mipi;
	unsigned afm;
} DRVISPProfileCtrl;

typedef struct {
	unsigned prop;			// 0xb0210404
	unsigned hoffs;			// 0xb0210408
	unsigned voffs;			// 0xb021040c
	unsigned hsize;			// 0xb0210410
	unsigned vsize;			// 0xb0210414

//	unsigned isp_hoffs;		// 0xb0210594
//	unsigned isp_voffs;		// 0xb0210598
//	unsigned isp_hsize;		// 0xb021059c
//	unsigned isp_vsize;		// 0xb02105a0
} DRVISPProfileAcq;

typedef struct {
	//unsigned	mode;
	DRVISPRect 	window;
	unsigned	maxdx;
	unsigned 	maxdy;
} DRVISPProfileIS;

typedef struct {
	unsigned num;
	unsigned skip;
	//DRVISPRect rsz;
	DRVISPRectSize dst;
	unsigned char* addr1;
	unsigned char* addr2;
	unsigned char* addr3;
	unsigned  size1;
	unsigned  size2;
	unsigned  size3;
	unsigned dtype;
	unsigned char* lastaddr1;
	unsigned char* lastaddr2;
	unsigned char* lastaddr3;
	unsigned  lastsize;
	unsigned char* nextaddr1;
	unsigned char* nextaddr2;
	unsigned char* nextaddr3;
} DRVISPProfileMPath;

typedef struct {
	unsigned num;
	unsigned skip;
	//DRVISPRect rsz;
	DRVISPRegion dst;
	unsigned char* addr1;
	unsigned char* addr2;
	unsigned char* addr3;
	unsigned  size1;
	unsigned  size2;
	unsigned  size3;
	unsigned dtype;
	unsigned char* lastaddr1;
	unsigned char* lastaddr2;
	unsigned char* lastaddr3;
	unsigned char* nextaddr1;
	unsigned char* nextaddr2;
	unsigned char* nextaddr3;

} DRVISPProfileSPath;

typedef struct {
	unsigned sel;
	unsigned num;
	DRVISPRegion src;
	unsigned char* addr1;
	unsigned char* addr2;
	unsigned char* addr3;
	unsigned dtype;
} DRVISPProfileRPath;

typedef struct {
	int	reserved;
} DRVISPProfileRegion;

typedef struct {
	int	reserved;
} DRVISPProfileClk;

typedef struct {
	int	reserved;
} DRVISPProfileEvent;

#define DRVISPSCTBL_MAX 256
// 48byte * DRVISPSCTBL_MAX
typedef struct DRVISPSCTBL
{
	unsigned short offx;
	unsigned short offy;
	unsigned short w;
	unsigned short h;
	unsigned short mctrl;
	unsigned short mschy;
	unsigned short mschcb;
	unsigned short mschcr;
	unsigned short mscvy;
	unsigned short mscvc;
	unsigned short mphhy;
	unsigned short mphhc;
	unsigned short mphvy;
	unsigned short mphvc;
	unsigned short sctrl;
	unsigned short sschy;
	unsigned short sschcb;
	unsigned short sschcr;
	unsigned short sscvy;
	unsigned short sscvc;
	unsigned short sphhy;
	unsigned short sphhc;
	unsigned short sphvy;
	unsigned short sphvc;

} DRVISPSCTBL;

//static DRVISPSCTBL s_drvisp_sctbl[DRVISPSCTBL_MAX];
typedef struct DRVISPProfile {
	unsigned 				mode;
	unsigned				path;
	DRVISPPreset			*preset;

	DRVISPProfileCtrl		ctrl;
	DRVISPProfileAcq		acq;
	DRVISPProfileIS			is;
	DRVISPProfileMPath		mp;
	DRVISPProfileSPath		sp;
	DRVISPProfileRPath		rp;
	DRVISPSCTBL 			sctbl[DRVISPSCTBL_MAX];
	unsigned				sctbl_max;
	int						zoomlvl;
	unsigned				iemode;
	
	HIsp             		hisp;
	HProfile				next;
} DRVISPProfile;

typedef struct DRVISPFBM {
	unsigned		mode;
	unsigned		status;
	unsigned		count;
	unsigned		sub_count;
	unsigned		*addr1;
	unsigned		*addr2;
	unsigned		*addr3;
	
} DRVISPFBM ;

static unsigned s_drvisp_transtbl[DRVISPST_MAX] =
{ 
	DRVISPSTBIT_IDLE ,		// DRVISPST_START
	DRVISPSTBIT_IDLE ,		// DRVISPST_POWERDOWN
	DRVISPSTBIT_START|DRVISPSTBIT_POWERDOWN|DRVISPSTBIT_STREAMING, // DRVISPST_IDLE
	DRVISPSTBIT_IDLE ,		// DRVISPST_STREAMING
	DRVISPSTBIT_IDLE  		// DRVISPST_DEBUG
};

int drvispprofile_setzoom(HProfile hprofile, int level, int apply);
int drvispprofile_setzoomtbl(HProfile hprofile, int level, DRVISPRect* zoom, int reserved);
//int drvispprofile_makezoomtbl(HProfile hprofile, DRVISPRect* zoom1, DRVISPRect* zoom2, int outw, int outh, int step, int reserved);
static int drvispprofile_zoom_apply(HProfile hprofile, int reserved);
static int drvispprofile_ie_apply(HProfile hprofile, int reserved);
static int drvispprofile_setacq_apply (HProfile hprofile);

int	drvispprofile_setacq(HProfile hprofile, unsigned prop, unsigned hoffs, unsigned voffs, unsigned hsize, unsigned vsize );
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static int drvisp_irq_apply(HIsp hisp);
static int drvisp_stream_apply(HIsp hisp, unsigned ctrl);
static int drvisp_stop_apply(HIsp hisp, unsigned ctrl, unsigned term );
static int drvispprofile_window_apply (HProfile hprofile);
static int drvispprofile_mi_apply( HProfile hprofile);
static int drvisp_setprofile_apply(HIsp hisp, int update);
static unsigned	drvisp_irq1_get(HIsp hisp, unsigned events);
static unsigned 	drvisp_irq1_getraw(HIsp hisp, unsigned events);
static unsigned 	drvisp_irq1_clear(HIsp hisp, unsigned events);
static unsigned	drvisp_irq1_set(HIsp hisp, unsigned events);
static unsigned	drvisp_irq1_mask(HIsp hisp, unsigned events);
static unsigned	drvisp_irq1_unmask(HIsp hisp, unsigned events);

static unsigned	drvisp_irq2_get(HIsp hisp, unsigned events);
static unsigned 	drvisp_irq2_getraw(HIsp hisp, unsigned events);
static unsigned 	drvisp_irq2_clear(HIsp hisp, unsigned events);
static unsigned	drvisp_irq2_set(HIsp hisp, unsigned events);
static unsigned	drvisp_irq2_mask(HIsp hisp, unsigned events);
static unsigned	drvisp_irq2_unmask(HIsp hisp, unsigned events);

static unsigned	drvisp_irq3_get(HIsp hisp, unsigned events);
static unsigned 	drvisp_irq3_getraw(HIsp hisp, unsigned events);
static unsigned 	drvisp_irq3_clear(HIsp hisp, unsigned events);
static unsigned	drvisp_irq3_set(HIsp hisp, unsigned events);
static unsigned	drvisp_irq3_mask(HIsp hisp, unsigned events);
static unsigned	drvisp_irq3_unmask(HIsp hisp, unsigned events);

static unsigned	drvisp_irq4_get(HIsp hisp, unsigned events);
static unsigned 	drvisp_irq4_getraw(HIsp hisp, unsigned events);
static unsigned 	drvisp_irq4_clear(HIsp hisp, unsigned events);
static unsigned	drvisp_irq4_set(HIsp hisp, unsigned events);
static unsigned	drvisp_irq4_mask(HIsp hisp, unsigned events);
static unsigned	drvisp_irq4_unmask(HIsp hisp, unsigned events);
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

int drvisppreset_getdata(DRVISPPreset *preset, unsigned devid, unsigned addr, unsigned *data)
{
	int errono = 0;
	int count = DRVISP_PRESET_MAX;
	int end_of_profile = 0;

	while (count--)
	{
		if ( preset->devid == devid)
		{
			if (preset->addr == addr)
			{	
				*data = preset->data;
				break;
			}
		} else if ( (preset->devid&0xff) == 0xff )
		{
			end_of_profile = 1;
			break;
		}
		preset++;
	}
	
	if (count)
	{
		return DRVISP_PRESET_MAX-count; // ret : count	
	}
	else
	{
		return 0; // error
	}
}

int drvisp_loadpreset_apply (HIsp hisp, DRVISPPreset * preset)
{
	int errno = 0;
	unsigned BaseAddr = 0xffffffff;
	volatile unsigned long *pMrv;
	volatile unsigned char *pMrv2;
	int count = TCISP_PROFILE_MAX;
	int end_of_profile = 0;
	
	while (count--)
	{
		switch (preset->devid & DRVISPPRS_DEVID_TYPE)
		{
			case DRVISPPRS_DEVID_TYPE_ISP:
				
				if ((preset->devid & DRVISPPRS_DEVID_DATA) == DRVISPPRS_DEVID_DATA_32BIT)
				{
					pMrv =  (HwMRVISP_BASE_PTR( preset->addr));
					*pMrv = (unsigned long) preset->data;
				}
				else if ((preset->devid & DRVISPPRS_DEVID_DATA) == DRVISPPRS_DEVID_DATA_8BIT)
				{
					pMrv2 = (volatile unsigned char *) (HwMRVISP_BASE_PTR( preset->addr));
					*pMrv2 = (unsigned char) preset->data;				
				}
				
				break;
			case DRVISPPRS_DEVID_TYPE_I2C:
				hisp->cbfunc.I2cWrite( preset->devid&DRVISPPRS_DEVID_ID,preset->addr, preset->data ,( (preset->devid&DRVISPPRS_DEVID_DATA_16BIT)>>7) | ((preset->devid&DRVISPPRS_DEVID_ADDR_16BIT)>>10) );
				{
					int i=0;
					for (i=0; i < 0x10000; i++) ;
				}
				break;
			case DRVISPPRS_DEVID_TYPE_CMD:
				switch (preset->devid&0xff)
				{
					case 1 : // sleep
					{
						int i;
					for (i=0; i < 0x100*preset->data; i++)    /* __asm { nop; }*/ ;
					}
					break;
					
					case 0x80 : // reset
					break;
					case 0x81 : // set base address
						BaseAddr = preset->data;
					break;
					
					case 0xff : // end of profile
						end_of_profile = 1;
					break;
				}
				break;
			case DRVISPPRS_DEVID_TYPE_ADDR:
				if (BaseAddr == 0xffffffff)
				{
					end_of_profile = 1;
					errno = 1;				// baseaddr isn't initialized.
					break;
				}
				
				if ((preset->devid & DRVISPPRS_DEVID_DATA) == DRVISPPRS_DEVID_DATA_32BIT)
				{
					pMrv =  (volatile unsigned long*)(BaseAddr + preset->addr );
					*pMrv = (unsigned long) preset->data;
				}
				else if ((preset->devid & DRVISPPRS_DEVID_DATA) == DRVISPPRS_DEVID_DATA_8BIT)
				{
					pMrv2 = (volatile unsigned char *) (BaseAddr + preset->addr );
					*pMrv2 = (unsigned char) preset->data;				
				}
				
				break;
			//default:
				// error handler required
		}
		if (end_of_profile)
		{
			break;
		}
		preset++;
	}
	
	if (count)
	{
		return DRVISP_PRESET_MAX-count; // ret : count	
	}
	else
	{
		return 0; // error
	}
}

int drvisp_loadpreset(HIsp hisp, int idx, char* str, int reserved)
{
	int errno = 0;
	if (!hisp)
	{
		errno = 1; // invalid hisp handler
		return errno;
	}
	if (idx >= DRVISPPRESET_PRESET_MAX) 
	{
		errno = 1; // invalid preset index
		return errno;
	}

	errno = drvisp_loadpreset_apply( hisp, hisp->preset[idx] );
	return errno;
}

int drvisp_makeprofile(HIsp hisp, HProfile hprofile, DRVISPPreset *preset, int reserved)
{
	int errno = 0;
	if (!hisp)
	{
		errno = 1; // invalid hisp handler
		return errno;
	}
	if (!hprofile)
	{
		errno = 1; // invalid hprofile handler
		return errno;
	}
	memset( hprofile, 0, sizeof(DRVISPProfile) );
	
	// profile 초기화 
	hprofile->sctbl_max = 0;
	hprofile->zoomlvl = -1;
	hprofile->preset = preset;
	hprofile->hisp = hisp;
	hprofile->next = 0;
	// link연결
	{
		HProfile node = hisp->hprofile;
		if (node)
		{
			while (node->next)
			{
				node = node->next;
			}
			node->next = hprofile;
		}
		else
		{
			hisp->hprofile = hprofile;
		}
		hisp->nprofiles++;
	}
	drvispprofile_setacq(hprofile, -1, -1, -1, -1, -1);		// preset에서 정보를 가져옴 
			
	return 0;
}


int drvisp_setmode(HIsp hisp, int mode, int reserved )
{
	int errno = 0;
	if (!hisp)
	{
		errno = 1; // invalid hisp handler
		return errno;
	}
	if (mode == DRVISPMODE_PARALLEL_YUV)
	{
		hisp->properties |= DRVISPFLAG_PARALLELYUV_MODE;
	}
	return 0;
}

int	drvispprofile_setacq(HProfile hprofile, unsigned prop, unsigned hoffs, unsigned voffs, unsigned hsize, unsigned vsize )
{
	int errno = 0;
	
	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}
		
	hprofile->acq.prop = prop;
	hprofile->acq.hoffs = hoffs;
	hprofile->acq.voffs = voffs;
	hprofile->acq.hsize = hsize;
	hprofile->acq.vsize = vsize;

	return 0;
}
		
int drvisp_setprofile(HIsp hisp, HProfile hprofile, int apply)
{
	int errno = 0;
	if (!hisp)	//invalid isp handler
	{
		errno = 1;
		return errno;
	}
	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}
	/*
	// status가 idle이 아니면 error
	// 
	if (hisp->status != DRVISPST_IDLE)
	{
		errno = 1;
		return errno; // 
	}
	*/
	hisp->hnextprofile = hprofile;


	
	switch (apply & 0xf)
	{
		case DRVISPAPPLY_IMMEDIATELY:
			errno = drvisp_setprofile_apply(hisp, 1);
			break;
		case DRVISPAPPLY_ENDOFFRAME:
			hisp->properties |= DRVISPFLAG_PROFILEUPDATE_ENDOFFRAME;
			break;
		case DRVISPAPPLY_MODECHANGE:
			hisp->properties |= DRVISPFLAG_PROFILEUPDATE_MODECHANGE;
			break;
		default:
			errno = 1; // invlaid apply param
	};
	
	return 0;
}

static int drvisp_setprofile_apply(HIsp hisp, int update)
{
	HProfile hprofile;
	int errno = 0;
	if (!hisp)	//invalid profile handler  // 내부 호출 
	{
		errno = 1;
		return errno;
	}
	if (update)
	{
		hisp->hprevprofile = hisp->hcurprofile;
		hisp->hcurprofile = hisp->hnextprofile;
		hisp->hnextprofile = 0;
	}
	hprofile = hisp->hcurprofile;
	if (!hprofile)	// 잘 못 등록되어지는 것은 driver 개발자 실수 
	{
		errno = 1;
		return errno;		
	}

	// @ 센서 출력 disable 
	drvisp_loadpreset_apply(hisp, hisp->preset[0]);
	drvisp_loadpreset_apply(hisp, hprofile->preset);
	errno = drvispprofile_ie_apply(hprofile, 1);
	//drvispprofile_si_apply(hprofile);
	errno = drvisp_irq_apply(hisp);
	errno = drvispprofile_window_apply(hprofile);
	errno = drvispprofile_setacq_apply(hprofile);
	errno = drvispprofile_mi_apply(hprofile);
	if (hprofile->zoomlvl >= 0)
	{
		errno = drvisp_zoom_apply(hisp,1);
	}

	if (hprofile->mp.size1 && ((hprofile->mp.dtype & DRVISPDTYPE_MAX) == DRVISPDTYPE_JPEG) )
	{
		unsigned result;
		unsigned size = 0;
		result = MrvJpeInitEx(hprofile->mp.dst.width, hprofile->mp.dst.height, 1 /* JPEG_90_PERCENT */, 1); // for BT.601 scale up
		if (result != RET_SUCCESS) {
			DBG_OUT( (DERR, "ERR: MrvSls_DoSnapshot: failed to init/setup JPEG encode subsystem\n") );
		} 

		{ /////////////////////////////////////////////////////////////////////////////////
			RESULT RetVal = RET_SUCCESS;

			// generate header
			RetVal = MrvJpeGenerateHeader(MRV_JPE_HEADER_MODE_JFIF);
			if (RetVal != RET_SUCCESS)
			{
				errno = 1;
				return errno;
			}

			// now encode JPEG
			errno = drvisp_stream_apply(hisp, 0x100);    //profile_mi_apply(hprofile);
			if (errno)
				return errno;
			drvispevent_ctrlisp(&hisp->event, DRVISPINT_OUT_FRAME_END);
			drvispevent_waitisp( &hisp->event, DRVISPINT_OUT_FRAME_END, 0);
			
			MrvJpePrepEnc(eMrvJpe_SingleShot);

			RetVal = MrvJpeWaitForEncodeDone();
			if (RetVal != RET_SUCCESS)
			{
				errno = 1;
				return errno;
			}
			size = MrvMifGetByteCnt();    
			hisp->jpeginfo.addr = hprofile->mp.addr1; 
			hisp->jpeginfo.size = size;
			hisp->jpeginfo.tickcount = 0;			// temporary
			hisp->jpeginfo.framenum = 0;
			hisp->jpeginfo.flags = 0;
			hisp->jpeginfo.idx = 0;
			hisp->jpeginfo.iso = 100;			// temporary
			hisp->jpeginfo.exposure = 170000;	// temporary
			hisp->jpeginfo.wb = 0;				// temporary
				
		}  /////////////////////////////////////////////////////////////////////////////////

	}
	else
	{
		/*
		if (hprofile->mp.size1 && (hprofile->mp.num) )
		{
			volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;			
			REG_WRITE(ptMrvReg->isp_acq_nr_frames, hprofile->mp.num);
		}
		*/
		errno = drvisp_stream_apply(hisp, 0x100);    //profile_mi_apply(hprofile);
	}
	return errno;
}

#define DRVISPCTRL_SYNC_UPD				1		
#define DRVISPCTRL_WAIT_SYNC			1		
#define DRVISPCTRL_ACTIVE				1		
#define DRVISPCTRL_UNDER_CTRL			1		

#define DRVISPCTRL_SENSOR_ACTIVE		0x20
#define DRVISPCTRL_ISP_ACTIVE			0x40
#define DRVISPCTRL_MI_ACTIVE			0x80

#define DRVISPCTRL_AT_FIRST_TIME		0x100

#define DRVISPAT_ANY_TIME				0
#define DRVISPAT_SENSOR_BLANK			1	// syncronous sensor control
#define DRVISPAT_ISP_BLANK				2	// ? 3A control
#define DRVISPAT_MI_BLANK				3	// suncronous buffer control

int drvisp_reset_apply (HIsp hisp, int mask)
{
	int errno = 0;
	int count = 0;
	int pathnum = 0;
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;			
		
	if (!hisp)	//invalid isp handler
	{
		errno = 1;
		return errno;
	}
 	drvisp_loadpreset_apply(hisp, hisp->preset[0]);	// isp_reset
	if (hisp->status == DRVISPST_STREAMING)
	{
		hisp->errcnt = 0;
		hisp->errflag = 0;
		hisp->properties &= ~DRVISPFLAG_ERROR_OCCURED;		  		
	
		//errno = drvisp_setprofile_apply(hisp,0);
		errno = drvisp_setstatus (hisp, DRVISPST_IDLE);
		//errno = drvisp_setstatus (hisp, DRVISPST_STREAMING);
	}
	return errno;
}
static int drvisp_irq_apply(HIsp hisp)
{
	int errno = 0;
	HEvent node;
	unsigned mask1 = 0;
	unsigned mask2 = 0;
	unsigned mask3 = 0;
	unsigned mask4 = 0;
	
	if (!hisp)	//invalid isp handler
	{
		errno = 1;
		return errno;
	}

	node = hisp->hevent;;
	while (node)
	{
		node->event1 = 0;
		node->event2 = 0;
		node->event3 = 0;
		node->event4 = 0;
		mask1 |= node->eventmask1;
		mask2 |= node->eventmask2;
		mask3 |= node->eventmask3;
		mask4 |= node->eventmask4;

		node = node->next;
	}
	drvisp_irq1_clear(hisp, mask1 );
	drvisp_irq1_unmask(hisp, ~mask1  );
	drvisp_irq1_mask(hisp, mask1  );

	drvisp_irq2_clear(hisp, mask2 );
	drvisp_irq1_unmask(hisp, ~mask2  );
	drvisp_irq2_mask(hisp, mask2  );

	drvisp_irq3_clear(hisp, mask3 );
	drvisp_irq1_unmask(hisp, ~mask3  );
	drvisp_irq3_mask(hisp, mask3  );

	drvisp_irq4_clear(hisp, mask4 );
	drvisp_irq1_unmask(hisp, ~mask4  );
	drvisp_irq4_mask(hisp, mask4 );
	
	return 0;
}

static int drvisp_stream_apply(HIsp hisp, unsigned ctrl)
{
	int errno = 0;
	int count = 0;
	int pathnum = 0;
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;			
	unsigned ispctrl = REG_READ(ptMrvReg->isp_ctrl);
	
	if (!hisp)	//invalid isp handler
	{
		errno = 1;
		return errno;
	}

	if( (ptMrvReg->isp_flags_shd & (MRV_ISP_INFORM_EN_SHD_MASK | MRV_ISP_ISP_ON_SHD_MASK) )  == (MRV_ISP_INFORM_EN_SHD_MASK | MRV_ISP_ISP_ON_SHD_MASK) )
	{
		errno = 1; // 완전히 멈추지 않으면 진입 불가 
		return errno;
	}

	// mi enable
	// isp enable
	// sensor enable

	if (hisp->hcurprofile->mp.size1)
	{
		hisp->hcurprofile->mp.lastaddr1 = 0 ;
		hisp->hcurprofile->mp.lastaddr2 = 0 ;
		hisp->hcurprofile->mp.lastaddr3 = 0 ;
		hisp->hcurprofile->mp.lastsize = 0 ;
		hisp->hcurprofile->mp.nextaddr1 = (unsigned char *) (ptMrvReg->mi_mp_y_base_ad_init + ptMrvReg->mi_mp_y_offs_cnt_init  ) ;
		hisp->hcurprofile->mp.nextaddr2 = (unsigned char *) (ptMrvReg->mi_mp_cb_base_ad_init + ptMrvReg->mi_mp_cb_offs_cnt_init );
		hisp->hcurprofile->mp.nextaddr3 = (unsigned char *) (ptMrvReg->mi_mp_cr_base_ad_init + ptMrvReg->mi_mp_cr_offs_cnt_init  );
    	}
	if (hisp->hcurprofile->sp.size1)
	{
		hisp->hcurprofile->sp.lastaddr1 = 0;
		hisp->hcurprofile->sp.lastaddr2 = 0;
		hisp->hcurprofile->sp.lastaddr3 = 0;
		hisp->hcurprofile->sp.nextaddr1 = (unsigned char *) (ptMrvReg->mi_sp_y_base_ad_init + ptMrvReg->mi_sp_y_offs_cnt_init );
		hisp->hcurprofile->sp.nextaddr2 = (unsigned char *)(ptMrvReg->mi_sp_cb_base_ad_init + ptMrvReg->mi_sp_cb_offs_cnt_init );
		hisp->hcurprofile->sp.nextaddr3 = (unsigned char *) (ptMrvReg->mi_sp_cr_base_ad_init + ptMrvReg->mi_sp_cr_offs_cnt_init );
	}   
		
	drvispevent_ctrlmi( &hisp->event, DRVISPINTMI_MP_FRAME_END|DRVISPINTMI_SP_FRAME_END);
	REG_SET_SLICE(ispctrl, MRV_ISP_ISP_INFORM_ENABLE, 1);
	REG_SET_SLICE(ispctrl, MRV_ISP_ISP_ENABLE, 1);
	REG_SET_SLICE(ispctrl, MRV_ISP_ISP_CFG_UPD, 1);
	REG_WRITE(ptMrvReg->isp_ctrl, ispctrl);

#if 0
	if  ((hisp->hcurprofile->mp.dtype & DRVISPDTYPE_MAX) != DRVISPDTYPE_JPEG) 
	{
		if (hisp->hcurprofile->mp.size1)
		{
			unsigned result;
			result = drvispevent_waitmi( &hisp->event, DRVISPINTMI_MP_FRAME_END, 0);
			if (result == 0x80000001)	errno = 0x80000001;				
	    }
		if (hisp->hcurprofile->sp.size1)
		{
			unsigned result;
			result = drvispevent_waitmi( &hisp->event, DRVISPINTMI_SP_FRAME_END, 0);
			if (result == 0x80000001)	errno = 0x80000001;				
		}   
	}
	else
	{
		volatile unsigned *pflag;
		pflag	= &(hisp->properties);
		if ( *pflag & DRVISPFLAG_ERROR_OCCURED )
		{
			errno = 0x80000001;
			return errno; // ISP_ERROR
		}		
	}
#else
	{
		volatile unsigned *pflag;
		pflag	= &(hisp->properties);
		*pflag |= DRVISPFLAG_WAIT_ONE_FRAME;
	}
#endif	

	return errno;
}

int drvisp_stop_irq_apply(HIsp hisp)
{
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*) MEM_MRV_REG_BASE;
	
	REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_INFORM_ENABLE, 0);
	REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_ENABLE, 0);
	
	REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_CFG_UPD, 1);
}

static int drvisp_stop_apply(HIsp hisp, unsigned ctrl, unsigned term )
{
	int errno = 0;
	int count = 0;
	int wait_mi = 1;
	unsigned dtype;
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;			
	unsigned isp_ctrl = REG_READ(ptMrvReg->isp_ctrl);
	volatile unsigned *pflag;
	
	if (!hisp)	//invalid isp handler
	{
		errno = 1;
		return errno;
	}
	if( ! (ptMrvReg->isp_flags_shd & MRV_ISP_INFORM_EN_SHD_MASK) )
	{
		errno = 1;
		//return errno;
	}
	if( ! (hisp->hcurprofile->mp.size1 || hisp->hcurprofile->sp.size1) )
	{
		errno = 1;
		//return errno;
	}
	hisp->properties |= 	DRVISPFLAG_STOP_CMD;

	drvispevent_ctrlisp( &hisp->event, DRVISPINT_ISP_OFF);
	if (hisp->hcurprofile->mp.size1)
	{
		REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_MP_ENABLE, 0);
	}
	else 
	{
		REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_SP_ENABLE, 0);
	}

	REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_INFORM_ENABLE, 0);
	REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_ENABLE, 0);
#if 0
	if( ptMrvReg->isp_flags_shd & (MRV_ISP_INFORM_EN_SHD_MASK|MRV_ISP_ISP_ON_SHD_MASK)  == (MRV_ISP_INFORM_EN_SHD_MASK|MRV_ISP_ISP_ON_SHD_MASK) )
	{
		REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, 1);
	}
	else
	{
		REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_CFG_UPD, 1);
		wait_mi = 0;
	}
	REG_WRITE(ptMrvReg->isp_ctrl, isp_ctrl);


	dtype = hisp->hcurprofile->mp.dtype & DRVISPDTYPE_MAX;
	pflag	= &(hisp->properties);
	//if (!errno && xxxxx)	// error가 없다면 
	{
		if (hisp->hcurprofile->mp.size1 && wait_mi )
		{
			if  (dtype != DRVISPDTYPE_JPEG) 
			{
				while ( ! (*pflag &  DRVISPFLAG_MP_LASTFRAME_END)  )	
				{
					if((ptMrvReg->isp_flags_shd & (MRV_ISP_INFORM_EN_SHD_MASK|MRV_ISP_ISP_ON_SHD_MASK)) == (MRV_ISP_INFORM_EN_SHD_MASK|MRV_ISP_ISP_ON_SHD_MASK))
					{
						count++;
					}
					else
					{
						errno = 0x80000001;
						break;
					}
				}
			}
			*pflag &= ~DRVISPFLAG_MP_LASTFRAME_END;
		}
		if (hisp->hcurprofile->sp.size1 && wait_mi)
		{
				while ( ! (*pflag &  DRVISPFLAG_SP_LASTFRAME_END)  )	
				{
					if((ptMrvReg->isp_flags_shd & (MRV_ISP_INFORM_EN_SHD_MASK|MRV_ISP_ISP_ON_SHD_MASK)) == (MRV_ISP_INFORM_EN_SHD_MASK|MRV_ISP_ISP_ON_SHD_MASK))
					{
						count++;
					}
					else
					{
						errno = 0x80000001;
						break;
					}
				}

			*pflag &= ~DRVISPFLAG_SP_LASTFRAME_END;
		}
	}
	//else
	//{
	//	time_delay
	//}

	*pflag &= ~DRVISPFLAG_ISP_OFF;	
#else
	if( ptMrvReg->isp_flags_shd & (MRV_ISP_INFORM_EN_SHD_MASK|MRV_ISP_ISP_ON_SHD_MASK)  == (MRV_ISP_INFORM_EN_SHD_MASK|MRV_ISP_ISP_ON_SHD_MASK) )
	{
		REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, 1);
		
		pflag	= &(hisp->properties);
		*pflag |= DRVISPFLAG_WAIT_ONE_FRAME;
	}
	else
	{
		REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_CFG_UPD, 1);
		wait_mi = 0;
	}
	REG_WRITE(ptMrvReg->isp_ctrl, isp_ctrl);

#endif

	return errno;
}

static int drvispprofile_setacq_apply (HProfile hprofile)
{
	int errno = 0;
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	DRVISPProfileAcq *pacq = &(hprofile->acq);
	unsigned acq_prop = 0;
	
	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}
	if ( pacq->prop != -1 )
	{
		acq_prop = REG_READ(ptMrvReg->isp_acq_prop);
		if ((pacq->prop & DRVISPACQ_MODE_MASK) && (pacq->prop & DRVISPACQ_MODE_PARALLEL))
		{	hprofile->hisp->properties |= DRVISPFLAG_PARALLELYUV_MODE;	
		}else
		{
		}
		/*
		if ((pacq->prop & DRVISPACQ_SYNC_MASK) && (pacq->prop & DRVISPACQ_SYNC_BT601))
		{
		}else
		{
		}
		*/
		if ((pacq->prop & DRVISPACQ_PCLKPOL_MASK) && (pacq->prop & DRVISPACQ_PCLKPOL_NEGA))
		{	REG_SET_SLICE(acq_prop, MRV_ISP_SAMPLE_EDGE, 0);
		}else
		{	REG_SET_SLICE(acq_prop, MRV_ISP_SAMPLE_EDGE, 1);
		}
		if ((pacq->prop & DRVISPACQ_HSYNCPOL_MASK) && (pacq->prop & DRVISPACQ_HSYNCPOL_HIGH))
		{	REG_SET_SLICE(acq_prop, MRV_ISP_HSYNC_POL, 0);
		}else
		{	REG_SET_SLICE(acq_prop, MRV_ISP_HSYNC_POL, 1);
		}
		if ((pacq->prop & DRVISPACQ_VSYNCPOL_MASK) && (pacq->prop & DRVISPACQ_VSYNCPOL_HIGH))
		{	REG_SET_SLICE(acq_prop, MRV_ISP_VSYNC_POL, 0);
		}else
		{	REG_SET_SLICE(acq_prop, MRV_ISP_VSYNC_POL, 1);
		}
		/*
		if (pacq->prop & DRVISPACQ_BIT_MASK)
		{
		}
		*/
		if (pacq->prop & DRVISPACQ_CCIRSEQ_MASK)
		{
			if ((pacq->prop & DRVISPACQ_CCIRSEQ_MASK) == DRVISPACQ_CCIRSEQ_YCBYCR)
			{	REG_SET_SLICE(acq_prop, MRV_ISP_CCIR_SEQ, 0);
		}
			else if ((pacq->prop & DRVISPACQ_CCIRSEQ_MASK) == DRVISPACQ_CCIRSEQ_YCRYCB)
			{	REG_SET_SLICE(acq_prop, MRV_ISP_CCIR_SEQ, 1);
			}
			else if ((pacq->prop & DRVISPACQ_CCIRSEQ_MASK) == DRVISPACQ_CCIRSEQ_CBYCRY)
			{	REG_SET_SLICE(acq_prop, MRV_ISP_CCIR_SEQ, 2);
			}
			else if ((pacq->prop & DRVISPACQ_CCIRSEQ_MASK) == DRVISPACQ_CCIRSEQ_CRYCBY)
			{	REG_SET_SLICE(acq_prop, MRV_ISP_CCIR_SEQ, 3);
			}
		}

		if (pacq->prop & DRVISPACQ_FIELD_MASK)
		{
			if ((pacq->prop & DRVISPACQ_FIELD_MASK) == DRVISPACQ_FIELD_ALL)
			{	REG_SET_SLICE(acq_prop, MRV_ISP_FIELD_SELECTION, 0);
		}		
			else if ((pacq->prop & DRVISPACQ_FIELD_MASK) == DRVISPACQ_FIELD_EVEN)
			{	REG_SET_SLICE(acq_prop, MRV_ISP_FIELD_SELECTION, 1);
			}
			else if ((pacq->prop & DRVISPACQ_FIELD_MASK) == DRVISPACQ_FIELD_ODD)
			{	REG_SET_SLICE(acq_prop, MRV_ISP_FIELD_SELECTION, 2);
			}
		}		
		/*
		if (pacq->prop & DRVISPACQ_BAYERPAT_MASK)
		{
		}	
	
		if (pacq->prop & DRVISPACQ_CONV422_MASK)
		{
		}			
		*/
		REG_WRITE(ptMrvReg->isp_acq_prop,acq_prop );
	}
	if (pacq->hoffs != -1)
	{
		ptMrvReg->isp_acq_h_offs = pacq->hoffs;
	}
	if (pacq->voffs != -1)
	{
		ptMrvReg->isp_acq_v_offs = pacq->voffs;
	}
	if (pacq->hsize != -1)
	{
		ptMrvReg->isp_acq_h_size = pacq->hsize;
	}
	if (pacq->vsize != -1)
	{
		ptMrvReg->isp_acq_v_size = pacq->vsize;
	}
	
	return 0;
}

static int drvispprofile_window_apply (HProfile hprofile)
{
	int errno = 0;
	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}
	{
		tsMrvIsConfig is;
		is.tMrvIsWindow.usHOffs = hprofile->is.window.offx;
		is.tMrvIsWindow.usVOffs = hprofile->is.window.offy;
		is.tMrvIsWindow.usHSize = hprofile->is.window.width;
		is.tMrvIsWindow.usVSize = hprofile->is.window.height;
		is.usMaxDx = hprofile->is.maxdx;
		is.usMaxDy = hprofile->is.maxdy;
		
		errno = MrvIsSetConfig( &is );
	}
	return 0;
}

static unsigned convDType(unsigned dtype)
{
	unsigned result=0;

	switch (dtype & DRVISPDTYPE_MAX)
	{
		case DRVISPDTYPE_YCbCr444:
			result = MRV_DPD_MODE_ISPYC | MRV_DPD_DMA_IN_444 ;
			break;
		case DRVISPDTYPE_YCbCr422:
			result = MRV_DPD_MODE_ISPYC | MRV_DPD_DMA_IN_422 ;
			break;
		case DRVISPDTYPE_YCbCr420:
			result = MRV_DPD_MODE_ISPYC | MRV_DPD_DMA_IN_422 | MRV_DPD_CSS_V2 ;
			break;
		case DRVISPDTYPE_RGB888:
			result = MRV_DPD_MODE_ISPYC | MRV_DPD_HWRGB_888 ;
			break;
		case DRVISPDTYPE_RGB666:
			result = MRV_DPD_MODE_ISPYC | MRV_DPD_HWRGB_666 ;
			break;
		case DRVISPDTYPE_RGB565:
			result = MRV_DPD_MODE_ISPYC | MRV_DPD_HWRGB_565 ;
			break;
		case DRVISPDTYPE_RAW12:
			result =  MRV_DPD_MODE_ISPRAW_16B ;
			break;
		case DRVISPDTYPE_RAW8:
			result =  MRV_DPD_MODE_ISPRAW ;
			break;
		case DRVISPDTYPE_JPEG:
			result =  MRV_DPD_MODE_ISPJPEG ;
			break;
		case DRVISPDTYPE_DP:
			result = MRV_DPD_MODE_DPCC ;
			break;
	};

	switch (dtype & DRVISPDTYPE_YUVTYPE_MAX)
	{
		case DRVISPDTYPE_PLANAR:
			break;
		case DRVISPDTYPE_SEMIPLANAR:
			break;
		case DRVISPDTYPE_INTERLEAVED:
			result |= MRV_DPD_INTERLEAVED ;
			break;
	};
	
	return result;
}

static unsigned conv2DType(unsigned dtype)
{
	unsigned result=0;

	switch (dtype & DRVISPDTYPE_MAX)
	{
		case DRVISPDTYPE_YCbCr444:
			result =eBufType_Yuv422 ;
			break;
		case DRVISPDTYPE_YCbCr422:
			result = eBufType_Yuv422 ;
			break;
		case DRVISPDTYPE_YCbCr420:
			result = eBufType_Yuv420;
			break;
		case DRVISPDTYPE_RGB888:
			result = eBufType_Yuv422 ;
			break;
		case DRVISPDTYPE_RGB666:
			result = eBufType_Yuv422 ;
			break;
		case DRVISPDTYPE_RGB565:
			result = eBufType_Yuv422 ;
			break;
		case DRVISPDTYPE_RAW12:
			result =  eBufType_16bitBayer ;
			break;
		case DRVISPDTYPE_RAW8:
			result =  eBufType_8bitBayer ;
			break;
		case DRVISPDTYPE_JPEG:
			result =  eBufType_Yuv422 ;
			break;
		case DRVISPDTYPE_DP:
			result = eBufType_Yuv422 ;
			break;
	};

	switch (dtype & DRVISPDTYPE_YUVTYPE_MAX)
	{
		case DRVISPDTYPE_PLANAR:
			break;
		case DRVISPDTYPE_SEMIPLANAR:
			break;
		case DRVISPDTYPE_INTERLEAVED:
			//result |= MRV_DPD_INTERLEAVED ;
			break;
	};
	
	return result;
}

int drvispprofile_makezoomtbl(HProfile hprofile, DRVISPRect* zoom1, DRVISPRect* zoom2, int outw, int outh, int step, int reserved)
{
	int errno = 0;
	HIsp hisp = hprofile->hisp;

	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}
	hprofile->sctbl_max = step;
	
	return 0;
}

// 선행 함수 
// drvispprofile_setmpath_type()
// drvispprofile_setmpath_size()
// drvispprofile_setspath_type()
// drvispprofile_setspath_region()
int drvispprofile_setzoomtbl(HProfile hprofile, int level, DRVISPRect* zoom, int reserved)
{
	int errno = 0;
	HIsp hisp = hprofile->hisp;

	if (!hprofile)	//invalid profile handler
	{
		errno = 0x000001;
		return errno;
	}

	{
		tsMrvDatapathDesc Source;
		tsMrvDatapathDesc Main;
		tsMrvDatapathDesc Self;
		tsMrvScale MScale;
		tsMrvScale SScale;
		unsigned ctrl;
		
		Source.usOutW = zoom->width;
		Source.usOutH = zoom->height;
		Source.ulFlags= 0 ;	// 사용안함 

		if (level >= hprofile->sctbl_max)
			hprofile->sctbl_max	= level;

		if (hprofile->mp.size1)
		{
			Main.usOutW = hprofile->mp.dst.width;
			Main.usOutH = hprofile->mp.dst.height;
		}
		else
		{
			Main.usOutW = Source.usOutW;
			Main.usOutH = Source.usOutH;
		}
		Main.ulFlags= convDType (hprofile->mp.dtype) | MRV_DPD_UPSCALE_SMOOTH_LIN | MRV_DPD_BURST_16 | ( (hprofile->mp.size1) ? MRV_DPD_ENABLE : MRV_DPD_DISABLE );

		if (hprofile->sp.size1)
		{
			Self.usOutW = hprofile->sp.dst.width;
			Self.usOutH = hprofile->sp.dst.height;
		}
		else
		{
			Self.usOutW = Source.usOutW;
			Self.usOutH = Source.usOutH;
		}
		Self.ulFlags= convDType (hprofile->sp.dtype) |MRV_DPD_UPSCALE_SMOOTH_LIN | MRV_DPD_BURST_16 | ( (hprofile->sp.size1) ? MRV_DPD_ENABLE : MRV_DPD_DISABLE );
		
		MrvSls_CalcScaleFactors( &Source, &Main, &MScale, MARVIN_FEATURE_MSCALE_FACTORCALC );
		//MrvResSetMainResize( &MScale, eMrvCfgUpdateLater, (tsMrvRszLut *) MrvSls_GetRszLut(Main.ulFlags) );
		MrvSls_CalcScaleFactors( &Source, &Self, &SScale, MARVIN_FEATURE_SSCALE_FACTORCALC );
		//MrvResSetSelfResize( &SScale, eMrvCfgUpdateLater, (tsMrvRszLut *) MrvSls_GetRszLut(Self.ulFlags) );

		hprofile->sctbl[level].offx = zoom->offx;
		hprofile->sctbl[level].offy = zoom->offy;
		//hprofile->sctbl[level].offx = 0; //zoom->offx;
		//hprofile->sctbl[level].offy = 0; //zoom->offy;
		hprofile->sctbl[level].w	= zoom->width;
		hprofile->sctbl[level].h	= zoom->height;

		ctrl = (SScale.ulScaleVC & RSZ_UPSCALE_ENABLE)?0x10:0;	
		ctrl |= (SScale.ulScaleVC & RSZ_SCALER_BYPASS)?0:1;		ctrl <<=1;
		ctrl |= (SScale.ulScaleVY & RSZ_UPSCALE_ENABLE)?0x10:0;	
		ctrl |= (SScale.ulScaleVY & RSZ_SCALER_BYPASS)?0:1;		ctrl <<=1;
		ctrl |= (SScale.ulScaleHCB & RSZ_UPSCALE_ENABLE)?0x10:0;	
		ctrl |= (SScale.ulScaleHCB & RSZ_SCALER_BYPASS)?0:1;		ctrl <<=1;
		ctrl |= (SScale.ulScaleHY & RSZ_UPSCALE_ENABLE)?0x10:0;	
		ctrl |= (SScale.ulScaleHY & RSZ_SCALER_BYPASS)?0:1;		

		//errno = 0x40000000 | ctrl<<8; // info
		
		hprofile->sctbl[level].sctrl	= ctrl;
		hprofile->sctbl[level].sschy 	= SScale.ulScaleHY;
		hprofile->sctbl[level].sschcb	= SScale.ulScaleHCB;
		hprofile->sctbl[level].sschcr	= SScale.ulScaleHCR;
		hprofile->sctbl[level].sscvy	= SScale.ulScaleVY ;  
		hprofile->sctbl[level].sscvc	= SScale.ulScaleVC ;  
		hprofile->sctbl[level].sphhy	= SScale.usPhaseHY;
		hprofile->sctbl[level].sphhc	= SScale.usPhaseHC;
		hprofile->sctbl[level].sphvy	= SScale.usPhaseVY;
		hprofile->sctbl[level].sphvc	= SScale.usPhaseVC;

		ctrl = (MScale.ulScaleVC & RSZ_UPSCALE_ENABLE)?0x10:0;	
		ctrl |= (MScale.ulScaleVC & RSZ_SCALER_BYPASS)?0:1;		ctrl <<=1;
		ctrl |= (MScale.ulScaleVY & RSZ_UPSCALE_ENABLE)?0x10:0;	
		ctrl |= (MScale.ulScaleVY & RSZ_SCALER_BYPASS)?0:1;		ctrl <<=1;
		ctrl |= (MScale.ulScaleHCB & RSZ_UPSCALE_ENABLE)?0x10:0;	
		ctrl |= (MScale.ulScaleHCB & RSZ_SCALER_BYPASS)?0:1;		ctrl <<=1;
		ctrl |= (MScale.ulScaleHY & RSZ_UPSCALE_ENABLE)?0x10:0;	
		ctrl |= (MScale.ulScaleHY & RSZ_SCALER_BYPASS)?0:1;		

		//errno |= 0x40000000 | ctrl; // info

		hprofile->sctbl[level].mctrl	= ctrl;
		hprofile->sctbl[level].mschy	= MScale.ulScaleHY;  
		hprofile->sctbl[level].mschcb	= MScale.ulScaleHCB; 
		hprofile->sctbl[level].mschcr	= MScale.ulScaleHCR; 
		hprofile->sctbl[level].mscvy	= MScale.ulScaleVY ;  
		hprofile->sctbl[level].mscvc	= MScale.ulScaleVC ;  
		hprofile->sctbl[level].mphhy	= MScale.usPhaseHY;  
		hprofile->sctbl[level].mphhc	= MScale.usPhaseHC;  
		hprofile->sctbl[level].mphvy	= MScale.usPhaseVY ;  
		hprofile->sctbl[level].mphvc	= MScale.usPhaseVC ;  

		//if (zoom->height == 468)
		//	hprofile->sctbl[level].mscvy++;

		#if 0
		{
			int j;
			unsigned short *pdata;
			pdata = (unsigned short *) &hprofile->sctbl[level];				
			dbg_printf("%4d ",level);
			for (j=0; j< 4; j++)
			{
				dbg_printf("0x%04x(%4d) ",pdata[j],pdata[j]);
			}
			for (j=14; j< 24; j++)
			{
				dbg_printf("0x%04x ",pdata[j]);
			}
			dbg_printf("\n");
		}
		#endif
	}
	return 0;
}

int drvispprofile_setzoom(HProfile hprofile, int level, int apply)
{
	int errno = 0;
	HIsp hisp = hprofile->hisp;
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}
	hprofile->zoomlvl = level;
	if (level == -1)
	{
		return 0;
	}

	switch (apply & 0xf)
	{
		case DRVISPAPPLY_IMMEDIATELY:
		{
			ptMrvReg->isp_is_h_offs	= hprofile->sctbl[level].offx;
			ptMrvReg->isp_is_v_offs	= hprofile->sctbl[level].offy;
			ptMrvReg->isp_is_h_size	= hprofile->sctbl[level].w;
			ptMrvReg->isp_is_v_size	= hprofile->sctbl[level].h;

			ptMrvReg->srsz_ctrl			= hprofile->sctbl[level].sctrl & 0xff | 0x100;
			ptMrvReg->srsz_scale_hy		= hprofile->sctbl[level].sschy;
			ptMrvReg->srsz_scale_hcb	= hprofile->sctbl[level].sschcb;
			ptMrvReg->srsz_scale_hcr	= hprofile->sctbl[level].sschcr;
			ptMrvReg->srsz_scale_vy		= hprofile->sctbl[level].sscvy;
			ptMrvReg->srsz_scale_vc		= hprofile->sctbl[level].sscvc;
			ptMrvReg->srsz_phase_hy		= hprofile->sctbl[level].sphhy;
			ptMrvReg->srsz_phase_hc		= hprofile->sctbl[level].sphhc;
			ptMrvReg->srsz_phase_vy		= hprofile->sctbl[level].sphvy;
			ptMrvReg->srsz_phase_vc		= hprofile->sctbl[level].sphvc;
			
			ptMrvReg->mrsz_ctrl			= hprofile->sctbl[level].mctrl & 0xff | 0x100;
			ptMrvReg->mrsz_scale_hy		= hprofile->sctbl[level].mschy;
			ptMrvReg->mrsz_scale_hcb	= hprofile->sctbl[level].mschcb;
			ptMrvReg->mrsz_scale_hcr	= hprofile->sctbl[level].mschcr;
			ptMrvReg->mrsz_scale_vy		= hprofile->sctbl[level].mscvy;
			ptMrvReg->mrsz_scale_vc		= hprofile->sctbl[level].mscvc;
			ptMrvReg->mrsz_phase_hy		= hprofile->sctbl[level].mphhy;
			ptMrvReg->mrsz_phase_hc		= hprofile->sctbl[level].mphhc;
			ptMrvReg->mrsz_phase_vy		= hprofile->sctbl[level].mphvy;
			ptMrvReg->mrsz_phase_vc		= hprofile->sctbl[level].mphvc;
		}
			break;
		case DRVISPAPPLY_ENDOFFRAME:
		{
			ptMrvReg->isp_is_h_offs	= hprofile->sctbl[level].offx;
			ptMrvReg->isp_is_v_offs	= hprofile->sctbl[level].offy;
			ptMrvReg->isp_is_h_size	= hprofile->sctbl[level].w;
			ptMrvReg->isp_is_v_size	= hprofile->sctbl[level].h;

			ptMrvReg->srsz_ctrl			= hprofile->sctbl[level].sctrl & 0xff;
			ptMrvReg->srsz_scale_hy		= hprofile->sctbl[level].sschy;
			ptMrvReg->srsz_scale_hcb	= hprofile->sctbl[level].sschcb;
			ptMrvReg->srsz_scale_hcr	= hprofile->sctbl[level].sschcr;
			ptMrvReg->srsz_scale_vy		= hprofile->sctbl[level].sscvy;
			ptMrvReg->srsz_scale_vc		= hprofile->sctbl[level].sscvc;
			ptMrvReg->srsz_phase_hy		= hprofile->sctbl[level].sphhy;
			ptMrvReg->srsz_phase_hc		= hprofile->sctbl[level].sphhc;
			ptMrvReg->srsz_phase_vy		= hprofile->sctbl[level].sphvy;
			ptMrvReg->srsz_phase_vc		= hprofile->sctbl[level].sphvc;

			ptMrvReg->mrsz_ctrl			= hprofile->sctbl[level].mctrl & 0xff;
			ptMrvReg->mrsz_scale_hy		= hprofile->sctbl[level].mschy;
			ptMrvReg->mrsz_scale_hcb	= hprofile->sctbl[level].mschcb;
			ptMrvReg->mrsz_scale_hcr	= hprofile->sctbl[level].mschcr;
			ptMrvReg->mrsz_scale_vy		= hprofile->sctbl[level].mscvy;
			ptMrvReg->mrsz_scale_vc		= hprofile->sctbl[level].mscvc;
			ptMrvReg->mrsz_phase_hy		= hprofile->sctbl[level].mphhy;
			ptMrvReg->mrsz_phase_hc		= hprofile->sctbl[level].mphhc;
			ptMrvReg->mrsz_phase_vy		= hprofile->sctbl[level].mphvy;
			ptMrvReg->mrsz_phase_vc		= hprofile->sctbl[level].mphvc;

			REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, 1);
		}
			break;
		case DRVISPAPPLY_LATER:
		{
#if 1
			ptMrvReg->isp_out_h_offs	= hprofile->sctbl[level].offx;
			ptMrvReg->isp_out_v_offs	= hprofile->sctbl[level].offy;
			ptMrvReg->isp_out_h_size	= hprofile->sctbl[level].w;
			ptMrvReg->isp_out_v_size	= hprofile->sctbl[level].h;
			
			ptMrvReg->isp_is_h_offs	= 0;
			ptMrvReg->isp_is_v_offs	= 0;
			ptMrvReg->isp_is_h_size	= hprofile->sctbl[level].w;
			ptMrvReg->isp_is_v_size	= hprofile->sctbl[level].h;

#else

			ptMrvReg->isp_is_h_offs	= hprofile->sctbl[level].offx;
			ptMrvReg->isp_is_v_offs	= hprofile->sctbl[level].offy;
			ptMrvReg->isp_is_h_size	= hprofile->sctbl[level].w;
			ptMrvReg->isp_is_v_size	= hprofile->sctbl[level].h;
#endif
			ptMrvReg->srsz_ctrl			= hprofile->sctbl[level].sctrl & 0xff;
			ptMrvReg->srsz_scale_hy		= hprofile->sctbl[level].sschy;
			ptMrvReg->srsz_scale_hcb	= hprofile->sctbl[level].sschcb;
			ptMrvReg->srsz_scale_hcr	= hprofile->sctbl[level].sschcr;
			ptMrvReg->srsz_scale_vy		= hprofile->sctbl[level].sscvy;
			ptMrvReg->srsz_scale_vc		= hprofile->sctbl[level].sscvc;
			ptMrvReg->srsz_phase_hy		= hprofile->sctbl[level].sphhy;
			ptMrvReg->srsz_phase_hc		= hprofile->sctbl[level].sphhc;
			ptMrvReg->srsz_phase_vy		= hprofile->sctbl[level].sphvy;
			ptMrvReg->srsz_phase_vc		= hprofile->sctbl[level].sphvc;

			ptMrvReg->mrsz_ctrl			= hprofile->sctbl[level].mctrl & 0xff;
			ptMrvReg->mrsz_scale_hy		= hprofile->sctbl[level].mschy;
			ptMrvReg->mrsz_scale_hcb	= hprofile->sctbl[level].mschcb;
			ptMrvReg->mrsz_scale_hcr	= hprofile->sctbl[level].mschcr;
			ptMrvReg->mrsz_scale_vy		= hprofile->sctbl[level].mscvy;
			ptMrvReg->mrsz_scale_vc		= hprofile->sctbl[level].mscvc;
			ptMrvReg->mrsz_phase_hy		= hprofile->sctbl[level].mphhy;
			ptMrvReg->mrsz_phase_hc		= hprofile->sctbl[level].mphhc;
			ptMrvReg->mrsz_phase_vy		= hprofile->sctbl[level].mphvy;
			ptMrvReg->mrsz_phase_vc		= hprofile->sctbl[level].mphvc;
		}
			break;
		case DRVISPAPPLY_MODECHANGE:
			break;
		default:
			errno = 1; // invlaid apply param
			return errno;
	};

	return 0;	
}


int drvisp_zoom_apply(HIsp hisp, int reserved)
{
	int errno = 0;
	HProfile hprofile = hisp->hcurprofile;
	int level;
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;		
	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}
	level = hprofile->zoomlvl;
	if (level == -1)		// assert(level>=0);
	{
		errno = 1;
		return errno;
	}
	
	//if ( !(hprofile->sctbl[level].mctrl & 0xf0) )
	{
#if 0	
		ptMrvReg->isp_out_h_offs	= hprofile->sctbl[level].offx;
		ptMrvReg->isp_out_v_offs	= hprofile->sctbl[level].offy;
		ptMrvReg->isp_out_h_size	= hprofile->sctbl[level].w;
		ptMrvReg->isp_out_v_size	= hprofile->sctbl[level].h;
		
		ptMrvReg->isp_is_h_offs	= 0;
		ptMrvReg->isp_is_v_offs	= 0;
		ptMrvReg->isp_is_h_size	= hprofile->sctbl[level].w;
		ptMrvReg->isp_is_v_size	= hprofile->sctbl[level].h;
	
#else		
		ptMrvReg->isp_is_h_offs	= hprofile->sctbl[level].offx;
		ptMrvReg->isp_is_v_offs	= hprofile->sctbl[level].offy;
		ptMrvReg->isp_is_h_size	= hprofile->sctbl[level].w;
		ptMrvReg->isp_is_v_size	= hprofile->sctbl[level].h;
#endif
		ptMrvReg->srsz_ctrl			= hprofile->sctbl[level].sctrl & 0xff;
		ptMrvReg->srsz_scale_hy		= hprofile->sctbl[level].sschy;
		ptMrvReg->srsz_scale_hcb	= hprofile->sctbl[level].sschcb;
		ptMrvReg->srsz_scale_hcr	= hprofile->sctbl[level].sschcr;
		ptMrvReg->srsz_scale_vy		= hprofile->sctbl[level].sscvy;
		ptMrvReg->srsz_scale_vc		= hprofile->sctbl[level].sscvc;
		ptMrvReg->srsz_phase_hy		= hprofile->sctbl[level].sphhy;
		ptMrvReg->srsz_phase_hc		= hprofile->sctbl[level].sphhc;
		ptMrvReg->srsz_phase_vy		= hprofile->sctbl[level].sphvy;
		ptMrvReg->srsz_phase_vc		= hprofile->sctbl[level].sphvc;

		ptMrvReg->mrsz_ctrl			= hprofile->sctbl[level].mctrl & 0xff;
		ptMrvReg->mrsz_scale_hy		= hprofile->sctbl[level].mschy;
		ptMrvReg->mrsz_scale_hcb	= hprofile->sctbl[level].mschcb;
		ptMrvReg->mrsz_scale_hcr	= hprofile->sctbl[level].mschcr;
		ptMrvReg->mrsz_scale_vy		= hprofile->sctbl[level].mscvy;
		ptMrvReg->mrsz_scale_vc		= hprofile->sctbl[level].mscvc;
		ptMrvReg->mrsz_phase_hy		= hprofile->sctbl[level].mphhy;
		ptMrvReg->mrsz_phase_hc		= hprofile->sctbl[level].mphhc;
		ptMrvReg->mrsz_phase_vy		= hprofile->sctbl[level].mphvy;
		ptMrvReg->mrsz_phase_vc		= hprofile->sctbl[level].mphvc;
	if (reserved)
	{
		REG_SET_SLICE(ptMrvReg->srsz_ctrl, MRV_SRSZ_CFG_UPD, 1);
		REG_SET_SLICE(ptMrvReg->mrsz_ctrl, MRV_MRSZ_CFG_UPD, 1);
		REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_CFG_UPD, 1);
	}
	}

	return 0;
}

static int drvispprofile_mi_apply( HProfile hprofile)
{

	int errno = 0;
	HIsp hisp = hprofile->hisp;
	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}
	{	// rsz, address, config, control
		tsMrvDatapathDesc Source;
		tsMrvDatapathDesc Main;
		tsMrvDatapathDesc Self;
		tsMrvScale MScale;
		tsMrvScale SScale;
		tsMrvMiPathConf MPConf;
		tsMrvMiPathConf SPConf;
		tsMrvMiDmaConf DMAConf;
		tsMrvMiCtrl MCtrl;
		//tsMrvMiCtrl SCtrl;

		//////////////////////////////////////////////
		// set VI_DPCL
		{
			volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;		
			REG_SET_SLICE(ptMrvReg->vi_dpcl, MRV_VI_CHAN_MODE, (hprofile->mp.size1 != 0)*MRV_VI_CHAN_MODE_MP | (hprofile->sp.size1 != 0)*MRV_VI_CHAN_MODE_SP );
			REG_SET_SLICE(ptMrvReg->vi_dpcl, MRV_VI_MP_MUX, ((hprofile->mp.dtype & DRVISPDTYPE_MAX) == DRVISPDTYPE_JPEG) ? MRV_VI_MP_MUX_JPEG : MRV_VI_MP_MUX_MP );
			
		}
		
		memset( &MCtrl, 0, sizeof(MCtrl) );
		memset( &DMAConf, 0, sizeof(DMAConf) );    
		
		Source.usOutW = hprofile->is.window.width;
		Source.usOutH = hprofile->is.window.height;
		Source.ulFlags= 0 ;	// 사용안함 
		
		Main.usOutW = hprofile->mp.dst.width;
		Main.usOutH = hprofile->mp.dst.height;
		Main.ulFlags= convDType (hprofile->mp.dtype) | MRV_DPD_UPSCALE_SMOOTH_LIN | MRV_DPD_BURST_16 | ( (hprofile->mp.size1) ? MRV_DPD_ENABLE : MRV_DPD_DISABLE );

		Self.usOutW = hprofile->sp.dst.width;
		Self.usOutH = hprofile->sp.dst.height;
		Self.ulFlags= convDType (hprofile->sp.dtype) |MRV_DPD_UPSCALE_SMOOTH_LIN | MRV_DPD_BURST_16 | ( (hprofile->sp.size1) ? MRV_DPD_ENABLE : MRV_DPD_DISABLE );

		
		MrvSls_CalcScaleFactors( &Source, &Main, &MScale, MARVIN_FEATURE_MSCALE_FACTORCALC );
		MrvResSetMainResize( &MScale, eMrvCfgUpdateImmediate, (tsMrvRszLut *) MrvSls_GetRszLut(Main.ulFlags) );
		MrvSls_CalcScaleFactors( &Source, &Self, &SScale, MARVIN_FEATURE_SSCALE_FACTORCALC );
		MrvResSetSelfResize( &SScale, eMrvCfgUpdateImmediate, (tsMrvRszLut *) MrvSls_GetRszLut(Self.ulFlags) );

		MiscSetupExtMemory(hprofile->mp.dst.width, hprofile->mp.dst.height, conv2DType (hprofile->mp.dtype), &MPConf, (unsigned) hprofile->mp.addr1);
		//MiscSetupExtMemory(hprofile->mp.dst.width, hprofile->mp.dst.height, eBufType_Yuv422, &MPConf, (unsigned) hprofile->mp.addr1);
		MPConf.tCbBuffer.pucBuffer = (UINT8 *) hprofile->mp.addr2;
		MPConf.tCrBuffer.pucBuffer = (UINT8 *) hprofile->mp.addr3;
		//MrvMif_SetMainBuffer( &MPConf, eMrvCfgUpdateFrameSync);
		MiscSetupExtMemory(hprofile->sp.dst.width, hprofile->sp.dst.height, conv2DType (hprofile->sp.dtype), &SPConf, (unsigned) hprofile->sp.addr1);
		//MiscSetupExtMemory(hprofile->sp.dst.width, hprofile->sp.dst.height, eBufType_Yuv422, &SPConf, (unsigned) hprofile->sp.addr1);
		SPConf.tCbBuffer.pucBuffer = (UINT8 *) hprofile->sp.addr2;
		SPConf.tCrBuffer.pucBuffer = (UINT8 *) hprofile->sp.addr3;
		//MrvMif_SetSelfBuffer( &SPConf, eMrvCfgUpdateFrameSync);

		MrvSls_CalcMainPathSettings(&Source, &Main , &MScale, &MCtrl);
		MrvSls_CalcSelfPathSettings(&Source, &Self , &SScale, &MCtrl);

		MrvMif_SetConfiguration(&MCtrl, &MPConf, &SPConf, &DMAConf ); 

		{
			volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;			
			ptMrvReg->mi_mp_y_size_init		= hprofile->mp.size1;
			ptMrvReg->mi_mp_cb_size_init	= hprofile->mp.size2;
			ptMrvReg->mi_mp_cr_size_init	= hprofile->mp.size3;

			ptMrvReg->mi_mp_y_offs_cnt_init = 0;
			ptMrvReg->mi_mp_cb_offs_cnt_init = 0;
			ptMrvReg->mi_mp_cr_offs_cnt_init = 0;

			ptMrvReg->mi_sp_y_size_init		= hprofile->sp.size1;
			ptMrvReg->mi_sp_cb_size_init	= hprofile->sp.size2;
			ptMrvReg->mi_sp_cr_size_init		= hprofile->sp.size3;

			ptMrvReg->mi_sp_y_offs_cnt_init = 0;
			ptMrvReg->mi_sp_cb_offs_cnt_init = 0;
			ptMrvReg->mi_sp_cr_offs_cnt_init = 0;
 
			REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_BURST_LEN_LUM, MRV_MI_BURST_LEN_LUM_16);
			REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_BURST_LEN_CHROM, MRV_MI_BURST_LEN_CHROM_16);

			if (hprofile->mp.size1 && (hprofile->mp.dtype == DRVISPDTYPE_RAW12) )
			{
				REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_RAW);
				//REG_SET_SLICE(ptMrvReg->isp_acq_prop, MRV_ISP_INPUT_SELECTION, MRV_ISP_INPUT_SELECTION_12EXT);			
				//REG_SET_SLICE(ptMrvReg->isp_acq_prop, MRV_ISP_BAYER_PAT, MRV_ISP_BAYER_PAT_RG);			
				//REG_SET_SLICE(ptMrvReg->isp_acq_prop, MRV_ISP_VSYNC_POL, 0);			
				//REG_SET_SLICE(ptMrvReg->isp_acq_prop, MRV_ISP_HSYNC_POL, 0);			
				//REG_SET_SLICE(ptMrvReg->isp_acq_prop, MRV_ISP_SAMPLE_EDGE, 1);		
				//REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_MP_WRITE_FORMAT, MRV_MI_MP_WRITE_FORMAT_RAW_12);
			}
			else if (hprofile->mp.size1 && (hprofile->mp.dtype == DRVISPDTYPE_RAW8) )
				{
				REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_RAW);
				//REG_SET_SLICE(ptMrvReg->isp_acq_prop, MRV_ISP_INPUT_SELECTION, MRV_ISP_INPUT_SELECTION_12EXT);			
				//REG_SET_SLICE(ptMrvReg->isp_acq_prop, MRV_ISP_BAYER_PAT, MRV_ISP_BAYER_PAT_RG);			
				//REG_SET_SLICE(ptMrvReg->isp_acq_prop, MRV_ISP_VSYNC_POL, 0);			
				//REG_SET_SLICE(ptMrvReg->isp_acq_prop, MRV_ISP_HSYNC_POL, 0);			
				//REG_SET_SLICE(ptMrvReg->isp_acq_prop, MRV_ISP_SAMPLE_EDGE, 1);		
				//REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_MP_WRITE_FORMAT, MRV_MI_MP_WRITE_FORMAT_RAW_8);
				}
		 	else
	 		{
	 			if (hisp->properties & DRVISPFLAG_PARALLELYUV_MODE )
	 			{
					if ( (hprofile->acq.prop & DRVISPACQ_SYNC_MASK)&& (hprofile->acq.prop & DRVISPACQ_SYNC_BT656))
					{
						REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_656);
					}
					else
					{
					REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_601);
				}
				}
				else
				{
					REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_RGB);
				}
	 		}
			ptMrvReg->mi_init		= 0x10;
		}
	}


	return 0 ;
}

int drvispprofile_setie(HProfile hprofile, int mode, int detail1, int detail2, int apply)
{
	int errno = 0;
	HIsp hisp = hprofile->hisp;
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}
	hprofile->iemode = mode;
	
	switch (apply & 0xf)
	{
		case DRVISPAPPLY_IMMEDIATELY:
			errno = drvispprofile_ie_apply(hprofile,0);
			REG_SET_SLICE(ptMrvReg->img_eff_ctrl, MRV_IMGEFF_CFG_UPD, 1);
			break;
		case DRVISPAPPLY_ENDOFFRAME:
			errno = drvispprofile_ie_apply(hprofile,0);
			REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, 1);
			break;
		case DRVISPAPPLY_LATER:
			errno = drvispprofile_ie_apply(hprofile,0);
			break;
		case DRVISPAPPLY_MODECHANGE:
			break;
		default:
			errno = 1; // invlaid apply param
	};

	return errno;
}

static int drvispprofile_ie_apply(HProfile hprofile, int reserved)
{
	int errno = 0;
	HIsp hisp = hprofile->hisp;
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}

	drvisp_loadpreset_apply(hisp, hisp->preset[DRVISPPRESET_EFT0+hprofile->iemode]);
	if (reserved)
	{
		REG_SET_SLICE(ptMrvReg->img_eff_ctrl, MRV_IMGEFF_CFG_UPD, 1);
	}
	return 0;
}

static int drvispprofile_si_apply(HProfile hprofile)
{
	int errno = 0;
	HIsp hisp = hprofile->hisp;
	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}


	hisp->hprevprofile = hisp->hcurprofile;
	hisp->hcurprofile = hisp->hnextprofile;
	hisp->hnextprofile = 0;
	drvisp_loadpreset_apply(hprofile->hisp, hprofile->preset);

	// set main rsz & path
	// set self rsz & path
	// set read path
	
	return 0;
}

//
int drvispprofile_setwindow(HProfile hprofile, DRVISPRect* window , int apply)
{
	int errno = 0;
	HIsp hisp = hprofile->hisp;
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}

	hprofile->is.maxdx = 0;
	hprofile->is.maxdy = 0;
	hprofile->is.window.offx 	= window->offx;
	hprofile->is.window.offy 	= window->offy;
	hprofile->is.window.width 	= window->width;
	hprofile->is.window.height 	= window->height;

	switch (apply & 0xf)
	{
		case DRVISPAPPLY_IMMEDIATELY:
			errno = drvispprofile_window_apply(hprofile);
			{
				tsMrvDatapathDesc Source;
				tsMrvDatapathDesc Main;
				tsMrvDatapathDesc Self;
				tsMrvScale MScale;
				tsMrvScale SScale;
				
				Source.usOutW = hprofile->is.window.width;
				Source.usOutH = hprofile->is.window.height;
				Source.ulFlags= 0 ;	// 사용안함 
				
				Main.usOutW = hprofile->mp.dst.width;
				Main.usOutH = hprofile->mp.dst.height;
				Main.ulFlags= convDType (hprofile->mp.dtype) | MRV_DPD_UPSCALE_SMOOTH_LIN | MRV_DPD_BURST_16 | ( (hprofile->mp.size1) ? MRV_DPD_ENABLE : MRV_DPD_DISABLE );

				Self.usOutW = hprofile->sp.dst.width;
				Self.usOutH = hprofile->sp.dst.height;
				Self.ulFlags= convDType (hprofile->sp.dtype) |MRV_DPD_UPSCALE_SMOOTH_LIN | MRV_DPD_BURST_16 | ( (hprofile->sp.size1) ? MRV_DPD_ENABLE : MRV_DPD_DISABLE );
				
				MrvSls_CalcScaleFactors( &Source, &Main, &MScale, MARVIN_FEATURE_MSCALE_FACTORCALC );
				MrvResSetMainResize( &MScale, eMrvCfgUpdateLater, (tsMrvRszLut *) MrvSls_GetRszLut(Main.ulFlags) );
				MrvSls_CalcScaleFactors( &Source, &Self, &SScale, MARVIN_FEATURE_SSCALE_FACTORCALC );
				MrvResSetSelfResize( &SScale, eMrvCfgUpdateLater, (tsMrvRszLut *) MrvSls_GetRszLut(Self.ulFlags) );
			}
			REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_CFG_UPD, 1);
			break;
		case DRVISPAPPLY_ENDOFFRAME:
			errno = drvispprofile_window_apply(hprofile);
			{
				tsMrvDatapathDesc Source;
				tsMrvDatapathDesc Main;
				tsMrvDatapathDesc Self;
				tsMrvScale MScale;
				tsMrvScale SScale;
				
				Source.usOutW = hprofile->is.window.width;
				Source.usOutH = hprofile->is.window.height;
				Source.ulFlags= 0 ;	// 사용안함 
				
				Main.usOutW = hprofile->mp.dst.width;
				Main.usOutH = hprofile->mp.dst.height;
				Main.ulFlags= convDType (hprofile->mp.dtype) | MRV_DPD_UPSCALE_SMOOTH_LIN | MRV_DPD_BURST_16 | ( (hprofile->mp.size1) ? MRV_DPD_ENABLE : MRV_DPD_DISABLE );

				Self.usOutW = hprofile->sp.dst.width;
				Self.usOutH = hprofile->sp.dst.height;
				Self.ulFlags= convDType (hprofile->sp.dtype) |MRV_DPD_UPSCALE_SMOOTH_LIN | MRV_DPD_BURST_16 | ( (hprofile->sp.size1) ? MRV_DPD_ENABLE : MRV_DPD_DISABLE );
				
				MrvSls_CalcScaleFactors( &Source, &Main, &MScale, MARVIN_FEATURE_MSCALE_FACTORCALC );
				MrvResSetMainResize( &MScale, eMrvCfgUpdateLater, (tsMrvRszLut *) MrvSls_GetRszLut(Main.ulFlags) );
				MrvSls_CalcScaleFactors( &Source, &Self, &SScale, MARVIN_FEATURE_SSCALE_FACTORCALC );
				MrvResSetSelfResize( &SScale, eMrvCfgUpdateLater, (tsMrvRszLut *) MrvSls_GetRszLut(Self.ulFlags) );
			}
			REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, 1);
			break;
		case DRVISPAPPLY_MODECHANGE:
			break;
		default:
			errno = 1; // invlaid apply param
	};
	
	return 0;
}

int drvispprofile_setmpath (HProfile hprofile, int num, int skip, void *addr, int size, DRVISPRectSize* rectsize, int dtype, int reserved)
{
	int errno = 0;
	HIsp hisp = hprofile->hisp;
	unsigned w,h;
	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}

	hprofile->mp.addr1 = (unsigned char*)addr;
	hprofile->mp.size1  = size;

	hprofile->mp.dst.width = w = rectsize->width;
	hprofile->mp.dst.height = h = rectsize->height;
	hprofile->mp.dtype = dtype;
	hprofile->mp.num = num;
	hprofile->mp.skip = skip;

	switch (dtype&DRVISPDTYPE_MAX)
	{
		case DRVISPDTYPE_RAW8:			// 1byte
		case DRVISPDTYPE_RAW12:			// 2byte
		case DRVISPDTYPE_JPEG:			// 8byte??? (max : ???)
		case DRVISPDTYPE_DP:			// ??byte (max : ???)
			hprofile->mp.addr2 = (unsigned char*)addr;
			hprofile->mp.addr3 = (unsigned char*)addr;
			hprofile->mp.size2  = 0;
			hprofile->mp.size3  = 0;
			break;

		case DRVISPDTYPE_YCbCr444:		// planar 3byte(1:1:1), semi 3byte(1:2)
			switch (dtype&DRVISPDTYPE_YUVTYPE_MAX)
			{
				case DRVISPDTYPE_PLANAR:
					hprofile->mp.addr2 = (unsigned char*)addr + w*h;
					hprofile->mp.addr3 = (unsigned char*)addr + w*h*2;
					hprofile->mp.size2  = size;
					hprofile->mp.size3  = size;
					break;
				case DRVISPDTYPE_SEMIPLANAR:
					hprofile->mp.addr2 = (unsigned char*)addr + w*h;
					hprofile->mp.addr3 = (unsigned char*)addr;
					hprofile->mp.size2  = size;
					hprofile->mp.size3  = 0;
					break;
				case DRVISPDTYPE_INTERLEAVED:
					break;
				default:
					break;					
			}
			break;
		case DRVISPDTYPE_YCbCr422:		// planar 2byte(2:1:1), semi 2byte(1:1), seq 2byte
			switch (dtype&DRVISPDTYPE_YUVTYPE_MAX)
			{
				case DRVISPDTYPE_PLANAR:
					hprofile->mp.addr2 = (unsigned char*)addr + w*h;
					hprofile->mp.addr3 = (unsigned char*)addr + w*h + w*h/2;
					hprofile->mp.size2  = size;
					hprofile->mp.size3  = size;
					break;
				case DRVISPDTYPE_SEMIPLANAR:
					hprofile->mp.addr2 = (unsigned char*)addr + w*h;
					hprofile->mp.addr3 = (unsigned char*)addr;
					hprofile->mp.size2  = size;
					hprofile->mp.size3  = size;
					break;
				case DRVISPDTYPE_INTERLEAVED:
					hprofile->mp.addr2 = (unsigned char*)addr;
					hprofile->mp.addr3 = (unsigned char*)addr;
					hprofile->mp.size2  = 0;
					hprofile->mp.size3  = 0;
					break;
				default:
					break;					
			}
			break;
		case DRVISPDTYPE_YCbCr420:		// planar 1.5byte(4:1:1), semi 1.5bye(2:1)
			switch (dtype&DRVISPDTYPE_YUVTYPE_MAX)
			{
				case DRVISPDTYPE_PLANAR:
					hprofile->mp.addr2 = (unsigned char*)addr + w*h;
					hprofile->mp.addr3 = (unsigned char*)addr + w*h + w*h/4;
					hprofile->mp.size2  = size;
					hprofile->mp.size3  = size;
					break;
				case DRVISPDTYPE_SEMIPLANAR:
					hprofile->mp.addr2 = (unsigned char*)addr + w*h/2;
					hprofile->mp.addr3 = (unsigned char*)addr;
					hprofile->mp.size2  = size;
					hprofile->mp.size3  = size;
					break;
				case DRVISPDTYPE_INTERLEAVED:
					break;
				default:		
					break;			
			}
		case DRVISPDTYPE_YCbCr400:		// ???
			switch (dtype&DRVISPDTYPE_YUVTYPE_MAX)
			{
				case DRVISPDTYPE_PLANAR:
					hprofile->mp.addr2 = (unsigned char*)addr + w*h;
					hprofile->mp.addr3 = (unsigned char*)addr + w*h + w*h/2;
					break;
				case DRVISPDTYPE_SEMIPLANAR:
					hprofile->mp.addr2 = (unsigned char*)addr + w*h;
					hprofile->mp.addr3 = (unsigned char*)addr;
					break;
				case DRVISPDTYPE_INTERLEAVED:
					break;
				default:	
					break;				
			}
		default :
			break;
			
	};
	
	
	return 0;
}
int drvispprofile_setmpath_number (HProfile hprofile, int num, int reserved)
{
	int errno = 0;
	HIsp hisp = hprofile->hisp;

	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}
	
	hprofile->mp.num = num;
	
	return 0;
}
int drvispprofile_setmpath_size (HProfile hprofile, DRVISPRectSize* rectsize, int reserved)
{
	int errno = 0;
	HIsp hisp = hprofile->hisp;
	unsigned w,h;
	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}
	
	hprofile->mp.dst.width = w = rectsize->width;
	hprofile->mp.dst.height = h = rectsize->height;
	return 0;
}

// type을 설정 후, 반드시 set address를 해야 함 
int drvispprofile_setmpath_type (HProfile hprofile, int dtype, int reserved)
{
	int errno = 0;
	HIsp hisp = hprofile->hisp;
	unsigned w,h;
	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}

	hprofile->mp.dtype = dtype;
	return 0;
}
int drvispprofile_setspath (HProfile hprofile, int num, int skip, void *addr, int size, DRVISPRegion* region, int dtype, int orientation, int reserved)
{
	int errno = 0;
	HIsp hisp = hprofile->hisp;
	unsigned w,h;
	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}

	hprofile->sp.addr1 = (unsigned char*)addr;
	hprofile->sp.addr2 = (unsigned char*)addr;
	hprofile->sp.addr3 = (unsigned char*)addr;
	hprofile->sp.size1  = size;
	hprofile->sp.size2  = size;
	hprofile->sp.size3  = size;

	hprofile->sp.dst.offx = region->offx  ;
	hprofile->sp.dst.offy = region->offy  ;
	hprofile->sp.dst.width  = w = region->width;
	hprofile->sp.dst.height = h = region->height;
	hprofile->sp.dst.xpitch = region->xpitch ;
	hprofile->sp.dst.ypitch = region->ypitch  ;

	hprofile->sp.dtype = dtype;
	hprofile->sp.num = num;
	hprofile->sp.skip = skip;

	switch (dtype&DRVISPDTYPE_MAX)
	{
		case DRVISPDTYPE_RAW8:			// 1byte
		case DRVISPDTYPE_RAW12:			// 2byte
		case DRVISPDTYPE_JPEG:			// 8byte??? (max : ???)
		case DRVISPDTYPE_DP:			// ??byte (max : ???)
			hprofile->sp.addr2 = (unsigned char*)addr;
			hprofile->sp.addr3 = (unsigned char*)addr;
			hprofile->sp.size2  = 0;
			hprofile->sp.size3  = 0;
			break;

		case DRVISPDTYPE_YCbCr444:		// planar 3byte(1:1:1), semi 3byte(1:2)
			switch (dtype&DRVISPDTYPE_YUVTYPE_MAX)
			{
				case DRVISPDTYPE_PLANAR:
					hprofile->sp.addr2 = (unsigned char*)addr + w*h;
					hprofile->sp.addr3 = (unsigned char*)addr + w*h*2;
					hprofile->sp.size2  = size;
					hprofile->sp.size3  = size;
					break;
				case DRVISPDTYPE_SEMIPLANAR:
					hprofile->sp.addr2 = (unsigned char*)addr + w*h;
					hprofile->sp.addr3 = (unsigned char*)addr;
					hprofile->sp.size2  = size;
					hprofile->sp.size3  = 0;
					break;
				case DRVISPDTYPE_INTERLEAVED:
					break;
				default:	
					break;				
			}
			break;
		case DRVISPDTYPE_YCbCr422:		// planar 2byte(2:1:1), semi 2byte(1:1), seq 2byte
			switch (dtype&DRVISPDTYPE_YUVTYPE_MAX)
			{
				case DRVISPDTYPE_PLANAR:
					hprofile->sp.addr2 = (unsigned char*)addr + w*h;
					hprofile->sp.addr3 = (unsigned char*)addr + w*h + w*h/2;
					hprofile->sp.size2  = size;
					hprofile->sp.size3  = size;
					break;
				case DRVISPDTYPE_SEMIPLANAR:
					hprofile->sp.addr2 = (unsigned char*)addr + w*h;
					hprofile->sp.addr3 = (unsigned char*)addr;
					hprofile->sp.size2  = size;
					hprofile->sp.size3  = size;
					break;
				case DRVISPDTYPE_INTERLEAVED:
					hprofile->sp.addr2 = (unsigned char*)addr;
					hprofile->sp.addr3 = (unsigned char*)addr;
					hprofile->sp.size2  = 0;
					hprofile->sp.size3  = 0;
					break;
				default:
					break;					
			}
			break;
		case DRVISPDTYPE_YCbCr420:		// planar 1.5byte(4:1:1), semi 1.5bye(2:1)
			switch (dtype&DRVISPDTYPE_YUVTYPE_MAX)
			{
				case DRVISPDTYPE_PLANAR:
					hprofile->sp.addr2 = (unsigned char*)addr + w*h;
					hprofile->sp.addr3 = (unsigned char*)addr + w*h + w*h/4;
					hprofile->sp.size2  = size;
					hprofile->sp.size3  = size;
					break;
				case DRVISPDTYPE_SEMIPLANAR:
					hprofile->sp.addr2 = (unsigned char*)addr + w*h/2;
					hprofile->sp.addr3 = (unsigned char*)addr;
					hprofile->sp.size2  = size;
					hprofile->sp.size3  = size;
					break;
				case DRVISPDTYPE_INTERLEAVED:
					break;
				default:		
					break;			
			}
		case DRVISPDTYPE_YCbCr400:		// ???
			switch (dtype&DRVISPDTYPE_YUVTYPE_MAX)
			{
				case DRVISPDTYPE_PLANAR:
					hprofile->sp.addr2 = (unsigned char*)addr + w*h;
					hprofile->sp.addr3 = (unsigned char*)addr + w*h + w*h/2;
					break;
				case DRVISPDTYPE_SEMIPLANAR:
					hprofile->sp.addr2 = (unsigned char*)addr + w*h;
					hprofile->sp.addr3 = (unsigned char*)addr;
					break;
				case DRVISPDTYPE_INTERLEAVED:
					break;
				default:	
					break;				
			}
		default :
			break;
			
	};
	return 0;
}
int drvispprofile_setspath_number (HProfile hprofile, int num, int reserved)
{
	int errno = 0;
	HIsp hisp = hprofile->hisp;

	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}
	
	hprofile->sp.num = num;
	
	return 0;
}
int drvispprofile_setspath_region (HProfile hprofile, DRVISPRegion* region, int reserved)
{
	int errno = 0;
	HIsp hisp = hprofile->hisp;
	unsigned w,h;
	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}

	hprofile->sp.dst.offx = region->offx;
	hprofile->sp.dst.offy = region->offy;
	hprofile->sp.dst.width = w = region->width;
	hprofile->sp.dst.height = h = region->height;
	hprofile->sp.dst.xpitch = region->xpitch;
	hprofile->sp.dst.ypitch = region->ypitch;
	
	return 0;
}
int drvispprofile_setspath_type (HProfile hprofile, int dtype, int reserved)
{
	int errno = 0;
	HIsp hisp = hprofile->hisp;
	unsigned w,h;
	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}
	hprofile->sp.dtype = dtype;

	return 0;
}

int drvispprofile_setmfb(HProfile hprofile, void *addr1, void *addr2, void *addr3, int size1, int size2, int size3, int uid, int apply )
{
	int errno = 0;
	HIsp hisp = hprofile->hisp;
	unsigned w,h;
	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}

	hprofile->mp.addr1 = (unsigned char*)addr1;
	hprofile->mp.addr2 = (unsigned char*)addr2;
	hprofile->mp.addr3 = (unsigned char*)addr3;
	hprofile->mp.size1  = size1;
	hprofile->mp.size2  = size2;
	hprofile->mp.size3  = size3;

	switch (apply & DRVISPAPPLY_UPDATE_MAX)
	{
		case DRVISPAPPLY_IMMEDIATELY:
			if (hisp->status == DRVISPST_STREAMING && hisp->hcurprofile == hprofile)
			{
				volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
				ptMrvReg->mi_mp_y_base_ad_init	= (unsigned int) addr1;  
				ptMrvReg->mi_mp_y_size_init		= size1;
				ptMrvReg->mi_mp_y_offs_cnt_init = 0;
				ptMrvReg->mi_mp_cb_base_ad_init = (unsigned int) addr2;
				ptMrvReg->mi_mp_cb_size_init	= size1;
				ptMrvReg->mi_mp_cb_offs_cnt_init = 0;
				ptMrvReg->mi_mp_cr_base_ad_init = (unsigned int) addr3;
				ptMrvReg->mi_mp_cr_size_init	= size3;
				ptMrvReg->mi_mp_cr_offs_cnt_init = 0;

				REG_SET_SLICE( ptMrvReg->mi_init, MRV_MI_MI_CFG_UPD, 1);
			}
			break;
		case DRVISPAPPLY_ENDOFFRAME:
			if (hisp->status == DRVISPST_STREAMING && hisp->hcurprofile == hprofile)
			{
				volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
				ptMrvReg->mi_mp_y_base_ad_init	= (unsigned int) addr1;  
				ptMrvReg->mi_mp_y_size_init		= size1;
				ptMrvReg->mi_mp_y_offs_cnt_init = 0;
				ptMrvReg->mi_mp_cb_base_ad_init = (unsigned int) addr2;
				ptMrvReg->mi_mp_cb_size_init	= size1;
				ptMrvReg->mi_mp_cb_offs_cnt_init = 0;
				ptMrvReg->mi_mp_cr_base_ad_init = (unsigned int) addr3;
				ptMrvReg->mi_mp_cr_size_init	= size3;
				ptMrvReg->mi_mp_cr_offs_cnt_init = 0;

				REG_SET_SLICE( ptMrvReg->isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, 1);
			}
			break;
		case DRVISPAPPLY_MODECHANGE:
			break;
		default:
			errno = 1;	// error: invalid apply value 
	}


	return 0;
}	

// 
int drvispprofile_setmfb_addr(HProfile hprofile, void *addr1, void *addr2, void *addr3, int apply)
{
	int errno = 0;
	HIsp hisp = hprofile->hisp;
	unsigned w,h;
	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}

	hprofile->mp.addr1 = (unsigned char*)addr1;
	hprofile->mp.addr2 = (unsigned char*)addr2;
	hprofile->mp.addr3 = (unsigned char*)addr3;
	
	switch (apply & DRVISPAPPLY_UPDATE_MAX)
	{
		case DRVISPAPPLY_IMMEDIATELY:
			if (hisp->status == DRVISPST_STREAMING && hisp->hcurprofile == hprofile)
			{
				volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
				ptMrvReg->mi_mp_y_base_ad_init	= (unsigned int) addr1;  
				ptMrvReg->mi_mp_y_offs_cnt_init = 0;
				ptMrvReg->mi_mp_cb_base_ad_init = (unsigned int) addr2;
				ptMrvReg->mi_mp_cb_offs_cnt_init = 0;
				ptMrvReg->mi_mp_cr_base_ad_init  = (unsigned int) addr3;
				ptMrvReg->mi_mp_cr_offs_cnt_init = 0;

				REG_SET_SLICE( ptMrvReg->mi_init, MRV_MI_MI_CFG_UPD, 1);
			}
			break;
		case DRVISPAPPLY_ENDOFFRAME:
			if (hisp->status == DRVISPST_STREAMING && hisp->hcurprofile == hprofile)
			{
				volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
				ptMrvReg->mi_mp_y_base_ad_init	= (unsigned int) addr1;  
				ptMrvReg->mi_mp_y_offs_cnt_init = 0;
				ptMrvReg->mi_mp_cb_base_ad_init = (unsigned int) addr2;
				ptMrvReg->mi_mp_cb_offs_cnt_init = 0;
				ptMrvReg->mi_mp_cr_base_ad_init  = (unsigned int) addr3;
				ptMrvReg->mi_mp_cr_offs_cnt_init = 0;

				REG_SET_SLICE( ptMrvReg->isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, 1);
			}
			break;
		case DRVISPAPPLY_MODECHANGE:
			break;
		default:
			errno = 1;	// error: invalid apply value 
	}

	return 0;
}

int drvispprofile_setmfb_size(HProfile hprofile, unsigned size1, unsigned size2, unsigned size3, int reserved)
{
	int errno = 0;
	HIsp hisp = hprofile->hisp;
	unsigned w,h;
	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}

	hprofile->mp.size1  = size1;
	hprofile->mp.size2  = size2;
	hprofile->mp.size3  = size3;
	return 0;
}

int drvispprofile_setsfb(HProfile hprofile, void *addr1, void *addr2, void *addr3, int size1, int size2, int size3, int uid, int apply )
{
	int errno = 0;
	HIsp hisp = hprofile->hisp;
	unsigned w,h;
	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}

	hprofile->sp.addr1 = (unsigned char*)addr1;
	hprofile->sp.addr2 = (unsigned char*)addr2;
	hprofile->sp.addr3 = (unsigned char*)addr3;
	hprofile->sp.size1  = size1;
	hprofile->sp.size2  = size2;
	hprofile->sp.size3  = size3;

	switch (apply & DRVISPAPPLY_UPDATE_MAX)
	{
		case DRVISPAPPLY_IMMEDIATELY:
			if (hisp->status == DRVISPST_STREAMING && hisp->hcurprofile == hprofile)
			{
				volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
				ptMrvReg->mi_sp_y_base_ad_init	= (unsigned int) addr1;
				ptMrvReg->mi_sp_y_size_init		= size1;
				ptMrvReg->mi_sp_y_offs_cnt_init = 0;
				ptMrvReg->mi_sp_cb_base_ad_init = (unsigned int) addr2;
				ptMrvReg->mi_sp_cb_size_init	= size2;
				ptMrvReg->mi_sp_cb_offs_cnt_init = 0;
				ptMrvReg->mi_sp_cr_base_ad_init  = (unsigned int) addr3;
				ptMrvReg->mi_sp_cr_size_init	= size3;
				ptMrvReg->mi_sp_cr_offs_cnt_init = 0;

				REG_SET_SLICE( ptMrvReg->mi_init, MRV_MI_MI_CFG_UPD, 1);
			}
			break;
		case DRVISPAPPLY_ENDOFFRAME:
			if (hisp->status == DRVISPST_STREAMING && hisp->hcurprofile == hprofile)
			{
				volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
				ptMrvReg->mi_sp_y_base_ad_init	= (unsigned int) addr1;
				ptMrvReg->mi_sp_y_size_init		= size1;
				ptMrvReg->mi_sp_y_offs_cnt_init = 0;
				ptMrvReg->mi_sp_cb_base_ad_init = (unsigned int) addr2;
				ptMrvReg->mi_sp_cb_size_init	= size2;
				ptMrvReg->mi_sp_cb_offs_cnt_init = 0;
				ptMrvReg->mi_sp_cr_base_ad_init  = (unsigned int) addr3;
				ptMrvReg->mi_sp_cr_size_init	= size3;
				ptMrvReg->mi_sp_cr_offs_cnt_init = 0;

				REG_SET_SLICE( ptMrvReg->isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, 1);
			}
			break;
		case DRVISPAPPLY_MODECHANGE:
			break;
		default:
			errno = 1;	// error: invalid apply value 
	}

	return 0;
}	

int drvispprofile_setsfb_addr(HProfile hprofile, void *addr1, void *addr2, void *addr3, int apply)
{
	int errno = 0;
	HIsp hisp = hprofile->hisp;
	unsigned w,h;
	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}

	hprofile->sp.addr1 = (unsigned char*)addr1;
	hprofile->sp.addr2 = (unsigned char*)addr2;
	hprofile->sp.addr3 = (unsigned char*)addr3;

	switch (apply & DRVISPAPPLY_UPDATE_MAX)
	{
		case DRVISPAPPLY_IMMEDIATELY:
			if (hisp->status == DRVISPST_STREAMING && hisp->hcurprofile == hprofile)
			{
				volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
				ptMrvReg->mi_sp_y_base_ad_init	= (unsigned int) addr1;  
				ptMrvReg->mi_sp_y_offs_cnt_init = 0;
				ptMrvReg->mi_sp_cb_base_ad_init = (unsigned int) addr2;
				ptMrvReg->mi_sp_cb_offs_cnt_init = 0;
				ptMrvReg->mi_sp_cr_base_ad_init  = (unsigned int) addr3;
				ptMrvReg->mi_sp_cr_offs_cnt_init = 0;

				REG_SET_SLICE( ptMrvReg->mi_init, MRV_MI_MI_CFG_UPD, 1);
			}
			break;
		case DRVISPAPPLY_ENDOFFRAME:
			if (hisp->status == DRVISPST_STREAMING && hisp->hcurprofile == hprofile)
			{
				volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
				ptMrvReg->mi_sp_y_base_ad_init	= (unsigned int) addr1;  
				ptMrvReg->mi_sp_y_offs_cnt_init = 0;
				ptMrvReg->mi_sp_cb_base_ad_init = (unsigned int) addr2;
				ptMrvReg->mi_sp_cb_offs_cnt_init = 0;
				ptMrvReg->mi_sp_cr_base_ad_init  = (unsigned int) addr3;
				ptMrvReg->mi_sp_cr_offs_cnt_init = 0;

				REG_SET_SLICE( ptMrvReg->isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, 1);
			}
			break;
		case DRVISPAPPLY_MODECHANGE:
			break;
		default:
			errno = 1;	// error: invalid apply value 
	}
	
	return errno;
}

int drvispprofile_setsfb_size(HProfile hprofile, unsigned size1, unsigned size2, unsigned size3, int reserved)
{
	int errno = 0;
	HIsp hisp = hprofile->hisp;
	unsigned w,h;
	if (!hprofile)	//invalid profile handler
	{
		errno = 1;
		return errno;
	}

	hprofile->sp.size1  = size1;
	hprofile->sp.size2  = size2;
	hprofile->sp.size3  = size3;
	return 0;
}


int drvispprofile_getmfb(HProfile hprofile, void **addr1, void **addr2, void **addr3, int *size1, int *size2, int *size3, int *hfb )
{
	return 0;
}
int drvispprofile_getsfb(HProfile hprofile, void **addr1, void **addr2, void **addr3, int *size1, int *size2, int *size3, int *hfb )
{
	return 0;
}
int drvispprofile_releasemfb(HProfile hprofile, int hfb)
{
	return 0;
}
int drvispprofile_releasesfb(HProfile hprofile, int hfb)
{
	return 0;
}

int drvispprofile_setcproc(HProfile hprofile, int enable, int range, int apply )
{
	return 0;
}
int drvispprofile_setcontrast(HProfile hprofile, int contrast, int apply )
{
	return 0;
}

int drvispprofile_setbrightness(HProfile hprofile, int brightness, int apply )
{
	return 0;
}
int drvispprofile_setsaturation(HProfile hprofile, int saturation, int apply )
{
	return 0;
}
int drvispprofile_sethue(HProfile hprofile, int hue, int apply )
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int drvisp_init(HIsp hisp, DRVISPPreset **preset, DRVISPCBFunc *cbfunc )
{
	unsigned errno = 0;
	hisp->status = DRVISPST_EMPTY;
	hisp->oldstatus = DRVISPST_EMPTY;
	hisp->properties = 0;
	hisp->errflag = 0;
	hisp->errcnt = 0;
	
	hisp->nprofiles = 0;
	hisp->preset = preset;
	hisp->cbfunc.I2cInit = cbfunc->I2cInit;
	hisp->cbfunc.I2cExit = cbfunc->I2cExit;
	hisp->cbfunc.I2cRead = cbfunc->I2cRead;
	hisp->cbfunc.I2cWrite = cbfunc->I2cWrite;
	hisp->cbfunc.TransInit = cbfunc->TransInit;
	hisp->cbfunc.TransExit = cbfunc->TransExit;
	hisp->cbfunc.TransTurnOn = cbfunc->TransTurnOn;
	hisp->cbfunc.TransTurnOff = cbfunc->TransTurnOff;
	hisp->cbfunc.TransWakeUp = cbfunc->TransWakeUp;
	hisp->cbfunc.TransSleep = cbfunc->TransSleep;
	hisp->cbfunc.TransStream = cbfunc->TransStream;
	hisp->cbfunc.TransStop = cbfunc->TransStop;
	hisp->cbfunc.Flesh = cbfunc->Flesh;

	hisp->hprofile = 0;
	hisp->hcurprofile = 0;
	hisp->hnextprofile = 0;
	hisp->hprevprofile = 0;

	hisp->nevents= 0;
	hisp->hevent = 0;

	hisp->jpeginfo.addr = 0; 
	hisp->jpeginfo.size = 0;
	hisp->jpeginfo.tickcount = 0;			
	hisp->jpeginfo.framenum = 0;
	hisp->jpeginfo.flags = 0;
	hisp->jpeginfo.idx = 0;
	hisp->jpeginfo.iso = 0;				
	hisp->jpeginfo.exposure = 0;	
	hisp->jpeginfo.wb = 0;				

	if (sizeof(DRVISP) != DRVISP_HANDLER_SIZE)
	{
		errno = 1; 
		return errno;
	}
	if (sizeof(DRVISPProfile) != DRVISP_HPROFILE_SIZE)
	{
		errno = 1; 
		return errno;
	}

	if (sizeof(DRVISPEvent) != DRVISP_HEVENT_SIZE)
	{
		errno = 1; 
		return errno;
	}

	drvisp_makeevent(hisp, &hisp->event, 0);
	#if 1
	{
		HEvent hevent;
		hevent = &hisp->event;
/*
		drvispevent_maskmipi(hevent, 0x00000000 );
		drvispevent_maskmi(hevent, 0x00000003 );
		drvispevent_maskisp(hevent, 0x00000023 );
		drvispevent_maskjpe(hevent, 0x00000000 );
*/
		drvispevent_maskmi(hevent, 0x00000001 );
	}
	#endif
	drvisp_setstatus (hisp, DRVISPST_START);

	hisp->cbfunc.I2cInit();

	return errno;
} //  DRVISP;
	

int drvisp_exit(HIsp hisp)
{
	unsigned errno = 0;
	if (sizeof(DRVISP) != DRVISP_HANDLER_SIZE)
	{
		errno = 1; 
		return errno;
	}

	drvisp_setstatus (hisp, DRVISPST_START);
	
	return 0;
}

int drvisp_getjpeginfo(HIsp hisp, int idx, int reserved, DRVISPJpegInfo *jpeginfo)
{
	unsigned errno = 0;
	if (sizeof(DRVISP) != DRVISP_HANDLER_SIZE)
	{
		errno = 1; 
		return errno;
	}
	if (hisp->jpeginfo.size == 0)
	{
		errno = 1;
		return errno;
	}
	jpeginfo->addr		= hisp->jpeginfo.addr; 
	jpeginfo->size		= hisp->jpeginfo.size ;
	jpeginfo->tickcount	= hisp->jpeginfo.tickcount;
	jpeginfo->framenum	= hisp->jpeginfo.framenum;
	jpeginfo->flags		= hisp->jpeginfo.flags;
	jpeginfo->idx		= hisp->jpeginfo.idx;
	jpeginfo->iso		= hisp->jpeginfo.iso;
	jpeginfo->exposure	= hisp->jpeginfo.exposure;
	jpeginfo->wb		= hisp->jpeginfo.wb;			

	return 0;
}

int drvisp_getimginfo(HIsp hisp, int idx, int reserved, DRVISPImgInfo *imginfo)
{
	unsigned errno = 0;
	if (sizeof(DRVISP) != DRVISP_HANDLER_SIZE)
	{
		errno = 1; 
		return errno;
	}

	imginfo->mp_type		= 0;
	imginfo->mp_addr1		= hisp->hcurprofile->mp.lastaddr1;
	imginfo->mp_addr2		= hisp->hcurprofile->mp.lastaddr2;
	imginfo->mp_addr3		= hisp->hcurprofile->mp.lastaddr3;
//	imginfo->mp_size1		= (hisp->hcurprofile->mp.nextaddr1) ? (hisp->hcurprofile->mp.nextaddr1 - hisp->hcurprofile->mp.lastaddr1) : ( hisp->hcurprofile->mp.addr1 + hisp->hcurprofile->mp.size1 - hisp->hcurprofile->mp.lastaddr1);
//	imginfo->mp_size2		= (hisp->hcurprofile->mp.nextaddr2) ? (hisp->hcurprofile->mp.nextaddr2 - hisp->hcurprofile->mp.lastaddr2) : ( hisp->hcurprofile->mp.addr2 + hisp->hcurprofile->mp.size2 - hisp->hcurprofile->mp.lastaddr2);
//	imginfo->mp_size3		= (hisp->hcurprofile->mp.nextaddr3) ? (hisp->hcurprofile->mp.nextaddr3 - hisp->hcurprofile->mp.lastaddr3) : ( hisp->hcurprofile->mp.addr3 + hisp->hcurprofile->mp.size3 - hisp->hcurprofile->mp.lastaddr3);

	// in case : DRVISPDTYPE_YCbCr422 | DRVISPDTYPE_PLANAR
	imginfo->mp_size1		= hisp->hcurprofile->mp.dst.width * hisp->hcurprofile->mp.dst.height; 
	imginfo->mp_size2		= (hisp->hcurprofile->mp.dst.width * hisp->hcurprofile->mp.dst.height) >> 1; 
	imginfo->mp_size3		= (hisp->hcurprofile->mp.dst.width * hisp->hcurprofile->mp.dst.height) >> 1; 

	imginfo->sp_type		= 0;
	imginfo->sp_addr1		= hisp->hcurprofile->sp.lastaddr1;
	imginfo->sp_addr2		= hisp->hcurprofile->sp.lastaddr2;
	imginfo->sp_addr3		= hisp->hcurprofile->sp.lastaddr3;
//	imginfo->sp_size1		= (hisp->hcurprofile->sp.nextaddr1) ? (hisp->hcurprofile->sp.nextaddr1 - hisp->hcurprofile->sp.lastaddr1) : ( hisp->hcurprofile->sp.addr1 + hisp->hcurprofile->sp.size1 - hisp->hcurprofile->sp.lastaddr1);
//	imginfo->sp_size2		= (hisp->hcurprofile->sp.nextaddr2) ? (hisp->hcurprofile->sp.nextaddr2 - hisp->hcurprofile->sp.lastaddr2) : ( hisp->hcurprofile->sp.addr2 + hisp->hcurprofile->sp.size2 - hisp->hcurprofile->sp.lastaddr2);
//	imginfo->sp_size3		= (hisp->hcurprofile->sp.nextaddr3) ? (hisp->hcurprofile->sp.nextaddr3 - hisp->hcurprofile->sp.lastaddr3) : ( hisp->hcurprofile->sp.addr3 + hisp->hcurprofile->sp.size3 - hisp->hcurprofile->sp.lastaddr3);

	// in case : DRVISPDTYPE_YCbCr422 | DRVISPDTYPE_PLANAR
	imginfo->sp_size1		= hisp->hcurprofile->sp.dst.xpitch * hisp->hcurprofile->sp.dst.ypitch; 
	imginfo->sp_size2		= (hisp->hcurprofile->sp.dst.xpitch * hisp->hcurprofile->sp.dst.ypitch) >> 1; 
	imginfo->sp_size3		= (hisp->hcurprofile->sp.dst.xpitch * hisp->hcurprofile->sp.dst.ypitch) >> 1; 

	imginfo->tickcount	= hisp->jpeginfo.tickcount;
	imginfo->framenum	= hisp->jpeginfo.framenum;
	imginfo->flags		= hisp->jpeginfo.flags;
	imginfo->idx		= hisp->jpeginfo.idx;
	imginfo->iso		= hisp->jpeginfo.iso;
	imginfo->exposure	= hisp->jpeginfo.exposure;
	imginfo->wb		= hisp->jpeginfo.wb;			

	return 0;
}
static int drvisp_transition(HIsp hisp, int status, int newstate)
{
	int errno = 0;

	if ( hisp->properties & DRVISPFLAG_IN_PROGRESS)
	{
		errno = 9; 
		return errno;
	}

	hisp->properties |=DRVISPFLAG_IN_PROGRESS;
	
	switch (status)
	{
		case DRVISPST_EMPTY:
			switch (newstate)
			{
				case DRVISPST_START:
				{
					hisp->cbfunc.TransInit(hisp);
					break;
				}
				default :
					errno = 2;	// wrong newstate
//					break;
			};
			break;
			
		case DRVISPST_START:
			switch (newstate)
			{
				case DRVISPST_IDLE:
				{
					hisp->cbfunc.TransTurnOn(hisp);
					break;
				}
				case DRVISPST_EMPTY:
				{
					hisp->cbfunc.TransExit(hisp);
					break;
				}
				default :
					errno = 3;	// wrong newstate
//					break;
			};
			break;
		case DRVISPST_POWERDOWN:
			switch (newstate)
			{
				case DRVISPST_IDLE:
				{
					hisp->cbfunc.TransWakeUp(hisp);
					break;
				}
				default :
					errno = 4;	// wrong newstate
//					break;
			};
			break;
		case DRVISPST_IDLE:
			switch (newstate)
			{
				case DRVISPST_START:
				{	hisp->cbfunc.TransTurnOff(hisp);
					break;
				}
				case DRVISPST_POWERDOWN:
				{	hisp->cbfunc.TransSleep(hisp);
					break;
				}
				case DRVISPST_STREAMING:
				{
					{
						int update = 0;
						if (hisp->properties & DRVISPFLAG_PROFILEUPDATE_MODECHANGE)
						{
							update = 1;
							hisp->properties &= ~DRVISPFLAG_PROFILEUPDATE_MODECHANGE;
						}		
						hisp->errcnt = 0;
						hisp->errflag = 0;
						hisp->properties &= ~DRVISPFLAG_ERROR_OCCURED;
						
						errno = drvisp_setprofile_apply(hisp,update);
						// * 메모리 출력 
					}
					hisp->cbfunc.TransStream(hisp);
					break;
				}
				case DRVISPST_DEBUG:
				{
					hisp->cbfunc.TransDebugOn(hisp);
					break;
				}
				default :
					errno = 5;	// wrong newstate
//					break;
			};
			break;
		case DRVISPST_STREAMING:
			switch (newstate)
			{
				case DRVISPST_IDLE:
				{
					// * 메모리 출력 중지 
					volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
					
					//REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_PATH_ENABLE, OFF);
					//REG_SET_SLICE(ptMrvReg->mi_init, MRV_MI_MI_CFG_UPD, ON);
					errno = drvisp_stop_apply(hisp, 0,0);    //profile_mi_apply(hprofile);					

					hisp->cbfunc.TransStop(hisp);
					break;
				}
				default :
					errno = 6;	// wrong newstate
//					break;
			};
			break;
		case DRVISPST_DEBUG:
			switch (newstate)
			{
				case DRVISPST_IDLE:
				{
					hisp->cbfunc.TransDebugOff(hisp);
					break;
				}
				default :
					errno = 7;	// wrong newstate
//					break;
			};
			break;
		default :
			errno = 8;	// wrong status - fatal error
//			break;
	};

	if (errno)
	{
		hisp->properties &= ~DRVISPFLAG_IN_PROGRESS;
		if (errno == 0x80000001)      //neo:
		{
			hisp->oldstatus = hisp->status;
			hisp->status = DRVISPST_IDLE;
			hisp->properties &= ~DRVISPFLAG_IN_PROGRESS;
			return errno;
		}	
	}
	hisp->oldstatus = hisp->status;
	hisp->status = newstate;
	hisp->properties &= ~DRVISPFLAG_IN_PROGRESS;
	return 0;
}
// 모드 전환 
// transition 호출 
int drvisp_setstatus (HIsp hisp, int state)
{
	int errno = 0;
	if (!hisp)
	{
		errno = 1; // invalid hisp handler
		return errno;
	}

	errno = drvisp_transition(hisp, hisp->status, state);
	return errno;
}
#if 0
int drvisp_getstatus (HIsp hisp, int *state)
{
	int errno = 0;
	if (!hisp)
	{
		errno = 1; // invalid hisp handler
		return errno;
	}
	*state = hisp->status;
	{
		volatile unsigned *pflag;		
		pflag	= &(hisp->properties);	
		if ( *pflag & DRVISPFLAG_ERROR_OCCURED )
		{
			errno = 0x80000001;
			return 0x80000001; // ISP_ERROR
		}
	}
	
	return errno;
}
#else
int drvisp_getstatus (HIsp hisp, int *state)
{
	int errno = 0;
	if (!hisp)
	{
		errno = 1; // invalid hisp handler
		return errno;
	}
	*state = hisp->status;
	{
		volatile unsigned *pflag;
		unsigned dtype;
		pflag	= &(hisp->properties);	
		if ( *pflag & DRVISPFLAG_ERROR_OCCURED )
		{
			errno = 0x80000001;
			return errno; // ISP_ERROR
		}
		if ( *pflag & DRVISPFLAG_IN_PROGRESS )
		{
			errno = 0x80000002;  // IN_PROGRESS
			return errno; 
		}
		if ( *pflag & DRVISPFLAG_WAIT_ONE_FRAME )
		{
			switch (*state)
			{
				case DRVISPST_IDLE:
					{
					dtype = hisp->hcurprofile->mp.dtype & DRVISPDTYPE_MAX;
					pflag	= &(hisp->properties);
					//if (!errno && xxxxx)	// error가 없다면 

						if (hisp->hcurprofile->mp.size1)
						{
							if  (dtype != DRVISPDTYPE_JPEG) 
							{
								if (*pflag &  DRVISPFLAG_MP_LASTFRAME_END)
								{
									*pflag &= ~DRVISPFLAG_MP_LASTFRAME_END;
								}	
								else
								{
									// not yet
									errno |= 0x80000100;
								}
							}
						}
						if (hisp->hcurprofile->sp.size1)
						{
							if (*pflag &  DRVISPFLAG_SP_LASTFRAME_END)
							{
								*pflag &= ~DRVISPFLAG_SP_LASTFRAME_END;
							}	
							else
							{
								// not yet
								errno |= 0x80000200;
							}

						}
						if (!errno)
						{
							*pflag &= ~DRVISPFLAG_ISP_OFF;	
						}
					}
					break;
				case DRVISPST_STREAMING:
					if  ((hisp->hcurprofile->mp.dtype & DRVISPDTYPE_MAX) != DRVISPDTYPE_JPEG) 
					{
						if (hisp->hcurprofile->mp.size1)
						{
							if (! drvispevent_ctrlmi(&hisp->event, DRVISPINTMI_MP_FRAME_END))
							{
								// not yet
								errno |= 0x80000100;
							}
						}
						if (hisp->hcurprofile->sp.size1)
						{
							if (! drvispevent_ctrlmi(&hisp->event, DRVISPINTMI_SP_FRAME_END))
							{
								// not yet
								errno |= 0x80000200;
							}
						}   
					}
					else
					{
						// ok
					}					
					break;
			}			

			if (errno == 0)	// ok
			{
				*pflag &= ~DRVISPFLAG_WAIT_ONE_FRAME;
			}
		}
	}
	
	return errno;
}
#endif
// MIPI
unsigned drvisp_irq1_noti( HIsp hisp )
{
	unsigned 	errno = 0;
	unsigned 	result;
	HEvent		node;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	// get masked state
	// copy
	// set clear state
	result = drvisp_irq1_get(hisp, 0xffffffff);
	node = hisp->hevent;
	while (node)
	{
		node->event1 |= result & node->eventmask1;
		node = node->next;
	}
	drvisp_irq1_clear(hisp, result);
	return 0;
}

// MI
unsigned drvisp_irq2_noti( HIsp hisp )
{
	unsigned errno = 0;
	unsigned result;
	HEvent		node;
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
	   
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}	
	// get masked state
	// copy
	// set clear state
	result = drvisp_irq2_get(hisp, 0xffffffff);
	node = hisp->hevent;
	while (node)
	{
		node->event2 |= result & node->eventmask2;
		node = node->next;
	}
	// to do something....
	if (result & DRVISPINTMI_MP_FRAME_END)
	{
		if (hisp->properties & DRVISPFLAG_ISP_OFF)
		{
			hisp->properties |= DRVISPFLAG_MP_LASTFRAME_END;	
			hisp->properties &= ~DRVISPFLAG_ISP_OFF;
		}
		hisp->hcurprofile->mp.lastaddr1 = hisp->hcurprofile->mp.nextaddr1 ;
		hisp->hcurprofile->mp.lastaddr2 = hisp->hcurprofile->mp.nextaddr2 ;
		hisp->hcurprofile->mp.lastaddr3 = hisp->hcurprofile->mp.nextaddr3 ;
		hisp->hcurprofile->mp.lastsize = ptMrvReg->mi_byte_cnt;
		hisp->hcurprofile->mp.nextaddr1 = (unsigned char*) (ptMrvReg->mi_mp_y_base_ad_init + ptMrvReg->mi_mp_y_offs_cnt_start   ) ;
		hisp->hcurprofile->mp.nextaddr2 = (unsigned char*) (ptMrvReg->mi_mp_cb_base_ad_init + ptMrvReg->mi_mp_cb_offs_cnt_start );
		hisp->hcurprofile->mp.nextaddr3 = (unsigned char*) (ptMrvReg->mi_mp_cr_base_ad_init + ptMrvReg->mi_mp_cr_offs_cnt_start );

		//drvispprofile_zoom_apply(hisp->hcurprofile,1);
		
	}
	if (result & DRVISPINTMI_SP_FRAME_END) 
	{
		if (hisp->properties & DRVISPFLAG_ISP_OFF) 
		{
			hisp->properties |= DRVISPFLAG_SP_LASTFRAME_END;
			hisp->properties &= ~DRVISPFLAG_ISP_OFF;
		}
		hisp->hcurprofile->sp.lastaddr1 = hisp->hcurprofile->sp.nextaddr1;
		hisp->hcurprofile->sp.lastaddr2 = hisp->hcurprofile->sp.nextaddr2;
		hisp->hcurprofile->sp.lastaddr3 = hisp->hcurprofile->sp.nextaddr3;
		hisp->hcurprofile->sp.nextaddr1 = (unsigned char*) ( ptMrvReg->mi_sp_y_base_ad_init + ptMrvReg->mi_sp_y_offs_cnt_start )  ;
		hisp->hcurprofile->sp.nextaddr2 = (unsigned char*) ( ptMrvReg->mi_sp_cb_base_ad_init + ptMrvReg->mi_sp_cb_offs_cnt_start );
		hisp->hcurprofile->sp.nextaddr3 = (unsigned char*) ( ptMrvReg->mi_sp_cr_base_ad_init + ptMrvReg->mi_sp_cr_offs_cnt_start );
	}	
	drvisp_irq2_clear(hisp, result);
	return 0;
}

// ISP
unsigned drvisp_irq3_noti( HIsp hisp )
{
	unsigned errno = 0;
	unsigned result;
	HEvent		node;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}	
	// get masked state
	// copy
	// set clear state
	result = drvisp_irq3_get(hisp, 0xffffffff);
	node = hisp->hevent;
	while (node)
	{
		node->event3 |= result & node->eventmask3;
		node = node->next;
	}
	// to do something....
	if ( (hisp->properties & DRVISPFLAG_STOP_CMD) && (result & DRVISPINT_ISP_OFF) )
	{
		hisp->properties &= ~DRVISPFLAG_STOP_CMD;
		hisp->properties |= DRVISPFLAG_ISP_OFF;		//neo	
	}

	if ( result & DRVISPINT_PIC_ERR )
	{
		volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;		
		unsigned err;
		hisp->errcnt++;
		err = ptMrvReg->isp_err;

		hisp->errflag |= (err<<4) | DRVISPERRFLAG_PIC_SIZE_ERR;
		hisp->properties |= DRVISPFLAG_ERROR_OCCURED;
		ptMrvReg->isp_err_clr = err;
	}

	if ( result & DRVISPINT_DATA_LOSS )
	{
		volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;		
		unsigned err;
		hisp->errcnt++;
		err = ptMrvReg->mi_status;

		hisp->errflag |= (err<<8) | DRVISPERRFLAG_DATA_LOSS;
		hisp->properties |= DRVISPFLAG_ERROR_OCCURED;
		ptMrvReg->mi_status_clr= err;
	}	
#if 0
	if (result & DRVISPINT_VSTART) 	
	{	//neo: test code
		volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
		static unsigned direction =0;
		if (hisp->hcurprofile->zoomlvl >= hisp->hcurprofile->sctbl_max)
		{
			direction = 1;
			hisp->hcurprofile->zoomlvl = hisp->hcurprofile->sctbl_max;
		}
		if (hisp->hcurprofile->zoomlvl <= 0)
		{
			direction = 0;
			hisp->hcurprofile->zoomlvl = 0;
		}
		if (direction)  hisp->hcurprofile->zoomlvl--;
		else			hisp->hcurprofile->zoomlvl++;

		drvispprofile_zoom_apply(hisp->hcurprofile, 0);

		REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, 1);			
	}
#endif	
	drvisp_irq3_clear(hisp, result);
	return 0;
}
// JPE
unsigned drvisp_irq4_noti( HIsp hisp )
{
	unsigned errno = 0;
	unsigned result;
	HEvent		node;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}	
	// get masked state
	// copy
	// set clear state
	result = drvisp_irq4_get(hisp, 0xffffffff);
	node = hisp->hevent;
	while (node)
	{
		node->event4 |= result & node->eventmask4;
		node = node->next;
	}
	drvisp_irq4_clear(hisp, result);
	return 0;
}

unsigned drvisp_chkirq( HIsp hisp )
{
	unsigned errno = 0;
	unsigned r1 = 0;
	unsigned r2 = 0;
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
	
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	
	if (  ptMrvReg->mi_mis & 3)
	{
		r1 = 1;
	}
	if ( ptMrvReg->isp_mis & 0x40)
	{
		r2 = 1;
	}
	
	if (r1 && r2)
	{	return 1;
	}
	else
	{	return 0;
	}
}

void drvisp_clrirq( HIsp hisp )
{
	drvisp_irq2_clear(hisp,0xffffffff);
	drvisp_irq3_clear(hisp,0xffffffff);
}


int drvisp_makeevent(HIsp hisp, HEvent hevent, int reserved)
{
	int errno = 0;
	if (!hisp)
	{
		errno = 1; // invalid hisp handler
		return errno;
	}
	if (!hevent)
	{
		errno = 1; // invalid hprofile handler
		return errno;
	}

	memset( hevent, 0, sizeof(DRVISPEvent) );

	hevent->hisp = hisp;
	hevent->next = 0;
	{
		HEvent node = hisp->hevent;
		if (node)
		{
			while (node->next)
			{
				node = node->next;
			}
			node->next = hevent;
		}
		else
		{
			hisp->hevent = hevent;
		}
		hisp->nevents++;
	}
	return 0;
}

unsigned drvispevent_waitmipi(HEvent hevent, unsigned events, unsigned timeout)
{
	unsigned errno = 0;
	unsigned result;
	volatile unsigned *pevent;
	if (!hevent)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}

	// todo : using event handler 
	//while ( ! (result = (hevent->event1 & events)) )
	pevent = (volatile unsigned *) &(hevent->event1);
	while ( ! (result = *pevent & events)) 
	{
		// delay time - context switch 
	}
	hevent->event1 ^= result;
	return result;
}
unsigned drvispevent_ctrlmipi(HEvent hevent, unsigned events)
{
	unsigned errno = 0;
	unsigned result;

	if (!hevent)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}

	result = (hevent->event1 & events);
	if (result)
	{
		hevent->event1 ^= result;
		return result;
	}
	return 0;
}

// todo: mask가 안 되어 있다면 영원히 안들어 옴으로 error 처리 
unsigned drvispevent_waitmi(HEvent hevent, unsigned events, unsigned timeout)
{
	unsigned errno = 0;
	unsigned result;
	HIsp hisp = hevent->hisp;
	volatile unsigned *pflag;
	volatile unsigned *pevent;
	if (!hevent)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	if (!events)
	{
		errno = 0x80000000; // invalid events
		return errno;
	}

	pevent = (volatile unsigned *) &(hevent->event2);
	/*
	// todo : using event handler 
	result = (((volatile) hevent->event2) & events);
	while ( ! result )
	{
		// delay time - context switch 
		if (!events)
		{
			errno = 0x80000000; // invalid events
			return errno;
		}
		result = (((volatile) hevent->event2) & events);
	}
	*/
	pflag	= &(hisp->properties);	
	while ( ! (result = *pevent & events)) 
	{
		// delay time - context switch 
		if (!events)
		{
			errno = 0x80000000; // invalid events
			return errno;
		}
		if ( *pflag & DRVISPFLAG_ERROR_OCCURED )
		{
			errno = 0x80000001;
			return errno; // ISP_ERROR
		}
	}

	
	hevent->event2 &= ~result;
	return result;
}
unsigned drvispevent_ctrlmi (HEvent hevent, unsigned events)
{
	unsigned errno = 0;
	unsigned result;

	if (!hevent)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}

	result = (hevent->event2 & events);
	if (result)
	{
		hevent->event2 ^= result;
		return result;
	}
	return 0;
}

unsigned drvispevent_waitisp(HEvent hevent, unsigned events, unsigned timeout)
{
	unsigned errno = 0;
	unsigned result;
	volatile unsigned *pevent;
	HIsp hisp = hevent->hisp;
	volatile unsigned *pflag;
	if (!hevent)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}

	// todo : using event handler 
	//while ( ! (result = (hevent->event3 & events)) )
	pevent = (volatile unsigned *) &(hevent->event3);
	pflag	= &(hisp->properties);	
	while ( ! (result = *pevent & events)) 

	{
		// delay time - context switch 
		if ( *pflag & DRVISPFLAG_ERROR_OCCURED )
		{
			errno = 0x80000001;
			return errno; // ISP_ERROR
		}
	}
	hevent->event3 ^= result;
	return result;
}
unsigned drvispevent_ctrlisp (HEvent hevent, unsigned events)
{
	unsigned errno = 0;
	unsigned result;

	if (!hevent)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}

	result = (hevent->event3 & events);
	if (result)
	{
		hevent->event3 ^= result;
		return result;
	}
	return 0;
}

unsigned drvispevent_waitjpe(HEvent hevent, unsigned events, unsigned timeout)
{
	unsigned errno = 0;
	unsigned result;
	volatile unsigned *pevent;
	if (!hevent)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}

	// todo : using event handler 
	//while ( ! (result = (hevent->event4 & events)) )
	pevent = (volatile unsigned *) &(hevent->event4);
	while ( ! (result = *pevent & events)) 

	{
		// delay time - context switch 
	}
	hevent->event4 ^= result;
	return result;
}
unsigned drvispevent_ctrljpe (HEvent hevent, unsigned events)
{
	unsigned errno = 0;
	unsigned result;

	if (!hevent)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}

	result = (hevent->event4 & events);
	if (result)
	{
		hevent->event4 ^= result;
		return result;
	}
	return 0;
}

unsigned drvispevent_maskmipi(HEvent hevent, unsigned events)
{
	unsigned errno = 0;

	if (!hevent)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	
	hevent->eventmask1 |= events;
	drvisp_irq1_mask( hevent->hisp, events);

	return hevent->eventmask1;
}
unsigned drvispevent_unmaskmipi(HEvent hevent, unsigned events)
{
	unsigned errno = 0;
	unsigned mask = 0;
	HEvent 		node;
	
	if (!hevent)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}

	hevent->eventmask1 &= (~events);

	node = hevent->hisp->hevent;
	while (node)
	{
		mask |= hevent->eventmask1;
		node = node->next;
	}
	drvisp_irq1_unmask(hevent->hisp, ~mask  );
	drvisp_irq1_mask( hevent->hisp, mask);

	return mask;
}

unsigned drvispevent_maskmi(HEvent hevent, unsigned events)
{
	unsigned errno = 0;

	if (!hevent)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	
	hevent->eventmask2 |= events;
	drvisp_irq2_mask( hevent->hisp, events);

	return hevent->eventmask2;
}
unsigned drvispevent_unmaskmi(HEvent hevent, unsigned events)
{
	unsigned errno = 0;
	unsigned mask = 0;
	HEvent 		node;
	
	if (!hevent)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}

	hevent->eventmask2 &= (~events);

	node = hevent->hisp->hevent;
	while (node)
	{
		mask |= hevent->eventmask2;
		node = node->next;
	}
	drvisp_irq2_unmask(hevent->hisp, ~mask  );
	drvisp_irq2_mask( hevent->hisp, mask);

	return mask;
}

unsigned drvispevent_maskisp(HEvent hevent, unsigned events)
{
	unsigned errno = 0;
	unsigned result;

	if (!hevent)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	
	hevent->eventmask3 |= events;
	drvisp_irq3_mask( hevent->hisp, events);

	return hevent->eventmask3;
}
unsigned drvispevent_unmaskisp(HEvent hevent, unsigned events)
{
	unsigned errno = 0;
	unsigned mask = 0;
	HEvent 		node;
	
	if (!hevent)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}

	hevent->eventmask3 &= (~events);

	node = hevent->hisp->hevent;
	while (node)
	{
		mask |= hevent->eventmask3;
		node = node->next;
	}
	drvisp_irq3_unmask(hevent->hisp, ~mask  );
	drvisp_irq3_mask( hevent->hisp, mask);

	return mask;
}

unsigned drvispevent_maskjpe(HEvent hevent, unsigned events)
{
	unsigned errno = 0;
	unsigned result;

	if (!hevent)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	
	hevent->eventmask4 |= events;
	drvisp_irq4_mask( hevent->hisp, events);

	return hevent->eventmask4;
}
unsigned drvispevent_unmaskjpe(HEvent hevent, unsigned events)
{
	unsigned errno = 0;
	unsigned mask = 0;
	HEvent 		node;
	
	if (!hevent)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}

	hevent->eventmask4 &= (~events);

	node = hevent->hisp->hevent;
	while (node)
	{
		mask |= hevent->eventmask4;
		node = node->next;
	}
	drvisp_irq4_unmask(hevent->hisp, ~mask  );
	drvisp_irq4_mask( hevent->hisp, mask);

	return mask;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static unsigned	drvisp_irq1_get(HIsp hisp, unsigned events)
{
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	return ( ptMrvReg->mipi_mis & events );
}
static unsigned 	drvisp_irq1_getraw(HIsp hisp, unsigned events)
{
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	return ( ptMrvReg->mipi_ris & events );
}
static unsigned 	drvisp_irq1_clear(HIsp hisp, unsigned events)
{ 
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	ptMrvReg->mipi_icr = events;
	return 0;
}
static unsigned	drvisp_irq1_set(HIsp hisp, unsigned events)
{
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	ptMrvReg->mipi_isr = events;
	return 0;
}
static unsigned	drvisp_irq1_mask(HIsp hisp, unsigned events)
{
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	ptMrvReg->mipi_imsc |= events;
	return ptMrvReg->mipi_imsc;
}
static unsigned	drvisp_irq1_unmask(HIsp hisp, unsigned events)
{
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	ptMrvReg->mipi_imsc &= (~events);
	return ptMrvReg->mipi_imsc;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static unsigned	drvisp_irq2_get(HIsp hisp, unsigned events)
{
	//volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)tcc_p2v(MEM_MRV_REG_BASE);	
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	return ( ptMrvReg->mi_mis & events );
}
static unsigned 	drvisp_irq2_getraw(HIsp hisp, unsigned events)
{
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	return ( ptMrvReg->mi_ris & events );
}
static unsigned	drvisp_irq2_clear(HIsp hisp, unsigned events)
{ 
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	ptMrvReg->mi_icr = events;
	return 0;
}
static unsigned	drvisp_irq2_set(HIsp hisp, unsigned events)
{
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	ptMrvReg->mi_isr = events;
	return 0;
}
static unsigned	drvisp_irq2_mask(HIsp hisp, unsigned events)
{
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	ptMrvReg->mi_imsc |= events;
	return ptMrvReg->mi_imsc;
}
static unsigned	drvisp_irq2_unmask(HIsp hisp, unsigned events)
{
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	ptMrvReg->mi_imsc &= (~events);
	return ptMrvReg->mi_imsc;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static unsigned	drvisp_irq3_get(HIsp hisp, unsigned events)
{
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	return ( ptMrvReg->isp_mis & events );
}
static unsigned 	drvisp_irq3_getraw(HIsp hisp, unsigned events)
{
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	return ( ptMrvReg->isp_ris & events );
}
static unsigned 	drvisp_irq3_clear(HIsp hisp, unsigned events)
{ 
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	ptMrvReg->isp_icr = events;
	return 0;
}
static unsigned	drvisp_irq3_set(HIsp hisp, unsigned events)
{
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	ptMrvReg->isp_isr = events;
	return 0;
}
static unsigned	drvisp_irq3_mask(HIsp hisp, unsigned events)
{
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	ptMrvReg->isp_imsc |= events;
	return ptMrvReg->isp_imsc;
}
static unsigned	drvisp_irq3_unmask(HIsp hisp, unsigned events)
{
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	ptMrvReg->isp_imsc &= (~events);
	return ptMrvReg->isp_imsc;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static unsigned	drvisp_irq4_get(HIsp hisp, unsigned events)
{
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	result = ptMrvReg->jpe_status_mis | (ptMrvReg->jpe_error_mis << 16);
	return ( result & events );
}
static unsigned 	drvisp_irq4_getraw(HIsp hisp, unsigned events)
{
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	result = ptMrvReg->jpe_status_ris | (ptMrvReg->jpe_error_ris << 16);
	return ( result & events );
}
static unsigned 	drvisp_irq4_clear(HIsp hisp, unsigned events)
{ 
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	ptMrvReg->jpe_status_icr = events & 0xffff;
	ptMrvReg->jpe_error_icr  = events >> 16;
	return 0;
}
static unsigned	drvisp_irq4_set(HIsp hisp, unsigned events)
{
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}

	ptMrvReg->jpe_status_isr = events & 0xffff;
	ptMrvReg->jpe_error_isr  = events >> 16;

	return 0;
}
static unsigned	drvisp_irq4_mask(HIsp hisp, unsigned events)
{
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	ptMrvReg->jpe_status_imr |= (events & 0xffff);
	ptMrvReg->jpe_error_imr |= (events >> 16);
	result = ptMrvReg->jpe_status_imr | (ptMrvReg->jpe_error_imr << 16);
	return result;
}
static unsigned	drvisp_irq4_unmask(HIsp hisp, unsigned events)
{
	volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;	
	unsigned errno = 0;
	unsigned result;
	if (!hisp)
	{
		errno = 0x80000000; // invalid hisp handler
		return errno;
	}
	ptMrvReg->jpe_status_imr &= (~(events & 0xffff));
	ptMrvReg->jpe_error_imr &= (~(events >> 16));
	result = ptMrvReg->jpe_status_imr | (ptMrvReg->jpe_error_imr << 16);
	return result;

}


// 모드 전환시 처리해야 할 내용 
int drvispmode_init	(HIsp hisp)
{
	return 0;
}
int drvispmode_process(HIsp hisp)
{
	return 0;
}
int drvispmode_exit	(HIsp hisp)
{
	return 0;
}

int drvisp_sensoridx(HIsp hisp)
{
	return 0;
}

int drvisp_getsensorclk(HIsp hisp)
{
	return 24000000;	// 24mhz
}

int drvisp_getclk (HIsp hisp)
{
	return 0;
}
