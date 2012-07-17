#
# Makefile for the kernel character device drivers.
#

obj-y				+= mem.o random.o
obj-$(CONFIG_TTY_PRINTK)	+= ttyprintk.o
obj-y				+= misc.o
obj-$(CONFIG_ATARI_DSP56K)	+= dsp56k.o
obj-$(CONFIG_VIRTIO_CONSOLE)	+= virtio_console.o
obj-$(CONFIG_RAW_DRIVER)	+= raw.o
obj-$(CONFIG_SGI_SNSC)		+= snsc.o snsc_event.o
obj-$(CONFIG_MSM_SMD_PKT)	+= msm_smd_pkt.o
obj-$(CONFIG_MSPEC)		+= mspec.o
obj-$(CONFIG_MMTIMER)		+= mmtimer.o
obj-$(CONFIG_UV_MMTIMER)	+= uv_mmtimer.o
obj-$(CONFIG_VIOTAPE)		+= viotape.o
obj-$(CONFIG_IBM_BSR)		+= bsr.o
obj-$(CONFIG_SGI_MBCS)		+= mbcs.o
obj-$(CONFIG_BRIQ_PANEL)	+= briq_panel.o
obj-$(CONFIG_BFIN_OTP)		+= bfin-otp.o

obj-$(CONFIG_PRINTER)		+= lp.o

obj-$(CONFIG_APM_EMULATION)	+= apm-emulation.o

obj-$(CONFIG_DTLK)		+= dtlk.o
obj-$(CONFIG_APPLICOM)		+= applicom.o
obj-$(CONFIG_SONYPI)		+= sonypi.o
obj-$(CONFIG_RTC)		+= rtc.o
obj-$(CONFIG_HPET)		+= hpet.o
obj-$(CONFIG_GEN_RTC)		+= genrtc.o
obj-$(CONFIG_EFI_RTC)		+= efirtc.o
obj-$(CONFIG_DS1302)		+= ds1302.o
obj-$(CONFIG_XILINX_HWICAP)	+= xilinx_hwicap/
ifeq ($(CONFIG_GENERIC_NVRAM),y)
  obj-$(CONFIG_NVRAM)	+= generic_nvram.o
else
  obj-$(CONFIG_NVRAM)	+= nvram.o
endif
obj-$(CONFIG_TOSHIBA)		+= toshiba.o
obj-$(CONFIG_I8K)		+= i8k.o
obj-$(CONFIG_DS1620)		+= ds1620.o
obj-$(CONFIG_HW_RANDOM)		+= hw_random/
obj-$(CONFIG_PPDEV)		+= ppdev.o
obj-$(CONFIG_NWBUTTON)		+= nwbutton.o
obj-$(CONFIG_NWFLASH)		+= nwflash.o
obj-$(CONFIG_SCx200_GPIO)	+= scx200_gpio.o
obj-$(CONFIG_PC8736x_GPIO)	+= pc8736x_gpio.o
obj-$(CONFIG_NSC_GPIO)		+= nsc_gpio.o
obj-$(CONFIG_GPIO_TB0219)	+= tb0219.o
obj-$(CONFIG_TELCLOCK)		+= tlclk.o

obj-$(CONFIG_TCC_GRAPHIC)	+= tccgrp.o tcc_Gre2d.o tcc_Gre2d_api.o
ifeq ($(CONFIG_ARCH_TCC892X), y)
    obj-$(CONFIG_TCC_MEMSCALER) 	+= tcc_scaler.o 
    obj-$(CONFIG_TCC_MEMSCALER1) 	+= tcc_scaler1.o 
    obj-$(CONFIG_TCC_MEMSCALER2) 	+= tcc_scaler2.o 
    obj-$(CONFIG_TCC_WMIXER) 		+= tcc_wmixer.o 
else
    obj-$(CONFIG_TCC_MEMSCALER)	 += tcc_scaler.o tcc_scaler_ctrl.o
    obj-$(CONFIG_TCC_MEMSCALER1) += tcc_scaler1.o
endif
obj-$(CONFIG_TCC_HW_JPEG_ENC)	+= tcc_jpegenc.o tcc_jpegenc_app.o
obj-$(CONFIG_TCC_HW_JPEG_DEC)	+= tcc_jpegdec.o tcc_jpegdec_app.o
obj-$(CONFIG_TCC_VPU)		+= tcc_vpu.o
obj-$(CONFIG_TCC_CIPHER)    += tcc_cipher.o
obj-$(CONFIG_TCC_MEM)		+= tcc_mem.o

obj-$(CONFIG_MWAVE)		+= mwave/
obj-$(CONFIG_AGP)		+= agp/
obj-$(CONFIG_PCMCIA)		+= pcmcia/
obj-$(CONFIG_IPMI_HANDLER)	+= ipmi/

obj-$(CONFIG_HANGCHECK_TIMER)	+= hangcheck-timer.o
obj-$(CONFIG_TCG_TPM)		+= tpm/

obj-$(CONFIG_DCC_TTY)		+= dcc_tty.o
obj-$(CONFIG_PS3_FLASH)		+= ps3flash.o
obj-$(CONFIG_RAMOOPS)		+= ramoops.o

obj-$(CONFIG_JS_RTC)		+= js-rtc.o
js-rtc-y = rtc.o

obj-$(CONFIG_ARCH_TCC)    += tcc_dxb_ctrl/ 
obj-$(CONFIG_TCC_USER_INTR) += tcc_intr.o
obj-$(CONFIG_TCC_BT_DEV)       += tcc_bt_dev.o
obj-$(CONFIG_TCC_HDMI_DRIVER)	+= hdmi/

obj-$(CONFIG_SATA_TCC)		+= tcc_ll.o

obj-$(CONFIG_TCC_OUTPUT_STARTER)    += tcc_output_starter.o

# For Android Framework
obj-$(CONFIG_CPU_FREQ)		+= tcc_clockctrl.o

# S: telechips
obj-$(CONFIG_SENSOR_BMA150)		+= sensor_bma150.o
obj-$(CONFIG_SENSOR_BMA220)		+= sensor_bma220.o
obj-$(CONFIG_SENSOR_BMA023)		+= sensor_bma023.o
obj-$(CONFIG_SENSOR_BMA250)		+= sensor_bma250.o

obj-$(CONFIG_WIFI_PWR_CTL)		+= wifi_pwr/
obj-$(CONFIG_WPS_PBC)		+= wps_pbc/
# E: telechips
