/*************************************************************************************************/
/* AIT ISP Host API                                                                              */
/* All rights reserved by Alpah Image Technology Corp                                            */
/*-----------------------------------------------------------------------------------------------*/
/* File name: VenusV3_API.c                                                                      */
/* Description: ISP Host API hardware dependent layer                                            */
/*                                                                                               */
/* Version 0.0A   20090326                                                                       */
/*************************************************************************************************/

#include "venus_api.h"
#include "venus_i2c_ctl.h"

#include "venus_ait848_5mp.h"
#include <linux/delay.h>

#if AIT848_TEST_API

static int debug = 0;
#define dprintk(msg...)	if (debug) { printk( "Tcc_cam: " msg); }

#include "venusv3_api.h"
#if defined(CONFIG_VIDEO_CAMERA_SENSOR_AIT848_FWDN_EN)
#ifdef CONFIG_MACH_TCC9200S_SPACEY
#include "venusfw_new.h"
#else
#include "venusfw.h"
#endif
#endif

#define MAX_TIMEOUT_CNT			200
#define DEBUG_FILE_PATH         "/SystemFS/Driver/Logfile.hex"

#if 0//defined(CONFIG_VIDEO_CAMERA_SENSOR_AIT848_FWDN_EN)
extern unsigned char *gpCameraISPBuff;
extern unsigned int gnFile_length;
#endif
//extern void delay_us(uint32 us);

#if VA_PROJECT_AIT705DEMOKIT

	extern uint16 A810H_InitSensor(void);
	extern void delay_1us(uint32 us);

	extern FILE *gOutfp;

	void VA_DelayMS(uint16 ms)
	{
		delay_us(ms * 1000);
	}

	uint8 V3A_ChipEnable(void)
	{
		return 0;
	}

	uint8 V3A_ChipReset(void)
	{
		// this function includes CHIP_ENABLE and CHIP_RESET
		A810H_InitSensor();
		return 0;
	}

#elif VA_PROJECT_SPEACY
void delay_us(uint32 us)
{
	udelay(us);
}

void VA_DelayMS(uint16 ms)
{   
	delay_us(ms * 1000);
}

uint8 V3A_ChipEnable(void)
{
	sensor_venus_chip_enable();
   
    return 0;
}

uint8 V3A_ChipReset(void)
{
	sensor_venus_chip_reset();
	
    return 0;
}

#endif



/*************************************************************************************************/
/* Venus : Variables                                                                             */
/*************************************************************************************************/
uint8 VA_FirmwareUpdateStatus = 1;
uint8 VA_FirmwareOffset = 0;

uint8 V3_NeedInitSensor = 0;

uint8 *V3_FirmwareBinPtr = 0;
uint32 V3_FirmwareBinSize = 0;
 
uint8 g_fw_version[2]={0,0};
uint8 g_bin_version[2]={0,0};

/*************************************************************************************************/
/* Venus : Sample Functions                                                                      */
/*************************************************************************************************/

#if VA_PROJECT_SPEACY
uint8 V3A_PowerOn(void)
{
	//VA_HIGH(0, "V3A_PowerOn : start\n");

	// Enable Venus V3
	V3A_ChipEnable();

	// Reset Venus V3
	// If this command is too far from V3A_PLLOn(), VR_InitClock() in FW will be done before new PLL settings set.
	// Therefore, it shall be put before V3A_PLLOn() as close as possible to prevent from PLL ineffective.
	V3A_ChipReset();

	// Set Venus V3 PLL
    if (V3A_PLLOn()) 
    {
        printk("Venus V3 V3A_PLLOn fail\n");
        return 1;
    }   
	//VA_HIGH(0, "V3A_PowerOn : end\n");

	return 0;
}

uint8 V3A_PowerOff(void)
{

	//VA_HIGH(0, "V3A_PowerOff : start\n");
    // 848 power off routines
    VA_SetChipPowerOff();
   // VA_HIGH(0, "V3A_PowerOff : end\n");
    
    VA_FirmwareUpdateStatus = 1;
    
	return 0;
}

uint8 V3A_PowerUp(void)
{
	uint32 timeout_count = 0;
    uint8 Result=0;

	//VA_HIGH(0, "V3A_PowerUp : start\n");
	
//ZzaU :: Don't Update forcely.
#if !defined(CONFIG_VIDEO_CAMERA_SENSOR_AIT848_FWDN_EN)
#if V3_FORCE_TO_DOWNLOAD_FW
	dprintk("+++ force to download FW \n");
	VA_FirmwareUpdateStatus = 1;
#elif V3_FORCE_TO_SKIP_DOWNLOAD_FW
	dprintk("+++ force to skip download FW\n");
	VA_FirmwareUpdateStatus = 0;
#endif
#endif

	if (VA_FirmwareUpdateStatus) 
    {
        V3A_ChipReset(); // Need H/W reset
		V3A_FirmwareStart();
	}
    else
    {
    // set PLL //CYK For Preveiw TEST
    	if (V3A_PLLOn()) 
        {
    		printk("Venus V3 V3A_PLLOn fail\n");
    		return 1;
    	}    
    }
	// wait for initialization finish

	while (((receive_byte_via_i2c(HOST_STATUS_ADDR) & HOST_STATUS_READY) == 0) && timeout_count++ < V3_TIMEOUT_CNT) {
		VA_DelayMS(1);		
	}

	if (timeout_count >= V3_TIMEOUT_CNT) {
		printk("Time out! Venus V3 initialization fail\n");
		return 1;
	}

	VA_FirmwareUpdateStatus = 1;
	V3_NeedInitSensor = 1;

	VA_HIGH(0, "V3A_PowerUp : end\n");

	return 0;
}


