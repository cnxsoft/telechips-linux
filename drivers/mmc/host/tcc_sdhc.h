#ifndef __TCC_SDHC_H__
#define __TCC_SDHC_H__


/*******************************************************************************
*	SD/SDIO/MMC/CE-ATA Host Controller Register Define   (Base Addr = 0xB0020000)
********************************************************************************/
//Host controller 0
#define SDHC0_BASE         0xB0020000
//Host controller 1
#define SDHC1_BASE         0xB0020200
//Host controller 2
#define SDHC2_BASE         0xB0020400
//Host controller 3
#define SDHC3_BASE         0xB0020600
// Channel Control Register
#define SDCHCTRL_BASE             0xB0020800 //  SD/MMC Channel control register

/*
 * Controller registers
 */
#define TCCSDHC_DMA_ADDRESS 		0x00
#define TCCSDHC_BLOCK_SIZE			0x04

#define TCCSDHC_MAKE_BLKSZ(dma, blksz) (((dma & 0x7) << 12) | (blksz & 0xFFF))

#define TCCSDHC_BLOCK_COUNT 		0x06
#define TCCSDHC_ARGUMENT			0x08
#define TCCSDHC_TRANSFER_MODE		0x0C
#define TCCSDHC_COMMAND 			0x0E
#define TCCSDHC_TMODE_COM			0x0C

#define TCCSDHC_MAKE_CMD(c, f) (((c & 0xff) << 8) | (f & 0xff))

#define TCCSDHC_RESPONSE10			0x10
#define TCCSDHC_RESPONSE32			0x14
#define TCCSDHC_RESPONSE54			0x18
#define TCCSDHC_RESPONSE76			0x1c
#define TCCSDHC_BUFFER				0x20
#define TCCSDHC_PRESENT_STATE		0x24
#define TCCSDHC_HOST_CONTROL		0x28
#define TCCSDHC_POWER_CONTROL		0x29
#define TCCSDHC_BLOCK_GAP_CONTROL	0x2A
#define TCCSDHC_WAKE_UP_CONTROL 	0x2B
#define TCCSDHC_CLOCK_CONTROL		0x2C
#define TCCSDHC_TIMEOUT_CONTROL 	0x2E
#define TCCSDHC_SOFTWARE_RESET		0x2F
#define TCCSDHC_INT_STATUS			0x30
#define TCCSDHC_INT_ENABLE			0x34
#define TCCSDHC_SIGNAL_ENABLE		0x38
#define TCCSDHC_ACMD12_ERR			0x3C

#define TCCSDHC_ADMA_ERROR			0x54
#define TCCSDHC_ADMA_ADDRESS		0x58

#define TCCSDHC_SLOT_INT			0xFC
#define TCCSDHC_HOST_VERSION		0xFE


#define HwSD_COM_TRANS_ABORT	Hw23+Hw22
#define HwSD_COM_TRANS_DATSEL	Hw21		// data present select
#define HwSD_COM_TRANS_CICHK	Hw20		// command index check enable
#define HwSD_COM_TRANS_CRCHK	Hw19		// command CRC check enable
#define HwSD_COM_TRANS_SPI		Hw7 		// SPI mode
#define HwSD_COM_TRANS_ATACMD	Hw6 		// cmd completion enable for CE-ATA
#define HwSD_COM_TRANS_MS		Hw5 		// multi/single block select
#define HwSD_COM_TRANS_DIR		Hw4 		// data transfer direction select
#define HwSD_COM_TRANS_ACMD12	Hw2 		// auto CMD12 enable
#define HwSD_COM_TRANS_BCNTEN	Hw1 		// block count enable
#define HwSD_COM_TRANS_DMAEN	Hw0 		// DMA Enable


