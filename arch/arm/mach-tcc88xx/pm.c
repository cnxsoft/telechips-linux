/*
 * arch/arm/mach-tcc88xx/pm.c
 *
 * Author:  <linux@telechips.com>
 * Created: October, 2009
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

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/pm.h>
#include <linux/suspend.h>
#include <linux/io.h>
#include <linux/reboot.h>
#include <linux/gpio.h>

#include <asm/io.h>
#include <asm/tlbflush.h>
#include <linux/syscalls.h>		// sys_sync()
#include <asm/cacheflush.h>		// local_flush_tlb_all(), flush_cache_all();

//#include <bsp.h>
//#include <mach/tcc_ckc_ctrl.h>		// arm_changestack(), arm_restorestack()
#include <mach/system.h>
#include <mach/pm.h>

#include <mach/tcc_ddr.h>



/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#if defined(TCC_PM_SHUTDOWN_MODE)
extern void SRAM_Boot(void);
extern void save_cpu_reg(int sram_addr, unsigned int pReg, void *);
extern void resore_cpu_reg(void);
extern void tcc_mmu_switch(void);
#endif

#if defined(TCC_PM_SLEEP_MODE)
extern unsigned int IO_ARM_ChangeStackSRAM(void);
extern void IO_ARM_RestoreStackSRAM(unsigned int);
typedef void (*FuncPtr)(void);
#endif

#if defined(TCC_PMIC_COREPWR_CTRL_USED) || defined(CONFIG_REGULATOR)
#define PMIC_PARAM(x)   (*(volatile unsigned long *)(COREPWR_PARAM_ADDR + (4*(x))))
#define PMIC_PARAM_PHY(x)   (*(volatile unsigned long *)(COREPWR_PARAM_PHY + (4*(x))))
#define SLEEP           0
#define WAKEUP          1

enum {
	DEV_ADDR = 0,   /* i2c device address */
	CORE_CTRL_REG,  /* coreA power control register */
	READ_SIZE,      /* read data size */
	WRITE_SIZE,     /* write data size */
	WRITE_DATA1_S,  /* write data 1 (sleep) */
	WRITE_DATA2_S,  /* write data 2 (sleep) */
	WRITE_DATA1_W,  /* write data 1 (wakeup) */
	WRITE_DATA2_W   /* write data 2 (wakeup) */
};
typedef void (*CoreFuncPtr)(unsigned int goto_state);
#endif

#define addr(x) (0xF0000000+x)

#ifndef hword_of
#define	hword_of(X)					( *(volatile unsigned short *)((X)) )
#endif

#ifndef byte_of
#define	byte_of(X)					( *(volatile unsigned char *)((X)) )
#endif

#ifndef word_of
#define	word_of(X)					( *(volatile unsigned int *)((X)) )
#endif

#define time2cycle(time, tCK)		((int)((time + tCK - 1) / tCK))

#define TCC_PMU_BASE	0xf0404000
#define PMU_CONFIG1	(TCC_PMU_BASE + 0x14)

#ifdef CONFIG_PM_VERBOSE
#define pmdrv_dbg(fmt, arg...)     printk(fmt, ##arg)
#else
#define pmdrv_dbg(arg...)
#endif

#define NopDelay30() {\
	__asm__ volatile ("nop\n"); __asm__ volatile ("nop\n");__asm__ volatile ("nop\n"); __asm__ volatile ("nop\n");__asm__ volatile ("nop\n");\
	__asm__ volatile ("nop\n"); __asm__ volatile ("nop\n");__asm__ volatile ("nop\n"); __asm__ volatile ("nop\n");__asm__ volatile ("nop\n");\
	__asm__ volatile ("nop\n"); __asm__ volatile ("nop\n");__asm__ volatile ("nop\n"); __asm__ volatile ("nop\n");__asm__ volatile ("nop\n");\
	__asm__ volatile ("nop\n"); __asm__ volatile ("nop\n");__asm__ volatile ("nop\n"); __asm__ volatile ("nop\n");__asm__ volatile ("nop\n");\
	__asm__ volatile ("nop\n"); __asm__ volatile ("nop\n");__asm__ volatile ("nop\n"); __asm__ volatile ("nop\n");__asm__ volatile ("nop\n");\
	__asm__ volatile ("nop\n"); __asm__ volatile ("nop\n");__asm__ volatile ("nop\n"); __asm__ volatile ("nop\n");__asm__ volatile ("nop\n");\
}

/*===========================================================================
FUNCTION
===========================================================================*/
#ifdef PORT_ARRANGE_USED
static void port_arrange(void)
{
	//arrange GPIO for reduce sleep current
	
	if (machine_is_m801_88() || machine_is_m803()) {
#if defined(CONFIG_REGULATOR_AXP192_PEK)
		// GPIO_A
		*((volatile unsigned long *)0xF0308024) = 0x00000011;		//GPIOA Config - [0],[1] is i2c oherwise gpio
		*((volatile unsigned long *)0xF0308028) = 0x00000000;		//GPIOA Config - gpio
		*((volatile unsigned long *)0xF030802C) = 0x00000000;		//GPIOA Config - gpio
		*((volatile unsigned long *)0xF0308030) = 0x00000000;		//GPIOA Config - gpio
		*((volatile unsigned long *)0xF030801C) = 0x20000000;		//GPIOA [14] Pull-down, otherwise Pull-up/down disable
		*((volatile unsigned long *)0xF0308020) = 0x00000000;		//GPIOA Pull-up/down disable
		*((volatile unsigned long *)0xF0308004) = 0xFFFFBFFF;		//GPIOA [14] input, otherwise output
		if((system_rev == 0x0003 || system_rev == 0x0004 || system_rev == 0x0005 || system_rev == 0x0006) || (machine_is_m803())) //  for m803, m805 temp	
		    *((volatile unsigned long *)0xF0308000) = Hw5|Hw7|Hw8|Hw15;	//GPIOA [5],[7],[8] High, otherwise  Low
		else
		    *((volatile unsigned long *)0xF0308000) = Hw5|Hw7|Hw8;	//GPIOA [5],[7],[8] High, otherwise  Low
#else
		*((volatile unsigned long *)0xF0308024) = 0x00000000;		//GPIOA Config - gpio
		*((volatile unsigned long *)0xF0308028) = 0x00000000;		//GPIOA Config - gpio
		*((volatile unsigned long *)0xF030802C) = 0x00000000;		//GPIOA Config - gpio
		*((volatile unsigned long *)0xF0308030) = 0x00000000;		//GPIOA Config - gpio
		*((volatile unsigned long *)0xF030801C) = 0x20000000;		//GPIOA [14] Pull-down, otherwise Pull-up/down disable
		*((volatile unsigned long *)0xF0308020) = 0x00000000;		//GPIOA Pull-up/down disable
		*((volatile unsigned long *)0xF0308004) = 0xFFFFBFF7;		//GPIOA [3],[14] input, otherwise output
		*((volatile unsigned long *)0xF0308000) = Hw0|Hw1|Hw5|Hw7|Hw8;		//GPIOA [0],[1],[5],[7],[8] High, otherwise  Low
#endif

		// GPIO_B
		*((volatile unsigned long *)0xF0308064) = 0x00000000;	//GPIOB config - gpio
		*((volatile unsigned long *)0xF0308068) = 0x00000000;	//GPIOB config - gpio
		*((volatile unsigned long *)0xF030806C) = 0x10111010;	//GPIOB [16],[18],[22] Config - gpio, otherwise nand
		*((volatile unsigned long *)0xF0308070) = 0x01100000;	//GPIOB [24],[25],[26],[27],[28],[31] Config - gpio, otherwise nand	
		*((volatile unsigned long *)0xF030805C) = 0x00000000;	//GPIOB Pull-up/down disable
		*((volatile unsigned long *)0xF0308060) = 0x80000000;	//GPIOB [31] Pull-down, otherwise Pull-up/down disable
		*((volatile unsigned long *)0xF0308044) &= ~(Hw31|Hw28);			//GPIOB Input
		*((volatile unsigned long *)0xF0308044) |= (0x0000ffff|Hw27|Hw26|Hw25|Hw24|Hw22|Hw18|Hw16);	//GPIOB Output
		*((volatile unsigned long *)0xF0308040) &= ~(Hw27|Hw26|Hw22|Hw18|Hw16);	//GPIOB Low
		*((volatile unsigned long *)0xF0308040) |= (0x0000ffff|Hw25|Hw24);	//GPIOB High

		// GPIO_C
		*((volatile unsigned long *)0xF03080A4) = 0x00000000;		//GPIOC Config - gpio
		*((volatile unsigned long *)0xF03080A8) = 0x00000000;		//GPIOC Config - gpio
		*((volatile unsigned long *)0xF03080AC) = 0x00000000;		//GPIOC Config - gpio
		*((volatile unsigned long *)0xF03080B0) = 0x00000000;		//GPIOC Config - gpio
		*((volatile unsigned long *)0xF030809C) = 0x00000000;		//GPIOC Pull-up/down disable
		*((volatile unsigned long *)0xF03080A0) = 0x00000000;		//GPIOC Pull-up/down disable
		*((volatile unsigned long *)0xF0308084) = 0xFFFFFFFF;		//GPIOC output
		*((volatile unsigned long *)0xF0308080) = 0x00000000;		//GPIOC Low	

		// GPIO_D
		*((volatile unsigned long *)0xF03080E4) = 0x00000000;		//GPIOD Config - gpio
		*((volatile unsigned long *)0xF03080E8) = 0x00000000;		//GPIOD Config - gpio
		*((volatile unsigned long *)0xF03080EC) = 0x00000000;		//GPIOD Config - gpio
		*((volatile unsigned long *)0xF03080F0) = 0x00000000;		//GPIOD Config - gpio
		*((volatile unsigned long *)0xF03080DC) = 0x00000000;		//GPIOD Pull-up/down disable
		*((volatile unsigned long *)0xF03080E0) = 0x00000000;		//GPIOD Pull-up/down disable
		*((volatile unsigned long *)0xF03080C4) = 0xFFFFFFFF;		//GPIOD output
		*((volatile unsigned long *)0xF03080C0) = Hw22|Hw23|Hw24;		//GPIOD [22],[23],[24] High, otherwise  Low

		// GPIO_E
		*((volatile unsigned long *)0xF0308124) = 0x00000000;		//GPIOE Config - gpio
		*((volatile unsigned long *)0xF0308128) = 0x00000000;		//GPIOE Config - gpio
		*((volatile unsigned long *)0xF030812C) = 0x00000000;		//GPIOE Config - gpio
		*((volatile unsigned long *)0xF0308130) = 0x00000000;		//GPIOE Config - gpio
		*((volatile unsigned long *)0xF030811C) = 0x00000000;		//GPIOE Pull-up/down disable
		*((volatile unsigned long *)0xF0308120) = 0x00000000;		//GPIOE Pull-up/down disable
		*((volatile unsigned long *)0xF0308104) = 0xFFFFFFFF;		//GPIOE output
		*((volatile unsigned long *)0xF0308100) = Hw10|Hw11;		//GPIOE [10],[11]High, otherwise  Low

		// GPIO_F
		*((volatile unsigned long *)0xF0308164) = 0x00000000;		//GPIOF Config - gpio
		*((volatile unsigned long *)0xF0308168) = 0x00000000;		//GPIOF Config - gpio
		*((volatile unsigned long *)0xF030816C) = 0x00000000;		//GPIOF Config - gpio
		*((volatile unsigned long *)0xF0308170) = 0x00000000;		//GPIOF Config - gpio
		*((volatile unsigned long *)0xF030815C) = 0x00000000;		//GPIOF Pull-up/down disable
		*((volatile unsigned long *)0xF0308160) = 0x00000000;		//GPIOF Pull-up/down disable
#if defined(CONFIG_REGULATOR_AXP192_PEK)
		*((volatile unsigned long *)0xF0308144) = 0xFEFDFF83;		//GPIOF [2~6],[17],[24] input, otherwise output		
		*((volatile unsigned long *)0xF0308140) = 0x047E4000;		//GPIOF [14],[18],[19],[20],[21],[22],[26] High, otherwise  Low
#else
		*((volatile unsigned long *)0xF0308144) = 0xFCFDFF83;		//GPIOF [2~6],[17],[24],[25] input, otherwise output	
		*((volatile unsigned long *)0xF0308140) = 0x047E0002;		//GPIOF [18],[19],[20],[21],[22],[26] High, otherwise  Low
#endif

		// GPIO_G
		*((volatile unsigned long *)0xF03081E4) = 0x00000000;		//GPIOG Config - gpio
		*((volatile unsigned long *)0xF03081E8) = 0x00000000;		//GPIOG Config - gpio
		*((volatile unsigned long *)0xF03081EC) = 0x00000000;		//GPIOG Config - gpio
		*((volatile unsigned long *)0xF03081F0) = 0x00000000;		//GPIOG Config - gpio
		*((volatile unsigned long *)0xF03081DC) = 0x00000000;		//GPIOG Pull-up/down disable
		*((volatile unsigned long *)0xF03081E0) = 0x00000000;		//GPIOG Pull-up/down disable
		*((volatile unsigned long *)0xF03081C4) = 0xFFFFFFFF;		//GPIOG output
#if defined(CONFIG_REGULATOR_AXP192_PEK)
		*((volatile unsigned long *)0xF03081C0) = 0x00001000;		//GPIOG [12] HIGH, otherwise Low
#else
		*((volatile unsigned long *)0xF03081C0) = 0x00003000;		//GPIOG [12],[13] HIGH, otherwise Low
#endif

	}
	else if (machine_is_tcc8800st()) {
		// GPIO_A
		*((volatile unsigned long *)0xF0308024) = 0x00000000 | Hw20;//GPIOA Config - [5] is IRDA, otherwise gpio
		*((volatile unsigned long *)0xF0308028) = 0x00000000;		//GPIOA Config - gpio
		*((volatile unsigned long *)0xF030802C) = 0x00000000;		//GPIOA Config - gpio
		*((volatile unsigned long *)0xF0308030) = 0x00000000;		//GPIOA Config - gpio
		*((volatile unsigned long *)0xF030801C) = 0x00000000;		//GPIOA Pull-up/down disable
		*((volatile unsigned long *)0xF0308020) = 0x00000000;		//GPIOA Pull-up/down disable
		*((volatile unsigned long *)0xF0308004) = 0xFFFFFFFF & ~(Hw3 | Hw4 | Hw6 | Hw13);		//GPIOA [3],[4],[6],[13] input, otherwise output
		*((volatile unsigned long *)0xF0308000) = 0x00000000;		//GPIOA Low

		// GPIO_B
		*((volatile unsigned long *)0xF0308064) = 0x00000000;		//GPIOB config - gpio
		*((volatile unsigned long *)0xF0308068) = 0x00000000;		//GPIOB config - gpio
		*((volatile unsigned long *)0xF030806C) = 0x00111010;		//GPIOB [17],[19~21] NAND, otherwise gpio
		*((volatile unsigned long *)0xF0308070) = 0x11101010;		//GPIOB [25], [27], [29~31] NAND, otherwise gpio
		*((volatile unsigned long *)0xF030805C) = 0x00000000;		//GPIOB Pull-up/down disable
		*((volatile unsigned long *)0xF0308060) = 0x00000000;		//GPIOB Pull-up/down disable
		*((volatile unsigned long *)0xF0308044) = 0xFFFFFFFF;		//GPIOB all output
		*((volatile unsigned long *)0xF0308040) = 0x00000000 | 0xff; //GPIOB [0~7] High, otherwize Low

		// GPIO_C
		*((volatile unsigned long *)0xF03080A4) = 0x00000000;		//GPIOC Config - gpio
		*((volatile unsigned long *)0xF03080A8) = 0x00000000;		//GPIOC Config - gpio
		*((volatile unsigned long *)0xF03080AC) = 0x00000000;		//GPIOC Config - gpio
		*((volatile unsigned long *)0xF03080B0) = 0x00000000;		//GPIOC Config - gpio
		*((volatile unsigned long *)0xF030809C) = 0x00000000;		//GPIOC Pull-up/down disable
		*((volatile unsigned long *)0xF03080A0) = 0x00000000;		//GPIOC Pull-up/down disable
		*((volatile unsigned long *)0xF0308084) = 0xFFFFFFFF & ~(Hw28);		//GPIOC [28] is input, otherwize output
		*((volatile unsigned long *)0xF0308080) = 0x00000000;		//GPIOC Low	

		// GPIO_D
		*((volatile unsigned long *)0xF03080E4) = 0x00000000;		//GPIOD Config - gpio
		*((volatile unsigned long *)0xF03080E8) = 0x00000000;		//GPIOD Config - gpio
		*((volatile unsigned long *)0xF03080EC) = 0x00000000;		//GPIOD Config - gpio
		*((volatile unsigned long *)0xF03080F0) = 0x00000000;		//GPIOD Config - gpio
		*((volatile unsigned long *)0xF03080DC) = 0x00000000;		//GPIOD Pull-up/down disable
		*((volatile unsigned long *)0xF03080E0) = 0x00000000;		//GPIOD Pull-up/down disable
		*((volatile unsigned long *)0xF03080C4) = 0xFFFFFFFF & ~(Hw25);		//GPIOD [25] is input, otherwize output
		*((volatile unsigned long *)0xF03080C0) = 0x00000000 | Hw19;		//GPIOD [19] Hight, otherwise  Low

		// GPIO_E
		*((volatile unsigned long *)0xF0308124) = 0x00000000;		//GPIOE Config - gpio
		*((volatile unsigned long *)0xF0308128) = 0x00000000;		//GPIOE Config - gpio
		*((volatile unsigned long *)0xF030812C) = 0x00000000;		//GPIOE Config - gpio
		*((volatile unsigned long *)0xF0308130) = 0x00000000;		//GPIOE Config - gpio
		*((volatile unsigned long *)0xF030811C) = 0x00000000;		//GPIOE Pull-up/down disable
		*((volatile unsigned long *)0xF0308120) = 0x00000000;		//GPIOE Pull-up/down disable
		*((volatile unsigned long *)0xF0308104) = 0xFFFFFFFF & ~(Hw0|Hw1|Hw4);		//GPIOE [0],[1],[4] is input, otherwise output
		*((volatile unsigned long *)0xF0308100) &= 0x00000000 | (Hw21 | Hw22);		//GPIOE [21],[22] is core voltage control, otherwise  Low

		// GPIO_F
		*((volatile unsigned long *)0xF0308164) = 0x00000000;		//GPIOF Config - gpio
		*((volatile unsigned long *)0xF0308168) = 0x00000000;		//GPIOF Config - gpio
		*((volatile unsigned long *)0xF030816C) = 0x00000000;		//GPIOF Config - gpio
		*((volatile unsigned long *)0xF0308170) = 0x00000000;		//GPIOF Config - gpio
		*((volatile unsigned long *)0xF030815C) = 0x00000000;		//GPIOF Pull-up/down disable
		*((volatile unsigned long *)0xF0308160) = 0x00000000;		//GPIOF Pull-up/down disable
		*((volatile unsigned long *)0xF0308144) = 0xFFFFFFFF & ~(Hw1|Hw26|Hw27);	//GPIOF [13],[26],[27] is input, otherwise output	
		*((volatile unsigned long *)0xF0308140) = 0x00000000;		//GPIOF [18],[19],[20],[21],[22],[26] High, otherwise  Low

		// GPIO_G
		*((volatile unsigned long *)0xF03081E4) = 0x00000000;		//GPIOG Config - gpio
		*((volatile unsigned long *)0xF03081E8) = 0x00000000;		//GPIOG Config - gpio
		*((volatile unsigned long *)0xF03081EC) = 0x00000000;		//GPIOG Config - gpio
		*((volatile unsigned long *)0xF03081F0) = 0x00000000;		//GPIOG Config - gpio
		*((volatile unsigned long *)0xF03081DC) = 0x00000000;		//GPIOG Pull-up/down disable
		*((volatile unsigned long *)0xF03081E0) = 0x00000000;		//GPIOG Pull-up/down disable
		*((volatile unsigned long *)0xF03081C4) = 0xFFFFFFFF;		//GPIOG output
		*((volatile unsigned long *)0xF03081C0) &= 0x00000000 |Hw5;		//GPIOG [5] is VDDQ_MEM_ON, otherwise Low
	}	
	else {
		if(system_rev != 0x0610 && system_rev != 0x0613 && system_rev != 0x0615 && system_rev != 0x0623)
		{
			//Verified only on the boards rev 0x610 and 0x613.
			//Not guaranteed any other boards.
			return;
		}
		
		*((volatile unsigned long *)0xF030805C) = 0;		//GPIOB Pull-up/down disable
		*((volatile unsigned long *)0xF0308060) = 0;		//GPIOB Pull-up/down disable
		*((volatile unsigned long *)0xF0308064) = 0;		//GPIOB Config - gpio
		*((volatile unsigned long *)0xF0308068) = 0;		//GPIOB Config - gpio
		*((volatile unsigned long *)0xF030806C) = (*((volatile unsigned long *)0xF030806C) & ~(Hw27|Hw26|Hw25|Hw11|Hw8|Hw3|Hw0)) | (Hw24|Hw10|Hw9|Hw2|Hw1);	//set port EDI_RDY[2], EDI_RDY[3], EDI_XA[2]

#ifdef TCC_MEMBUS_PWR_CTRL_USED
		if(system_rev == 0x0612)
			*((volatile unsigned long *)0xF0308070) = (*((volatile unsigned long *)0xF0308070) & ~(Hw7|Hw6|Hw5|Hw3|Hw2|Hw1)) | (Hw4|Hw0); //set port EDI_CSN[1], EDI_CSN[2] 
		else
#endif
		*((volatile unsigned long *)0xF0308070) = (*((volatile unsigned long *)0xF0308070) & ~(Hw11|Hw10|Hw9|Hw7|Hw6|Hw5|Hw3|Hw2|Hw1)) | (Hw8|Hw4|Hw0); //set port EDI_CSN[1], EDI_CSN[2], EDI_CSN[3] 

		//*((volatile unsigned long *)0xF0308044) |= (Hw27|Hw15|Hw14|Hw13|Hw12|Hw11|Hw10|Hw9|Hw8);		//GPIOB[27], GPIOB[8]~GPIO[15] output
		//*((volatile unsigned long *)0xF0308040) &= ~(Hw27|Hw15|Hw14|Hw13|Hw12|Hw11|Hw10|Hw9|Hw8);	//GPIOB[27], GPIOB[8]~GPIO[15] Low
		*((volatile unsigned long *)0xF0308044) |= 0x0800FFFF;		//GPIOB[27], GPIOB[0]~GPIO[15] output
		*((volatile unsigned long *)0xF0308040) &= ~(0x0800FF00);	//GPIOB[27], GPIOB[8]~GPIO[15] Low
		*((volatile unsigned long *)0xF0308040) |= 0x000000FF;	//GPIOB[0]~GPIO[7] High

		*((volatile unsigned long *)0xF030809C) = 0;		//GPIOC Pull-up/down disable
		*((volatile unsigned long *)0xF03080A0) = 0;		//GPIOC Pull-up/down disable
		*((volatile unsigned long *)0xF03080A4) = 0;		//GPIOC Config - gpio
		*((volatile unsigned long *)0xF03080A8) = 0;		//GPIOC Config - gpio
		*((volatile unsigned long *)0xF03080AC) = 0;		//GPIOC Config - gpio
		*((volatile unsigned long *)0xF03080B0) = 0;		//GPIOC Config - gpio
		*((volatile unsigned long *)0xF0308084) = 0xffffffff;		//GPIOC output
		*((volatile unsigned long *)0xF0308080) = 0;		//GPIOC Low	

		*((volatile unsigned long *)0xF03080DC) = 0;		//GPIOD Pull-up/down disable
		*((volatile unsigned long *)0xF03080E0) = 0;		//GPIOD Pull-up/down disable
		*((volatile unsigned long *)0xF03080E4) = 0;		//GPIOD Config - gpio
		*((volatile unsigned long *)0xF03080E8) = 0;		//GPIOD Config - gpio
		*((volatile unsigned long *)0xF03080EC) = 0;		//GPIOD Config - gpio
		*((volatile unsigned long *)0xF03080F0) = 0;		//GPIOD Config - gpio
		*((volatile unsigned long *)0xF03080C4) = 0xffffffff;		//GPIOD output
		*((volatile unsigned long *)0xF03080C0) = 0;		//GPIOD Low	

		*((volatile unsigned long *)0xF030811C) = 0;		//GPIOE Pull-up/down disable
		*((volatile unsigned long *)0xF0308120) = 0;		//GPIOE Pull-up/down disable
		*((volatile unsigned long *)0xF0308124) = 0;		//GPIOE Config - gpio
		*((volatile unsigned long *)0xF0308128) = 0;		//GPIOE Config - gpio
		*((volatile unsigned long *)0xF030812C) = 0;		//GPIOE Config - gpio
		*((volatile unsigned long *)0xF0308130) = 0;		//GPIOE Config - gpio
		*((volatile unsigned long *)0xF0308104) = 0xffffffff;		//GPIOE output
		*((volatile unsigned long *)0xF0308100) = 0;		//GPIOE Low	
		
		*((volatile unsigned long *)0xF030815C) = 0;		//GPIOF Pull-up/down disable
		*((volatile unsigned long *)0xF0308160) = 0;		//GPIOF Pull-up/down disable
		*((volatile unsigned long *)0xF0308164) = 0;		//GPIOF Config - gpio
		*((volatile unsigned long *)0xF0308168) = 0;		//GPIOF Config - gpio
		*((volatile unsigned long *)0xF030816C) = 0;		//GPIOF Config - gpio
		*((volatile unsigned long *)0xF0308170) = 0;		//GPIOF Config - gpio
		*((volatile unsigned long *)0xF0308144) = 0xffffffff;		//GPIOF output
			*((volatile unsigned long *)0xF0308140) = 0;		//GPIOF Low

		*((volatile unsigned long *)0xF03081DC) = 0;		//GPIOG Pull-up/down disable
		*((volatile unsigned long *)0xF03081E0) = 0;		//GPIOG Pull-up/down disable	
		*((volatile unsigned long *)0xF03081E4) = 0;		//GPIOG Config - gpio
		*((volatile unsigned long *)0xF03081E8) = 0;		//GPIOG Config - gpio
		*((volatile unsigned long *)0xF03081EC) = 0;		//GPIOG Config - gpio
		*((volatile unsigned long *)0xF03081F0) = 0;		//GPIOG Config - gpio
		*((volatile unsigned long *)0xF03081C4) = 0xffffffff;		//GPIOG output
#ifdef TCC_MEMBUS_PWR_CTRL_USED
		if(system_rev == 0x0613 || system_rev == 0x0614 || system_rev == 0x0615 || system_rev == 0x0620 || system_rev == 0x0621 || system_rev == 0x0622 || system_rev == 0x0623 || system_rev == 0x0624)
			*((volatile unsigned long *)0xF03081C0) = 0|Hw5; //GPIOG Low	excluded MEMQZQ Power
		else
#endif
		*((volatile unsigned long *)0xF03081C0) = 0;		//GPIOG Low		
	}

#ifdef CONFIG_SHUTDOWN_MODE
	*((volatile unsigned long *)0xF0308084) &= ~Hw28;		//GPIO_C28, input mode [RTC_PMWKUP]
#endif
}
#endif //#ifdef PORT_ARRANGE_USED

