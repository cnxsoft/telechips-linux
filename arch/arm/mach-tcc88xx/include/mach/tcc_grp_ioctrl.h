
#include <mach/tcc_Gre2d_type.h>

#define TCC_GRP_COMMON_IOCTRL						0x1
#define TCC_GRP_COMMON_IOCTRL_KERNEL				0x101

#define TCC_GRP_ROTATE_IOCTRL						0x4
#define TCC_GRP_ROTATE_IOCTRL_KERNEL				0x104

#define TCC_OVERLAY_MIXER_IOCTRL					0x8
#define TCC_OVERLAY_MIXER_IOCTRL_KERNEL			0x108

#define TCC_OVERLAY_MIXER_CLUT_IOCTRL				0x10
#define TCC_OVERLAY_MIXER_CLUT_IOCTRL_KERNEL		0x110

#define TCC_GRP_ARITH_IOCTRL						0x20
#define TCC_GRP_ARITH_IOCTRL_KERNEL				0x120

#define TCC_GRP_ALPHA_VALUE_SET_IOCTRL			0x40
#define TCC_GRP_ALPHA_VALUE_SET_IOCTRL_KERNEL	0x140

#define TCC_OVERLAY_IOCTRL                      0x80
#define TCC_OVERLAY_IOCTRL_KERNEL               0x180

#ifndef ADDRESS_ALIGNED
#define ADDRESS_ALIGNED
#define ALIGN_BIT (0x8-1)
#define BIT_0 3
#define GET_ADDR_YUV42X_spY(Base_addr) 		(((((unsigned int)Base_addr) + ALIGN_BIT)>> BIT_0)<<BIT_0)
#define GET_ADDR_YUV42X_spU(Yaddr, x, y) 		(((((unsigned int)Yaddr+(x*y)) + ALIGN_BIT)>> BIT_0)<<BIT_0)
#define GET_ADDR_YUV422_spV(Uaddr, x, y) 		(((((unsigned int)Uaddr+(x*y/2)) + ALIGN_BIT) >> BIT_0)<<BIT_0)
#define GET_ADDR_YUV420_spV(Uaddr, x, y) 		(((((unsigned int)Uaddr+(x*y/4)) + ALIGN_BIT) >> BIT_0)<<BIT_0)
#endif

typedef enum
{
	G2D_POLLING,
	G2D_INTERRUPT,	
	G2D_CHECKING,
}G2D_ResponseType;

typedef struct
{
	G2D_ResponseType responsetype;
	unsigned int 	src0_y;
	unsigned int 	src0_u;
	unsigned int 	src0_v;
	G2D_FMT_CTRL 	srcfm;
	unsigned int  	src_imgx;
	unsigned int  	src_imgy;
	char 			DefaultBuffer; // 1 : defualt G2d buffer
	unsigned int 	tgt0_y;
	unsigned int 	tgt0_u;
	unsigned int 	tgt0_v;
	G2D_FMT_CTRL 	tgtfm;
	G2D_OP_MODE 	ch_mode;	
}G2D_COMMON_TYPE_EXT;

typedef struct
{
	unsigned int	responsetype; //G2D_ResponseType
	unsigned int 	src0;
	unsigned int 	src1;
	unsigned int 	src2;
	G2D_FMT_CTRL 	srcfm;		//G2D_DATA_FM
	unsigned int  	src_imgx;
	unsigned int  	src_imgy;
	char 			DefaultBuffer; // 1 : defualt G2d buffer
	unsigned int  	crop_offx;
	unsigned int  	crop_offy;
	unsigned int  	crop_imgx;
	unsigned int  	crop_imgy;
	unsigned int 	tgt0;
	unsigned int 	tgt1;
	unsigned int 	tgt2;
	G2D_FMT_CTRL 	tgtfm;		//G2D_DATA_FM
	unsigned int  	dst_imgx;
	unsigned int  	dst_imgy;
    unsigned int    dst_off_x;
    unsigned int    dst_off_y;
	unsigned int	ch_mode;	//G2D_OP_MODE
	unsigned int	parallel_ch_mode;
}G2D_COMMON_TYPE;