#define HwSDCLKSEL_DIV_256		0x80
#define HwSDCLKSEL_DIV_128		0x40
#define HwSDCLKSEL_DIV_64		0x20
#define HwSDCLKSEL_DIV_32		0x10
#define HwSDCLKSEL_DIV_16		0x08
#define HwSDCLKSEL_DIV_8		0x04
#define HwSDCLKSEL_DIV_4		0x02
#define HwSDCLKSEL_DIV_2		0x01
#define HwSDCLKSEL_DIV_0		0x00
#define HwSDCLKSEL_SCK_EN		Hw2
#define HwSDCLKSEL_INCLK_STABLE Hw1
#define HwSDCLKSEL_INCLK_EN 	Hw0

#define HwSD_POWER_POW			Hw8 		// SD bus power
#define HwSD_POWER_SD8			Hw5 		// SD 8-bit mode
#define HwSD_POWER_HS			Hw2 		// high speed enable
#define HwSD_POWER_SD4			Hw1 		// data transfer width
#define HwSD_POWER_VOL33		(Hw11+Hw10+Hw9)

#define HwSD_CTRL_SDMA			0x00
#define HwSD_CTRL_ADMA32		Hw4
#define HwSD_CTRL_DMA_MASK		Hw4+Hw3

#define HwSD_SRESET_RSTALL		Hw0 	// software reset for All
#define HwSD_SRESET_RSTCMD		Hw1 	// software reset for CMD line
#define HwSD_SRESET_RSTDAT		Hw2 	// software reset for DAT line


// Port Control
#define HwSD_PORTCTRL_CD(X) 		(Hw30 << (X))								// Card Detection for SLOT X. (X = 0 or 1)
#define HwSD_PORTCTRL_WP(X) 		(Hw27 << (X))								// Write Protect for SLOT X. (X = 0 or 1)
#if 0  //remove
#define HwSD_PORTCTRL_SLOT3(X)		((X) * Hw12)								// Port Select for SLOT 3 (X = 0 ~ 7)
#define HwSD_PORTCTRL_SLOT3_MASK	HwSD_PORTCTRL_SLOT3(15)
#define HwSD_PORTCTRL_SLOT2(X)		((X) * Hw8) 								// Port Select for SLOT 2 (X = 0 ~ 7)
#define HwSD_PORTCTRL_SLOT2_MASK	HwSD_PORTCTRL_SLOT2(15)
#define HwSD_PORTCTRL_SLOT1(X)		((X) * Hw4) 								// Port Select for SLOT 1 (X = 0 ~ 7)
#define HwSD_PORTCTRL_SLOT1_MASK	HwSD_PORTCTRL_SLOT1(15)
#define HwSD_PORTCTRL_SLOT0(X)		((X) * Hw0) 								// Port Select for SLOT 0 (X = 0 ~ 7)
#define HwSD_PORTCTRL_SLOT0_MASK	HwSD_PORTCTRL_SLOT0(15)
#endif //remove
#define HwSD_STATE_SDWP 		Hw19
#define HwSD_STATE_NODAT		Hw1 		// data inhibit
#define HwSD_STATE_NOCMD		Hw0 		// command inhibit

#define HwSDINT_STATUS_ADMA		Hw25		// ADMA error
#define HwSDINT_STATUS_DATEND	Hw22		// data end bit error
#define HwSDINT_STATUS_DATCRC	Hw21		// data crc error
#define HwSDINT_STATUS_DATTIME	Hw20		// data timeout error
#define HwSDINT_STATUS_CINDEX	Hw19		// command index error
#define HwSDINT_STATUS_CMDEND	Hw18		// command command end bit error
#define HwSDINT_STATUS_CMDCRC	Hw17		// command crc error
#define HwSDINT_STATUS_CMDTIME	Hw16		// command timeout error
#define HwSDINT_STATUS_ERR		Hw15		// error interrupt
#define HwSDINT_STATUS_CDINT	Hw8 		// card interrupt
#define HwSDINT_STATUS_CDOUT	Hw7 		// card removal
#define HwSDINT_STATUS_CDIN 	Hw6 		// card insertion
#define HwSDINT_STATUS_RDRDY	Hw5 		// buffer read ready
#define HwSDINT_STATUS_WRRDY	Hw4 		// buffer write ready
#define HwSDINT_STATUS_DMA		Hw3 		// DMA interrupt
#define HwSDINT_STATUS_BLKGAP	Hw2 		// block gap event
#define HwSDINT_STATUS_TDONE	Hw1 		// transfer complete
#define HwSDINT_STATUS_CDONE	Hw0 		// command complete

