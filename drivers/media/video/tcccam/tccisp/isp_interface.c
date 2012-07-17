/************************************************************************
*    Telechips Multi Media Player
*    ------------------------------------------------
*
*    FUNCTION    : ISP INTERFACE API
*    MODEL        : DMP
*    CPU NAME    : TCC93XX
*    SOURCE        : TCC_ISP_API.c
*
*    START DATE    : 2010. 2. 19.
*    MODIFY DATE :
*    DEVISION    : DEPT. SYSTEM BT 3 TEAM
*                : TELECHIPS, INC.
* 
************************************************************************/

//#define NUCLEUS_OS
#include "isp_interface.h"

#ifdef CAMERA_INCLUDE  
//#include "sensor_api.h"
#ifdef FEATURE_TCC_ISP
#include "sensor_if.h"

#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
#include "cam_reg.h"
extern TCC_SENSOR_INFO_TYPE tcc_sensor_info;
extern void drv_set_camera_setting();
#endif
extern void drv_set_camera_capturesize_setting();
extern void drv_set_camera_previewsize_setting();
extern void *memcpy(void *dest, void *src, int n);
///////////////////////////////////////////////////
// etc
///////////////////////////////////////////////////
//#include "sensor/8M_Sony_IMX046ES.c"

HIsp 		g_hisp;
HEvent		g_hispevent;
HProfile 	g_hpreview;
HProfile 	g_hcapture;

DRVISPJpegInfo g_jpg_info;
extern DRVISPPreset *g_SensorPreset;

static unsigned char _g_hisp[DRVISP_HANDLER_SIZE];
static unsigned char _g_hispevent[DRVISP_HEVENT_SIZE];
static unsigned char _g_hpreview[DRVISP_HPROFILE_SIZE];
static unsigned char _g_hcapture[DRVISP_HPROFILE_SIZE];

static unsigned _g_drvispi2c = 0;

//#define CKC_REPORT
#ifdef CKC_REPORT

#define XIN (12*1000) // 12MHz
#define XTIN (32) // 32.768KHz
#define PLL4502     0
#define PLL4500B    1
typedef volatile unsigned int vuint;
vuint f_pll[6];
vuint f_plldiv[6];
vuint f_xtdiv;
vuint f_xdiv;
vuint f_cpu;
vuint f_ddi;
vuint f_mem;
vuint f_grp;
vuint f_peri;
vuint f_vid;
vuint f_vc;
vuint f_smu;
vuint f_hsio;
vuint f_cam;
vuint f_ds;
vuint f_pclk[46];

#define IO_OFFSET		0x40000000	/* Virtual IO = 0xB0000000 */
#define HwCKL_PTR(X)    (volatile unsigned long *)(0xB0500000+IO_OFFSET+(X))
#define HwCKL_REG(X)    *(volatile unsigned long *)(0xB0500000+IO_OFFSET+(X))

unsigned int func_report_ckc_calc_pll(unsigned int nData, unsigned int type) {
    unsigned int P, M, S, VSEL;
    unsigned int FREF, FVCO, FOUT;
    if ( (nData>>31) ) {
        switch (type) {
            case (PLL4502) :
                // get
                P = (nData>>0) & 0x3f;
                M = (nData>>8) & 0x3ff;
                S = (nData>>24) & 0x7;
                VSEL = (nData>>30) & 0x1;

                // calc
                FREF = (XIN/P);
                FVCO = (FREF*M);
                FOUT = (FVCO>>S);
                
                // check
                if ( ( FREF < 1000 ) || ( FREF > 10000 ) ) return (-1);
                if ( VSEL == 1 ) {
                    if ( FVCO <= 1400000 || (FVCO > 2000000 ) ) return (-1);
                } else {
                    if ( FVCO <= 1000000 || (FVCO > 1400000 ) ) return (-1);
                }
                if ( ( FOUT < 32000 ) || ( FOUT > 2000000 ) ) return (-1);

                // ok
                return (FOUT);
                break;
            case (PLL4500B) :
                P = (nData>>0) & 0x3f;
                M = (nData>>8) & 0x1ff;
                S = (nData>>24) & 0x7;
                VSEL = (nData>>30) & 0x1;

                // calc
                FREF = (XIN/P);
                FVCO = (FREF*M);
                FOUT = (FVCO>>S);
                
                // check
                if ( ( FREF < 1000 ) || ( FREF > 10000 ) ) return (-1);
                if ( VSEL == 1 ) {
                    if ( FVCO <= 460000 || (FVCO > 660000 ) ) return (-1);
                } else {
                    if ( FVCO <= 330000 || (FVCO > 460000 ) ) return (-1);
                }
                if ( ( FOUT < 12000 ) || ( FOUT > 660000 ) ) return (-1);

                // ok
                return (FOUT);
                break;
            default :
                return (-1);
                break;
        }
        return ( ((XIN/P)*M)>>S );
    } else {
        return(0); // pll off
    }
}

unsigned int func_report_ckc_get_pll(unsigned int sel) {
    switch (sel) {
        case(0) : return( func_report_ckc_calc_pll( HwCKL_REG(0x30), PLL4502 ) ); break;
        case(1) : return( func_report_ckc_calc_pll( HwCKL_REG(0x34), PLL4502 ) ); break;
        case(2) : return( func_report_ckc_calc_pll( HwCKL_REG(0x38), PLL4502 ) ); break;
        case(3) : return( func_report_ckc_calc_pll( HwCKL_REG(0x3C), PLL4500B) ); break;
        case(4) : return( func_report_ckc_calc_pll( HwCKL_REG(0x40), PLL4500B) ); break;
        case(5) : return( func_report_ckc_calc_pll( HwCKL_REG(0x44), PLL4500B) ); break;
        default : return (-1); break;
    }
}
unsigned int func_report_ckc_get_plldiv(unsigned int sel) {
    unsigned int nData;
    nData = HwCKL_REG(48);
    switch (sel) {
        case(0) : return( ((nData>>31)&0x1) ? ( f_pll[0]/((nData>>24)&0x3f) ) : 0 ); break;
        case(1) : return( ((nData>>23)&0x1) ? ( f_pll[1]/((nData>>16)&0x3f) ) : 0 ); break;
        case(2) : return( ((nData>>15)&0x1) ? ( f_pll[2]/((nData>>8 )&0x3f) ) : 0 ); break;
        case(3) : return( ((nData>>7 )&0x1) ? ( f_pll[3]/((nData>>0 )&0x3f) ) : 0 ); break;
        default : break;
    }
    nData = HwCKL_REG(50);
    switch (sel) {
        case(4) : return( ((nData>>31)&0x1) ? ( f_pll[4]/((nData>>24)&0x3f) ) : 0 ); break;
        case(5) : return( ((nData>>23)&0x1) ? ( f_pll[5]/((nData>>16)&0x3f) ) : 0 ); break;
        default : break;
    }
    return (-1);
}

unsigned int func_report_ckc_get_cpu(void) {
    unsigned int nData;
    unsigned int nCEN;
    unsigned int cnt;

    nData = HwCKL_REG(00);
    //if ( ((nData>>21)&0x1) == 0 ) return (0);
    nCEN = 0;
    for(cnt=0;cnt<16;cnt++) nCEN += (nData>>(4+cnt))&0x1;
    switch ( nData&0xf ) {
        case ( 0) : return( f_pll[0]    * nCEN / 16 ); break;
        case ( 1) : return( f_pll[1]    * nCEN / 16 ); break;
        case ( 2) : return( f_pll[2]    * nCEN / 16 ); break;
        case ( 3) : return( f_pll[3]    * nCEN / 16 ); break;
        case ( 4) : return( XIN         * nCEN / 16 ); break;
        case ( 5) : return( f_plldiv[0] * nCEN / 16 ); break;
        case ( 6) : return( f_plldiv[1] * nCEN / 16 ); break;
        case ( 7) : return( XTIN        * nCEN / 16 ); break;
        case ( 8) : return( f_pll[4]    * nCEN / 16 ); break;
        case ( 9) : return( f_pll[5]    * nCEN / 16 ); break;
        case (10) : return( f_plldiv[2] * nCEN / 16 ); break;
        case (11) : return( f_plldiv[3] * nCEN / 16 ); break;
        case (12) : return( f_plldiv[4] * nCEN / 16 ); break;
        case (13) : return( f_plldiv[5] * nCEN / 16 ); break;
        case (14) : return( f_xdiv      * nCEN / 16 ); break;
        case (15) : return( f_xtdiv     * nCEN / 16 ); break;
    }
    return (-1);
}

