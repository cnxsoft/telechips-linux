/*
 *  drivers/media/video/tcccam/camera_core.h
 *
 * Copyright (C) 2008 Telechips, Inc. 
 * 
 * This package is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
 * 
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED 
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. 
 */

#ifndef CAMERA_CORE__H
#define CAMERA_CORE__H

struct camera_fh;

#include <media/videobuf-dma-sg.h>
#include <asm/scatterlist.h>
#include <linux/wakelock.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif
struct tccxxx_camera_device;

typedef void (*dma_callback_t)(void *arg1, void *arg2);
 
/* per-device data structure */
struct tccxxx_camera_device {
	struct device dev;
	struct video_device *vfd;
	
	spinlock_t overlay_lock;        /* spinlock for overlay DMA counter */
	int overlay_cnt;                /* count of queued overlay DMA xfers */
	unsigned long overlay_base_phys;
	unsigned long overlay_base;
	unsigned long overlay_size;

	spinlock_t vbq_lock;            /* spinlock for videobuf queues */
	unsigned long field_count;      /* field counter for videobuf_buffer */

	/* The img_lock is used to serialize access to the image parameters for 
	 * overlay and capture.  Need to use spin_lock_irq when writing to the 
	 * reading, streaming, and previewing parameters.  A regular spin_lock 
	 * will suffice for all other cases.
	 */
	spinlock_t img_lock;
 
 	/* We allow reading from at most one filehandle at a time.
 	 * non-NULL means reading is in progress.
 	 */
 	struct camera_fh *reading;
 	/* We allow streaming from at most one filehandle at a time.  
 	 * non-NULL means streaming is in progress.
 	 */
	struct camera_fh *streaming;
	/* We allow previewing from at most one filehandle at a time.  
	 * non-NULL means previewing is in progress.
	 */
	struct camera_fh *previewing;

	/* capture parameters (frame rate, number of buffers) */
	struct v4l2_captureparm cparm;

	/* This is the frame period actually requested by the user. */
	struct v4l2_fract nominal_timeperframe;
	
	/* frequency (in Hz) of camera interface xclk output */
	unsigned long xclk;

	/* Pointer to the sensor interface ops */
	struct tcc_camera_sensor *cam_sensor;
	void *sensor_data;
	
	/* Pointer to the camera interface hardware ops */
	struct camera_hardware *cam_hardware;
	void *hardware_data;                                     //struct TCCxxxCIF 

	/* pix defines the size and pixel format of the image captured by the 
	 * sensor.  This also defines the size of the framebuffers.  The 
	 * same pool of framebuffers is used for video capture and video 
	 * overlay.  These parameters are set/queried by the 
	 * VIDIOC_S_FMT/VIDIOC_G_FMT ioctls with a CAPTURE buffer type.
	 */
	struct v4l2_pix_format pix;
	struct v4l2_pix_format pix2;

	/* crop defines the size and offset of the video overlay source window 
	 * within the framebuffer.  These parameters are set/queried by the 
	 * VIDIOC_S_CROP/VIDIOC_G_CROP ioctls with an OVERLAY buffer type.  
	 * The cropping rectangle allows a subset of the captured image to be 
	 * previewed.  It only affects the portion of the image previewed, not 
	 * captured; the entire camera image is always captured.
	 */
	struct v4l2_rect crop;

	/* win defines the size and offset of the video overlay target window 
	 * within the video display.  These parameters are set/queried by the 
	 * VIDIOC_S_FMT/VIDIOC_G_FMT ioctls with an OVERLAY buffer type.
	 */
	struct v4l2_window win;

	/* fbuf reflects the size of the video display.  It is queried with the 
	 * VIDIOC_G_FBUF ioctl.  The size of the video display cannot be 
	 * changed with the VIDIOC_S_FBUF ioctl.
	 */
	struct v4l2_framebuffer fbuf;

	/* end of generic stuff, the above should be common to all omaps */

	/* note, 2420 uses videobuf to do caprure, it is more memory efficient
	   we need 1710 and 2420 do capture in the same way */
	/* Variables to store the capture state */
	/* Wait till DMA is completed */
	//wkjung
	//wait_queue_head_t new_video_frame;

	char capture_completed;
	char capture_started;
 	spinlock_t capture_lock;
	unsigned long capture_base;
	unsigned long capture_base_phys;
	struct mutex	lock;
	char active;
};

/* per-filehandle data structure */
struct camera_fh {
	struct tccxxx_camera_device *cam;
	enum v4l2_buf_type type;
};

#define CAM_NAME "tccxxx-camera"

#endif /* CAMERA_CORE__H */
