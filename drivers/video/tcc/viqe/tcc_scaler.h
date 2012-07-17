#ifndef __TCC_SCALER_H__
#define __TCC_SCALER_H__

typedef enum
{
	MSCL0,
	MSCL1
} M2M_CHANNEL;

void tcc_scaler_SetInterruptInit(M2M_CHANNEL ch);

void tcc_scaler_Disable(M2M_CHANNEL ch);

void tcc_scaler_Enable(M2M_CHANNEL ch);

void tcc_scaler_SWReset(M2M_CHANNEL ch);

void tcc_scaler_ChangeBaseAddress(int ch, unsigned int pBase0, unsigned int pBase1, unsigned int pBase2);

void tcc_scaler_ControlWithVIQE( M2M_CHANNEL ch,
							unsigned s_y, unsigned s_u, unsigned s_v, unsigned d_y, unsigned d_u, unsigned d_v,
							int s_hsize, int s_vsize, int d_hsize, int d_vsize,
							int crop_top, int crop_bottom, int crop_left, int crop_right);

void tcc_scaler_Control60HzWithVIQE( M2M_CHANNEL ch,
							unsigned s_y, unsigned s_u, unsigned s_v,
							int s_hsize, int s_vsize, int d_hsize, int d_vsize,
							int crop_top, int crop_bottom, int crop_left, int crop_right);

void tcc_scaler_Control60HzScalerOnly( M2M_CHANNEL ch, int DI_use,
							unsigned s_y, unsigned s_u, unsigned s_v,
							int s_hsize, int s_vsize, int d_hsize, int d_vsize,
							int crop_top, int crop_bottom, int crop_left, int crop_right);

void tcc_scaler_Start(M2M_CHANNEL ch);

unsigned int tcc_scaler_finalize(M2M_CHANNEL ch);

int tcc_scaler_interrupt_enable(M2M_CHANNEL ch);

int tcc_scaler_open(M2M_CHANNEL ch);

int tcc_scaler_close(M2M_CHANNEL ch);

#endif // __TCC_SCALER_H__