#else

uint8 V3A_PowerOn(void)
{
	VA_HIGH(0, "V3A_PowerOn : start\n");

	// Enable Venus V3
	V3A_ChipEnable();

	// Reset Venus V3
	// If this command is too far from V3A_PLLOn(), VR_InitClock() in FW will be done before new PLL settings set.
	// Therefore, it shall be put before V3A_PLLOn() as close as possible to prevent from PLL ineffective.
	V3A_ChipReset();

	// Set Venus V3 PLL
	V3A_PLLOn();

	VA_HIGH(0, "V3A_PowerOn : end\n");

	return 0;
}

uint8 V3A_PowerOff(void)
{
	VA_HIGH(0, "V3A_PowerOff : start\n");
	VA_HIGH(0, "V3A_PowerOff : end\n");
        
	return 0;
}

uint8 V3A_PowerUp(void)
{
	uint32 timeout_count = 0;

	VA_HIGH(0, "V3A_PowerUp : start\n");

#if V3_FORCE_TO_DOWNLOAD_FW
	VA_INFO(0, "+++ force to download FW\n");
	VA_FirmwareUpdateStatus = 1;
#elif V3_FORCE_TO_SKIP_DOWNLOAD_FW
	VA_INFO(0, "+++ force to skip download FW\n");
	VA_FirmwareUpdateStatus = 0;
#endif

	if (VA_FirmwareUpdateStatus) {
		V3A_FirmwareStart();
    }

	// wait for initialization finish
	while (((receive_byte_via_i2c(HOST_STATUS_ADDR) & HOST_STATUS_READY) == 0) && timeout_count++ < V3_TIMEOUT_CNT) {
		msleep(1);
	}

	if (timeout_count >= V3_TIMEOUT_CNT) {
		VA_MSG(0, "Time out! Venus V3 initialization fail\n");
		return 1;
	}

	VA_FirmwareUpdateStatus = 0;
	V3_NeedInitSensor = 1;

	VA_HIGH(0, "V3A_PowerUp : end\n");

	return 0;
}
#endif

uint8 V3A_CheckFirmwareVersion(void)
{
	dprintk("V3A_CheckFirmwareVersion : start\n");

	if (VA_FirmwareUpdateStatus) {

        if (V3A_PowerOn()) 
        {
            printk("Venus V3 V3A_PowerOn fail\n");
            return 1;
        }   
		VA_FirmwareUpdateStatus = V3A_CheckFirmwareUpdate();

		//V3A_PowerOff();
    }

	dprintk("V3A_CheckFirmwareVersion : end\n");

	return 0;
}

uint8 V3A_PLLOn(void)
{
    uint16 Reg, Val;
	uint32 timeout_count = 0;

	dprintk("V3A_PLLOn : start in VenusV_API.c\n");

	// wait for default PLL settings stable
	msleep(2);

	// Wait DMA complete from SIF to CPU program memory
	while (receive_byte_via_i2c(0x6713) != 0 && timeout_count++ < V3_TIMEOUT_CNT) {
		msleep(1);
	}
if (timeout_count >= 200) {

        printk("V3A_PLLOn: Time out!\n");
        
			VA_ERR( "Venus API Timeout!\n");
			Reg = 0x6900;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6902;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6904;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6906;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x690E;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6910;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6912;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6916;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6918;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x691E;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x6920;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x65FE;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x65E8;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
/*
			Reg = 0x7900;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x7902;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x792C;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x791C;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
			Reg = 0x791E;
			Val = receive_word_via_i2c( Reg );
			VA_ERR( "Reg[%x] %x\n", Reg, Val);
*/

			return 1;

	}


	dprintk("V3A_PLLOn : end\n");

	return 0;
}

#if VA_PROJECT_SPEACY

//extern uint8 venusfw[];			// AIT848 firmware array
//extern uint32 venusfw_size;		// AIT848 firmware size
//uint8 venusfw[];			// AIT848 firmware array
//uint32 venusfw_size;		// AIT848 firmware size

uint8 VenusV3_ReadFirmwareFile(void)
{
#if defined(CONFIG_VIDEO_CAMERA_SENSOR_AIT848_FWDN_EN)
	V3_FirmwareBinPtr = (uint8*)venusfw;
#else
    V3_FirmwareBinPtr = 0;
#endif
#ifdef CONFIG_MACH_TCC9200S_SPACEY
	V3_FirmwareBinSize = 116*1024;
#else	
    V3_FirmwareBinSize = 124*1024;
#endif

    return 0;
}

uint8 VenusV3_FreeFirmwareMemory(void)
{
	dprintk("VenusV3_FreeFirmwareMemory : start\n");

    if(V3_FirmwareBinPtr)
    {
        
        V3_FirmwareBinPtr = 0;
    }
    
	dprintk("VenusV3_FreeFirmwareMemory : end\n");
    
	return 0;
}

#endif

