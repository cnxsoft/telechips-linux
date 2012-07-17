#ifndef __ITV_COMMON_H__
#define __ITV_COMMON_H__

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/mutex.h>
#include <linux/poll.h>
#include <linux/ioctl.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <asm/system.h>
#include <linux/vmalloc.h>
#include <linux/kthread.h>
#include <linux/freezer.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/firmware.h>

#include <linux/i-tv/version.h>
#include <linux/i-tv/demux.h>
#include <linux/i-tv/frontend.h>

// Error printk
#define eprintk(args...) \
	do { printk("[ERR] %s() ----> ", __func__); printk(args); } while (0)

// Warning printk
#define wprintk(args...) \
	do { printk("[WARN] %s() ----> ", __func__); printk(args); } while (0)

#define ITV_UNSET			(-1)
#define ITV_MAX_ADAPTERS	8

#define ITV_MAX_DEV_IDS		4
#define CONV2MINOR(num, type, id)	((num << 6) | (id << 4) | (type))

#define ITV_MAJOR			221
#define ITV_MINORS			(ITV_MAX_ADAPTERS * 64)

#define ITV_EXTERN	extern
#define ITV_EXPORT(type, name, args)		\
	ITV_EXTERN type name args;

typedef enum {
	ITV_STD_TYPE_ATSC, 
	ITV_STD_TYPE_DVBC
} itv_std_type_e;

typedef enum {
	ITV_IF_TYPE_TS_PARALLEL
} itv_if_type_e;

typedef enum {
	ITV_TSIF_MODE_PARALLEL, 
	ITV_TSIF_MODE_SERIAL
} itv_tsif_mode_e;

#include <i-tv/itv_arrays.h>
#include <i-tv/itv_object.h>
#include <i-tv/itv_device.h>
#include <i-tv/itv_module.h>
#include <i-tv/itv_core.h>

#endif	/* __ITV_COMMON_H__ */
