/****************************************************************************
 *   FileName    : tca_sc.h
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
#ifndef _TCA_SC_H_
#define _TCA_SC_H_


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
extern void tca_sc_init_port(void);
extern void tca_sc_init_buffer(unsigned uPort, unsigned uTxRx);
extern void tca_sc_rx_push_char(unsigned uPort, char cChar);
extern unsigned tca_sc_rx_pop_char(unsigned uPort);
extern void tca_sc_make_reset(unsigned uReset);
extern void tca_sc_make_activate(unsigned uActivate);
extern void tca_sc_select_voltage(unsigned uVoloate_3V);
extern unsigned tca_sc_detect_card(void);
extern void tca_sc_ctrl_txport(unsigned uPort, unsigned uEnable);
extern unsigned tca_sc_receive_byte(unsigned int uPort);
extern void tca_sc_send_byte(unsigned int uPort, unsigned uChar);
extern void tca_sc_get_atr(unsigned char *pATRData, unsigned *pATRLength);
extern int tca_sc_parse_atr(unsigned char *pATRData, unsigned uATRLength, sTCC_SC_ATR_CHARS *pstATRParse);
extern int tca_sc_set_factor(sDRV_UART *pstDrvSc, unsigned char ucF, unsigned char ucD);
extern void tca_sc_interrupt_enable(unsigned uPort, unsigned uTxEn, unsigned uRxEn);
extern irqreturn_t tca_sc_interrupt_handler(int irq, void *dev_id);
extern void tca_sc_clock_ctrl(sDRV_UART *pstDrvSc, unsigned uEnable);
extern int tca_sc_config(sDRV_UART *pstDrvSc);
extern int tca_sc_enable(void);
extern int tca_sc_disable(void);
extern int tca_sc_reset(unsigned char *pATR, unsigned *pATRLength);
extern void tca_sc_send_data(char *pData, unsigned uLength);
extern unsigned tca_sc_receive_data(char *pData, unsigned uLength);
extern unsigned tca_sc_receive_data_len(char *pData, unsigned *pLength);
extern int	tca_sc_send_receive(char *pSend, unsigned uSendLength, char *pRcv, unsigned uRcvLength);
extern int	tca_sc_send_receive_len(char *pSend, unsigned uSendLength, char *pRcv, unsigned *pRcvLength);
extern int tca_sc_open(struct inode *inode, struct file *filp);
extern int tca_sc_close(struct inode *inode, struct file *file);
extern int tca_sc_init(void);
extern void tca_sc_exit(void);

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef _TCA_SC_H_  */

