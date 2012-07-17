/******************************************************************************
*
*  (C)Copyright All Rights Reserved by Telechips Inc.
*
*  This material is confidential and shall remain as such.
*  Any unauthorized use, distribution, reproduction is strictly prohibited.
*
*******************************************************************************
*
*  File Name   : vioc_wmix.c
*
*  Description : VIOC wmix components control module file.
*
*******************************************************************************
*
*  yyyy/mm/dd     ver            descriptions                Author
*	---------	--------   ---------------       -----------------
*    2011/08/10     0.1            created                      hskim
*******************************************************************************/
#include <linux/kernel.h>
#include <mach/bsp.h>
#include <mach/io.h>

#include <mach/vioc_wmix.h>
#include <mach/globals.h>
#include <mach/tccfb_ioctrl.h>
#include <mach/vioc_plugin_tcc892x.h>

#define VIOC_WMIX_IREQ_MUPD_MASK		0x00000001UL
#define VIOC_WMIX_IREQ_MEOFWF_MASK	0x00000002UL
#define VIOC_WMIX_IREQ_MEOFWR_MASK 	0x00000004UL
#define VIOC_WMIX_IREQ_MEOFR_MASK 	0x00000008UL
#define VIOC_WMIX_IREQ_MEOFF_MASK 	0x00000010UL

void VIOC_WMIX_SetOverlayPriority(VIOC_WMIX *pWMIX, unsigned int nOverlayPriority)
{
	//pWMIX->uCTRL.bREG.OVP = nOverlayPriority;
	BITCSET(pWMIX->uCTRL.nREG,0x0000001F,nOverlayPriority);
}

void VIOC_WMIX_GetOverlayPriority(VIOC_WMIX *pWMIX, unsigned int *nOverlayPriority)
{
	//*nOverlayPriority = pWMIX->uCTRL.bREG.OVP;
	*nOverlayPriority = pWMIX->uCTRL.nREG & 0x1F;
}

void VIOC_WMIX_SetUpdate(VIOC_WMIX *pWMIX)
{
	//pWMIX->uCTRL.bREG.UPD = 1;
	BITCSET(pWMIX->uCTRL.nREG, 1<<16, 1<<16);
}


#if 0 //[[ hskim_20110920_BEGIN -- Not Support
void VIOC_WMIX_SetAlphOpt(VIOC_WMIX *pWMIX, unsigned int opt)
{
	/* 0 : adopt alpha value 1: direct adopt alpha value / reset default : 0 */
	pWMIX->uCTRL.bREG.AOPT1 = opt;
}
#endif //]] hskim_20110920_END -- Not Support

void VIOC_WMIX_SetSize(VIOC_WMIX *pWMIX, unsigned int nWidth, unsigned int nHeight)
{
	/*
	pWMIX->uSIZE.bREG.WIDTH  = nWidth;
	pWMIX->uSIZE.bREG.HEIGHT = nHeight;
	*/

	BITCSET(pWMIX->uSIZE.nREG, 0xFFFFFFFF, (nHeight << 16) | (nWidth) );	
}

void VIOC_WMIX_SetBGColor(VIOC_WMIX *pWMIX, unsigned int nBG0, unsigned int nBG1, unsigned int nBG2, unsigned int nBG3)
{
	//pWMIX->uBG.bREG.BG0 = nBG0; /* Cr/R */
	//pWMIX->uBG.bREG.BG1 = nBG1; /* Cb/G */
	//pWMIX->uBG.bREG.BG2 = nBG2; /* Y/B */
	//pWMIX->uBG.bREG.BG3 = nBG3; /* ALPHA */
	
	BITCSET(pWMIX->uBG.nREG, 0xFFFFFFFF, (nBG3 << 24) |(nBG2 << 16) | (nBG1<< 8) | nBG0);
}

