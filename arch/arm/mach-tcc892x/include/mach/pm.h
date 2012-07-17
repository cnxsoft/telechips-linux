/*
 * arch/arm/mach-tcc88xx/pm.h
 *
 * Author:  <linux@telechips.com>
 * Created: April 21, 2008
 * Description: LINUX POWER MANAGEMENT FUNCTIONS
 *
 * Copyright (C) 2008-2009 Telechips 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */
#ifndef __TCC_PM_H__
#define __TCC_PM_H__

/*===========================================================================

                                  MODE

===========================================================================*/

//#define TCC_PM_PMU_CTRL
//#define TCC_PM_MEMQ_PWR_CTRL
#define TCC_PM_SSTLIO_CTRL
#define TCC_PM_CHECK_WAKEUP_SOURCE

#if defined(CONFIG_REGULATOR)
//#define TCC_PM_REGULATOR_CTRL
#if defined(TCC_PM_REGULATOR_CTRL)
  #if defined(CONFIG_REGULATOR_AXP192)
    #define TCC_PM_REGULATOR_DEV_ADDR       0x68
    #define TCC_PM_REGULATOR_DEV_REG        0x23	// dcdc2
    #define TCC_PM_REGULATOR_READ_SIZE      0x00
    #define TCC_PM_REGULATOR_WRITE_SIZE     0x01
    #define TCC_PM_REGULATOR_SLEEP_DATA1    0x0C	// 1.00V
    #define TCC_PM_REGULATOR_SLEEP_DATA2    0x00
    #define TCC_PM_REGULATOR_WAKEUP_DATA1   0x1C	// 1.40V
    #define TCC_PM_REGULATOR_WAKEUP_DATA2   0x00
  #else /* No Devices */
    #define TCC_PM_REGULATOR_DEV_ADDR       0x00	// no device
    #define TCC_PM_REGULATOR_DEV_REG        0x00
    #define TCC_PM_REGULATOR_READ_SIZE      0x00
    #define TCC_PM_REGULATOR_WRITE_SIZE     0x00
    #define TCC_PM_REGULATOR_SLEEP_DATA1    0x00
    #define TCC_PM_REGULATOR_SLEEP_DATA2    0x00
    #define TCC_PM_REGULATOR_WAKEUP_DATA1   0x00
    #define TCC_PM_REGULATOR_WAKEUP_DATA2   0x00
  #endif
  #define TCC_PM_REGULATOR_I2C_CH           0x00
#endif
#endif

#if defined(CONFIG_SHUTDOWN_MODE)
/*===========================================================================

                              Shut-down MAP

===========================================================================*/

/*---------------------------------------------------------------------------
 1) Shut-down (shut down + sram boot + sdram self-refresh)

     << sram >>
     0xF0000000(0x10000000) ------------------
                           |    Boot code     | 0x700
                     0x700  ------------------
                           |      Stack       | 0x100
                     0x800  ------------------
                           |     Shutdown     | 0x600
                     0xE00  ------------------
                           |     Wake-up      | 0x400
                    0x1200  ------------------
                           |    SDRAM Init    | 0x700
                    0x1900  ------------------
                           |   GPIO Storage   | 0x300
                    0x1C00  ------------------
                           |   cpu_reg/mmu data | 0x100
                    0x1D00  ------------------

---------------------------------------------------------------------------*/

#define SRAM_BOOT_ADDR           0xF0000000
#define SRAM_BOOT_SIZE           0x00000700

#define SRAM_STACK_ADDR          0xF0000700
#define SRAM_STACK_SIZE          0x00000100

#define SHUTDOWN_FUNC_ADDR       0xF0000800
#define SHUTDOWN_FUNC_SIZE       0x00000600

#define WAKEUP_FUNC_ADDR         0xF0000E00
#define WAKEUP_FUNC_SIZE         0x00000400

#define SDRAM_INIT_FUNC_ADDR     0xF0001200
#define SDRAM_INIT_FUNC_SIZE     0x00000700