uint8 V3A_CheckFirmwareUpdate(void)
{
	uint32	timeout_count = 0;
	uint8	fw_version[2];
	uint8	bin_version[2];

	dprintk("V3A_CheckFirmwareUpdate : start\n");

	bin_version[0] = V3_FirmwareBinPtr[V3_FirmwareBinSize-4] & 0x1F;
	bin_version[1] = V3_FirmwareBinPtr[V3_FirmwareBinSize-3];
	dprintk("binary version %x.%x\n", bin_version[0], bin_version[1]);
	
	while (((receive_byte_via_i2c(HOST_STATUS_ADDR) & HOST_STATUS_READY) == 0) && timeout_count++ < V3_TIMEOUT_CNT) {
		msleep(1);
	}

	if (timeout_count >= V3_TIMEOUT_CNT) {
		printk("Time out! Venus V3 initialization fail\n");
		printk("V3A_CheckFirmwareUpdate : end (need to update | FW not found)\n");
		return 1;
	}

	VA_GetFWVersion(fw_version);

    g_fw_version[0]= fw_version[0];    
    g_fw_version[1]= fw_version[1];
    g_bin_version[0]= bin_version[0];    
    g_bin_version[1]= bin_version[1];

	if (fw_version[0] == bin_version[0] && fw_version[1] == bin_version[1]) {
		dprintk("firmware version %x.%x\n", fw_version[0], fw_version[1]);
		dprintk("binary version %x.%x\n", bin_version[0], bin_version[1]);
		dprintk("V3A_CheckFirmwareUpdate : end (no need to update | the same version)\n");
		return 0;
	} else {
		printk("firmware version %x.%x\n", fw_version[0], fw_version[1]);
		printk("binary version %x.%x\n", bin_version[0], bin_version[1]);
		printk("V3A_CheckFirmwareUpdate : end (need to update | different versions)\n");
		return 1;
	}

	return 0;
}

#if VA_PROJECT_SPEACY

uint8 V3A_FirmwareStart(void)
{
	uint8 ret;
	uint32 timeout_count = 0;

	dprintk("VA Firmware Start : start\n");

	// set PLL
    //Result = V3A_PLLOn();
	if (V3A_PLLOn()) 
    {
		printk("Time out! V3A_PLLOn fail\n");
		return 1;
	}    
	// enable access program memory ; disable CPU
	transmit_byte_via_i2c(0x6901, 0x06);
	dprintk("enable access program memory\n");

	// download firmware
	if (V3A_DownloadFirmware()) 
    {
		printk("Time out! V3A_DownloadFirmware fail\n");
		return 1;
	}

	// disable access program memory ; enable CPU
	transmit_byte_via_i2c(0x6901, 0x00);
	dprintk("disable access program memory\n");

	VA_DelayMS(1);
    
	// wait for initialization finished
	while ((((ret = receive_byte_via_i2c(HOST_STATUS_ADDR) & HOST_STATUS_READY)) == 0) && timeout_count++ < V3_TIMEOUT_CNT) {

		if(timeout_count%50 == 0)
		{
			printk("ret = 0x%x \n", ret);
		}
		VA_DelayMS(1);		
	}

	if (timeout_count >= V3_TIMEOUT_CNT) {
		printk("ISP is not ready status~! Venus V3 V3A_FirmwareStart fail\n");
		return 1;
	}

#if V3_FORCE_TO_SKIP_FW_BURNING
	dprintk("+++ force to skip FW flash burning\n");
#else
    if (V3A_UpdateFirmware())   // flash burning
    {
        printk("V3A_UpdateFirmware fail\n");
        return 1;
    }
#endif

	dprintk("VA Firmware Start : end\n");

	return 0;
}

#else

uint8 V3A_FirmwareStart(void)
{
	uint32 timeout_count = 0;

	VA_HIGH(MM_CAMERA, "VA Firmware Start : start\n");

	// software reset
	transmit_byte_via_i2c(0x6908, 0xFF);
	transmit_byte_via_i2c(0x690A, 0xFF);
	transmit_byte_via_i2c(0x690A, 0x00);
	transmit_byte_via_i2c(0x6908, 0x00);

	// set PLL
	V3A_PLLOn();

	// enable access program memory ; disable CPU
	transmit_byte_via_i2c(0x6901, 0x06);

	// download firmware
	V3A_DownloadFirmware();

	// disable access program memory ; enable CPU
	transmit_byte_via_i2c(0x6901, 0x00);

	// wait for initialization finished
	while (((receive_byte_via_i2c(HOST_STATUS_ADDR) & HOST_STATUS_READY) == 0) && timeout_count++ < V3_TIMEOUT_CNT) {
		msleep(1);
	}

	if (timeout_count >= V3_TIMEOUT_CNT) {
		VA_MSG(MM_CAMERA, "Time out! Venus V3 initialization fail\n");
		return 1;
	}

#if V3_FORCE_TO_SKIP_FW_BURNING
	VA_INFO(MM_CAMERA, "+++ force to skip FW flash burning\n");
#else
	V3A_UpdateFirmware(); // flash burning
#endif

	VA_HIGH(MM_CAMERA, "VA Firmware Start : end\n");

	return 0;
}

#endif

#if VA_PROJECT_SPEACY //__CAM_ENABLE_AIT848_128K__            

