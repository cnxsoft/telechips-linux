/* 
 * linux/drivers/char/tcc_dxb/ctrl/tcc_dxb_control.h
 *
 * Author:  <linux@telechips.com>
 * Created: 10th Jun, 2008 
 * Description: Telechips Linux BACK-LIGHT DRIVER
 *
 * Copyright (c) Telechips, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef     _TCC_DXB_CTRL_H_
#define     _TCC_DXB_CTRL_H_

#include    <linux/types.h>

typedef enum
{
	BOARD_TDMB_TCC3150, //TCCXXXX_DMB_TCC3150_SV1.2
	BOARD_TDMB_TCC3161, //TCCXXXX_DMB_TCC3161_SV1.0
	BOARD_DVBT_DIB7070,
	BOARD_DVBT_DIB9090, //TCCXXXX_DVBT&H_9080&9090_SV1.0
	BOARD_ISDBT_DIB10096, //tcc89&93XX_ISDBT_DIB1009X_SV0.2
	BOARD_DXB_TCC3510, //TCCXXXX_DXB_TCC3510_SV5.0, OnBoard Module in TCC93XX STB	
	BOARD_DVBT_DIB9090M_PA, //TCC89&91&92XX_DVB_DIB90X0_SV0.1 //PARALLEL Interface
	BOARD_ISDBT_MTV818,	//TCCXXXX_ISDB-T_MTV818_SV0.1	//usable only for TCC89xx and TCC92xx
	BOARD_DXB_NMI32X, //ISDBT_NMI325_SV0.1	
	BOARD_ISDBT_TOSHIBA,
	BOARD_ISDBT_TCC353X,
	BOARD_ISDBT_TCC353X_FSMA,
	BOARD_MAX
}DXB_BOARD_TYPE;


#define DXB_RF_PATH_UHF 1
#define DXB_RF_PATH_VHF 2

typedef struct
{
	unsigned int uiI2C; //control channel of i2c
	unsigned int uiSPI; //control channel of spi 
}ST_CTRLINFO_ARG;

struct tcc_dxb_platform_data {
	void (*init)(void);
	void (*power_off)(int deviceIdx);
	void (*power_on)(int deviceIdx);
	void (*power_reset)(int deviceIdx);
	void (*rf_path)(unsigned int flag);
	void (*set_board)(unsigned int uiboardtype);
	void (*get_info)(ST_CTRLINFO_ARG *pstCtrlInfo);
};

#endif
