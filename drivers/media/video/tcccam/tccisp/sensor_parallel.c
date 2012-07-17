#include "sensor_if.h"
#include "isp_interface.h"

#ifdef SENSOR_PARALLEL

#define memory_test

#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
extern TCC_SENSOR_INFO_TYPE tcc_sensor_info;
#define SPREV_W			0
#define SPREV_H			0

#define SCAP_W			0
#define SCAP_H			0
#else
#define SPREV_W			PRV_W
#define SPREV_H			PRV_H

#define SCAP_W			CAP_W
#define SCAP_H			CAP_H
#endif

DRVISPPreset g_tcIspProfile_effect0[] = {	// normal
{0x0a00, 0x0200, 0x00000000},	
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};
DRVISPPreset g_tcIspProfile_effect1[] = {	// gray
{0x0a00, 0x0200, 0x00000001},
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};
DRVISPPreset g_tcIspProfile_effect2[] = {	// negative
{0x0a00, 0x0200, 0x00000003},
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};
DRVISPPreset g_tcIspProfile_effect3[] = {	// sephia
{0x0a00, 0x0200, 0x00000005},
{0x0a00, 0x021c, 0x0000880c},
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};
DRVISPPreset g_tcIspProfile_effect4[] = {	// aqua
{0x0a00, 0x0200, 0x00000005},
{0x0a00, 0x021c, 0x00000c88},
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};
DRVISPPreset g_tcIspProfile_effect5[] = {	// green
{0x0a00, 0x0200, 0x00000005},
{0x0a00, 0x021c, 0x00000000},
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};
DRVISPPreset g_tcIspProfile_effect6[] = {	// sketch
{0x0a00, 0x0200, 0x0000000B},
{0x0a00, 0x0210, 0x0000cccd},
{0x0a00, 0x0214, 0x0000ccbc},
{0x0a00, 0x0218, 0x000000cc},
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};
DRVISPPreset g_tcIspProfile_effect7[] = {	
{0x0a00, 0x0200, 0x00000000},		
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};
DRVISPPreset g_tcIspProfile_effect8[] = {
{0x0a00, 0x0200, 0x00000000},		
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};
DRVISPPreset g_tcIspProfile_effect9[] = {
{0x0a00, 0x0200, 0x00000000},		
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};

DRVISPPreset g_tcIspProfile_dpf0[] = {
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};

DRVISPPreset g_tcIspProfile_dpf1[] = {
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};
DRVISPPreset g_tcIspProfile_dpf2[] = {
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};

DRVISPPreset g_tcIspProfile_dpf3[] = {
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};
DRVISPPreset g_tcIspProfile_dpf4[] = {
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};

DRVISPPreset g_tcIspProfile_dpf5[] = {
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};
DRVISPPreset g_tcIspProfile_dpf6[] = {
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};

DRVISPPreset g_tcIspProfile_dpf7[] = {
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};
DRVISPPreset g_tcIspProfile_dpf8[] = {
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};

DRVISPPreset g_SensorInit[] = {
#ifdef memory_test
//=============================
// Marvin ISP Reset
//=============================

{0x0a00, 0x0014, 0x0080}, 	// ISP reset 

//=============================
// sensor  Setting
//=============================

{0x8001, 0, 	100},  

//=============================
// Marvin ISP Input size & SET.
//=============================

{0x0a00, 0x0410, SPREV_W*2},   // 1640 ISP_ACQ_H_SIZE
{0x0a00, 0x059c, SPREV_W},   // 1640 ISP_OUT_H_SIZE
{0x0a00, 0x2310, SPREV_W},   // 1640 ISP_IS_H_SIZE

{0x0a00, 0x0414, SPREV_H},   // 1230 ISP_ACQ_V_SIZE
{0x0a00, 0x05A0, SPREV_H},   // 1230 ISP_OUT_V_SIZE
{0x0a00, 0x2314, SPREV_H},   // 1230 ISP_IS_V_SIZE

// ISP_ACQ_PROP
// [10:9] FIELD_SELECTION
//			00 sample all fields
//			01 sample only even fields
//			10	sample only odd fields
//
// [8:7] CCIR_SEQ
//			00 YCbYCr
//			01 YCrYCb
//			10 CbYCrY
//			11 CrYCbY
//
// [2] VSYNC_POL	0:high active,	1:low active
// [1] HSYNC_POL	0:high active,	1:low active
// [0] SAMPLE_EDGE	0:negative edge,1:positive edge

{0x0a00, 0x0404, 0x0104},   // ISP_ACQ_PROP    
//{0x0a00, 0x0404, 0x0101},   // ISP_ACQ_PROP    
//{0x0a00, 0x0404, 0x0100},   // ISP_ACQ_PROP    

//{0x0a00, 0x0400, 0x6846},   // ISP_CTRL - Gamma ON, AWB OFF, ISP_INFORM OFF, ISP_ENABLE OFF, ISP_CFG_UPD
{0x0a00, 0x00000400, 0x00000005},    // ISP_CTRL - Gamma ON, AWB OFF, ISP_INFORM OFF, ISP_ENABLE OFF, ISP_CFG_UPD

//{0x8081, 0, 	 0xb0240000	},		// #define HwMIPI (0xB0240000)
//{0xca00, 0x0008, (8 << 18) 	},		// regw32(HwMIPI+0x8,(8 << 18) );
//{0xca00, 0x0004, 1  		},				// regw32(HwMIPI+0x4,1);
//{0x0a00, 0x1c00, 0x40001},  
//{0x0a00, 0x1c20, 0x2b},  

{0x0a00, 0x18, 0x0},  		// select mipi interface
//{0x0a00, 0x1400, 0x00B00008}, 	// raw 12bit cfg_upd 
//{0x0a00, 0x1400, 0x00000008},  	// raw 8bit cfg_upd

{0x0a00, 0x0c00, 0x100},		// Mrsz disable
{0x0a00, 0x1000, 0x100},		// Srsz disable
{0x0a00, 0x1400, 0x0},			// MI disable
{0x0a00, 0x1404, 0x10},

#endif // #ifdef memory_test

{0x80ff, 0xffff, 0xffffffff}   			//[END]
};