unsigned int func_report_ckc_get_bus(unsigned int nData) {
    if ( ((nData>>21)&0x1) == 0 ) return (0);
    switch ( nData&0xf ) {
        case ( 0) : return( f_pll[0]    / ( ( (nData>>4)&0xf ) + 1) ); break;
        case ( 1) : return( f_pll[1]    / ( ( (nData>>4)&0xf ) + 1) ); break;
        case ( 2) : return( f_pll[2]    / ( ( (nData>>4)&0xf ) + 1) ); break;
        case ( 3) : return( f_pll[3]    / ( ( (nData>>4)&0xf ) + 1) ); break;
        case ( 4) : return( XIN         / ( ( (nData>>4)&0xf ) + 1) ); break;
        case ( 5) : return( f_plldiv[0] / ( ( (nData>>4)&0xf ) + 1) ); break;
        case ( 6) : return( f_plldiv[1] / ( ( (nData>>4)&0xf ) + 1) ); break;
        case ( 7) : return( XTIN        / ( ( (nData>>4)&0xf ) + 1) ); break;
        case ( 8) : return( f_pll[4]    / ( ( (nData>>4)&0xf ) + 1) ); break;
        case ( 9) : return( f_pll[5]    / ( ( (nData>>4)&0xf ) + 1) ); break;
        case (10) : return( f_plldiv[2] / ( ( (nData>>4)&0xf ) + 1) ); break;
        case (11) : return( f_plldiv[3] / ( ( (nData>>4)&0xf ) + 1) ); break;
        case (12) : return( f_plldiv[4] / ( ( (nData>>4)&0xf ) + 1) ); break;
        case (13) : return( f_plldiv[5] / ( ( (nData>>4)&0xf ) + 1) ); break;
        case (14) : return( f_xdiv      / ( ( (nData>>4)&0xf ) + 1) ); break;
        case (15) : return( f_xtdiv     / ( ( (nData>>4)&0xf ) + 1) ); break;
    }
    return (-1);
}

unsigned int func_report_ckc_get_pclk(unsigned int nData) {
    unsigned int nDiv;
    if ( (nData>>29) & 0x1 ) {
        nDiv = ((nData>>0)&0xfff) + 1;
        if ( ((nData>>31)&0x1) == 1 ) return(-1); // MD - not yet implemented
        switch ( (nData>>24) & 0x1f ) {
            case ( 0) : return( ( f_pll[0])/nDiv ); break;
            case ( 1) : return( ( f_pll[1])/nDiv ); break;
            case ( 2) : return( ( f_pll[2])/nDiv ); break;
            case ( 3) : return( ( f_pll[3])/nDiv ); break;
            case ( 4) : return( ( XIN     )/nDiv ); break;
            case ( 5) : return( ( f_plldiv[0])/nDiv ); break;
            case ( 6) : return( ( f_plldiv[1])/nDiv ); break;
            case ( 7) : return( ( f_plldiv[2])/nDiv ); break;
            case ( 8) : return( ( f_plldiv[3])/nDiv ); break;
            case ( 9) : return( ( XTIN   )/nDiv ); break;
            case (10) : return( ( 0 )/nDiv ); break; // external clock
            case (11) : return( ( 0 )/nDiv ); break; // HDMI TMDS
            case (12) : return( ( 0 )/nDiv ); break; // HDMI PCLK
            case (13) : return( ( 27000 )/nDiv ); break; // HDMI Ref Clk
            case (14) : return( ( 0 )/nDiv ); break; // reserved
            case (15) : return( ( 48000 )/nDiv ); break; // USB0 PHY Clk
            case (16) : return( ( f_xdiv)/nDiv ); break;
            case (17) : return( ( f_xtdiv)/nDiv ); break;
            case (18) : return( ( f_pll[4])/nDiv ); break;
            case (19) : return( ( f_pll[5])/nDiv ); break;
            case (20) : return( ( f_plldiv[4])/nDiv ); break;
            case (21) : return( ( f_plldiv[5])/nDiv ); break;
            case (22) : return( ( 48000)/nDiv ); break; // USB1 PHY Clk
            case (23) : return( ( 0 )/nDiv ); break; // MIPI Pll
        }
    } else {
        return(0);
    }
    return(-1);
}

void func_report_ckc_get(void) {
    unsigned int i;
    unsigned int nData;

    for(i=0;i<6;i++) f_pll[i] = func_report_ckc_get_pll(i);
    for(i=0;i<6;i++) f_plldiv[i] = func_report_ckc_get_plldiv(i);
    nData = HwCKL_REG(0x4c);
    f_xdiv  = ((nData>>15)&0x1) ? ( XIN /((nData>>8)&0x3f) ) : 0;
    f_xtdiv = ((nData>>7 )&0x1) ? ( XTIN/((nData>>0)&0x3f) ) : 0;

    f_cpu   = func_report_ckc_get_cpu();
    f_ddi   = func_report_ckc_get_bus( HwCKL_REG(0x04) );
    nData = HwCKL_REG(0x2c);
    if ( ( nData & 0x1 ) == 1 ) { // async mode
        f_mem   = func_report_ckc_get_bus( HwCKL_REG(0x08) );
    } else {
        f_mem = f_cpu/( ( (nData>>8) & 0xff ) + 1 );
    }
    f_grp   = func_report_ckc_get_bus( HwCKL_REG(0x0c) );
    f_peri  = func_report_ckc_get_bus( HwCKL_REG(0x10) );
    f_vid   = func_report_ckc_get_bus( HwCKL_REG(0x14) );
    f_vc    = func_report_ckc_get_bus( HwCKL_REG(0x18) );
    f_smu   = func_report_ckc_get_bus( HwCKL_REG(0x1c) );
    f_hsio  = func_report_ckc_get_bus( HwCKL_REG(0x20) );
    f_cam   = func_report_ckc_get_bus( HwCKL_REG(0x24) );
    f_ds    = func_report_ckc_get_bus( HwCKL_REG(0x28) );

    for(i=0;i<46;i++) {
        nData = HwCKL_REG(0x80 + (4*i));
        f_pclk[i] = func_report_ckc_get_pclk(nData);
    }

}



#endif



int ISP_SensorLoad( DRVISPPreset*** pPreset)
{
	*pPreset = &g_SensorPreset;
	return 0;
}	

///////////////////////////////////////////////////
// Device interface
///////////////////////////////////////////////////

int DRV_ISPI2C_Init(void)
{
	//MSGPRINTF(" DRV_I2C_Init()\n");

	return 0;
}

int DRV_ISPI2C_Write( unsigned slaveaddr, unsigned subaddr, unsigned data, unsigned mode)
{
	// i2c로 센서 및 motor drive 제어용하는 용도로 사용한다.
	// 센서에 따라 burst모드를 지원할 경우 이를 적용한다.
	int iReturn = 0;
	MSGPRINTF(" DRV_I2C_Write()\n");

	return iReturn;
}

