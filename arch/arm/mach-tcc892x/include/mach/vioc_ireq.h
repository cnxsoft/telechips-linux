/******************************************************************************
*
*  (C)Copyright All Rights Reserved by Telechips Inc.
*
*  This material is confidential and shall remain as such.
*  Any unauthorized use, distribution, reproduction is strictly prohibited.
*
*******************************************************************************
*
*  File Name   : Vioc_ireq.h
*
*  Description : VIOC Interrupt Handler
*
*******************************************************************************
*
*  yyyy/mm/dd     ver            descriptions                Author
*	---------	--------   ---------------       -----------------
*    2011/08/23     0.1            created                      hskim
*******************************************************************************/

#ifndef __VIOC_IREQ_H__
#define	__VIOC_IREQ_H__

#include  <mach/reg_physical.h>


#if 0
/* IREQ0 */
#define VIOC_IREQ_DEV0		Hw0
#define VIOC_IREQ_TIMER		Hw3
#define VIOC_IREQ_RD0		Hw4
#define VIOC_IREQ_RD1		Hw5
#define VIOC_IREQ_RD2		Hw6
#define VIOC_IREQ_RD3		Hw7
#define VIOC_IREQ_RD4		Hw8
#define VIOC_IREQ_RD5		Hw9
#define VIOC_IREQ_RD6		Hw10

/* IREQ1 */
#define VIOC_IREQ_WD0		Hw0
#define VIOC_IREQ_WD1		Hw1
#define VIOC_IREQ_WD2		Hw2
#define VIOC_IREQ_WD3		Hw3
#define VIOC_IREQ_WD4		Hw4
#define VIOC_IREQ_WD5		Hw5
#define VIOC_IREQ_WD6		Hw6
#define VIOC_IREQ_WMIX0	Hw16
#define VIOC_IREQ_WMIX1	Hw17
#define VIOC_IREQ_WMIX2	Hw18
#define VIOC_IREQ_VIQE		Hw27
#define VIOC_IREQ_SC0		Hw28
#define VIOC_IREQ_SC1		Hw29
#define VIOC_IREQ_SC2		Hw30
#endif

typedef enum
{
	VIOC_IREQ_DEV0 = 0, 	/* IREQ0 */
	VIOC_IREQ_DEV1,
	VIOC_IREQ_DEV2,
	VIOC_IREQ_TIMER,
	VIOC_IREQ_RDMA00,
	VIOC_IREQ_RDMA01,
	VIOC_IREQ_RDMA02,
	VIOC_IREQ_RDMA03,
	VIOC_IREQ_RDMA04,
	VIOC_IREQ_RDMA05,
	VIOC_IREQ_RDMA06,
	VIOC_IREQ_RDMA07,
	VIOC_IREQ_RDMA08,
	VIOC_IREQ_RDMA09,
	VIOC_IREQ_RDMA10,
	VIOC_IREQ_RDMA11,
	VIOC_IREQ_RDMA12,
	VIOC_IREQ_RDMA13,
	VIOC_IREQ_RDMA14,
	VIOC_IREQ_RDMA15,
	VIOC_IREQ_RDMA16,
	VIOC_IREQ_RDMA17,
	VIOC_IREQ_RESERVE0,
	VIOC_IREQ_RESERVE1,
	VIOC_IREQ_MMU,
	VIOC_IREQ_RESERVE2,
	VIOC_IREQ_RESERVE3,
	VIOC_IREQ_RESERVE4,
	VIOC_IREQ_FIFO0,
	VIOC_IREQ_FIFO1,
	VIOC_IREQ_RESERVE5,
	VIOC_IREQ_RESERVE6,
	VIOC_IREQ_WDMA00 = 32, 	/* IREQ1 */
	VIOC_IREQ_WDMA01,
	VIOC_IREQ_WDMA02,
	VIOC_IREQ_WDMA03,
	VIOC_IREQ_WDMA04,
	VIOC_IREQ_WDMA05,
	VIOC_IREQ_WDMA06,
	VIOC_IREQ_WDMA07,
	VIOC_IREQ_WDMA08,
	VIOC_IREQ_RESERVE7,
	VIOC_IREQ_RESERVE8,
	VIOC_IREQ_RESERVE9,
	VIOC_IREQ_RESERVE10,
	VIOC_IREQ_RESERVE11,
	VIOC_IREQ_RESERVE12,
	VIOC_IREQ_RESERVE13,
	VIOC_IREQ_WMIX0,
	VIOC_IREQ_WMIX1,
	VIOC_IREQ_WMIX2,
	VIOC_IREQ_WMIX3,
	VIOC_IREQ_WMIX4,
	VIOC_IREQ_WMIX5,
	VIOC_IREQ_RESERVE14,
	VIOC_IREQ_RESERVE15,
	VIOC_IREQ_RESERVE16,
	VIOC_IREQ_RESERVE17,
	VIOC_IREQ_RESERVE18,
	VIOC_IREQ_VIQE,
	VIOC_IREQ_SC0,
	VIOC_IREQ_SC1,
	VIOC_IREQ_SC2,
	VIOC_IREQ_SC3,
	VIOC_IREQ_MAX
} VIOC_IREQ_VECTOR_ID_Type;

