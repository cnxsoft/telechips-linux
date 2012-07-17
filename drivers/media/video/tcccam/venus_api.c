/*************************************************************************************************/
/* AIT ISP Host API                                                                              */
/* All rights reserved by Alpah Image Technology Corp                                            */
/*-----------------------------------------------------------------------------------------------*/
/* File name: Venus_API.c                                                                        */
/* Description: ISP Host API hardware independent layer                                          */
/*                                                                                               */
/* Version 0.0A   20090326                                                                       */
/*************************************************************************************************/
#if defined(CONFIG_ARCH_TCC92XX)
#include <mach/bsp.h>
#elif defined(CONFIG_ARCH_TCC79X)
#include <mach/tcc79x.h>
#endif
#include "tcc_cam_i2c.h"
#include <asm/delay.h>
#include <linux/delay.h>
#include "sensor_if.h"
#include "cam.h"


#include "venus_api.h"
#include "venus_hostcmd.h"
#include "venus_i2c_ctl.h"

static int debug = 0;
#define dprintk(msg...)	if (debug) { printk( "venus_api: " msg); }

#define VENUS_CMD_SPEEDUP


/*************************************************************************************************/
/* Venus : Enable/Disable Face Detection                                                         */
/*************************************************************************************************/
#define MAX_TIMEOUT_CNT			1000
#define DEBUG_FILE_PATH         "/SystemFS/Driver/Logfile.hex"
extern void VA_DelayMS(uint16 ms);
extern void delay_us(uint32 us);

uint8 VA_CheckReadyForCmd(void)
{
	uint32 timeout_count = 0;
	uint32 timeout_count2 = 0;
    uint16 Reg, Val, i;
	uint8 ret;

    VA_HIGH(0, "Call VA_CheckReadyForCmd\n");

	 //while ((receive_byte_via_i2c_ISP(HOST_STATUS_ADDR) & HOST_STATUS_READY) == 0 && timeout_count++ < MAX_TIMEOUT_CNT) {
	 while (((ret = receive_byte_via_i2c(HOST_STATUS_ADDR)) & HOST_STATUS_READY) == 0 && timeout_count++ < MAX_TIMEOUT_CNT) {   	
		delay_us(1000);
		//NU_Sleep(4);

		if(timeout_count % 1000 == 0){
			printk("ret = 0x%x \n", ret);
		}
		
        timeout_count2++;
        if ( timeout_count2 >= 5000 )
        {
              timeout_count2=0;
        }
    }
	
    if (timeout_count >= MAX_TIMEOUT_CNT) {
			printk( "VA_CheckReadyForCmd: Venus API Timeout!\n");
			Reg = 0x65F0;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x65F2;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x65F4;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x65F6;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x65F8;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x65FA;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x65FC;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x65FE;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x65E8;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
            
            for (i=0; i<0x46;i+=2)
            {
    			Reg = 0x6500+i;
    			Val = receive_word_via_i2c_ISP( Reg );
    			VA_HIGH(0, "Reg[%x] %x\n", Reg, Val);
            }
			Reg = 0x6580;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6582;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x65A0;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x65A4;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);

            for (i=0; i<0x62;i+=2)
            {
    			Reg = 0x6200+i;
    			Val = receive_word_via_i2c_ISP( Reg );
    			VA_ERR( "Reg[%x] %x\n", Reg, Val);
            }

            for (i=0; i<0x38;i+=2)
            {
    			Reg = 0x6000+i;
    			Val = receive_word_via_i2c_ISP( Reg );
    			VA_ERR( "Reg[%x] %x\n", Reg, Val);
            }

            for (i=0; i<0x3C;i+=2)
            {
    			Reg = 0x6F00+i;
    			Val = receive_word_via_i2c_ISP( Reg );
    			VA_ERR( "Reg[%x] %x\n", Reg, Val);
            }

            for (i=0x50; i<0x5D;i+=2)
            {
    			Reg = 0x6F00+i;
    			Val = receive_word_via_i2c_ISP( Reg );
    			VA_ERR( "Reg[%x] %x\n", Reg, Val);
            }
            /*
			Reg = 0x6500;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6504;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6510;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6514;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6540;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6544;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x65A0;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x65A4;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6200;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6202;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6204;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6246;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
            */
			Reg = 0x7900;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x7902;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x792C;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x791C;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x791E;
			Val = receive_word_via_i2c_ISP( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);

			return 1;
		}
    
	return 0;
}



/*************************************************************************************************/
/* Venus : Enable/Disable Face Detection                                                         */
/*************************************************************************************************/
uint8 VA_FaceDetectionEnable(void)
{
    uint8 Result=0;
    
	VA_HIGH(0, "VA_FaceDetectionEnable\n");

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_FD_ENABLE);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);

	return Result;
}

uint8 VA_FaceDetectionDisable(void)
{
    uint8 Result=0;
    
	VA_HIGH(0, "VA_FaceDetectionDisable\n");

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_FD_DISABLE);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

