#define VIQE_DEV_NAME		"viqe"
#define VIQE_DEV_MAJOR		230
#define VIQE_DEV_MINOR		0

#define IOCTL_VIQE_INITIALIZE		0xAF0
#define IOCTL_VIQE_EXCUTE			0xAF1
#define IOCTL_VIQE_DEINITIALIZE		0xAF2
#define IOCTL_VIQE_SETTING			0xAF3
/*****************************************************************************
*
* structures
*
******************************************************************************/
typedef struct
{
	int lcdCtrlNo;
	int scalerCh;
	int onTheFly;
	unsigned int srcWidth;
	unsigned int srcHeight;
	unsigned int crop_top, crop_bottom, crop_left, crop_right;
	int deinterlaceM2M;
	int renderCnt;
	int OddFirst;
	int DI_use;

	unsigned int address[6];
	unsigned int dstAddr;
}VIQE_DI_TYPE;


static long tcc_viqe_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