DRVISPPreset g_SensorExit[] = {
{0x8001, 0,  100},   
{0x8080, 0,  1},   //RESET	1 // Reset 
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};

DRVISPPreset g_SensorPreview[] = {
//{0x4434, 0x0100, 0x0},		// stream control

//=============================
// Marvin ISP Input size & SET.
//=============================
{0x0a00, 0x0410, SPREV_W*2},   // 1640 ISP_ACQ_H_SIZE
{0x0a00, 0x059c, SPREV_W},   // 1640 ISP_OUT_H_SIZE
{0x0a00, 0x2310, SPREV_W},   // 1640 ISP_IS_H_SIZE

{0x0a00, 0x0414, SPREV_H},   // 1230 ISP_ACQ_V_SIZE
{0x0a00, 0x05A0, SPREV_H},   // 1230 ISP_OUT_V_SIZE
{0x0a00, 0x2314, SPREV_H},   // 1230 ISP_IS_V_SIZE

//{0x0a00, 0x0404, 0x0104},   // ISP_ACQ_PROP    
{0x0a00, 0x00000400, 0x00000005},    // ISP_CTRL - Gamma ON, AWB OFF, ISP_INFORM OFF, ISP_ENABLE OFF, ISP_CFG_UPD

{0x8001, 0,  100},  	
//{0x4434, 0x0100, 0x1},	//stream out
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};

DRVISPPreset g_SensorCapture[] = {

//{0x4434, 0x0100, 0x0},		// stream control

//=============================
// Marvin ISP Input size & SET.
//=============================
{0x0a00, 0x0410, SCAP_W*2},   // 1640 ISP_ACQ_H_SIZE
{0x0a00, 0x059c, SCAP_W},   // 1640 ISP_OUT_H_SIZE
{0x0a00, 0x2310, SCAP_W},   // 1640 ISP_IS_H_SIZE

{0x0a00, 0x0414, SCAP_H},   // 1230 ISP_ACQ_V_SIZE
{0x0a00, 0x05A0, SCAP_H},   // 1230 ISP_OUT_V_SIZE
{0x0a00, 0x2314, SCAP_H},   // 1230 ISP_IS_V_SIZE

//{0x0a00, 0x0404, 0x0104},   // ISP_ACQ_PROP    
{0x0a00, 0x00000400, 0x00000005},    // ISP_CTRL - Gamma ON, AWB OFF, ISP_INFORM OFF, ISP_ENABLE OFF, ISP_CFG_UPD

{0x8001, 0,  100 },
//{0x4434, 0x0100, 0x1},	//stream out
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};

DRVISPPreset g_SensorVideo[] = {
{0x8001, 0,  100},   
{0x8080, 0,  1},   //RESET	1 // Reset 
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};

DRVISPPreset g_SensorDebug[] = {
{0x8001, 0,  100},   
{0x8080, 0,  1},   //RESET	1 // Reset 
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};

DRVISPPreset g_SensorStreamOut[] = {
//{0x4434, 0x0100, 0x1},	//stream out
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};

