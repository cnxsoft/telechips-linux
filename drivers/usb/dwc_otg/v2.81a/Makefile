#
# Synopsys DWC OTG Controller Drivers
#       for Telechips System-on-Chips
#

PORTLIB		:= dwc_common_port
OTGDRV		:= driver

obj-$(CONFIG_TCC_DWC_OTG) += tcc_dwc_otg.o

tcc_dwc_otg-objs	:= $(PORTLIB)/dwc_cc.o \
					   $(PORTLIB)/dwc_modpow.o \
					   $(PORTLIB)/dwc_dh.o \
					   $(PORTLIB)/dwc_crypto.o \
					   $(PORTLIB)/dwc_notifier.o \
					   $(PORTLIB)/dwc_common_linux.o \
					   $(PORTLIB)/dwc_mem.o

tcc_dwc_otg-objs	+= $(OTGDRV)/dwc_otg_driver.o \
					   $(OTGDRV)/dwc_otg_attr.o \
					   $(OTGDRV)/dwc_otg_cil.o \
					   $(OTGDRV)/dwc_otg_cil_intr.o \
					   $(OTGDRV)/dwc_otg_pcd_linux.o \
					   $(OTGDRV)/dwc_otg_pcd.o \
					   $(OTGDRV)/dwc_otg_pcd_intr.o \
					   $(OTGDRV)/dwc_otg_hcd.o \
					   $(OTGDRV)/dwc_otg_hcd_linux.o \
					   $(OTGDRV)/dwc_otg_hcd_intr.o \
					   $(OTGDRV)/dwc_otg_hcd_queue.o
ifneq ($(CFI),)
tcc_dwc_otg-objs	+= dwc_otg_cfi.o
endif

tcc_dwc_otg-objs	+= $(OTGDRV)/tcc_usb_phy.o


#
# DWC_OTG Define
#
EXTRA_CFLAGS	+= -DDWC_LINUX -Dlinux #-DDEBUG
EXTRA_CFLAGS	+= $(CFI)
EXTRA_CFLAGS	+= -I$(PWD)/drivers/usb/dwc_otg/v2.81a/$(PORTLIB)
#EXTRA_CFLAGS	+= -DCONFIG_USB_DWC_OTG_LPM				### LPM (Link Power Management) support --> move Kconfig?
#EXTRA_CFLAGS	+= -DDWC_UTE_CFI						### CFI interface support
#EXTRA_CFLAGS	+= -DDWC_EN_ISOC						### _EN_ is enhancement? or enable?

# 
# TCC_DWC_OTG Define
#
#EXTRA_CFLAGS	+= -DDWC_HOST_NON_ALIGN_BUF_PATCH		### CHECK: non-aligned buffer patch
#EXTRA_CFLAGS	+= -DDWC_HOST_BANDWIDTH_PATCH			### TODO: bandwidth caculation patch

# 
# OTG Mode
#
ifeq ($(CONFIG_TCC_DWC_OTG_DUAL_ROLE),y)
	EXTRA_CFLAGS += -DDWC_DUAL_ROLE
endif
ifeq ($(CONFIG_TCC_DWC_OTG_DEVICE_ONLY),y)
	EXTRA_CFLAGS += -DDWC_DEVICE_ONLY
endif
ifeq ($(CONFIG_TCC_DWC_OTG_HOST_ONLY),y)
	EXTRA_CFLAGS += -DDWC_HOST_ONLY
endif

#
# Debugging
#
ifeq ($(CONFIG_TCC_DWC_OTG_DEBUG),y)
	EXTRA_CFLAGS += -DDEBUG
	EXTRA_CFLAGS += -DDEBUG_EP0
#	EXTRA_CFLAGS += -DDEBUG_MEMORY
endif

