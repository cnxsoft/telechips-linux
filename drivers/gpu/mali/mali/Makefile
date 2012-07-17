#
# Copyright (C) 2010-2012 ARM Limited. All rights reserved.
# 
# This program is free software and is provided to you under the terms of the GNU General Public License version 2
# as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
# 
# A copy of the licence is included with the program, and can also be obtained from Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#

OSKOS=linux
FILES_PREFIX=

MALI_RELEASE_NAME = r3p0-04rel0

ARCH = arm
BUILD = release
ifeq ($(CONFIG_MALI_UMP),y)
USING_UMP = 1
else
USING_UMP = 0
endif
ifeq ($(CONFIG_MALI_OS_MEMORY),y)
USING_OS_MEMORY = 1
else
USING_OS_MEMORY = 0
endif
ifeq ($(CONFIG_GPU_BUS_SCALING),y)
USING_GPU_UTILIZATION = 1
else
USING_GPU_UTILIZATION = 0
endif
ifeq ($(CONFIG_ARCH_TCC),y)
  ifeq ($(CONFIG_ARCH_TCC892X),y)
  TARGET_PLATFORM = tcc-m400
  else
  TARGET_PLATFORM = tcc-m200
  endif
  CONFIG = $(TARGET_PLATFORM)
endif

# set up defaults if not defined by the user
ARCH = arm
USING_MALI_RUN_TIME_PM ?= 1
USING_MALI_PMM_TESTSUITE ?= 0
OS_MEMORY_KERNEL_BUFFER_SIZE_IN_MB ?= 6
USING_PROFILING ?= 0
USING_INTERNAL_PROFILING ?= 0
DISABLE_PP0 ?= 0
DISABLE_PP1 ?= 0
DISABLE_PP2 ?= 0
DISABLE_PP3 ?= 0
TIMESTAMP ?= default
KERNEL_RUNTIME_PM_ENABLED ?= 0
MALI_PP_SCHEDULER_FORCE_NO_JOB_OVERLAP ?= 0
MALI_PP_SCHEDULER_KEEP_SUB_JOB_STARTS_ALIGNED ?= 0
MALI_PP_SCHEDULER_FORCE_NO_JOB_OVERLAP_BETWEEN_APPS ?= 0

DEFINES := $(EXTRA_DEFINES)


EXTRA_CFLAGS += -I$(src)/arch-$(CONFIG)

ifeq ($(USING_PROFILING),1)
ifeq ($(USING_INTERNAL_PROFILING),0)
ifndef CONFIG_TRACEPOINTS
# Should default to gator profiling, but we dont have the kernel feature required, so disable profiling
override USING_PROFILING = 0
$(warning "CONFIG_TRACEPOINTS required for USING_PROFILING")
endif
endif
endif

ifeq ($(USING_PROFILING),0)
# make sure user hasnt selected incompatible flags
override USING_INTERNAL_PROFILING = 0
endif

# Check if a Mali Core sub module should be enabled, true or false returned
ifneq ($(CONFIG),)
submodule_enabled = $(shell gcc $(DEFINES) -E $(1)/arch-$(CONFIG)/config.h | grep type | grep -c $(2))
endif

# This conditional makefile exports the global definition ARM_INTERNAL_BUILD. Customer releases will not include arm_internal.mak
-include ../../../arm_internal.mak

OSKFILES = \
	$(FILES_PREFIX)$(OSKOS)/mali_osk_atomics.c \
	$(FILES_PREFIX)$(OSKOS)/mali_osk_irq.c \
	$(FILES_PREFIX)$(OSKOS)/mali_osk_locks.c \
	$(FILES_PREFIX)$(OSKOS)/mali_osk_wait_queue.c \
	$(FILES_PREFIX)$(OSKOS)/mali_osk_low_level_mem.c \
	$(FILES_PREFIX)$(OSKOS)/mali_osk_math.c \
	$(FILES_PREFIX)$(OSKOS)/mali_osk_memory.c \
	$(FILES_PREFIX)$(OSKOS)/mali_osk_misc.c \
	$(FILES_PREFIX)$(OSKOS)/mali_osk_mali.c \
	$(FILES_PREFIX)$(OSKOS)/mali_osk_notification.c \
	$(FILES_PREFIX)$(OSKOS)/mali_osk_time.c \
	$(FILES_PREFIX)$(OSKOS)/mali_osk_timers.c