#if defined(TCC_PMIC_COREPWR_CTRL_USED) || defined(CONFIG_REGULATOR)
#define I2C_WR				0
#define I2C_RD				1
#define I2C_WAIT_INTR_CNT	10
#define I2C_PRES_REG		*(volatile unsigned long *)0xF0530000
#define I2C_CTRL_REG		*(volatile unsigned long *)0xF0530004
#define I2C_TXR_REG			*(volatile unsigned long *)0xF0530008
#define I2C_CMD_REG			*(volatile unsigned long *)0xF053000C
#define I2C_RXR_REG			*(volatile unsigned long *)0xF0530010
#define I2C_SR_REG			*(volatile unsigned long *)0xF0530014

#if defined(TCC_PM_SHUTDOWN_MODE)
/*===========================================================================
FUNCTION
===========================================================================*/
static void set_core_voltage_shutdown(unsigned int state)
{
	unsigned long slave_addr = PMIC_PARAM_PHY(DEV_ADDR);
	unsigned long reg_addr;
	unsigned long buf[2];
	unsigned long backup_reg[8];

	int cnt;
	unsigned int i = 0;

	if (PMIC_PARAM_PHY(DEV_ADDR) == 0)
		return;

	/* i2c_set_gpio  GPIO_A0, GPIO_A1 */
//	BITCSET(*((volatile unsigned long *)0xF0308024), 0x000000FF, 0x00000011);

	/* i2c_set_clock */
	backup_reg[0] = *(volatile unsigned long *)addr(0x400000);	// CPU
	backup_reg[1] = *(volatile unsigned long *)addr(0x400010);	// IO Bus
	backup_reg[2] = *(volatile unsigned long *)addr(0x40001C);	// SMU Bus
	backup_reg[3] = *(volatile unsigned long *)addr(0x5F5010);
	backup_reg[4] = *(volatile unsigned long *)addr(0x4000b8);
	backup_reg[5] = *(volatile unsigned long *)addr(0x5F5020);
	backup_reg[6] = I2C_PRES_REG;
	backup_reg[7] = I2C_CTRL_REG;

	*(volatile unsigned long *)addr(0x400000)  = 0x002FFFF4;	// CPU Clock
	*(volatile unsigned long *)addr(0x400010)  = 0x00200014;	// io clk set to 6MHz Xin
	*(volatile unsigned long *)addr(0x40001C)  = 0x00200014;	// SMU Bus
	*(volatile unsigned long *)addr(0x5F5020) &= ~Hw7;			// S/W RESET
	*(volatile unsigned long *)addr(0x5F5010) |= Hw7;			// HCLK
	*(volatile unsigned long *)addr(0x4000B8)  = 0x2400000B;	// PCLK		1MHz
	*(volatile unsigned long *)addr(0x5F5020) |= Hw7;			// S/W RESET

	/* i2c init */
	I2C_PRES_REG = 0x00000001;
	I2C_CTRL_REG = 0x000000C0;
	I2C_CMD_REG |= Hw0;	/* clear pending interrupt */

	/* init. values */
	reg_addr = PMIC_PARAM_PHY(CORE_CTRL_REG);
	buf[0] = 0;
	buf[1] = 0;

	/* READ */
	if (PMIC_PARAM_PHY(READ_SIZE)) {
		/* write dev_addr, reg_addr */
		I2C_TXR_REG = slave_addr | I2C_WR;
		I2C_CMD_REG = Hw7 | Hw4;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt

		I2C_TXR_REG = (reg_addr & 0xFF);
		I2C_CMD_REG = Hw4;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt

		I2C_CMD_REG = Hw6;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt

		/* read_data */

		I2C_TXR_REG = slave_addr | I2C_RD;
		I2C_CMD_REG = Hw7 | Hw4;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt

		for (i = 0; i < PMIC_PARAM_PHY(READ_SIZE); i++) {
			I2C_CMD_REG = Hw5 | Hw3;

			cnt = I2C_WAIT_INTR_CNT;
			while(cnt--) {
				if (I2C_SR_REG & Hw0) {
					if (!(I2C_SR_REG & Hw7))
						break;
				}
			}

			if (cnt == 0) goto set_core_voltage_end;
			BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt

			buf[i] = I2C_RXR_REG;
		}

		I2C_CMD_REG = Hw6;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt
	}

	if (PMIC_PARAM_PHY(WRITE_SIZE)) {
#if defined(TCC_PMIC_COREPWR_CTRL_USED)
		if (state == SLEEP) {
			buf[0] &= ~(unsigned long)PMIC_PARAM_PHY(WRITE_DATA1_S);
			buf[1] &= ~(unsigned long)PMIC_PARAM_PHY(WRITE_DATA2_S);
		}
		else {
			buf[0] |= (unsigned long)PMIC_PARAM_PHY(WRITE_DATA1_W);
			buf[1] |= (unsigned long)PMIC_PARAM_PHY(WRITE_DATA2_W);
		}
#else
		if (state == SLEEP) {
			buf[0] = (unsigned long)PMIC_PARAM_PHY(WRITE_DATA1_S);
			buf[1] = (unsigned long)PMIC_PARAM_PHY(WRITE_DATA2_S);
		}
		else {
			buf[0] = (unsigned long)PMIC_PARAM_PHY(WRITE_DATA1_W);
			buf[1] = (unsigned long)PMIC_PARAM_PHY(WRITE_DATA2_W);
		}
#endif

		/* WRITE */
		I2C_TXR_REG = slave_addr | I2C_WR;
		I2C_CMD_REG = Hw7 | Hw4;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt

		I2C_TXR_REG = (reg_addr & 0xFF);
		I2C_CMD_REG = Hw4;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt

		for (i = 0; i < PMIC_PARAM_PHY(WRITE_SIZE); i++) {
			I2C_TXR_REG = (buf[i] & 0xFF);
			I2C_CMD_REG = Hw4;

			cnt = I2C_WAIT_INTR_CNT;
			while(cnt--) {
				if (I2C_SR_REG & Hw0) {
					if (!(I2C_SR_REG & Hw7))
						break;
				}
			}

			if (cnt == 0) goto set_core_voltage_end;
			BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt
		}

		I2C_CMD_REG = Hw6;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt
	}

set_core_voltage_end:
	I2C_PRES_REG = backup_reg[7];
	I2C_CTRL_REG = backup_reg[6];
	*(volatile unsigned long *)addr(0x5F5020) = backup_reg[5];
	*(volatile unsigned long *)addr(0x4000b8) = backup_reg[4];
	*(volatile unsigned long *)addr(0x5F5010) = backup_reg[3];
	*(volatile unsigned long *)addr(0x40001C) = backup_reg[2];
	*(volatile unsigned long *)addr(0x400010) = backup_reg[1];
	*(volatile unsigned long *)addr(0x400000) = backup_reg[0];
}
#endif

#if defined(TCC_PM_SLEEP_MODE)
/*===========================================================================
FUNCTION
===========================================================================*/
static void set_core_voltage_sleep(unsigned int state)
{
	unsigned long slave_addr = PMIC_PARAM(DEV_ADDR);
	unsigned long reg_addr;
	unsigned long buf[2];
	unsigned long backup_reg[8];

	int cnt;
	unsigned int i = 0;

	if (PMIC_PARAM(DEV_ADDR) == 0)
		return;

	/* i2c_set_gpio  GPIO_A0, GPIO_A1 */
//	BITCSET(*((volatile unsigned long *)0xF0308024), 0x000000FF, 0x00000011);

	/* i2c_set_clock */
	backup_reg[0] = *(volatile unsigned long *)addr(0x400000);	// CPU
	backup_reg[1] = *(volatile unsigned long *)addr(0x400010);	// IO Bus
	backup_reg[2] = *(volatile unsigned long *)addr(0x40001C);	// SMU Bus
	backup_reg[3] = *(volatile unsigned long *)addr(0x5F5010);
	backup_reg[4] = *(volatile unsigned long *)addr(0x4000b8);
	backup_reg[5] = *(volatile unsigned long *)addr(0x5F5020);
	backup_reg[6] = I2C_PRES_REG;
	backup_reg[7] = I2C_CTRL_REG;

	*(volatile unsigned long *)addr(0x400000)  = 0x002FFFF4;	// CPU Clock
	*(volatile unsigned long *)addr(0x400010)  = 0x00200014;	// io clk set to 6MHz Xin
	*(volatile unsigned long *)addr(0x40001C)  = 0x00200014;	// SMU Bus
	*(volatile unsigned long *)addr(0x5F5020) &= ~Hw7;			// S/W RESET
	*(volatile unsigned long *)addr(0x5F5010) |= Hw7;			// HCLK
	*(volatile unsigned long *)addr(0x4000B8)  = 0x2400000B;	// PCLK		1MHz
	*(volatile unsigned long *)addr(0x5F5020) |= Hw7;			// S/W RESET

	/* i2c init */
	I2C_PRES_REG = 0x00000001;
	I2C_CTRL_REG = 0x000000C0;
	I2C_CMD_REG |= Hw0;	/* clear pending interrupt */

	/* init. values */
	reg_addr = PMIC_PARAM(CORE_CTRL_REG);
	buf[0] = 0;
	buf[1] = 0;

	/* READ */
	if (PMIC_PARAM(READ_SIZE)) {
		/* write dev_addr, reg_addr */
		I2C_TXR_REG = slave_addr | I2C_WR;
		I2C_CMD_REG = Hw7 | Hw4;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt

		I2C_TXR_REG = (reg_addr & 0xFF);
		I2C_CMD_REG = Hw4;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt

		I2C_CMD_REG = Hw6;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt

		/* read_data */

		I2C_TXR_REG = slave_addr | I2C_RD;
		I2C_CMD_REG = Hw7 | Hw4;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt

		for (i = 0; i < PMIC_PARAM(READ_SIZE); i++) {
			I2C_CMD_REG = Hw5 | Hw3;

			cnt = I2C_WAIT_INTR_CNT;
			while(cnt--) {
				if (I2C_SR_REG & Hw0) {
					if (!(I2C_SR_REG & Hw7))
						break;
				}
			}

			if (cnt == 0) goto set_core_voltage_end;
			BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt

			buf[i] = I2C_RXR_REG;
		}

		I2C_CMD_REG = Hw6;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt
	}

	if (PMIC_PARAM(WRITE_SIZE)) {
#if defined(TCC_PMIC_COREPWR_CTRL_USED)
		if (state == SLEEP) {
			buf[0] &= ~(unsigned long)PMIC_PARAM(WRITE_DATA1_S);
			buf[1] &= ~(unsigned long)PMIC_PARAM(WRITE_DATA2_S);
		}
		else {
			buf[0] |= (unsigned long)PMIC_PARAM(WRITE_DATA1_W);
			buf[1] |= (unsigned long)PMIC_PARAM(WRITE_DATA2_W);
		}
#else
		if (state == SLEEP) {
			buf[0] = (unsigned long)PMIC_PARAM(WRITE_DATA1_S);
			buf[1] = (unsigned long)PMIC_PARAM(WRITE_DATA2_S);
		}
		else {
			buf[0] = (unsigned long)PMIC_PARAM(WRITE_DATA1_W);
			buf[1] = (unsigned long)PMIC_PARAM(WRITE_DATA2_W);
		}
#endif

		/* WRITE */
		I2C_TXR_REG = slave_addr | I2C_WR;
		I2C_CMD_REG = Hw7 | Hw4;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt

		I2C_TXR_REG = (reg_addr & 0xFF);
		I2C_CMD_REG = Hw4;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt

		for (i = 0; i < PMIC_PARAM(WRITE_SIZE); i++) {
			I2C_TXR_REG = (buf[i] & 0xFF);
			I2C_CMD_REG = Hw4;

			cnt = I2C_WAIT_INTR_CNT;
			while(cnt--) {
				if (I2C_SR_REG & Hw0) {
					if (!(I2C_SR_REG & Hw7))
						break;
				}
			}

			if (cnt == 0) goto set_core_voltage_end;
			BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt
		}

		I2C_CMD_REG = Hw6;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt
	}

set_core_voltage_end:
	I2C_PRES_REG = backup_reg[7];
	I2C_CTRL_REG = backup_reg[6];
	*(volatile unsigned long *)addr(0x5F5020) = backup_reg[5];
	*(volatile unsigned long *)addr(0x4000b8) = backup_reg[4];
	*(volatile unsigned long *)addr(0x5F5010) = backup_reg[3];
	*(volatile unsigned long *)addr(0x40001C) = backup_reg[2];
	*(volatile unsigned long *)addr(0x400010) = backup_reg[1];
	*(volatile unsigned long *)addr(0x400000) = backup_reg[0];
}
#endif
#endif
/*=========================================================================*/



#if defined(TCC_PM_SHUTDOWN_MODE)
/*===========================================================================

                                 Shut-down

===========================================================================*/

#if defined(CONFIG_PM_CONSOLE_NOT_SUSPEND)
static void tcc_pm_uart_suspend(bkUART *pBackupUART)
{
	UART *pHwUART = (UART *)tcc_p2v(HwUART0_BASE);

	pBackupUART->IER	= pHwUART->REG2.IER;	//0x04 : IER
	BITCLR(pHwUART->REG2.IER, Hw2);	//disable interrupt : ELSI
	pBackupUART->LCR	= pHwUART->LCR;	//0x0C : LCR
	BITSET(pHwUART->LCR, Hw7);		// DLAB = 1
	pBackupUART->DLL	= pHwUART->REG1.DLL;	//0x00 : DLL
	pBackupUART->DLM	= pHwUART->REG2.DLM;	//0x04 : DLM
	pBackupUART->MCR	= pHwUART->MCR;	//0x10 : MCR
	pBackupUART->AFT	= pHwUART->AFT;		//0x20 : AFT
	pBackupUART->UCR	= pHwUART->UCR;	//0x24 : UCR
}

static void tcc_pm_uart_resume(bkUART *pBackupUART)
{
	UART *pHwUART = (UART *)tcc_p2v(HwUART0_BASE);

	BITCLR(pHwUART->REG2.IER, Hw2);	//disable interrupt : ELSI
	BITSET(pHwUART->LCR, Hw7);		// DLAB = 1
	pHwUART->REG3.FCR	= Hw2 + Hw1 + Hw0;	//0x08 : FCR
	pHwUART->REG1.DLL	= pBackupUART->DLL;	//0x00 : DLL
	pHwUART->REG2.DLM	= pBackupUART->DLM;	//0x04 : DLM
	pHwUART->MCR	= pBackupUART->MCR;	//0x10 : MCR
	pHwUART->AFT	= pBackupUART->AFT;	//0x20 : AFT
	pHwUART->UCR	= pBackupUART->UCR;	//0x24 : UCR
	pHwUART->LCR	= pBackupUART->LCR;	//0x0C : LCR
	pHwUART->REG2.IER	= pBackupUART->IER;	//0x04 : IER
}
#endif

/*===========================================================================
VARIABLE
===========================================================================*/
static TCC_REG *pReg;
static TCC_REG reg_backup;