#if 0
typedef	struct
{
	unsigned int	DEV0	: 1;
	unsigned int	DEV1	: 1;
	unsigned int	DEV2	: 1;
	unsigned int	TIMER	: 1;
	unsigned int	RDMA00	: 1;
	unsigned int	RDMA01	: 1;
	unsigned int	RDMA02	: 1;
	unsigned int	RDMA03	: 1;
	unsigned int	RDMA04	: 1;
	unsigned int	RDMA05	: 1;
	unsigned int	RDMA06	: 1;
	unsigned int	RDMA07	: 1;
	unsigned int	RDMA08	: 1;
	unsigned int	RDMA09	: 1;
	unsigned int	RDMA10	: 1;
	unsigned int	RDMA11	: 1;
	unsigned int	RDMA12	: 1;
	unsigned int	RDMA13	: 1;
	unsigned int	RDMA14	: 1;
	unsigned int	RDMA15	: 1;
	unsigned int	RDMA16	: 1;
	unsigned int	RDMA17	: 1;
	unsigned int	RESERVE0: 2;
	unsigned int	MMU		: 1;
	unsigned int	RESERVE1: 3;
	unsigned int	FIFO0	: 1;
	unsigned int	FIFO1	: 1;
	unsigned int	RESERVE2: 2;
	unsigned int	WDMA00	: 1;
	unsigned int	WDMA01	: 1;
	unsigned int	WDMA02	: 1;
	unsigned int	WDMA03	: 1;
	unsigned int	WDMA04	: 1;
	unsigned int	WDMA05	: 1;
	unsigned int	WDMA06	: 1;
	unsigned int	WDMA07	: 1;
	unsigned int	WDMA08	: 1;
	unsigned int	RESERVE3: 7;
	unsigned int	WMIX0	: 1;
	unsigned int	WMIX1	: 1;
	unsigned int	WMIX2	: 1;
	unsigned int	WMIX3	: 1;
	unsigned int	WMIX4	: 1;
	unsigned int	WMIX5	: 1;
	unsigned int	RESERVE4: 1;
	unsigned int	VIN00	: 1;
	unsigned int	VIN01	: 1;
	unsigned int	RESERVE5: 2;
	unsigned int	VIQE	: 1;
	unsigned int	SC0		: 1;
	unsigned int	SC1		: 1;
	unsigned int	SC2		: 1;
	unsigned int	SC3		: 1;
}	VIOC_IREQ_IREQ;

typedef	union
{
	unsigned int 		nReg[2];
	VIOC_IREQ_IREQ 	bReg;
} VIOC_IREQ_IREQ_u;

typedef	struct
{
	unsigned int VECTORID 	: 6;
	unsigned int RESERVE 	: 25;
	unsigned int IVALID 	: 1;
} VIOC_IREQ_VECTORID;

typedef	union
{
	unsigned int 		nReg;
	VIOC_IREQ_VECTORID 	bReg;
} VIOC_IREQ_VECTORID_u;

typedef	struct
{
	volatile VIOC_IREQ_IREQ_u 		uRAWSTATUS;		// 0, 1
	volatile VIOC_IREQ_IREQ_u 		uSYNCSTATUS; 	// 2, 3
	volatile VIOC_IREQ_IREQ_u 		uIREQSELECT; 	// 4, 5
	volatile unsigned int 			nIRQMASKSET0; 	// 6
	volatile unsigned int 			nIRQMASKSET1; 	// 7
	volatile unsigned int 			nIRQMASKCLR0; 	// 8
	volatile unsigned int 			nIRQMASKCLR1; 	//9
	volatile VIOC_IREQ_VECTORID_u 	uVECTORID; 		// 10
	volatile unsigned int 			reserved0[5]; 	// 11 ~ 15
} VIOC_IREQ;
#endif

/* Interface APIs */
extern void VIOC_IREQ_DummyHandler(int index, int irq, void *client_data);
extern void VIOC_IREQ_IrqSyncModeSet(unsigned int select);
extern void VIOC_IREQ_IrqAsyncModeSet(unsigned int select);
extern void VIOC_IREQ_IrqMaskSet(unsigned int mask);
#if 1
extern void VIOC_IREQ_IrqMaskClear(unsigned int mask);
#else
extern void VIOC_IREQ_IrqMaskClear(PVIOC_IREQ_CONFIG pIREQConfig, unsigned int mask0, unsigned int mask1);
#endif
extern void VIOC_IREQ_SetInterruptInit(void);
extern int VIOC_IREQ_RegisterHandle(void);

#endif