#define GPIO_REPOSITORY_ADDR     0xF0001900
#define GPIO_REPOSITORY_SIZE     0x00000300

#define CPU_DATA_REPOSITORY_ADDR 0xF0001C00
#define CPU_DATA_REPOSITORY_SIZE 0x00000100

#if defined(TCC_PM_REGULATOR_CTRL)
#define COREPWR_FUNC_ADDR        0xF0002000
#define COREPWR_FUNC_SIZE        0x00000FD0

#define COREPWR_PARAM_ADDR       0xF0002FD0
#define COREPWR_PARAM_SIZE       0x00000030
#endif

/*-------------------------------------------------------------------------*/


/*===========================================================================

                        Shut-down Backup Registers

===========================================================================*/
#if defined(CONFIG_PM_CONSOLE_NOT_SUSPEND)
typedef struct _BACKUP_UART {
	volatile unsigned int	DLL;	// 0x000  R/W  0x00000000   Divisor Latch (LSB) (DLAB=1)
	volatile unsigned int	IER;		// 0x004  R/W  0x00000000   Interrupt Enable Register (DLAB=0)
	volatile unsigned int	DLM;	// 0x004  R/W  0x00000000   Divisor Latch (MSB) (DLAB=1)
	volatile unsigned int	LCR;	// 0x00C  R/W  0x00000003   Line Control Register
	volatile unsigned int	MCR;	// 0x010  R/W  0x00000040   MODEM Control Register
	volatile unsigned int	AFT;	// 0x020  R/W  0x00000000   AFC Trigger Level Register
	volatile unsigned int	UCR;	// 0x024  R/W  0x00000000   UART Control register
	volatile unsigned int	CFG0;	// R/W  0x00000000   Port Configuration Register 0(PCFG0)
	volatile unsigned int	CFG1;	// R/W  0x00000000   Port Configuration Register 1(PCFG1)
} bkUART;
#endif

typedef struct _TCC_REG_{
	CKC ckc;
	PIC pic;
	VIC vic;
	TIMER timer;
#if defined(TCC_PM_PMU_CTRL)
	PMU pmu;
#endif
	SMUCONFIG smuconfig;
	IOBUSCFG iobuscfg;
	MEMBUSCFG membuscfg;
	NFC nfc;
	unsigned L2_aux;

#if defined(CONFIG_PM_CONSOLE_NOT_SUSPEND)
	bkUART	bkuart;
#endif
} TCC_REG, *PTCC_REG;

#elif defined(CONFIG_SLEEP_MODE)

/*===========================================================================

                                  MODE

===========================================================================*/

#define TCC_PM_SLEEP_WFI_USED

/*===========================================================================

                              Sleep MAP

===========================================================================*/

/*---------------------------------------------------------------------------
 1) Sleep (Sleep + sdram self-refresh)

     << sram >>
     0xF0000000(0x10000000) ------------------
                           |                  | 0x700
                     0x700  ------------------
                           |      Stack       | 0x100
                     0x800  ------------------
                           |      Sleep       | 0xA00
                    0x1200  ------------------
                           |    SDRAM Init    | 0x700
                    0x1900  ------------------

---------------------------------------------------------------------------*/

#define SRAM_STACK_ADDR          0xF0000700
#define SRAM_STACK_SIZE          0x00000100

#define SLEEP_FUNC_ADDR          0xF0000800
#define SLEEP_FUNC_SIZE          0x00000A00

#define SDRAM_INIT_FUNC_ADDR     0xF0001200
#define SDRAM_INIT_FUNC_SIZE     0x00000700

/*-------------------------------------------------------------------------*/


/*===========================================================================

                         Sleep Backup Registers

===========================================================================*/

typedef struct _TCC_REG_{
	CKC ckc;
	PMU pmu;
	unsigned L2_aux;
} TCC_REG, *PTCC_REG;
#endif

/*-------------------------------------------------------------------------*/

#endif  /*__TCC_PM_H__*/