DRVISPPreset g_SensorStop[] = {
//{0x4434, 0x0100, 0x0},	//stream out
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};

DRVISPPreset g_SensorRreset[] = {
{0x8001, 0,  100},   
{0x8080, 0,  1},   //RESET	1 // Reset 
{0x80ff, 0xffff, 0xffffffff}   			//[END]
};

#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
void drv_set_camera_setting()
{

	g_SensorInit[2].data = tcc_sensor_info.preview_w*2;
	g_SensorInit[3].data = tcc_sensor_info.preview_w;
	g_SensorInit[4].data = tcc_sensor_info.preview_w;
	g_SensorInit[5].data = tcc_sensor_info.preview_h;
	g_SensorInit[6].data = tcc_sensor_info.preview_h;
	g_SensorInit[7].data = tcc_sensor_info.preview_h;

	g_SensorPreview[0].data = tcc_sensor_info.preview_w*2;
	g_SensorPreview[1].data = tcc_sensor_info.preview_w;
	g_SensorPreview[2].data = tcc_sensor_info.preview_w;
	g_SensorPreview[3].data = tcc_sensor_info.preview_h;
	g_SensorPreview[4].data = tcc_sensor_info.preview_h;
	g_SensorPreview[5].data = tcc_sensor_info.preview_h;

	g_SensorCapture[0].data = tcc_sensor_info.capture_w*2;
	g_SensorCapture[1].data = tcc_sensor_info.capture_w;
	g_SensorCapture[2].data = tcc_sensor_info.capture_w;
	g_SensorCapture[3].data = tcc_sensor_info.capture_h;
	g_SensorCapture[4].data = tcc_sensor_info.capture_h;
	g_SensorCapture[5].data = tcc_sensor_info.capture_h;
}

void drv_set_camera_capturesize_setting()
{
	g_SensorCapture[0].data = tcc_sensor_info.capture_w*2;
	g_SensorCapture[1].data = tcc_sensor_info.capture_w;
	g_SensorCapture[2].data = tcc_sensor_info.capture_w;
	g_SensorCapture[3].data = tcc_sensor_info.capture_h;
	g_SensorCapture[4].data = tcc_sensor_info.capture_h;
	g_SensorCapture[5].data = tcc_sensor_info.capture_h;
}

void drv_set_camera_previewsize_setting()
{
	g_SensorCapture[0].data = tcc_sensor_info.preview_w*2;
	g_SensorCapture[1].data = tcc_sensor_info.preview_w;
	g_SensorCapture[2].data = tcc_sensor_info.preview_w;
	g_SensorCapture[3].data = tcc_sensor_info.preview_h;
	g_SensorCapture[4].data = tcc_sensor_info.preview_h;
	g_SensorCapture[5].data = tcc_sensor_info.preview_h;
}
#else
void drv_set_camera_capturesize_setting()
{
	g_SensorCapture[0].data = SCAP_W*2;
	g_SensorCapture[1].data = SCAP_W;
	g_SensorCapture[2].data = SCAP_W;
	g_SensorCapture[3].data = SCAP_H;
	g_SensorCapture[4].data = SCAP_H;
	g_SensorCapture[5].data = SCAP_H;
}

void drv_set_camera_previewsize_setting()
{
	g_SensorCapture[0].data = SPREV_W*2;
	g_SensorCapture[1].data = SPREV_W;
	g_SensorCapture[2].data = SPREV_W;
	g_SensorCapture[3].data = SPREV_H;
	g_SensorCapture[4].data = SPREV_H;
	g_SensorCapture[5].data = SPREV_H;
}

#endif

DRVISPPreset *g_SensorPreset[] =
{
	g_SensorInit,
	g_SensorExit,
	g_SensorPreview,
	g_SensorCapture,
	g_SensorVideo,
	g_SensorDebug,
	g_SensorStreamOut,
	g_SensorStop,
	g_SensorRreset,
	
g_tcIspProfile_dpf0,
g_tcIspProfile_dpf1,
g_tcIspProfile_dpf2,
g_tcIspProfile_dpf3,
g_tcIspProfile_dpf4,
g_tcIspProfile_dpf5,
g_tcIspProfile_dpf6,
g_tcIspProfile_dpf7,
g_tcIspProfile_dpf8,

g_tcIspProfile_effect0,
g_tcIspProfile_effect1,
g_tcIspProfile_effect2,
g_tcIspProfile_effect3,
g_tcIspProfile_effect4,
g_tcIspProfile_effect5,
g_tcIspProfile_effect6,
g_tcIspProfile_effect7,
g_tcIspProfile_effect8,
g_tcIspProfile_effect9

};


#endif //#ifdef SENSOR_PARALLEL