void VIOC_WMIX_SetPosition(VIOC_WMIX *pWMIX, unsigned int nChannel, unsigned int nX, unsigned int nY)
{
	switch (nChannel)
	{
		case 0 :
			//pWMIX->uPOS0.bREG.XPOS = nX;
			//pWMIX->uPOS0.bREG.YPOS = nY;
			BITCSET(pWMIX->uPOS0.nREG, 0xFFFFFFFF,  nY << 16 | nX);
			break;
		case 1 :
			//pWMIX->uPOS1.bREG.XPOS = nX;
			//pWMIX->uPOS1.bREG.YPOS = nY;
			BITCSET(pWMIX->uPOS1.nREG, 0xFFFFFFFF,  nY << 16 | nX);
			break;
		case 2 :
			//pWMIX->uPOS2.bREG.XPOS = nX;
			//pWMIX->uPOS2.bREG.YPOS = nY;
			BITCSET(pWMIX->uPOS2.nREG, 0xFFFFFFFF,  nY << 16 | nX);			
			break;
		case 3 :
			//pWMIX->uPOS3.bREG.XPOS = nX;
			//pWMIX->uPOS3.bREG.YPOS = nY;
			BITCSET(pWMIX->uPOS3.nREG, 0xFFFFFFFF,  nY << 16 | nX);						
			break;
		default:
			break;
	}
}

void VIOC_WMIX_GetPosition(VIOC_WMIX *pWMIX, unsigned int nChannel, unsigned int *nX, unsigned int *nY)
{
	switch (nChannel)
	{
		case 0 :
			//*nX = pWMIX->uPOS0.bREG.XPOS;
			//*nY = pWMIX->uPOS0.bREG.YPOS;

			*nX = pWMIX->uPOS0.nREG & 0x0000FFFF; 
			*nY = pWMIX->uPOS0.nREG & 0xFFFF0000; 
			break;
		case 1 :
			//*nX = pWMIX->uPOS1.bREG.XPOS;
			//*nY = pWMIX->uPOS1.bREG.YPOS;
			*nX = pWMIX->uPOS1.nREG & 0x0000FFFF; 
			*nY = pWMIX->uPOS1.nREG & 0xFFFF0000; 
			break;
		case 2 :
			//*nX = pWMIX->uPOS2.bREG.XPOS;
			//*nY = pWMIX->uPOS2.bREG.YPOS;

			*nX = pWMIX->uPOS2.nREG & 0x0000FFFF; 
			*nY = pWMIX->uPOS2.nREG & 0xFFFF0000; 
			break;
		case 3 :
			//*nX = pWMIX->uPOS3.bREG.XPOS;
			//*nY = pWMIX->uPOS3.bREG.YPOS;
			*nX = pWMIX->uPOS3.nREG & 0x0000FFFF; 
			*nY = pWMIX->uPOS3.nREG & 0xFFFF0000; 
			break;
		default:
			break;
	}
}

void VIOC_WMIX_SetChromaKey(VIOC_WMIX *pWMIX, unsigned int nLayer, unsigned int nKeyEn, unsigned int nKeyR, unsigned int nKeyG, unsigned int nKeyB, unsigned int nKeyMaskR, unsigned int nKeyMaskG, unsigned int nKeyMaskB)
{
	switch (nLayer)
	{
		case 0 :    // top layer
			/*
			pWMIX->uKEY0.bREG.KEYEN = nKeyEn;
			pWMIX->uKEY0.bREG.KEYR  = nKeyR;
			pWMIX->uKEY0.bREG.KEYG  = nKeyG;
			pWMIX->uKEY0.bREG.KEYB  = nKeyB;
			pWMIX->uKEY0.bREG.KEYMR = nKeyMaskR;
			pWMIX->uKEY0.bREG.KEYMG = nKeyMaskG;
			pWMIX->uKEY0.bREG.KEYMB = nKeyMaskB;
			*/
			BITCSET(pWMIX->uKEY0.nREG[0], 0x80FFFFFF, (nKeyEn << 31) | (nKeyR << 16) | (nKeyB << 8) | nKeyB );
			BITCSET(pWMIX->uKEY0.nREG[1], 0x00FFFFFF, (nKeyMaskR<< 16) | (nKeyMaskG << 8) | nKeyMaskB);
			break;
		case 1 :
			/*
			pWMIX->uKEY1.bREG.KEYEN = nKeyEn;
			pWMIX->uKEY1.bREG.KEYR  = nKeyR;
			pWMIX->uKEY1.bREG.KEYG  = nKeyG;
			pWMIX->uKEY1.bREG.KEYB  = nKeyB;
			pWMIX->uKEY1.bREG.KEYMR = nKeyMaskR;
			pWMIX->uKEY1.bREG.KEYMG = nKeyMaskG;
			pWMIX->uKEY1.bREG.KEYMB = nKeyMaskB;
			*/
			BITCSET(pWMIX->uKEY1.nREG[0], 0x80FFFFFF, (nKeyEn << 31) | (nKeyR << 16) | (nKeyB << 8) | nKeyB );
			BITCSET(pWMIX->uKEY1.nREG[1], 0x00FFFFFF, (nKeyMaskR<< 16) | (nKeyMaskG << 8) | nKeyMaskB);			
			break;
		case 2 :

			/*
			pWMIX->uKEY2.bREG.KEYEN = nKeyEn;
			pWMIX->uKEY2.bREG.KEYR  = nKeyR;
			pWMIX->uKEY2.bREG.KEYG  = nKeyG;
			pWMIX->uKEY2.bREG.KEYB  = nKeyB;
			pWMIX->uKEY2.bREG.KEYMR = nKeyMaskR;
			pWMIX->uKEY2.bREG.KEYMG = nKeyMaskG;
			pWMIX->uKEY2.bREG.KEYMB = nKeyMaskB;
			*/
			BITCSET(pWMIX->uKEY2.nREG[0], 0x80FFFFFF, (nKeyEn << 31) | (nKeyR << 16) | (nKeyB << 8) | nKeyB );
			BITCSET(pWMIX->uKEY2.nREG[1], 0x00FFFFFF, (nKeyMaskR<< 16) | (nKeyMaskG << 8) | nKeyMaskB);
			break;
		default:
			break;
	}
}