int DRV_ISPI2C_Read( unsigned slaveaddr, unsigned subaddr, unsigned *data, unsigned mode)
{
	// i2c로 센서 및 motor drive 제어용하는 용도로 사용한다.
	int iReturn = 0;
	MSGPRINTF(" DRV_I2C_Read()\n");

	return iReturn;
}

int DRV_ISPI2C_Exit( )
{
	// 종료시 호출
	MSGPRINTF(" DRV_I2C_Exit()\n");

	return 0;
}

int DRV_INTC_Enable( unsigned mask )
{
	// ISP interrupt enable
	return 0;
}
int DRV_INTC_Disable( unsigned mask )
{
	// ISP interrupt disable
	return 0;
}

int DRV_CKC_Isp (unsigned clk)
{

	return 0;	
}

// 센서 클럭 설정
int	DRV_SensorCkc(unsigned clk)
{
	MSGPRINTF(" isn't DRV_SensorCkc()\n");
	return 0;	
}

	// 센서의 reset pin을 조정
int DRV_SensorReset(unsigned enable)
{
	MSGPRINTF(" isn't DRV_SensorReset()\n");
	if(enable) // 할당된 gpio를 가 없고 port 특성을 모르기 때문에 임시로 
	{
		//SENSOR_RESET_HIGH();
		//NU_Sleep(1);
		//SENSOR_RESET_LOW();
		//NU_Sleep(1);
		//SENSOR_RESET_HIGH();
		//NU_Sleep(1);
	}
	else
	{
		MSGPRINTF(" test_mipi_isp()\n");
		//init_sensor();
		//test_mipi_isp(); 
	}
	return 0;
}

	// 센서의 standby pin을 조정
int DRV_SensorStandby(unsigned enable)
{
	MSGPRINTF(" isn't DRV_SensorStandby()\n");
//	if(enable)
//	{
//		SENSOR_PWRDN_ENABLE()
//	}
//	else
//	{
//		SENSOR_PWRDN_DISABLE()
//	}
	return 0;
}


int DRV_Flesh  (unsigned onoff)
{
	return 0;
}
int DRV_AFDrive(unsigned handle)
{
	return 0;
}
int DRV_Xenon  (unsigned handle)
{
	return 0;
}

// ISP state 전이시 호출 되는 함수들 
	// 처음 IO초기화 시 호출.
	// clk disable, 전원 차단. GPIO 셋팅
int DRV_Trans_Init(HIsp hisp)
{
	int err;
		
	DRV_CKC_Isp (0);
	DRV_INTC_Disable( 0xffffffff );
	DRV_SensorCkc(0);
	DRV_SensorReset(1);
	DRV_SensorStandby(1);


	//err = drvisp_setstatus (hisp, DRVISPST_START);
	return err;
}

int DRV_Trans_Exit(HIsp hisp)
{
	int err;

	DRV_CKC_Isp (0);		
	DRV_INTC_Disable( 0xffffffff );
	DRV_SensorCkc(0);
	DRV_SensorReset(1);
	DRV_SensorStandby(1);
	
	return err;
}
int DRV_Trans_TurnOn(HIsp hisp)
{
	int err;
	{
		// 주의 booting sequence와 해당 delay time 고려해서 코딩 
		DRV_SensorCkc(drvisp_getsensorclk(hisp));

		DRV_SensorReset(0);
		DRV_SensorStandby(0);
	}
	drvisp_loadpreset(hisp, DRVISPPRESET_INIT, 0, 0);

	return err;
}

int DRV_Trans_TurnOff(HIsp hisp)
{
	int err;

	//err = drvisp_setstatus (hisp, DRVISPST_START);

	{
		DRV_SensorCkc(0);
		DRV_SensorReset(1);
		DRV_SensorStandby(1);
	}
	drvisp_loadpreset(hisp, DRVISPPRESET_EXIT, 0, 0);	
	return err;
}

	// standby 상태에서 깨어남
int DRV_Trans_WakeUp(HIsp hisp)
{
	int err;
	
	{
		DRV_SensorCkc(0);
		DRV_SensorStandby(0);
	}

	//err = drvisp_setstatus (hisp, DRVISPST_IDLE);
	return err;
}

	// standby 모드로 진입
int DRV_Trans_Sleep(HIsp hisp)
{
	int err;
	//err = drvisp_setstatus (hisp, DRVISPST_POWERDOWN);

	{
		DRV_SensorCkc(0);
		DRV_SensorStandby(1);
	}

	return err;
}

	// sensor에서 data를 내보냄
	// isp에서 data를 받아 frame buffer 저장
int DRV_Trans_Stream(HIsp hisp)
{
	int err;

	if (drvisp_sensoridx(hisp)) // 0 : first sensor, 1: second sensor
	{
		DRV_CKC_Isp ( drvisp_getclk(hisp) );
	}
	else
	{
		DRV_CKC_Isp ( drvisp_getclk(hisp) );
	}
	//err = drvisp_setstatus (hisp, DRVISPST_STREAMING);
	return err;
}	

	// sensor는 정상 동작하나, 단지 data만 내보내지 않음.
	// mode 변환시 설정 값 변경시 불림
int DRV_Trans_Stop(HIsp hisp)
{
	int err;
	//err = drvisp_setstatus (hisp, DRVISPST_IDLE);

	DRV_CKC_Isp ( 0 );

	return err;
}

///////////////////////////////////////////////////
// Platform adaptation functions
///////////////////////////////////////////////////

// event handler
// mutex 
// task create / destroy
// memory allocation / free
// delay time 


int SAL_TimeDelay (int duration)
{

	return 0;
}

int SAL_TimeDelay_micro (int duration)
{

}
//inline unsigned SAL_getTickCount(unsigned *extra )
//{
//	return 0;
//}

///////////////////////////////////////////////////
// ISP_API
///////////////////////////////////////////////////