uint8 VA_FaceDetectionSetUnlock(void)
{
    uint8 Result=0;
    
	VA_HIGH(0, "VA_FaceDetectionSetUnlock\n");

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_FD_UNLOCK);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

/*************************************************************************************************/
/* Venus : Zoom Settings                                                                         */
/*************************************************************************************************/
uint8 VA_SetZoom(uint16 x)
{
    uint8 Result=0;
    
	dprintk("VA_SetZoom: X = %d\n", x);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, x & 0xFF);
	transmit_byte_via_i2c(HOST_PARAMETER_1, (x >> 8) & 0xFF);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_PREVIEW_ZOOM);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}


/*************************************************************************************************/
/* Venus : RawProc Store/Fetch                                                                   */
/*************************************************************************************************/
uint8 VA_SetRawProcStore(uint32 addr)
{
    uint8 Result=0;
    
	dprintk("-##- VA_SetRawProcStore: addr = 0x%08X\n", addr);

	Result = VA_CheckReadyForCmd();
	transmit_word_via_i2c(HOST_PARAMETER_0, addr & 0xFFFF);
	transmit_word_via_i2c(HOST_PARAMETER_2, addr >> 16);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_RAWPROC_STORE);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

uint8 VA_SetRawProcFetch(uint32 addr, uint8 type)
{
    uint8 Result=0;
    
	dprintk("-##- VA_SetRawProcFetch: addr = 0x%08X\n", addr);
	dprintk("-##- VA_SetRawProcFetch: type = %d\n", type);

	Result = VA_CheckReadyForCmd();
	transmit_word_via_i2c(HOST_PARAMETER_0, addr & 0xFFFF);
	transmit_word_via_i2c(HOST_PARAMETER_2, addr >> 16);
	transmit_byte_via_i2c(HOST_PARAMETER_4, type);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_RAWPROC_FETCH);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}


/*************************************************************************************************/
/* Venus : Snapshot                                                                              */
/*************************************************************************************************/
uint8 VA_SetSnapshotMode(uint8 mode)
{
    uint8 Result=0;
    
	dprintk( "VA_SetSnapshotMode: mode = %d\n", mode);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, mode);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_SNAPSHOT_MODE);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

uint8 VA_SetSnapshotMainResolution(uint16 width, uint16 height)
{
    uint8 Result=0;
    
	dprintk( "VA_SetSnapshotResolution: (Width x Height) = (%d x %d)\n", width, height);

	Result = VA_CheckReadyForCmd();
	transmit_word_via_i2c(HOST_PARAMETER_0, width);
	transmit_word_via_i2c(HOST_PARAMETER_2, height);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_SNAPSHOT_MAIN_RESOLUTION);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

uint8 VA_SetSnapshotThumbnailResolution(uint16 width, uint16 height)
{
    uint8 Result=0;
    
	dprintk( "VA_SetThumbnailResolution: (Width x Height) = (%d x %d)\n", width, height);

	Result = VA_CheckReadyForCmd();
	transmit_word_via_i2c(HOST_PARAMETER_0, width);
	transmit_word_via_i2c(HOST_PARAMETER_2, height);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_THUMBNAIL_RESOLUTION);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

// new API (to support different main image formats)
uint8 VA_SetSnapshotMainFormat(VENUS_SNAPSHOT_FORMAT format)
{
    uint8 Result=0;
    
	dprintk( "VA_SetSnapshotMainFormat: format = %d\n", format);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, format);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_SNAPSHOT_MAIN_FORMAT);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

// new API (to support different thumbnail formats)
uint8 VA_SetSnapshotThumbFormat(VENUS_SNAPSHOT_FORMAT format)
{
    uint8 Result=0;
    
	dprintk( "VA_SetSnapshotThumbFormat: format = %d\n", format);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, format);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_SNAPSHOT_THUMB_FORMAT);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

uint32 VA_GetSnapshotMainJPEGCompressedSize(void)
{
	uint32 ret;

	VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_GET_MAIN_JPEG_COMPRESSED_SIZE);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	VA_CheckReadyForCmd();

	ret = (uint32)receive_word_via_i2c(HOST_PARAMETER_0) + ((uint32)receive_word_via_i2c(HOST_PARAMETER_2) << 16);

	dprintk( "VA_GetSnapshotMainJPEGCompressedSize: Size = %d\n", ret);

	return ret;
}

uint32 VA_GetSnapshotThumbJPEGCompressedSize(void)
{
	uint32 ret;

	VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_GET_THUMB_JPEG_COMPRESSED_SIZE);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	VA_CheckReadyForCmd();

	ret = (uint32)receive_word_via_i2c(HOST_PARAMETER_0) + ((uint32)receive_word_via_i2c(HOST_PARAMETER_2) << 16);

	dprintk( "VA_GetSnapshotThumbJPEGCompressedSize: Size = %d\n", ret);

	return ret;
}