void VIOC_WMIX_GetChromaKey(VIOC_WMIX *pWMIX, unsigned int nLayer, unsigned int *nKeyEn, unsigned int *nKeyR, unsigned int *nKeyG, unsigned int *nKeyB, unsigned int *nKeyMaskR, unsigned int *nKeyMaskG, unsigned int *nKeyMaskB)
{
	switch (nLayer)
	{
		case 0 :    // top layer
			#if 0
			*nKeyEn = pWMIX->uKEY0.bREG.KEYEN;
			*nKeyR = pWMIX->uKEY0.bREG.KEYR;
			*nKeyG = pWMIX->uKEY0.bREG.KEYG;
			*nKeyB = pWMIX->uKEY0.bREG.KEYB;
			*nKeyMaskR = pWMIX->uKEY0.bREG.KEYMR;
			*nKeyMaskG = pWMIX->uKEY0.bREG.KEYMG;
			*nKeyMaskB = pWMIX->uKEY0.bREG.KEYMB;
			#endif

			*nKeyEn = pWMIX->uKEY0.nREG[0] & 0x80000000;
			*nKeyR = pWMIX->uKEY0.nREG[0] & 0x00FF0000;
			*nKeyG = pWMIX->uKEY0.nREG[0] & 0x0000FF00;
			*nKeyB = pWMIX->uKEY0.nREG[0] & 0x000000FF;
			*nKeyMaskR = pWMIX->uKEY0.nREG[1] & 0x00FF0000;
			*nKeyMaskG = pWMIX->uKEY0.nREG[1] & 0x0000FF00;
			*nKeyMaskB = pWMIX->uKEY0.nREG[1] & 0x000000FF;
						
			break;
		case 1 :
			#if 0
			*nKeyEn = pWMIX->uKEY1.bREG.KEYEN;
			*nKeyR = pWMIX->uKEY1.bREG.KEYR;
			*nKeyG = pWMIX->uKEY1.bREG.KEYG;
			*nKeyB = pWMIX->uKEY1.bREG.KEYB;
			*nKeyMaskR = pWMIX->uKEY1.bREG.KEYMR;
			*nKeyMaskG = pWMIX->uKEY1.bREG.KEYMG;
			*nKeyMaskB = pWMIX->uKEY1.bREG.KEYMB;
			#endif

			*nKeyEn = pWMIX->uKEY1.nREG[0] & 0x80000000;
			*nKeyR = pWMIX->uKEY1.nREG[0] & 0x00FF0000;
			*nKeyG = pWMIX->uKEY1.nREG[0] & 0x0000FF00;
			*nKeyB = pWMIX->uKEY1.nREG[0] & 0x000000FF;
			*nKeyMaskR = pWMIX->uKEY1.nREG[1] & 0x00FF0000;
			*nKeyMaskG = pWMIX->uKEY1.nREG[1] & 0x0000FF00;
			*nKeyMaskB = pWMIX->uKEY1.nREG[1] & 0x000000FF;
			
			break;
		case 2 :
			#if 0
			*nKeyEn = pWMIX->uKEY2.bREG.KEYEN;
			*nKeyR = pWMIX->uKEY2.bREG.KEYR;
			*nKeyG = pWMIX->uKEY2.bREG.KEYG;
			*nKeyB = pWMIX->uKEY2.bREG.KEYB;
			*nKeyMaskR = pWMIX->uKEY2.bREG.KEYMR;
			*nKeyMaskG = pWMIX->uKEY2.bREG.KEYMG;
			*nKeyMaskB = pWMIX->uKEY2.bREG.KEYMB;
			#endif
			*nKeyEn = pWMIX->uKEY2.nREG[0] & 0x80000000;
			*nKeyR = pWMIX->uKEY2.nREG[0] & 0x00FF0000;
			*nKeyG = pWMIX->uKEY2.nREG[0] & 0x0000FF00;
			*nKeyB = pWMIX->uKEY2.nREG[0] & 0x000000FF;
			*nKeyMaskR = pWMIX->uKEY2.nREG[1] & 0x00FF0000;
			*nKeyMaskG = pWMIX->uKEY2.nREG[1] & 0x0000FF00;
			*nKeyMaskB = pWMIX->uKEY2.nREG[1] & 0x000000FF;			
			break;
			
		default:
			break;
			
	}
}