uint8 ISP_Init()
{
	int err;
	
	DRVISPCBFunc stcbfunc, *hcbfunc	;
	DRVISPPreset **hpreset;
	g_hisp = (HIsp) _g_hisp;
	g_hispevent = (HEvent) _g_hispevent;
	g_hpreview = (HProfile) _g_hpreview;
	g_hcapture = (HProfile) _g_hcapture;

	MSGPRINTF(" ISP_Init()\n");
	hcbfunc = &stcbfunc;
	hcbfunc->I2cInit = DRV_ISPI2C_Init;
	hcbfunc->I2cExit = DRV_ISPI2C_Exit;
	hcbfunc->I2cRead = DRV_ISPI2C_Read;
	hcbfunc->I2cWrite = DRV_ISPI2C_Write;
	hcbfunc->TransInit = DRV_Trans_Init;
	hcbfunc->TransExit = DRV_Trans_Exit;
	hcbfunc->TransTurnOn = DRV_Trans_TurnOn;
	hcbfunc->TransTurnOff = DRV_Trans_TurnOff;
	hcbfunc->TransWakeUp = DRV_Trans_WakeUp;
	hcbfunc->TransSleep = DRV_Trans_Sleep;
	hcbfunc->TransStream = DRV_Trans_Stream;
	hcbfunc->TransStop = DRV_Trans_Stop;
	hcbfunc->Flesh = DRV_Flesh;
#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	drv_set_camera_setting();
#endif	
	ISP_SensorLoad( &hpreset );

	err = drvisp_init( g_hisp, hpreset, hcbfunc );
	err = drvisp_makeevent( g_hisp, g_hispevent, 0);

	err = drvisp_makeprofile( g_hisp, g_hpreview, hpreset[DRVISPPRESET_PREVIEW],0);
	err = drvisp_makeprofile( g_hisp, g_hcapture, hpreset[DRVISPPRESET_CAPTURE],0);

#ifndef PARALLELCAMERA_MODE
	err = drvisp3A_init( g_hisp );
	err = drvispAdptvFunc_init( g_hisp );
#endif
	
    //drvisp_event3_setmask(DRVISPINT_IN_FRAME_END);
    //drvisp_event2_setmask(DRVISPINTMI_MP_FRAME_END | DRVISPINTMI_SP_FRAME_END);
    //drvisp_event4_setmask(DRVISPINTJPE_GEN_HEADER_DONE | DRVISPINTJPE_ENCODE_DONE);
/*
	drvispevent_maskmipi(g_hispevent, DRVISPINTMIPI_FRAME_END);
	drvispevent_maskmi(g_hispevent, DRVISPINTMI_MP_FRAME_END | DRVISPINTMI_SP_FRAME_END);
	drvispevent_maskisp(g_hispevent, DRVISPINT_IN_FRAME_END | DRVISPINT_OUT_FRAME_END 
							| DRVISPINT_EXP_END | DRVISPINT_AFM_FIN|DRVISPINT_PIC_ERR | DRVISPINT_DATA_LOSS 
							| DRVISPINT_AFM_LUM_OF | DRVISPINT_AFM_SUM_OF | DRVISPINT_ISP_OFF |DRVISPINT_VSTART |DRVISPINT_HSTART);	
*/
	drvispevent_maskmi(g_hispevent, DRVISPINTMI_MP_FRAME_END );
	drvispevent_maskisp(g_hispevent, DRVISPINT_IN_FRAME_END | DRVISPINT_OUT_FRAME_END 
  						| DRVISPINT_PIC_ERR | DRVISPINT_DATA_LOSS 
						| DRVISPINT_ISP_OFF |DRVISPINT_VSTART |DRVISPINT_HSTART);	
	
	/*drvispevent_maskjpe(g_hispevent, DRVISPINTJPE_VLC_TABLE_ERR 
							| DRVISPINTJPE_R2B_IMG_SIZE_ERR
							| DRVISPINTJPE_DCT_ERR
							| DRVISPINTJPE_VLC_SYMBOL_ERR
							| DRVISPINTJPE_ENCODE_DONE);
	*/						

	err = drvisp_setstatus (g_hisp, DRVISPST_IDLE);
	{
		unsigned mode = 0;
		mode = 	DRVISPACQ_MODE_PARALLEL;
#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
		if (tcc_sensor_info.v_sync_pol == ACT_HIGH)
		{
			mode |= DRVISPACQ_VSYNCPOL_HIGH;
		}
		else
		{
			mode |= DRVISPACQ_VSYNCPOL_LOW;
		}
		
		if(tcc_sensor_info.h_sync_pol == ACT_HIGH)
		{
			mode |= DRVISPACQ_HSYNCPOL_HIGH;
		}
		else
		{
			mode |= DRVISPACQ_HSYNCPOL_LOW;
		}
		
		if (tcc_sensor_info.p_clock_pol == NEGATIVE_EDGE)
		{
			mode |= DRVISPACQ_PCLKPOL_NEGA;
		}
		else
		{
			mode |= DRVISPACQ_PCLKPOL_POSI;
		}

#else
#ifdef	CAM_POLARITY_VSYNC
		if (!CAM_POLARITY_VSYNC)		mode |= DRVISPACQ_VSYNCPOL_HIGH;
		else						mode |= DRVISPACQ_VSYNCPOL_LOW;		
#endif
#ifdef 	CAM_POLARITY_HSYNC	
		if (!CAM_POLARITY_HSYNC)		mode |= DRVISPACQ_HSYNCPOL_HIGH;
		else						mode |= DRVISPACQ_HSYNCPOL_LOW;
#endif
#ifdef CAM_POLARITY_PCLK
		if (!CAM_POLARITY_PCLK)		mode |= DRVISPACQ_PCLKPOL_NEGA;
		else						mode |= DRVISPACQ_PCLKPOL_POSI;
#endif
#endif //CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
#ifdef	CAM_CCIRSEQ
		switch (CAM_CCIRSEQ)
		{
			case 0: mode |= DRVISPACQ_CCIRSEQ_YCBYCR; break;
			case 1: mode |= DRVISPACQ_CCIRSEQ_YCRYCB; break;
			case 2: mode |= DRVISPACQ_CCIRSEQ_CBYCRY; break;
			case 3: mode |= DRVISPACQ_CCIRSEQ_CRYCBY; break;
			default:
				break;
		}
#endif

#if defined(CAM_ISP_BT656)
		if (CAM_ISP_BT656==1)		mode |= DRVISPACQ_SYNC_BT656;
		else						mode |= DRVISPACQ_SYNC_BT601;
#endif

#ifdef	CAM_ISP_FIELD
		switch (CAM_ISP_FIELD)
		{
			case 0: mode |= DRVISPACQ_FIELD_ALL; break;
			case 1: mode |= DRVISPACQ_FIELD_EVEN; break;
			case 2: mode |= DRVISPACQ_FIELD_ODD; break;
			default:
				break;
		}
#endif

		drvispprofile_setacq (g_hpreview,mode,-1,-1,-1,-1);
		drvispprofile_setacq (g_hcapture,mode,-1,-1,-1,-1);
	}
	drvisp_setmode ( g_hisp, DRVISPMODE_PARALLEL_YUV , 0);

#ifdef CKC_REPORT
	func_report_ckc_get();
#endif
	
	return 0;
}

uint8 ISP_Exit()
{
	int err;
	MSGPRINTF(" ISP_Exit()\n");
	//err = DRV_Trans_TurnOff( g_hisp );
	err = drvisp_setstatus (g_hisp, DRVISPST_EMPTY);
#ifndef PARALLELCAMERA_MODE
	err = drvisp3A_exit( g_hisp );
	err = drvispAdptvFunc_exit( g_hisp );
#endif
	err = drvisp_exit( g_hisp );	
	return 0;
}

uint8 ISP_Check()
{
	int err,state;
	MSGPRINTF(" ISP_Check()\n");
	err = drvisp_getstatus (g_hisp, &state);

	while (err)
	{
		err = drvisp_reset_apply (g_hisp, 0);
	}
}

unsigned ISP_ChkStatus(int *stat, int count)
{
	int err = 0;
	//int count = 200;

	do {
 		int j;
		//for (j=0; j<10000; j++) ;	// for test
		msleep(10);
		err = drvisp_getstatus(g_hisp, stat);
		count --;
	} while ( err != 0 && (count != 0) );
	MSGPRINTF("ISP_GetStatus() status: %x, count : %d, err : %x \n",*stat,  count, err);	
	return err;
}

uint8 ISP_SetPreview_Control(uint8 status)
{
 	int err;
	int i=0;
	int stat;
	int count;
 	MSGPRINTF(" ISP_SetPreview_Control(%d)\n",status);
 	if (status == TCC_ISP_PREVIEW_STATUS_START)
 	{
 		err = drvisp_setprofile (g_hisp, g_hpreview, DRVISPAPPLY_MODECHANGE);
#ifndef PARALLELCAMERA_MODE
	#ifndef PREV8M
		ConvPreIntTime();		
	#endif
		CapToPre = 5;			//STEVE to prevent break screen
		AF_gpio_trigger = 4;
#endif
		err = drvisp_setstatus(g_hisp, DRVISPST_STREAMING);
		err = ISP_ChkStatus(&stat, 20);

		count = 5;
//		while ( (err & 0x80000000) && (count>0))
		while (err == 0x80000001 && (count>0))
		{
			err = drvisp_setstatus(g_hisp, DRVISPST_IDLE);
			err = ISP_ChkStatus(&stat,3);
	 		//{	int j;
			//	for (j=0; j<100000; j++) ;	// for test
			//}
			msleep(1);
			MSGPRINTF(" ISP_Capture_Shooting() retry\n");
			err = drvisp_setstatus(g_hisp, DRVISPST_STREAMING);		
			err = ISP_ChkStatus(&stat,20);
			count--;
		} 	
		
#ifndef PARALLELCAMERA_MODE
		drvisp3A_enable(g_hisp);  // STEVE 3A HOLD recovery
#endif
	}
	else
	{
#ifndef PARALLELCAMERA_MODE	
		drvisp3A_disable(g_hisp);
#endif
 		err = drvisp_setstatus(g_hisp, DRVISPST_IDLE);
		msleep(200);
		//err = ISP_ChkStatus(&stat);
 		//{	int j;
		//	for (j=0; j<100000; j++) ;	// for test
		//}
/*
		if (err == 0x80000001)
		{
			drvisp_reset_apply(g_hisp, 0);
		}
*/
	}	
 	return err; 
}

