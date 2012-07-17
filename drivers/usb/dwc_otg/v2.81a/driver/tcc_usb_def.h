/*
 *
 */
#ifndef __TCC_USB_DEF_H__
#define __TCC_USB_DEF_H__

#include <mach/bsp.h>

//===================================================================
//
//		DRIVER SIGNATURE
//
//===================================================================
#define SIGBYAHONG				'S','I','G','B','Y','A','H','O','N','G','_'

#if defined(_WINCE_)
#define SIGN_OS					'W','I','N','C','E','_'
#elif defined(_LINUX_)
#define SIGN_OS					'L','I','N','U','X','_'
#else
#define SIGN_OS					'N','U','_'
#endif

#if defined(TCC77X)
#define SIGN_CHIPSET			'T','C','C','7','7','X','X','_'
#elif defined(TCC78X)
#define SIGN_CHIPSET			'T','C','C','7','8','X','X','_'
#elif defined(TCC79X)
#define SIGN_CHIPSET			'T','C','C','7','9','X','X','_'
#elif defined(TCC81XX)
#define SIGN_CHIPSET			'T','C','C','8','1','X','X','_'
#elif defined(TCC82XX)
#define SIGN_CHIPSET			'T','C','C','8','2','X','X','_'
#elif defined(TCC83XX)
#define SIGN_CHIPSET			'T','C','C','8','3','X','X','_'
#elif defined(TCC87XX)
#define SIGN_CHIPSET			'T','C','C','8','7','X','X','_'
#elif defined(TCC8900)
#define SIGN_CHIPSET			'T','C','C','8','9','X','X','_'
#elif defined(TCC91XX)
#define SIGN_CHIPSET			'T','C','C','9','1','X','X','_'
#elif defined(TCC92XX)
#define SIGN_CHIPSET			'T','C','C','9','2','X','X','_'
#elif defined(CONFIG_ARCH_TCC93XX)
#define SIGN_CHIPSET			'T','C','C','9','3','X','X','_'
#elif defined(CONFIG_ARCH_TCC88XX)
#define SIGN_CHIPSET			'T','C','C','8','8','X','X','_'
#elif defined(CONFIG_ARCH_TCC892X)
#define SIGN_CHIPSET			'T','C','C','8','9','2','X','_'
#else
#error
#endif

typedef enum {
	USB_MODE_NONE,
	USB_MODE_HOST,
	USB_MODE_DEVICE
} USB_MODE_T;

#endif /*__TCC_USB_DEF_H__*/