UKKFILES = \
	$(FILES_PREFIX)$(OSKOS)/mali_ukk_mem.c \
	$(FILES_PREFIX)$(OSKOS)/mali_ukk_gp.c \
	$(FILES_PREFIX)$(OSKOS)/mali_ukk_pp.c \
	$(FILES_PREFIX)$(OSKOS)/mali_ukk_core.c

ifeq ($(USING_PROFILING),1)
UKKFILES += \
	$(FILES_PREFIX)$(OSKOS)/mali_ukk_profiling.c
endif

ifeq ($(MALI_PLATFORM_FILE),)
MALI_PLATFORM_FILE = platform/default/mali_platform.c
endif

# Get subversion revision number, fall back to only ${MALI_RELEASE_NAME} if no svn info is available
SVN_REV := $(shell (cd $(DRIVER_DIR); (svnversion | grep -qv exported && svnversion) || git svn info | grep '^Revision: '| sed -e 's/^Revision: //' ) 2>/dev/null )
ifeq ($(SVN_REV),)
SVN_REV := $(MALI_RELEASE_NAME)
else
SVN_REV := $(MALI_RELEASE_NAME)-r$(SVN_REV)
endif

# Set up our defines, which will be passed to gcc
DEFINES += -DUSING_OS_MEMORY=$(USING_OS_MEMORY)
DEFINES += -DUSING_MMU=1
DEFINES += -DUSING_UMP=$(USING_UMP)
DEFINES += -D_MALI_OSK_SPECIFIC_INDIRECT_MMAP
DEFINES += -DMALI_INTERNAL_TIMELINE_PROFILING_ENABLED=$(USING_INTERNAL_PROFILING)
DEFINES += -DDISABLE_PP0=$(DISABLE_PP0)
DEFINES += -DDISABLE_PP1=$(DISABLE_PP1)
DEFINES += -DDISABLE_PP2=$(DISABLE_PP2)
DEFINES += -DDISABLE_PP3=$(DISABLE_PP3)
DEFINES += -DMALI_PP_SCHEDULER_FORCE_NO_JOB_OVERLAP=$(MALI_PP_SCHEDULER_FORCE_NO_JOB_OVERLAP)
DEFINES += -DMALI_PP_SCHEDULER_KEEP_SUB_JOB_STARTS_ALIGNED=$(MALI_PP_SCHEDULER_KEEP_SUB_JOB_STARTS_ALIGNED)
DEFINES += -DMALI_PP_SCHEDULER_FORCE_NO_JOB_OVERLAP_BETWEEN_APPS=$(MALI_PP_SCHEDULER_FORCE_NO_JOB_OVERLAP_BETWEEN_APPS)
DEFINES += -DMALI_TIMELINE_PROFILING_ENABLED=$(USING_PROFILING)
DEFINES += -DMALI_POWER_MGMT_TEST_SUITE=$(USING_MALI_PMM_TESTSUITE)
ifeq ($(shell test $(SUBLEVEL) -gt 32 -a $(PATCHLEVEL) = 6 -a $(VERSION) = 2 -o $(VERSION) -gt 2 && echo "OK"),OK)
# MALI_STATE_TRACKING is only supported on Linux kernels from version 2.6.32.
DEFINES += -DMALI_STATE_TRACKING=1
else
DEFINES += -DMALI_STATE_TRACKING=0
endif
DEFINES += -DMALI_OS_MEMORY_KERNEL_BUFFER_SIZE_IN_MB=$(OS_MEMORY_KERNEL_BUFFER_SIZE_IN_MB)

MALI_PLATFORM_FILE = platform/$(TARGET_PLATFORM)/mali_platform.c

DEFINES += -DMALI_GPU_UTILIZATION=$(USING_GPU_UTILIZATION)

ifdef CONFIG_PM
ifdef CONFIG_PM_RUNTIME
	KERNEL_RUNTIME_PM_ENABLED = 1
endif
endif

DEFINES += -DMALI_PMM_RUNTIME_JOB_CONTROL_ON=$(KERNEL_RUNTIME_PM_ENABLED)

ifeq ($(BUILD), debug)
DEFINES += -DDEBUG
endif
DEFINES += -DSVN_REV=$(SVN_REV)
DEFINES += -DSVN_REV_STRING=\"$(SVN_REV)\"

# Linux has its own mmap cleanup handlers (see mali_kernel_memory.c)
DEFINES += -DMALI_UKK_HAS_IMPLICIT_MMAP_CLEANUP