uint8 ISP_Capture_Shooting(int mode)
{
	int err;
	int status;
	int count;
	MSGPRINTF(" ISP_Capture_Shooting()\n");

#ifndef PARALLELCAMERA_MODE
	drvisp3A_disable(g_hisp);  // STEVE 3A HOLD
#endif	
	err = drvisp_setstatus(g_hisp, DRVISPST_IDLE);
	//err = ISP_ChkStatus(&status);
	msleep(200);

	if(mode)
		drv_set_camera_capturesize_setting();
	else
		drv_set_camera_previewsize_setting();

	err = drvisp_setprofile (g_hisp, g_hcapture, DRVISPAPPLY_MODECHANGE);
#ifndef PARALLELCAMERA_MODE
	ConvCapIntTime();
#endif
	//{	int j;
	//	for (j=0; j<300000; j++) ;	// for test
	//}
/*
	do
	{
		err = drvisp_setstatus(g_hisp, DRVISPST_STREAMING);
	} while (err == 0x80000001);
*/
	err = drvisp_setstatus(g_hisp, DRVISPST_STREAMING);
	err = ISP_ChkStatus(&status,50);
	
	count = 5;
	while (err == 0x80000001 && (count>0))
	{
		err = drvisp_setstatus(g_hisp, DRVISPST_IDLE);
		err = ISP_ChkStatus(&status, 6);
 		//{	int j;
		//	for (j=0; j<100000; j++) ;	// for test
		//}
		msleep(10);
		MSGPRINTF(" ISP_Capture_Shooting() retry\n");
		err = drvisp_setstatus(g_hisp, DRVISPST_STREAMING);		
		err = ISP_ChkStatus(&status,50);
		count--;

	} 	

	return err;	
}

///////////////////////////////////////////////////////////////////

uint8 ISP_SetPreviewH_Resolution(uint16 width, uint16 height)
{
	int err;	
	DRVISPRectSize size;

	size.width = width;
	size.height = height;
	
 	MSGPRINTF(" ISP_SetPreviewH() width=%d, height=%d\n", width, height);
	err = drvispprofile_setmpath_size (g_hpreview, &size, DRVISPAPPLY_MODECHANGE); 

 	return 0; 
}

uint8 ISP_SetPreviewH_Format(FORMAT format)
{
	int err;
	switch (format)
	{
		case ISP_FORMAT_YUV422:
	#ifndef HV_YUV422SEQ			
			err = drvispprofile_setmpath_type (g_hpreview, DRVISPDTYPE_YCbCr422 | DRVISPDTYPE_PLANAR, 0); 
	#else
			err = drvispprofile_setmpath_type (g_hpreview, DRVISPDTYPE_YCbCr422 | DRVISPDTYPE_INTERLEAVED, 0); 	
	#endif
			break;
		case ISP_FORMAT_YUV420:
			err = drvispprofile_setmpath_type (g_hpreview, DRVISPDTYPE_YCbCr420 | DRVISPDTYPE_PLANAR, 0); 
			break;
		case ISP_FORMAT_RGB565:
		case ISP_FORMAT_BAYER:
		case ISP_FORMAT_JPEG:
		default:
				// invalid format
			err = 1;
			break;
	}
	return 0;
}


uint8 ISP_SetPreviewH_StartAddress(uint32 *y, uint32 *u, uint32 *v)
{
	int err;
#ifdef TCCISP_GEN_CFG_UPD
	err = drvispprofile_setmfb_addr(g_hpreview , (void*) y, (void*) u, (void*) v, DRVISPAPPLY_ENDOFFRAME);
#else
	err = drvispprofile_setmfb_addr(g_hpreview , (void*) y, (void*) u, (void*) v, DRVISPAPPLY_IMMEDIATELY);
#endif
	return 0;
}

uint8 ISP_SetPreviewH_Size (uint32 szy, uint32 szu, uint32 szv)
{
	int err;
		
	err = drvispprofile_setmfb_size(g_hpreview , szy, szu, szv, DRVISPAPPLY_ENDOFFRAME);
	return 0;	
}

///////////////////////////////////////////////////////////////////

uint8 ISP_SetPreview_Resolution(uint16 width, uint16 height)
{
	int err;	
	DRVISPRegion region;
	region.offx = 0;
	region.offy = 0;
	region.width = width;
	region.height = height;
	region.xpitch = width;
	region.ypitch = height;
 	MSGPRINTF(" ISP_SetPreview_Resolution() width=%d, height=%d\n", width, height);
	err = drvispprofile_setspath_region (g_hpreview, &region, DRVISPAPPLY_MODECHANGE); 
 	return 0; 
}

uint8 ISP_SetPreview_Format(FORMAT format)
{
	int err;
	switch (format)
	{
		case ISP_FORMAT_YUV422:
	#ifndef HV_YUV422SEQ			
			err = drvispprofile_setspath_type (g_hpreview, DRVISPDTYPE_YCbCr422 | DRVISPDTYPE_PLANAR, 0); 
	#else
			err = drvispprofile_setspath_type (g_hpreview, DRVISPDTYPE_YCbCr422 | DRVISPDTYPE_INTERLEAVED, 0); 	
	#endif
			break;
		case ISP_FORMAT_YUV420:
			err = drvispprofile_setspath_type (g_hpreview, DRVISPDTYPE_YCbCr420 | DRVISPDTYPE_PLANAR, 0); 
			break;
		case ISP_FORMAT_RGB565:
			err = drvispprofile_setspath_type (g_hpreview, DRVISPDTYPE_RGB565, 0); 
			break;
		case ISP_FORMAT_BAYER:
		case ISP_FORMAT_JPEG:
		default:
				// invalid format
			break;
	}
	return 0;
}


uint8 ISP_SetPreview_StartAddress(uint32 *y, uint32 *u, uint32 *v)
{
	int err;
	err = drvispprofile_setsfb_addr(g_hpreview , (void*) y, (void*) u, (void*) v, DRVISPAPPLY_ENDOFFRAME);
	return 0;
}

uint8 ISP_SetPreview_Size (uint32 szy, uint32 szu, uint32 szv)
{
	int err;
	err = drvispprofile_setsfb_size(g_hpreview , szy, szu, szv, DRVISPAPPLY_ENDOFFRAME);
	return 0;	
}


///////////////////////////////////////////////////////////////////

uint8 ISP_SetCapture_MainResolution(uint16 width, uint16 height)
{
	int err;	
	DRVISPRectSize size;

	size.width = width;
	size.height = height;
	
 	MSGPRINTF(" ISP_SetCapture_MainResolution() width=%d, height=%d\n", width, height);
	err = drvispprofile_setmpath_size (g_hcapture, &size, DRVISPAPPLY_MODECHANGE); 
 	return 0; 
}

