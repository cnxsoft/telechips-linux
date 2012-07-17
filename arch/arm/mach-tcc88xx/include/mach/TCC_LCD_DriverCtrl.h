
typedef enum{
	LCD_M_INITIALIZE,
	LCD_M_ON,
	LCD_M_OFF,
	LCD_M_MAX
}LCD_MODULE_CTRL;



// FPW + FSWC = VBP - 2
// FEWC = (VFP - 1)
// LPW = HAW - 1
// LPW + LSWC = HBP - 2
// LEWC = HFP - 1
typedef struct{
	unsigned int lcdc_clk;		// pixel clock
	unsigned int lcdc_clk_div;		// pixel clock divide
	unsigned int lcd_bus_width;	// data bus width
	unsigned int lcd_width;		// lcd width
	unsigned int lcd_height;	// lcd height
	unsigned int lcd_LPW;		// line pulse width
	unsigned int lcd_LPC;		// line pulse count (active horizontal pixel - 1)
	unsigned int lcd_LSWC;		// line start wait clock (the number of dummy pixel clock - 1)
	unsigned int lcd_LEWC;		// line end wait clock (the number of dummy pixel clock - 1)
	unsigned int lcd_VDB;		// Back porch Vsync delay
	unsigned int lcd_VDF;		// front porch of Vsync delay

	unsigned int lcd_FPW1;		// TFT/TV : Frame pulse width is the pulse width of frmae clock
	unsigned int lcd_FLC1;		// frmae line count is the number of lines in each frmae on the screen
	unsigned int lcd_FSWC1;		// frmae start wait cycle is the number of lines to insert at the end each frame
	unsigned int lcd_FEWC1;		// frame start wait cycle is the number of lines to insert at the begining each frame

	unsigned int lcd_FPW2;		// TFT/TV : Frame pulse width is the pulse width of frmae clock
	unsigned int lcd_FLC2;		// frmae line count is the number of lines in each frmae on the screen
	unsigned int lcd_FSWC2;		// frmae start wait cycle is the number of lines to insert at the end each frame
	unsigned int lcd_FEWC2;		// frame start wait cycle is the number of lines to insert at the begining each frame
	unsigned int SyncSignal_Invert;		// Invered sync signal 
}LCD_MODULE_SPEC_TYPE;

extern void DEV_LCD_MODULE_SPEC(LCD_MODULE_SPEC_TYPE* spec);

extern void DEV_LCD_Module (LCD_MODULE_CTRL uiControl);