ifeq ($(USING_UMP),1)
	DEFINES += -DMALI_USE_UNIFIED_MEMORY_PROVIDER=1
	EXTRA_CFLAGS += -I$(src)/../ump/include/ump
else
	DEFINES += -DMALI_USE_UNIFIED_MEMORY_PROVIDER=0
endif

# Use our defines when compiling
EXTRA_CFLAGS += $(DEFINES) -I$(src) -I$(src)/include -I$(src)/common -I$(src)/linux -I$(src)/platform

# For customer releases the Linux Device Drivers will be provided as ARM proprietary and GPL releases:
# The ARM proprietary product will only include the license/proprietary directory
# The GPL product will only include the license/gpl directory
ifeq ($(wildcard $(src)/linux/license/gpl/*),)
EXTRA_CFLAGS += -I$(src)/linux/license/proprietary
# Disable profiling for proprietary
override USING_PROFILING := 0
$(warning "USING_PROFILING not supported, disabling.")
else
EXTRA_CFLAGS += -I$(src)/linux/license/gpl
endif
# Source files which always are included in a build
SRC = \
	common/mali_kernel_core.c \
	linux/mali_kernel_linux.c \
	$(OSKOS)/mali_osk_indir_mmap.c \
	common/mali_kernel_descriptor_mapping.c \
	common/mali_session.c \
	common/mali_device_pause_resume.c \
	common/mali_kernel_vsync.c \
	linux/mali_ukk_vsync.c \
	linux/mali_kernel_sysfs.c \
	common/mali_mmu.c \
	common/mali_mmu_page_directory.c \
	common/mali_memory.c \
	common/mali_kernel_memory_engine.c \
	common/mali_block_allocator.c \
	common/mali_kernel_mem_os.c \
	common/mali_mem_validation.c \
	common/mali_hw_core.c \
	common/mali_gp.c \
	common/mali_pp.c \
	common/mali_pp_job.c \
	common/mali_gp_job.c \
	common/mali_scheduler.c \
	common/mali_gp_scheduler.c \
	common/mali_pp_scheduler.c \
	common/mali_cluster.c \
	common/mali_group.c \
	common/mali_dlbu.c \
	common/mali_pm.c \
	common/mali_pmu.c \
	common/mali_user_settings_db.c \
	$(OSKOS)/mali_osk_pm.c \
	linux/mali_kernel_pm.c \
	linux/mali_pmu_power_up_down.c \
	$(MALI_PLATFORM_FILE) \
	$(OSKFILES) \
	$(UKKFILES)

# Selecting files to compile by parsing the config file

ifeq ($(USING_INTERNAL_PROFILING),1)
PROFILING_BACKEND_SOURCES = \
    linux/mali_osk_profiling_internal.c \
    timestamp-$(TIMESTAMP)/mali_timestamp.c
EXTRA_CFLAGS += -I$(src)/timestamp-$(TIMESTAMP)
else
ifeq ($(USING_PROFILING),1)
PROFILING_BACKEND_SOURCES = \
    linux/mali_osk_profiling_gator.c
endif
endif

# Add the profiling sources
SRC += $(PROFILING_BACKEND_SOURCES)

ifeq ($(USING_MALI_PMM_TESTSUITE),1)
EXTRA_CFLAGS += -I$(src)/platform/mali_pmu_testing
endif

ifeq ($(USING_GPU_UTILIZATION),1)
SRC += \
	common/mali_kernel_utilization.c
endif

ifeq ($(CONFIG_GPU_BUS_SCALING),y)
SRC += \
	platform/$(TARGET_PLATFORM)/mali_platform_dvfs.c
endif
ifneq ($(call submodule_enabled, $(src), MALI400PP),0)
	# Mali-400 PP in use
	EXTRA_CFLAGS += -DUSING_MALI400
endif

ifneq ($(call submodule_enabled, $(src), MALI300PP),0)
	# Mali-400 PP in use
	EXTRA_CFLAGS += -DUSING_MALI400
endif

ifneq ($(call submodule_enabled, $(src), MALI200),0)
	# Mali200 in use
	EXTRA_CFLAGS += -DUSING_MALI200
endif

# Always build in support for Mali L2 cache
SRC += common/mali_l2_cache.c

# Tell the Linux build system to enable building of our .c files
# Tell the Linux build system from which .o file to create the kernel module
obj-$(CONFIG_MALI_GPU) += mali.o

mali-y := $(SRC:.c=.o)