uint8 V3A_DownloadFirmware(void)
{
	uint8 *fw_ptr = V3_FirmwareBinPtr;
#if V3_FW_DOWNLOAD_CHECK
	uint8 *fw_read_ptr = V3_FirmwareBinPtr + V3_FW_SIZE;
#endif

	uint32 section_num, section_idx;
	uint32 section_addr;
	uint32 section_size, remain_section_size;

	uint32 part_idx;
	uint32 part_size;

	uint32 i, error_count;

	dprintk("V3A_DownloadFirmware : start\n");


	fw_ptr += 8; // skip pkg_name

	section_num = fw_ptr[0] + (fw_ptr[1] << 8);
	fw_ptr += 2;

	dprintk("package name: %s | section number = %d\n", "ait848fw", section_num);
    
    dprintk("[AIT848] Rt section number = %d\n", section_num);    
    
	dprintk("V3_FW_DOWNLOAD_SIZE_PER_PART = %d bytes\n", V3_FW_DOWNLOAD_SIZE_PER_PART);
	dprintk("V3_FW_DOWNLOAD_PARTS_PER_BANK = %d\n", V3_FW_DOWNLOAD_PARTS_PER_BANK);

	for (section_idx = 0; section_idx < section_num; section_idx++) {

		dprintk("  SECTION %d ++++++++++++++++++++++++++++++++++++++++++++++++\n", section_idx);

		section_addr = fw_ptr[0] + (fw_ptr[1] << 8) + (fw_ptr[2] << 16) + (fw_ptr[3] << 24);
		fw_ptr += 4;
		section_size = remain_section_size = fw_ptr[0] + (fw_ptr[1] << 8) + (fw_ptr[2] << 16) + (fw_ptr[3] << 24);
		fw_ptr += 4;

		dprintk("  # section addr = 0x%08X\n", section_addr);
		dprintk("  # section size = 0x%08X (%d)\n", section_size, section_size);

		if (section_addr == 0x01000000) { // if section is in DRAM

            // put into SRAM buffer first, then AIT848 FW move to DRAM after booting up
            // SRAM buffer address is 0x0010F000 which is the same as auto flash boot loading from flash address address (0x10000-0x1FFFF)
            section_addr = 0x0010F000;

			dprintk("  # redirect section address to 0x%08X, since the section is in DRAM)\n", section_addr);
		}

		transmit_byte_via_i2c(0x691E, (section_addr >> 27) & 0x03);
		transmit_byte_via_i2c(0x691D, (section_addr >> 19) & 0xFF);

		for (part_idx = 0, error_count = 0; remain_section_size > 0; part_idx++) {

			transmit_byte_via_i2c(0x691C, (section_addr >> 15) & 0x0F);

			part_size = VA_MIN(remain_section_size, V3_FW_DOWNLOAD_SIZE_PER_PART);

			// write process
			transmit_multibytes_via_i2c(fw_ptr + part_idx * V3_FW_DOWNLOAD_SIZE_PER_PART, 0x8000 + section_addr % 0x8000, part_size);

#if V3_FW_DOWNLOAD_CHECK
			// read process
			receive_multibytes_via_i2c(fw_read_ptr, 0x8000 + section_addr % 0x8000, part_size);

			// verification
			for (i = 0; i < part_size; i++) {
				if (fw_ptr[part_idx * V3_FW_DOWNLOAD_SIZE_PER_PART + i] != fw_read_ptr[i]) error_count++;
			}
            
            dprintk("[AIT848] Rt error_count number = %d\n", error_count);    
#endif

			remain_section_size -= part_size;
			section_addr += V3_FW_DOWNLOAD_SIZE_PER_PART;

			dprintk("    (0x691E, 0x691D, 0x691C) = (0x%02X, 0x%02X, 0x%02X) | %5d (%3d%%)\n", receive_byte_via_i2c(0x691E), receive_byte_via_i2c(0x691D), receive_byte_via_i2c(0x691C), section_size - remain_section_size, (section_size - remain_section_size) * 100 / section_size);
		}

		fw_ptr += section_size;


		printk("  # section error = %d (%d bytes verified)\n", error_count, section_size);
	}
    dprintk("[AIT848] section error = %d (%d bytes verified)\n", error_count, section_size);  
    
    if(error_count!=0)
    {
    	printk("V3A_DownloadFirmware : fail \n");
    	return 1;
        //SSAssert("error_count Err~!!");
    }
    
	dprintk("V3A_DownloadFirmware : end\n");

	return 0;
}