uint32 VA_GetSnapshotMainJPEGStartAddress(void)
{
	uint32 ret;

	VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_GET_MAIN_JPEG_START_ADDRESS);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	VA_CheckReadyForCmd();

	ret = (uint32)receive_word_via_i2c(HOST_PARAMETER_0) + ((uint32)receive_word_via_i2c(HOST_PARAMETER_2) << 16);

	dprintk( "VA_GetSnapshotMainJPEGStartAddress: Addr = 0x%08X\n", ret);

	return ret;
}

uint32 VA_GetSnapshotThumbJPEGStartAddress(void)
{
	uint32 ret;

	VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_GET_THUMB_JPEG_START_ADDRESS);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	VA_CheckReadyForCmd();

	ret = (uint32)receive_word_via_i2c(HOST_PARAMETER_0) + ((uint32)receive_word_via_i2c(HOST_PARAMETER_2) << 16);

	dprintk( "VA_GetSnapshotThumbJPEGStartAddress: Addr = 0x%08X\n", ret);

	return ret;
}


uint16 VA_GetLastSnapshotISOSpeed(void)
{
	uint16 ret;

	VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_GET_LAST_SHOT_ISO_SPEED);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	VA_CheckReadyForCmd();

	ret = (uint16)receive_word_via_i2c(HOST_PARAMETER_0);

	dprintk( "VA_GetLastSnapshotISOSpeed: ISO = 0x%04X\n", ret);

	return ret;
}


uint32 VA_GetLastSnapshotShutterSpeed(void)
{
	uint32 ret;

	VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_GET_LAST_SHOT_SHUTTER_SPEED);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	VA_CheckReadyForCmd();

	ret = (uint32)receive_word_via_i2c(HOST_PARAMETER_0) + ((uint32)receive_word_via_i2c(HOST_PARAMETER_2) << 16);

	dprintk( "VA_GetLastSnapshotShutterSpeed: ShutterSpeed = 0x%08X\n", ret);

	return ret;
}
uint8 VA_SetSnapshotImageStabilizerCount(uint8 is_count)
{
    uint8 Result=0;
    
	dprintk( "VA_SetImageStabilizerSnapshotCount: ISCount = %d\n", is_count);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, is_count);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_IMAGE_STABILIZER_COUNT);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

uint8 VA_SetSnapshotMultiShotCount(uint8 ms_count)
{
    uint8 Result=0;
    
	dprintk( "VA_SetMultiShotCount: MSCount = %d\n", ms_count);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, ms_count);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_MULTISHOT_COUNT);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

uint8 VA_SetDumpDataLinePixelsViaCCIR(uint32 line_pixels)
{
    uint8 Result=0;
    
	dprintk( "VA_SetDumpDataLinePixelsViaCCIR: Line Pixels = %d\n", line_pixels);

	Result = VA_CheckReadyForCmd();
	transmit_word_via_i2c(HOST_PARAMETER_0, line_pixels & 0xFFFF);
	transmit_word_via_i2c(HOST_PARAMETER_2, line_pixels >> 16);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_DUMP_DATA_LINE_PIXELS_VIA_CCIR);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

uint8 VA_SetDumpDataSpeedViaCCIR(uint8 speed)
{
    uint8 Result=0;
    
	dprintk( "VA_SetDumpDataSpeedViaCCIR: Speed = %d\n", speed);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, speed);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_DUMP_DATA_SPEED_VIA_CCIR);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

uint8 VA_TriggerSnapshot(void)
{
    uint8 Result=0;
    
	dprintk( "VA_TriggerSnapshot!\n");

	Result = VA_CheckReadyForCmd();

    if(Result)
    {
    	printk( "ISP is not Ready!\n");
    	return Result;
    } 
    transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_CAPTURE);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);

	Result = VA_CheckReadyForCmd();
    if(Result)
    {
        printk( "ISP is not Ready!\n");
        return Result;
    }

	dprintk( "VA_TriggerSnapshot End!\n");

	return Result;
}

uint8 VA_SetSnapshotFlashControl(uint8 flash_en)
{
    uint8 Result=0;
    
	dprintk( "VA_SetSnapshotFlashControl: flash_en = %d\n", flash_en);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, flash_en);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_FLASH_EN);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

uint8 VA_PrepareSnapshotDataForCCIR(uint32 thumbnail_offset)
{
    uint8 Result=0;
    
	dprintk( "VA_PrepareSnapshotDataForCCIR: Thumbnail Offset = 0x%08X\n", thumbnail_offset);

	Result = VA_CheckReadyForCmd();
	transmit_word_via_i2c(HOST_PARAMETER_0, thumbnail_offset & 0xFFFF);
	transmit_word_via_i2c(HOST_PARAMETER_2, thumbnail_offset >> 16);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_PREPARE_JPEG_DATA_FOR_CCIR);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