void VIOC_WMIX_ALPHA_SetAlphaValueControl( VIOC_WMIX_ALPHA * pWALPHA, unsigned int region, unsigned int acon0, unsigned int acon1)
{
	switch (region)
	{
		case 0 : /*Region A*/
			//pWALPHA->uACON.bREG.ACON0_00 = acon0;
			//pWALPHA->uACON.bREG.ACON1_00 = acon1;

			BITCSET(pWALPHA->uACON.nREG, 0xFFFFFFFF, (acon1 << 4 | acon0) );
			break;
		case 1 : /*Region B*/
			//pWALPHA->uACON.bREG.ACON0_10 = acon0;
			//pWALPHA->uACON.bREG.ACON1_10 = acon1;

			BITCSET(pWALPHA->uACON.nREG,0xFFFFFFFF, (acon1 << 4 | acon0) << 16 );
			break;
		case 2 : /*Region C*/
			//pWALPHA->uACON.bREG.ACON0_11 = acon0;
			//pWALPHA->uACON.bREG.ACON1_11 = acon1;

			BITCSET(pWALPHA->uACON.nREG,0xFFFFFFFF, (acon1 << 4 | acon0) << 24 );
			break;
		case 3 :/*Region D*/
			//pWALPHA->uACON.bREG.ACON0_01 = acon0;
			//pWALPHA->uACON.bREG.ACON1_01 = acon1;

			BITCSET(pWALPHA->uACON.nREG,0xFFFFFFFF, (acon1 << 4 | acon0) << 8 );
			break;

		default:
			break;
	}

}

void VIOC_WMIX_ALPHA_SetColorControl( VIOC_WMIX_ALPHA * pWALPHA, unsigned int region, unsigned int ccon0, unsigned int ccon1)
{
	switch (region)
	{
		case 0 :/*Region A*/
			//pWALPHA->uCCON.bREG.CCON0_00 = ccon0;
			//pWALPHA->uCCON.bREG.CCON1_00 = ccon1;
			
			BITCSET(pWALPHA->uCCON.nREG,0xFFFFFFFF, ccon1 << 4 | ccon0 );
			break;
		case 1 :/*Region B*/
			//pWALPHA->uCCON.bREG.CCON0_10 = ccon0;
			//pWALPHA->uCCON.bREG.CCON1_10 = ccon1;

			BITCSET(pWALPHA->uCCON.nREG,0xFFFFFFFF, (ccon1 << 4 | ccon0) << 16 );
			break;
		case 2 :/*Region C*/
			//pWALPHA->uCCON.bREG.CCON0_11 = ccon0;
			//pWALPHA->uCCON.bREG.CCON1_11 = ccon1;

			BITCSET(pWALPHA->uCCON.nREG,0xFFFFFFFF, (ccon1 << 4 | ccon0) << 24 );
			break;
		case 3 :/*Region D*/
			//pWALPHA->uCCON.bREG.CCON0_01 = ccon0;
			//pWALPHA->uCCON.bREG.CCON1_01 = ccon1;

			BITCSET(pWALPHA->uCCON.nREG,0xFFFFFFFF, (ccon1 << 4 | ccon0) << 8 );
			break;

		default:
			break;
	}

}