uint8 ISP_SetCapture_MainFormat(FORMAT format)
{
	int err;
	switch (format)
	{
		case ISP_FORMAT_YUV422:
			#ifndef HV_YUV422SEQ			
				err = drvispprofile_setmpath_type (g_hcapture, DRVISPDTYPE_YCbCr422 | DRVISPDTYPE_PLANAR, 0); 
			#else
				err = drvispprofile_setmpath_type (g_hcapture, DRVISPDTYPE_YCbCr422 | DRVISPDTYPE_INTERLEAVED, 0); 	
			#endif
			break;
		case ISP_FORMAT_YUV420:
			err = drvispprofile_setmpath_type (g_hcapture, DRVISPDTYPE_YCbCr420 | DRVISPDTYPE_PLANAR, 0); 
			break;
		case ISP_FORMAT_BAYER:
			err = drvispprofile_setmpath_type (g_hcapture, DRVISPDTYPE_RAW12, 0); 
			break;
		case ISP_FORMAT_JPEG:
			err = drvispprofile_setmpath_type (g_hcapture, DRVISPDTYPE_JPEG, 0); 
			break;
		case ISP_FORMAT_RGB565:
		default:
				// invalid format
			err = 1;
			break;
	}
	return 0;
}

uint8 ISP_SetCapture_MainImageStartAddress(uint32 *y, uint32 *u, uint32 *v)
{
	int err;
	err = drvispprofile_setmfb_addr(g_hcapture , (void*) y, (void*) u, (void*) v, DRVISPAPPLY_ENDOFFRAME);
	return 0;
}


uint8 ISP_SetCapture_MainImageSize (uint32 szy, uint32 szu, uint32 szv)
{
	int err;
		
	err = drvispprofile_setmfb_size(g_hcapture , szy, szu, szv, DRVISPAPPLY_ENDOFFRAME);
	return 0;	
}


///////////////////////////////////////////////////////////////////

uint8 ISP_SetCapture_ThumbResolution(uint16 width, uint16 height)
{
	int err;	
	DRVISPRegion region;
	region.offx = 0;
	region.offy = 0;
	region.width = width;
	region.height = height;
	region.xpitch = width;
	region.ypitch = height;
 	MSGPRINTF(" ISP_SetCapture_ThumbResolution() width = %d, height=%d\n", width, height);
	err = drvispprofile_setspath_region (g_hcapture, &region, DRVISPAPPLY_MODECHANGE); 	
 	return 0; 
}
uint8 ISP_SetCapture_ThumbFormat(FORMAT format)
{
	int err;
	switch (format)
	{
		case ISP_FORMAT_YUV422:
			#ifndef HV_YUV422SEQ			
				err = drvispprofile_setspath_type (g_hcapture, DRVISPDTYPE_YCbCr422 | DRVISPDTYPE_PLANAR, 0); 
			#else
				err = drvispprofile_setspath_type (g_hcapture, DRVISPDTYPE_YCbCr422 | DRVISPDTYPE_INTERLEAVED, 0); 	
			#endif
			break;
		case ISP_FORMAT_YUV420:
			err = drvispprofile_setspath_type (g_hcapture, DRVISPDTYPE_YCbCr420 | DRVISPDTYPE_PLANAR, 0); 
			break;
		case ISP_FORMAT_RGB565:
			err = drvispprofile_setspath_type (g_hcapture, DRVISPDTYPE_RGB565, 0); 
			break;
		case ISP_FORMAT_BAYER:
		case ISP_FORMAT_JPEG:
		default:
			// invalid format
			err = 1;
			break;
	}
	return 0;
}

uint8 ISP_SetCapture_ThumbImageStartAddress(uint32 *y, uint32 *u, uint32 *v)
{
	int err;
	err = drvispprofile_setsfb_addr(g_hcapture , (void*) y, (void*) u, (void*) v, DRVISPAPPLY_ENDOFFRAME);
	return 0;
}

uint8 ISP_SetCapture_ThumbImageSize (uint32 szy, uint32 szu, uint32 szv)
{
	int err;
		
	err = drvispprofile_setsfb_size(g_hcapture , szy, szu, szv, DRVISPAPPLY_ENDOFFRAME);
	return 0;	
}

uint8 ISP_getJpeg_ExifData(ISPJpegInfo* jpg_info)
{
	MSGPRINTF(" ISP_getJpeg_ExifData()\n");
	drvisp_getjpeginfo(g_hisp, 0,0,&g_jpg_info);
	memcpy(jpg_info, (ISPJpegInfo*)&g_jpg_info, sizeof(ISPJpegInfo));

	return 1;
}

uint8 ISP_SetCapture_MultiShotCount(uint8 count)
{
	int err;
	MSGPRINTF(" ISP_SetCapture_MultiShotCount()\n");
	err = drvispprofile_setmpath_number(g_hcapture, count, -1); 
	return 1;
}

uint8 ISP_Stop_Irq()
{
	int err;
	MSGPRINTF(" ISP_Stop_Irq()\n");	
	drvisp_stop_irq_apply(g_hisp);
	return 1;		
}


/* : not yet by lee k
uint32 ISP_GetCapture_MainImage_Length(void)
{
	uint32 length;
	MSGPRINTF(" ISP_GetCapture_MainImage_Length()\n");
	return length;	
}

uint32 ISP_GetCapture_ThumbImage_Length(void)
{
	uint32 length;
	MSGPRINTF(" ISP_GetCapture_ThumbImage_Length()\n");
	return length;
}

uint8 ISP_SetCapture_MultiShotCount(uint8 count)
{
	int err;
	MSGPRINTF(" ISP_SetCapture_MultiShotCount()\n");
	err = drvispprofile_setmpath_number(g_hcapture, count, -1); 
	return 1;
}

uint8 ISP_SetRollingCapture_JumpingLine(uint32 line_pixels)
{
	MSGPRINTF(" ISP_SetRollingCapture_JumpingLine()\n");
	return 1;
}

uint8 ISP_SetRollingCapture_InterruptLine(uint32 line_int)
{
	MSGPRINTF(" ISP_SetRollingCapture_InterruptLine()\n");
	return 1;
}

uint8 ISP_SetJpegQuality(JPEG_QUALITY quality)
{
	int err;
	MSGPRINTF(" ISP_SetJpegQuality()\n");
	err = drvispprofile_setjpeg(_g_hcapture, quality, 0);
	return 1;
}

uint8 ISP_getJpeg_ExifData(void* raw_data)
{
	MSGPRINTF(" ISP_getJpeg_ExifData()\n");
	return 1;
}

not yet by lee k */


/*****************************************************************
  ISP COMMON LIST  
*****************************************************************/  
uint8 ISP_SetPreview_Window(uint16 sx, uint16 sy, uint16 width, uint16 height)
{
	int err;
	DRVISPRect window;
	window.offx = sx;
	window.offy = sy;
	window.width = width;
	window.height = height;
	MSGPRINTF(" ISP_SetPreviewWindow()\n");
	err = drvispprofile_setwindow(g_hpreview, &window , DRVISPAPPLY_ENDOFFRAME);			
	return 0;
}

uint8 ISP_SetCapture_Window(uint16 sx, uint16 sy, uint16 width, uint16 height)
{
	int err;
	DRVISPRect window;
	window.offx = sx;
	window.offy = sy;
	window.width = width;
	window.height = height;
	MSGPRINTF(" ISP_SetCaptureWindow()\n");
	err = drvispprofile_setwindow(g_hcapture, &window , DRVISPAPPLY_ENDOFFRAME);			
	return 0;
}

static int s_zoomsteplimit = -1;

uint8 ISP_SetCapture_MakeZoomTable(uint16 stepx, uint16 stepy, uint16 width, uint16 height, uint16 number)
{
	int err;
	int i;
	DRVISPRect zoom;

	MSGPRINTF(" ISP_SetCapture_MakeZoomTable(%d,%d, %d, %d, %d \n)", stepx, stepy, width, height, number);

	for (i=0; i < number; i++)
	{
		zoom.offx =		stepx*i;
		zoom.offy =		stepy*i;
		zoom.width =	width - zoom.offx*2;
		zoom.height =	height - zoom.offy*2;
		
		err = drvispprofile_setzoomtbl(g_hcapture, i, &zoom, 0);	
		if (err)	break;			
	}
	return 0;
}