#define HwSDINT_EN_ADMA 		Hw25		// ADMA error signal enable
#define HwSDINT_EN_ACMD12		Hw24		// auto CMD12 error signal enable
#define HwSDINT_EN_CLIMIT		Hw23		// current limit error signal enable
#define HwSDINT_EN_DATEND		Hw22		// data end bit error signal enable
#define HwSDINT_EN_DATCRC		Hw21		// data crc error signal enable
#define HwSDINT_EN_DATTIME		Hw20		// data timeout error signal enable
#define HwSDINT_EN_CINDEX		Hw19		// command index error signal enable
#define HwSDINT_EN_CMDEND		Hw18		// command end bit error signal enable
#define HwSDINT_EN_CMDCRC		Hw17		// command crc error signal enable
#define HwSDINT_EN_CMDTIME		Hw16		// command timeout error signal enable
#define HwSDINT_EN_CDINT		Hw8 		// card interrupt signal enable
#define HwSDINT_EN_CDOUT		Hw7 		// card removal signal enable
#define HwSDINT_EN_CDIN 		Hw6 		// card insertion signal enable
#define HwSDINT_EN_RDRDY		Hw5 		// buffer read ready signal enable
#define HwSDINT_EN_WRRDY		Hw4 		// buffer write ready signal enable
#define HwSDINT_EN_DMA			Hw3 		// DMA interrupt signal enable
#define HwSDINT_EN_BLKGAP		Hw2 		// block gap event signal enable
#define HwSDINT_EN_TDONE		Hw1 		// transfer complete signal enable
#define HwSDINT_EN_CDONE		Hw0 		// command complete signal enable


#define  HwSDINT_NORMAL_MASK	0x00007FFF
#define  HwSDINT_ERROR_MASK 0xFFFF8000

#define  HwSDINT_CMD_MASK	(HwSDINT_EN_CDONE | HwSDINT_EN_CMDTIME | \
		HwSDINT_EN_CMDCRC | HwSDINT_EN_CMDEND | HwSDINT_EN_CINDEX)
		
#define  HwSDINT_DATA_MASK	(HwSDINT_EN_TDONE | HwSDINT_EN_DMA | \
		HwSDINT_EN_RDRDY | HwSDINT_EN_WRRDY | \
		HwSDINT_EN_DATTIME | HwSDINT_EN_DATCRC | \
		HwSDINT_EN_DATEND | HwSDINT_EN_ADMA)