uint8 VA_DumpDataViaCCIR(uint32 start_addr, uint32 size)
{
    uint8 Result=0;
    
	dprintk( "VA_DumpDataViaCCIR: Start Address = 0x%08X, Size = %d\n", start_addr, size);

	Result = VA_CheckReadyForCmd();
	transmit_word_via_i2c(HOST_PARAMETER_0, start_addr & 0xFFFF);
	transmit_word_via_i2c(HOST_PARAMETER_2, start_addr >> 16);
	transmit_word_via_i2c(HOST_PARAMETER_4, size & 0xFFFF);
	transmit_word_via_i2c(HOST_PARAMETER_6, size >> 16);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_DUMP_DATA_VIA_CCIR);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);

	// new message (to ensure that AIT848 starts to output thumbnail and main image data)
	dprintk( "VA_DumpDataViaCCIR...\n");

	return Result;
}


/*************************************************************************************************/
/* Venus : Configure Sensor                                                                      */
/*************************************************************************************************/
uint8 VA_InitializeSensor(void)
{
    uint8 Result=0;
    
	dprintk( "VA_InitializeSensor\n");

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_INIT_SENSOR);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

/*************************************************************************************************/
/* Venus : Start/Stop Preview                                                                    */
/*************************************************************************************************/
uint8 VA_SetPreviewControl(uint8 status)
{
    uint8 Result=0;
    
	uint32 timeout = 0;
	uint16 Reg, Val;

	if (status == VA_PREVIEW_STATUS_START) {

		dprintk( "VA_SetPreviewControl: VA_PREVIEW_STATUS_START\n");

		Result = VA_CheckReadyForCmd();
		transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_PREVIEW_START);
		transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
		while ((receive_byte_via_i2c(HOST_STATUS_ADDR) & HOST_STATUS_READY) == 0 && timeout++ < 0x1000);

        
		if (timeout >= 0x1000) {

			VA_ERR( "VA_SetPreviewControl: timeout\n");
			Reg = 0x654E;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6510;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6512;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6002;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6010;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6020;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6022;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6024;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6030;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6032;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6034;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6036;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6c00;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6c02;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6c0e;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6c10;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6c12;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6c14;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6900;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6902;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x690A;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6910;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6912;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6914;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6F40;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6F42;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6F44;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6F50;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6F52;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6F54;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6F00;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6F02;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6F04;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6F06;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);

			}

	} else { // status == VA_PREVIEW_STATUS_STOP

		dprintk( "VA_SetPreviewControl: VA_PREVIEW_STATUS_STOP\n");

		Result = VA_CheckReadyForCmd();
		transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_PREVIEW_STOP);
		transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
		Result = VA_CheckReadyForCmd();

	}
    
	return Result;
}

/*************************************************************************************************/
/* Venus : Configure Preview                                                                     */
/*************************************************************************************************/
uint8 VA_SetPreviewResolution(uint16 width, uint16 height)
{
    uint8 Result=0;
    
	dprintk( "VA_SetPreviewResolution: (Width x Height) = (%d x %d)\n", width, height);

	Result = VA_CheckReadyForCmd();
	transmit_word_via_i2c(HOST_PARAMETER_0, width);
	transmit_word_via_i2c(HOST_PARAMETER_2, height);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_PREVIEW_RESOLUTION);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

uint8 VA_SetPreviewFormat(VENUS_PREVIEW_FORMAT format)
{
    uint8 Result=0;
	dprintk( "VA_SetPreviewFormat : format = %d\n", format);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, format);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_PREVIEW_FORMAT);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);

	return Result;
}
uint8 VA_SetPreviewFPS(VENUS_AE_FPS fps)
{
    uint8 Result=0;
    
	dprintk( "VA_SetPreviewFPS: FPS = %d\n", fps);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, fps);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_PREVIEW_FPS);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
#ifndef VENUS_CMD_SPEEDUP
	Result = VA_CheckReadyForCmd();
#endif

	return Result;
}


/*************************************************************************************************/
/* Venus : Start/Stop ISP Auto Focus                                                             */
/*************************************************************************************************/
uint8 VA_SetISPAutoFocusControl(uint8 status)
{
    uint8 Result=0;
    
	switch (status) {
	case VA_ISP_AUTO_FOCUS_STATUS_START1:
		dprintk( "VA_SetISPAutoFocusControl: VA_ISP_AUTO_FOCUS_STATUS_START1\n");

		Result = VA_CheckReadyForCmd();
		transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_ISP_AUTO_FOCUS_START1);
		transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
		Result = VA_CheckReadyForCmd();
		break;

	case VA_ISP_AUTO_FOCUS_STATUS_START2:
		dprintk( "VA_SetISPAutoFocusControl: VA_ISP_AUTO_FOCUS_STATUS_START2\n");

		Result = VA_CheckReadyForCmd();
		transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_ISP_AUTO_FOCUS_START2);
		transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
		Result = VA_CheckReadyForCmd();
		break;

	case VA_ISP_AUTO_FOCUS_STATUS_STOP: default:
		dprintk( "VA_SetISPAutoFocusControl: VA_ISP_AUTO_FOCUS_STATUS_STOP\n");

		Result = VA_CheckReadyForCmd();
		transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_ISP_AUTO_FOCUS_STOP);
		transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
		Result = VA_CheckReadyForCmd();
		break;
	}

	return Result;
}