uint8 V3A_UpdateFirmware(void)
{
	uint8 *old_fw_ptr = V3_FirmwareBinPtr;
	uint8 *new_fw_ptr = V3_FirmwareBinPtr + V3_FW_SIZE;
#if V3_FW_UPDATE_CHECK
	uint8 *fw_read_ptr = V3_FirmwareBinPtr;
#endif

	uint8 section_num;
	uint32 section_addr;
	uint32 section_size, remain_section_size;

	uint32 part_size;

	uint32 i, j;
	uint32 error_count = 0;
	uint32 flash_size;
    uint8 Result=0;
#if V3_SAVE_REARRANGED_FW
	FILE* fp;
#endif

	printk("V3A_UpdateFirmware : start\n");

	// delay 25ms to make read ID correctly
	VA_DelayMS(25);

	// get flash ID and determine the flash size.
	if (VA_GetFlashID() == 0x1120C2) {
		flash_size = 0x20000; // Flash ID = 0x1120C2 | 128KB
		dprintk("flash size = 128KB\n");
	} else {
		flash_size = 0x10000; // Flash ID = 0x1020C2 | 64KB
		dprintk("flash size = 64KB\n");
	}

	// rearrange firmware binary
	dprintk("rearrange firmware binary start\n");
	old_fw_ptr += 8; // skip pkg_name

	section_num = old_fw_ptr[0] + (old_fw_ptr[1] << 8);
	old_fw_ptr += 2;

	while (section_num--) {

		section_addr = old_fw_ptr[0] + (old_fw_ptr[1] << 8) + (old_fw_ptr[2] << 16) + (old_fw_ptr[3] << 24);
		old_fw_ptr += 4;

		section_size = old_fw_ptr[0] + (old_fw_ptr[1] << 8) + (old_fw_ptr[2] << 16) + (old_fw_ptr[3] << 24);
		old_fw_ptr += 4;

		remain_section_size = 0;

		if (section_addr == 0x00000000 || section_addr == 0x00080000) {
			remain_section_size = 0x1000 - section_size; // force 4KB
		}

		// if flash size is 128KB, fill section 2 to 60KB
		if (flash_size == 0x20000 && section_addr == 0x00100000) {
			remain_section_size = 0xF000 - section_size; // force 60KB
		}

		for (i = 0; i < section_size; i++) {
			*new_fw_ptr++ = *old_fw_ptr++;
		}

		for (i = 0; i < remain_section_size; i++) {
			*new_fw_ptr++ = 0;
		}

	}

	// make fw to a big section
	remain_section_size = section_size = (uint32)new_fw_ptr - ((uint32)V3_FirmwareBinPtr + V3_FW_SIZE);
	new_fw_ptr = V3_FirmwareBinPtr + V3_FW_SIZE;

	printk(" + rearrange firmware binary size: 0x%04X\n", section_size);
	printk(" + rearrange firmware binary addr: 0x%04X\n", (uint32) new_fw_ptr);
#if V3_SAVE_REARRANGED_FW
	fp = fopen("../../../../../rearrange_fw.bin", "wb");
	fwrite(new_fw_ptr, sizeof(uint8), section_size, fp);
	fclose(fp);
#endif

	dprintk("rearrange firmware binary finished\n");


	// read flash_size KB data from flash to DRAM (DRAM bank0) to prevent from erasing calibration data
	VA_ReadDataFromFlash(0x01020000L, 0x00000000L, flash_size - 1);


#if V3_FW_UPDATE_CHECK
	fw_read_ptr = V3_FirmwareBinPtr;
	for (i = 0; i < section_size; i++) {
		fw_read_ptr[i] = 0;
	}
#endif

	// write FW to DRAM (from DRAM bank0)
	section_addr = 0x01020000L;

	transmit_byte_via_i2c(0x691E, (section_addr >> 27) & 0x03);
	transmit_byte_via_i2c(0x691D, (section_addr >> 19) & 0xFF);

	for (j = 0; remain_section_size > 0; j++) {
		transmit_byte_via_i2c(0x691C, (section_addr >> 15) & 0x0F);

		part_size = VA_MIN(remain_section_size, V3_FW_UPDATE_SIZE_PER_PART);

		transmit_multibytes_via_i2c(new_fw_ptr + j * V3_FW_UPDATE_SIZE_PER_PART, 0x8000 + section_addr % 0x8000, part_size);
		VA_DelayMS(V3_FW_UPDATE_DELAY_PER_PART);

#if V3_FW_UPDATE_CHECK
		receive_multibytes_via_i2c(fw_read_ptr + j * V3_FW_UPDATE_SIZE_PER_PART, 0x8000 + section_addr % 0x8000, part_size);
		VA_DelayMS(V3_FW_UPDATE_DELAY_PER_PART);
#endif

		remain_section_size -= part_size;
		section_addr += V3_FW_UPDATE_SIZE_PER_PART;


		VA_INFO(MM_CAMERA, "downloaded fw size: %5d (%3d%%)\n", section_size - remain_section_size, (section_size - remain_section_size) * 100 / section_size);
	}

#if V3_FW_UPDATE_CHECK
	for (i = error_count = 0; i < section_size; i++) {
		if (fw_read_ptr[i] != new_fw_ptr[i])
			error_count++;
	}
	printk("Venus V3 FW %d bytes verified (#error = %d)\n", section_size, error_count);
	if (error_count)
	{
           printk("Venus V3 FW verified : fail\n");
            return 1;
	}    
#endif


	// erase flash first
	if (VA_EraseFlash()) 
    {
		printk("Time out! VA_EraseFlash fail\n");
		return 1;
	}  
	// write flash_size KB data to flash
	if (VA_WriteDataToFlash(0x01020000L, 0x00000000L, flash_size - 1)) 
    {
		printk("Time out! VA_WriteDataToFlash fail\n");
		return 1;
	}  

	// compare all data (not only fw binary...)
	printk("flash comparison: %d\n", flash_size);

	// read fw to DRAM to check data correctness
	if (VA_ReadDataFromFlash(0x01020000L + flash_size, 0x00000000L, flash_size - 1)) 
    {
		printk("Time out! VA_ReadDataFromFlash fail\n");
		return 1;
	}  
	error_count = VA_CompareMem(0x01020000L, 0x01020000L + flash_size, flash_size);

	if (error_count)
	{
		printk("flash comparison fail (%d errors in %d bytes)\n", error_count, flash_size);
        printk("V3A_UpdateFirmware : fail\n");
            return 1;
	}
	else
	{
		printk("flash comparison finished (%d bytes verified)\n", flash_size);
	}
	printk("V3A_UpdateFirmware : end\n");

    return 0;
}
#else