typedef struct _SDHOST_T{
	volatile unsigned int	 	SDMA;                       // 0x000 R/W 0x0000 SDMA System Address
//	volatile unsigned short     NOTDEFINE0;                 // 0x002
	volatile unsigned short 	BSIZE;                      // 0x004 R/W 0x0000 Block Size
	volatile unsigned short 	BCNT;                       // 0x006 R/W 0x0000 Block Count
	volatile unsigned int 		ARG;                        // 0x008 R/W 0x0000 Argument
//	volatile unsigned short     NOTDEFINE1;                 // 0x00A

/////	
//	volatile unsigned short 	TMODE;                      // 0x00C R/W 0x0000 Transfer Mode
//	volatile unsigned short 	CMD;                        // 0x00E R/W 0x0000 Command
	volatile unsigned int		CTMODE;						// 0x00C R/W Command and Transfer Mode
/////	

////	
//	volatile unsigned short 	RESP0;                      // 0x010 R 0x0000 Response0
//	volatile unsigned short 	RESP1;                      // 0x012 R 0x0000 Response1
	volatile unsigned int	 	RESP1_0;					// 0x010  R  	Response0,1	
///

/////
//	volatile unsigned short 	RESP2;                      // 0x014 R 0x0000 Response2
//	volatile unsigned short 	RESP3;                      // 0x016 R 0x0000 Response3
	volatile unsigned int		RESP3_2;					// 0x014  R 	Response2,3
/////

//////	
//	volatile unsigned short 	RESP4;                      // 0x018 R 0x0000 Response4
//	volatile unsigned short 	RESP5;                      // 0x01A R 0x0000 Response5
	volatile unsigned int		RESP5_4;					// 0x018  R 	Response4,5
/////

//////
//	volatile unsigned short 	RESP6;                      // 0x01C R 0x0000 Response6
//	volatile unsigned short 	RESP7;                      // 0x01E R 0x0000 Response7
	volatile unsigned int		RESP7_6;					// 0x01C  R 	Response6,7
//////
	
	volatile unsigned short 	DATAL;                      // 0x020 R/W - Buffer Data Port(Low)
	volatile unsigned short 	DATAH;                      // 0x022 R/W - Buffer Data Port(High)
////
//	volatile unsigned short 	STATEL;                     // 0x024 R 0x0000 Present State(Low)
//	volatile unsigned short 	STATEH;                     // 0x026 R 0x0000 Present State(High)
	volatile unsigned int 		STATE; 						// 0x024  R  	Present State	
////

	volatile unsigned short 	CONTL;                      // 0x028 R/W 0x0000 Power Control / Host Control
	volatile unsigned short 	CONTH;                      // 0x02A R/W 0x0000 Wakeup Control / Block Gap Control
	volatile unsigned short 	CLK;                        // 0x02C R/W 0x0000 Clock Control
//////	
	volatile unsigned short 	TIME;                       // 0x02E R/W 0x0000 Software Reset / Timeout Control
//	volatile unsigned char	 	TIME;						// 0x02E  R/W	Software Reset / Timeout Control
//	volatile unsigned char	 	RESET;						// 0x02F  R/W	Software Reset / Timeout Control
//////	

/////////////
///	volatile unsigned short 	STSL;                       // 0x030 R 0x0000 Normal Interrupt Status(Low)
///	volatile unsigned short 	STSH;                       // 0x032 R 0x0000 Normal Interrupt Status(High)
	volatile unsigned int		STS;						// 0x030  R 	Normal Interrupt Status
/////////////
	
//////	
//	volatile unsigned short 	STSENL;                     // 0x034 R/W 0x0000 Normal Interrupt Status Enable(Low)
//	volatile unsigned short 	STSENH;                     // 0x036 R/W 0x0000 Normal Interrupt Status Enable(High)
	volatile unsigned int	 	STSEN; 						// 0x034  R/W	Normal Interrupt Status Enable	
/////
/////
//	volatile unsigned short 	INTENL;                     // 0x038 R/W 0x0000 Normal Interrupt Signal Enable(Low)
//	volatile unsigned short 	INTENH;                     // 0x03A R/W 0x0000 Normal Interrupt Signal Enable(High)
	volatile unsigned int	 	INTEN; 						// 0x038  R/W	Normal Interrupt Signal Enable	
/////	
	volatile unsigned short 	CMD12ERR;                   // 0x03C R 0x0000 Auto CMD12 Error Status
    volatile unsigned short     CONT2;                      // 0x03E R/W 0x0000 Host Control2	
	volatile unsigned short 	CAPL;                       // 0x040 R 0x30B0 Capabilities(Low)
	volatile unsigned short 	CAPH;                       // 0x042 R 0x69EF Capabilities(High)
	volatile unsigned short     NOTDEFINE3[2];              // 0x044, 0x046
	volatile unsigned short 	CURL;                       // 0x048 R 0x0001 Maximum Current Capabilities(Low)
	volatile unsigned short 	CURH;                       // 0x04A R 0x0000 Maximum Current Capabilities(High)
	volatile unsigned short     NOTDEFINE4[2];              // 0x04C, 0x04E
	volatile unsigned short 	FORCEL;                     // 0x050 W 0x0000 Force event for AutoCmd12 Error
	volatile unsigned short 	FORCEH;                     // 0x052 W 0x0000 Force event for Error Interrupt Status
	volatile unsigned short 	AUDIO_DMAERR;               // 0x054 R/W 0x0000 AUDIO DMA Error Status
	volatile unsigned short     NOTDEFINE5;                 // 0x056
	volatile unsigned short 	ADDR0;                      // 0x058 R/W 0x0000 AUDIO DMA Address[15:0]
	volatile unsigned short 	ADDR1;                      // 0x05A R/W 0x0000 AUDIO DMA Address[31:16]
	volatile unsigned short 	ADDR2;                      // 0x05C R/W 0x0000 AUDIO DMA Address[47:32]
	volatile unsigned short 	ADDR3;                      // 0x05E R/W 0x0000 AUDIO DMA Address[63:48]
	volatile unsigned short     NOTDEFINE6[78];             // 0x060~0x0FA
	volatile unsigned short 	SLOT;                       // 0x0FC R 0x0000 Slot Interrupt Status
	volatile unsigned short 	VERSION;                    // 0x0FE R 0x0002 Host Controller Version    
}SDHOST_T, *PSDHOST_T;

