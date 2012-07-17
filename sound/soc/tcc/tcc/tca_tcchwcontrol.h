/****************************************************************************
 *   FileName    : tca_tcchwcontrol.h
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
#ifndef __TCA_TCCHWCONTROL_H__
#define __TCA_TCCHWCONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
*
* Defines
*
******************************************************************************/

//#define	AUDIO_DMA_PAGE_SIZE		(256)					// Size in bytes
//#define	AUDIO_DMA_IN_PAGE_SIZE	(256)					// Size in bytes

//i2s control api
extern unsigned int tca_i2s_dai_init(void *pADMADAIBaseAddr);
extern unsigned int tca_i2s_deinit(void *pADMADAIBaseAddr);
extern unsigned int tca_i2s_setregister(void *pADMADAIBaseAddr, unsigned int nRegval);
extern unsigned int tca_i2s_getregister(void *pADMADAIBaseAddr);

extern void tca_i2s_start(void *pADMADAIBaseAddr, unsigned int nMode);
extern void tca_i2s_stop(void *pADMADAIBaseAddr, unsigned int nMode);

extern void tca_i2s_initoutput(void *pADMABaseAddr, unsigned int,unsigned int,unsigned int);
extern void tca_i2s_initinput(void *pADMABaseAddr, unsigned int,unsigned int,unsigned int);

//Get IRQ GetNumber
extern unsigned int tca_irq_getnumber(void);

//dma control api
extern unsigned int tca_dma_clrstatus(void *pADMABaseAddr, unsigned int nDmanum);
extern unsigned int tca_dma_getstatus(void *pADMABaseAddr, unsigned int nDmanum);
extern unsigned int tca_dma_control(void *pADMABaseAddr, void *pADMADAIBaseAddr, unsigned int nMode, unsigned int nDmanum, unsigned int nInMode);
extern unsigned int tca_dma_setsrcaddr(void *pADMABaseAddr, unsigned int DADONum, unsigned int nDmanum, unsigned int nAddr);
extern unsigned int tca_dma_setdestaddr(void *pADMABaseAddr, unsigned int DADINum, unsigned int nDmanum, unsigned int nAddr);

//GPIO port INIT
extern unsigned int tca_tcc_initport(void);
#ifdef __cplusplus
}
#endif
#endif