typedef struct
{
	unsigned int	responsetype; //G2D_ResponseType
	unsigned int 	src0;
	unsigned int 	src1;
	unsigned int 	src2;
	G2D_FMT_CTRL 	srcfm;		//G2D_DATA_FM
	unsigned int  	src_imgx;
	unsigned int  	src_imgy;
	unsigned int	DefaultBuffer; // 1 : defualt G2d buffer
	unsigned int 	tgt0;
	unsigned int 	tgt1;
	unsigned int 	tgt2;
	G2D_FMT_CTRL 	tgtfm;		//G2D_DATA_FM
	unsigned int  	dst_imgx;
	unsigned int  	dst_imgy;
    unsigned int    dst_off_x;
    unsigned int    dst_off_y;
	unsigned int	arith;	//G2D_ARITH_TYPE
	unsigned int 	R;
	unsigned int 	G;	
	unsigned int 	B;
}G2D_ARITH_OP_TYPE;



typedef struct
{
	unsigned int	responsetype; //G2D_ResponseType
	unsigned int 	src0;
	unsigned int 	src1;
	unsigned int 	src2;
	G2D_FMT_CTRL 	srcfm;		//G2D_DATA_FM
	unsigned int  	src_imgx;
	unsigned int  	src_imgy;
	unsigned int  	crop_offx;
	unsigned int  	crop_offy;
	unsigned int  	crop_imgx;
	unsigned int  	crop_imgy;
	unsigned int 	tgt0;
	unsigned int 	tgt1;
	unsigned int 	tgt2;
	G2D_FMT_CTRL	tgtfm;		//G2D_DATA_FM
	unsigned int  	dst_imgx;
	unsigned int  	dst_imgy;
    unsigned int    dst_off_x;
    unsigned int    dst_off_y;
	unsigned int	ch_mode;	//G2D_OP_MODE
	unsigned int	alpha_type; //G2d_ALAPH_RGB
	unsigned int 	alpha_value;
}G2D_BITBLIT_TYPE;

/*****************************************************/
typedef struct
{
	unsigned int	responsetype; //G2D_ResponseType
	unsigned int 	src0;
	unsigned int 	src1;
	unsigned int 	src2;

	unsigned int  	src0_imgx;
	unsigned int  	src0_imgy;
	unsigned int  	src0_crop_offx;
	unsigned int  	src0_crop_offy;
	unsigned int  	src0_crop_imgx;
	unsigned int  	src0_crop_imgy;
	unsigned int	src0_clut_en;
	G2D_FMT_CTRL 	src0_fmt;

	unsigned int  	src1_imgx;
	unsigned int  	src1_imgy;
	unsigned int  	src1_crop_offx;
	unsigned int  	src1_crop_offy;
	unsigned int  	src1_crop_imgx;
	unsigned int  	src1_crop_imgy;
	unsigned int	src1_clut_en;
	G2D_FMT_CTRL 	src1_fmt;

	unsigned int  	src2_imgx;
	unsigned int  	src2_imgy;
	unsigned int  	src2_crop_offx;
	unsigned int  	src2_crop_offy;
	unsigned int  	src2_crop_imgx;
	unsigned int  	src2_crop_imgy;
	unsigned int	src2_clut_en;
	G2D_FMT_CTRL 	src2_fmt;
	
	unsigned int 	tgt0;
	unsigned int 	tgt1;
	unsigned int 	tgt2;
	G2D_FMT_CTRL 	tgtfm;		//G2D_DATA_FM
	
	unsigned int  	dst_imgx;
	unsigned int  	dst_imgy;
	unsigned int    dst_off_x;
	unsigned int    dst_off_y;
	
	unsigned int	ch_mode;	//G2D_OP_MODE
	unsigned int	alpha_type; //G2d_ALAPH_RGB
	unsigned int 	alpha_value;
}G2D_BITBLIT_TYPE_1;

typedef enum{
	CLUT_DATA_ONE,
	CLUT_DATA_ALL,
	
	CLUT_DATA_MAX
}G2D_CLUT_DATA_TYPE;

typedef struct{
	unsigned int ch;
	unsigned int index;
	unsigned int data;
}G2D_CLUT_ONE_TYPE;

typedef struct{
	unsigned int ch;
	unsigned int data[0xFF];
}G2D_CLUT_ALL_TYPE;

typedef struct{
	G2D_CLUT_DATA_TYPE clut_type;

	union{
		G2D_CLUT_ONE_TYPE	one;
		G2D_CLUT_ALL_TYPE	all;
	}clut;
}G2D_CLUT_TYPE;