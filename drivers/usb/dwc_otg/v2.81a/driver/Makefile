#
# Makefile for DWC_otg Highspeed USB controller driver
#

#PWD			:= $(shell pwd)
#PORTLIB		:= $(PWD)/../dwc_common_port
#KDIR		:= $(PWD)/../../../kernel/linux-2.6.28-tcc/


ifneq ($(KERNELRELEASE),)
#CPPFLAGS	+= -DDEBUG 

# Use one of the following flags to compile the software in host-only or
# device-only mode.
#CPPFLAGS        += -DDWC_HOST_ONLY
#CPPFLAGS        += -DDWC_DEVICE_ONLY

CPPFLAGS	+= -Dlinux #-DDWC_HS_ELECT_TST 
CPPFLAGS	+= -DDWC_EN_ISOC
CPPFLAGS   	+= -I$(PWD)/../dwc_common_port
#CPPFLAGS   	+= -I$(PORTLIB)
CPPFLAGS   	+= -DDWC_LINUX
CPPFLAGS   	+= $(CFI)

obj-m		+= dwc_otg.o

dwc_otg-objs	:= dwc_otg_driver.o dwc_otg_attr.o 
dwc_otg-objs	+= dwc_otg_cil.o dwc_otg_cil_intr.o 
dwc_otg-objs	+= dwc_otg_pcd_linux.o dwc_otg_pcd.o dwc_otg_pcd_intr.o 
dwc_otg-objs	+= dwc_otg_hcd.o dwc_otg_hcd_linux.o dwc_otg_hcd_intr.o dwc_otg_hcd_queue.o
ifneq ($(CFI),)
dwc_otg-objs	+= dwc_otg_cfi.o
endif

else

PWD				:= $(shell pwd)
PORTLIB			:= $(PWD)/../dwc_common_port

### alanK
#KDIR			:= /house/twkwon/Magellan/linux/kernel/linux-2.6.28-tcc
#CROSS_COMPILE	:= /opt/armv6/codesourcery/bin/arm-none-linux-gnueabi-
#ARCH			:= arm
ARCH			:= TEST_DO_NOT_DISPLAY_THIS_MESSAGE
#CPPFLAGS		+= -DDWC_LINUX -Dlinux -DDWC_HS_ELECT_TST -DDWC_EN_ISOC

# Command paths
CTAGS		:= $(CTAGS)
DOXYGEN		:= $(DOXYGEN)

default: portlib
	$(MAKE) -C$(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

install: default
ifneq ($(INSTALL_MOD_PATH),)
	$(MAKE) -C$(KDIR) M=$(PORTLIB) modules_install
	$(MAKE) -C$(KDIR) M=$(PWD) modules_install	
else
	@echo "No install path defined"
endif

portlib:
	$(MAKE) -C$(KDIR) M=$(PORTLIB) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules
	cp $(PORTLIB)/Module.symvers $(PWD)/
	
docs:	$(wildcard *.[hc]) doc/doxygen.cfg
	$(DOXYGEN) doc/doxygen.cfg

tags:	$(wildcard *.[hc])
	$(CTAGS) -e $(wildcard *.[hc]) $(wildcard linux/*.[hc]) $(wildcard $(KDIR)/include/linux/usb*.h)


clean:
	rm -rf   *.o *.ko .*cmd *.mod.c .tmp_versions

endif
