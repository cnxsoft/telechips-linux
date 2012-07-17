#
# Makefile for Telechips TCC88xx processors
#

# Common support
obj-$(CONFIG_ARCH_TCC88XX)      += cpu_early_init.o
obj-$(CONFIG_ARCH_TCC88XX)      += time.o irq.o io.o idle.o clock.o tcc_ddr.o
obj-$(CONFIG_ARCH_TCC88XX)      += pm.o gpio.o 
obj-$(CONFIG_ARCH_TCC88XX)      += devices.o
obj-$(CONFIG_ARCH_TCC88XX)      += tca_ckc.o
obj-$(CONFIG_ARCH_TCC88XX)      += tca_adconv.o tcc_adc.o
obj-$(CONFIG_ARCH_TCC88XX)      += tcc_asm.o
obj-$(CONFIG_ARCH_TCC88XX)      += tca_serial.o
obj-$(CONFIG_ARCH_TCC88XX)      += tca_i2c.o
obj-$(CONFIG_ARCH_TCC88XX)      += tca_spi.o tca_tco.o
obj-$(CONFIG_ARCH_TCC88XX)      += tca_tsif.o
obj-$(CONFIG_ARCH_TCC88XX)      += tca_intr.o
obj-$(CONFIG_CPU_FREQ)          += cpufreq.o

obj-$(CONFIG_ARCH_TCC88XX)		+= tca_cipher.o
obj-$(CONFIG_SERIAL_TCC_SC) += tca_sc.o 

obj-$(CONFIG_TCC_ECID_SUPPORT)  += tcc_ecid.o

# Specific board support
obj-$(CONFIG_MACH_TCC8800)      += board-tcc8800.o board-tcc8800-gpio.o board-tcc8800-keypad.o board-tcc8800-power.o
obj-$(CONFIG_MACH_TCC8800)      += board-tcc8800-panel.o
obj-$(CONFIG_MACH_TCC8800)      += board-tcc8800-mmc.o
obj-$(CONFIG_MACH_TCC8800)      += board-tcc8800-dxb.o
obj-$(CONFIG_MACH_TCC8800)      += board-tcc8800-hdmi.o
obj-$(CONFIG_TCC_BT_DEV)        += board-tcc8800-bluetooth.o
obj-$(CONFIG_MACH_TCC8800)        += board-tcc8800-camera.o
obj-$(CONFIG_MACH_M801_88)      += board-m801_88.o board-m801_88-gpio.o board-m801_88-keypad.o
obj-$(CONFIG_MACH_M801_88)      += board-m801_88-panel.o
obj-$(CONFIG_MACH_M801_88)      += board-m801_88-mmc.o
obj-$(CONFIG_MACH_M801_88)      += board-m801_88-dxb.o
obj-$(CONFIG_MACH_M801_88)      += board-m801_88-camera.o
obj-$(CONFIG_MACH_TCC8800ST)    += board-tcc8800st.o board-tcc8800st-gpio.o board-tcc8800st-keypad.o
obj-$(CONFIG_MACH_TCC8800ST)    += board-tcc8800st-panel.o
obj-$(CONFIG_MACH_TCC8800ST)    += board-tcc8800st-mmc.o
obj-$(CONFIG_MACH_TCC8800ST)	+= board-tcc8800st-hdmi.o
obj-$(CONFIG_MACH_TCC8800ST)	+= board-tcc8800st-dxb.o
obj-$(CONFIG_MACH_TCC8800ST)	+= board-tcc8800st-camera.o
obj-$(CONFIG_MACH_TCC8800ST)    += tcc_display.o
obj-$(CONFIG_MACH_M803)         += board-m803.o board-m803-gpio.o board-m803-keypad.o
obj-$(CONFIG_MACH_M803)         += board-m803-panel.o
obj-$(CONFIG_MACH_M803)         += board-m803-mmc.o
obj-$(CONFIG_MACH_M803)         += board-m803-camera.o

obj-$(CONFIG_INPUT_TCC_REMOTE)  += board-tcc8800-remocon.o