uint8 VA_SetAEWnd(uint8 type)
{
    uint8 Result=0;
    
	dprintk( "VA_SetAEWnd: Type = %d\n", type);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, type);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_AE_WINDOW_WEIGHT);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
#ifndef VENUS_CMD_SPEEDUP
	Result = VA_CheckReadyForCmd();
#endif

	return Result;
}
uint8 VA_CheckAFStatus(uint8 *status)
{
    uint8 Result=0;
    
	dprintk( "VA_CheckAFStatus\n");

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_AF_GET_STATUS);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();
	*status = receive_byte_via_i2c(HOST_PARAMETER_0);

	return Result;
}

uint8 VA_GetFWVersion(uint8 *version)
{
    uint8 Result=0;
    
	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_GET_FW_VERSION);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	version[0] = receive_byte_via_i2c(HOST_PARAMETER_0); // major
	version[1] = receive_byte_via_i2c(HOST_PARAMETER_1); // minor
	version[2] = receive_byte_via_i2c(HOST_PARAMETER_2); // major
	version[3] = receive_byte_via_i2c(HOST_PARAMETER_3); // minor

	dprintk( "VA_GetFWVersion: Version = 0x%02X.0x%02X.0x%02X.0x%02X\n", version[0], version[1], version[2], version[3]);

	return Result;
}

uint8 VA_GetLowlightCond(uint8 *status)
{
    uint8 Result=0;
    
	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_GET_LIGHT_COND);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	*status = receive_byte_via_i2c(HOST_PARAMETER_0); // TRUE, FALSE

	dprintk( "VA_GetLowlightCond: Condition = %d\n", *status);

	return Result;
}

uint8 VA_AFMode(uint8 enable)
{
    uint8 Result=0;
    uint8 Result1=0;
    uint32 timeout_count = 0;
    
	dprintk( "VA_AFMode: Enable = %d\n", enable);

	Result = VA_CheckReadyForCmd();
	switch(enable) {
	case AF_MODE_AUTO:
		transmit_byte_via_i2c(HOST_PARAMETER_0, AF_MODE_AUTO);
		break;

	case AF_MODE_MANUAL:
		transmit_byte_via_i2c(HOST_PARAMETER_0, AF_MODE_MACRO);
		break;

	case AF_MODE_MACRO:
		transmit_byte_via_i2c(HOST_PARAMETER_0, AF_MODE_MACRO);
		break;
	case AF_MODE_PanFocus:
		transmit_byte_via_i2c(HOST_PARAMETER_0, AF_MODE_PanFocus);
		break;	
	}
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_AF_MODE);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);

	Result = VA_CheckReadyForCmd();

	while (timeout_count++ < 500) 
    {
		VA_DelayMS(1);
        timeout_count++;
        Result1 = receive_byte_via_i2c(HOST_PARAMETER_0); // TRUE, FALSE
        dprintk( "VA_AFMode: Result1 = %d\n", Result1);
        if(Result1==0)
            break;
    }
	return Result;
}



uint8 VA_SetSharpness(uint8 level)
{
    uint8 Result=0;
    
	dprintk( "VA_SetSharpness: Level = %d\n", level);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, level);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SHARPNESS);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
#ifndef VENUS_CMD_SPEEDUP
	Result = VA_CheckReadyForCmd();
#endif

	return Result;
}

// new parameter type (to make the API easier to maintain)
uint8 VA_SetISC(VENUS_ISC_MODE enable)
{
    uint8 Result=0;
    
	dprintk( "VA_SetISC: status = %d\n", enable);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, enable);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_ISC);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

uint8 VA_GetISCStatus(uint8 *status)
{
    uint8 Result=0;
    
	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_GET_ISC_STATUS);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	*status = receive_byte_via_i2c(HOST_PARAMETER_0);

	dprintk( "VA_GetISCStatus: Status = %d\n", *status);

	return Result;
}

uint8 VA_SetJPEGQuality(VENUS_SNAPSHOT_QUALITY quality)
{
    uint8 Result=0;
    

	dprintk( "VA_SetJPEGQuality: Quality = %d\n", quality);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, quality);

	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_JPEG_QUALITY);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

uint8 VA_SetAETarget(uint8 level)
{
    uint8 Result=0;
    
	dprintk( "VA_SetAETarget: Level = %d\n", level);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, level);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_EXPO_VALUE);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