typedef struct _SDCHCTRL_T{
	volatile unsigned int	SDCTRL; 						// 0x00 R/W 0x0000	Host Controller Control Register
	volatile unsigned int	SD0CMDDAT;						// 0x04 R/W 0x0000	SD/MMC0 output delay control register
	volatile unsigned int	SD1CMDDAT;						// 0x08 R/W 0x0000	SD/MMC1 output delay control register
	volatile unsigned int	SD2CMDDAT;						// 0x0C R/W 0x0000	SD/MMC2 output delay control register
	volatile unsigned int	SD3CMDDAT;						// 0x10 R/W 0x0000	SD/MMC3 output delay control register
	volatile unsigned int	SD0PRESET1; 					// 0x14 R/W 0x0000	SD/MMC0 Preset Register1
	volatile unsigned int	SD0PRESET2; 					// 0x18 R/W 0x0000	SD/MMC0 Preset Register2
	volatile unsigned int	SD0PRESET3; 					// 0x1C R/W 0x0000	SD/MMC0 Preset Register3
	volatile unsigned int	SD0PRESET4; 					// 0x20 R/W 0x0000	SD/MMC0 Preset Register4
	volatile unsigned int	SD0PRESET5; 					// 0x24 R/W 0x0000	SD/MMC0 Preset Register5
	volatile unsigned int	SD0PRESET6; 					// 0x28 R/W 0x0000	SD/MMC0 Preset Register6
	volatile unsigned int	SD1PRESET1; 					// 0x2C R/W 0x0000	SD/MMC1 Preset Register1
	volatile unsigned int	SD1PRESET2; 					// 0x30 R/W 0x0000	SD/MMC1 Preset Register2
	volatile unsigned int	SD1PRESET3; 					// 0x34 R/W 0x0000	SD/MMC1 Preset Register3
	volatile unsigned int	SD1PRESET4; 					// 0x38 R/W 0x0000	SD/MMC1 Preset Register4
	volatile unsigned int	SD1PRESET5; 					// 0x3C R/W 0x0000	SD/MMC1 Preset Register5
	volatile unsigned int	SD1PRESET6; 					// 0x40 R/W 0x0000	SD/MMC1 Preset Register6
	volatile unsigned int	SD2PRESET1; 					// 0x44 R/W 0x0000	SD/MMC2 Preset Register1
	volatile unsigned int	SD2PRESET2; 					// 0x48 R/W 0x0000	SD/MMC2 Preset Register2
	volatile unsigned int	SD2PRESET3; 					// 0x4C R/W 0x0000	SD/MMC2 Preset Register3
	volatile unsigned int	SD2PRESET4; 					// 0x50 R/W 0x0000	SD/MMC2 Preset Register4
	volatile unsigned int	SD2PRESET5; 					// 0x54 R/W 0x0000	SD/MMC2 Preset Register5
	volatile unsigned int	SD2PRESET6; 					// 0x58 R/W 0x0000	SD/MMC2 Preset Register6
	volatile unsigned int	SD3PRESET1; 					// 0x5C R/W 0x0000	SD/MMC3 Preset Register1
	volatile unsigned int	SD3PRESET2; 					// 0x60 R/W 0x0000	SD/MMC3 Preset Register2
	volatile unsigned int	SD3PRESET3; 					// 0x64 R/W 0x0000	SD/MMC3 Preset Register3
	volatile unsigned int	SD3PRESET4; 					// 0x68 R/W 0x0000	SD/MMC3 Preset Register4
	volatile unsigned int	SD3PRESET5; 					// 0x6C R/W 0x0000	SD/MMC3 Preset Register5
	volatile unsigned int	SD3PRESET6; 					// 0x70 R/W 0x0000	SD/MMC3 Preset Register6

}SDCHCTRL_T, *PSDCHCTRL_T;