uint8 ISP_SetPreview_MakeZoomTable(uint16 stepx, uint16 stepy, uint16 width, uint16 height, uint16 number)
{
	int err = 0;
	int i;
	DRVISPRect zoom;

	MSGPRINTF(" ISP_SetPreview_MakeZoomTable(%d,%d, %d, %d, %d \n)", stepx, stepy, width, height, number);	
	for (i=0; i < number; i++)
	{
		zoom.offx =		stepx*i;
		zoom.offy =		stepy*i;
		zoom.width =	width - zoom.offx*2;
		zoom.height =	height - zoom.offy*2;
		
		err = drvispprofile_setzoomtbl(g_hpreview, i, &zoom, 0);
		if (err) break;
		#if 0
		{
			int j;
			unsigned short *pdata;
			pdata = &g_hpreview->sctbl[0];				
			dbg_printf("0x%04x ",i);
			for (j=0; j< 24; j++)
			{

				dbg_printf("0x%04x ",*pdata);
			}
			dbg_printf("\n");
		}		
		#endif
#if 0
		if (err & 0x4000F0F0)	break;			
#endif
	}
#if 0
	if (err & 0x4000F0F0)
	{
		int maxstep = i-1;
		if (maxstep>=0)
		{
			zoom.offx =		stepx*i;
			zoom.offy =		stepy*i;
			zoom.width =	width - zoom.offx*2;
			zoom.height =	height - zoom.offy*2;
			for (; i < number; i++)
			{
				err = drvispprofile_setzoomtbl(g_hpreview, i, &zoom, 0);
			}
		}
		else
		{
			return 1;	// error occured
		}
	}
#endif
	return 0;
}
/*
uint8 ISP_SetPreview_Zoom_Apply (int mode)
{
	int err;
	MSGPRINTF(" ISP_SetPreview_Zoom_Apply\n");
	err = drvispprofile_zoom_apply(g_hpreview, mode);
	return 0;

}

uint8 ISP_SetCapture_Zoom_Apply (int mode)
{
	int err;
	MSGPRINTF(" ISP_SetCapture_Zoom_Apply\n");
	err = drvispprofile_zoom_apply(g_hcapture, mode);
	return 0;

}
*/
uint8 ISP_Zoom_Apply (int mode)
{
	int err;
	MSGPRINTF(" ISP_Zoom_Apply (mode:%d)\n",mode);
	err = drvisp_zoom_apply(g_hisp, mode);
	return 0;
}
uint8 ISP_SetPreview_Zoom(uint16 zoom)
{
	int err;
	MSGPRINTF(" ISP_SetPreview_Zoom(%d)\n",zoom);
	#ifdef TCCISP_GEN_CFG_UPD
	err = drvispprofile_setzoom(g_hpreview, zoom, DRVISPAPPLY_LATER);	
	#else
		err = drvispprofile_setzoom(g_hpreview, zoom, DRVISPAPPLY_MODECHANGE);		
	#endif
	return 0;
}

uint8 ISP_SetCapture_Zoom(uint16 zoom)
{
	int err;
	MSGPRINTF(" ISP_SetCapture_Zoom(%d)\n",zoom);
	err = drvispprofile_setzoom(g_hcapture, zoom, DRVISPAPPLY_MODECHANGE);	
	return 0;
}

uint8 ISP_SetContrast(uint8 level)
{
	int err;
	MSGPRINTF(" ISP_SetContrast(%d)\n",level);
	
	err = drvispprofile_setcontrast(g_hcapture, level, DRVISPAPPLY_MODECHANGE );
	err = drvispprofile_setcontrast(g_hpreview, level, DRVISPAPPLY_ENDOFFRAME );

	return 0;
}

uint8 ISP_SetBrightness(uint8 level)
{
	int err;
	MSGPRINTF(" ISP_SetBrightness(%d)\n",level);
	
	err = drvispprofile_setbrightness(g_hcapture, level, DRVISPAPPLY_MODECHANGE );
	err = drvispprofile_setbrightness(g_hpreview, level, DRVISPAPPLY_ENDOFFRAME );

	return 0;
}

uint8 ISP_SetSaturation(uint8 level)
{
	int err;
	MSGPRINTF(" ISP_SetSaturation(%d)\n",level);
	
	err = drvispprofile_setsaturation(g_hcapture, level, DRVISPAPPLY_MODECHANGE );
	err = drvispprofile_setsaturation(g_hpreview, level, DRVISPAPPLY_ENDOFFRAME );

	return 0;
}

uint8 ISP_SetHue(uint8 level)
{
	int err;
	MSGPRINTF(" ISP_SetHue(%d)\n",level);
	
	err = drvispprofile_sethue(g_hcapture, level, DRVISPAPPLY_MODECHANGE );
	err = drvispprofile_sethue(g_hpreview, level, DRVISPAPPLY_ENDOFFRAME );

	return 0;
}

uint8 ISP_SetImg_Effect(IMAGE_EFFECT effectID)
{
	int err;
	MSGPRINTF(" ISP_SetImg_Effect(%d)\n",effectID);
	err = drvispprofile_setie(g_hcapture, effectID, 0, 0, DRVISPAPPLY_MODECHANGE);
	err = drvispprofile_setie(g_hpreview, effectID, 0, 0, DRVISPAPPLY_ENDOFFRAME);

	return 1;
}