/*===========================================================================
FUNCTION
===========================================================================*/
static void shutdown(void)
{
	volatile unsigned int nCount = 0;

#if defined(CONFIG_DRAM_DDR3)
	*(volatile unsigned int *)addr(0x303020) &= ~(0x1<<18); // CSYSREQ3
	if(DDR3_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x309208)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
	else//DDR3_BANK_NUM is 4
		while (((*(volatile unsigned long *)addr(0x309208)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED
#elif defined(CONFIG_DRAM_DDR2)
	*(volatile unsigned int *)addr(0x303020) &= ~(0x1<<17); // CSYSREQ2
	if(DDR2_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x306048)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
	else//DDR2_BANK_NUM == 4
		while (((*(volatile unsigned long *)addr(0x306048)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED
#elif defined(CONFIG_DRAM_LPDDR2)
	*(volatile unsigned int *)addr(0x303020) &= ~(0x1<<17); // CSYSREQ2
	if(LPDDR2_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x306048)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
	else//LPDDR2_BANK_NUM == 4
		while (((*(volatile unsigned long *)addr(0x306048)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED
#else
 #error "not selected"
#endif

	for (nCount = 20000; nCount > 0; nCount --);	// Wait

	*(volatile unsigned long *)addr(0x400000) = 0x002ffff4; // CKC-CLKCTRL0 - set cpu clk to XIN
	*(volatile unsigned long *)addr(0x400004) = 0x00200014; // CKC-CLKCTRL1 - set display clk to XIN
	*(volatile unsigned long *)addr(0x400008) = 0x00200014; // CKC-CLKCTRL2 - set memory clk to XIN
	*(volatile unsigned long *)addr(0x40000C) = 0x00200014; // CKC-CLKCTRL3 - set graphic clk to XIN
	*(volatile unsigned long *)addr(0x400010) = 0x00200014; // CKC-CLKCTRL4 - set Peripheral(io) clk to XIN
	*(volatile unsigned long *)addr(0x400014) = 0x00200014; // CKC-CLKCTRL5 - set video bus clk to XIN
	*(volatile unsigned long *)addr(0x400018) = 0x00200014; // CKC-CLKCTRL6 - set video core clk to XIN
	*(volatile unsigned long *)addr(0x40001C) = 0x00200014; // CKC-CLKCTRL7 - set SMU clk to XIN
	*(volatile unsigned long *)addr(0x400048) = 0x00200014; // CKC-CLKCTRL8 - set HSIO clk to XIN
	*(volatile unsigned long *)addr(0x40004C) = 0x00200014; // CKC-CLKCTRL9 - set Camera Bus clk to XIN

	*(volatile unsigned long *)addr(0x400020) &= ~0x80000000; // CKC-PLL0CFG - PLL disable
	*(volatile unsigned long *)addr(0x400024) &= ~0x80000000; // CKC-PLL1CFG - PLL disable
	*(volatile unsigned long *)addr(0x400028) &= ~0x80000000; // CKC-PLL2CFG - PLL disable
	*(volatile unsigned long *)addr(0x40002C) &= ~0x80000000; // CKC-PLL3CFG - PLL disable
	*(volatile unsigned long *)addr(0x400050) &= ~0x80000000; // CKC-PLL4CFG - PLL disable
	*(volatile unsigned long *)addr(0x400054) &= ~0x80000000; // CKC-PLL5CFG - PLL disable

// Core Voltage Control
#ifdef TCC_SHUTDOWN_CORE_V_CTRL_USED
#if defined(CONFIG_MACH_M801_88) || defined(CONFIG_MACH_M803)
	// Set the coreA to the lowest voltage.
	*(volatile unsigned long *)addr(0x308140) &= ~0x00000003;	// GPIO_F1 to low, GPIO_F0 to low
#endif
#if defined(TCC_PMIC_COREPWR_CTRL_USED) || defined(CONFIG_REGULATOR)
	{
		CoreFuncPtr pFunc = (CoreFuncPtr)(COREPWR_FUNC_PHY);
		pFunc(SLEEP);
	}
#endif
#endif

#if defined(CONFIG_MACH_TCC8800ST)
	/* SLEEP_MODE_CTRL : GPIO_D21 */
	*(volatile unsigned long *)addr(0x3080C4) |= Hw21;	// set output GPIO_D21
	*(volatile unsigned long *)addr(0x3080C0) &= ~Hw21;	// set GPIO_D21 to low

	/*
		CORE_CTL0 : GPIO_E21
		CORE_CTL1 : GPIO_E22

		CORE_CTL0 CORE_CTL1 : Core Voltage
		   Low       Low    :     0.96 V
		   Low       High   :     1.20 V
		   High      High   :     1.35 V		
	*/
	
	// 0.96V
	*(volatile unsigned long *)addr(0x308104) |= (Hw21|Hw22);	// set GPIO_E21 and GPIO_E22 to output 
	*(volatile unsigned long *)addr(0x308100) &= ~(Hw21|Hw22);	// set GPIO_E21 and GPIO_E22 to low
#endif /* CONFIG_MACH_TCC8800ST */

	/* ZQ/VDDQ Power OFF */
#ifdef TCC_MEMBUS_PWR_CTRL_USED
	if(*(volatile unsigned long *)SHUTDOWN_STACK_PHY == 1) // Physical Address
	{
		//*(volatile unsigned long *)addr(0x308044) |= Hw26;
		*(volatile unsigned long *)addr(0x308040) &= ~Hw26; // GPIO_B26
	}
	else if(*(volatile unsigned long *)SHUTDOWN_STACK_PHY == 2) // Physical Address
	{
		//*(volatile unsigned long *)addr(0x3081C4) |= Hw5;
		*(volatile unsigned long *)addr(0x3081C0) &= ~Hw5; // GPIO_G5
	}
#ifdef CONFIG_MACH_M801_88
	else if(*(volatile unsigned long *)SHUTDOWN_STACK_PHY == 3)
	{
		//*(volatile unsigned long *)addr(0x308004) |= Hw15;
		*(volatile unsigned long *)addr(0x308000) &= ~Hw15; // GPIO_A15
	}
#endif
#ifdef CONFIG_MACH_TCC8800ST
//	else if(*(volatile unsigned long *)SHUTDOWN_STACK_PHY == 4) // Physical Address
	{
		*(volatile unsigned long *)addr(0x3081C4) |= Hw5;
		*(volatile unsigned long *)addr(0x3081C0) &= ~Hw5; // GPIO_G5
	}
#endif /* CONFIG_MACH_TCC8800ST */
#endif

	/* Power Down */
	//*((volatile unsigned long *)addr(0x40403C) |= (Hw16|Hw17|Hw18|Hw19); // PWRCFG.MEM_PDN : IRAM0/1 Power Down
	*(volatile unsigned long *)addr(0x40401C) |= Hw0; // PWRCFG.BSRC : Reboot from SRAM
	*(volatile unsigned long *)addr(0x404014) |= Hw12; // CONFIG1.BBEN : Backup RAM Boot Enable
	*(volatile unsigned long *)addr(0x404014) &= ~(Hw10|Hw9|Hw8); // CONFIG1.REMAP
	*(volatile unsigned long *)addr(0x404014) |= (Hw8); // CONFIG1.REMAP
	*(volatile unsigned long *)addr(0x404000) |= (Hw28|Hw31); // CONTROL.IOR_M|IOR : I/O Retention
	//*(volatile unsigned long *)addr(0x404000) |= (1<<4); //L2 Cache retention

	//BUS Power Off
	*(volatile unsigned long *)addr(0x404018) &= ~(Hw0|Hw1|Hw2|Hw3|Hw4|Hw26|Hw29|Hw30); //ip power-off
	*(volatile unsigned long *)addr(0x400044) = ~(*(volatile unsigned long *)addr(0x400044)) & ~(Hw1|Hw3|Hw5|Hw6|Hw8|Hw9); //reset on
	*(volatile unsigned long *)addr(0x305008) |= (Hw4|Hw5); //Memory Bus Config . SW Reset
	for (nCount = 1000; nCount > 0; nCount --);	// Wait
	*(volatile unsigned long *)addr(0x404018) &= ~(Hw5|Hw11|Hw14|Hw17|Hw20|Hw23); //iso : 0
	*(volatile unsigned long *)addr(0x404018) |= (Hw6|Hw12|Hw15|Hw18|Hw21|Hw24); //pre-off : 1
	*(volatile unsigned long *)addr(0x404018) |= (Hw7|Hw13|Hw16|Hw19|Hw22|Hw25); //off : 1

	*(volatile unsigned long *)addr(0x404050) |= Hw20; // CONTROL.MEM_PDN : IRAM Powerdown.!!

	*(volatile unsigned long *)addr(0x404000) |= Hw2; // CONTROL.SHTDN : Shutdown.!!
}

/*===========================================================================
FUNCTION
===========================================================================*/
static void io_restore(void)
{
	volatile unsigned int nCount;
	volatile unsigned *src;
	volatile unsigned *dest;

#if defined(PORT_ARRANGE_USED) && !defined(CONFIG_MACH_TCC8800ST)
	src = (unsigned*)GPIO_BUF_PHY_;
	dest = (unsigned*)addr(0x308000);//&HwGPIO_BASE;

	for(nCount=0 ; nCount<(sizeof(GPIO)/sizeof(unsigned)) ; nCount++)
		dest[nCount] = src[nCount];

	*(volatile unsigned int *)addr(0x404000) &= ~(0x1<<28); // IO Retention Dis
	*(volatile unsigned int *)addr(0x404000) &= ~(0x1<<31); // IO Retention Dis

	src = (unsigned*)GPIO_BUF_PHY;
	dest = (unsigned*)addr(0x308000);//&HwGPIO_BASE;

	for(nCount=0 ; nCount<(sizeof(GPIO)/sizeof(unsigned)) ; nCount++)
		dest[nCount] = src[nCount];
#else
	src = (unsigned*)GPIO_BUF_PHY;
	dest = (unsigned*)addr(0x308000);//&HwGPIO_BASE;

	for(nCount=0 ; nCount<(sizeof(GPIO)/sizeof(unsigned)) ; nCount++)
		dest[nCount] = src[nCount];

	*(volatile unsigned int *)addr(0x404000) &= ~(0x1<<28); // IO Retention Dis
	*(volatile unsigned int *)addr(0x404000) &= ~(0x1<<31); // IO Retention Dis
#endif

	// ZQ/VDDQ Power ON
#ifdef TCC_MEMBUS_PWR_CTRL_USED
	if(*(volatile unsigned long *)SHUTDOWN_STACK_PHY == 1) // Physical Address
	{
		//*(volatile unsigned long *)addr(0x308044) |= Hw26;
		*(volatile unsigned long *)addr(0x308040) |= Hw26; // GPIO_B26
	}
	else if(*(volatile unsigned long *)SHUTDOWN_STACK_PHY == 2) // Physical Address
	{
		//*(volatile unsigned long *)addr(0x3081C4) |= Hw5;
		*(volatile unsigned long *)addr(0x3081C0) |= Hw5; // GPIO_G5
	}
#ifdef CONFIG_MACH_M801_88
	else if(*(volatile unsigned long *)SHUTDOWN_STACK_PHY == 3)
	{
		//*(volatile unsigned long *)addr(0x308004) |= Hw15;
		*(volatile unsigned long *)addr(0x308000) |= Hw15; // GPIO_A15
	}
#endif
#ifdef CONFIG_MACH_TCC8800ST
//	else if(*(volatile unsigned long *)SHUTDOWN_STACK_PHY == 4)
	{
		*(volatile unsigned long *)addr(0x3081C0) |= Hw5; //GPIO_G5
	}
#endif /* CONFIG_MACH_TCC8800ST */
#endif

// Core Voltage Control
#ifdef TCC_SHUTDOWN_CORE_V_CTRL_USED
#if defined(CONFIG_MACH_M801_88) || defined(CONFIG_MACH_M803)
	// Set the coreA to the high voltage.
	*(volatile unsigned long *)addr(0x308140) |= 0x00000002;	// GPIO_F1 to high
#endif
#if defined(TCC_PMIC_COREPWR_CTRL_USED) || defined(CONFIG_REGULATOR)
	{
		CoreFuncPtr pFunc = (CoreFuncPtr)(COREPWR_FUNC_PHY);
		pFunc(WAKEUP);
	}
#endif
#endif

#if defined(CONFIG_MACH_TCC8800ST)
	/* SLEEP_MODE_CTRL : GPIO_D21 */
	*(volatile unsigned long *)addr(0x3080C4) |= Hw21;	// set output GPIO_D21
	*(volatile unsigned long *)addr(0x3080C0) |= Hw21;	// set GPIO_D21 to high

	/*
		CORE_CTL0 : GPIO_E21
		CORE_CTL1 : GPIO_E22

		CORE_CTL0 CORE_CTL1 : Core Voltage
		   Low       Low    :     0.96 V
		   Low       High   :     1.20 V
		   High      High   :     1.35 V		
	*/
	
	// 1.35V
	*(volatile unsigned long *)addr(0x308104) |= (Hw21|Hw22);	// set GPIO_E21 and GPIO_E22 to output 
	*(volatile unsigned long *)addr(0x308100) |= (Hw21 | Hw22);	// set GPIO_E21 and GPIO_E22 to high

#endif /* CONFIG_MACH_TCC8800ST */

	//Bruce, 20110624, for reset from watchdog.
	*(volatile unsigned long *)addr(0x404014) &= ~Hw12; // CONFIG1.BBEN : Backup RAM Boot Enable
	*(volatile unsigned long *)addr(0x404014) &= ~(Hw10|Hw9|Hw8); // CONFIG1.REMAP

	*(volatile unsigned long *)addr(0x400044) = ~(*(volatile unsigned long *)addr(0x400044)) & ~(Hw1|Hw3|Hw5|Hw6|Hw8|Hw9); //reset on
	*(volatile unsigned long *)addr(0x305008) |= (Hw4|Hw5); //Memory Bus Config . SW Reset
	for (nCount = 1000; nCount > 0; nCount --);	// Wait
	*(volatile unsigned long *)addr(0x404018) &= ~(Hw6|Hw12|Hw15|Hw18|Hw21|Hw24); //pre-off : 0
	*(volatile unsigned long *)addr(0x404018) &= ~(Hw7|Hw13|Hw16|Hw19|Hw22|Hw25); //off : 0
	*(volatile unsigned long *)addr(0x404018) |= (Hw5|Hw11|Hw14|Hw17|Hw20|Hw23); //iso : 1
	for (nCount = 1000; nCount > 0; nCount --);	// Wait
	*(volatile unsigned long *)addr(0x400044) = ~(*(volatile unsigned long *)addr(0x400044)) | (Hw1|Hw3|Hw5|Hw6|Hw8|Hw9); //reset off
	*(volatile unsigned long *)addr(0x305008) &= ~(Hw4|Hw5); //Memory Bus Config . SW Reset
	//*(volatile unsigned long *)addr(0x404018) |= (Hw0|Hw1|Hw2|Hw3|Hw4|Hw26|Hw29|Hw30); //ip power-on
	*(volatile unsigned long *)addr(0x404018) |= (Hw1|Hw2|Hw3|Hw4|Hw26|Hw29|Hw30); //ip power-on	-> remove video DAC bit for current
}

/*===========================================================================
FUNCTION
===========================================================================*/
static void edi_init(void)
{

	PEDI pEDI = (PEDI)HwEDI_BASE;
#if defined(CONFIG_MACH_M801_88) || defined(CONFIG_MACH_M803)
	BITCSET(pEDI->EDI_RDYCFG,       0x000FFFFF, 0x00011112 );
	BITCSET(pEDI->EDI_CSNCFG0,      0x00F00FFF, 0x00500876 );
#else 
	if(system_rev == 0x0602 || system_rev == 0x0612)
	{
		BITCSET(pEDI->EDI_RDYCFG,       0x000FFFFF, 0x00011112 );
		BITCSET(pEDI->EDI_CSNCFG0,      0x00F00FFF, 0x00500876 );
	}
	else
	{
		BITCSET(pEDI->EDI_RDYCFG,       0x000FFFFF, 0x00032014 );
		BITCSET(pEDI->EDI_CSNCFG0,      0x00F00FFF, 0x00500876 );
	}
#endif
}


/*===========================================================================
FUNCTION
===========================================================================*/
static void tcc_nfc_suspend(PNFC pBackupNFC, PNFC pHwNFC)
{
	pBackupNFC->NFC_CACYC = pHwNFC->NFC_CACYC;
	pBackupNFC->NFC_WRCYC = pHwNFC->NFC_WRCYC;
	pBackupNFC->NFC_RECYC = pHwNFC->NFC_RECYC;
	pBackupNFC->NFC_CTRL = pHwNFC->NFC_CTRL;
	pBackupNFC->NFC_IRQCFG = pHwNFC->NFC_IRQCFG;
	pBackupNFC->NFC_RFWBASE = pHwNFC->NFC_RFWBASE;
}

/*===========================================================================
FUNCTION
===========================================================================*/
static void tcc_nfc_resume(PNFC pHwNFC, PNFC pBackupNFC)
{
	edi_init();
	pHwNFC->NFC_CACYC = pBackupNFC->NFC_CACYC;
	pHwNFC->NFC_WRCYC = pBackupNFC->NFC_WRCYC;
	pHwNFC->NFC_RECYC = pBackupNFC->NFC_RECYC;
	pHwNFC->NFC_CTRL = pBackupNFC->NFC_CTRL;
	pHwNFC->NFC_IRQCFG = pBackupNFC->NFC_IRQCFG;
	pHwNFC->NFC_IRQ = 0xFFFFFFFF;
	pHwNFC->NFC_RFWBASE = pBackupNFC->NFC_RFWBASE;
}

/*===========================================================================
FUNCTION
===========================================================================*/
static void shutdown_mode(void)
{
	volatile unsigned int nCount = 0;

	CKC *ckc = (CKC *)tcc_p2v(HwCKC_BASE);
	PIC *pic = (PIC *)tcc_p2v(HwPIC_BASE);
	VIC *vic = (VIC *)tcc_p2v(HwVIC_BASE);
	TIMER *timer = (TIMER *)tcc_p2v(HwTMR_BASE);
	PMU *pmu = (PMU *)tcc_p2v(HwPMU_BASE);
	SMUCONFIG *smuconfig = (SMUCONFIG*)tcc_p2v(HwSMUCONFIG_BASE);
	GPIO *gpio = (GPIO *)tcc_p2v(HwGPIO_BASE);
	UARTPORTMUX *uartportmux = (UARTPORTMUX *)tcc_p2v(HwUART_PORTMUX_BASE);

	IOBUSCFG *iobuscfg = (IOBUSCFG *)tcc_p2v(HwIOBUSCFG_BASE);

	NFC *nfc = (NFC *)tcc_p2v(HwNFC_BASE);

	//pReg = ioremap_nocache(DRAM_DATA_ADDRESS, PAGE_ALIGN(sizeof(TCC_REG)));
	pReg = &reg_backup;

	/* backup iobus state */
	pReg->backup_peri_iobus0 = iobuscfg->HCLKEN0;
	pReg->backup_peri_iobus1 = iobuscfg->HCLKEN1;

	/* all peri io bus on */
	iobuscfg->HCLKEN0 = 0xFFFFFFFF;
	iobuscfg->HCLKEN1 = 0xFFFFFFFF;

	/*--------------------------------------------------------------
	 NFC register BackUp
	--------------------------------------------------------------*/
	tcc_nfc_suspend(&pReg->nfc, nfc);	
	
	pmdrv_dbg("Enter Suspend_mode !!\n");

	/*--------------------------------------------------------------
	 UART block suspend
	--------------------------------------------------------------*/
#ifdef CONFIG_PM_CONSOLE_NOT_SUSPEND
	tcc_pm_uart_suspend(&pReg->bkuart);
#endif
	memcpy(&pReg->uartportmux, uartportmux, sizeof(UARTPORTMUX));

	/*--------------------------------------------------------------
	 IO BUS
	--------------------------------------------------------------*/
	memcpy(&pReg->iobuscfg, iobuscfg, sizeof(IOBUSCFG));

	/*--------------------------------------------------------------
	 SMU & PMU
	--------------------------------------------------------------*/
	memcpy(&pReg->smuconfig, smuconfig, sizeof(SMUCONFIG));
	memcpy(&pReg->pmu, pmu, sizeof(PMU));
	memcpy(&pReg->timer, timer, sizeof(TIMER));
	memcpy(&pReg->vic, vic, sizeof(VIC));
	memcpy(&pReg->pic, pic, sizeof(PIC));
	memcpy(&pReg->ckc, ckc, sizeof(CKC));

	/*--------------------------------------------------------------
	 GPIO
	--------------------------------------------------------------*/
	memcpy((void*)GPIO_BUF_ADDR, gpio, sizeof(GPIO));
#ifdef PORT_ARRANGE_USED
	port_arrange();	//for saving current
	memcpy((void*)GPIO_BUF_ADDR_, gpio, sizeof(GPIO));
#endif

	/*--------------------------------------------------------------
	 power on video core & bus to use MMU_SWITCH_EXEC_ADDR
	--------------------------------------------------------------*/
	{
		ckc->CLK6CTRL = 0x00200014; // CKC-CLKCTRL6 - set video core clk to XIN
		pmu->PWROFF &= ~Hw12; //pre-off : 0
		pmu->PWROFF &= ~Hw13; //off : 0
		pmu->PWROFF |= Hw11; //iso : 1
		for (nCount = 20000; nCount > 0; nCount --);	// Wait
		ckc->SWRESET = ~(ckc->SWRESET) | (Hw5|Hw6);
		ckc->CLK5CTRL = 0x00200014; // CKC-CLKCTRL5 - set video bus clk to XIN
		for (nCount = 20000; nCount > 0; nCount --);	// Wait
		*(volatile unsigned long *)addr(0x720000) &= ~(Hw2|Hw3);
		*(volatile unsigned long *)addr(0x720004) &= ~(Hw2|Hw3);

		memcpy((void*)MMU_SWITCH_EXEC_ADDR, (void*)tcc_mmu_switch, MMU_SWITCH_FUNC_SIZE);
	}

	/////////////////////////////////////////////////////////////////
	save_cpu_reg(SHUTDOWN_FUNC_PHY, REG_MMU_DATA_ADDR, resore_cpu_reg);
	/////////////////////////////////////////////////////////////////

	__asm__ __volatile__ ("nop\n");

	/* all peri io bus on */
	iobuscfg->HCLKEN0 = 0xFFFFFFFF;
	iobuscfg->HCLKEN1 = 0xFFFFFFFF;

	/*--------------------------------------------------------------
	 SMU & PMU
	--------------------------------------------------------------*/
	//memcpy(ckc, &pReg->ckc, sizeof(CKC));
	{
		//PLL
		ckc->PLL0CFG = pReg->ckc.PLL0CFG;
		ckc->PLL1CFG = pReg->ckc.PLL1CFG;
		ckc->PLL2CFG = pReg->ckc.PLL2CFG;
		ckc->PLL3CFG = pReg->ckc.PLL3CFG;
		//ckc->PLL4CFG = pReg->ckc.PLL4CFG; //PLL4 is used as Memory Bus Clock
		//ckc->PLL5CFG = pReg->ckc.PLL5CFG;
		nCount = 1000; while(nCount--) NopDelay30();

		//Divider
		ckc->CLKDIVC0 = pReg->ckc.CLKDIVC0;
		ckc->CLKDIVC1 = pReg->ckc.CLKDIVC1;
		ckc->CLKDIVC2 = pReg->ckc.CLKDIVC2;
		NopDelay30();

		//ckc->CLK0CTRL = pReg->ckc.CLK0CTRL;
		ckc->CLK1CTRL = pReg->ckc.CLK1CTRL;
		//ckc->CLK2CTRL = pReg->ckc.CLK2CTRL; //Memory Clock can't be adjusted freely.
		ckc->CLK3CTRL = pReg->ckc.CLK3CTRL;
		ckc->CLK4CTRL = pReg->ckc.CLK4CTRL;
		ckc->CLK5CTRL = pReg->ckc.CLK5CTRL;
		ckc->CLK6CTRL = pReg->ckc.CLK6CTRL;
		ckc->CLK7CTRL = pReg->ckc.CLK7CTRL;
		ckc->CLK8CTRL = pReg->ckc.CLK8CTRL;
		ckc->CLK9CTRL = pReg->ckc.CLK9CTRL;

		ckc->SWRESET = ~(pReg->ckc.SWRESET);

		//Peripheral clock
		memcpy((void*)&(ckc->PCLK_TCX), (void*)&(pReg->ckc.PCLK_TCX), 0x160-0x80);
	}
	NopDelay30();

	memcpy(pic, &pReg->pic, sizeof(PIC));
	memcpy(vic, &pReg->vic, sizeof(VIC));
	memcpy(timer, &pReg->timer, sizeof(TIMER));

	//memcpy(pmu, &pReg->pmu, sizeof(PMU));
	{
		//pmu->PWROFF = pReg->pmu.PWROFF;

		if(pReg->pmu.PWROFF&Hw7) //if DDI Bus has been turn off.
		{
			pmu->PWROFF &= ~(Hw5); //iso : 0
			pmu->PWROFF |= (Hw6); //pre-off : 1
			pmu->PWROFF |= (Hw7); //off : 1
		}
		if(pReg->pmu.PWROFF&Hw13) //if Video Bus has been turn off.
		{
			pmu->PWROFF &= ~(Hw11); //iso : 0
			pmu->PWROFF |= (Hw12); //pre-off : 1
			pmu->PWROFF |= (Hw13); //off : 1
		}
		if(pReg->pmu.PWROFF&Hw16) //if Graphic Bus has been turn off.
		{
			pmu->PWROFF &= ~(Hw14); //iso : 0
			pmu->PWROFF |= (Hw15); //pre-off : 1
			pmu->PWROFF |= (Hw16); //off : 1
		}
		if(pReg->pmu.PWROFF&Hw19) //if Camera Bus has been turn off.
		{
			pmu->PWROFF &= ~(Hw17); //iso : 0
			pmu->PWROFF |= (Hw18); //pre-off : 1
			pmu->PWROFF |= (Hw19); //off : 1
		}
		if(pReg->pmu.PWROFF&Hw22) //if High Speed I/O Bus has been turn off.
		{
			pmu->PWROFF &= ~(Hw20); //iso : 0
			pmu->PWROFF |= (Hw21); //pre-off : 1
			pmu->PWROFF |= (Hw22); //off : 1
		}
		if(pReg->pmu.PWROFF&Hw25) //if Memory Bus has been turn off.
		{
			pmu->PWROFF &= ~(Hw23); //iso : 0
			pmu->PWROFF |= (Hw24); //pre-off : 1
			pmu->PWROFF |= (Hw25); //off : 1
		}

		//ip power
		pmu->PWROFF &= ~(Hw0|Hw1|Hw2|Hw3|Hw4|Hw26|Hw29|Hw30);
		pmu->PWROFF |= (pReg->pmu.PWROFF & (Hw0|Hw1|Hw2|Hw3|Hw4|Hw26|Hw29|Hw30));
	}

	memcpy(smuconfig, &pReg->smuconfig, sizeof(SMUCONFIG));

	/*--------------------------------------------------------------
	 IO BUS
	--------------------------------------------------------------*/
	memcpy(iobuscfg, &pReg->iobuscfg, sizeof(IOBUSCFG));

	/*--------------------------------------------------------------
	 UART block resume
	--------------------------------------------------------------*/
	memcpy(uartportmux, &pReg->uartportmux, sizeof(UARTPORTMUX));
#ifdef CONFIG_PM_CONSOLE_NOT_SUSPEND
	tcc_pm_uart_resume(&pReg->bkuart);
#endif

	pmdrv_dbg("Wake up !!\n");

	/*--------------------------------------------------------------
	 NFC register Restore
	--------------------------------------------------------------*/
	tcc_nfc_resume(nfc, &pReg->nfc);
	
	//all peri io bus restore
	iobuscfg->HCLKEN0 = pReg->backup_peri_iobus0;
	iobuscfg->HCLKEN1 = pReg->backup_peri_iobus1;

	//iounmap(pReg);
}
/*=========================================================================*/
#endif /* TCC_PM_SHUTDOWN_MODE */



#if defined(TCC_PM_SLEEP_MODE)
/*===========================================================================

                                 SLEEP

===========================================================================*/

/*===========================================================================
FUNCTION
===========================================================================*/
static void sleep(void)
{
	volatile unsigned int nCount = 0;
	//CKC *ckc = (CKC *)tcc_p2v(HwCKC_BASE);

// Enter SDRAM Self-refresh ------------------------------------------------------------

#if defined(CONFIG_DRAM_DDR3)
	*(volatile unsigned int *)addr(0x303020) &= ~(0x1<<18); // CSYSREQ3
	if(DDR3_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x309208)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
	else//DDR3_BANK_NUM is 4
		while (((*(volatile unsigned long *)addr(0x309208)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED
#elif defined(CONFIG_DRAM_DDR2)
	*(volatile unsigned int *)addr(0x303020) &= ~(0x1<<17); // CSYSREQ2
	if(DDR2_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x306048)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
	else//DDR2_BANK_NUM == 4
		while (((*(volatile unsigned long *)addr(0x306048)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED
#elif defined(CONFIG_DRAM_LPDDR2)
	*(volatile unsigned int *)addr(0x303020) &= ~(0x1<<17); // CSYSREQ2
	if(LPDDR2_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x306048)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
	else//LPDDR2_BANK_NUM == 4
		while (((*(volatile unsigned long *)addr(0x306048)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED
#else
 #error "not selected"
#endif

	for (nCount = 20000; nCount > 0; nCount --);	// Wait

// Clock Disable -----------------------------------------------------------------------
#if defined(TCC_PM_SLEEP_WFI_USED) && defined(CONFIG_MACH_TCC8800ST)
	{
		*(volatile unsigned long *)addr(0x400034) = 0x00008a00;
		for (nCount = 20000; nCount > 0; nCount --);	// Wait
	}
#endif

#ifdef CONFIG_MACH_TCC8800ST
	*(volatile unsigned long *)addr(0x400000) = 0x002ffffe; // CKC-CLKCTRL0 - set cpu clk to XIN
#else
	*(volatile unsigned long *)addr(0x400000) = 0x002ffff7; // CKC-CLKCTRL0 - set cpu clk to XTIN
#endif
	if(*(volatile unsigned long *)addr(0x400004) & 0x00200000)
		*(volatile unsigned long *)addr(0x400004) = 0x00200017; // CKC-CLKCTRL1 - set display clk to XTIN
#ifdef CONFIG_MACH_TCC8800ST
	*(volatile unsigned long *)addr(0x400008) = 0x0020001e; // CKC-CLKCTRL2 - set memory clk to XIN
#else
	*(volatile unsigned long *)addr(0x400008) = 0x00200017; // CKC-CLKCTRL2 - set memory clk to XTIN
#endif
	if(*(volatile unsigned long *)addr(0x40000C) & 0x00200000)
		*(volatile unsigned long *)addr(0x40000C) = 0x00200017; // CKC-CLKCTRL3 - set graphic clk to XTIN
#ifdef CONFIG_MACH_TCC8800ST
	*(volatile unsigned long *)addr(0x400010) = 0x0020001e; // CKC-CLKCTRL4 - set io clk to XIN
#else
	*(volatile unsigned long *)addr(0x400010) = 0x00200017; // CKC-CLKCTRL4 - set io clk to XTIN
#endif
	if(*(volatile unsigned long *)addr(0x400014) & 0x00200000)
		*(volatile unsigned long *)addr(0x400014) = 0x00200017; // CKC-CLKCTRL5 - set video bus clk to XTIN
	if(*(volatile unsigned long *)addr(0x400018) & 0x00200000)
		*(volatile unsigned long *)addr(0x400018) = 0x00200017; // CKC-CLKCTRL6 - set video core clk to XTIN
#ifdef CONFIG_MACH_TCC8800ST
	*(volatile unsigned long *)addr(0x40001C) = 0x0020001e; // CKC-CLKCTRL7 - set SMU clk to XIN
#else
	*(volatile unsigned long *)addr(0x40001C) = 0x00200017; // CKC-CLKCTRL7 - set SMU clk to XTIN
#endif
	if(*(volatile unsigned long *)addr(0x400048) & 0x00200000)
		*(volatile unsigned long *)addr(0x400048) = 0x00200017; // CKC-CLKCTRL8 - set HSIObus clk to XTIN
	if(*(volatile unsigned long *)addr(0x40004C) & 0x00200000)
		*(volatile unsigned long *)addr(0x40004C) = 0x00200017; // CKC-CLKCTRL9 - set Camera busclk to XTIN

	*(volatile unsigned long *)addr(0x400020) &= ~0x80000000; // CKC-PLL0CFG - PLL disable
	*(volatile unsigned long *)addr(0x400024) &= ~0x80000000; // CKC-PLL1CFG - PLL disable
	*(volatile unsigned long *)addr(0x400028) &= ~0x80000000; // CKC-PLL2CFG - PLL disable
	*(volatile unsigned long *)addr(0x40002C) &= ~0x80000000; // CKC-PLL3CFG - PLL disable
	*(volatile unsigned long *)addr(0x400050) &= ~0x80000000; // CKC-PLL4CFG - PLL disable
	*(volatile unsigned long *)addr(0x400054) &= ~0x80000000; // CKC-PLL5CFG - PLL disable

// Core Voltage Control ----------------------------------------------------------------
#if defined(CONFIG_MACH_M801_88) || defined(CONFIG_MACH_M803)
//	if (machine_is_m801_88()) {
		// Sangwon_temp
		// Core Voltage set to 1.0V
		*(volatile unsigned long *)addr(0x308140) &= ~0x00000003;	// GPIO_F1 to low, GPIO_F0 to low
//	}
#endif
#if defined(TCC_PMIC_COREPWR_CTRL_USED) || defined(CONFIG_REGULATOR)
	{
		CoreFuncPtr pFunc = (CoreFuncPtr)(COREPWR_FUNC_ADDR);
		pFunc(SLEEP);
	}
#endif

#if defined(CONFIG_MACH_TCC8800ST)
	/* SLEEP_MODE_CTRL : GPIO_D21 */
	*(volatile unsigned long *)addr(0x3080C4) |= Hw21;	// set output GPIO_D21
	*(volatile unsigned long *)addr(0x3080C0) &= ~Hw21;	// set GPIO_D21 to low

	/*
		CORE_CTL0 : GPIO_E21
		CORE_CTL1 : GPIO_E22

		CORE_CTL0 CORE_CTL1 : Core Voltage
		   Low       Low    :     0.96 V
		   Low       High   :     1.20 V
		   High      High   :     1.35 V		
	*/
	
	// 0.96V
	*(volatile unsigned long *)addr(0x308104) |= (Hw21|Hw22);	// set GPIO_E21 and GPIO_E22 to output 
	*(volatile unsigned long *)addr(0x308100) &= ~(Hw21 | Hw22);	// set GPIO_E21 and GPIO_E22 to low

#endif /* CONFIG_MACH_TCC8800ST */


// ZQ/VDDQ Power OFF ------------------------------------------------------------------
#ifdef TCC_MEMBUS_PWR_CTRL_USED
	if(*(volatile unsigned long *)SLEEP_STACK_ADDR == 1)
	{
		//*(volatile unsigned long *)addr(0x308044) |= Hw26;
		*(volatile unsigned long *)addr(0x308040) &= ~Hw26; // GPIO_B26
	}
	else if(*(volatile unsigned long *)SLEEP_STACK_ADDR == 2)
	{
		//*(volatile unsigned long *)addr(0x3081C4) |= Hw5;
		*(volatile unsigned long *)addr(0x3081C0) &= ~Hw5; // GPIO_G5
	}
#ifdef CONFIG_MACH_M801_88
	else if(*(volatile unsigned long *)SLEEP_STACK_ADDR == 3)
	{
		//*(volatile unsigned long *)addr(0x308004) |= Hw15;
		*(volatile unsigned long *)addr(0x308000) &= ~Hw15; // GPIO_A15
	}
#endif
#if defined(CONFIG_MACH_TCC8800ST)
	else if(*(volatile unsigned long *)SLEEP_STACK_ADDR == 4)
	{
		*(volatile unsigned long *)addr(0x3081C0) &= ~Hw5; //GPIO_G5
	}
#endif /* CONFIG_MACH_TCC8800ST */
#endif

// Enable I/O Retention ---------------------------------------------------------------
	*(volatile unsigned long *)addr(0x404000) |= (Hw28|Hw31); // CONTROL.IOR_M|IOR : I/O Retention

// BUS Power Off ----------------------------------------------------------------------

	*(volatile unsigned long *)addr(0x404018) &= ~(Hw0|Hw1|Hw2|Hw3|Hw4|Hw26|Hw29|Hw30); //ip power-off
	*(volatile unsigned long *)addr(0x400044) = ~(*(volatile unsigned long *)addr(0x400044)) & ~(Hw1|Hw3|Hw5|Hw6|/*Hw8|*/Hw9); //reset on
	*(volatile unsigned long *)addr(0x305008) |= (Hw4|Hw5); //Memory Bus Config . SW Reset
	for (nCount = 20; nCount > 0; nCount --);	// Wait
	*(volatile unsigned long *)addr(0x404018) &= ~(Hw5|Hw11|Hw14|Hw17|/*Hw20|*/Hw23); //iso : 0
	*(volatile unsigned long *)addr(0x404018) |= (Hw6|Hw12|Hw15|Hw18|/*Hw21|*/Hw24); //pre-off : 1
	*(volatile unsigned long *)addr(0x404018) |= (Hw7|Hw13|Hw16|Hw19|/*Hw22|*/Hw25); //off : 1

// Sleep mode -------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////
#ifdef TCC_PM_SLEEP_WFI_USED
	asm("dsb");
	asm("wfi");
#else
	*(volatile unsigned long *)addr(0x404000) |= Hw1; // CONTROL.SLEEP : Sleep.!!
#endif
///////////////////////////////////////////////////////////////////////////////////////

// Disable I/O Retention --------------------------------------------------------------
	*(volatile unsigned long *)addr(0x404000) &= ~(Hw28|Hw31); // CONTROL.IOR_M|IOR : I/O Retention

// ZQ/VDDQ Power ON -------------------------------------------------------------------
#ifdef TCC_MEMBUS_PWR_CTRL_USED
	if(*(volatile unsigned long *)SLEEP_STACK_ADDR == 1)
	{
		//*(volatile unsigned long *)addr(0x308044) |= Hw26;
		*(volatile unsigned long *)addr(0x308040) |= Hw26; // GPIO_B26
	}
	else if(*(volatile unsigned long *)SLEEP_STACK_ADDR == 2)
	{
		//*(volatile unsigned long *)addr(0x3081C4) |= Hw5;
		*(volatile unsigned long *)addr(0x3081C0) |= Hw5; // GPIO_G5
	}
#ifdef CONFIG_MACH_M801_88
	else if(*(volatile unsigned long *)SLEEP_STACK_ADDR == 3)
	{
		//*(volatile unsigned long *)addr(0x308004) |= Hw15;
		*(volatile unsigned long *)addr(0x308000) |= Hw15; // GPIO_A15
	}
#endif
#if defined(CONFIG_MACH_TCC8800ST)
	else if(*(volatile unsigned long *)SLEEP_STACK_ADDR == 4)
	{
		*(volatile unsigned long *)addr(0x3081C0) |= Hw5; //GPIO_G5
	}
#endif /* CONFIG_MACH_TCC8800ST */
#endif

// Core Voltage Control ----------------------------------------------------------------
#if defined(CONFIG_MACH_M801_88) || defined(CONFIG_MACH_M803)
//	if (machine_is_m801_88()) {
		// Sangwon_temp
		// Core Voltage set to 1.3V
		*(volatile unsigned long *)addr(0x308140) |= 0x00000002;	// GPIO_F1 to high
//	}
#endif
#if defined(TCC_PMIC_COREPWR_CTRL_USED) || defined(CONFIG_REGULATOR)
	{
		CoreFuncPtr pFunc = (CoreFuncPtr)(COREPWR_FUNC_ADDR);
		pFunc(WAKEUP);
	}
#endif

#if defined(CONFIG_MACH_TCC8800ST)
	/* SLEEP_MODE_CTRL : GPIO_D21 */
	*(volatile unsigned long *)addr(0x3080C4) |= Hw21;	// set output GPIO_D21
	*(volatile unsigned long *)addr(0x3080C0) |= Hw21;	// set GPIO_D21 to high

	/*
		CORE_CTL0 : GPIO_E21
		CORE_CTL1 : GPIO_E22

		CORE_CTL0 CORE_CTL1 : Core Voltage
		   Low       Low    :     0.96 V
		   Low       High   :     1.20 V
		   High      High   :     1.35 V		
	*/
	
	// 1.35V
	*(volatile unsigned long *)addr(0x308104) |= (Hw21|Hw22);	// set GPIO_E21 and GPIO_E22 to output 
	*(volatile unsigned long *)addr(0x308100) |= (Hw21|Hw22);	// set GPIO_E21 and GPIO_E22 to high 
#endif /* CONFIG_MACH_TCC8800ST */

// BUS Power On -----------------------------------------------------------------------
	*(volatile unsigned long *)addr(0x400044) = ~(*(volatile unsigned long *)addr(0x400044)) & ~(Hw1|Hw3|Hw5|Hw6|/*Hw8|*/Hw9); //reset on
	*(volatile unsigned long *)addr(0x305008) |= (Hw4|Hw5); //Memory Bus Config . SW Reset
	for (nCount = 20; nCount > 0; nCount --);	// Wait
	*(volatile unsigned long *)addr(0x404018) &= ~(Hw6|Hw12|Hw15|Hw18|/*Hw21|*/Hw24); //pre-off : 0
	*(volatile unsigned long *)addr(0x404018) &= ~(Hw7|Hw13|Hw16|Hw19|/*Hw22|*/Hw25); //off : 0
	*(volatile unsigned long *)addr(0x404018) |= (Hw5|Hw11|Hw14|Hw17|/*Hw20|*/Hw23); //iso : 1
	for (nCount = 20; nCount > 0; nCount --);	// Wait
	*(volatile unsigned long *)addr(0x400044) = ~(*(volatile unsigned long *)addr(0x400044)) | (Hw1|Hw3|Hw5|Hw6|/*Hw8|*/Hw9); //reset off
	*(volatile unsigned long *)addr(0x305008) &= ~(Hw4|Hw5); //Memory Bus Config . SW Reset
	//*(volatile unsigned long *)addr(0x404018) |= (Hw0|Hw1|Hw2|Hw3|Hw4|Hw26|Hw29|Hw30); //ip power-on
	*(volatile unsigned long *)addr(0x404018) |= (Hw1|Hw2|Hw3|Hw4|Hw26|Hw29|Hw30); //ip power-on	-> remove video DAC bit for current

// Exit SDRAM Self-refresh ------------------------------------------------------------
	{
		FuncPtr pFunc = (FuncPtr)(SDRAM_INIT_FUNC_ADDR);
		pFunc();
	}

}

/*===========================================================================
VARIABLE
===========================================================================*/
static CKC ckc_backup; // 0x160 = 352 Bytes
static PMU pmu_backup;
static GPIO gpio_backup;
#ifdef TCC_PM_SLEEP_WFI_USED
static PIC pic_backup;
static unsigned eint_backup;
#endif
#ifdef CONFIG_MACH_TCC8800ST
static LCDC lcdc0_backup, lcdc1_backup;
#endif

/*===========================================================================
FUNCTION
===========================================================================*/
static void sleep_mode(void)
{
	unsigned stack;
	volatile unsigned int nCount;
	FuncPtr  pFunc = (FuncPtr )SLEEP_FUNC_ADDR;
	CKC *ckc = (CKC *)tcc_p2v(HwCKC_BASE);
	PMU *pmu = (PMU *)tcc_p2v(HwPMU_BASE);
#ifdef TCC_PM_SLEEP_WFI_USED
	PIC *pic = (PIC *)tcc_p2v(HwPIC_BASE);
	RTC *rtc = (RTC *)tcc_p2v(HwRTC_BASE);
#endif
	GPIO *gpio = (GPIO *)tcc_p2v(HwGPIO_BASE);
#ifdef CONFIG_MACH_TCC8800ST
	LCDC *lcdc0 = (LCDC *)tcc_p2v(HwLCDC0_BASE);
	LCDC *lcdc1 = (LCDC *)tcc_p2v(HwLCDC1_BASE);
#endif

	pmdrv_dbg("Enter sleep_mode !!\n");
	/*--------------------------------------------------------------
	 save Reg.
	--------------------------------------------------------------*/
	memcpy(&ckc_backup, ckc, sizeof(CKC));
	memcpy(&pmu_backup, pmu, sizeof(PMU));
	memcpy(&gpio_backup, gpio, sizeof(GPIO));
#ifdef TCC_PM_SLEEP_WFI_USED
	memcpy(&pic_backup, pic, sizeof(PIC));
	eint_backup = gpio->EINTSEL0;
#endif
#ifdef CONFIG_MACH_TCC8800ST
	memcpy(&lcdc0_backup, lcdc0, sizeof(LCDC));
	memcpy(&lcdc1_backup, lcdc1, sizeof(LCDC));
#endif

	/*--------------------------------------------------------------
	 Set wake-up interrupt..
	--------------------------------------------------------------*/
#ifdef TCC_PM_SLEEP_WFI_USED
	// 1. Power key. GPIO A3 -------------------
	pic->IEN0 = (Hw3); // Ext0
#if defined(CONFIG_REGULATOR_AXP192_PEK)
	gpio->EINTSEL0 = 31; // External Interrupt 0 <- GPIO_F24
#else
	gpio->EINTSEL0 = 3; // External Interrupt 0 <- GPIO_A3
#endif
	if(system_rev == 0x500 || system_rev == 0x601)		// TCC8800
		pic->POL0 &= ~(Hw3); // Ext0 <- active high(0)
	else // if(system_rev == 0x600)	// TCC8801
		pic->POL0 |= (Hw3); // Ext0 <- active low(1)
	pic->MODE0 |= (Hw3); // Ext0 <- level trigger(1)
	pic->INTMSK0 |= (Hw3); // Ext0 <- INT Mask on

	// 2. RTC ----------------------------------
	rtc->RTCCON |= Hw1;	//RTC write enable
	rtc->INTCON |= Hw0;	//Interrupt Block Write Enable
#ifdef CONFIG_MACH_TCC8800ST
	pic->IEN1 = (Hw10 | Hw11);	// RTC & RemoteController interrupt enable.
#else
	pic->IEN1 = (Hw11);			// RTC interrupt enable
#endif
	pic->POL1 &= ~(Hw11); // RTCINT <- active high(0)
	pic->MODE1 |= (Hw11); // RTC <- level trigger(1)
	pic->INTMSK1 |= (Hw11); // RTC <- INT Mask on
#endif

#ifdef PORT_ARRANGE_USED
	port_arrange();	//for saving current
#endif

	/*--------------------------------------------------------------
	 flush tlb & cache
	--------------------------------------------------------------*/
	local_flush_tlb_all();
	flush_cache_all();


	stack = IO_ARM_ChangeStackSRAM();
	/////////////////////////////////////////////////////////////////
	pFunc();
	/////////////////////////////////////////////////////////////////
	IO_ARM_RestoreStackSRAM(stack);


	/*--------------------------------------------------------------
	 restore CKC
	--------------------------------------------------------------*/
	//memcpy(ckc, &ckc_backup, sizeof(CKC));
	{
		//PLL
		ckc->PLL0CFG = ckc_backup.PLL0CFG;
		ckc->PLL1CFG = ckc_backup.PLL1CFG;
		ckc->PLL2CFG = ckc_backup.PLL2CFG;
		ckc->PLL3CFG = ckc_backup.PLL3CFG;
		//ckc->PLL4CFG = ckc_backup.PLL4CFG; //PLL4 is used as Memory Bus Clock
		//ckc->PLL5CFG = ckc_backup.PLL5CFG;
		nCount = 1000; while(nCount--) NopDelay30();

		//Divider
		ckc->CLKDIVC0 = ckc_backup.CLKDIVC0;
		ckc->CLKDIVC1 = ckc_backup.CLKDIVC1;
		ckc->CLKDIVC2 = ckc_backup.CLKDIVC2;
		NopDelay30();

		//ckc->CLK0CTRL = ckc_backup.CLK0CTRL; //CPU
		ckc->CLK1CTRL = ckc_backup.CLK1CTRL;
		//ckc->CLK2CTRL = ckc_backup.CLK2CTRL; //Memory Clock can't be adjusted freely.
		ckc->CLK3CTRL = ckc_backup.CLK3CTRL;
		ckc->CLK4CTRL = ckc_backup.CLK4CTRL;
		ckc->CLK5CTRL = ckc_backup.CLK5CTRL;
		ckc->CLK6CTRL = ckc_backup.CLK6CTRL;
		ckc->CLK7CTRL = ckc_backup.CLK7CTRL; //SMU
		ckc->CLK8CTRL = ckc_backup.CLK8CTRL;
		ckc->CLK9CTRL = ckc_backup.CLK9CTRL;

		ckc->SWRESET = ~(ckc_backup.SWRESET);

		//Peripheral clock
		memcpy((void*)&(ckc->PCLK_TCX), (void*)&(ckc_backup.PCLK_TCX), 0x160-0x80);
	}

	/*--------------------------------------------------------------
	 restore PMU
	--------------------------------------------------------------*/
	//memcpy(pmu, &pmu_backup, sizeof(PMU));
	{
		//pmu->PWROFF = pmu_backup.PWROFF;

		if(pmu_backup.PWROFF&Hw7) //if DDI Bus has been turn off.
		{
			pmu->PWROFF &= ~(Hw5); //iso : 0
			pmu->PWROFF |= (Hw6); //pre-off : 1
			pmu->PWROFF |= (Hw7); //off : 1
		}
		if(pmu_backup.PWROFF&Hw13) //if Video Bus has been turn off.
		{
			pmu->PWROFF &= ~(Hw11); //iso : 0
			pmu->PWROFF |= (Hw12); //pre-off : 1
			pmu->PWROFF |= (Hw13); //off : 1
		}
		if(pmu_backup.PWROFF&Hw16) //if Graphic Bus has been turn off.
		{
			pmu->PWROFF &= ~(Hw14); //iso : 0
			pmu->PWROFF |= (Hw15); //pre-off : 1
			pmu->PWROFF |= (Hw16); //off : 1
		}
		if(pmu_backup.PWROFF&Hw19) //if Camera Bus has been turn off.
		{
			pmu->PWROFF &= ~(Hw17); //iso : 0
			pmu->PWROFF |= (Hw18); //pre-off : 1
			pmu->PWROFF |= (Hw19); //off : 1
		}
		if(pmu_backup.PWROFF&Hw22) //if High Speed I/O Bus has been turn off.
		{
			pmu->PWROFF &= ~(Hw20); //iso : 0
			pmu->PWROFF |= (Hw21); //pre-off : 1
			pmu->PWROFF |= (Hw22); //off : 1
		}
		if(pmu_backup.PWROFF&Hw25) //if Memory Bus has been turn off.
		{
			pmu->PWROFF &= ~(Hw23); //iso : 0
			pmu->PWROFF |= (Hw24); //pre-off : 1
			pmu->PWROFF |= (Hw25); //off : 1
		}

		//ip power
		pmu->PWROFF &= ~(Hw0|Hw1|Hw2|Hw3|Hw4|Hw26|Hw29|Hw30);
		pmu->PWROFF |= (pmu_backup.PWROFF & (Hw0|Hw1|Hw2|Hw3|Hw4|Hw26|Hw29|Hw30));
	}

	/*--------------------------------------------------------------
	 restore GPIO
	--------------------------------------------------------------*/
	memcpy(gpio, &gpio_backup, sizeof(GPIO));

#ifdef TCC_PM_SLEEP_WFI_USED
	/*--------------------------------------------------------------
	 restore PIC
	--------------------------------------------------------------*/
	memcpy(pic, &pic_backup, sizeof(PIC));

	/*--------------------------------------------------------------
	 restore External interrupt selection reg 0
	--------------------------------------------------------------*/
	gpio->EINTSEL0 = eint_backup;
#endif

#ifdef CONFIG_MACH_TCC8800ST
	/*--------------------------------------------------------------
	 restore LCDC
	--------------------------------------------------------------*/
	memcpy(lcdc0, &lcdc0_backup, sizeof(LCDC));
	memcpy(lcdc1, &lcdc1_backup, sizeof(LCDC));
#endif

	pmdrv_dbg("Wake up !!\n");
}
/*=========================================================================*/
#endif /* TCC_PM_SLEEP_MODE */




/*===========================================================================

                               SDRAM Init


===========================================================================*/

#if defined(TCC_PM_SHUTDOWN_MODE)
/*===========================================================================
FUNCTION
===========================================================================*/
static void sdram_init_shutdown(void)
{
	volatile unsigned int i;
#if defined(CONFIG_DRAM_DDR3)
	register unsigned int mr;

	#define DDR3_CLK      200
	//#define nCK (1000000/(DDR3_MAX_SPEED/2))
	#define tCK (1000000/DDR3_CLK)
#elif defined(CONFIG_DRAM_DDR2)
	register unsigned int tmp;
#if defined(CONFIG_MACH_TCC8800ST)
	#define DDR2_CLK      400
#else
	#define DDR2_CLK      180
#endif /* CONFIG_MACH_TCC8800ST */
	//#define nCK (1000000/(DDR2_MAX_SPEED/2))
	#define tCK (1000000/DDR2_CLK)
#elif defined(CONFIG_DRAM_LPDDR2)
	register unsigned int tmp;

	#define LPDDR2_CLK    180
	//#define nCK (1000000/(LPDDR2_MAX_SPEED/2))
	#define tCK (1000000/LPDDR2_CLK)
#endif


//--------------------------------------------------------------------------
// Change to config mode

#if defined(CONFIG_DRAM_DDR3)
	*(volatile unsigned long *)addr(0x303020 ) = 0x0003010b ;//EMCCFG

	if(DDR3_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x309208)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
	else//DDR3_BANK_NUM is 4
		while (((*(volatile unsigned long *)addr(0x309208)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED
#if (0)
	if(DDR3_LOGICAL_CHIP_NUM == 2){
		if(DDR3_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr(0x30920C)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
		else//DDR3_BANK_NUM is 4
			while (((*(volatile unsigned long *)addr(0x30920C)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED
	}
#endif
#elif defined(CONFIG_DRAM_DDR2)
	*(volatile unsigned long *)addr(0x303020 ) =  0x0005010A;

	if(DDR2_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x306048)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
	else//DDR2_BANK_NUM == 4
		while (((*(volatile unsigned long *)addr(0x306048)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED

#if (0)
	if(DDR2_LOGICAL_CHIP_NUM == 2){
		if(DDR2_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
		else//DDR2_BANK_NUM == 4
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED
	}
#endif
#elif defined(CONFIG_DRAM_LPDDR2)
	*(volatile unsigned long *)addr(0x303020 ) =  0x0005010A;

	if(LPDDR2_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x306048)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
	else//LPDDR2_BANK_NUM == 4
		while (((*(volatile unsigned long *)addr(0x306048)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED

#if (0)
	if(LPDDR2_LOGICAL_CHIP_NUM == 2){
		if(LPDDR2_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
		else//LPDDR2_BANK_NUM == 4
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED
	}
#endif
#endif

//--------------------------------------------------------------------------
// Clock setting..

	//clock setting start
	//Set CLKDIVC0,CLKDIVC1. But it is diabled.
	*(volatile unsigned long *)addr(0x400030) = 0x01010101;
	*(volatile unsigned long *)addr(0x400034) = 0x01010101;
	*(volatile unsigned long *)addr(0x400038) = 0x01010101;

	//cpu bus - DirectXIN,
	*(volatile unsigned long *)addr(0x400000) = 0x002ffff4;
	//mem bus - DirectXIN/2
	*(volatile unsigned long *)addr(0x400008) = 0x002f1f14;
	//io bus - DirectXIN/2
	*(volatile unsigned long *)addr(0x400010) = 0x002f1f14;
	//smu bus
	*(volatile unsigned long *)addr(0x40001C) = 0x002f1f14;

	//PLL5 - 600MHz
	*(volatile unsigned long *)addr(0x400054) = 0x01012C03;
	*(volatile unsigned long *)addr(0x400054) = 0x81012C03;

	//MEM PLL
	//*(volatile unsigned long *)addr(0x400050) = 0x02012203; // 290MHz
	//*(volatile unsigned long *)addr(0x400050) = 0x82012203;
#if defined(CONFIG_MACH_TCC8800ST)
	*(volatile unsigned long *)addr(0x400050) = 0x01019003; // 800MHz
	*(volatile unsigned long *)addr(0x400050) = 0xC1019003;
#else
	*(volatile unsigned long *)addr(0x400050) = 0x02014003; // 320MHz
	*(volatile unsigned long *)addr(0x400050) = 0x82014003;
#endif

	for (i = 3000; i > 0; i --);	// Wait

	//cpu bus - PLL5, 600 = 600MHz
	*(volatile unsigned long *)addr(0x400000) = 0x002ffff9;

	//mem bus - PLL4, 290/2 = 145MHz
	*(volatile unsigned long *)addr(0x400008) = 0x00200018;
	//io bus - PLL4, 290/2 = 145MHz
	*(volatile unsigned long *)addr(0x400010) = 0x00200018;
	//smu bus
	*(volatile unsigned long *)addr(0x40001C) = 0x00200039;

	for (i = 3000; i > 0; i --);	// Wait

#if defined(CONFIG_DRAM_DDR3)
//--------------------------------------------------------------------------
// Controller setting

	*(volatile unsigned long *)addr(0x303024 ) = 0x00000300 ;//PHYCFG
	*(volatile unsigned long *)addr(0x304400 ) = 0x0000000A ;//PHYMODE

	//Bruce, 101029, modify according to soc guide
	//*(volatile unsigned long *)addr(0x309400 ) = 0x00101708  ;//PhyControl0
	*(volatile unsigned long *)addr(0x309400 ) = 0x0110140A  ;//PhyControl0

	*(volatile unsigned long *)addr(0x309404 ) = 0x00000086  ;//PhyControl1
	*(volatile unsigned long *)addr(0x309408 ) = 0x00000000  ;//PhyControl2
	*(volatile unsigned long *)addr(0x30940c ) = 0x00000000  ;//PhyControl3
	*(volatile unsigned long *)addr(0x309410 ) = 0x201c7004  ;//PhyControl4

	//Bruce, 101029, modify according to soc guide
	//*(volatile unsigned long *)addr(0x309400 ) = 0x0110170B  ;//PhyControl0
	//while (((*(volatile unsigned long *)addr(0x309418)) & (0x04)) != 4);	// dll locked
	if(DDR3_CLK >= 333)
		*(volatile unsigned long *)addr(0x309400 ) = 0x0110140B  ;//PhyControl0
	else
		*(volatile unsigned long *)addr(0x309400 ) = 0x0110140F  ;//PhyControl0
	while (((*(volatile unsigned long *)addr(0x309418)) & (0x04)) != 4);	// dll locked

	*(volatile unsigned long *)addr(0x309404 ) = 0x0000008e  ;//PhyControl1
	*(volatile unsigned long *)addr(0x309404 ) = 0x00000086  ;//PhyControl1

	//Bruce, 101029, modify according to soc guide
	//*(volatile unsigned long *)addr(0x309414 ) = 0x00030003  ;//PhyControl5
	*(volatile unsigned long *)addr(0x309414 ) = 0x00020003  ;//PhyControl5

	*(volatile unsigned long *)addr(0x309414 ) = 0x0000000b | (MEMCTRL_DDS<<15) | (MEMCTRL_TERM<<11);//PhyControl5

	while (((*(volatile unsigned long *)addr(0x309420)) & (0x01)) != 1);	// zq end

//--------------------------------------------------------------------------
// Memory config

	*(volatile unsigned long *)addr(0x309004 ) = 0x0000018A ; //MemControl

	if(DDR3_BURST_LEN == BL_8) // BL
		BITCSET(*(volatile unsigned long *)addr(0x309004), 0x7<<7, 0x3<<7);
	else
		BITCSET(*(volatile unsigned long *)addr(0x309004), 0x7<<7, 0x2<<7);

	if(DDR3_LOGICAL_CHIP_NUM == 1) // num_chip
		BITCSET(*(volatile unsigned long *)addr(0x309004), 0x3<<5, 0x0);
	else
		BITCSET(*(volatile unsigned long *)addr(0x309004), 0x3<<5, 0x1<<5);

    // Chip 0 Configuration ------------------------------------------------
    {
		*(volatile unsigned long *)addr(0x309008) = 0x40F01313; //MemConfig0 //address mapping method - interleaved
		BITCSET(*(volatile unsigned long *)addr(0x309008), 0xFF<<16, (0xFF - ((DDR3_TOTAL_MB_SIZE)/(DDR3_LOGICAL_CHIP_NUM*0x10)-1))<<16);//set chip mask
		BITCSET(*(volatile unsigned long *)addr(0x309008), 0xF<<8, (DDR3_COLBITS - 7)<<8);//set column bits
		BITCSET(*(volatile unsigned long *)addr(0x309008), 0xF<<4, (DDR3_ROWBITS - 12)<<4);//set row bits
		if(DDR3_BANK_NUM == 8)//8 banks
			BITCSET(*(volatile unsigned long *)addr(0x309008), 0xF, 0x3);
		else // 4 banks
			BITCSET(*(volatile unsigned long *)addr(0x309008), 0xF, 0x2);
    }

    // Chip 1 Configuration ------------------------------------------------
	if(DDR3_LOGICAL_CHIP_NUM == 2)
	{
		*(volatile unsigned long *)addr(0x30900C) = 0x50E01313; //MemConfig1 //address mapping method - interleaved
		BITCSET(*(volatile unsigned long *)addr(0x30900C), 0xFF<<24, (0x40 + (DDR3_TOTAL_MB_SIZE)/(DDR3_LOGICAL_CHIP_NUM*0x10))<<24);//set chip base
		BITCSET(*(volatile unsigned long *)addr(0x30900C), 0xFF<<16, (0xFF - ((DDR3_TOTAL_MB_SIZE)/(DDR3_LOGICAL_CHIP_NUM*0x10)-1))<<16);//set chip mask
		BITCSET(*(volatile unsigned long *)addr(0x30900C), 0xF<<8, (DDR3_COLBITS - 7)<<8);//set column bits
		BITCSET(*(volatile unsigned long *)addr(0x30900C), 0xF<<4, (DDR3_ROWBITS - 12)<<4);//set row bits
		if(DDR3_BANK_NUM == 8)// 8 banks
			BITCSET(*(volatile unsigned long *)addr(0x30900C), 0xF, 0x3);
		else // 4 banks
			BITCSET(*(volatile unsigned long *)addr(0x30900C), 0xF, 0x2);
	}

//--------------------------------------------------------------------------

	*(volatile unsigned long *)addr(0x309000) = 0x40FF3010  ;//ConControl
	*(volatile unsigned long *)addr(0x309014) = 0x01000000  ;//PrechConfig

//--------------------------------------------------------------------------
// Timing parameter setting.

#if (1)
	*(volatile unsigned long *)addr(0x309100) = time2cycle(DDR3_tREFI_ps, tCK); //T_REFI
	*(volatile unsigned long *)addr(0x309104) = time2cycle(DDR3_tRFC_ps, tCK); //T_RFC
	*(volatile unsigned long *)addr(0x309108) = time2cycle(DDR3_tRRD_ps, tCK); //T_RRD
	if(*(volatile unsigned long *)addr(0x309108)<DDR3_tRRD_ck)
		*(volatile unsigned long *)addr(0x309108) = DDR3_tRRD_ck;
	*(volatile unsigned long *)addr(0x30910c) = DDR3_CL; //T_RP
	*(volatile unsigned long *)addr(0x309110) = DDR3_CL; //T_RCD
	*(volatile unsigned long *)addr(0x309114) = time2cycle(DDR3_tRC_ps, tCK); //T_RC
	*(volatile unsigned long *)addr(0x309118) = time2cycle(DDR3_tRAS_ps, tCK); //T_RAS
	*(volatile unsigned long *)addr(0x30911c) = time2cycle(DDR3_tWTR_ps, tCK); //T_WTR
	if(*(volatile unsigned long *)addr(0x30911c)<DDR3_tWTR_ck)
		*(volatile unsigned long *)addr(0x30911c) = DDR3_tWTR_ck;
	*(volatile unsigned long *)addr(0x309120) = time2cycle(DDR3_tWR_ps, tCK); //T_WR
	*(volatile unsigned long *)addr(0x309124) = time2cycle(DDR3_tRTP_ps, tCK); //T_RTP
	if(*(volatile unsigned long *)addr(0x309124)<DDR3_tRTP_ck)
		*(volatile unsigned long *)addr(0x309124) = DDR3_tRTP_ck;
	*(volatile unsigned long *)addr(0x309128) = DDR3_CL; //CL

	if(tCK >= 2500 /* 2.5 ns */)
		*(volatile unsigned long *)addr(0x30912c) = 5;
	else if(tCK >= 1875 /* 1.875 ns */)
		*(volatile unsigned long *)addr(0x30912c) = 6;
	else if(tCK >= 1500 /* 1.5 ns */)
		*(volatile unsigned long *)addr(0x30912c) = 7;
	else if(tCK >= 1250 /* 1.25 ns */)
		*(volatile unsigned long *)addr(0x30912c) = 8;
	else if(tCK >= 1070 /* 1.07 ns */)
		*(volatile unsigned long *)addr(0x30912c) = 9;
	else if(tCK >= 935 /* 0.935 ns */)
		*(volatile unsigned long *)addr(0x30912c) = 10;
	else if(tCK >= 833 /* 0.833 ns */)
		*(volatile unsigned long *)addr(0x30912c) = 11;
	else if(tCK >= 750 /* 0.75 ns */)
		*(volatile unsigned long *)addr(0x30912c) = 12;

	*(volatile unsigned long *)addr(0x309130) = DDR3_CL; //RL = AL+CL

	if(DDR3_AL == AL_CL_MINUS_ONE){ //nAL = nCL - 1;
		*(volatile unsigned long *)addr(0x30912c) += (DDR3_CL-1); //WL = AL+CWL
		*(volatile unsigned long *)addr(0x309130) += (DDR3_CL-1); //RL = AL+CL
	}else if(DDR3_AL == AL_CL_MINUS_TWO){ //	nAL = nCL - 2;
		*(volatile unsigned long *)addr(0x30912c) += (DDR3_CL-2); //WL = AL+CWL
		*(volatile unsigned long *)addr(0x309130) += (DDR3_CL-2); //RL = AL+CL
	}

	*(volatile unsigned long *)addr(0x309134) = time2cycle(DDR3_tFAW_ps, tCK); //T_FAW
	*(volatile unsigned long *)addr(0x309138) = time2cycle(DDR3_tXS_ps, tCK); //T_XSR
	*(volatile unsigned long *)addr(0x30913c) = time2cycle(DDR3_tXP_ps, tCK); //T_XP
	if(*(volatile unsigned long *)addr(0x30913c)<DDR3_tXP_ck)
		*(volatile unsigned long *)addr(0x30913c) = DDR3_tXP_ck;
	*(volatile unsigned long *)addr(0x309140) = time2cycle(DDR3_tCKE_ps, tCK); //T_CKE
	if(*(volatile unsigned long *)addr(0x309140)<DDR3_tCKE_ck)
		*(volatile unsigned long *)addr(0x309140) = DDR3_tCKE_ck;
	*(volatile unsigned long *)addr(0x309144) = DDR3_tMRD_ck; //T_MRD
#else
	*(volatile unsigned long *)addr(0x309100) = 0x618;
	*(volatile unsigned long *)addr(0x309104) = 0x16;
	*(volatile unsigned long *)addr(0x309108) = 0x4;
	*(volatile unsigned long *)addr(0x30910c) = 0x6;
	*(volatile unsigned long *)addr(0x309110) = 0x6;
	*(volatile unsigned long *)addr(0x309114) = 0xA;
	*(volatile unsigned long *)addr(0x309118) = 0x8;
	*(volatile unsigned long *)addr(0x30911c) = 0x4;
	*(volatile unsigned long *)addr(0x309120) = 0x3;
	*(volatile unsigned long *)addr(0x309124) = 0x4;
	*(volatile unsigned long *)addr(0x309128) = 0x6;
	*(volatile unsigned long *)addr(0x30912c) = 0x5;
	*(volatile unsigned long *)addr(0x309130) = 0x6;
	*(volatile unsigned long *)addr(0x309134) = 0x9;
	*(volatile unsigned long *)addr(0x309138) = 0x18;
	*(volatile unsigned long *)addr(0x30913c) = 0x3;
	*(volatile unsigned long *)addr(0x309140) = 0x3;
	*(volatile unsigned long *)addr(0x309144) = 0x4;
#endif

//--------------------------------------------------------------------------
// MRS Setting

	*(volatile unsigned long *)addr(0x309010) = 0x08000000 ;//DirectCmd - XSR

	//after 500 us
	*(volatile unsigned long *)addr(0x309010) = 0x07000000;//DirectCmd - NOP

	//*(volatile unsigned long *)addr(0x309010) = 0x00020000;//DirectCmd - MRS : MR2
	{
		mr = 0x00020000;

		if(DDR3_CLK*2 <= DDR3_800)
			mr |= 0;
		else if(DDR3_CLK*2 <= DDR3_1066)
			mr |= (1<<3);
		else if(DDR3_CLK*2 <= DDR3_1333)
			mr |= (2<<3);
		else if(DDR3_CLK*2 <= DDR3_1600)
			mr |= (3<<3);

		*(volatile unsigned long *)addr(0x309010) = mr;
	}

	*(volatile unsigned long *)addr(0x309010) = 0x00030000;//DirectCmd - MRS : MR3
	*(volatile unsigned long *)addr(0x309010) = 0x00010000 | (DDR3_AL<<3) | (DDR3_ODT<<2) | (DDR3_DIC<<1);//DirectCmd - MRS : MR1 : DLL(enable)

	//*(volatile unsigned long *)addr(0x309010) = 0x00001220;//DirectCmd - MRS : MR0 : DLLPRE(off), WR(), DLL Reset(Yes), MODE(0), CL(), BL(8)
	{
		mr = DDR3_BURST_LEN | (DDR3_READ_BURST_TYPE<<3) | (FAST_EXIT<<12);

		if(DDR3_CL < 5)
			mr |= ((5-4)<<4);
		else if(DDR3_CL > 11)
			mr |= ((11-4)<<4);
		else
			mr |= ((DDR3_CL-4)<<4);

		if(tCK >= 2500 /* 2.5 ns */)
			mr |= (WR_5<<9);
		else if(tCK >= 1875 /* 1.875 ns */)
			mr |= (WR_6<<9);
		else if(tCK >= 1500 /* 1.5 ns */)
			mr |= (WR_7<<9);
		else if(tCK >= 1250 /* 1.25 ns */)
			mr |= (WR_8<<9);
		else if(tCK >= 935 /* 0.935 ns */)
			mr |= (WR_10<<9);
		else
			mr |= (WR_12<<9);

		*(volatile unsigned long *)addr(0x309010) = mr;
	}

	*(volatile unsigned long *)addr(0x309010) = 0x0a000400 ;//DirectCmd - ZQCL

	if(DDR3_LOGICAL_CHIP_NUM == 2){
		*(volatile unsigned long *)addr(0x309010) = 0x08000000 | Hw20;//DirectCmd - XSR
		
		//after 500 us
		*(volatile unsigned long *)addr(0x309010) = 0x07000000 | Hw20;//DirectCmd - NOP

		//*(volatile unsigned long *)addr(0x309010) = 0x00020000;//DirectCmd - MRS : MR2
		{
			mr = 0x00020000;

			if(DDR3_CLK*2 <= DDR3_800)
				mr |= 0;
			else if(DDR3_CLK*2 <= DDR3_1066)
				mr |= (1<<3);
			else if(DDR3_CLK*2 <= DDR3_1333)
				mr |= (2<<3);
			else if(DDR3_CLK*2 <= DDR3_1600)
				mr |= (3<<3);

			*(volatile unsigned long *)addr(0x309010) = mr | Hw20;
		}

		*(volatile unsigned long *)addr(0x309010) = 0x00030000 | Hw20;//DirectCmd - MRS : MR3
		*(volatile unsigned long *)addr(0x309010) = 0x00010000 | (DDR3_AL<<3) | (DDR3_ODT<<2) | (DDR3_DIC<<1) | Hw20;//DirectCmd - MRS : MR1 : DLL(enable)

		//*(volatile unsigned long *)addr(0x309010) = 0x00001420 | Hw20;//DirectCmd - MRS : MR0 : DLLPRE(off), WR(), DLL Reset(Yes), MODE(0), CL(), BL(8)
		{
			mr = DDR3_BURST_LEN | (DDR3_READ_BURST_TYPE<<3) | (FAST_EXIT<<12);

			if(DDR3_CL < 5)
				mr |= ((5-4)<<4);
			else if(DDR3_CL > 11)
				mr |= ((11-4)<<4);
			else
				mr |= ((DDR3_CL-4)<<4);

			if(tCK >= 2500 /* 2.5 ns */)
				mr |= (WR_5<<9);
			else if(tCK >= 1875 /* 1.875 ns */)
				mr |= (WR_6<<9);
			else if(tCK >= 1500 /* 1.5 ns */)
				mr |= (WR_7<<9);
			else if(tCK >= 1250 /* 1.25 ns */)
				mr |= (WR_8<<9);
			else if(tCK >= 935 /* 0.935 ns */)
				mr |= (WR_10<<9);
			else
				mr |= (WR_12<<9);

			*(volatile unsigned long *)addr(0x309010) = mr | Hw20;
		}

		*(volatile unsigned long *)addr(0x309010) = 0x0a000400 | Hw20;//DirectCmd - ZQCL
	}

//--------------------------------------------------------------------------

	*(volatile unsigned long *)addr(0x309000) = 0x60ff3030  ;//ConControl
	*(volatile unsigned long *)addr(0x303020) = 0x0007010b ;//EMCCFG

	if(DDR3_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x309208)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
	else //DDR3_BANK_NUM is 4
		while (((*(volatile unsigned long *)addr(0x309208)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED

	if(DDR3_LOGICAL_CHIP_NUM == 2){
		if(DDR3_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr(0x30920C)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
		else //DDR3_BANK_NUM is 4
			while (((*(volatile unsigned long *)addr(0x30920C)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED
	}

//--------------------------------------------------------------------------
#elif defined(CONFIG_DRAM_DDR2)

//--------------------------------------------------------------------------
// Controller setting

	//phy configuration
	*(volatile unsigned long *)addr(0x303024 ) = 0x200;//PHYCFG

	//PhyZQControl
	if (DDR2_CLK >= DDR2_ODT_ENABLE_MIN_FREQ) {
		*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL ;
		*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL | Hw1 ;//zq start
	} else {
		*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL | Hw0;
		*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL | Hw1 | Hw0 ;//zq start
	}
	while (((*(volatile unsigned long *)addr(0x306040)) & (0x10000)) != 0x10000);	// Wait until ZQ End

	if (DDR2_CLK >= DDR2_ODT_ENABLE_MIN_FREQ) {
		*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL ;
	} else {
		*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL | Hw0;
	}

	*(volatile unsigned long *)addr(0x306018 ) = 0x0010100A; //PHY Control0
	*(volatile unsigned long *)addr(0x30601C ) = 0xE0000086; //PHY Control1 // modify by crony : [31:30] : ODT Enable for Write and Read
	*(volatile unsigned long *)addr(0x306020 ) = 0x00000000; //PHY Control2
	*(volatile unsigned long *)addr(0x306024 ) = 0x00000000; //PHY Control3
	*(volatile unsigned long *)addr(0x306018 ) = 0x0010100B; //PHY Control0

	while (((*(volatile unsigned long *)addr(0x306040)) & (0x7)) != 0x7);// Wait until FLOCK == 1

	//PHY Control1
	*(volatile unsigned long *)addr(0x30601C) = 0xE000008E; //resync = 1
	*(volatile unsigned long *)addr(0x30601C) = 0xE0000086; //resync = 0

//--------------------------------------------------------------------------
// Memory config

	//Enable Out of order scheduling
	*(volatile unsigned long *)addr(0x306000 ) = 0x30FF2018;

	//MEMCTRL
	*(volatile unsigned long *)addr(0x306004 ) = (0x2 << 20) |
	                                             ((DDR2_LOGICAL_CHIP_NUM-1)<<16) |
	                                             ((DDR2_LOGICAL_DATA_BITS/16)<<12) |
	                                             (0x4 << 8) |
	                                             (0x0 << 6) |
	                                             (0x0 << 5) |
	                                             (0x0 << 4) |
	                                             (0x0 << 2) |
	                                             (0x0 << 1) |
	                                             (0x0);

	//MEMCHIP0
	*(volatile unsigned long *)addr(0x306008 ) = (0x40<<24) |
	                                             ((0xFF - (DDR2_TOTAL_MB_SIZE/(DDR2_LOGICAL_CHIP_NUM*0x10)-1))<<16) |
	                                             (0x1 << 12) |
	                                             ((DDR2_COLBITS - 7)<<8) |
	                                             ((DDR2_ROWBITS - 12)<<4) |
	                                             DDR2_BANK_BITS;

	//MEMCHIP1
	if(DDR2_LOGICAL_CHIP_NUM == 2)
	*(volatile unsigned long *)addr(0x30600C ) = ((0x40 + DDR2_TOTAL_MB_SIZE/(DDR2_LOGICAL_CHIP_NUM*0x10))<<24) |
		                                         ((0xFF - (DDR2_TOTAL_MB_SIZE/(DDR2_LOGICAL_CHIP_NUM*0x10)-1))<<16) |
		                                         (0x1 << 12) |
		                                         ((DDR2_COLBITS - 7)<<8) |
		                                         ((DDR2_ROWBITS - 12)<<4) |
		                                         DDR2_BANK_BITS;

//--------------------------------------------------------------------------

	*(volatile unsigned long *)addr(0x306014 ) = 0x0; //PRECONFIG
	*(volatile unsigned long *)addr(0x306028 ) = 0xFFFF00FF; //PRECONFIG

//--------------------------------------------------------------------------
// Timing parameter setting.

	//T_REFI
	*(volatile unsigned long *)addr(0x306030 ) = time2cycle(DDR2_tREFI_ps, tCK);

	//TROW
	*(volatile unsigned long *)addr(0x306034 ) = time2cycle(DDR2_tRAS_ps, tCK); //tRAS
	*(volatile unsigned long *)addr(0x306034 ) |= (time2cycle(DDR2_tRC_ps, tCK)<<6); //tRC
	*(volatile unsigned long *)addr(0x306034 ) |= (DDR2_CL<<12); //tRCD
	*(volatile unsigned long *)addr(0x306034 ) |= (DDR2_CL<<16); //tRP
	*(volatile unsigned long *)addr(0x306034 ) |= (time2cycle(DDR2_tRRD_ps, tCK)<<20); //tRRD
	*(volatile unsigned long *)addr(0x306034 ) |= (time2cycle(DDR2_tRFC_ps, tCK)<<24); //tRFC

	//TDATA
	*(volatile unsigned long *)addr(0x306038 ) = DDR2_CL; //tRL
	*(volatile unsigned long *)addr(0x306038 ) |= ((DDR2_CL-1)<<8); //tWL
	*(volatile unsigned long *)addr(0x306038 ) |= (DDR2_CL<<16); //tCL
	tmp = time2cycle(DDR2_tRTP_ps, tCK);
	if(tmp<DDR2_tRTP_ck) tmp=DDR2_tRTP_ck;
	*(volatile unsigned long *)addr(0x306038 ) |= (tmp<<20); //tRTP
	*(volatile unsigned long *)addr(0x306038 ) |= (time2cycle(DDR2_tWR_ps, tCK)<<24); //tWR
	tmp = time2cycle(DDR2_tWTR_ps, tCK);
	if(tmp<DDR2_tWTR_ck) tmp=DDR2_tWTR_ck;
	*(volatile unsigned long *)addr(0x306038 ) |= (tmp<<28); //tWTR

	//TPOWER
	*(volatile unsigned long *)addr(0x30603C ) = DDR2_tMRD_ck; //tMRD
	*(volatile unsigned long *)addr(0x30603C ) |= (DDR2_tCKE_ck<<4); //tCKE
	*(volatile unsigned long *)addr(0x30603C ) |= (DDR2_tXP_ck<<8); //tXP
	*(volatile unsigned long *)addr(0x30603C ) |= (DDR2_tXSR_ck<<16); //tXSR
	*(volatile unsigned long *)addr(0x30603C ) |= (time2cycle(DDR2_tFAW_ps, tCK)<<24); //tFAW

//--------------------------------------------------------------------------
// MRS Setting

	//Direct Command
	*(volatile unsigned long *)addr(0x306010 ) = 0x07000000;//NOP
	*(volatile unsigned long *)addr(0x306010 ) = 0x01000000;//precharge all
	*(volatile unsigned long *)addr(0x306010 ) = 0x00020000;
	*(volatile unsigned long *)addr(0x306010 ) = 0x00030000;
	*(volatile unsigned long *)addr(0x306010 ) = 0x00010000;
	*(volatile unsigned long *)addr(0x306010 ) = 0x00000100;
	*(volatile unsigned long *)addr(0x306010 ) = 0x01000000;//precharge all
	*(volatile unsigned long *)addr(0x306010 ) = 0x05000000;//AREF
	*(volatile unsigned long *)addr(0x306010 ) = 0x05000000;//AREF
	*(volatile unsigned long *)addr(0x306010 ) = 0x05000000;//AREF
	*(volatile unsigned long *)addr(0x306010 ) = 0x00000000;	// DLL reset release.
	*(volatile unsigned long *)addr(0x306010 ) = (DDR2_BURST_LEN|(DDR2_READ_BURST_TYPE<<3)|(DDR2_CL<<4)|((time2cycle(DDR2_tWR_ps, tCK)-1)<<9));
	i = 100; while(i--);
	*(volatile unsigned long *)addr(0x306010 ) = 0x00010380; // OCD Calibration default
	i = 100; while(i--);
	if (DDR2_CLK >= DDR2_ODT_ENABLE_MIN_FREQ)
		*(volatile unsigned long *)addr(0x306010 ) = 0x00010000 | (DDR2_ODT<<2) | (DDR2_DIC<<1);
	else
		*(volatile unsigned long *)addr(0x306010 ) = 0x00010000 | (DDR2_DIC<<1);

	if(DDR2_LOGICAL_CHIP_NUM == 2)
	{
		*(volatile unsigned long *)addr(0x306010 ) = 0x07000000 | Hw20;//NOP
		*(volatile unsigned long *)addr(0x306010 ) = 0x01000000 | Hw20;//precharge all
		*(volatile unsigned long *)addr(0x306010 ) = 0x00020000 | Hw20;
		*(volatile unsigned long *)addr(0x306010 ) = 0x00030000 | Hw20;
		*(volatile unsigned long *)addr(0x306010 ) = 0x00010000 | Hw20;
		*(volatile unsigned long *)addr(0x306010 ) = 0x00000100 | Hw20;
		*(volatile unsigned long *)addr(0x306010 ) = 0x01000000 | Hw20;//precharge all
		*(volatile unsigned long *)addr(0x306010 ) = 0x05000000 | Hw20;//AREF
		*(volatile unsigned long *)addr(0x306010 ) = 0x05000000 | Hw20;//AREF
		*(volatile unsigned long *)addr(0x306010 ) = 0x05000000 | Hw20;//AREF
		*(volatile unsigned long *)addr(0x306010 ) = 0x00000000 | Hw20;	// DLL reset release.
		*(volatile unsigned long *)addr(0x306010 ) = (DDR2_BURST_LEN|(DDR2_READ_BURST_TYPE<<3)|(DDR2_CL<<4)|((time2cycle(DDR2_tWR_ps, tCK)-1)<<9)) | Hw20;
		i = 100; while(i--);
		*(volatile unsigned long *)addr(0x306010 ) = 0x00010380 | Hw20; // OCD Calibration default
		i = 100; while(i--);
		if (DDR2_CLK >= DDR2_ODT_ENABLE_MIN_FREQ)
			*(volatile unsigned long *)addr(0x306010 ) = 0x00010000 | (DDR2_ODT<<2) | (DDR2_DIC<<1) | Hw20;
		else
			*(volatile unsigned long *)addr(0x306010 ) = 0x00010000 | (DDR2_DIC<<1) | Hw20;
	}

//--------------------------------------------------------------------------

	*(volatile unsigned long *)addr(0x303020 ) =  0x0007010A;//EMCCFG
	*(volatile unsigned long *)addr(0x306000 ) |= 0x20;

	if(DDR2_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x306048)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
	else//DDR2_BANK_NUM == 4
		while (((*(volatile unsigned long *)addr(0x306048)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED

	if(DDR2_LOGICAL_CHIP_NUM == 2){
		if(DDR2_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
		else//DDR2_BANK_NUM == 4
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED
	}

//--------------------------------------------------------------------------

#elif defined(CONFIG_DRAM_LPDDR2)

//--------------------------------------------------------------------------
// Controller setting

	//phy configuration
	*(volatile unsigned long *)addr(0x303024) = 0x200;//PHYCFG
	//*(volatile unsigned long *)addr(0x304400) = 0x00000509; //PHYMODE , LPDDR2 , diff DQS , under 333MHz

//--------------------------------------------------------------------------
// ZQ Calibration

	*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL;
	*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL | Hw1;//zq start
	while (((*(volatile unsigned long *)addr(0x306040)) & (0x10000)) != 0x10000);	// Wait until ZQ End
	*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL ;

//--------------------------------------------------------------------------
// Phy config

	if(LPDDR2_CLK < LPDDR2_DLL_LOW_FREQ_MODE_LIMIT)
		*(volatile unsigned long *)addr(0x306018) = (1<<3)|(1<<2); // ctrl_dfdqs, ctrl_half
	else
		*(volatile unsigned long *)addr(0x306018) = (1<<3); // ctrl_dfdqs

	*(volatile unsigned long *)addr(0x30601C) = (1<<23)|(8<<4)|(4<<0); // ctrl_cmosrcv, ctrl_ref, ctrl_shiftc
	*(volatile unsigned long *)addr(0x306020) = 0;

	if(LPDDR2_CLK < LPDDR2_DLL_LOW_FREQ_MODE_LIMIT){
		*(volatile unsigned long *)addr(0x306018) = (20<<16)|(20<<8)|(1<<3)|(1<<2)|(1<<1); // ctrl_inc, ctrl_start_point, ctrl_dfdqs, ctrl_half, ctrl_dll_on
		*(volatile unsigned long *)addr(0x306018) = (20<<16)|(20<<8)|(1<<3)|(1<<2)|(1<<1)|(1<<0); // ctrl_inc, ctrl_start_point, ctrl_dfdqs, ctrl_half, ctrl_dll_on, ctrl_start
	}else{
		*(volatile unsigned long *)addr(0x306018) = (20<<16)|(20<<8)|(1<<3)|(1<<1); // ctrl_inc, ctrl_start_point, ctrl_dfdqs, ctrl_dll_on
		*(volatile unsigned long *)addr(0x306018) = (20<<16)|(20<<8)|(1<<3)|(1<<1)|(1<<0); // ctrl_inc, ctrl_start_point, ctrl_dfdqs, ctrl_dll_on, ctrl_start
	}

//--------------------------------------------------------------------------
// Memory config

	//Enable Out of order scheduling
	*(volatile unsigned long *)addr(0x306000) =
		  ( 3 << 28) // phy_type, DDR3PHY
		| (0xff << 16) // timeout_cnt
		| ( 2 << 12) // rd_fetch
		| ( 0 << 11) // qos_fast_en
		| ( 0 << 10) // dq_swap
		| ( 0 <<  9) // chip1_empty
		| ( 0 <<  8) // chip0_empty
		| ( 0 <<  7) // drv_en
		| ( 1 <<  6) // ctc_trt_gap_en
		| ( 1 <<  5) // aref_en   // auto refresh should be disabled at this moment
		| ( 1 <<  4) // out_of
		| ( 1 <<  3) // div_pipe
		| ( 0 <<  1) // clk_ratio
		| ( 0 <<  0) // async
		;

	//MEMCTRL
	*(volatile unsigned long *)addr(0x306004 ) = (0x2 << 20) |
	                                             ((LPDDR2_LOGICAL_CHIP_NUM-1)<<16) |
	                                             ((LPDDR2_LOGICAL_DATA_BITS/16)<<12) |
	                                             (0x2 << 8) |
	                                             (0x1 << 6) |
	                                             (0x0 << 5) |
	                                             (0x0 << 4) |
	                                             (0x0 << 2) |
	                                             (0x0 << 1) |
	                                             (0x0);
	//MEMCHIP0
	*(volatile unsigned long *)addr(0x306008 ) = (0x40<<24) |
	                                             ((0xFF - (LPDDR2_TOTAL_MB_SIZE/(LPDDR2_LOGICAL_CHIP_NUM*0x10)-1))<<16) |
	                                             (0x1 << 12) |
	                                             ((LPDDR2_COLBITS - 7)<<8) |
	                                             ((LPDDR2_ROWBITS - 12)<<4) |
	                                             LPDDR2_BANK_BITS;
	//MEMCHIP1
	if(LPDDR2_LOGICAL_CHIP_NUM == 2)
	*(volatile unsigned long *)addr(0x30600C ) = ((0x40 + LPDDR2_TOTAL_MB_SIZE/(LPDDR2_LOGICAL_CHIP_NUM*0x10))<<24) |
		                                         ((0xFF - (LPDDR2_TOTAL_MB_SIZE/(LPDDR2_LOGICAL_CHIP_NUM*0x10)-1))<<16) |
		                                         (0x1 << 12) |
		                                         ((LPDDR2_COLBITS - 7)<<8) |
		                                         ((LPDDR2_ROWBITS - 12)<<4) |
		                                         LPDDR2_BANK_BITS;


//--------------------------------------------------------------------------

	*(volatile unsigned long *)addr(0x306014 ) = 0x0; //PRECONFIG
	//*(volatile unsigned long *)addr(0x306028 ) = 0xFFFF00FF; //PWRDNCONFIG


//--------------------------------------------------------------------------
// Timing parameter setting.

	//T_REFI
	*(volatile unsigned long *)addr(0x306030 ) = time2cycle(LPDDR2_tREFI_ps, tCK);

	//TROW
	*(volatile unsigned long *)addr(0x306034 ) = time2cycle(LPDDR2_tRAS_ps, tCK); //tRAS
	*(volatile unsigned long *)addr(0x306034 ) |= (time2cycle(LPDDR2_tRC_ps, tCK)<<6); //tRC
	*(volatile unsigned long *)addr(0x306034 ) |= (time2cycle(LPDDR2_tRCD_ps, tCK)<<12); //tRCD
	*(volatile unsigned long *)addr(0x306034 ) |= (time2cycle(LPDDR2_tRP_ps, tCK)<<16); //tRP
	*(volatile unsigned long *)addr(0x306034 ) |= (time2cycle(LPDDR2_tRRD_ps, tCK)<<20); //tRRD
	*(volatile unsigned long *)addr(0x306034 ) |= (time2cycle(LPDDR2_tRFC_ps, tCK)<<24); //tRFC

	//TDATA
	*(volatile unsigned long *)addr(0x306038 ) = LPDDR2_tRL_ck; //tRL
	*(volatile unsigned long *)addr(0x306038 ) |= (LPDDR2_tWL_ck<<8); //tWL
	*(volatile unsigned long *)addr(0x306038 ) |= (time2cycle(LPDDR2_tRTP_ps, tCK)<<20); //tRTP
	*(volatile unsigned long *)addr(0x306038 ) |= (time2cycle(LPDDR2_tWR_ps, tCK)<<24); //tWR
	*(volatile unsigned long *)addr(0x306038 ) |= (time2cycle(LPDDR2_tWTR_ps, tCK)<<28); //tWTR

	//TPOWER
	*(volatile unsigned long *)addr(0x30603C ) = LPDDR2_tMRD_ck; //tMRD
	*(volatile unsigned long *)addr(0x30603C ) |= (time2cycle(LPDDR2_tCKE_ps, tCK)<<4); //tCKE
	*(volatile unsigned long *)addr(0x30603C ) |= (time2cycle(LPDDR2_tXP_ps, tCK)<<8); //tXP
	*(volatile unsigned long *)addr(0x30603C ) |= (time2cycle(LPDDR2_tXSR_ps, tCK)<<16); //tXSR
	*(volatile unsigned long *)addr(0x30603C ) |= (time2cycle(LPDDR2_tFAW_ps, tCK)<<24); //tFAW


//--------------------------------------------------------------------------
// If QoS scheme is required, set the QosControl0~15 and QosConfig0~15 registers.
    //0xf0306060

//--------------------------------------------------------------------------
// Check whether PHY DLL is locked.

	while (((*(volatile unsigned long *)addr(0x306040)) & (0x4)) != 0x4);

	// Update DLL information
	*(volatile unsigned long *)addr(0x30601C) = (1<<23)|(8<<4)|(1<<3)|(4<<0); // ctrl_cmosrcv, ctrl_ref, fp_resync, ctrl_shiftc
	//*(volatile unsigned long *)addr(0x30601C) = (1<<23)|(8<<4)|(4<<0); // ctrl_cmosrcv, ctrl_ref, ctrl_shiftc

	i = 2000; while(i--); // Wait 100 ns

//--------------------------------------------------------------------------
// MRS Setting

	//Direct Command
	*(volatile unsigned long *)addr(0x306010 ) = 0x07000000;//NOP
	// Wait 200 us 
	i = 2000; while(i--);

	// MRW (Reset)
	*(volatile unsigned long *)addr(0x306010) = (0<<24)|(0<<20)| // cmd_type|chip_sel
		((0x3f&0xc0)>>6)|(((0x3f&0x38)>>3)<<16)|((0x3f&0x7)<<10)| // mr_addr
		((0x00)<<2); // mr_value
	i = 100; while(i--); // Wait 1us

	// MRW (Device Feature1)
	*(volatile unsigned long *)addr(0x306010) = (0<<24)|(0<<20)| // cmd_type|chip_sel
		((0x01&0xc0)>>6)|(((0x01&0x38)>>3)<<16)|((0x01&0x7)<<10)| // mr_addr
		((((time2cycle(LPDDR2_tWR_ps, tCK)-2)<<5)|0x2)<<2); // mr_value

	// MRW (Device Feature2)
	*(volatile unsigned long *)addr(0x306010) = (0<<24)|(0<<20)| // cmd_type|chip_sel
		((0x02&0xc0)>>6)|(((0x02&0x38)>>3)<<16)|((0x02&0x7)<<10)| // mr_addr
		((LPDDR2_tRL_ck-2)<<2); // mr_value

	// MRW (I/O Config-1)
	*(volatile unsigned long *)addr(0x306010) = (0<<24)|(0<<20)| // cmd_type|chip_sel
		((0x03&0xc0)>>6)|(((0x03&0x38)>>3)<<16)|((0x03&0x7)<<10)| // mr_addr
		((LPDDR2_DS)<<2); // mr_value

	// MRW (ZQ Calibration)
	//-- add ZQ calibration MRW here if LPDDR2 memmory supports ZQ calibration
	*(volatile unsigned long *)addr(0x306010) = (0<<24)|(0<<20)| // cmd_type|chip_sel
		((0x0A&0xc0)>>6)|(((0x0A&0x38)>>3)<<16)|((0x0A&0x7)<<10)| // mr_addr
		((0xFF)<<2); // mr_value

	// MRR (Device Auto-Initialization, DAI, polling)
	*(volatile unsigned long *)addr(0x306010) = (0<<24)|(0<<20)| // cmd_type|chip_sel
		((0x00&0xc0)>>6)|(((0x00&0x38)>>3)<<16)|((0x00&0x7)<<10)| // mr_addr
		((0x00)<<2); // mr_value

	while((((*(volatile unsigned long *)addr(0x306010))&0x3FC)>>2)&0x01);
	i = 100; while(i--); // Wait 1us

	if(LPDDR2_LOGICAL_CHIP_NUM == 2)
	{
		//Direct Command
		*(volatile unsigned long *)addr(0x306010 ) = 0x07000000|Hw20;//NOP
		// Wait 200 us 
		i = 2000; while(i--);

		// MRW (Reset)
		*(volatile unsigned long *)addr(0x306010) = (0<<24)|(1<<20)| // cmd_type|chip_sel
			((0x3f&0xc0)>>6)|(((0x3f&0x38)>>3)<<16)|((0x3f&0x7)<<10)| // mr_addr
			((0x00)<<2); // mr_value
		i = 100; while(i--); // Wait 1us

		// MRW (Device Feature1)
		*(volatile unsigned long *)addr(0x306010) = (0<<24)|(1<<20)| // cmd_type|chip_sel
			((0x01&0xc0)>>6)|(((0x01&0x38)>>3)<<16)|((0x01&0x7)<<10)| // mr_addr
			((((time2cycle(LPDDR2_tWR_ps, tCK)-2)<<5)|0x2)<<2); // mr_value

		// MRW (Device Feature2)
		*(volatile unsigned long *)addr(0x306010) = (0<<24)|(1<<20)| // cmd_type|chip_sel
			((0x02&0xc0)>>6)|(((0x02&0x38)>>3)<<16)|((0x02&0x7)<<10)| // mr_addr
			((LPDDR2_tRL_ck-2)<<2); // mr_value

		// MRW (I/O Config-1)
		*(volatile unsigned long *)addr(0x306010) = (0<<24)|(1<<20)| // cmd_type|chip_sel
			((0x03&0xc0)>>6)|(((0x03&0x38)>>3)<<16)|((0x03&0x7)<<10)| // mr_addr
			((LPDDR2_DS)<<2); // mr_value

		// MRW (ZQ Calibration)
		//-- add ZQ calibration MRW here if LPDDR2 memmory supports ZQ calibration
		*(volatile unsigned long *)addr(0x306010) = (0<<24)|(1<<20)| // cmd_type|chip_sel
			((0x0A&0xc0)>>6)|(((0x0A&0x38)>>3)<<16)|((0x0A&0x7)<<10)| // mr_addr
			((0xFF)<<2); // mr_value

		// MRR (Device Auto-Initialization, DAI, polling)
		*(volatile unsigned long *)addr(0x306010) = (0<<24)|(1<<20)| // cmd_type|chip_sel
			((0x00&0xc0)>>6)|(((0x00&0x38)>>3)<<16)|((0x00&0x7)<<10)| // mr_addr
			((0x00)<<2); // mr_value

		while((((*(volatile unsigned long *)addr(0x306010))&0x3FC)>>2)&0x01);
		i = 100; while(i--); // Wait 1us
	}

//--------------------------------------------------------------------------

	*(volatile unsigned long *)addr(0x303020 ) =  0x0007010A;//EMCCFG
	*(volatile unsigned long *)addr(0x306000 ) |= 0x20; // Auto Refresh Enable 

	if(LPDDR2_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x306048)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
	else//LPDDR2_BANK_NUM == 4
		while (((*(volatile unsigned long *)addr(0x306048)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED

	if(LPDDR2_LOGICAL_CHIP_NUM == 2){
		if(LPDDR2_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
		else//LPDDR2_BANK_NUM == 4
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED
	}

//--------------------------------------------------------------------------

#endif
}
#endif

#if defined(TCC_PM_SLEEP_MODE)
/*===========================================================================
FUNCTION
===========================================================================*/
static void sdram_init_sleep(void)
{
	volatile unsigned int i;
#if defined(CONFIG_DRAM_DDR3)
	register unsigned int mr;

	#define DDR3_CLK      200
	//#define nCK (1000000/(DDR3_MAX_SPEED/2))
	#define tCK (1000000/DDR3_CLK)
#elif defined(CONFIG_DRAM_DDR2)
	register unsigned int tmp;

#if defined(CONFIG_MACH_TCC8800ST)
	#define DDR2_CLK      400
#else
	#define DDR2_CLK      180
#endif /* CONFIG_MACH_TCC8800ST */

	//#define nCK (1000000/(DDR2_MAX_SPEED/2))
	#define tCK (1000000/DDR2_CLK)
#elif defined(CONFIG_DRAM_LPDDR2)
	register unsigned int tmp;

	#define LPDDR2_CLK    180
	//#define nCK (1000000/(LPDDR2_MAX_SPEED/2))
	#define tCK (1000000/LPDDR2_CLK)
#endif


//--------------------------------------------------------------------------
// Change to config mode

#if defined(CONFIG_DRAM_DDR3)
	*(volatile unsigned long *)addr(0x303020 ) = 0x0003010b ;//EMCCFG

	if(DDR3_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x309208)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
	else//DDR3_BANK_NUM is 4
		while (((*(volatile unsigned long *)addr(0x309208)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED
#if (0)
	if(DDR3_LOGICAL_CHIP_NUM == 2){
		if(DDR3_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr(0x30920C)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
		else//DDR3_BANK_NUM is 4
			while (((*(volatile unsigned long *)addr(0x30920C)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED
	}
#endif
#elif defined(CONFIG_DRAM_DDR2)
	*(volatile unsigned long *)addr(0x303020 ) =  0x0005010A;

	if(DDR2_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x306048)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
	else//DDR2_BANK_NUM == 4
		while (((*(volatile unsigned long *)addr(0x306048)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED

#if (0)
	if(DDR2_LOGICAL_CHIP_NUM == 2){
		if(DDR2_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
		else//DDR2_BANK_NUM == 4
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED
	}
#endif
#elif defined(CONFIG_DRAM_LPDDR2)
	*(volatile unsigned long *)addr(0x303020 ) =  0x0005010A;

	if(LPDDR2_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x306048)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
	else//LPDDR2_BANK_NUM == 4
		while (((*(volatile unsigned long *)addr(0x306048)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED

#if (0)
	if(LPDDR2_LOGICAL_CHIP_NUM == 2){
		if(LPDDR2_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
		else//LPDDR2_BANK_NUM == 4
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED
	}
#endif
#endif

//--------------------------------------------------------------------------
// Clock setting..

	//clock setting start
	//Set CLKDIVC0,CLKDIVC1. But it is diabled.
	*(volatile unsigned long *)addr(0x400030) = 0x01010101;
#ifndef CONFIG_MACH_TCC8800ST
	*(volatile unsigned long *)addr(0x400034) = 0x01010101;
#endif
	*(volatile unsigned long *)addr(0x400038) = 0x01010101;

	//cpu bus - DirectXIN,
	*(volatile unsigned long *)addr(0x400000) = 0x002ffff4;
	//mem bus - DirectXIN/2
	*(volatile unsigned long *)addr(0x400008) = 0x002f1f14;
	//io bus - DirectXIN/2
	*(volatile unsigned long *)addr(0x400010) = 0x002f1f14;
	//smu bus
	*(volatile unsigned long *)addr(0x40001C) = 0x002f1f14;

	//PLL5 - 600MHz
	*(volatile unsigned long *)addr(0x400054) = 0x01012C03;
	*(volatile unsigned long *)addr(0x400054) = 0x81012C03;

	//MEM PLL
	//*(volatile unsigned long *)addr(0x400050) = 0x02012203; // 290MHz
	//*(volatile unsigned long *)addr(0x400050) = 0x82012203;
#if defined(CONFIG_MACH_TCC8800ST)
	*(volatile unsigned long *)addr(0x400050) = 0x01019003; // 800MHz
	*(volatile unsigned long *)addr(0x400050) = 0xC1019003;
#else
	*(volatile unsigned long *)addr(0x400050) = 0x02014003; // 320MHz
	*(volatile unsigned long *)addr(0x400050) = 0x82014003;
#endif /* CONFIG_MACH_TCC8800ST */

	for (i = 3000; i > 0; i --);	// Wait

	//cpu bus - PLL5, 600 = 600MHz
	*(volatile unsigned long *)addr(0x400000) = 0x002ffff9;
	//mem bus - PLL4, 290/2 = 145MHz
	*(volatile unsigned long *)addr(0x400008) = 0x00200018;
	//io bus - PLL4, 290/2 = 145MHz
	*(volatile unsigned long *)addr(0x400010) = 0x00200018;
	//smu bus
	*(volatile unsigned long *)addr(0x40001C) = 0x00200039;

	for (i = 3000; i > 0; i --);	// Wait

#if defined(CONFIG_DRAM_DDR3)
//--------------------------------------------------------------------------
// Controller setting

	*(volatile unsigned long *)addr(0x303024 ) = 0x00000300 ;//PHYCFG
	*(volatile unsigned long *)addr(0x304400 ) = 0x0000000A ;//PHYMODE

	//Bruce, 101029, modify according to soc guide
	//*(volatile unsigned long *)addr(0x309400 ) = 0x00101708  ;//PhyControl0
	*(volatile unsigned long *)addr(0x309400 ) = 0x0110140A  ;//PhyControl0

	*(volatile unsigned long *)addr(0x309404 ) = 0x00000086  ;//PhyControl1
	*(volatile unsigned long *)addr(0x309408 ) = 0x00000000  ;//PhyControl2
	*(volatile unsigned long *)addr(0x30940c ) = 0x00000000  ;//PhyControl3
	*(volatile unsigned long *)addr(0x309410 ) = 0x201c7004  ;//PhyControl4

	//Bruce, 101029, modify according to soc guide
	//*(volatile unsigned long *)addr(0x309400 ) = 0x0110170B  ;//PhyControl0
	//while (((*(volatile unsigned long *)addr(0x309418)) & (0x04)) != 4);	// dll locked
	if(DDR3_CLK >= 333)
		*(volatile unsigned long *)addr(0x309400 ) = 0x0110140B  ;//PhyControl0
	else
		*(volatile unsigned long *)addr(0x309400 ) = 0x0110140F  ;//PhyControl0
	while (((*(volatile unsigned long *)addr(0x309418)) & (0x04)) != 4);	// dll locked

	*(volatile unsigned long *)addr(0x309404 ) = 0x0000008e  ;//PhyControl1
	*(volatile unsigned long *)addr(0x309404 ) = 0x00000086  ;//PhyControl1

	//Bruce, 101029, modify according to soc guide
	//*(volatile unsigned long *)addr(0x309414 ) = 0x00030003  ;//PhyControl5
	*(volatile unsigned long *)addr(0x309414 ) = 0x00020003  ;//PhyControl5

	*(volatile unsigned long *)addr(0x309414 ) = 0x0000000b | (MEMCTRL_DDS<<15) | (MEMCTRL_TERM<<11);//PhyControl5

	while (((*(volatile unsigned long *)addr(0x309420)) & (0x01)) != 1);	// zq end

//--------------------------------------------------------------------------
// Memory config

	*(volatile unsigned long *)addr(0x309004 ) = 0x0000018A ; //MemControl

	if(DDR3_BURST_LEN == BL_8) // BL
		BITCSET(*(volatile unsigned long *)addr(0x309004), 0x7<<7, 0x3<<7);
	else
		BITCSET(*(volatile unsigned long *)addr(0x309004), 0x7<<7, 0x2<<7);

	if(DDR3_LOGICAL_CHIP_NUM == 1) // num_chip
		BITCSET(*(volatile unsigned long *)addr(0x309004), 0x3<<5, 0x0);
	else
		BITCSET(*(volatile unsigned long *)addr(0x309004), 0x3<<5, 0x1<<5);

    // Chip 0 Configuration ------------------------------------------------
    {
		*(volatile unsigned long *)addr(0x309008) = 0x40F01313; //MemConfig0 //address mapping method - interleaved
		BITCSET(*(volatile unsigned long *)addr(0x309008), 0xFF<<16, (0xFF - ((DDR3_TOTAL_MB_SIZE)/(DDR3_LOGICAL_CHIP_NUM*0x10)-1))<<16);//set chip mask
		BITCSET(*(volatile unsigned long *)addr(0x309008), 0xF<<8, (DDR3_COLBITS - 7)<<8);//set column bits
		BITCSET(*(volatile unsigned long *)addr(0x309008), 0xF<<4, (DDR3_ROWBITS - 12)<<4);//set row bits
		if(DDR3_BANK_NUM == 8)//8 banks
			BITCSET(*(volatile unsigned long *)addr(0x309008), 0xF, 0x3);
		else // 4 banks
			BITCSET(*(volatile unsigned long *)addr(0x309008), 0xF, 0x2);
    }

    // Chip 1 Configuration ------------------------------------------------
	if(DDR3_LOGICAL_CHIP_NUM == 2)
	{
		*(volatile unsigned long *)addr(0x30900C) = 0x50E01313; //MemConfig1 //address mapping method - interleaved
		BITCSET(*(volatile unsigned long *)addr(0x30900C), 0xFF<<24, (0x40 + (DDR3_TOTAL_MB_SIZE)/(DDR3_LOGICAL_CHIP_NUM*0x10))<<24);//set chip base
		BITCSET(*(volatile unsigned long *)addr(0x30900C), 0xFF<<16, (0xFF - ((DDR3_TOTAL_MB_SIZE)/(DDR3_LOGICAL_CHIP_NUM*0x10)-1))<<16);//set chip mask
		BITCSET(*(volatile unsigned long *)addr(0x30900C), 0xF<<8, (DDR3_COLBITS - 7)<<8);//set column bits
		BITCSET(*(volatile unsigned long *)addr(0x30900C), 0xF<<4, (DDR3_ROWBITS - 12)<<4);//set row bits
		if(DDR3_BANK_NUM == 8)// 8 banks
			BITCSET(*(volatile unsigned long *)addr(0x30900C), 0xF, 0x3);
		else // 4 banks
			BITCSET(*(volatile unsigned long *)addr(0x30900C), 0xF, 0x2);
	}

//--------------------------------------------------------------------------

	*(volatile unsigned long *)addr(0x309000) = 0x40FF3010  ;//ConControl
	*(volatile unsigned long *)addr(0x309014) = 0x01000000  ;//PrechConfig

//--------------------------------------------------------------------------
// Timing parameter setting.

#if (1)
	*(volatile unsigned long *)addr(0x309100) = time2cycle(DDR3_tREFI_ps, tCK); //T_REFI
	*(volatile unsigned long *)addr(0x309104) = time2cycle(DDR3_tRFC_ps, tCK); //T_RFC
	*(volatile unsigned long *)addr(0x309108) = time2cycle(DDR3_tRRD_ps, tCK); //T_RRD
	if(*(volatile unsigned long *)addr(0x309108)<DDR3_tRRD_ck)
		*(volatile unsigned long *)addr(0x309108) = DDR3_tRRD_ck;
	*(volatile unsigned long *)addr(0x30910c) = DDR3_CL; //T_RP
	*(volatile unsigned long *)addr(0x309110) = DDR3_CL; //T_RCD
	*(volatile unsigned long *)addr(0x309114) = time2cycle(DDR3_tRC_ps, tCK); //T_RC
	*(volatile unsigned long *)addr(0x309118) = time2cycle(DDR3_tRAS_ps, tCK); //T_RAS
	*(volatile unsigned long *)addr(0x30911c) = time2cycle(DDR3_tWTR_ps, tCK); //T_WTR
	if(*(volatile unsigned long *)addr(0x30911c)<DDR3_tWTR_ck)
		*(volatile unsigned long *)addr(0x30911c) = DDR3_tWTR_ck;
	*(volatile unsigned long *)addr(0x309120) = time2cycle(DDR3_tWR_ps, tCK); //T_WR
	*(volatile unsigned long *)addr(0x309124) = time2cycle(DDR3_tRTP_ps, tCK); //T_RTP
	if(*(volatile unsigned long *)addr(0x309124)<DDR3_tRTP_ck)
		*(volatile unsigned long *)addr(0x309124) = DDR3_tRTP_ck;
	*(volatile unsigned long *)addr(0x309128) = DDR3_CL; //CL

	if(tCK >= 2500 /* 2.5 ns */)
		*(volatile unsigned long *)addr(0x30912c) = 5;
	else if(tCK >= 1875 /* 1.875 ns */)
		*(volatile unsigned long *)addr(0x30912c) = 6;
	else if(tCK >= 1500 /* 1.5 ns */)
		*(volatile unsigned long *)addr(0x30912c) = 7;
	else if(tCK >= 1250 /* 1.25 ns */)
		*(volatile unsigned long *)addr(0x30912c) = 8;
	else if(tCK >= 1070 /* 1.07 ns */)
		*(volatile unsigned long *)addr(0x30912c) = 9;
	else if(tCK >= 935 /* 0.935 ns */)
		*(volatile unsigned long *)addr(0x30912c) = 10;
	else if(tCK >= 833 /* 0.833 ns */)
		*(volatile unsigned long *)addr(0x30912c) = 11;
	else if(tCK >= 750 /* 0.75 ns */)
		*(volatile unsigned long *)addr(0x30912c) = 12;

	*(volatile unsigned long *)addr(0x309130) = DDR3_CL; //RL = AL+CL

	if(DDR3_AL == AL_CL_MINUS_ONE){ //nAL = nCL - 1;
		*(volatile unsigned long *)addr(0x30912c) += (DDR3_CL-1); //WL = AL+CWL
		*(volatile unsigned long *)addr(0x309130) += (DDR3_CL-1); //RL = AL+CL
	}else if(DDR3_AL == AL_CL_MINUS_TWO){ //	nAL = nCL - 2;
		*(volatile unsigned long *)addr(0x30912c) += (DDR3_CL-2); //WL = AL+CWL
		*(volatile unsigned long *)addr(0x309130) += (DDR3_CL-2); //RL = AL+CL
	}

	*(volatile unsigned long *)addr(0x309134) = time2cycle(DDR3_tFAW_ps, tCK); //T_FAW
	*(volatile unsigned long *)addr(0x309138) = time2cycle(DDR3_tXS_ps, tCK); //T_XSR
	*(volatile unsigned long *)addr(0x30913c) = time2cycle(DDR3_tXP_ps, tCK); //T_XP
	if(*(volatile unsigned long *)addr(0x30913c)<DDR3_tXP_ck)
		*(volatile unsigned long *)addr(0x30913c) = DDR3_tXP_ck;
	*(volatile unsigned long *)addr(0x309140) = time2cycle(DDR3_tCKE_ps, tCK); //T_CKE
	if(*(volatile unsigned long *)addr(0x309140)<DDR3_tCKE_ck)
		*(volatile unsigned long *)addr(0x309140) = DDR3_tCKE_ck;
	*(volatile unsigned long *)addr(0x309144) = DDR3_tMRD_ck; //T_MRD
#else
	*(volatile unsigned long *)addr(0x309100) = 0x618;
	*(volatile unsigned long *)addr(0x309104) = 0x16;
	*(volatile unsigned long *)addr(0x309108) = 0x4;
	*(volatile unsigned long *)addr(0x30910c) = 0x6;
	*(volatile unsigned long *)addr(0x309110) = 0x6;
	*(volatile unsigned long *)addr(0x309114) = 0xA;
	*(volatile unsigned long *)addr(0x309118) = 0x8;
	*(volatile unsigned long *)addr(0x30911c) = 0x4;
	*(volatile unsigned long *)addr(0x309120) = 0x3;
	*(volatile unsigned long *)addr(0x309124) = 0x4;
	*(volatile unsigned long *)addr(0x309128) = 0x6;
	*(volatile unsigned long *)addr(0x30912c) = 0x5;
	*(volatile unsigned long *)addr(0x309130) = 0x6;
	*(volatile unsigned long *)addr(0x309134) = 0x9;
	*(volatile unsigned long *)addr(0x309138) = 0x18;
	*(volatile unsigned long *)addr(0x30913c) = 0x3;
	*(volatile unsigned long *)addr(0x309140) = 0x3;
	*(volatile unsigned long *)addr(0x309144) = 0x4;
#endif

//--------------------------------------------------------------------------
// MRS Setting

	*(volatile unsigned long *)addr(0x309010) = 0x08000000 ;//DirectCmd - XSR

	//after 500 us
	*(volatile unsigned long *)addr(0x309010) = 0x07000000;//DirectCmd - NOP

	//*(volatile unsigned long *)addr(0x309010) = 0x00020000;//DirectCmd - MRS : MR2
	{
		mr = 0x00020000;

		if(DDR3_CLK*2 <= DDR3_800)
			mr |= 0;
		else if(DDR3_CLK*2 <= DDR3_1066)
			mr |= (1<<3);
		else if(DDR3_CLK*2 <= DDR3_1333)
			mr |= (2<<3);
		else if(DDR3_CLK*2 <= DDR3_1600)
			mr |= (3<<3);

		*(volatile unsigned long *)addr(0x309010) = mr;
	}

	*(volatile unsigned long *)addr(0x309010) = 0x00030000;//DirectCmd - MRS : MR3
	*(volatile unsigned long *)addr(0x309010) = 0x00010000 | (DDR3_AL<<3) | (DDR3_ODT<<2) | (DDR3_DIC<<1);//DirectCmd - MRS : MR1 : DLL(enable)

	//*(volatile unsigned long *)addr(0x309010) = 0x00001220;//DirectCmd - MRS : MR0 : DLLPRE(off), WR(), DLL Reset(Yes), MODE(0), CL(), BL(8)
	{
		mr = DDR3_BURST_LEN | (DDR3_READ_BURST_TYPE<<3) | (FAST_EXIT<<12);

		if(DDR3_CL < 5)
			mr |= ((5-4)<<4);
		else if(DDR3_CL > 11)
			mr |= ((11-4)<<4);
		else
			mr |= ((DDR3_CL-4)<<4);

		if(tCK >= 2500 /* 2.5 ns */)
			mr |= (WR_5<<9);
		else if(tCK >= 1875 /* 1.875 ns */)
			mr |= (WR_6<<9);
		else if(tCK >= 1500 /* 1.5 ns */)
			mr |= (WR_7<<9);
		else if(tCK >= 1250 /* 1.25 ns */)
			mr |= (WR_8<<9);
		else if(tCK >= 935 /* 0.935 ns */)
			mr |= (WR_10<<9);
		else
			mr |= (WR_12<<9);

		*(volatile unsigned long *)addr(0x309010) = mr;
	}

	*(volatile unsigned long *)addr(0x309010) = 0x0a000400 ;//DirectCmd - ZQCL

	if(DDR3_LOGICAL_CHIP_NUM == 2){
		*(volatile unsigned long *)addr(0x309010) = 0x08000000 | Hw20;//DirectCmd - XSR
		
		//after 500 us
		*(volatile unsigned long *)addr(0x309010) = 0x07000000 | Hw20;//DirectCmd - NOP

		//*(volatile unsigned long *)addr(0x309010) = 0x00020000;//DirectCmd - MRS : MR2
		{
			mr = 0x00020000;

			if(DDR3_CLK*2 <= DDR3_800)
				mr |= 0;
			else if(DDR3_CLK*2 <= DDR3_1066)
				mr |= (1<<3);
			else if(DDR3_CLK*2 <= DDR3_1333)
				mr |= (2<<3);
			else if(DDR3_CLK*2 <= DDR3_1600)
				mr |= (3<<3);

			*(volatile unsigned long *)addr(0x309010) = mr | Hw20;
		}

		*(volatile unsigned long *)addr(0x309010) = 0x00030000 | Hw20;//DirectCmd - MRS : MR3
		*(volatile unsigned long *)addr(0x309010) = 0x00010000 | (DDR3_AL<<3) | (DDR3_ODT<<2) | (DDR3_DIC<<1) | Hw20;//DirectCmd - MRS : MR1 : DLL(enable)

		//*(volatile unsigned long *)addr(0x309010) = 0x00001420 | Hw20;//DirectCmd - MRS : MR0 : DLLPRE(off), WR(), DLL Reset(Yes), MODE(0), CL(), BL(8)
		{
			mr = DDR3_BURST_LEN | (DDR3_READ_BURST_TYPE<<3) | (FAST_EXIT<<12);

			if(DDR3_CL < 5)
				mr |= ((5-4)<<4);
			else if(DDR3_CL > 11)
				mr |= ((11-4)<<4);
			else
				mr |= ((DDR3_CL-4)<<4);

			if(tCK >= 2500 /* 2.5 ns */)
				mr |= (WR_5<<9);
			else if(tCK >= 1875 /* 1.875 ns */)
				mr |= (WR_6<<9);
			else if(tCK >= 1500 /* 1.5 ns */)
				mr |= (WR_7<<9);
			else if(tCK >= 1250 /* 1.25 ns */)
				mr |= (WR_8<<9);
			else if(tCK >= 935 /* 0.935 ns */)
				mr |= (WR_10<<9);
			else
				mr |= (WR_12<<9);

			*(volatile unsigned long *)addr(0x309010) = mr | Hw20;
		}

		*(volatile unsigned long *)addr(0x309010) = 0x0a000400 | Hw20;//DirectCmd - ZQCL
	}

//--------------------------------------------------------------------------

	*(volatile unsigned long *)addr(0x309000) = 0x60ff3030  ;//ConControl
	*(volatile unsigned long *)addr(0x303020) = 0x0007010b ;//EMCCFG

	if(DDR3_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x309208)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
	else //DDR3_BANK_NUM is 4
		while (((*(volatile unsigned long *)addr(0x309208)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED

	if(DDR3_LOGICAL_CHIP_NUM == 2){
		if(DDR3_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr(0x30920C)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
		else //DDR3_BANK_NUM is 4
			while (((*(volatile unsigned long *)addr(0x30920C)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED
	}

//--------------------------------------------------------------------------
#elif defined(CONFIG_DRAM_DDR2)

//--------------------------------------------------------------------------
// Controller setting

	//phy configuration
	*(volatile unsigned long *)addr(0x303024 ) = 0x200;//PHYCFG

	//PhyZQControl
	if (DDR2_CLK >= DDR2_ODT_ENABLE_MIN_FREQ) {
		*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL ;
		*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL | Hw1 ;//zq start
	} else {
		*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL | Hw0;
		*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL | Hw1 | Hw0 ;//zq start
	}
	while (((*(volatile unsigned long *)addr(0x306040)) & (0x10000)) != 0x10000);	// Wait until ZQ End

	if (DDR2_CLK >= DDR2_ODT_ENABLE_MIN_FREQ) {
		*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL ;
	} else {
		*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL | Hw0;
	}

	*(volatile unsigned long *)addr(0x306018 ) = 0x0010100A; //PHY Control0
	*(volatile unsigned long *)addr(0x30601C ) = 0xE0000086; //PHY Control1 // modify by crony : [31:30] : ODT Enable for Write and Read
	*(volatile unsigned long *)addr(0x306020 ) = 0x00000000; //PHY Control2
	*(volatile unsigned long *)addr(0x306024 ) = 0x00000000; //PHY Control3
	*(volatile unsigned long *)addr(0x306018 ) = 0x0010100B; //PHY Control0

	while (((*(volatile unsigned long *)addr(0x306040)) & (0x7)) != 0x7);// Wait until FLOCK == 1

	//PHY Control1
	*(volatile unsigned long *)addr(0x30601C) = 0xE000008E; //resync = 1
	*(volatile unsigned long *)addr(0x30601C) = 0xE0000086; //resync = 0

//--------------------------------------------------------------------------
// Memory config

	//Enable Out of order scheduling
	*(volatile unsigned long *)addr(0x306000 ) = 0x30FF2018;

	//MEMCTRL
	*(volatile unsigned long *)addr(0x306004 ) = (0x2 << 20) |
	                                             ((DDR2_LOGICAL_CHIP_NUM-1)<<16) |
	                                             ((DDR2_LOGICAL_DATA_BITS/16)<<12) |
	                                             (0x4 << 8) |
	                                             (0x0 << 6) |
	                                             (0x0 << 5) |
	                                             (0x0 << 4) |
	                                             (0x0 << 2) |
	                                             (0x0 << 1) |
	                                             (0x0);

	//MEMCHIP0
	*(volatile unsigned long *)addr(0x306008 ) = (0x40<<24) |
	                                             ((0xFF - (DDR2_TOTAL_MB_SIZE/(DDR2_LOGICAL_CHIP_NUM*0x10)-1))<<16) |
	                                             (0x1 << 12) |
	                                             ((DDR2_COLBITS - 7)<<8) |
	                                             ((DDR2_ROWBITS - 12)<<4) |
	                                             DDR2_BANK_BITS;

	//MEMCHIP1
	if(DDR2_LOGICAL_CHIP_NUM == 2)
	*(volatile unsigned long *)addr(0x30600C ) = ((0x40 + DDR2_TOTAL_MB_SIZE/(DDR2_LOGICAL_CHIP_NUM*0x10))<<24) |
		                                         ((0xFF - (DDR2_TOTAL_MB_SIZE/(DDR2_LOGICAL_CHIP_NUM*0x10)-1))<<16) |
		                                         (0x1 << 12) |
		                                         ((DDR2_COLBITS - 7)<<8) |
		                                         ((DDR2_ROWBITS - 12)<<4) |
		                                         DDR2_BANK_BITS;

//--------------------------------------------------------------------------

	*(volatile unsigned long *)addr(0x306014 ) = 0x0; //PRECONFIG
	*(volatile unsigned long *)addr(0x306028 ) = 0xFFFF00FF; //PRECONFIG

//--------------------------------------------------------------------------
// Timing parameter setting.

	//T_REFI
	*(volatile unsigned long *)addr(0x306030 ) = time2cycle(DDR2_tREFI_ps, tCK);

	//TROW
	*(volatile unsigned long *)addr(0x306034 ) = time2cycle(DDR2_tRAS_ps, tCK); //tRAS
	*(volatile unsigned long *)addr(0x306034 ) |= (time2cycle(DDR2_tRC_ps, tCK)<<6); //tRC
	*(volatile unsigned long *)addr(0x306034 ) |= (DDR2_CL<<12); //tRCD
	*(volatile unsigned long *)addr(0x306034 ) |= (DDR2_CL<<16); //tRP
	*(volatile unsigned long *)addr(0x306034 ) |= (time2cycle(DDR2_tRRD_ps, tCK)<<20); //tRRD
	*(volatile unsigned long *)addr(0x306034 ) |= (time2cycle(DDR2_tRFC_ps, tCK)<<24); //tRFC

	//TDATA
	*(volatile unsigned long *)addr(0x306038 ) = DDR2_CL; //tRL
	*(volatile unsigned long *)addr(0x306038 ) |= ((DDR2_CL-1)<<8); //tWL
	*(volatile unsigned long *)addr(0x306038 ) |= (DDR2_CL<<16); //tCL
	tmp = time2cycle(DDR2_tRTP_ps, tCK);
	if(tmp<DDR2_tRTP_ck) tmp=DDR2_tRTP_ck;
	*(volatile unsigned long *)addr(0x306038 ) |= (tmp<<20); //tRTP
	*(volatile unsigned long *)addr(0x306038 ) |= (time2cycle(DDR2_tWR_ps, tCK)<<24); //tWR
	tmp = time2cycle(DDR2_tWTR_ps, tCK);
	if(tmp<DDR2_tWTR_ck) tmp=DDR2_tWTR_ck;
	*(volatile unsigned long *)addr(0x306038 ) |= (tmp<<28); //tWTR

	//TPOWER
	*(volatile unsigned long *)addr(0x30603C ) = DDR2_tMRD_ck; //tMRD
	*(volatile unsigned long *)addr(0x30603C ) |= (DDR2_tCKE_ck<<4); //tCKE
	*(volatile unsigned long *)addr(0x30603C ) |= (DDR2_tXP_ck<<8); //tXP
	*(volatile unsigned long *)addr(0x30603C ) |= (DDR2_tXSR_ck<<16); //tXSR
	*(volatile unsigned long *)addr(0x30603C ) |= (time2cycle(DDR2_tFAW_ps, tCK)<<24); //tFAW

//--------------------------------------------------------------------------
// MRS Setting

	//Direct Command
	*(volatile unsigned long *)addr(0x306010 ) = 0x07000000;//NOP
	*(volatile unsigned long *)addr(0x306010 ) = 0x01000000;//precharge all
	*(volatile unsigned long *)addr(0x306010 ) = 0x00020000;
	*(volatile unsigned long *)addr(0x306010 ) = 0x00030000;
	*(volatile unsigned long *)addr(0x306010 ) = 0x00010000;
	*(volatile unsigned long *)addr(0x306010 ) = 0x00000100;
	*(volatile unsigned long *)addr(0x306010 ) = 0x01000000;//precharge all
	*(volatile unsigned long *)addr(0x306010 ) = 0x05000000;//AREF
	*(volatile unsigned long *)addr(0x306010 ) = 0x05000000;//AREF
	*(volatile unsigned long *)addr(0x306010 ) = 0x05000000;//AREF
	*(volatile unsigned long *)addr(0x306010 ) = 0x00000000;	// DLL reset release.
	*(volatile unsigned long *)addr(0x306010 ) = (DDR2_BURST_LEN|(DDR2_READ_BURST_TYPE<<3)|(DDR2_CL<<4)|((time2cycle(DDR2_tWR_ps, tCK)-1)<<9));
	i = 100; while(i--);
	*(volatile unsigned long *)addr(0x306010 ) = 0x00010380; // OCD Calibration default
	i = 100; while(i--);
	if (DDR2_CLK >= DDR2_ODT_ENABLE_MIN_FREQ)
		*(volatile unsigned long *)addr(0x306010 ) = 0x00010000 | (DDR2_ODT<<2) | (DDR2_DIC<<1);
	else
		*(volatile unsigned long *)addr(0x306010 ) = 0x00010000 | (DDR2_DIC<<1);

	if(DDR2_LOGICAL_CHIP_NUM == 2)
	{
		*(volatile unsigned long *)addr(0x306010 ) = 0x07000000 | Hw20;//NOP
		*(volatile unsigned long *)addr(0x306010 ) = 0x01000000 | Hw20;//precharge all
		*(volatile unsigned long *)addr(0x306010 ) = 0x00020000 | Hw20;
		*(volatile unsigned long *)addr(0x306010 ) = 0x00030000 | Hw20;
		*(volatile unsigned long *)addr(0x306010 ) = 0x00010000 | Hw20;
		*(volatile unsigned long *)addr(0x306010 ) = 0x00000100 | Hw20;
		*(volatile unsigned long *)addr(0x306010 ) = 0x01000000 | Hw20;//precharge all
		*(volatile unsigned long *)addr(0x306010 ) = 0x05000000 | Hw20;//AREF
		*(volatile unsigned long *)addr(0x306010 ) = 0x05000000 | Hw20;//AREF
		*(volatile unsigned long *)addr(0x306010 ) = 0x05000000 | Hw20;//AREF
		*(volatile unsigned long *)addr(0x306010 ) = 0x00000000 | Hw20;	// DLL reset release.
		*(volatile unsigned long *)addr(0x306010 ) = (DDR2_BURST_LEN|(DDR2_READ_BURST_TYPE<<3)|(DDR2_CL<<4)|((time2cycle(DDR2_tWR_ps, tCK)-1)<<9)) | Hw20;
		i = 100; while(i--);
		*(volatile unsigned long *)addr(0x306010 ) = 0x00010380 | Hw20; // OCD Calibration default
		i = 100; while(i--);
		if (DDR2_CLK >= DDR2_ODT_ENABLE_MIN_FREQ)
			*(volatile unsigned long *)addr(0x306010 ) = 0x00010000 | (DDR2_ODT<<2) | (DDR2_DIC<<1) | Hw20;
		else
			*(volatile unsigned long *)addr(0x306010 ) = 0x00010000 | (DDR2_DIC<<1) | Hw20;
	}

//--------------------------------------------------------------------------

	*(volatile unsigned long *)addr(0x303020 ) =  0x0007010A;//EMCCFG
	*(volatile unsigned long *)addr(0x306000 ) |= 0x20;

	if(DDR2_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x306048)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
	else//DDR2_BANK_NUM == 4
		while (((*(volatile unsigned long *)addr(0x306048)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED

	if(DDR2_LOGICAL_CHIP_NUM == 2){
		if(DDR2_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
		else//DDR2_BANK_NUM == 4
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED
	}

//--------------------------------------------------------------------------

#elif defined(CONFIG_DRAM_LPDDR2)

//--------------------------------------------------------------------------
// Controller setting

	//phy configuration
	*(volatile unsigned long *)addr(0x303024) = 0x200;//PHYCFG
	//*(volatile unsigned long *)addr(0x304400) = 0x00000509; //PHYMODE , LPDDR2 , diff DQS , under 333MHz

//--------------------------------------------------------------------------
// ZQ Calibration

	*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL;
	*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL | Hw1;//zq start
	while (((*(volatile unsigned long *)addr(0x306040)) & (0x10000)) != 0x10000);	// Wait until ZQ End
	*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL ;

//--------------------------------------------------------------------------
// Phy config

	if(LPDDR2_CLK < LPDDR2_DLL_LOW_FREQ_MODE_LIMIT)
		*(volatile unsigned long *)addr(0x306018) = (1<<3)|(1<<2); // ctrl_dfdqs, ctrl_half
	else
		*(volatile unsigned long *)addr(0x306018) = (1<<3); // ctrl_dfdqs

	*(volatile unsigned long *)addr(0x30601C) = (1<<23)|(8<<4)|(4<<0); // ctrl_cmosrcv, ctrl_ref, ctrl_shiftc
	*(volatile unsigned long *)addr(0x306020) = 0;

	if(LPDDR2_CLK < LPDDR2_DLL_LOW_FREQ_MODE_LIMIT){
		*(volatile unsigned long *)addr(0x306018) = (20<<16)|(20<<8)|(1<<3)|(1<<2)|(1<<1); // ctrl_inc, ctrl_start_point, ctrl_dfdqs, ctrl_half, ctrl_dll_on
		*(volatile unsigned long *)addr(0x306018) = (20<<16)|(20<<8)|(1<<3)|(1<<2)|(1<<1)|(1<<0); // ctrl_inc, ctrl_start_point, ctrl_dfdqs, ctrl_half, ctrl_dll_on, ctrl_start
	}else{
		*(volatile unsigned long *)addr(0x306018) = (20<<16)|(20<<8)|(1<<3)|(1<<1); // ctrl_inc, ctrl_start_point, ctrl_dfdqs, ctrl_dll_on
		*(volatile unsigned long *)addr(0x306018) = (20<<16)|(20<<8)|(1<<3)|(1<<1)|(1<<0); // ctrl_inc, ctrl_start_point, ctrl_dfdqs, ctrl_dll_on, ctrl_start
	}

//--------------------------------------------------------------------------
// Memory config

	//Enable Out of order scheduling
	*(volatile unsigned long *)addr(0x306000) =
		  ( 3 << 28) // phy_type, DDR3PHY
		| (0xff << 16) // timeout_cnt
		| ( 2 << 12) // rd_fetch
		| ( 0 << 11) // qos_fast_en
		| ( 0 << 10) // dq_swap
		| ( 0 <<  9) // chip1_empty
		| ( 0 <<  8) // chip0_empty
		| ( 0 <<  7) // drv_en
		| ( 1 <<  6) // ctc_trt_gap_en
		| ( 1 <<  5) // aref_en   // auto refresh should be disabled at this moment
		| ( 1 <<  4) // out_of
		| ( 1 <<  3) // div_pipe
		| ( 0 <<  1) // clk_ratio
		| ( 0 <<  0) // async
		;

	//MEMCTRL
	*(volatile unsigned long *)addr(0x306004 ) = (0x2 << 20) |
	                                             ((LPDDR2_LOGICAL_CHIP_NUM-1)<<16) |
	                                             ((LPDDR2_LOGICAL_DATA_BITS/16)<<12) |
	                                             (0x2 << 8) |
	                                             (0x1 << 6) |
	                                             (0x0 << 5) |
	                                             (0x0 << 4) |
	                                             (0x0 << 2) |
	                                             (0x0 << 1) |
	                                             (0x0);
	//MEMCHIP0
	*(volatile unsigned long *)addr(0x306008 ) = (0x40<<24) |
	                                             ((0xFF - (LPDDR2_TOTAL_MB_SIZE/(LPDDR2_LOGICAL_CHIP_NUM*0x10)-1))<<16) |
	                                             (0x1 << 12) |
	                                             ((LPDDR2_COLBITS - 7)<<8) |
	                                             ((LPDDR2_ROWBITS - 12)<<4) |
	                                             LPDDR2_BANK_BITS;
	//MEMCHIP1
	if(LPDDR2_LOGICAL_CHIP_NUM == 2)
	*(volatile unsigned long *)addr(0x30600C ) = ((0x40 + LPDDR2_TOTAL_MB_SIZE/(LPDDR2_LOGICAL_CHIP_NUM*0x10))<<24) |
		                                         ((0xFF - (LPDDR2_TOTAL_MB_SIZE/(LPDDR2_LOGICAL_CHIP_NUM*0x10)-1))<<16) |
		                                         (0x1 << 12) |
		                                         ((LPDDR2_COLBITS - 7)<<8) |
		                                         ((LPDDR2_ROWBITS - 12)<<4) |
		                                         LPDDR2_BANK_BITS;


//--------------------------------------------------------------------------

	*(volatile unsigned long *)addr(0x306014 ) = 0x0; //PRECONFIG
	//*(volatile unsigned long *)addr(0x306028 ) = 0xFFFF00FF; //PWRDNCONFIG


//--------------------------------------------------------------------------
// Timing parameter setting.

	//T_REFI
	*(volatile unsigned long *)addr(0x306030 ) = time2cycle(LPDDR2_tREFI_ps, tCK);

	//TROW
	*(volatile unsigned long *)addr(0x306034 ) = time2cycle(LPDDR2_tRAS_ps, tCK); //tRAS
	*(volatile unsigned long *)addr(0x306034 ) |= (time2cycle(LPDDR2_tRC_ps, tCK)<<6); //tRC
	*(volatile unsigned long *)addr(0x306034 ) |= (time2cycle(LPDDR2_tRCD_ps, tCK)<<12); //tRCD
	*(volatile unsigned long *)addr(0x306034 ) |= (time2cycle(LPDDR2_tRP_ps, tCK)<<16); //tRP
	*(volatile unsigned long *)addr(0x306034 ) |= (time2cycle(LPDDR2_tRRD_ps, tCK)<<20); //tRRD
	*(volatile unsigned long *)addr(0x306034 ) |= (time2cycle(LPDDR2_tRFC_ps, tCK)<<24); //tRFC

	//TDATA
	*(volatile unsigned long *)addr(0x306038 ) = LPDDR2_tRL_ck; //tRL
	*(volatile unsigned long *)addr(0x306038 ) |= (LPDDR2_tWL_ck<<8); //tWL
	*(volatile unsigned long *)addr(0x306038 ) |= (time2cycle(LPDDR2_tRTP_ps, tCK)<<20); //tRTP
	*(volatile unsigned long *)addr(0x306038 ) |= (time2cycle(LPDDR2_tWR_ps, tCK)<<24); //tWR
	*(volatile unsigned long *)addr(0x306038 ) |= (time2cycle(LPDDR2_tWTR_ps, tCK)<<28); //tWTR

	//TPOWER
	*(volatile unsigned long *)addr(0x30603C ) = LPDDR2_tMRD_ck; //tMRD
	*(volatile unsigned long *)addr(0x30603C ) |= (time2cycle(LPDDR2_tCKE_ps, tCK)<<4); //tCKE
	*(volatile unsigned long *)addr(0x30603C ) |= (time2cycle(LPDDR2_tXP_ps, tCK)<<8); //tXP
	*(volatile unsigned long *)addr(0x30603C ) |= (time2cycle(LPDDR2_tXSR_ps, tCK)<<16); //tXSR
	*(volatile unsigned long *)addr(0x30603C ) |= (time2cycle(LPDDR2_tFAW_ps, tCK)<<24); //tFAW


//--------------------------------------------------------------------------
// If QoS scheme is required, set the QosControl0~15 and QosConfig0~15 registers.
    //0xf0306060

//--------------------------------------------------------------------------
// Check whether PHY DLL is locked.

	while (((*(volatile unsigned long *)addr(0x306040)) & (0x4)) != 0x4);

	// Update DLL information
	*(volatile unsigned long *)addr(0x30601C) = (1<<23)|(8<<4)|(1<<3)|(4<<0); // ctrl_cmosrcv, ctrl_ref, fp_resync, ctrl_shiftc
	//*(volatile unsigned long *)addr(0x30601C) = (1<<23)|(8<<4)|(4<<0); // ctrl_cmosrcv, ctrl_ref, ctrl_shiftc

	i = 2000; while(i--); // Wait 100 ns

//--------------------------------------------------------------------------
// MRS Setting

	//Direct Command
	*(volatile unsigned long *)addr(0x306010 ) = 0x07000000;//NOP
	// Wait 200 us 
	i = 2000; while(i--);

	// MRW (Reset)
	*(volatile unsigned long *)addr(0x306010) = (0<<24)|(0<<20)| // cmd_type|chip_sel
		((0x3f&0xc0)>>6)|(((0x3f&0x38)>>3)<<16)|((0x3f&0x7)<<10)| // mr_addr
		((0x00)<<2); // mr_value
	i = 100; while(i--); // Wait 1us

	// MRW (Device Feature1)
	*(volatile unsigned long *)addr(0x306010) = (0<<24)|(0<<20)| // cmd_type|chip_sel
		((0x01&0xc0)>>6)|(((0x01&0x38)>>3)<<16)|((0x01&0x7)<<10)| // mr_addr
		((((time2cycle(LPDDR2_tWR_ps, tCK)-2)<<5)|0x2)<<2); // mr_value

	// MRW (Device Feature2)
	*(volatile unsigned long *)addr(0x306010) = (0<<24)|(0<<20)| // cmd_type|chip_sel
		((0x02&0xc0)>>6)|(((0x02&0x38)>>3)<<16)|((0x02&0x7)<<10)| // mr_addr
		((LPDDR2_tRL_ck-2)<<2); // mr_value

	// MRW (I/O Config-1)
	*(volatile unsigned long *)addr(0x306010) = (0<<24)|(0<<20)| // cmd_type|chip_sel
		((0x03&0xc0)>>6)|(((0x03&0x38)>>3)<<16)|((0x03&0x7)<<10)| // mr_addr
		((LPDDR2_DS)<<2); // mr_value

	// MRW (ZQ Calibration)
	//-- add ZQ calibration MRW here if LPDDR2 memmory supports ZQ calibration
	*(volatile unsigned long *)addr(0x306010) = (0<<24)|(0<<20)| // cmd_type|chip_sel
		((0x0A&0xc0)>>6)|(((0x0A&0x38)>>3)<<16)|((0x0A&0x7)<<10)| // mr_addr
		((0xFF)<<2); // mr_value

	// MRR (Device Auto-Initialization, DAI, polling)
	*(volatile unsigned long *)addr(0x306010) = (0<<24)|(0<<20)| // cmd_type|chip_sel
		((0x00&0xc0)>>6)|(((0x00&0x38)>>3)<<16)|((0x00&0x7)<<10)| // mr_addr
		((0x00)<<2); // mr_value

	while((((*(volatile unsigned long *)addr(0x306010))&0x3FC)>>2)&0x01);
	i = 100; while(i--); // Wait 1us

	if(LPDDR2_LOGICAL_CHIP_NUM == 2)
	{
		//Direct Command
		*(volatile unsigned long *)addr(0x306010 ) = 0x07000000|Hw20;//NOP
		// Wait 200 us 
		i = 2000; while(i--);

		// MRW (Reset)
		*(volatile unsigned long *)addr(0x306010) = (0<<24)|(1<<20)| // cmd_type|chip_sel
			((0x3f&0xc0)>>6)|(((0x3f&0x38)>>3)<<16)|((0x3f&0x7)<<10)| // mr_addr
			((0x00)<<2); // mr_value
		i = 100; while(i--); // Wait 1us

		// MRW (Device Feature1)
		*(volatile unsigned long *)addr(0x306010) = (0<<24)|(1<<20)| // cmd_type|chip_sel
			((0x01&0xc0)>>6)|(((0x01&0x38)>>3)<<16)|((0x01&0x7)<<10)| // mr_addr
			((((time2cycle(LPDDR2_tWR_ps, tCK)-2)<<5)|0x2)<<2); // mr_value

		// MRW (Device Feature2)
		*(volatile unsigned long *)addr(0x306010) = (0<<24)|(1<<20)| // cmd_type|chip_sel
			((0x02&0xc0)>>6)|(((0x02&0x38)>>3)<<16)|((0x02&0x7)<<10)| // mr_addr
			((LPDDR2_tRL_ck-2)<<2); // mr_value

		// MRW (I/O Config-1)
		*(volatile unsigned long *)addr(0x306010) = (0<<24)|(1<<20)| // cmd_type|chip_sel
			((0x03&0xc0)>>6)|(((0x03&0x38)>>3)<<16)|((0x03&0x7)<<10)| // mr_addr
			((LPDDR2_DS)<<2); // mr_value

		// MRW (ZQ Calibration)
		//-- add ZQ calibration MRW here if LPDDR2 memmory supports ZQ calibration
		*(volatile unsigned long *)addr(0x306010) = (0<<24)|(1<<20)| // cmd_type|chip_sel
			((0x0A&0xc0)>>6)|(((0x0A&0x38)>>3)<<16)|((0x0A&0x7)<<10)| // mr_addr
			((0xFF)<<2); // mr_value

		// MRR (Device Auto-Initialization, DAI, polling)
		*(volatile unsigned long *)addr(0x306010) = (0<<24)|(1<<20)| // cmd_type|chip_sel
			((0x00&0xc0)>>6)|(((0x00&0x38)>>3)<<16)|((0x00&0x7)<<10)| // mr_addr
			((0x00)<<2); // mr_value

		while((((*(volatile unsigned long *)addr(0x306010))&0x3FC)>>2)&0x01);
		i = 100; while(i--); // Wait 1us
	}

//--------------------------------------------------------------------------

	*(volatile unsigned long *)addr(0x303020 ) =  0x0007010A;//EMCCFG
	*(volatile unsigned long *)addr(0x306000 ) |= 0x20; // Auto Refresh Enable 

	if(LPDDR2_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x306048)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
	else//LPDDR2_BANK_NUM == 4
		while (((*(volatile unsigned long *)addr(0x306048)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED

	if(LPDDR2_LOGICAL_CHIP_NUM == 2){
		if(LPDDR2_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
		else//LPDDR2_BANK_NUM == 4
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED
	}

//--------------------------------------------------------------------------

#endif
}
#endif
/*=========================================================================*/



/*===========================================================================

                        Power Management Driver

===========================================================================*/

/*===========================================================================
FUNCTION
	mode 0: sleep mode, mode 1: shut down mode
===========================================================================*/
static int tcc_pm_enter(suspend_state_t state)
{
	unsigned long flags;
	unsigned reg_backup[20];
	unsigned suspend_mode_flag; // 0:sleep, 1:shutdown

	pmdrv_dbg("[%s] Start func\n", __func__);

// -------------------------------------------------------------------------
// disable interrupt
	local_irq_save(flags);
	local_irq_disable();

	// save power key gpio -------------------------------------------------
	reg_backup[0] = *((volatile unsigned long *)0xF0308004);	//GPAEN

	if( machine_is_tcc8800st())
	{
		reg_backup[1] = *((volatile unsigned long *)0xF0308100);	//GPEDAT
		reg_backup[2] = *((volatile unsigned long *)0xF0308104);	//GPEEN
		reg_backup[3] = *((volatile unsigned long *)0xF0308134);	//GPEIN
	}

#if defined(CONFIG_REGULATOR_AXP192_PEK)
	// set power key gpio for wake-up --------------------------------------
	*((volatile unsigned long *)0xF0308144) &= ~Hw24; //GPIO_F24 : input

#if !defined(TCC_PM_SLEEP_WFI_USED) || defined(TCC_PM_BOTH_SUSPEND_USED)
	// set wake-up source --------------------------------------------------
	*((volatile unsigned long *)0xF0404004) = HwPMU_WKUP_GPIOF24 | HwPMU_WKUP_RTCWKUP;	// WKUPEN0 : Config Wake-up Event
	if (machine_is_m801_88())
	{
		*((volatile unsigned long *)0xF0404008) = HwPMU_WKUP_GPIOF24;		//PWR_KEY => Active Low,

		// Start : Wakeup for SD Insert/Remove in suspend. - 110902, hjbae
		if(system_rev == 0x0004 || system_rev == 0x0005 || system_rev == 0x0006)
		{
			if(gpio_get_value(TCC_GPE(25)) == 0)		// if(E25=0) : Low(insert) -> High(remove) : Active High
				*((volatile unsigned long *)0xF0404004) |= HwPMU_WKUP_GPIOE25;
		}
		// End
	}
	else if (machine_is_m803())
	{
		*((volatile unsigned long *)0xF0404008) = HwPMU_WKUP_GPIOF24;		//PWR_KEY => Active Low,

		// Start : Wakeup for SD Insert/Remove in suspend. - 110909, hjbae
		if(system_rev == 0x0012 || system_rev == 0x0013)		// system_rev : 0x0012 = M803 V0.2A
		{
			if(gpio_get_value(TCC_GPE(25)) == 0)		// if(E25=0) : Low(insert) -> High(remove) : Active High
				*((volatile unsigned long *)0xF0404004) |= HwPMU_WKUP_GPIOE25;
		}
		// End
	}
	else if(system_rev == 0x500 || system_rev == 0x601)		// TCC8800
	{
		*((volatile unsigned long *)0xF0404008) = 0;	//PWR_KEY => Active High, RTCWKUP => Active High
	}
	else // if(system_rev == 0x600)	// TCC8801
	{
		*((volatile unsigned long *)0xF0404008) = HwPMU_WKUP_GPIOF24;		//PWR_KEY => Active Low,
	}
#endif
#else
	// set power key gpio for wake-up --------------------------------------
	*((volatile unsigned long *)0xF0308004) &= ~Hw3; //GPIO_A3 : input

#if defined(CONFIG_TCC_CSR_BC0406_MODULE_SUPPORT) && defined(CONFIG_TCC_CSR_HOST_WAKE_UP)
	*((volatile unsigned long *)0xF0308044) &= ~Hw31; // GPIO_B31 : input for Bluetooth Host Wake Up		
#endif
#if !defined(TCC_PM_SLEEP_WFI_USED) || defined(TCC_PM_BOTH_SUSPEND_USED)
	// set wake-up source --------------------------------------------------
	#if defined(CONFIG_TCC_CSR_BC0406_MODULE_SUPPORT) && defined(CONFIG_TCC_CSR_HOST_WAKE_UP)
	*((volatile unsigned long *)0xF0404004) = HwPMU_WKUP_GPIOA03 | HwPMU_WKUP_RTCWKUP | HwPMU_WKUP_GPIOB31;	// WKUPEN0 : Config Wake-up Event, GPIOB31 WAKEUP source for CSR bluetooth host wake up
	#else
	*((volatile unsigned long *)0xF0404004) = HwPMU_WKUP_GPIOA03 | HwPMU_WKUP_RTCWKUP;	// WKUPEN0 : Config Wake-up Event
	#endif
	if (machine_is_m801_88())
	{
		*((volatile unsigned long *)0xF0404008) = HwPMU_WKUP_GPIOA03;		//PWR_KEY => Active Low,
	}
	else if(system_rev == 0x500 || system_rev == 0x601)		// TCC8800
	{
		*((volatile unsigned long *)0xF0404008) = 0;	//PWR_KEY => Active High, RTCWKUP => Active High
	}
	else // if(system_rev == 0x600)	// TCC8801
	{
		*((volatile unsigned long *)0xF0404008) = HwPMU_WKUP_GPIOA03;		//PWR_KEY => Active Low,

		// Start : Wakeup for SD Insert/Remove in suspend. - 110721, hjbae
		if(system_rev == 0x0614 || system_rev == 0x0615 || system_rev == 0x0621 || system_rev == 0x0622 || system_rev == 0x0623 || system_rev == 0x0624)
		{
			if(gpio_get_value(TCC_GPA(15)) == 0)		// if(A15=0) : Low(insert) -> High(remove) : Active High
				*((volatile unsigned long *)0xF0404004) |= HwPMU_WKUP_GPIOA15;

			#if defined(CONFIG_MMC_TCC_PORT3)
			if(gpio_get_value(TCC_GPA(13)) == 0)		// if(A13=0) : Low(insert) -> High(remove) : Active High
				*((volatile unsigned long *)0xF0404004) |= HwPMU_WKUP_GPIOA13;
			#endif
		}
		// End
	}
#endif
#endif

#if 0
	//jckim 101230 for reduce sleep current
	*((volatile unsigned long *)0xf0404018) |= (Hw2 | Hw3);		//USB0,1 Power-On (isolation off)
	*((volatile unsigned long *)0xf0400048) |= Hw21;		//HSIO clk enable
	*((volatile unsigned long *)0xF08A0034) &= ~Hw8;				//USB 2.0 HOST PHY power off
	*((volatile unsigned long *)0xf0400048) &= ~Hw21;		//HSIO clk disable
	*((volatile unsigned long *)0xf0404018) &= ~(Hw2 | Hw3);		//USB0,1 Power-Off (isolation on)
#endif

// -------------------------------------------------------------------------
// Shut-down or Sleep

#if defined(TCC_PMIC_COREPWR_CTRL_USED) || defined(CONFIG_REGULATOR)
	#if defined(CONFIG_REGULATOR_AXP192)		// axp192 pmic
	PMIC_PARAM(DEV_ADDR)      = 0x68;
	PMIC_PARAM(CORE_CTRL_REG) = 0x23;	// dcdc1
	PMIC_PARAM(READ_SIZE)     = 0;
	PMIC_PARAM(WRITE_SIZE)    = 1;
	PMIC_PARAM(WRITE_DATA1_S) = 0x0A;	// 0.95V
	PMIC_PARAM(WRITE_DATA2_S) = 0x00;
	PMIC_PARAM(WRITE_DATA1_W) = 0x1A;	// 1.35V
	PMIC_PARAM(WRITE_DATA2_W) = 0x00;
	#elif defined(TCC_PMIC_COREPWR_CTRL_USED)	// expender
	PMIC_PARAM(DEV_ADDR)      = 0xEE;
	PMIC_PARAM(CORE_CTRL_REG) = 0x02;	// output0
	PMIC_PARAM(READ_SIZE)     = 2;
	PMIC_PARAM(WRITE_SIZE)    = 2;
	PMIC_PARAM(WRITE_DATA1_S) = 0x00;
	PMIC_PARAM(WRITE_DATA2_S) = 0x04;
	PMIC_PARAM(WRITE_DATA1_W) = 0x00;
	PMIC_PARAM(WRITE_DATA2_W) = 0x04;
	#else
	/* No Devices */
	PMIC_PARAM(DEV_ADDR)      = 0x00;
	#endif
#endif

#ifdef TCC_PM_BOTH_SUSPEND_USED
	#ifdef CONFIG_SLEEP_MODE
	suspend_mode_flag = 0; // sleep mode
	#endif
	#ifdef CONFIG_SHUTDOWN_MODE
	suspend_mode_flag = 1; // shutdown mode

	*((volatile unsigned long *)0xF0308084) &= ~Hw28;		//GPIO_C28, input mode [RTC_PMWKUP]
	*((volatile unsigned long *)0xF0404004) |= HwPMU_WKUP_GPIOC28;	// Set wake-up source = GPIO_C28
	*((volatile unsigned long *)0xF0404008) &= ~HwPMU_WKUP_GPIOC28;	// Active High = GPIO_C28
	#endif

	if(suspend_mode_flag == 1)
#endif
#ifdef TCC_PM_SHUTDOWN_MODE
	{
		memcpy((void*)SRAM_BOOT_ADDR,       (void*)SRAM_Boot,      SRAM_BOOT_SIZE);
		memcpy((void*)IO_RESTORE_FUNC_ADDR, (void*)io_restore,     IO_RESTORE_FUNC_SIZE);
		memcpy((void*)SDRAM_INIT_FUNC_ADDR, (void*)sdram_init_shutdown, SDRAM_INIT_FUNC_SIZE);
		memcpy((void*)SHUTDOWN_FUNC_ADDR,   (void*)shutdown,       SHUTDOWN_FUNC_SIZE);
		memcpy((void*)MMU_SWITCH_FUNC_ADDR, (void*)tcc_mmu_switch, MMU_SWITCH_FUNC_SIZE);

		#ifdef TCC_MEMBUS_PWR_CTRL_USED
		if(system_rev == 0x0612)
			*(volatile unsigned long *)SHUTDOWN_STACK_ADDR = 1;
		else if(system_rev == 0x0613 || system_rev == 0x0614 || system_rev == 0x0615 || system_rev == 0x0620 || system_rev == 0x0621 || system_rev == 0x0622 || system_rev == 0x0623 || system_rev == 0x0624)
			*(volatile unsigned long *)SHUTDOWN_STACK_ADDR = 2;
		#ifdef CONFIG_MACH_M801_88
		else if(system_rev == 0x0003 || system_rev == 0x0004 || system_rev == 0x0005 || system_rev == 0x0006)
			*(volatile unsigned long *)SHUTDOWN_STACK_ADDR = 3;
		#endif
		else
			*(volatile unsigned long *)SHUTDOWN_STACK_ADDR = 0;

		#ifdef CONFIG_MACH_TCC8800ST
			*(volatile unsigned long *)SHUTDOWN_STACK_ADDR = 4;
		#endif
		#endif

		#if defined(TCC_PMIC_COREPWR_CTRL_USED) || defined(CONFIG_REGULATOR)
		memcpy((void*)COREPWR_FUNC_ADDR,    (void*)set_core_voltage_shutdown, COREPWR_FUNC_SIZE);
		#endif

		shutdown_mode();
	}
#endif

#ifdef TCC_PM_BOTH_SUSPEND_USED
	if(suspend_mode_flag == 0)
#endif
#ifdef TCC_PM_SLEEP_MODE
	{
		memcpy((void*)SLEEP_FUNC_ADDR,      (void*)sleep, SLEEP_FUNC_SIZE);
		memcpy((void*)SDRAM_INIT_FUNC_ADDR, (void*)sdram_init_sleep, SDRAM_INIT_FUNC_SIZE);

		#ifdef TCC_MEMBUS_PWR_CTRL_USED
		if(system_rev == 0x0612)
			*(volatile unsigned long *)SLEEP_STACK_ADDR = 1;
		else if(system_rev == 0x0613 || system_rev == 0x0614 || system_rev == 0x0615 || system_rev == 0x0620 || system_rev == 0x0621 || system_rev == 0x0622 || system_rev == 0x0623 || system_rev == 0x0624)
			*(volatile unsigned long *)SLEEP_STACK_ADDR = 2;
		#ifdef CONFIG_MACH_M801_88
		else if(system_rev == 0x0003 || system_rev == 0x0004 || system_rev == 0x0005 || system_rev == 0x0006)
			*(volatile unsigned long *)SLEEP_STACK_ADDR = 3;
		#endif
		else
			*(volatile unsigned long *)SLEEP_STACK_ADDR = 0;

		#ifdef CONFIG_MACH_TCC8800ST
			*(volatile unsigned long *)SLEEP_STACK_ADDR = 4;
		#endif
		#endif

		#if defined(TCC_PMIC_COREPWR_CTRL_USED) || defined(CONFIG_REGULATOR)
		memcpy((void*)COREPWR_FUNC_ADDR,    (void*)set_core_voltage_sleep, COREPWR_FUNC_SIZE);
		#endif

		sleep_mode();
	}
#endif

// -------------------------------------------------------------------------

	// restore power key gpio ----------------------------------------------
	*((volatile unsigned long *)0xF0308004) = reg_backup[0];

	if( machine_is_tcc8800st())
	{
		*((volatile unsigned long *)0xF0308100) = reg_backup[1];	//GPEDAT
		*((volatile unsigned long *)0xF0308104) = reg_backup[2];	//GPEEN
		*((volatile unsigned long *)0xF0308134) = reg_backup[3];	//GPEIN
	}

	//Bruce, 110405, To prevent to control BUSes of PMU, before memory clock is initialized.
	{
		extern unsigned int mem_clk_initialized;
		mem_clk_initialized = 0;
	}

// -------------------------------------------------------------------------
// enable interrupt
	local_irq_restore(flags);

	//for debugging rtc block - 110106, hjbae
	//*((volatile unsigned long *)0xF05F2000) = 0xC2;
	//*((volatile unsigned long *)0xF05F2004) = 0x9003;
	//printk("---> RTC[%02X][%02X][%02X]", *((volatile unsigned long *)0xF05F2044), *((volatile unsigned long *)0xF05F2048), *((volatile unsigned long *)0xF05F204C));

	pmdrv_dbg("[%s] End func\n", __func__);
	return 0;
}

/*===========================================================================
FUNCTION
===========================================================================*/
static void tcc_pm_power_off(void)
{
#ifdef CONFIG_RTC_DISABLE_ALARM_FOR_PWROFF_STATE		//Disable the RTC Alarm during the power off state
	extern volatile void tca_alarm_disable(unsigned int rtcbaseaddresss);

	tca_alarm_disable(tcc_p2v(HwRTC_BASE));
#endif

#if defined(CONFIG_REGULATOR_AXP192)
	extern void axp192_power_off(void);
	axp192_power_off();
#endif

	if(machine_is_m801_88() || machine_is_m803()){
		BITCLR(HwGPIOA->GPDAT, Hw4);    // LCD_BLCTL
		BITCLR(HwGPIOG->GPDAT, Hw2);    // LCD_PWREN

		BITCLR(HwGPIOF->GPDAT, Hw26);   // SHDN
	}

	while(1);
}

/*===========================================================================
VARIABLE
===========================================================================*/
static struct platform_suspend_ops tcc_pm_ops = {
	.valid   = suspend_valid_only_mem,
	.enter   = tcc_pm_enter,
};

/*===========================================================================
VARIABLE
===========================================================================*/
static uint32_t restart_reason = 0x776655AA;

/*===========================================================================
FUNCTION
===========================================================================*/
static void tcc_pm_restart(char str)
{
	/* store restart_reason to USTS register */
	if (restart_reason != 0x776655AA)
		writel((readl(PMU_CONFIG1) & 0xFFFFFF00) | (restart_reason & 0xFF), PMU_CONFIG1);

	arch_reset(str, NULL);
}

/*===========================================================================
FUNCTION
===========================================================================*/
static int tcc_reboot_call(struct notifier_block *this, unsigned long code, void *cmd)
{
	/* XXX: convert reboot mode value because USTS register
	 * hold only 8-bit value
	 */
	if (code == SYS_RESTART) {
		if (cmd) {
			if (!strcmp(cmd, "bootloader")) {
				restart_reason = 1;	/* fastboot mode */
			} else if (!strcmp(cmd, "recovery")) {
				restart_reason = 2;	/* recovery mode */
			} else {
				restart_reason = 0;
			}
		} else {
			restart_reason = 0;
		}
	}
	return NOTIFY_DONE;
}

/*===========================================================================
VARIABLE
===========================================================================*/
static struct notifier_block tcc_reboot_notifier = {
	.notifier_call = tcc_reboot_call,
};

/*===========================================================================
FUNCTION
===========================================================================*/
static int __init tcc_pm_init(void)
{
	pm_power_off = tcc_pm_power_off;
	arm_pm_restart = tcc_pm_restart;

	register_reboot_notifier(&tcc_reboot_notifier);

	suspend_set_ops(&tcc_pm_ops);
	return 0;
}

__initcall(tcc_pm_init);