struct mmc_direct_req {
	u32 sector;			/* start sector */
	u32 nr_sectors;		/* # of sectors */
	u32 cmd_flags;		/* direction */
	void *buf;			/* read/write buf from FSG */
	dma_addr_t dma;		/* dma address of buf */
};

struct tcc_mmc_host {
	int initialized;
    int suspended;
    struct mmc_request *mrq;
    struct mmc_command *cmd;
    struct mmc_data *data;
    int data_early:1;				/* Data finished before cmd */

    struct mmc_host *mmc;
    struct device *dev;
    unsigned char id;				/* 16xx chips have 2 MMC blocks */
    unsigned long peri_clk;			/* Controller base clock */
	unsigned int clk_div;			/* base clock divider */
	struct clk *iclk;
	struct clk *fclk;
    struct resource	*res;
	void __iomem *base;
	u32 iobase;
    int	irq;
    unsigned char bus_mode;
    //unsigned char hw_bus_mode;

	char slot_desc[16]; 			/* Name for reservations */

    //unsigned int sg_len;
    //int sg_idx;
    //u16 *buffer;
    //u32 buffer_bytes_left;
    //u32 total_bytes_left;

    struct scatterlist *cur_sg;		/* We're working on this */
    int	num_sg;						/* Entries left */
    int	offset;						/* Offset into current sg */
    int	remain;						/* Bytes left in current */

	int sg_count;					/* Mapped sg entries */

	unsigned char *adma_desc;		/* ADMA descriptor table */
	unsigned char *align_buffer;	/* Bounce buffer */

	dma_addr_t adma_addr;			/* Mapped ADMA descr. table */
	dma_addr_t align_addr;			/* Mapped bounce buffer */

    //short wp_pin;

    short card_inserted;			/* to mark the card is inserted or not */
	short card_changed;
	int cd_irq;
	unsigned int card_insert;
	unsigned int oldstat;

    struct tasklet_struct finish_tasklet;

    spinlock_t lock;				/* Mutex */

    int flags;						/* Host attributes */
#define TCC_MMC_USE_DMA		(1<<0)	/* Host is DMA capable */
#define TCC_MMC_USE_ADMA	(1<<1)	/* Host is ADMA capable */
#define TCC_MMC_REQ_USE_DMA	(1<<2)	/* Use DMA for this req. */

    struct timer_list detect_timer;
    struct timer_list timer;

	int is_direct;
	struct mmc_direct_req *req;

	sector_t phys_nr_sects;			/* physical total sectors */
	sector_t phys_boot_sect;		/* bootloader start sector */
	sector_t phys_kern_sect;		/* kernel start sector */

	struct tcc_mmc_platform_data *pdata;	
};

#endif /*__TCC_SDHC_H__*/