/* : not yet by lee k

uint8 ISP_SetExposure_Mode(EXPOSURE_MODE_TYPE mode)
{
	MSGPRINTF(" ISP_SetExposure_Mode()\n");
	return 1;
}

uint8 ISP_SetAutoExposure_Windowtype(AE_WINTYPE type)
{
	MSGPRINTF(" ISP_SetAutoExposure_Windowtype()\n");
	return 1;
}

uint8 ISP_SetISO(AE_ISO iso)
{
	MSGPRINTF(" ISP_SetISO()\n");
	return 1;
}

uint8 ISP_SetAWB(AWB_MODE mode)
{
	MSGPRINTF(" ISP_SetAWB()\n");
	return 1;
}

uint8 ISP_AE_AWB_Lock(uint8 AE_enable, uint8 AWB_enable)
{
	MSGPRINTF(" ISP_AE_AWB_Lock()\n");
	return 1;
}

uint8 ISP_SetAutoFocus_Mode(AF_MODE_TYPE mode)
{
	MSGPRINTF(" ISP_SetAutoFocus_Mode()\n");
	return 1;
}

uint8 ISP_SetAutoFocus_Control(uint8 ctrl_cmd)
{
	MSGPRINTF(" ISP_SetAutoFocus_Control()\n");
	return 1;
}

AF_STATUS_RESULT ISP_GetAutoFocus_Status(void)
{
	AF_STATUS_RESULT result;
	MSGPRINTF(" ISP_GetAutoFocus_Status()\n");
	return result;
}

uint8 ISP_SetSceneMode(SCENE_MODE scene)
{
	MSGPRINTF(" ISP_SetSceneMode()\n");
	return 1;
}

uint8 ISP_SetWDR(uint8 enable)
{
	MSGPRINTF(" ISP_SetWDR()\n");
	return 1;
}



uint8 ISP_SetFlickerMode(FLICKER_MODE mode)
{
	MSGPRINTF(" ISP_SetFlickerMode()\n");
	return 1;
}

LIGHT_COND ISP_GetLowlight_Condition(void)
{
	LIGHT_COND result;
	MSGPRINTF(" ISP_GetLowlight_Condition()\n");	
	return result;
}

uint8 ISP_SetColorTemperature(LIGHT_TEMP Temp)
{
	MSGPRINTF(" ISP_SetColorTemperature()\n");
	return 1;
}

LIGHT_TEMP ISP_GetColorTemperature(void)
{
	LIGHT_TEMP result;
	MSGPRINTF(" ISP_GetColorTemperature()\n");
	return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
uint8 ISP_SetSyncPolarity(uint8 Hsync, uint8 Vsync)
{
	MSGPRINTF(" ISP_SetSyncPolarity()\n");
	return 1;
}


uint8 ISP_SetRotate(ROTATE_TYPE Rotate_type)
{
	MSGPRINTF(" ISP_SetRotate()\n");
	return 1;
}

uint8 ISP_SetSharpness(uint8 level)
{
	MSGPRINTF(" ISP_SetSharpness()\n");
	return 1;
}

uint8 ISP_SetSmooth(uint8 level)
{
	MSGPRINTF(" ISP_SetSmooth()\n");
	return 1;
}

uint8 ISP_SetSaturation(uint8 level)
{
	MSGPRINTF(" ISP_SetSaturation()\n");
	return 1;
}

uint8 ISP_SetContrast(uint8 level)
{
	MSGPRINTF(" ISP_SetContrast()\n");
	return 1;
}

void ISP_SetNoiseReduction(uint8 onoff)
{
	MSGPRINTF(" ISP_SetNoiseReduction()\n");
}

void ISP_SetHistogram(uint8 onoff)
{
	MSGPRINTF(" ISP_SetHistogram()\n");
}

void ISP_GetHistogram_values(uint8 * value[])
{
	MSGPRINTF(" ISP_GetHistogram_values()\n");
}

uint8 ISP_SetFlashControl(FLASH_CTRL_TYPE flash_ctrl)
{
	MSGPRINTF(" ISP_SetFlashControl()\n");
	return 1;
}

//////////// ????????????????????????????????????????????????
//////////// ????????????????????????????????????????????????

uint8 ISP_CheckStatusForCmd(void)
{
	MSGPRINTF(" ISP_CheckStatusForCmd()\n");
	return 1;
}


uint8 ISP_SetFaceDetection(FD_STATUS status)
{
	MSGPRINTF(" ISP_SetFaceDetection()\n");
	return 1;
}

uint8 ISP_SetImageStabilizer_Mode(ISC_MODE mode)
{
	MSGPRINTF(" ISP_SetImageStabilizer_Mode()\n");
	return 1;
}

uint8 ISP_SetImageStabilizer_Count(uint8 count)
{
	MSGPRINTF(" ISP_SetImageStabilizer_Count()\n");
	return 1;
}
 not yet by lee k  */

///////////////////////////////////////////////////
// ISP process
///////////////////////////////////////////////////

// MASK_ISPINT_OUT_FRAME_END event가 발생하면 즉각 실행한다.
int ISP_Process(void)
{
	int err;
#ifndef PARALLELCAMERA_MODE
	
	// todo : 순서를 확실히 
	if (drvispevent_ctrlisp(g_hispevent,  DRVISPINT_EXP_END))
	{		
		err = drvisp3A_process(g_hisp, 1, 0, 0);
		err = drvispAdptvFunc_process(g_hisp, 1, 0, 0);
	}

	if (drvispevent_ctrlisp(g_hispevent,  DRVISPINT_IN_FRAME_END))
	{		
		err = drvisp3A_process(g_hisp, 0, 0, 0);
		err = drvispAdptvFunc_process(g_hisp, 0,0,0);
	}
#endif	
	return 0;
}

//int ISPTask_Exit(void)
//{
//
//}

///////////////////////////////////////////////////
// Event handler
///////////////////////////////////////////////////
#ifdef FEATURE_TCC_ISP
//extern int T3A_Run(unsigned a);

unsigned char test=0;
void HISR_ISP_INT2_Handler(void)
{
	ISP_Process();
}
#endif //FEATURE_TCC_ISP


// ISP interrupt의해 불리는 함수. 초기화시 연결요.
// for mipi
int ISP_EventHandler1_getEvent(unsigned event)
{
	return drvispevent_ctrlmipi(g_hispevent, event);
}
void ISP_EventHandler1(void)
{
	drvisp_irq1_noti( g_hisp );
}

// ISP interrupt의해 불리는 함수. 초기화시 연결요.
// for mi
int ISP_EventHandler2_getEvent(unsigned event)
{
	return drvispevent_ctrlmi(g_hispevent, event);
}
void ISP_EventHandler2(void)
{
	drvisp_irq2_noti( g_hisp );
#if 0
	if (drvispevent_ctrlmi(g_hispevent, DRVISPINTMI_MP_FRAME_END))
	{		
		// to do
		// main path로 image전송 완료. 응용에게 event를 보낸다.
	}
	if (drvispevent_ctrlmi(g_hispevent, DRVISPINTMI_SP_FRAME_END))
	{		
		// to do
		// self path로 image전송 완료. 응용에게 event를 보낸다.
#ifdef NUCLEUS_OS
		DEV_LCDC_SetImageBase(1/*LCD_LC*/, 2/*IMG_CH0*/, 
			HwISP(0x143c) + HwISP(0x1448) ,
			HwISP(0x1450) + HwISP(0x145c) ,
			HwISP(0x1460) + HwISP(0x146c) 	);
		DEV_LCDC_SetImageCtrl(1/*LCD_LC*/, 0, 2/*IMG_CH0*/, 0);
#else  // NON_OS
#endif  // #ifdef NUCLEUS_OS
	}	
#endif	
}

extern unsigned int Tc_AF_FV_calc(void);
	
// ISP interrupt의해 불리는 함수. 초기화시 연결요.
// for isp
int ISP_EventHandler3_getEvent(unsigned event)
{
	return drvispevent_ctrlisp(g_hispevent, event);
}
void ISP_EventHandler3(void)
{	
	drvisp_irq3_noti( g_hisp );

	if (drvispevent_ctrlisp(g_hispevent, DRVISPINT_AFM_FIN))
	{		

	}
}

// ISP interrupt의해 불리는 함수. 초기화시 연결요.
// for jpeg
int ISP_EventHandler4_getEvent(unsigned event)
{
	return drvispevent_ctrljpe(g_hispevent, event);
}
void ISP_EventHandler4(void)
{
	drvisp_irq4_noti( g_hisp );
}

int ISP_CheckInterrupt()
{
	return drvisp_chkirq( g_hisp); 
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void ISP_SetInterrupt()
{
	MSGPRINTF(" ISP_SetInterrupt()\n");
#ifdef NUCLEUS_OS
	BITSET(HwPIC->IEN1, HwINT1_SPI0);    
	BITSET(HwPIC->SEL1, HwINT1_SPI0);

	BITSET(HwPIC->IEN1, HwINT1_SPI1);    
	BITSET(HwPIC->SEL1, HwINT1_SPI1);
	

	BITSET(HwPIC->IEN0, HwINT0_ISP2);    
	BITSET(HwPIC->SEL0, HwINT0_ISP2);

	BITSET(HwPIC->IEN1, HwINT1_ISP3);    
	BITSET(HwPIC->SEL1, HwINT1_ISP3);
#else

#endif	//#ifdef NUCLEUS_OS
}
	
void ISP_ClearInterrupt()
{
	MSGPRINTF(" ISP_clearInterrupt()\n");
#ifdef NUCLEUS_OS
	BITCLR(HwPIC->IEN1, HwINT1_SPI0);    
	BITCLR(HwPIC->IEN1, HwINT1_SPI1);    
	BITCLR(HwPIC->IEN0, HwINT0_ISP2);    
	BITCLR(HwPIC->IEN1, HwINT1_ISP3);    
#else

#endif	//#ifdef NUCLEUS_OS
	
	drvisp_clrirq(g_hisp);
	
}	

#endif 		//#ifdef FEATURE_TCC_ISP
#endif		// #ifdef CAMERA_INCLUDE  

