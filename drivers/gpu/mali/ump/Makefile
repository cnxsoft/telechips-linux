#
# Copyright (C) 2010-2012 ARM Limited. All rights reserved.
# 
# This program is free software and is provided to you under the terms of the GNU General Public License version 2
# as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
# 
# A copy of the licence is included with the program, and can also be obtained from Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#

UMP_FILE_PREFIX =

ifneq ($(KBUILD_EXTMOD),)
include $(KBUILD_EXTMOD)/Makefile.common
else
include $(src)/Makefile.common
endif

# For each arch check: CROSS_COMPILE , KDIR , CFLAGS += -DARCH

ARCH ?= arm
## @note Should allow overriding of building UMP for non-debug:

EXTRA_CFLAGS += -I$(src) -I$(src)/common -I$(src)/linux -I$(src)/../mali/common -I$(src)/../mali/linux -I$(src)/include/ump
EXTRA_CFLAGS += -I$(src)/arch
EXTRA_CFLAGS += -DDEBUG -DMALI_STATE_TRACKING=0

# For customer releases the Linux Device Drivers will be provided as ARM proprietary and GPL releases:
# The ARM proprietary product will only include the license/proprietary directory
# The GPL product will only include the license/gpl directory

ifeq ($(wildcard $(src)/linux/license/gpl/*),)
EXTRA_CFLAGS += -I$(src)/linux/license/proprietary
else
EXTRA_CFLAGS += -I$(src)/linux/license/gpl
endif

SRC +=	$(UMP_FILE_PREFIX)common/ump_kernel_common.c \
	$(UMP_FILE_PREFIX)common/ump_kernel_descriptor_mapping.c \
	$(UMP_FILE_PREFIX)common/ump_kernel_api.c \
	$(UMP_FILE_PREFIX)common/ump_kernel_ref_drv.c \
	$(UMP_FILE_PREFIX)linux/ump_kernel_linux.c \
	$(UMP_FILE_PREFIX)linux/ump_kernel_memory_backend_os.c \
	$(UMP_FILE_PREFIX)linux/ump_kernel_memory_backend_dedicated.c \
	$(UMP_FILE_PREFIX)linux/ump_memory_backend.c \
	$(UMP_FILE_PREFIX)linux/ump_ukk_wrappers.c \
	$(UMP_FILE_PREFIX)linux/ump_ukk_ref_wrappers.c \
	$(UMP_FILE_PREFIX)linux/ump_osk_atomics.c \
	$(UMP_FILE_PREFIX)linux/ump_osk_low_level_mem.c \
	$(UMP_FILE_PREFIX)linux/ump_osk_misc.c

# Selecting files to compile by parsing the config file

obj-$(CONFIG_MALI_UMP) += ump.o

ump-y := $(SRC:.c=.o)