uint8 V3A_DownloadFirmware(void)
{
	uint8 *fw_ptr = V3_FirmwareBinPtr;
#if V3_FW_DOWNLOAD_CHECK
	uint8 *fw_read_ptr = V3_FirmwareBinPtr + V3_FW_SIZE;
#endif

	uint8 section_num, section_idx;
	uint32 section_addr;
	uint32 section_size, remain_section_size;

	uint8 part_idx;
	uint32 part_size;

	uint32 i, error_count;


	VA_HIGH(MM_CAMERA, "V3A_DownloadFirmware : start\n");


	fw_ptr += 8; // skip pkg_name

	section_num = fw_ptr[0] + (fw_ptr[1] << 8);
	fw_ptr += 2;

	VA_INFO(MM_CAMERA, "package name: %s | section number = %d\n", "ait848fw", section_num);
    
    VA_MSG(MM_CAMERA, "[AIT848] Rt section number = %d\n", section_num);    
    
	VA_INFO(MM_CAMERA, "V3_FW_DOWNLOAD_SIZE_PER_PART = %d bytes\n", V3_FW_DOWNLOAD_SIZE_PER_PART);
	VA_INFO(MM_CAMERA, "V3_FW_DOWNLOAD_PARTS_PER_BANK = %d\n", V3_FW_DOWNLOAD_PARTS_PER_BANK);

	for (section_idx = 0; section_idx < section_num; section_idx++) {

		VA_MSG(MM_CAMERA, "  SECTION %d ++++++++++++++++++++++++++++++++++++++++++++++++\n", section_idx);

		section_addr = fw_ptr[0] + (fw_ptr[1] << 8) + (fw_ptr[2] << 16) + (fw_ptr[3] << 24);
		fw_ptr += 4;
		section_size = remain_section_size = fw_ptr[0] + (fw_ptr[1] << 8) + (fw_ptr[2] << 16) + (fw_ptr[3] << 24);
		fw_ptr += 4;

		VA_MSG(MM_CAMERA, "  # section addr = 0x%08X\n", section_addr);
		VA_MSG(MM_CAMERA, "  # section size = 0x%08X (%d)\n", section_size, section_size);

		transmit_byte_via_i2c(0x691E, (section_addr >> 27) & 0x03);
		transmit_byte_via_i2c(0x691D, (section_addr >> 19) & 0xFF);

		for (part_idx = 0, error_count = 0; remain_section_size > 0; part_idx++) {

			transmit_byte_via_i2c(0x691C, part_idx / V3_FW_DOWNLOAD_PARTS_PER_BANK);

			part_size = VA_MIN(remain_section_size, V3_FW_DOWNLOAD_SIZE_PER_PART);

			// write process
			transmit_multibytes_via_i2c(fw_ptr + part_idx * V3_FW_DOWNLOAD_SIZE_PER_PART, 0x8000 + V3_FW_DOWNLOAD_SIZE_PER_PART * (part_idx % V3_FW_DOWNLOAD_PARTS_PER_BANK), part_size);

#if V3_FW_DOWNLOAD_CHECK
			// read process
			receive_multibytes_via_i2c(fw_read_ptr, 0x8000 + V3_FW_DOWNLOAD_SIZE_PER_PART * (part_idx % V3_FW_DOWNLOAD_PARTS_PER_BANK), part_size);

			// verification
			for (i = 0; i < part_size; i++) {
				if (fw_ptr[part_idx * V3_FW_DOWNLOAD_SIZE_PER_PART + i] != fw_read_ptr[i]) error_count++;
			}
            
            VA_MSG(MM_CAMERA, "[AIT848] Rt error_count number = %d\n", error_count);    
#endif

			remain_section_size -= part_size;

			VA_MSG(MM_CAMERA, "    (0x691E, 0x691D, 0x691C) = (0x%02X, 0x%02X, 0x%02X) | %5d (%3d%%)\n", receive_byte_via_i2c(0x691E), receive_byte_via_i2c(0x691D), receive_byte_via_i2c(0x691C), section_size - remain_section_size, (section_size - remain_section_size) * 100 / section_size);
		}

		fw_ptr += section_size;


		VA_MSG(MM_CAMERA, "  # section error = %d (%d bytes verified)\n", error_count, section_size);
	}
    VA_MSG(MM_CAMERA, "[AIT848] section error = %d (%d bytes verified)\n", error_count, section_size);    
    
    if(error_count!=0)
    {
    	VA_HIGH(MM_CAMERA, "V3A_DownloadFirmware : fail \n");
    	return 1;
        //SSAssert("error_count Err~!!");
    }
	VA_HIGH(MM_CAMERA, "V3A_DownloadFirmware : end\n");

	return 0;
}