#ifndef VENUS_CMD_SPEEDUP
	Result = VA_CheckReadyForCmd();
#endif

	return Result;
}

// new parameter type (to make the API easier to maintain)
uint8 VA_SetAWB(VENUS_AWB_MODE temp)
{
    uint8 Result=0;
    
	dprintk( "VA_SetAWB: Temp = %d\n", temp);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, temp);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_WB_MODE);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
#ifndef VENUS_CMD_SPEEDUP
	Result = VA_CheckReadyForCmd();
#endif

	return Result;
}

// new parameter type (to make the API easier to maintain)
uint8 VA_SetImgEffect(VENUS_IMAGE_EFFECT effectID)
{
    uint8 Result=0;
    
	dprintk( "VA_SetImgEffect: ImageEffectID = %d\n", effectID);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, effectID);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_IMAGE_EFFECT);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
#ifndef VENUS_CMD_SPEEDUP
	Result = VA_CheckReadyForCmd();
#endif

	return Result;
}

uint8 VA_SetSaturation(uint8 cmd)
{
    uint8 Result=0;
    
	dprintk( "VA_SetSaturation: Level = %d\n", cmd);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, cmd);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SATURATION);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
#ifndef VENUS_CMD_SPEEDUP
	Result = VA_CheckReadyForCmd();
#endif

	return Result;
}

uint8 VA_SetContrast(uint8 cmd)
{
    uint8 Result=0;
    
	dprintk( "VA_SetContrast: Level = %d\n", cmd);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, cmd);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_CONTRAST);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
#ifndef VENUS_CMD_SPEEDUP
	Result = VA_CheckReadyForCmd();
#endif

	return Result;
}

// new parameter type (to make the API easier to maintain)
uint8 VA_SetISO(VENUS_AE_ISO iso)
{
    uint8 Result=0;
    
	dprintk( "VA_SetISO: ISO = %d\n", iso);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, iso);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_ISO);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
#ifndef VENUS_CMD_SPEEDUP
	Result = VA_CheckReadyForCmd();
#endif

	return Result;
}

// new parameter type (to make the API easier to maintain)
uint8 VA_SetISPSceneMode(VENUS_SCENE_MODE scene)
{
    uint8 Result=0;
    
	dprintk( "VA_SetISPSceneMode: Scene = %d\n", scene);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, scene);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_SCENE_MODE);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
#ifndef VENUS_CMD_SPEEDUP
	Result = VA_CheckReadyForCmd();
#endif

	return Result;
}

uint8 VA_SetWDR(uint8 enable)
{
    uint8 Result=0;
    
	dprintk( "VA_SetWDR: Status = %d\n", enable);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, enable);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_WDR);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
#ifndef VENUS_CMD_SPEEDUP
	Result = VA_CheckReadyForCmd();
#endif

	return Result;
}

uint8 VA_SetFlickerMode(uint8 mode)
{
    uint8 Result=0;
    
	dprintk( "VA_SetFlickerMode: Mode = %d\n", mode);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, mode);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SET_FLICKER_MODE);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
#ifndef VENUS_CMD_SPEEDUP
	Result = VA_CheckReadyForCmd();
#endif

	return Result;
}

uint8 VA_SetFrameTypePreview(uint8 enable)
{
    uint8 Result=0;
    
	dprintk( "VA_SetFrameTypePreview: Status = %d\n", enable);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, enable);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_PREVIEWDRAWRECT);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
#ifndef VENUS_CMD_SPEEDUP
	Result = VA_CheckReadyForCmd();
#endif

	return Result;
}


/*************************************************************************************************/
/* Venus : SPI                                                                                    */
/*************************************************************************************************/
uint32 VA_GetFlashID(void)
{
	uint32 ret;

	VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SPI_ID_READ);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	VA_CheckReadyForCmd();

	ret = (uint32)receive_word_via_i2c(HOST_PARAMETER_0) + ((uint32)receive_word_via_i2c(HOST_PARAMETER_2) << 16);

	dprintk( "VA_GetFlashID: ID = 0x%X\n", ret);

	return ret;
}

uint8 VA_EraseFlash(void)
{
    uint8 Result=0;
    
	dprintk( "VA_EraseFlash\n");

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SPI_CHIP_ERASE);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

uint8 VA_ReadDataFromFlash(uint32 dst_addr, uint32 flash_addr, uint32 size)
{
    uint8 Result=0;
    
	dprintk( "VA_ReadDataFromFlash: dst_addr = 0x%08X | flash_addr = 0x%08X | size = 0x%08X\n", dst_addr, flash_addr, size);

	Result = VA_CheckReadyForCmd();
	transmit_word_via_i2c(HOST_PARAMETER_0, dst_addr & 0xFFFF);
	transmit_word_via_i2c(HOST_PARAMETER_2, dst_addr >> 16);
	transmit_word_via_i2c(HOST_PARAMETER_4, flash_addr & 0xFFFF);
	transmit_word_via_i2c(HOST_PARAMETER_6, flash_addr >> 16);
	transmit_word_via_i2c(HOST_PARAMETER_8, size & 0xFFFF);
	transmit_word_via_i2c(HOST_PARAMETER_A, size >> 16);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SPI_DATA_READ);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

