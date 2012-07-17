#
# Makefile for Telechips TCC88xx processors
#

# Common support

obj-$(CONFIG_ARCH_TCC892X)      += cpu_early_init.o
obj-$(CONFIG_ARCH_TCC892X)      += time.o irq.o io.o clock.o tcc_ddr.o
obj-$(CONFIG_ARCH_TCC892X)      += pm.o gpio.o 
obj-$(CONFIG_ARCH_TCC892X)      += devices.o
obj-$(CONFIG_ARCH_TCC892X)      += tca_ckc.o
obj-$(CONFIG_ARCH_TCC892X)      += tca_adconv.o tcc_adc.o
obj-$(CONFIG_ARCH_TCC892X)      += tcc_asm.o
obj-$(CONFIG_ARCH_TCC892X)      += tca_serial.o
obj-$(CONFIG_ARCH_TCC892X)      += tca_i2c.o
obj-$(CONFIG_ARCH_TCC892X)      += tca_spi.o tca_tco.o
obj-$(CONFIG_ARCH_TCC892X)      += tca_tsif.o
obj-$(CONFIG_ARCH_TCC892X)      += tca_intr.o
obj-$(CONFIG_ARCH_TCC892X)      += tccfb_address.o
obj-$(CONFIG_ARCH_TCC892X)      += vioc/vioc_disp.o vioc/vioc_outcfg.o vioc/vioc_rdma.o vioc/vioc_wdma.o vioc/vioc_wmix.o vioc/vioc_viqe.o \
                                                           vioc/vioc_scaler.o vioc/vioc_ireq.o vioc/vioc_api.o vioc/vioc_config.o vioc/vioc_vin.o

obj-$(CONFIG_TCC_CIPHER)		+= tca_cipher.o
obj-$(CONFIG_CPU_FREQ)          += cpufreq.o
obj-$(CONFIG_SERIAL_TCC_SC) += tca_sc.o 

obj-$(CONFIG_TCC_ECID_SUPPORT)  += tcc_ecid.o

# Specific board support
obj-$(CONFIG_MACH_TCC8920)      += board-tcc8920.o board-tcc8920-gpio.o board-tcc8920-keypad.o board-tcc8920-power.o
obj-$(CONFIG_MACH_TCC8920)      += board-tcc8920-panel.o
obj-$(CONFIG_MACH_TCC8920)      += board-tcc8920-mmc.o
obj-$(CONFIG_MACH_TCC8920)      += board-tcc8920-dxb.o
obj-$(CONFIG_MACH_TCC8920)      += board-tcc8920-hdmi.o
obj-$(CONFIG_MACH_TCC8920)      += board-tcc8920-bluetooth.o
obj-$(CONFIG_MACH_TCC8920)      += board-tcc8920-camera.o
obj-$(CONFIG_MACH_TCC8920)      += board-tcc8920-pmic.o
# obj-$(CONFIG_INPUT_TCC_REMOTE) += board-tcc8920-remocon.o
obj-$(CONFIG_MACH_TCC8920ST)     += board-tcc8920st.o board-tcc8920st-gpio.o board-tcc8920st-keypad.o board-tcc8920st-power.o
obj-$(CONFIG_MACH_TCC8920ST)     += board-tcc8920st-panel.o
obj-$(CONFIG_MACH_TCC8920ST)    += board-tcc8920st-mmc.o
obj-$(CONFIG_MACH_TCC8920ST)    += board-tcc8920st-dxb.o
obj-$(CONFIG_MACH_TCC8920ST)    += board-tcc8920st-hdmi.o
obj-$(CONFIG_INPUT_TCC_REMOTE)  += board-tcc8920st-remocon.o
obj-$(CONFIG_MACH_TCC8920ST)    += board-tcc8920st-camera.o
obj-$(CONFIG_MACH_TCC8920ST)    += tcc_display.o
obj-$(CONFIG_MACH_M805_892X)    += board-m805_892x.o board-m805_892x-gpio.o board-m805_892x-keypad.o board-m805_892x-power.o
obj-$(CONFIG_MACH_M805_892X)    += board-m805_892x-panel.o
obj-$(CONFIG_MACH_M805_892X)    += board-m805_892x-mmc.o
obj-$(CONFIG_MACH_M805_892X)    += board-m805_892x-dxb.o
obj-$(CONFIG_MACH_M805_892X)    += board-m805_892x-hdmi.o
obj-$(CONFIG_MACH_M805_892X)    += board-m805_892x-camera.o
obj-$(CONFIG_MACH_M805_892X)    += board-m805_892x-pmic.o
