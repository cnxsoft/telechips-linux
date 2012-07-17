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

#define TCC_PM_BOTH_SUSPEND_USED

#if defined(CONFIG_TCC88XX_RTC_INTERNAL_PATH)
#define CONFIG_SLEEP_MODE
#else
#define CONFIG_SHUTDOWN_MODE
#endif

#ifdef TCC_PM_BOTH_SUSPEND_USED
 #define TCC_PM_SHUTDOWN_MODE
 #define TCC_PM_SLEEP_MODE
 #define TCC_PM_SLEEP_WFI_USED
#else
 #if defined(CONFIG_SHUTDOWN_MODE)
 #define TCC_PM_SHUTDOWN_MODE
 #elif defined(CONFIG_SLEEP_MODE)
 #define TCC_PM_SLEEP_MODE
 #define TCC_PM_SLEEP_WFI_USED
 #endif
#endif

#define TCC_MEMBUS_PWR_CTRL_USED

//#define TCC_PMIC_COREPWR_CTRL_USED

//#define PORT_ARRANGE_USED

#define TCC_SHUTDOWN_CORE_V_CTRL_USED

#if defined(TCC_PM_SHUTDOWN_MODE)
/*===========================================================================

                              Shut-down MAP

===========================================================================*/

/*---------------------------------------------------------------------------
 1) Shut-down (shut down + sram boot + sdram self-refresh)

     << sram >>
     0x10000000(0xF0A00000) ------------------
                           |    Boot code     | 0x700
                     0x700  ------------------
                           |      Stack       | 0x100
                     0x800  ------------------
                           |   I/O restore    | 0x200
                     0xA00  ------------------
                           |   SDRAM Init     | 0x700
                    0x1100  ------------------
                           | GPIO Backup Data | 0x300
                    0x1400  ------------------
                           |  Enter Shutdown  | 0x400
                    0x1800  ------------------
                           |   mmu on code    | 0x50
                    0x1850  ------------------
                           | cpu_reg/mmu data | 0x100
                    0x1950  ------------------
#ifdef PORT_ARRANGE_USED
                           | GPIO Backup Data | 0x300
                    0x1C50  ------------------
#endif
                           |                  |
                             : : : : : : : : :
                           |                  |
                    0x2000  ------------------  
                           | core power ctrl. | 0x1000
                    0x3000  ------------------

---------------------------------------------------------------------------*/

#define SRAM_BOOT_ADDR           0xF0A00000
#define SRAM_BOOT_SIZE           0x00000700

#define SHUTDOWN_STACK_ADDR      0xF0A00700
#define SHUTDOWN_STACK_SIZE      0x00000100
#define SHUTDOWN_STACK_PHY       0x10000700

#define IO_RESTORE_FUNC_ADDR     0xF0A00800
#define IO_RESTORE_FUNC_SIZE     0x00000200

#define GPIO_BUF_ADDR            0xF0A01100
#define GPIO_BUF_PHY             0x10001100

#define SHUTDOWN_FUNC_ADDR       0xF0A01400
#define SHUTDOWN_FUNC_SIZE       0x00000400
#define SHUTDOWN_FUNC_PHY        0x10001400

#define MMU_SWITCH_FUNC_ADDR     0xF0A01800
#define MMU_SWITCH_FUNC_SIZE     0x00000020
#define MMU_SWITCH_FUNC_PHY      0x10001800

#define REG_MMU_DATA_ADDR        0xF0A01850
#define REG_MMU_DATA_PHY         0x10001850

#ifdef PORT_ARRANGE_USED
#define GPIO_BUF_ADDR_           0xF0A01950
#define GPIO_BUF_PHY_            0x10001950
#endif

#define MMU_SWITCH_EXEC_ADDR     0xF0700100

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
} bkUART;
#endif

typedef struct _TCC_REG_{
	CKC ckc;
	PIC pic;
	VIC vic;
	TIMER timer;
	PMU pmu;
	SMUCONFIG smuconfig;
	//GPIO gpio;

	IOBUSCFG iobuscfg;

	NFC		nfc;
#if defined(CONFIG_PM_CONSOLE_NOT_SUSPEND)
	bkUART	bkuart;
#endif
	UARTPORTMUX uartportmux;

	volatile unsigned int backup_peri_iobus0;
	volatile unsigned int backup_peri_iobus1;
} TCC_REG, *PTCC_REG;

/*-------------------------------------------------------------------------*/
#endif /* TCC_PM_SHUTDOWN_MODE */



#if defined(TCC_PM_SLEEP_MODE)
/*===========================================================================

                                Sleep MAP

===========================================================================*/

/*---------------------------------------------------------------------------
 2) sleep (sleep + sdram self-refresh)

     0x10000000(0xF0A00000) ------------------
                           |    Sleep code    | 0x700
                     0x700  ------------------
                           |      Stack       | 0x100
                     0x800  ------------------
                           |     not used     | 0x200
                     0xA00  ------------------
                           |   SDRAM Init     | 0x700
                    0x1100  ------------------
                           |                  |
                             : : : : : : : : :
                           |                  |
                    0x2000  ------------------  
                           | core power ctrl. | 0x1000
                    0x3000  ------------------

---------------------------------------------------------------------------*/

#define SLEEP_FUNC_ADDR          0xF0A00000
#define SLEEP_FUNC_SIZE          0x00000700

#define SLEEP_STACK_ADDR         0xF0A00700
#define SLEEP_STACK_SIZE         0x00000100
#define SLEEP_STACK_PHY          0x10000700

/*-------------------------------------------------------------------------*/
#endif


/*===========================================================================

                         Shut-down/Sleep Common MAP

===========================================================================*/

#define SDRAM_INIT_FUNC_ADDR     0xF0A00A00
#define SDRAM_INIT_FUNC_SIZE     0x00000700
#define SDRAM_INIT_FUNC_PHY      0x10000A00

#define COREPWR_FUNC_ADDR        0xF0A02000
#define COREPWR_FUNC_SIZE        0x00000FE0
#define COREPWR_FUNC_PHY         0x10002000

#define COREPWR_PARAM_ADDR       0xF0A02FE0
#define COREPWR_PARAM_SIZE       0x00000020
#define COREPWR_PARAM_PHY        0x10002FE0

/*-------------------------------------------------------------------------*/

#endif  /*__TCC_PM_H__*/
