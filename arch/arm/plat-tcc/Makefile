# "Telechips Platform Common Modules"

#obj-y := clock.o system.o
obj-$(CONFIG_ARCH_TCC_926) := clock.o system.o


obj-$(CONFIG_TCA_CKC)	+= tcc_ckc.o

obj-$(CONFIG_TCA_PM)	+= tcc_pm.o
ifeq ($(CONFIG_PM),y)
obj-$(CONFIG_TCA_PM)	+= tcc_asm.o
endif

obj-$(CONFIG_DPM) 	+= tcc_dpm.o

obj-$(CONFIG_ARCH_TCC)	+= nand.o pmap.o