uint8 VA_WriteDataToFlash(uint32 src_addr, uint32 flash_addr, uint32 size)
{
    uint8 Result=0;
    
	dprintk( "VA_WriteDataToFlash: src_addr = 0x%08X | flash_addr = 0x%08X | size = 0x%08X\n", src_addr, flash_addr, size);

	Result = VA_CheckReadyForCmd();
	transmit_word_via_i2c(HOST_PARAMETER_0, src_addr & 0xFFFF);
	transmit_word_via_i2c(HOST_PARAMETER_2, src_addr >> 16);
	transmit_word_via_i2c(HOST_PARAMETER_4, flash_addr & 0xFFFF);
	transmit_word_via_i2c(HOST_PARAMETER_6, flash_addr >> 16);
	transmit_word_via_i2c(HOST_PARAMETER_8, size & 0xFFFF);
	transmit_word_via_i2c(HOST_PARAMETER_A, size >> 16);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SPI_DATA_WRITE);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

uint32 VA_VerifyExtMem(uint32 mem_size)
{
	uint32 ret;

	dprintk( "VA_VerifyExtMem: size = 0x%08X\n", mem_size);

	VA_CheckReadyForCmd();
	transmit_word_via_i2c(HOST_PARAMETER_0, mem_size & 0xFFFF);
	transmit_word_via_i2c(HOST_PARAMETER_2, mem_size >> 16);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_EXT_MEM_TEST);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	VA_CheckReadyForCmd();

	ret = (uint32)receive_word_via_i2c(HOST_PARAMETER_0) + ((uint32)receive_word_via_i2c(HOST_PARAMETER_2) << 16);

	dprintk( "VA_VerifyExtMem: #error = %d\n", ret);

	return ret;
}

uint32 VA_CompareMem(uint32 mem_addr0, uint32 mem_addr1, uint32 mem_size)
{
	uint32 ret;

	dprintk( "VA_CompareMem: addr0 = 0x%08X, addr1 = 0x%08X, size = 0x%08X\n", mem_addr0, mem_addr1, mem_size);

	VA_CheckReadyForCmd();
	transmit_word_via_i2c(HOST_PARAMETER_0, mem_addr0 & 0xFFFF);
	transmit_word_via_i2c(HOST_PARAMETER_2, mem_addr0 >> 16);
	transmit_word_via_i2c(HOST_PARAMETER_4, mem_addr1 & 0xFFFF);
	transmit_word_via_i2c(HOST_PARAMETER_6, mem_addr1 >> 16);
	transmit_word_via_i2c(HOST_PARAMETER_8, mem_size & 0xFFFF);
	transmit_word_via_i2c(HOST_PARAMETER_A, mem_size >> 16);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_COMPARE_MEM);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	VA_CheckReadyForCmd();

	ret = (uint32)receive_word_via_i2c(HOST_PARAMETER_0) + ((uint32)receive_word_via_i2c(HOST_PARAMETER_2) << 16);

	dprintk( "VA_CompareMem: #error = %d\n", ret);

	return ret;
}

uint16 VA_GetSensorReg(uint16 sensor_reg_addr)
{
	uint16 reg_value;

	VA_CheckReadyForCmd();
	transmit_word_via_i2c(HOST_PARAMETER_0, sensor_reg_addr);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_READ_SENSOR);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	VA_CheckReadyForCmd();

	reg_value = receive_word_via_i2c(HOST_PARAMETER_A);

	return reg_value;
}

uint16 VA_SetSensorReg(uint16 sensor_reg_addr, uint16 sensor_reg_value)
{
	VA_CheckReadyForCmd();
	transmit_word_via_i2c(HOST_PARAMETER_0, sensor_reg_addr);
	transmit_word_via_i2c(HOST_PARAMETER_2, sensor_reg_value);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_WRITE_SENSOR);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	VA_CheckReadyForCmd();

	return 0;
}

uint16 VA_GetMotorReg(uint16 motor_reg_addr)
{
	uint16 reg_value;

	VA_CheckReadyForCmd();
	transmit_word_via_i2c(HOST_PARAMETER_0, motor_reg_addr);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_READ_MOTOR);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	VA_CheckReadyForCmd();

	reg_value = receive_word_via_i2c(HOST_PARAMETER_A);

	return reg_value;
}

uint16 VA_SetMotorReg(uint16 motor_reg_addr, uint16 motor_reg_value)
{
	VA_CheckReadyForCmd();
	transmit_word_via_i2c(HOST_PARAMETER_0, motor_reg_addr);
	transmit_word_via_i2c(HOST_PARAMETER_2, motor_reg_value);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_WRITE_MOTOR);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	VA_CheckReadyForCmd();

	return 0;
}

