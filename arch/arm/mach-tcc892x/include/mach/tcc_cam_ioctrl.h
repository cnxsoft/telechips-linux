/****************************************************************************
 *	 TCC Version 0.6
 *	 Copyright (c) telechips, Inc.
 *	 ALL RIGHTS RESERVED
 *
****************************************************************************/
#define ATAG_CAMERA     0x5441000c

/* Our own specific controls */
#define V4L2_CID_ISO 								V4L2_CID_PRIVATE_BASE+0
#define V4L2_CID_EFFECT 							V4L2_CID_PRIVATE_BASE+1
#define V4L2_CID_ZOOM 								V4L2_CID_PRIVATE_BASE+2
#define V4L2_CID_FLIP 								V4L2_CID_PRIVATE_BASE+3
#define V4L2_CID_SCENE 								V4L2_CID_PRIVATE_BASE+4
#define V4L2_CID_METERING_EXPOSURE 					V4L2_CID_PRIVATE_BASE+5
#define V4L2_CID_FLASH 								V4L2_CID_PRIVATE_BASE+6
#define V4L2_CID_FOCUS_MODE							V4L2_CID_PRIVATE_BASE+7
#define V4L2_CID_LAST_PRIV 							V4L2_CID_FLASH
#define V4L2_CID_MAX 								V4L2_CID_LAST_PRIV+1

//#define VIDIOC_USER_CIF_OVERLAY 					_IOWR ('V', BASE_VIDIOC_PRIVATE, 	cif_SuperImpose)
#define VIDIOC_USER_JPEG_CAPTURE 					_IOWR ('V', BASE_VIDIOC_PRIVATE+1, int)
#define VIDIOC_USER_GET_CAPTURE_INFO 				_IOWR ('V', BASE_VIDIOC_PRIVATE+2, TCCXXX_JPEG_ENC_DATA)
//#define VIDIOC_USER_REQBUFS 						_IOWR ('V', BASE_VIDIOC_PRIVATE+3, struct v4l2_requestbuffers)
#define VIDIOC_USER_PROC_AUTOFOCUS 				_IOWR ('V', BASE_VIDIOC_PRIVATE+3, int)
#define VIDIOC_USER_SET_CAMINFO_TOBEOPEN 		_IOWR ('V', BASE_VIDIOC_PRIVATE+4, int)
#define VIDIOC_USER_GET_MAX_RESOLUTION 			_IOWR ('V', BASE_VIDIOC_PRIVATE+5, int)
#define VIDIOC_USER_GET_SENSOR_FRAMERATE		_IOWR ('V', BASE_VIDIOC_PRIVATE+6, int)
#define VIDIOC_USER_GET_ZOOM_SUPPORT			_IOWR ('V', BASE_VIDIOC_PRIVATE+7, int)
#define VIDIOC_USER_SET_CAMERA_ADDR				_IOWR ('V', BASE_VIDIOC_PRIVATE+8, struct v4l2_requestbuffers)