void VIOC_WMIX_ALPHA_SetROPMode( VIOC_WMIX_ALPHA * pWALPHA, unsigned int mode)
{
	//pWALPHA->uROPC.bREG.ROPMODE = mode;
	BITCSET(pWALPHA->uROPC.nREG, 0x1F, mode);
}

void VIOC_WMIX_ALPHA_SetAlphaSelection( VIOC_WMIX_ALPHA * pWALPHA, unsigned int asel)
{
	//pWALPHA->uROPC.bREG.ASEL = asel;
	BITCSET(pWALPHA->uROPC.nREG, 0x3 << 14, asel << 14);
}

void VIOC_WMIX_ALPHA_SetAlphaValue( VIOC_WMIX_ALPHA * pWALPHA, unsigned int alpha0, unsigned int alpha1)
{
	//pWALPHA->uROPC.bREG.ALPHA0 = alpha0;
	//pWALPHA->uROPC.bREG.ALPHA1 = alpha1;

	BITCSET(pWALPHA->uROPC.nREG, 0xFFFF0000,  (alpha1 << 24) | ( alpha0<< 16) );
}

void VIOC_WMIX_ALPHA_SetROPPattern( VIOC_WMIX_ALPHA * pWALPHA, unsigned int patR, unsigned int patG, unsigned int patB)
{
	/*
	pWALPHA->uPAT.bREG.RED = patR;
	pWALPHA->uPAT.bREG.GREEN = patG;
	pWALPHA->uPAT.bREG.BLUE = patB;
	*/
	
	BITCSET(pWALPHA->uPAT.nREG, 0x00FFFFFF, (patR << 16 ) | (patG << 8 ) | (patB ) );
}

void VIOC_WMIX_SetInterruptMask(VIOC_WMIX *pWMIX, unsigned int nMask, unsigned int set)
{
	if( set == 0 ) /* Interrupt Enable*/
	{
		//pWMIX->uIRQMSK.nREG &= ~nMask;
		BITCSET(pWMIX->uIRQMSK.nREG, 0x1F, ~nMask);
	}
	else/* Interrupt Diable*/
	{
		//pWMIX->uIRQMSK.nREG |= nMask;
		BITCSET(pWMIX->uIRQMSK.nREG, 0x1F, ~nMask);
	}
}

unsigned int VIOC_WMIX_GetStatus(VIOC_WMIX *pWMIX )
{
	volatile unsigned int STATUS = 0;
	STATUS = pWMIX->uSTATUS.nREG;
	return STATUS;
}

void VIOC_WMIX_IreqHandler( unsigned int vectorID )
{
#if 0	
	unsigned int 	WMIXBase;
	VIOC_WMIX *pWMIX;
	unsigned int idx;

	if ( vectorID > VIOC_IREQ_MAX)
		return;

	idx = vectorID - VIOC_IREQ_WMIX0;
	
	WMIXBase = ((unsigned int)HwVIOC_WMIX0 + idx*0x100);
	pWMIX = (VIOC_WMIX *)WMIXBase;

	if (pWMIX->uSTATUS.nREG & VIOC_WMIX_IREQ_MUPD_MASK && pWMIX->uIRQMSK.bREG.UPDDONE == 0 )
	{
		/* Interrput Source Clear */
		/* Status Clear */
		pWMIX->uSTATUS.bREG.UPDDONE = 1;
		/* DO Action for Interrupt */

		/* Interrput Re-Setting */
	}

	if (pWMIX->uSTATUS.nREG & VIOC_WMIX_IREQ_MEOFWF_MASK && pWMIX->uIRQMSK.bREG.EOFWAITFALL == 0 )
	{
		/* Interrput Source Clear */
		/* Status Clear */
		pWMIX->uSTATUS.bREG.EOFWAITFALL = 1;

		/* DO Action for Interrupt */

		/* Interrput Re-Setting */
	}

	if (pWMIX->uSTATUS.nREG & VIOC_WMIX_IREQ_MEOFWR_MASK && pWMIX->uIRQMSK.bREG.EOFWAITRISE == 0 )
	{
		/* Interrput Source Clear */
		/* Status Clear */
		pWMIX->uSTATUS.bREG.EOFWAITRISE = 1;

		/* DO Action for Interrupt */

		/* Interrput Re-Setting */
	}

	if (pWMIX->uSTATUS.nREG & VIOC_WMIX_IREQ_MEOFR_MASK && pWMIX->uIRQMSK.bREG.EOFRISE == 0 )
	{
		/* Interrput Source Clear */
		/* Status Clear */
		pWMIX->uSTATUS.bREG.EOFRISE = 1;

		/* DO Action for Interrupt */

		/* Interrput Re-Setting */
	}
	
	if (pWMIX->uSTATUS.nREG & VIOC_WMIX_IREQ_MEOFF_MASK && pWMIX->uIRQMSK.bREG.EOFFALL == 0 )
	{
		/* Interrput Source Clear */
		/* Status Clear */
		pWMIX->uSTATUS.bREG.EOFFALL = 1;

		/* DO Action for Interrupt */

		/* Interrput Re-Setting */
	}
#endif//	
}


