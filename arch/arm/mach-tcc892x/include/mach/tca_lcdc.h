
typedef enum{
	TCC_LCDC0,
	TCC_LCDC1,
	TCC_LCDC_MAX
}TCCLCDC_type;


// uiCS
#define	LCDSI_CS00 			0x00000001
#define	LCDSI_CS01 			0x00000002
#define LCDSI_CS10 			0x00000004
#define	LCDSI_CS11 			0x00000008

// uiRW
#define	LCDSI_WRITE 		0x00000001
#define	LCDSI_READ			0x00000002

// uiBW
#define LCDSI_BW_8BIT		0x00000000
#define LCDSI_BW_16BIT		0x00008000
#define LCDSI_BW_18BIT		0x80008000


#define	CPU_MODE			0
#define	LCDC_MODE			1

#define	IF80				0
#define	IF68				1


#define	FMT565_8BIT			0    
#define	FMT565_16BIT		14
#define	FMT888_8BIT			1    
#define	FMT888_9BIT			3    
#define	FMT888_16BIT_1		5
#define	FMT888_16BIT_2		7
#define	FMT666_16BIT_1		8
#define	FMT666_16BIT_2		10
#define	FMT666_16BIT_3		12
#define	FMT565_16BIT_SWAP	13

#define	LCDSI_RS_HIGH		1
#define	LCDSI_RS_LOW		0

#define	LCDSI_CS0			0
#define	LCDSI_CS1			1

#define SET_LCDSI_OUTPUTMODE		0x00000001
#define READ_LCDSI_MODE				0x00000002
#define SET_LCDSI_INPUTMODE			0x00000004
#define SET_LCDSI_FORMAT			0x00000008
#define SET_LCDSI_RESPONCE			0x00000010
#define SET_LCDSI_CS				0x00000020
#define SET_LCDSI_IA				0x00000040
#define SET_LCDSI_IVS				0x00000080
#define SET_LCDSI_ALL				0x000000FF


typedef unsigned int		uint32;

extern void DEV_LCDSI_DATA(uint32 uiCS, uint32 usData);


extern void DEV_LCDSI_BW_Set(unsigned int bus_width);


extern void DEV_LCDSI_CTRL_SET(TCCLCDC_type lcdc, uint32 LcdSi_reg);


extern uint32 DEV_LCDSI_CTRL (TCCLCDC_type lcdc, uint32 uiFlag, uint32 uiOM, uint32 uiIM, uint32 uiFmt, uint32 uiRsp, uint32 uiCs, uint32 uiIa, uint32 uiIvs);

extern void DEV_LCDSI_Time(unsigned int system_clk, unsigned int stp_ns, unsigned int pw_ns, unsigned int hld_ns);


extern unsigned char tcc_LCDC_Wait_signal_disable(char lcdc);
extern unsigned int tcc_LCDC_ctrl_layer(unsigned lcdc, unsigned layer, unsigned onoff);
extern unsigned int tcc_LCDC_set_alpha(unsigned lcdc, unsigned layer, unsigned alpha);
extern unsigned int tcc_LCDC_set_bgcolor(unsigned lcdc, unsigned red, unsigned green, unsigned blue);

extern  void tca_lcdc_interrupt_onoff(char onoff, char lcdc);
extern unsigned int DEV_LCDC_Wait_signal(char lcdc);
extern unsigned int DEV_LCDC_Wait_signal_Ext(void);
extern unsigned int DEV_LCDC_Status(char lcdc);
extern void tcc_LCDC_onoff_ctrl(unsigned int lcdc, unsigned int onoff);
extern unsigned int DEV_RDMA_Status(char lcdc);


#define ID_INVERT	0x01 	// Invered Data Enable(ACBIS pin)  anctive Low
#define IV_INVERT	0x02	// Invered Vertical sync  anctive Low
#define IH_INVERT	0x04	// Invered Horizontal sync	 anctive Low
#define IP_INVERT	0x08	// Invered Pixel Clock : anctive Low