uint8 V3A_UpdateFirmware(void)
{
	uint8 *old_fw_ptr = V3_FirmwareBinPtr;
	uint8 *new_fw_ptr = V3_FirmwareBinPtr + V3_FW_SIZE;
#if V3_FW_UPDATE_CHECK
	uint8 *fw_read_ptr = V3_FirmwareBinPtr;
#endif

	uint8 section_num;
	uint32 section_addr;
	uint32 section_size, remain_section_size;

	uint32 part_size;

	uint32 i, j;
	uint32 error_count = 0;

#if V3_SAVE_REARRANGED_FW
	FILE* fp;
#endif


	VA_HIGH(MM_CAMERA, "V3A_UpdateFirmware : start\n");


	// rearrange firmware binary
	VA_INFO(MM_CAMERA, "rearrange firmware binary\n");
	old_fw_ptr += 8; // skip pkg_name

	section_num = old_fw_ptr[0] + (old_fw_ptr[1] << 8);
	old_fw_ptr += 2;

	while (section_num--) {

		section_addr = old_fw_ptr[0] + (old_fw_ptr[1] << 8) + (old_fw_ptr[2] << 16) + (old_fw_ptr[3] << 24);
		old_fw_ptr += 4;

		section_size = old_fw_ptr[0] + (old_fw_ptr[1] << 8) + (old_fw_ptr[2] << 16) + (old_fw_ptr[3] << 24);
		old_fw_ptr += 4;

		if (section_addr == 0x00000000 || section_addr == 0x00080000) {
			remain_section_size = 0x1000 - section_size; // force 4KB
		} else {
			remain_section_size = 0;
		}

		for (i = 0; i < section_size; i++) {
			*new_fw_ptr++ = *old_fw_ptr++;
		}

		for (i = 0; i < remain_section_size; i++) {
			*new_fw_ptr++ = 0;
		}

	}

	// make fw to a big section
	remain_section_size = section_size = (uint32)new_fw_ptr - (uint32)V3_FirmwareBinPtr - V3_FW_SIZE;
	new_fw_ptr = V3_FirmwareBinPtr + V3_FW_SIZE;

#if V3_SAVE_REARRANGED_FW
	fp = fopen("../../../../../rearrange_fw.bin", "wb");
	fwrite(new_fw_ptr, sizeof(uint8), section_size, fp);
	fclose(fp);
#endif

	VA_INFO(MM_CAMERA, "rearrange firmware binary finished\n");


	// dump device ID.
	VA_GetFlashID();

	// read 64KB data from flash to DRAM (from DRAM bank0) to prevent from erasing calibration data
	VA_ReadDataFromFlash(0x01000000L, 0x00000000L, 0x0000FFFFL);


#if V3_FW_UPDATE_CHECK
	fw_read_ptr = V3_FirmwareBinPtr;
	for (i = 0; i < section_size; i++) {
		fw_read_ptr[i] = 0;
	}
#endif

	// write FW to DRAM (from DRAM bank0)
	transmit_byte_via_i2c(0x691E, 0x00);
	transmit_byte_via_i2c(0x691D, 0x20);

	for (j = 0; remain_section_size > 0; j++) {
		transmit_byte_via_i2c(0x691C, j / V3_FW_UPDATE_PARTS_PER_BANK);

		part_size = VA_MIN(remain_section_size, V3_FW_UPDATE_SIZE_PER_PART);

		transmit_multibytes_via_i2c(new_fw_ptr + j * V3_FW_UPDATE_SIZE_PER_PART, 0x8000 + (j % V3_FW_UPDATE_PARTS_PER_BANK) * V3_FW_UPDATE_SIZE_PER_PART, part_size);
		VA_DelayMS(V3_FW_UPDATE_DELAY_PER_PART);

#if V3_FW_UPDATE_CHECK
		receive_multibytes_via_i2c(fw_read_ptr + j * V3_FW_UPDATE_SIZE_PER_PART, 0x8000 + (j % V3_FW_UPDATE_PARTS_PER_BANK) * V3_FW_UPDATE_SIZE_PER_PART, part_size);
		VA_DelayMS(V3_FW_UPDATE_DELAY_PER_PART);
#endif

		remain_section_size -= part_size;


		VA_INFO(MM_CAMERA, "downloaded fw size: %5d (%3d%%)\n", section_size - remain_section_size, (section_size - remain_section_size) * 100 / section_size);
	}

#if V3_FW_UPDATE_CHECK
    for (i = error_count = 0; i < section_size; i++) {
        if (fw_read_ptr[i] != new_fw_ptr[i])
            error_count++;
    }
    VA_INFO(MM_CAMERA, "Venus V3 FW %d bytes verified (#error = %d)\n", section_size, error_count);
    if (error_count)
    {
        VA_HIGH(MM_CAMERA, "Venus V3 FW %d bytes verified : fail\n");
        return 1;
    }    
#endif
    
    
        // erase flash first
        if (VA_EraseFlash()) 
        {
            VA_MSG(MM_CAMERA, "Time out! VA_EraseFlash fail\n");
            return 1;
        }  
        // write flash_size KB data to flash
        if (VA_WriteDataToFlash(0x01000000L, 0x00000000L, 0x0000FFFFL)) 
        {
            VA_MSG(MM_CAMERA, "Time out! VA_WriteDataToFlash fail\n");
            return 1;
        }  
        // read fw to DRAM to check data correctness
        if (VA_ReadDataFromFlash(0x01010000L, 0x00000000L, 0x0000FFFFL)) 
        {
            VA_MSG(MM_CAMERA, "Time out! VA_ReadDataFromFlash fail\n");
            return 1;
        }  
        error_count = VA_CompareMem(0x01000000L, 0x01010000L, section_size);
    
        if (error_count)
        {
            VA_HIGH(MM_CAMERA, "V3A_UpdateFirmware : fail\n");
                return 1;
        }
        else
        VA_INFO(MM_CAMERA, "flash comparison finished\n");
    
        VA_HIGH(MM_CAMERA, "V3A_UpdateFirmware : end\n");
    
        return 0;
}

#endif