void VIOC_API_WMIX_SetOverlayAlphaROPMode( VIOC_WMIX *pWMIX, unsigned int layer, unsigned int opmode )
{
	unsigned int baseAddr;
	baseAddr = ((unsigned int)pWMIX + 0x40  + (0x10 * layer));
	VIOC_WMIX_ALPHA_SetROPMode((VIOC_WMIX_ALPHA *)baseAddr, opmode);
}

void VIOC_API_WMIX_SetOverlayAlphaValue( VIOC_WMIX *pWMIX, unsigned int layer, unsigned int alpha0, unsigned int alpha1 )
{
	unsigned int baseAddr;
	baseAddr = ((unsigned int)pWMIX + 0x40  + (0x10 * layer));
	VIOC_WMIX_ALPHA_SetAlphaValue((VIOC_WMIX_ALPHA *)baseAddr, alpha0, alpha1);
}

void VIOC_API_WMIX_SetOverlayAlphaSelection(VIOC_WMIX *pWMIX, unsigned int layer,unsigned int asel )
{
	unsigned int baseAddr;
	baseAddr = ((unsigned int)pWMIX + 0x40  + (0x10 * layer));
	VIOC_WMIX_ALPHA_SetAlphaSelection((VIOC_WMIX_ALPHA *)baseAddr, asel);
}

void VIOC_API_WMIX_SetOverlayAlphaValueControl(VIOC_WMIX *pWMIX, unsigned int layer, unsigned int region, unsigned int acon0, unsigned int acon1 )
{
	unsigned int baseAddr = 0;
	baseAddr = ((unsigned int)pWMIX + 0x40  + (0x10 * layer));
	VIOC_WMIX_ALPHA_SetAlphaValueControl((VIOC_WMIX_ALPHA *)baseAddr, region, acon0, acon1);
}

void VIOC_API_WMIX_SetOverlayAlphaColorControl(VIOC_WMIX *pWMIX, unsigned int layer, unsigned int region, unsigned int ccon0, unsigned int ccon1 )
{
	unsigned int baseAddr;
	baseAddr = ((unsigned int)pWMIX + 0x40  + (0x10 * layer));
	VIOC_WMIX_ALPHA_SetColorControl((VIOC_WMIX_ALPHA *)baseAddr, region, ccon0, ccon1);
}

void VIOC_WMIX_SetSWReset(unsigned int WMIX, unsigned int RDMA, unsigned int WDMA)
{
	volatile PVIOC_IREQ_CONFIG pIREQConfig;
	pIREQConfig = (volatile PVIOC_IREQ_CONFIG)tcc_p2v((unsigned int)HwVIOC_IREQ);

	if(WMIX <= VIOC_WMIX6) {
		BITCSET(pIREQConfig->uSOFTRESET.nREG[1], (0x1<<(9+WMIX)), (0x1<<(9+WMIX))); // wmix reset
		BITCSET(pIREQConfig->uSOFTRESET.nREG[1], (0x1<<(9+WMIX)), (0x0<<(9+WMIX))); // wmix reset
	}

	if(RDMA <= VIOC_WMIX_RDMA_17) {
		BITCSET(pIREQConfig->uSOFTRESET.nREG[0], (0x1<<RDMA), (0x1<<RDMA)); // rdma reset
		BITCSET(pIREQConfig->uSOFTRESET.nREG[0], (0x1<<RDMA), (0x0<<RDMA)); // rdma reset
	}

	if(WDMA <= VIOC_WMIX_WDMA_08) {
		BITCSET(pIREQConfig->uSOFTRESET.nREG[1], (0x1<<WDMA), (0x1<<WDMA)); // wdma reset
		BITCSET(pIREQConfig->uSOFTRESET.nREG[1], (0x1<<WDMA), (0x0<<WDMA)); // wdma reset
	}
}

/* EOF */


