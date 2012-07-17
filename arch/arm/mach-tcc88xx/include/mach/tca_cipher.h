/****************************************************************************
 *   FileName    : tca_cipher.h
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
#ifndef _TCA_CIPHER_H_
#define _TCA_CIPHER_H_


#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
*
* Defines
*
******************************************************************************/


/*****************************************************************************
*
* APIs
*
******************************************************************************/
extern void tca_cipher_dma_enable(unsigned uEnable, unsigned uEndian, unsigned uAddrModeTx, unsigned uAddrModeRx);
extern void tca_cipher_dma_enable_request(unsigned uEnable);
extern void tca_cipher_interrupt_config(unsigned uTxSel, unsigned uDoneIrq, unsigned uPacketIrq);
extern void tca_cipher_interrupt_enable(unsigned uEnable);
extern irqreturn_t tca_cipher_interrupt_handler(int irq, void *dev_id);
extern void tca_cipher_set_opmode(unsigned uOpMode);
extern void tca_cipher_set_algorithm(unsigned uAlgorithm, unsigned uArg1, unsigned uArg2);
extern void tca_cipher_set_baseaddr(unsigned uTxRx, unsigned char *pBaseAddr);
extern void tca_cipher_set_packet(unsigned uCount, unsigned uSize);
extern void tca_cipher_set_key(unsigned char *pucData, unsigned uLength, unsigned uOption);
extern void tca_cipher_set_vector(unsigned char *pucData, unsigned uLength);
extern int tca_cipher_get_packetcount(unsigned uTxRx);
extern int tca_cipher_get_blocknum(void);
extern void tca_cipher_clear_counter(unsigned uIndex);
extern void tca_cipher_wait_done(void);
extern int tca_cipher_encrypt(unsigned char *pucSrcAddr, unsigned char *pucDstAddr, unsigned uLength);
extern int tca_cipher_decrypt(unsigned char *pucSrcAddr, unsigned char *pucDstAddr, unsigned uLength);
extern int tca_cipher_open(struct inode *inode, struct file *filp);
extern int tca_cipher_release(struct inode *inode, struct file *file);

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef _TCA_CIPHER_H_  */