uint8 VA_SetChipStandby(void)
{
    uint8 Result=0;
	uint32 timeout_count = 0;
    
	dprintk( "VA_SetChipStandby\n");

	while ((receive_byte_via_i2c_ISP(HOST_STATUS_ADDR) & HOST_STATUS_READY) == 0) {

		VA_DelayMS(1);

		if (++timeout_count > MAX_TIMEOUT_CNT) {
			printk( "Venus API Timeout!\n");
			return 1;
		}
	}
    transmit_byte_via_i2c_ISP(HOST_CMD_ADDR1, VENUS_CHIP_STANDBY);
    transmit_byte_via_i2c_ISP(HOST_CMD_ADDR0, VENUS_CMD_TAIL);

	return Result;
}

uint8 VA_SetChipStreaming(void)
{
    uint8 Result=0;
    
	dprintk( "VA_SetChipStreaming\n");

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_CHIP_STREAMING);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

uint8 VA_SetChipTriState(void)
{
    uint8 Result=0;
    
	dprintk( "VA_SetChipTriState\n");

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c_ISP(HOST_CMD_ADDR1, VENUS_CHIP_TRISTATE);
	transmit_byte_via_i2c_ISP(HOST_CMD_ADDR0, VENUS_CMD_TAIL);

	return Result;
}

// new API (to make the 848 execute power off routines)
uint8 VA_SetChipPowerOff(void)
{
    uint8 Result=0;
	dprintk( "VA_SetChipPowerOff\n");

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c_ISP(HOST_CMD_ADDR1, VENUS_CHIP_POWEROFF);
	transmit_byte_via_i2c_ISP(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

uint8 VA_SetSensorResolutionAsPreview(uint8 sensor_resolution)
{
    uint8 Result=0;
    
	dprintk( "VA_SetSensorResolutionAsPreview : sensor_resolution = %d\n", sensor_resolution);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, sensor_resolution);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SENSOR_PREVIEW_RESOLUTION);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

uint8 VA_SetSensorResolutionAsSnapshot(uint8 sensor_resolution)
{
    uint8 Result=0;
    
	dprintk( "VA_SetSensorResolutionAsSnapshot : sensor_resolution = %d\n", sensor_resolution);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, sensor_resolution);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SENSOR_SNAPSHOT_RESOLUTION);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}


/*************************************************************************************************/
/* Function name: VA_SetSensorMirrorFlip                                                         */
/* Description: Set the sensor image orientation, horizontal mirror or vertical flip             */
/* Parameter: sensor_mirrorflip = VENUS_SENSOR_ORIENT_NORMAL      (0) ...... Normal              */
/*                              = VENUS_SENSOR_ORIENT_HOR_MIRROR  (1) ...... Horizontal mirror   */
/*                              = VENUS_SENSOR_ORIENT_VER_FLIP    (2) ...... Vertical flip       */
/*                              = VENUS_SENSOR_ORIENT_MIRROR_FLIP (3) ...... Mirror and flip     */
/*************************************************************************************************/
uint8 VA_SetSensorMirrorFlip(uint8 sensor_mirrorflip)
{
    uint8 Result=0;
    
	dprintk( "VA_SetSensorMirrorFlip : sensor_mirrorflip = %d\n", sensor_mirrorflip);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, sensor_mirrorflip);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_SENSOR_MIRROR_FLIP);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

uint8 VA_HostEvent(uint16 event)
{
    uint8 Result=0;
	dprintk( "VA_HostEvent : event = %d\n", event);

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, event & 0xff);
	transmit_byte_via_i2c(HOST_PARAMETER_1, (event>>8) & 0xff);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_BUTTON_EVENT);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

uint8 VA_CheckISPStatus(void)
{
	uint8 ret;

	VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_CHECK_ISP_STATUS);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	VA_CheckReadyForCmd();

	ret = receive_byte_via_i2c(HOST_PARAMETER_0);

	dprintk( "VA_CheckISPStatus: status = %d\n", ret);

	return ret;
}

uint8 VA_AE_AWB_Lock(uint8 Item)
{
	uint8 Result=0;
	dprintk( "VA_AE_AWB_Lock\n");

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, Item);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_LOCK_AE_AWB);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}

uint8 VA_AE_AWB_Unlock(uint8 Item)
{
	uint8 Result=0;
	dprintk( "VA_AE_AWB_Unlock\n");

	Result = VA_CheckReadyForCmd();
	transmit_byte_via_i2c(HOST_PARAMETER_0, Item);
	transmit_byte_via_i2c(HOST_CMD_ADDR1, VENUS_UNLOCK_AE_AWB);
	transmit_byte_via_i2c(HOST_CMD_ADDR0, VENUS_CMD_TAIL);
	Result = VA_CheckReadyForCmd();

	return Result;
}
