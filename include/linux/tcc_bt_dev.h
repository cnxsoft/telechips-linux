/*
 * File:        include/linux/tcc_bt_dev.h
 */
 
#ifndef __TCC_BT_DEV_H__
#define __TCC_BT_DEV_H__

#define BT_DEV_MAJOR_NUM 234
#define BT_DEV_MINOR_NUM 1

#define IOCTL_BT_DEV_POWER   	_IO(BT_DEV_MAJOR_NUM, 100)
#define IOCTL_BT_DEV_SPECIFIC	_IO(BT_DEV_MAJOR_NUM, 101)
#define IOCTL_BT_DEV_CLOCK_LIMIT    _IO(BT_DEV_MAJOR_NUM, 105)

#define BT_DEV_ON					1
#define BT_DEV_OFF					0

#endif