/*************************************************************************************************/
/* Venus : Memory                                                                                */
/*************************************************************************************************/
uint8 V3A_ReadMemAddr(uint32 addr)
{
	uint8 ret;

	transmit_byte_via_i2c(0x691E, (addr >> 27) & 0x03);
	transmit_byte_via_i2c(0x691D, (addr >> 19) & 0xFF);
	transmit_byte_via_i2c(0x691C, (addr >> 15) & 0x0F);

	ret = receive_byte_via_i2c(0x8000 + addr % 0x8000);

	VA_HIGH(MM_CAMERA, "V3A_ReadMemAddr : (Addr, Value) = (0x%08X, 0x%02X)\n", addr, ret);

	return ret;
}

uint8 V3A_WriteMemAddr(uint32 addr, uint8 value)
{
	VA_HIGH(MM_CAMERA, "V3A_WriteMemAddr : (Addr, Value) = (0x%08X, 0x%02X)\n", addr, value);

	transmit_byte_via_i2c(0x691E, (addr >> 27) & 0x03);
	transmit_byte_via_i2c(0x691D, (addr >> 19) & 0xFF);
	transmit_byte_via_i2c(0x691C, (addr >> 15) & 0x0F);

	transmit_byte_via_i2c(0x8000 + addr % 0x8000, value);

	return 0;
}

/*************************************************************************************************/
/* Venus : Camera Interface                                                                      */
/*************************************************************************************************/
uint8 V3A_TristateChipCamIF(void)
{
	uint32 timeout_count = 0;
    
	VA_HIGH(MM_CAMERA, "VA_TristateChipCamIF Start\n");
	
	while ((receive_byte_via_i2c_ISP(HOST_STATUS_ADDR) & HOST_STATUS_READY) == 0) {

		VA_DelayMS(1);

		if (++timeout_count > MAX_TIMEOUT_CNT) {
			printk("Venus API Timeout!\n");
			return 1;
		}
	}
    transmit_byte_via_i2c_ISP(0x6C01, receive_byte_via_i2c_ISP(0x6C01) | 0x40);
    
	VA_HIGH(MM_CAMERA, "VA_TristateChipCamIF End\n");
    
	return 0;
}


#endif


uint8 AccessTest(void)
{
	uint8 Result=0;
	uint16 i=0, j=0;
	uint16 temp=0;
	uint32 mem_addr=0;
	uint32 mem_size=0;
	uint8 test=0;
	uint32 error_count=0;
	uint8 *fw_ptr=0;
	uint8 *fw_read_ptr=0;
	uint32 nCntOfMulti=0;
	uint8 read_buf[132];

	VenusV3_ReadFirmwareFile();

	fw_ptr = V3_FirmwareBinPtr;
	fw_read_ptr = read_buf;//V3_FirmwareBinPtr + 0x8000;

//	V3A_ChipEnable();
//	V3A_ChipReset();
	
	msleep(10);
	
	//Reset test
	transmit_byte_via_i2c(0x69F0, 0x01);
	Result = receive_byte_via_i2c(0x69FF);
	
	if(Result == 0xA4){
		printk("AIT848 Reset OK... result = 0x%x \n", Result);
	} else{
		printk("AIT848 Reset failed... result = 0x%x \n", Result);
	}


	msleep(10);

	//SRAM test
	mem_addr=0x00100000;
	mem_size=0x400;
	test=0x55;

	// enable access program memory ; disable CPU
	transmit_byte_via_i2c(0x6901, 0x06);
	
	//Write
	for (i = 0; i < mem_size; i++) {
		V3A_WriteMemAddr(mem_addr + i, test);
	}
	
	//Read
	for (i = 0; i < mem_size; i++) {
		temp = V3A_ReadMemAddr(mem_addr + i);
		if (temp != test) 
			error_count++;
	}
	printk("Error count %d bytes.\n", error_count);

	// disable access program memory ; enable CPU
	transmit_byte_via_i2c(0x6901, 0x00);

	msleep(10);
	//SRAM bust
	error_count = 0;
	mem_addr = 0x00100000;
	nCntOfMulti = 128;//192;

	// enable access program memory ; disable CPU
	transmit_byte_via_i2c(0x6901, 0x06);

	for (i=0; i < 124*1024/0x8000; i++)
	{
		//switch bank
		transmit_byte_via_i2c(0x691E, (mem_addr >> 27) & 0x03);
		transmit_byte_via_i2c(0x691D, (mem_addr >> 19) & 0xFF);
		transmit_byte_via_i2c(0x691C, (mem_addr >> 15) & 0x0F);
		
		//Write
		transmit_multibytes_via_i2c(fw_ptr, 0x8000 + mem_addr % 0x8000, nCntOfMulti);
		//for(i=0; i<128; i++)
		//	transmit_byte_via_i2c(i  0x8000 + addr % 0x8000, fw_ptr[i]);
		
		//Read
		receive_multibytes_via_i2c(fw_read_ptr, 0x8000 + mem_addr % 0x8000, nCntOfMulti);
		//for(i=0; i<128; i++)
		//	 fw_read_ptr[i] = receive_byte_via_i2c(i);

		//Compare
		for (j = 0; j < nCntOfMulti; j++) {
				if (fw_ptr[j] != fw_read_ptr[j]) error_count++;
		}
		printk("Error count %d bytes.\n", error_count);
	}

	mem_addr +=0x8000;

	// disable access program memory ; enable CPU
	transmit_byte_via_i2c(0x6901, 0x00);

}





