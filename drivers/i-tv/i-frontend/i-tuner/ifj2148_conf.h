#ifndef __IFJ2148_CONF_H__
#define __IFJ2148_CONF_H__


/* Standard Preset Definitions: */
#define TDA18273_INSTANCE_CUSTOM_STD_QAM_6MHZ \
    {                                                               /* QAM 6MHz */ \
        3600000,				                        /* IF */ \
        0,                                                          /* CF_Offset */ \
        TDA18273_LPF_6MHz,                                          /* LPF */ \
        TDA18273_LPFOffset_min_4pc,                                 /* LPF_Offset */ \
        TDA18273_DC_Notch_IF_PPF_Enabled,                           /* DC notch IF PPF */ \
        TDA18273_IF_HPF_1MHz,                                   /* Hi Pass */ \
        TDA18273_IF_Notch_Enabled,                                 /* IF notch */ \
        TDA18273_IFnotchToRSSI_Enabled,                             /* IF notch To RSSI */ \
        TDA18273_AGC1_TOP_I2C_DN_UP_d95_u89wdBuV,                   /* AGC1 TOP I2C DN/UP */ \
        TDA18273_AGC1_Adapt_TOP_DN_UP_0_Step,                       /* AGC1 Adapt TOP DN/UP */ \
		TDA18273_AGC1_DN_Time_Constant_8_188ms,                     /* AGC1 DN Time Constant */ \
        TDA18273_AGC1_Mode_LNA_ADAPT_WIDE,                          /* AGC1 mode */ \
        TDA18273_Range_LNA_Adapt_20dB_11dB,                         /* Range LNA Adapt */ \
        TDA18273_LNA_Adapt_RFAGC_Gv_Threshold_18_25dB,              /* LNA Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC1_Top_Adapt_RFAGC_Gv_Threshold_16_75dB,         /* AGC1 TOP Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC2_TOP_DN_UP_d90_u84dBuV,                        /* AGC2 TOP DN/UP */ \
		TDA18273_AGC2_DN_Time_Constant_8_188ms,                     /* AGC2 DN Time Constant */ \
        TDA18273_AGC3_TOP_I2C_100dBuV,                              /* AGC3 TOP I2C Low Band */ \
		TDA18273_AGC3_TOP_I2C_100dBuV,                              /* AGC3 TOP I2C High Band */ \
        TDA18273_AGC4_TOP_DN_UP_d110_u105dBuV,                      /* AGC4 TOP DN/UP */ \
        TDA18273_AGC5_TOP_DN_UP_d110_u105dBuV,                      /* AGC5 TOP DN/UP */ \
        TDA18273_Top_Adapt_NO_TOP_ADAPT,                            /* AGC3 TOP Adapt Algorithm */ \
        TDA18273_AGC3_Adapt_TOP_0_Step,                             /* AGC3 Adapt Low Band */ \
		TDA18273_AGC3_Adapt_TOP_0_Step,                             /* AGC3 Adapt High Band */ \
        TDA18273_AGC_Overload_TOP_plus_9_plus_4_5_min_3_5,          /* AGC Overload TOP */ \
        TDA18273_TH_AGC_Adapt34_5dB,                                /* Adapt TOP 34 Gain Threshold */ \
        TDA18273_RF_Atten_3dB_Disabled,                             /* RF Atten 3dB */ \
        TDA18273_IF_Output_Level_1Vpp_min_6_24dB,                   /* IF Output Level */ \
        TDA18273_S2D_Gain_6dB,                                      /* S2D Gain */ \
        TDA18273_Negative_Modulation_Disabled,                      /* Negative Modulation */ \
        TDA18273_AGCK_Steps_0_2dB,                                  /* Step */ \
        TDA18273_AGCK_Time_Constant_32_768ms,                       /* AGCK Time Constant */ \
        TDA18273_AGC5_HPF_Disabled,                                 /* AGC5 HPF */ \
        TDA18273_Pulse_Shaper_Disable_Enabled,                      /* Pulse Shaper Disable */ \
        TDA18273_VHF_III_Mode_Disabled,                             /* VHF III Mode */ \
        TDA18273_LO_CP_Current_Enabled,                             /* LO CP Current */ \
		TDA18273_PD_Underload_Disabled,                             /* PD Underload */ \
		754000000													/* Frequency Start LTE */ \
    }

#define TDA18273_INSTANCE_CUSTOM_STD_QAM_8MHZ \
    {                                                               /* QAM 8MHz */ \
        5000000,				      				/* IF */ \
        0,                                                          /* CF_Offset */ \
        TDA18273_LPF_9MHz,                                          /* LPF */ \
        TDA18273_LPFOffset_min_8pc,                                 /* LPF_Offset */ \
        TDA18273_DC_Notch_IF_PPF_Enabled,                           /* DC notch IF PPF */ \
        TDA18273_IF_HPF_0_85MHz,                                    /* Hi Pass */ \
        TDA18273_IF_Notch_Disabled,                                 /* IF notch */ \
        TDA18273_IFnotchToRSSI_Disabled,                            /* IF notch To RSSI */ \
        TDA18273_AGC1_TOP_I2C_DN_UP_d95_u89wdBuV,                   /* AGC1 TOP I2C DN/UP */ \
        TDA18273_AGC1_Adapt_TOP_DN_UP_0_Step,                       /* AGC1 Adapt TOP DN/UP */ \
		TDA18273_AGC1_DN_Time_Constant_8_188ms,                     /* AGC1 DN Time Constant */ \
        TDA18273_AGC1_Mode_LNA_ADAPT_WIDE,                          /* AGC1 mode */ \
        TDA18273_Range_LNA_Adapt_20dB_11dB,                         /* Range LNA Adapt */ \
        TDA18273_LNA_Adapt_RFAGC_Gv_Threshold_18_25dB,              /* LNA Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC1_Top_Adapt_RFAGC_Gv_Threshold_16_75dB,         /* AGC1 TOP Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC2_TOP_DN_UP_d90_u84dBuV,                        /* AGC2 TOP DN/UP */ \
		TDA18273_AGC2_DN_Time_Constant_8_188ms,                     /* AGC2 DN Time Constant */ \
        TDA18273_AGC3_TOP_I2C_100dBuV,                              /* AGC3 TOP I2C Low Band */ \
		TDA18273_AGC3_TOP_I2C_100dBuV,                              /* AGC3 TOP I2C High Band */ \
        TDA18273_AGC4_TOP_DN_UP_d110_u105dBuV,                      /* AGC4 TOP DN/UP */ \
        TDA18273_AGC5_TOP_DN_UP_d110_u105dBuV,                      /* AGC5 TOP DN/UP */ \
        TDA18273_Top_Adapt_NO_TOP_ADAPT,                            /* AGC3 TOP Adapt Algorithm */ \
        TDA18273_AGC3_Adapt_TOP_0_Step,                             /* AGC3 Adapt Low Band */ \
		TDA18273_AGC3_Adapt_TOP_0_Step,                             /* AGC3 Adapt High Band */ \
        TDA18273_AGC_Overload_TOP_plus_9_plus_4_5_min_3_5,          /* AGC Overload TOP */ \
        TDA18273_TH_AGC_Adapt34_5dB,                                /* Adapt TOP 34 Gain Threshold */ \
        TDA18273_RF_Atten_3dB_Disabled,                             /* RF Atten 3dB */ \
        TDA18273_IF_Output_Level_1Vpp_min_6_24dB,                   /* IF Output Level */ \
        TDA18273_S2D_Gain_6dB,                                      /* S2D Gain */ \
        TDA18273_Negative_Modulation_Disabled,                      /* Negative Modulation */ \
        TDA18273_AGCK_Steps_0_2dB,                                  /* Step */ \
        TDA18273_AGCK_Time_Constant_32_768ms,                       /* AGCK Time Constant */ \
        TDA18273_AGC5_HPF_Disabled,                                 /* AGC5 HPF */ \
        TDA18273_Pulse_Shaper_Disable_Enabled,                      /* Pulse Shaper Disable */ \
        TDA18273_VHF_III_Mode_Disabled,                             /* VHF III Mode */ \
        TDA18273_LO_CP_Current_Enabled,                             /* LO CP Current */ \
		TDA18273_PD_Underload_Disabled,                             /* PD Underload */ \
		754000000													/* Frequency Start LTE */ \
    }

#define TDA18273_INSTANCE_CUSTOM_STD_ATSC_6MHZ \
    {                                                               /* ATSC */ \
        3250000,								/* IF */ \
        0,                                                          /* CF_Offset */ \
        TDA18273_LPF_6MHz,                                          /* LPF */ \
        TDA18273_LPFOffset_0pc,                                     /* LPF_Offset */ \
        TDA18273_DC_Notch_IF_PPF_Enabled,                           /* DC notch IF PPF */ \
        TDA18273_IF_HPF_0_4MHz,                                     /* Hi Pass */ \
        TDA18273_IF_Notch_Enabled,                                  /* IF notch */ \
        TDA18273_IFnotchToRSSI_Disabled,                            /* IF notch To RSSI */ \
        TDA18273_AGC1_TOP_I2C_DN_UP_d100_u94dBuV,                   /* AGC1 TOP I2C DN/UP */ \
        TDA18273_AGC1_Adapt_TOP_DN_UP_0_Step,                       /* AGC1 Adapt TOP DN/UP */ \
		TDA18273_AGC1_DN_Time_Constant_8_188ms,                     /* AGC1 DN Time Constant */ \
        TDA18273_AGC1_Mode_LNA_ADAPT,                               /* AGC1 mode */ \
        TDA18273_Range_LNA_Adapt_20dB_11dB,                         /* Range LNA Adapt */ \
        TDA18273_LNA_Adapt_RFAGC_Gv_Threshold_18_25dB,              /* LNA Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC1_Top_Adapt_RFAGC_Gv_Threshold_16_75dB,         /* AGC1 TOP Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC2_TOP_DN_UP_d90_u84dBuV,                        /* AGC2 TOP DN/UP */ \
		TDA18273_AGC2_DN_Time_Constant_8_188ms,                     /* AGC2 DN Time Constant */ \
        TDA18273_AGC3_TOP_I2C_104dBuV,                              /* AGC3 TOP I2C Low Band */ \
		TDA18273_AGC3_TOP_I2C_104dBuV,                              /* AGC3 TOP I2C High Band */ \
        TDA18273_AGC4_TOP_DN_UP_d112_u107dBuV,                      /* AGC4 TOP DN/UP */ \
        TDA18273_AGC5_TOP_DN_UP_d112_u107dBuV,                      /* AGC5 TOP DN/UP */ \
        TDA18273_Top_Adapt_TOP_ADAPT35,                             /* AGC3 TOP Adapt Algorithm */ \
        TDA18273_AGC3_Adapt_TOP_3_Step,                             /* AGC3 Adapt Low Band */ \
		TDA18273_AGC3_Adapt_TOP_3_Step,                             /* AGC3 Adapt High Band */ \
        TDA18273_AGC_Overload_TOP_plus_9_plus_4_5_min_3_5,          /* AGC Overload TOP */ \
        TDA18273_TH_AGC_Adapt34_5dB,                                /* Adapt TOP 34 Gain Threshold */ \
        TDA18273_RF_Atten_3dB_Enabled,                              /* RF Atten 3dB */ \
        TDA18273_IF_Output_Level_0_6Vpp_min_10_3_19_7dB,           /* IF Output Level */ \
        TDA18273_S2D_Gain_3dB,                                      /* S2D Gain */ \
        TDA18273_Negative_Modulation_Disabled,                      /* Negative Modulation */ \
        TDA18273_AGCK_Steps_0_2dB,                                  /* Step */ \
        TDA18273_AGCK_Time_Constant_32_768ms,                       /* AGCK Time Constant */ \
        TDA18273_AGC5_HPF_Disabled,                                 /* AGC5 HPF */ \
        TDA18273_Pulse_Shaper_Disable_Enabled,                      /* Pulse Shaper Disable */ \
        TDA18273_VHF_III_Mode_Enabled,                              /* VHF III Mode */ \
        TDA18273_LO_CP_Current_Enabled,                             /* LO CP Current */ \
		TDA18273_PD_Underload_Disabled,                             /* PD Underload */ \
		662000000													/* Frequency Start LTE */ \
    }

#define TDA18273_INSTANCE_CUSTOM_STD_ISDBT_6MHZ \
    {                                                               /* ISDBT */ \
        3250000,                                                    /* IF */ \
        0,                                                          /* CF_Offset */ \
        TDA18273_LPF_6MHz,                                          /* LPF */ \
        TDA18273_LPFOffset_0pc,                                     /* LPF_Offset */ \
        TDA18273_DC_Notch_IF_PPF_Enabled,                           /* DC notch IF PPF */ \
        TDA18273_IF_HPF_0_4MHz,                                     /* Hi Pass */ \
        TDA18273_IF_Notch_Enabled,                                  /* IF notch */ \
        TDA18273_IFnotchToRSSI_Disabled,                            /* IF notch To RSSI */ \
        TDA18273_AGC1_TOP_I2C_DN_UP_d95_u89dBuV,                    /* AGC1 TOP I2C DN/UP */ \
        TDA18273_AGC1_Adapt_TOP_DN_UP_0_Step,                       /* AGC1 Adapt TOP DN/UP */ \
		TDA18273_AGC1_DN_Time_Constant_8_188ms,                     /* AGC1 DN Time Constant */ \
        TDA18273_AGC1_Mode_LNA_ADAPT,                               /* AGC1 mode */ \
        TDA18273_Range_LNA_Adapt_20dB_11dB,                         /* Range LNA Adapt */ \
        TDA18273_LNA_Adapt_RFAGC_Gv_Threshold_18_25dB,              /* LNA Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC1_Top_Adapt_RFAGC_Gv_Threshold_16_75dB,         /* AGC1 TOP Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC2_TOP_DN_UP_d90_u84dBuV,                        /* AGC2 TOP DN/UP */ \
		TDA18273_AGC2_DN_Time_Constant_8_188ms,                     /* AGC2 DN Time Constant */ \
        TDA18273_AGC3_TOP_I2C_100dBuV,                              /* AGC3 TOP I2C Low Band */ \
		TDA18273_AGC3_TOP_I2C_100dBuV,                              /* AGC3 TOP I2C High Band */ \
        TDA18273_AGC4_TOP_DN_UP_d110_u105dBuV,                      /* AGC4 TOP DN/UP */ \
        TDA18273_AGC5_TOP_DN_UP_d110_u105dBuV,                      /* AGC5 TOP DN/UP */ \
        TDA18273_Top_Adapt_TOP_ADAPT35,                             /* AGC3 TOP Adapt Algorithm */ \
        TDA18273_AGC3_Adapt_TOP_2_Step,                             /* AGC3 Adapt Low Band */ \
		TDA18273_AGC3_Adapt_TOP_2_Step,                             /* AGC3 Adapt High Band */ \
        TDA18273_AGC_Overload_TOP_plus_9_plus_4_5_min_3_5,          /* AGC Overload TOP */ \
        TDA18273_TH_AGC_Adapt34_5dB,                                /* Adapt TOP 34 Gain Threshold */ \
        TDA18273_RF_Atten_3dB_Enabled,                              /* RF Atten 3dB */ \
        TDA18273_IF_Output_Level_0_6Vpp_min_10_3_19_7dB,            /* IF Output Level */ \
        TDA18273_S2D_Gain_6dB,                                      /* S2D Gain */ \
        TDA18273_Negative_Modulation_Disabled,                      /* Negative Modulation */ \
        TDA18273_AGCK_Steps_0_2dB,                                  /* Step */ \
        TDA18273_AGCK_Time_Constant_32_768ms,                       /* AGCK Time Constant */ \
        TDA18273_AGC5_HPF_Disabled,                                 /* AGC5 HPF */ \
        TDA18273_Pulse_Shaper_Disable_Enabled,                      /* Pulse Shaper Disable */ \
        TDA18273_VHF_III_Mode_Enabled,                              /* VHF III Mode */ \
        TDA18273_LO_CP_Current_Enabled,                             /* LO CP Current */ \
		TDA18273_PD_Underload_Disabled,                             /* PD Underload */ \
		754000000													/* Frequency Start LTE */ \
    }

#define TDA18273_INSTANCE_CUSTOM_STD_DVBT_1_7MHZ \
    {                                                               /* DVB-T/T2 1.7MHz */ \
        850000,                                                     /* IF */ \
        0,                                                          /* CF_Offset */ \
        TDA18273_LPF_1_5MHz,                                        /* LPF */ \
        TDA18273_LPFOffset_0pc,                                     /* LPF_Offset */ \
        TDA18273_DC_Notch_IF_PPF_Disabled,                          /* DC notch IF PPF */ \
        TDA18273_IF_HPF_Disabled,                                   /* Hi Pass */ \
        TDA18273_IF_Notch_Disabled,                                 /* IF notch */ \
        TDA18273_IFnotchToRSSI_Enabled,                             /* IF notch To RSSI */ \
        TDA18273_AGC1_TOP_I2C_DN_UP_d95_u89dBuV,                    /* AGC1 TOP I2C DN/UP */ \
        TDA18273_AGC1_Adapt_TOP_DN_UP_0_Step,                       /* AGC1 Adapt TOP DN/UP */ \
		TDA18273_AGC1_DN_Time_Constant_4_094ms,                     /* AGC1 DN Time Constant */ \
        TDA18273_AGC1_Mode_LNA_ADAPT,                               /* AGC1 mode */ \
        TDA18273_Range_LNA_Adapt_20dB_11dB,                         /* Range LNA Adapt */ \
        TDA18273_LNA_Adapt_RFAGC_Gv_Threshold_18_25dB,              /* LNA Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC1_Top_Adapt_RFAGC_Gv_Threshold_16_75dB,         /* AGC1 TOP Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC2_TOP_DN_UP_d90_u84dBuV,                        /* AGC2 TOP DN/UP */ \
		TDA18273_AGC2_DN_Time_Constant_2_047ms,                     /* AGC2 DN Time Constant */ \
        TDA18273_AGC3_TOP_I2C_100dBuV,                              /* AGC3 TOP I2C Low Band */ \
		TDA18273_AGC3_TOP_I2C_94dBuV,                               /* AGC3 TOP I2C High Band */ \
        TDA18273_AGC4_TOP_DN_UP_d110_u105dBuV,                      /* AGC4 TOP DN/UP */ \
        TDA18273_AGC5_TOP_DN_UP_d110_u105dBuV,                      /* AGC5 TOP DN/UP */ \
        TDA18273_Top_Adapt_TOP_ADAPT35,                             /* AGC3 TOP Adapt Algorithm */ \
        TDA18273_AGC3_Adapt_TOP_2_Step,                             /* AGC3 Adapt Low Band */ \
		TDA18273_AGC3_Adapt_TOP_0_Step,                             /* AGC3 Adapt High Band */ \
        TDA18273_AGC_Overload_TOP_plus_9_plus_4_5_min_3_5,          /* AGC Overload TOP */ \
        TDA18273_TH_AGC_Adapt34_5dB,                                /* Adapt TOP 34 Gain Threshold */ \
        TDA18273_RF_Atten_3dB_Enabled,                              /* RF Atten 3dB */ \
        TDA18273_IF_Output_Level_1Vpp_min_6_24dB,                   /* IF Output Level */ \
        TDA18273_S2D_Gain_6dB,                                      /* S2D Gain */ \
        TDA18273_Negative_Modulation_Disabled,                      /* Negative Modulation */ \
        TDA18273_AGCK_Steps_0_2dB,                                  /* Step */ \
        TDA18273_AGCK_Time_Constant_32_768ms,                       /* AGCK Time Constant */ \
        TDA18273_AGC5_HPF_Disabled,                                 /* AGC5 HPF */ \
        TDA18273_Pulse_Shaper_Disable_Enabled,                      /* Pulse Shaper Disable */ \
        TDA18273_VHF_III_Mode_Enabled,                              /* VHF III Mode */ \
        TDA18273_LO_CP_Current_Enabled,                             /* LO CP Current */ \
		TDA18273_PD_Underload_Enabled,                              /* PD Underload */ \
		754000000													/* Frequency Start LTE */ \
    }

#define TDA18273_INSTANCE_CUSTOM_STD_DVBT_6MHZ \
    {                                                               /* DVB-T/T2 6MHz */ \
        3250000,                                                    /* IF */ \
        0,                                                          /* CF_Offset */ \
        TDA18273_LPF_6MHz,                                          /* LPF */ \
        TDA18273_LPFOffset_0pc,                                     /* LPF_Offset */ \
        TDA18273_DC_Notch_IF_PPF_Enabled,                           /* DC notch IF PPF */ \
        TDA18273_IF_HPF_0_4MHz,                                     /* Hi Pass */ \
        TDA18273_IF_Notch_Enabled,                                  /* IF notch */ \
        TDA18273_IFnotchToRSSI_Disabled,                            /* IF notch To RSSI */ \
        TDA18273_AGC1_TOP_I2C_DN_UP_d95_u89dBuV,                    /* AGC1 TOP I2C DN/UP */ \
        TDA18273_AGC1_Adapt_TOP_DN_UP_0_Step,                       /* AGC1 Adapt TOP DN/UP */ \
		TDA18273_AGC1_DN_Time_Constant_4_094ms,                     /* AGC1 DN Time Constant */ \
        TDA18273_AGC1_Mode_LNA_ADAPT,                               /* AGC1 mode */ \
        TDA18273_Range_LNA_Adapt_20dB_11dB,                         /* Range LNA Adapt */ \
        TDA18273_LNA_Adapt_RFAGC_Gv_Threshold_18_25dB,              /* LNA Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC1_Top_Adapt_RFAGC_Gv_Threshold_16_75dB,         /* AGC1 TOP Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC2_TOP_DN_UP_d90_u84dBuV,                        /* AGC2 TOP DN/UP */ \
		TDA18273_AGC2_DN_Time_Constant_2_047ms,                     /* AGC2 DN Time Constant */ \
        TDA18273_AGC3_TOP_I2C_100dBuV,                              /* AGC3 TOP I2C Low Band */ \
		TDA18273_AGC3_TOP_I2C_94dBuV,                               /* AGC3 TOP I2C High Band */ \
        TDA18273_AGC4_TOP_DN_UP_d110_u105dBuV,                      /* AGC4 TOP DN/UP */ \
        TDA18273_AGC5_TOP_DN_UP_d110_u105dBuV,                      /* AGC5 TOP DN/UP */ \
        TDA18273_Top_Adapt_TOP_ADAPT35,                             /* AGC3 TOP Adapt Algorithm */ \
        TDA18273_AGC3_Adapt_TOP_2_Step,                             /* AGC3 Adapt Low Band */ \
		TDA18273_AGC3_Adapt_TOP_0_Step,                             /* AGC3 Adapt High Band */ \
        TDA18273_AGC_Overload_TOP_plus_9_plus_4_5_min_3_5,          /* AGC Overload TOP */ \
        TDA18273_TH_AGC_Adapt34_5dB,                                /* Adapt TOP 34 Gain Threshold */ \
        TDA18273_RF_Atten_3dB_Enabled,                              /* RF Atten 3dB */ \
        TDA18273_IF_Output_Level_1Vpp_min_6_24dB,                   /* IF Output Level */ \
        TDA18273_S2D_Gain_6dB,                                      /* S2D Gain */ \
        TDA18273_Negative_Modulation_Disabled,                      /* Negative Modulation */ \
        TDA18273_AGCK_Steps_0_2dB,                                  /* Step */ \
        TDA18273_AGCK_Time_Constant_32_768ms,                       /* AGCK Time Constant */ \
        TDA18273_AGC5_HPF_Disabled,                                 /* AGC5 HPF */ \
        TDA18273_Pulse_Shaper_Disable_Enabled,                      /* Pulse Shaper Disable */ \
        TDA18273_VHF_III_Mode_Enabled,                              /* VHF III Mode */ \
        TDA18273_LO_CP_Current_Enabled,                             /* LO CP Current */ \
		TDA18273_PD_Underload_Enabled,                              /* PD Underload */ \
		754000000													/* Frequency Start LTE */ \
    }

#define TDA18273_INSTANCE_CUSTOM_STD_DVBT_7MHZ \
    {                                                               /* DVB-T/T2 7MHz */ \
        3500000,                                                    /* IF */ \
        0,                                                          /* CF_Offset */ \
        TDA18273_LPF_7MHz,                                          /* LPF */ \
        TDA18273_LPFOffset_min_8pc,                                 /* LPF_Offset */ \
        TDA18273_DC_Notch_IF_PPF_Enabled,                           /* DC notch IF PPF */ \
        TDA18273_IF_HPF_Disabled,                                   /* Hi Pass */ \
        TDA18273_IF_Notch_Enabled,                                  /* IF notch */ \
        TDA18273_IFnotchToRSSI_Disabled,                            /* IF notch To RSSI */ \
        TDA18273_AGC1_TOP_I2C_DN_UP_d95_u89dBuV,                    /* AGC1 TOP I2C DN/UP */ \
        TDA18273_AGC1_Adapt_TOP_DN_UP_0_Step,                       /* AGC1 Adapt TOP DN/UP */ \
		TDA18273_AGC1_DN_Time_Constant_4_094ms,                     /* AGC1 DN Time Constant */ \
        TDA18273_AGC1_Mode_LNA_ADAPT,                               /* AGC1 mode */ \
        TDA18273_Range_LNA_Adapt_20dB_11dB,                         /* Range LNA Adapt */ \
        TDA18273_LNA_Adapt_RFAGC_Gv_Threshold_18_25dB,              /* LNA Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC1_Top_Adapt_RFAGC_Gv_Threshold_16_75dB,         /* AGC1 TOP Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC2_TOP_DN_UP_d90_u84dBuV,                        /* AGC2 TOP DN/UP */ \
		TDA18273_AGC2_DN_Time_Constant_2_047ms,                     /* AGC2 DN Time Constant */ \
        TDA18273_AGC3_TOP_I2C_100dBuV,                              /* AGC3 TOP I2C Low Band */ \
		TDA18273_AGC3_TOP_I2C_94dBuV,                               /* AGC3 TOP I2C High Band */ \
        TDA18273_AGC4_TOP_DN_UP_d110_u105dBuV,                      /* AGC4 TOP DN/UP */ \
        TDA18273_AGC5_TOP_DN_UP_d110_u105dBuV,                      /* AGC5 TOP DN/UP */ \
        TDA18273_Top_Adapt_TOP_ADAPT35,                             /* AGC3 TOP Adapt Algorithm */ \
        TDA18273_AGC3_Adapt_TOP_2_Step,                             /* AGC3 Adapt Low Band */ \
		TDA18273_AGC3_Adapt_TOP_0_Step,                             /* AGC3 Adapt High Band */ \
        TDA18273_AGC_Overload_TOP_plus_9_plus_4_5_min_3_5,          /* AGC Overload TOP */ \
        TDA18273_TH_AGC_Adapt34_5dB,                                /* Adapt TOP 34 Gain Threshold */ \
        TDA18273_RF_Atten_3dB_Enabled,                              /* RF Atten 3dB */ \
        TDA18273_IF_Output_Level_1Vpp_min_6_24dB,                   /* IF Output Level */ \
        TDA18273_S2D_Gain_6dB,                                      /* S2D Gain */ \
        TDA18273_Negative_Modulation_Disabled,                      /* Negative Modulation */ \
        TDA18273_AGCK_Steps_0_2dB,                                  /* Step */ \
        TDA18273_AGCK_Time_Constant_32_768ms,                       /* AGCK Time Constant */ \
        TDA18273_AGC5_HPF_Disabled,                                 /* AGC5 HPF */ \
        TDA18273_Pulse_Shaper_Disable_Enabled,                      /* Pulse Shaper Disable */ \
        TDA18273_VHF_III_Mode_Enabled,                              /* VHF III Mode */ \
        TDA18273_LO_CP_Current_Enabled,                             /* LO CP Current */ \
		TDA18273_PD_Underload_Enabled,                              /* PD Underload */ \
		754000000													/* Frequency Start LTE */ \
    }

#define TDA18273_INSTANCE_CUSTOM_STD_DVBT_8MHZ \
    {                                                               /* DVB-T/T2 8MHz */ \
        4000000,                                                    /* IF */ \
        0,                                                          /* CF_Offset */ \
        TDA18273_LPF_8MHz,                                          /* LPF */ \
        TDA18273_LPFOffset_0pc,                                     /* LPF_Offset */ \
        TDA18273_DC_Notch_IF_PPF_Enabled,                           /* DC notch IF PPF */ \
        TDA18273_IF_HPF_Disabled,                                   /* Hi Pass */ \
        TDA18273_IF_Notch_Enabled,                                  /* IF notch */ \
        TDA18273_IFnotchToRSSI_Disabled,                            /* IF notch To RSSI */ \
        TDA18273_AGC1_TOP_I2C_DN_UP_d95_u89dBuV,                    /* AGC1 TOP I2C DN/UP */ \
        TDA18273_AGC1_Adapt_TOP_DN_UP_0_Step,                       /* AGC1 Adapt TOP DN/UP */ \
		TDA18273_AGC1_DN_Time_Constant_4_094ms,                     /* AGC1 DN Time Constant */ \
        TDA18273_AGC1_Mode_LNA_ADAPT,                               /* AGC1 mode */ \
        TDA18273_Range_LNA_Adapt_20dB_11dB,                         /* Range LNA Adapt */ \
        TDA18273_LNA_Adapt_RFAGC_Gv_Threshold_18_25dB,              /* LNA Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC1_Top_Adapt_RFAGC_Gv_Threshold_16_75dB,         /* AGC1 TOP Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC2_TOP_DN_UP_d90_u84dBuV,                        /* AGC2 TOP DN/UP */ \
		TDA18273_AGC2_DN_Time_Constant_2_047ms,                     /* AGC2 DN Time Constant */ \
        TDA18273_AGC3_TOP_I2C_100dBuV,                              /* AGC3 TOP I2C Low Band */ \
		TDA18273_AGC3_TOP_I2C_94dBuV,                               /* AGC3 TOP I2C High Band */ \
        TDA18273_AGC4_TOP_DN_UP_d110_u105dBuV,                      /* AGC4 TOP DN/UP */ \
        TDA18273_AGC5_TOP_DN_UP_d110_u105dBuV,                      /* AGC5 TOP DN/UP */ \
        TDA18273_Top_Adapt_TOP_ADAPT35,                             /* AGC3 TOP Adapt Algorithm */ \
        TDA18273_AGC3_Adapt_TOP_2_Step,                             /* AGC3 Adapt Low Band */ \
		TDA18273_AGC3_Adapt_TOP_0_Step,                             /* AGC3 Adapt High Band */ \
        TDA18273_AGC_Overload_TOP_plus_9_plus_4_5_min_3_5,          /* AGC Overload TOP */ \
        TDA18273_TH_AGC_Adapt34_5dB,                                /* Adapt TOP 34 Gain Threshold */ \
        TDA18273_RF_Atten_3dB_Enabled,                              /* RF Atten 3dB */ \
        TDA18273_IF_Output_Level_1Vpp_min_6_24dB,                   /* IF Output Level */ \
        TDA18273_S2D_Gain_6dB,                                      /* S2D Gain */ \
        TDA18273_Negative_Modulation_Disabled,                      /* Negative Modulation */ \
        TDA18273_AGCK_Steps_0_2dB,                                  /* Step */ \
        TDA18273_AGCK_Time_Constant_32_768ms,                       /* AGCK Time Constant */ \
        TDA18273_AGC5_HPF_Disabled,                                 /* AGC5 HPF */ \
        TDA18273_Pulse_Shaper_Disable_Enabled,                      /* Pulse Shaper Disable */ \
        TDA18273_VHF_III_Mode_Enabled,                              /* VHF III Mode */ \
        TDA18273_LO_CP_Current_Enabled,                             /* LO CP Current */ \
		TDA18273_PD_Underload_Enabled,                              /* PD Underload */ \
		754000000													/* Frequency Start LTE */ \
    }

#define TDA18273_INSTANCE_CUSTOM_STD_DVBT_10MHZ \
    {                                                               /* DVB-T/T2 10MHz */ \
        5000000,                                                    /* IF */ \
        0,                                                          /* CF_Offset */ \
        TDA18273_LPF_9MHz,                                          /* LPF */ \
        TDA18273_LPFOffset_0pc,                                     /* LPF_Offset */ \
        TDA18273_DC_Notch_IF_PPF_Enabled,                           /* DC notch IF PPF */ \
        TDA18273_IF_HPF_Disabled,                                   /* Hi Pass */ \
        TDA18273_IF_Notch_Disabled,                                 /* IF notch */ \
        TDA18273_IFnotchToRSSI_Enabled,                             /* IF notch To RSSI */ \
        TDA18273_AGC1_TOP_I2C_DN_UP_d95_u89dBuV,                    /* AGC1 TOP I2C DN/UP */ \
        TDA18273_AGC1_Adapt_TOP_DN_UP_0_Step,                       /* AGC1 Adapt TOP DN/UP */ \
		TDA18273_AGC1_DN_Time_Constant_8_188ms,                     /* AGC1 DN Time Constant */ \
        TDA18273_AGC1_Mode_LNA_ADAPT,                               /* AGC1 mode */ \
        TDA18273_Range_LNA_Adapt_20dB_11dB,                         /* Range LNA Adapt */ \
        TDA18273_LNA_Adapt_RFAGC_Gv_Threshold_18_25dB,              /* LNA Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC1_Top_Adapt_RFAGC_Gv_Threshold_16_75dB,         /* AGC1 TOP Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC2_TOP_DN_UP_d90_u84dBuV,                        /* AGC2 TOP DN/UP */ \
		TDA18273_AGC2_DN_Time_Constant_8_188ms,                     /* AGC2 DN Time Constant */ \
        TDA18273_AGC3_TOP_I2C_100dBuV,                              /* AGC3 TOP I2C Low Band */ \
		TDA18273_AGC3_TOP_I2C_100dBuV,                              /* AGC3 TOP I2C High Band */ \
        TDA18273_AGC4_TOP_DN_UP_d110_u105dBuV,                      /* AGC4 TOP DN/UP */ \
        TDA18273_AGC5_TOP_DN_UP_d110_u105dBuV,                      /* AGC5 TOP DN/UP */ \
        TDA18273_Top_Adapt_TOP_ADAPT35,                             /* AGC3 TOP Adapt Algorithm */ \
        TDA18273_AGC3_Adapt_TOP_2_Step,                             /* AGC3 Adapt Low Band */ \
		TDA18273_AGC3_Adapt_TOP_2_Step,                             /* AGC3 Adapt High Band */ \
        TDA18273_AGC_Overload_TOP_plus_9_plus_4_5_min_3_5,          /* AGC Overload TOP */ \
        TDA18273_TH_AGC_Adapt34_5dB,                                /* Adapt TOP 34 Gain Threshold */ \
        TDA18273_RF_Atten_3dB_Enabled,                              /* RF Atten 3dB */ \
        TDA18273_IF_Output_Level_1Vpp_min_6_24dB,                   /* IF Output Level */ \
        TDA18273_S2D_Gain_6dB,                                      /* S2D Gain */ \
        TDA18273_Negative_Modulation_Disabled,                      /* Negative Modulation */ \
        TDA18273_AGCK_Steps_0_2dB,                                  /* Step */ \
        TDA18273_AGCK_Time_Constant_32_768ms,                       /* AGCK Time Constant */ \
        TDA18273_AGC5_HPF_Disabled,                                 /* AGC5 HPF */ \
        TDA18273_Pulse_Shaper_Disable_Enabled,                      /* Pulse Shaper Disable */ \
        TDA18273_VHF_III_Mode_Enabled,                              /* VHF III Mode */ \
        TDA18273_LO_CP_Current_Enabled,                             /* LO CP Current */ \
		TDA18273_PD_Underload_Disabled,                             /* PD Underload */ \
		754000000													/* Frequency Start LTE */ \
    }

#define TDA18273_INSTANCE_CUSTOM_STD_DMBT_8MHZ \
    {                                                               /* DMB-T */ \
        4000000,                                                    /* IF */ \
        0,                                                          /* CF_Offset */ \
        TDA18273_LPF_8MHz,                                          /* LPF */ \
        TDA18273_LPFOffset_0pc,                                     /* LPF_Offset */ \
        TDA18273_DC_Notch_IF_PPF_Enabled,                           /* DC notch IF PPF */ \
        TDA18273_IF_HPF_Disabled,                                   /* Hi Pass */ \
        TDA18273_IF_Notch_Enabled,                                  /* IF notch */ \
        TDA18273_IFnotchToRSSI_Disabled,                            /* IF notch To RSSI */ \
        TDA18273_AGC1_TOP_I2C_DN_UP_d95_u89dBuV,                    /* AGC1 TOP I2C DN/UP */ \
        TDA18273_AGC1_Adapt_TOP_DN_UP_0_Step,                       /* AGC1 Adapt TOP DN/UP */ \
		TDA18273_AGC1_DN_Time_Constant_8_188ms,                     /* AGC1 DN Time Constant */ \
        TDA18273_AGC1_Mode_LNA_ADAPT,                               /* AGC1 mode */ \
        TDA18273_Range_LNA_Adapt_20dB_11dB,                         /* Range LNA Adapt */ \
        TDA18273_LNA_Adapt_RFAGC_Gv_Threshold_18_25dB,              /* LNA Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC1_Top_Adapt_RFAGC_Gv_Threshold_16_75dB,         /* AGC1 TOP Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC2_TOP_DN_UP_d90_u84dBuV,                        /* AGC2 TOP DN/UP */ \
		TDA18273_AGC2_DN_Time_Constant_8_188ms,                     /* AGC2 DN Time Constant */ \
        TDA18273_AGC3_TOP_I2C_100dBuV,                              /* AGC3 TOP I2C Low Band */ \
		TDA18273_AGC3_TOP_I2C_100dBuV,                              /* AGC3 TOP I2C High Band */ \
        TDA18273_AGC4_TOP_DN_UP_d110_u105dBuV,                      /* AGC4 TOP DN/UP */ \
        TDA18273_AGC5_TOP_DN_UP_d110_u105dBuV,                      /* AGC5 TOP DN/UP */ \
        TDA18273_Top_Adapt_TOP_ADAPT35,                             /* AGC3 TOP Adapt Algorithm */ \
        TDA18273_AGC3_Adapt_TOP_2_Step,                             /* AGC3 Adapt Low Band */ \
		TDA18273_AGC3_Adapt_TOP_2_Step,                             /* AGC3 Adapt High Band */ \
        TDA18273_AGC_Overload_TOP_plus_9_plus_4_5_min_3_5,          /* AGC Overload TOP */ \
        TDA18273_TH_AGC_Adapt34_5dB,                                /* Adapt TOP 34 Gain Threshold */ \
        TDA18273_RF_Atten_3dB_Enabled,                              /* RF Atten 3dB */ \
        TDA18273_IF_Output_Level_1Vpp_min_6_24dB,                   /* IF Output Level */ \
        TDA18273_S2D_Gain_6dB,                                      /* S2D Gain */ \
        TDA18273_Negative_Modulation_Disabled,                      /* Negative Modulation */ \
        TDA18273_AGCK_Steps_0_2dB,                                  /* Step */ \
        TDA18273_AGCK_Time_Constant_32_768ms,                       /* AGCK Time Constant */ \
        TDA18273_AGC5_HPF_Disabled,                                 /* AGC5 HPF */ \
        TDA18273_Pulse_Shaper_Disable_Enabled,                      /* Pulse Shaper Disable */ \
        TDA18273_VHF_III_Mode_Enabled,                              /* VHF III Mode */ \
        TDA18273_LO_CP_Current_Enabled,                             /* LO CP Current */ \
		TDA18273_PD_Underload_Disabled,                             /* PD Underload */ \
		754000000													/* Frequency Start LTE */ \
    }

#define TDA18273_INSTANCE_CUSTOM_STD_ANALOG_FM_RADIO \
    {                                                               /* FM */ \
        1250000,                                                    /* IF */ \
        0,                                                          /* CF_Offset */ \
        TDA18273_LPF_1_5MHz,                                        /* LPF */ \
        TDA18273_LPFOffset_0pc,                                     /* LPF_Offset */ \
        TDA18273_DC_Notch_IF_PPF_Enabled,                           /* DC notch IF PPF */ \
        TDA18273_IF_HPF_0_85MHz,                                    /* Hi Pass */ \
        TDA18273_IF_Notch_Disabled,                                 /* IF notch */ \
        TDA18273_IFnotchToRSSI_Disabled,                            /* IF notch To RSSI */ \
        TDA18273_AGC1_TOP_I2C_DN_UP_d95_u89dBuV,                    /* AGC1 TOP I2C DN/UP */ \
        TDA18273_AGC1_Adapt_TOP_DN_UP_1_Step,                       /* AGC1 Adapt TOP DN/UP */ \
		TDA18273_AGC1_DN_Time_Constant_8_188ms,                     /* AGC1 DN Time Constant */ \
        TDA18273_AGC1_Mode_LNA_ADAPT_TOP_ADAPT,                     /* AGC1 mode */ \
        TDA18273_Range_LNA_Adapt_20dB_11dB,                         /* Range LNA Adapt */ \
        TDA18273_LNA_Adapt_RFAGC_Gv_Threshold_18_25dB,              /* LNA Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC1_Top_Adapt_RFAGC_Gv_Threshold_15_25dB,         /* AGC1 TOP Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC2_TOP_DN_UP_d90_u84dBuV,                        /* AGC2 TOP DN/UP */ \
		TDA18273_AGC2_DN_Time_Constant_8_188ms,                     /* AGC2 DN Time Constant */ \
        TDA18273_AGC3_TOP_I2C_96dBuV,                               /* AGC3 TOP I2C Low Band */ \
		TDA18273_AGC3_TOP_I2C_96dBuV,                               /* AGC3 TOP I2C High Band */ \
        TDA18273_AGC4_TOP_DN_UP_d105_u100dBuV,                      /* AGC4 TOP DN/UP */ \
        TDA18273_AGC5_TOP_DN_UP_d105_u100dBuV,                      /* AGC5 TOP DN/UP */ \
        TDA18273_Top_Adapt_TOP_ADAPT34,                             /* AGC3 TOP Adapt Algorithm */ \
        TDA18273_AGC3_Adapt_TOP_1_Step,                             /* AGC3 Adapt Low Band */ \
		TDA18273_AGC3_Adapt_TOP_1_Step,                             /* AGC3 Adapt High Band */ \
        TDA18273_AGC_Overload_TOP_plus_9_plus_4_5_min_3_5,          /* AGC Overload TOP */ \
        TDA18273_TH_AGC_Adapt34_5dB,                                /* Adapt TOP 34 Gain Threshold */ \
        TDA18273_RF_Atten_3dB_Disabled,                             /* RF Atten 3dB */ \
        TDA18273_IF_Output_Level_0_8Vpp_min_8_22dB,                 /* IF Output Level */ \
        TDA18273_S2D_Gain_6dB,                                      /* S2D Gain */ \
        TDA18273_Negative_Modulation_Disabled,                      /* Negative Modulation */ \
        TDA18273_AGCK_Steps_0_2dB,                                  /* Step */ \
        TDA18273_AGCK_Time_Constant_32_768ms,                       /* AGCK Time Constant */ \
        TDA18273_AGC5_HPF_Disabled,                                 /* AGC5 HPF */ \
        TDA18273_Pulse_Shaper_Disable_Enabled,                      /* Pulse Shaper Disable */ \
        TDA18273_VHF_III_Mode_Disabled,                             /* VHF III Mode */ \
        TDA18273_LO_CP_Current_Enabled,                             /* LO CP Current */ \
		TDA18273_PD_Underload_Disabled,                             /* PD Underload */ \
		754000000													/* Frequency Start LTE */ \
    }

#define TDA18273_INSTANCE_CUSTOM_STD_ANALOG_MN \
    {                                                               /* NTSC M/N */ \
        5400000,                                                    /* IF */ \
        1750000,                                                    /* CF_Offset */ \
        TDA18273_LPF_6MHz,                                          /* LPF */ \
        TDA18273_LPFOffset_0pc,                                     /* LPF_Offset */ \
        TDA18273_DC_Notch_IF_PPF_Disabled,                          /* DC notch IF PPF */ \
        TDA18273_IF_HPF_Disabled,                                   /* Hi Pass */ \
        TDA18273_IF_Notch_Disabled,                                 /* IF notch */ \
        TDA18273_IFnotchToRSSI_Enabled,                             /* IF notch To RSSI */ \
        TDA18273_AGC1_TOP_I2C_DN_UP_d95_u89dBuV,                    /* AGC1 TOP I2C DN/UP */ \
        TDA18273_AGC1_Adapt_TOP_DN_UP_1_Step,                       /* AGC1 Adapt TOP DN/UP */ \
		TDA18273_AGC1_DN_Time_Constant_8_188ms,                     /* AGC1 DN Time Constant */ \
        TDA18273_AGC1_Mode_LNA_ADAPT_TOP_ADAPT,                     /* AGC1 mode */ \
        TDA18273_Range_LNA_Adapt_20dB_11dB,                         /* Range LNA Adapt */ \
        TDA18273_LNA_Adapt_RFAGC_Gv_Threshold_18_25dB,              /* LNA Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC1_Top_Adapt_RFAGC_Gv_Threshold_15_25dB,         /* AGC1 TOP Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC2_TOP_DN_UP_d90_u84dBuV,                        /* AGC2 TOP DN/UP */ \
		TDA18273_AGC2_DN_Time_Constant_8_188ms,                     /* AGC2 DN Time Constant */ \
        TDA18273_AGC3_TOP_I2C_96dBuV,                               /* AGC3 TOP I2C Low Band */ \
		TDA18273_AGC3_TOP_I2C_96dBuV,                               /* AGC3 TOP I2C High Band */ \
        TDA18273_AGC4_TOP_DN_UP_d105_u100dBuV,                      /* AGC4 TOP DN/UP */ \
        TDA18273_AGC5_TOP_DN_UP_d105_u100dBuV,                      /* AGC5 TOP DN/UP */ \
        TDA18273_Top_Adapt_TOP_ADAPT34,                             /* AGC3 TOP Adapt Algorithm */ \
        TDA18273_AGC3_Adapt_TOP_1_Step,                             /* AGC3 Adapt Low Band */ \
		TDA18273_AGC3_Adapt_TOP_1_Step,                             /* AGC3 Adapt High Band */ \
        TDA18273_AGC_Overload_TOP_plus_9_plus_4_5_min_3_5,          /* AGC Overload TOP */ \
        TDA18273_TH_AGC_Adapt34_5dB,                                /* Adapt TOP 34 Gain Threshold */ \
        TDA18273_RF_Atten_3dB_Disabled,                             /* RF Atten 3dB */ \
        TDA18273_IF_Output_Level_0_7Vpp_min_9_21dB,                 /* IF Output Level */ \
        TDA18273_S2D_Gain_6dB,                                      /* S2D Gain */ \
        TDA18273_Negative_Modulation_Enabled,                       /* Negative Modulation */ \
        TDA18273_AGCK_Steps_0_2dB,                                  /* Step */ \
        TDA18273_AGCK_Time_Constant_1_STEP_EACH_VSYNC_RISING_EDGE,  /* AGCK Time Constant */ \
        TDA18273_AGC5_HPF_Enabled,                                  /* AGC5 HPF */ \
        TDA18273_Pulse_Shaper_Disable_Enabled,                      /* Pulse Shaper Disable */ \
        TDA18273_VHF_III_Mode_Disabled,                             /* VHF III Mode */ \
        TDA18273_LO_CP_Current_Enabled,                             /* LO CP Current */ \
		TDA18273_PD_Underload_Disabled,                             /* PD Underload */ \
		754000000													/* Frequency Start LTE */ \
    }

#define TDA18273_INSTANCE_CUSTOM_STD_ANALOG_B \
    {                                                               /* PAL B */ \
        6400000,                                                    /* IF */ \
        2250000,                                                    /* CF_Offset */ \
        TDA18273_LPF_7MHz,                                          /* LPF */ \
        TDA18273_LPFOffset_0pc,                                     /* LPF_Offset */ \
        TDA18273_DC_Notch_IF_PPF_Disabled,                          /* DC notch IF PPF */ \
        TDA18273_IF_HPF_Disabled,                                   /* Hi Pass */ \
        TDA18273_IF_Notch_Disabled,                                 /* IF notch */ \
        TDA18273_IFnotchToRSSI_Enabled,                             /* IF notch To RSSI */ \
        TDA18273_AGC1_TOP_I2C_DN_UP_d95_u89dBuV,                    /* AGC1 TOP I2C DN/UP */ \
        TDA18273_AGC1_Adapt_TOP_DN_UP_1_Step,                       /* AGC1 Adapt TOP DN/UP */ \
		TDA18273_AGC1_DN_Time_Constant_8_188ms,                     /* AGC1 DN Time Constant */ \
        TDA18273_AGC1_Mode_LNA_ADAPT_TOP_ADAPT,                     /* AGC1 mode */ \
        TDA18273_Range_LNA_Adapt_20dB_11dB,                         /* Range LNA Adapt */ \
        TDA18273_LNA_Adapt_RFAGC_Gv_Threshold_18_25dB,              /* LNA Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC1_Top_Adapt_RFAGC_Gv_Threshold_15_25dB,         /* AGC1 TOP Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC2_TOP_DN_UP_d90_u84dBuV,                        /* AGC2 TOP DN/UP */ \
		TDA18273_AGC2_DN_Time_Constant_8_188ms,                     /* AGC2 DN Time Constant */ \
        TDA18273_AGC3_TOP_I2C_96dBuV,                               /* AGC3 TOP I2C Low Band */ \
		TDA18273_AGC3_TOP_I2C_96dBuV,                               /* AGC3 TOP I2C High Band */ \
        TDA18273_AGC4_TOP_DN_UP_d105_u100dBuV,                      /* AGC4 TOP DN/UP */ \
        TDA18273_AGC5_TOP_DN_UP_d105_u100dBuV,                      /* AGC5 TOP DN/UP */ \
        TDA18273_Top_Adapt_TOP_ADAPT34,                             /* AGC3 TOP Adapt Algorithm */ \
        TDA18273_AGC3_Adapt_TOP_1_Step,                             /* AGC3 Adapt Low Band */ \
		TDA18273_AGC3_Adapt_TOP_1_Step,                             /* AGC3 Adapt High Band */ \
        TDA18273_AGC_Overload_TOP_plus_9_plus_4_5_min_3_5,          /* AGC Overload TOP */ \
        TDA18273_TH_AGC_Adapt34_5dB,                                /* Adapt TOP 34 Gain Threshold */ \
        TDA18273_RF_Atten_3dB_Disabled,                             /* RF Atten 3dB */ \
        TDA18273_IF_Output_Level_0_7Vpp_min_9_21dB,                 /* IF Output Level */ \
        TDA18273_S2D_Gain_6dB,                                      /* S2D Gain */ \
        TDA18273_Negative_Modulation_Enabled,                       /* Negative Modulation */ \
        TDA18273_AGCK_Steps_0_2dB,                                  /* Step */ \
        TDA18273_AGCK_Time_Constant_1_STEP_EACH_VSYNC_RISING_EDGE,  /* AGCK Time Constant */ \
        TDA18273_AGC5_HPF_Enabled,                                  /* AGC5 HPF */ \
        TDA18273_Pulse_Shaper_Disable_Enabled,                      /* Pulse Shaper Disable */ \
        TDA18273_VHF_III_Mode_Disabled,                             /* VHF III Mode */ \
        TDA18273_LO_CP_Current_Enabled,                             /* LO CP Current */ \
		TDA18273_PD_Underload_Disabled,                             /* PD Underload */ \
		754000000													/* Frequency Start LTE */ \
    }

#define TDA18273_INSTANCE_CUSTOM_STD_ANALOG_GH \
    {                                                               /* PAL G/H */ \
        6750000,                                                    /* IF */ \
        2750000,                                                    /* CF_Offset */ \
        TDA18273_LPF_8MHz,                                          /* LPF */ \
        TDA18273_LPFOffset_0pc,                                     /* LPF_Offset */ \
        TDA18273_DC_Notch_IF_PPF_Disabled,                          /* DC notch IF PPF */ \
        TDA18273_IF_HPF_Disabled,                                   /* Hi Pass */ \
        TDA18273_IF_Notch_Disabled,                                 /* IF notch */ \
        TDA18273_IFnotchToRSSI_Enabled,                             /* IF notch To RSSI */ \
        TDA18273_AGC1_TOP_I2C_DN_UP_d95_u89dBuV,                    /* AGC1 TOP I2C DN/UP */ \
        TDA18273_AGC1_Adapt_TOP_DN_UP_1_Step,                       /* AGC1 Adapt TOP DN/UP */ \
		TDA18273_AGC1_DN_Time_Constant_8_188ms,                     /* AGC1 DN Time Constant */ \
        TDA18273_AGC1_Mode_LNA_ADAPT_TOP_ADAPT,                     /* AGC1 mode */ \
        TDA18273_Range_LNA_Adapt_20dB_11dB,                         /* Range LNA Adapt */ \
        TDA18273_LNA_Adapt_RFAGC_Gv_Threshold_18_25dB,              /* LNA Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC1_Top_Adapt_RFAGC_Gv_Threshold_15_25dB,         /* AGC1 TOP Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC2_TOP_DN_UP_d90_u84dBuV,                        /* AGC2 TOP DN/UP */ \
		TDA18273_AGC2_DN_Time_Constant_8_188ms,                     /* AGC2 DN Time Constant */ \
        TDA18273_AGC3_TOP_I2C_96dBuV,                               /* AGC3 TOP I2C Low Band */ \
		TDA18273_AGC3_TOP_I2C_96dBuV,                               /* AGC3 TOP I2C High Band */ \
        TDA18273_AGC4_TOP_DN_UP_d105_u100dBuV,                      /* AGC4 TOP DN/UP */ \
        TDA18273_AGC5_TOP_DN_UP_d105_u100dBuV,                      /* AGC5 TOP DN/UP */ \
        TDA18273_Top_Adapt_TOP_ADAPT34,                             /* AGC3 TOP Adapt Algorithm */ \
        TDA18273_AGC3_Adapt_TOP_1_Step,                             /* AGC3 Adapt Low Band */ \
		TDA18273_AGC3_Adapt_TOP_1_Step,                             /* AGC3 Adapt High Band */ \
        TDA18273_AGC_Overload_TOP_plus_9_plus_4_5_min_3_5,          /* AGC Overload TOP */ \
        TDA18273_TH_AGC_Adapt34_5dB,                                /* Adapt TOP 34 Gain Threshold */ \
        TDA18273_RF_Atten_3dB_Disabled,                             /* RF Atten 3dB */ \
        TDA18273_IF_Output_Level_0_7Vpp_min_9_21dB,                 /* IF Output Level */ \
        TDA18273_S2D_Gain_6dB,                                      /* S2D Gain */ \
        TDA18273_Negative_Modulation_Enabled,                       /* Negative Modulation */ \
        TDA18273_AGCK_Steps_0_2dB,                                  /* Step */ \
        TDA18273_AGCK_Time_Constant_1_STEP_EACH_VSYNC_RISING_EDGE,  /* AGCK Time Constant */ \
        TDA18273_AGC5_HPF_Enabled,                                  /* AGC5 HPF */ \
        TDA18273_Pulse_Shaper_Disable_Enabled,                      /* Pulse Shaper Disable */ \
        TDA18273_VHF_III_Mode_Disabled,                             /* VHF III Mode */ \
        TDA18273_LO_CP_Current_Enabled,                             /* LO CP Current */ \
		TDA18273_PD_Underload_Disabled,                             /* PD Underload */ \
		754000000													/* Frequency Start LTE */ \
    }

#define TDA18273_INSTANCE_CUSTOM_STD_ANALOG_I \
    {                                                               /* PAL I */ \
        7250000,                                                    /* IF */ \
        2750000,                                                    /* CF_Offset */ \
        TDA18273_LPF_8MHz,                                          /* LPF */ \
        TDA18273_LPFOffset_0pc,                                     /* LPF_Offset */ \
        TDA18273_DC_Notch_IF_PPF_Disabled,                          /* DC notch IF PPF */ \
        TDA18273_IF_HPF_Disabled,                                   /* Hi Pass */ \
        TDA18273_IF_Notch_Disabled,                                 /* IF notch */ \
        TDA18273_IFnotchToRSSI_Enabled,                             /* IF notch To RSSI */ \
        TDA18273_AGC1_TOP_I2C_DN_UP_d95_u89dBuV,                    /* AGC1 TOP I2C DN/UP */ \
        TDA18273_AGC1_Adapt_TOP_DN_UP_1_Step,                       /* AGC1 Adapt TOP DN/UP */ \
		TDA18273_AGC1_DN_Time_Constant_8_188ms,                     /* AGC1 DN Time Constant */ \
        TDA18273_AGC1_Mode_LNA_ADAPT_TOP_ADAPT,                     /* AGC1 mode */ \
        TDA18273_Range_LNA_Adapt_20dB_11dB,                         /* Range LNA Adapt */ \
        TDA18273_LNA_Adapt_RFAGC_Gv_Threshold_18_25dB,              /* LNA Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC1_Top_Adapt_RFAGC_Gv_Threshold_15_25dB,         /* AGC1 TOP Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC2_TOP_DN_UP_d90_u84dBuV,                        /* AGC2 TOP DN/UP */ \
		TDA18273_AGC2_DN_Time_Constant_8_188ms,                     /* AGC2 DN Time Constant */ \
        TDA18273_AGC3_TOP_I2C_96dBuV,                               /* AGC3 TOP I2C Low Band */ \
		TDA18273_AGC3_TOP_I2C_96dBuV,                               /* AGC3 TOP I2C High Band */ \
        TDA18273_AGC4_TOP_DN_UP_d105_u100dBuV,                      /* AGC4 TOP DN/UP */ \
        TDA18273_AGC5_TOP_DN_UP_d105_u100dBuV,                      /* AGC5 TOP DN/UP */ \
        TDA18273_Top_Adapt_TOP_ADAPT34,                             /* AGC3 TOP Adapt Algorithm */ \
        TDA18273_AGC3_Adapt_TOP_1_Step,                             /* AGC3 Adapt Low Band */ \
		TDA18273_AGC3_Adapt_TOP_1_Step,                             /* AGC3 Adapt High Band */ \
        TDA18273_AGC_Overload_TOP_plus_9_plus_4_5_min_3_5,          /* AGC Overload TOP */ \
        TDA18273_TH_AGC_Adapt34_5dB,                                /* Adapt TOP 34 Gain Threshold */ \
        TDA18273_RF_Atten_3dB_Disabled,                             /* RF Atten 3dB */ \
        TDA18273_IF_Output_Level_0_7Vpp_min_9_21dB,                 /* IF Output Level */ \
        TDA18273_S2D_Gain_6dB,                                      /* S2D Gain */ \
        TDA18273_Negative_Modulation_Enabled,                       /* Negative Modulation */ \
        TDA18273_AGCK_Steps_0_2dB,                                  /* Step */ \
        TDA18273_AGCK_Time_Constant_1_STEP_EACH_VSYNC_RISING_EDGE,  /* AGCK Time Constant */ \
        TDA18273_AGC5_HPF_Enabled,                                  /* AGC5 HPF */ \
        TDA18273_Pulse_Shaper_Disable_Enabled,                      /* Pulse Shaper Disable */ \
        TDA18273_VHF_III_Mode_Disabled,                             /* VHF III Mode */ \
        TDA18273_LO_CP_Current_Enabled,                             /* LO CP Current */ \
		TDA18273_PD_Underload_Disabled,                             /* PD Underload */ \
		754000000													/* Frequency Start LTE */ \
    }

#define TDA18273_INSTANCE_CUSTOM_STD_ANALOG_DK \
    {                                                               /* SECAM D/K */ \
        6850000,                                                    /* IF */ \
        2750000,                                                    /* CF_Offset */ \
        TDA18273_LPF_8MHz,                                          /* LPF */ \
        TDA18273_LPFOffset_0pc,                                     /* LPF_Offset */ \
        TDA18273_DC_Notch_IF_PPF_Disabled,                          /* DC notch IF PPF */ \
        TDA18273_IF_HPF_Disabled,                                   /* Hi Pass */ \
        TDA18273_IF_Notch_Enabled,                                  /* IF notch */ \
        TDA18273_IFnotchToRSSI_Disabled,                            /* IF notch To RSSI */ \
        TDA18273_AGC1_TOP_I2C_DN_UP_d95_u89dBuV,                    /* AGC1 TOP I2C DN/UP */ \
        TDA18273_AGC1_Adapt_TOP_DN_UP_1_Step,                       /* AGC1 Adapt TOP DN/UP */ \
		TDA18273_AGC1_DN_Time_Constant_8_188ms,                     /* AGC1 DN Time Constant */ \
        TDA18273_AGC1_Mode_LNA_ADAPT_TOP_ADAPT,                     /* AGC1 mode */ \
        TDA18273_Range_LNA_Adapt_20dB_11dB,                         /* Range LNA Adapt */ \
        TDA18273_LNA_Adapt_RFAGC_Gv_Threshold_18_25dB,              /* LNA Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC1_Top_Adapt_RFAGC_Gv_Threshold_15_25dB,         /* AGC1 TOP Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC2_TOP_DN_UP_d90_u84dBuV,                        /* AGC2 TOP DN/UP */ \
		TDA18273_AGC2_DN_Time_Constant_8_188ms,                     /* AGC2 DN Time Constant */ \
        TDA18273_AGC3_TOP_I2C_96dBuV,                               /* AGC3 TOP I2C Low Band */ \
		TDA18273_AGC3_TOP_I2C_96dBuV,                               /* AGC3 TOP I2C High Band */ \
        TDA18273_AGC4_TOP_DN_UP_d105_u100dBuV,                      /* AGC4 TOP DN/UP */ \
        TDA18273_AGC5_TOP_DN_UP_d105_u100dBuV,                      /* AGC5 TOP DN/UP */ \
        TDA18273_Top_Adapt_TOP_ADAPT34,                             /* AGC3 TOP Adapt Algorithm */ \
        TDA18273_AGC3_Adapt_TOP_1_Step,                             /* AGC3 Adapt Low Band */ \
		TDA18273_AGC3_Adapt_TOP_1_Step,                             /* AGC3 Adapt High Band */ \
        TDA18273_AGC_Overload_TOP_plus_9_plus_4_5_min_3_5,          /* AGC Overload TOP */ \
        TDA18273_TH_AGC_Adapt34_5dB,                                /* Adapt TOP 34 Gain Threshold */ \
        TDA18273_RF_Atten_3dB_Disabled,                             /* RF Atten 3dB */ \
        TDA18273_IF_Output_Level_0_7Vpp_min_9_21dB,                 /* IF Output Level */ \
        TDA18273_S2D_Gain_6dB,                                      /* S2D Gain */ \
        TDA18273_Negative_Modulation_Enabled,                       /* Negative Modulation */ \
        TDA18273_AGCK_Steps_0_2dB,                                  /* Step */ \
        TDA18273_AGCK_Time_Constant_1_STEP_EACH_VSYNC_RISING_EDGE,  /* AGCK Time Constant */ \
        TDA18273_AGC5_HPF_Enabled,                                  /* AGC5 HPF */ \
        TDA18273_Pulse_Shaper_Disable_Enabled,                      /* Pulse Shaper Disable */ \
        TDA18273_VHF_III_Mode_Disabled,                             /* VHF III Mode */ \
        TDA18273_LO_CP_Current_Enabled,                             /* LO CP Current */ \
		TDA18273_PD_Underload_Disabled,                             /* PD Underload */ \
		754000000													/* Frequency Start LTE */ \
    }

#define TDA18273_INSTANCE_CUSTOM_STD_ANALOG_L \
    {                                                               /* SECAM L */ \
        6750000,                                                    /* IF */ \
        2750000,                                                    /* CF_Offset */ \
        TDA18273_LPF_8MHz,                                          /* LPF */ \
        TDA18273_LPFOffset_0pc,                                     /* LPF_Offset */ \
        TDA18273_DC_Notch_IF_PPF_Disabled,                          /* DC notch IF PPF */ \
        TDA18273_IF_HPF_Disabled,                                   /* Hi Pass */ \
        TDA18273_IF_Notch_Enabled,                                  /* IF notch */ \
        TDA18273_IFnotchToRSSI_Disabled,                            /* IF notch To RSSI */ \
        TDA18273_AGC1_TOP_I2C_DN_UP_d95_u89dBuV,                    /* AGC1 TOP I2C DN/UP */ \
        TDA18273_AGC1_Adapt_TOP_DN_UP_1_Step,                       /* AGC1 Adapt TOP DN/UP */ \
		TDA18273_AGC1_DN_Time_Constant_8_188ms,                     /* AGC1 DN Time Constant */ \
        TDA18273_AGC1_Mode_LNA_ADAPT_TOP_ADAPT,                     /* AGC1 mode */ \
        TDA18273_Range_LNA_Adapt_20dB_11dB,                         /* Range LNA Adapt */ \
        TDA18273_LNA_Adapt_RFAGC_Gv_Threshold_18_25dB,              /* LNA Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC1_Top_Adapt_RFAGC_Gv_Threshold_15_25dB,         /* AGC1 TOP Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC2_TOP_DN_UP_d90_u84dBuV,                        /* AGC2 TOP DN/UP */ \
		TDA18273_AGC2_DN_Time_Constant_8_188ms,                     /* AGC2 DN Time Constant */ \
        TDA18273_AGC3_TOP_I2C_96dBuV,                               /* AGC3 TOP I2C Low Band */ \
		TDA18273_AGC3_TOP_I2C_96dBuV,                               /* AGC3 TOP I2C High Band */ \
        TDA18273_AGC4_TOP_DN_UP_d105_u100dBuV,                      /* AGC4 TOP DN/UP */ \
        TDA18273_AGC5_TOP_DN_UP_d105_u100dBuV,                      /* AGC5 TOP DN/UP */ \
        TDA18273_Top_Adapt_TOP_ADAPT34,                             /* AGC3 TOP Adapt Algorithm */ \
        TDA18273_AGC3_Adapt_TOP_1_Step,                             /* AGC3 Adapt Low Band */ \
		TDA18273_AGC3_Adapt_TOP_1_Step,                             /* AGC3 Adapt High Band */ \
        TDA18273_AGC_Overload_TOP_plus_9_plus_6_min_9,              /* AGC Overload TOP */ \
        TDA18273_TH_AGC_Adapt34_5dB,                                /* Adapt TOP 34 Gain Threshold */ \
        TDA18273_RF_Atten_3dB_Disabled,                             /* RF Atten 3dB */ \
        TDA18273_IF_Output_Level_0_7Vpp_min_9_21dB,                 /* IF Output Level */ \
        TDA18273_S2D_Gain_6dB,                                      /* S2D Gain */ \
        TDA18273_Negative_Modulation_Disabled,                      /* Negative Modulation */ \
        TDA18273_AGCK_Steps_0_2dB,                                  /* Step */ \
        TDA18273_AGCK_Time_Constant_1_STEP_EACH_VSYNC_RISING_EDGE,  /* AGCK Time Constant */ \
        TDA18273_AGC5_HPF_Enabled,                                  /* AGC5 HPF */ \
        TDA18273_Pulse_Shaper_Disable_Enabled,                      /* Pulse Shaper Disable */ \
        TDA18273_VHF_III_Mode_Disabled,                             /* VHF III Mode */ \
        TDA18273_LO_CP_Current_Enabled,                             /* LO CP Current */ \
		TDA18273_PD_Underload_Disabled,                             /* PD Underload */ \
		754000000													/* Frequency Start LTE */ \
    }

#define TDA18273_INSTANCE_CUSTOM_STD_ANALOG_LL \
    {                                                               /* SECAM L' */ \
        1250000,                                                    /* IF */ \
        -2750000,                                                   /* CF_Offset */ \
        TDA18273_LPF_8MHz,                                          /* LPF */ \
        TDA18273_LPFOffset_0pc,                                     /* LPF_Offset */ \
        TDA18273_DC_Notch_IF_PPF_Disabled,                          /* DC notch IF PPF */ \
        TDA18273_IF_HPF_Disabled,                                   /* Hi Pass */ \
        TDA18273_IF_Notch_Disabled,                                 /* IF notch */ \
        TDA18273_IFnotchToRSSI_Enabled,                             /* IF notch To RSSI */ \
        TDA18273_AGC1_TOP_I2C_DN_UP_d95_u89dBuV,                    /* AGC1 TOP I2C DN/UP */ \
        TDA18273_AGC1_Adapt_TOP_DN_UP_1_Step,                       /* AGC1 Adapt TOP DN/UP */ \
		TDA18273_AGC1_DN_Time_Constant_8_188ms,                     /* AGC1 DN Time Constant */ \
        TDA18273_AGC1_Mode_LNA_ADAPT_TOP_ADAPT,                     /* AGC1 mode */ \
        TDA18273_Range_LNA_Adapt_20dB_11dB,                         /* Range LNA Adapt */ \
        TDA18273_LNA_Adapt_RFAGC_Gv_Threshold_18_25dB,              /* LNA Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC1_Top_Adapt_RFAGC_Gv_Threshold_15_25dB,         /* AGC1 TOP Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC2_TOP_DN_UP_d90_u84dBuV,                        /* AGC2 TOP DN/UP */ \
		TDA18273_AGC2_DN_Time_Constant_8_188ms,                     /* AGC2 DN Time Constant */ \
        TDA18273_AGC3_TOP_I2C_96dBuV,                               /* AGC3 TOP I2C Low Band */ \
		TDA18273_AGC3_TOP_I2C_96dBuV,                               /* AGC3 TOP I2C High Band */ \
        TDA18273_AGC4_TOP_DN_UP_d105_u100dBuV,                      /* AGC4 TOP DN/UP */ \
        TDA18273_AGC5_TOP_DN_UP_d105_u100dBuV,                      /* AGC5 TOP DN/UP */ \
        TDA18273_Top_Adapt_TOP_ADAPT34,                             /* AGC3 TOP Adapt Algorithm */ \
        TDA18273_AGC3_Adapt_TOP_1_Step,                             /* AGC3 Adapt Low Band */ \
		TDA18273_AGC3_Adapt_TOP_1_Step,                             /* AGC3 Adapt High Band */ \
        TDA18273_AGC_Overload_TOP_plus_9_plus_6_min_9,              /* AGC Overload TOP */ \
        TDA18273_TH_AGC_Adapt34_5dB,                                /* Adapt TOP 34 Gain Threshold */ \
        TDA18273_RF_Atten_3dB_Disabled,                             /* RF Atten 3dB */ \
        TDA18273_IF_Output_Level_0_7Vpp_min_9_21dB,                 /* IF Output Level */ \
        TDA18273_S2D_Gain_6dB,                                      /* S2D Gain */ \
        TDA18273_Negative_Modulation_Disabled,                      /* Negative Modulation */ \
        TDA18273_AGCK_Steps_0_2dB,                                  /* Step */ \
        TDA18273_AGCK_Time_Constant_1_STEP_EACH_VSYNC_RISING_EDGE,  /* AGCK Time Constant */ \
        TDA18273_AGC5_HPF_Disabled,                                 /* AGC5 HPF */ \
        TDA18273_Pulse_Shaper_Disable_Enabled,                      /* Pulse Shaper Disable */ \
        TDA18273_VHF_III_Mode_Disabled,                             /* VHF III Mode */ \
        TDA18273_LO_CP_Current_Enabled,                             /* LO CP Current */ \
		TDA18273_PD_Underload_Disabled,                             /* PD Underload */ \
		754000000													/* Frequency Start LTE */ \
    }

#define TDA18273_INSTANCE_CUSTOM_STD_ANALOG_BLIND_SCANNING \
    {                                                               /* Blind Scanning copy of PAL-I */ \
        7250000,                                                    /* IF */ \
        2750000,                                                    /* CF_Offset */ \
        TDA18273_LPF_8MHz,                                          /* LPF */ \
        TDA18273_LPFOffset_0pc,                                     /* LPF_Offset */ \
        TDA18273_DC_Notch_IF_PPF_Disabled,                          /* DC notch IF PPF */ \
        TDA18273_IF_HPF_Disabled,                                   /* Hi Pass */ \
        TDA18273_IF_Notch_Disabled,                                 /* IF notch */ \
        TDA18273_IFnotchToRSSI_Enabled,                             /* IF notch To RSSI */ \
        TDA18273_AGC1_TOP_I2C_DN_UP_d95_u89dBuV,                    /* AGC1 TOP I2C DN/UP */ \
        TDA18273_AGC1_Adapt_TOP_DN_UP_1_Step,                       /* AGC1 Adapt TOP DN/UP */ \
		TDA18273_AGC1_DN_Time_Constant_8_188ms,                     /* AGC1 DN Time Constant */ \
        TDA18273_AGC1_Mode_LNA_ADAPT_TOP_ADAPT,                     /* AGC1 mode */ \
        TDA18273_Range_LNA_Adapt_20dB_11dB,                         /* Range LNA Adapt */ \
        TDA18273_LNA_Adapt_RFAGC_Gv_Threshold_18_25dB,              /* LNA Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC1_Top_Adapt_RFAGC_Gv_Threshold_15_25dB,         /* AGC1 TOP Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC2_TOP_DN_UP_d90_u84dBuV,                        /* AGC2 TOP DN/UP */ \
		TDA18273_AGC2_DN_Time_Constant_8_188ms,                     /* AGC2 DN Time Constant */ \
        TDA18273_AGC3_TOP_I2C_96dBuV,                               /* AGC3 TOP I2C Low Band */ \
		TDA18273_AGC3_TOP_I2C_96dBuV,                               /* AGC3 TOP I2C High Band */ \
        TDA18273_AGC4_TOP_DN_UP_d105_u100dBuV,                      /* AGC4 TOP DN/UP */ \
        TDA18273_AGC5_TOP_DN_UP_d105_u100dBuV,                      /* AGC5 TOP DN/UP */ \
        TDA18273_Top_Adapt_TOP_ADAPT34,                             /* AGC3 TOP Adapt Algorithm */ \
        TDA18273_AGC3_Adapt_TOP_1_Step,                             /* AGC3 Adapt Low Band */ \
		TDA18273_AGC3_Adapt_TOP_1_Step,                             /* AGC3 Adapt High Band */ \
        TDA18273_AGC_Overload_TOP_plus_9_plus_4_5_min_3_5,          /* AGC Overload TOP */ \
        TDA18273_TH_AGC_Adapt34_5dB,                                /* Adapt TOP 34 Gain Threshold */ \
        TDA18273_RF_Atten_3dB_Disabled,                             /* RF Atten 3dB */ \
        TDA18273_IF_Output_Level_0_7Vpp_min_9_21dB,                 /* IF Output Level */ \
        TDA18273_S2D_Gain_6dB,                                      /* S2D Gain */ \
        TDA18273_Negative_Modulation_Disabled,                      /* Negative Modulation */ \
        TDA18273_AGCK_Steps_0_2dB,                                  /* Step */ \
        TDA18273_AGCK_Time_Constant_1_STEP_EACH_VSYNC_RISING_EDGE,  /* AGCK Time Constant */ \
        TDA18273_AGC5_HPF_Enabled,                                  /* AGC5 HPF */ \
        TDA18273_Pulse_Shaper_Disable_Enabled,                      /* Pulse Shaper Disable */ \
        TDA18273_VHF_III_Mode_Disabled,                             /* VHF III Mode */ \
        TDA18273_LO_CP_Current_Enabled,                             /* LO CP Current */ \
		TDA18273_PD_Underload_Disabled,                             /* PD Underload */ \
		754000000													/* Frequency Start LTE */ \
    }

#define TDA18273_INSTANCE_CUSTOM_STD_SCANXPRESS \
    {                                                               /* ScanXpress */ \
        5000000,                                                    /* IF */ \
        0,                                                          /* CF_Offset */ \
        TDA18273_LPF_9MHz,                                          /* LPF */ \
        TDA18273_LPFOffset_min_8pc,                                 /* LPF_Offset */ \
        TDA18273_DC_Notch_IF_PPF_Enabled,                           /* DC notch IF PPF */ \
        TDA18273_IF_HPF_0_85MHz,                                    /* Hi Pass */ \
        TDA18273_IF_Notch_Disabled,                                 /* IF notch */ \
        TDA18273_IFnotchToRSSI_Enabled,                             /* IF notch To RSSI */ \
        TDA18273_AGC1_TOP_I2C_DN_UP_d95_u89wdBuV,                   /* AGC1 TOP I2C DN/UP */ \
        TDA18273_AGC1_Adapt_TOP_DN_UP_0_Step,                       /* AGC1 Adapt TOP DN/UP */ \
		TDA18273_AGC1_DN_Time_Constant_8_188ms,                     /* AGC1 DN Time Constant */ \
        TDA18273_AGC1_Mode_LNA_ADAPT_WIDE,                          /* AGC1 mode */ \
        TDA18273_Range_LNA_Adapt_20dB_11dB,                         /* Range LNA Adapt */ \
        TDA18273_LNA_Adapt_RFAGC_Gv_Threshold_18_25dB,              /* LNA Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC1_Top_Adapt_RFAGC_Gv_Threshold_16_75dB,         /* AGC1 TOP Adapt RFAGC Gv Threshold */ \
        TDA18273_AGC2_TOP_DN_UP_d90_u84dBuV,                        /* AGC2 TOP DN/UP */ \
		TDA18273_AGC2_DN_Time_Constant_8_188ms,                     /* AGC2 DN Time Constant */ \
        TDA18273_AGC3_TOP_I2C_100dBuV,                              /* AGC3 TOP I2C Low Band */ \
		TDA18273_AGC3_TOP_I2C_100dBuV,                              /* AGC3 TOP I2C High Band */ \
        TDA18273_AGC4_TOP_DN_UP_d110_u105dBuV,                      /* AGC4 TOP DN/UP */ \
        TDA18273_AGC5_TOP_DN_UP_d110_u105dBuV,                      /* AGC5 TOP DN/UP */ \
        TDA18273_Top_Adapt_NO_TOP_ADAPT,                            /* AGC3 TOP Adapt Algorithm */ \
        TDA18273_AGC3_Adapt_TOP_0_Step,                             /* AGC3 Adapt Low Band */ \
		TDA18273_AGC3_Adapt_TOP_0_Step,                             /* AGC3 Adapt High Band */ \
        TDA18273_AGC_Overload_TOP_plus_9_plus_4_5_min_3_5,          /* AGC Overload TOP */ \
        TDA18273_TH_AGC_Adapt34_5dB,                                /* Adapt TOP 34 Gain Threshold */ \
        TDA18273_RF_Atten_3dB_Disabled,                             /* RF Atten 3dB */ \
        TDA18273_IF_Output_Level_1Vpp_min_6_24dB,                   /* IF Output Level */ \
        TDA18273_S2D_Gain_6dB,                                      /* S2D Gain */ \
        TDA18273_Negative_Modulation_Disabled,                      /* Negative Modulation */ \
        TDA18273_AGCK_Steps_0_2dB,                                  /* Step */ \
        TDA18273_AGCK_Time_Constant_32_768ms,                       /* AGCK Time Constant */ \
        TDA18273_AGC5_HPF_Disabled,                                 /* AGC5 HPF */ \
        TDA18273_Pulse_Shaper_Disable_Enabled,                      /* Pulse Shaper Disable */ \
        TDA18273_VHF_III_Mode_Disabled,                             /* VHF III Mode */ \
        TDA18273_LO_CP_Current_Enabled,                             /* LO CP Current */ \
		TDA18273_PD_Underload_Disabled,                             /* PD Underload */ \
		754000000													/* Frequency Start LTE */ \
    }

/* Standard Presets Aggregation: */
#define TDA18273_INSTANCE_CUSTOM_STD_DEF \
    { \
        TDA18273_INSTANCE_CUSTOM_STD_QAM_6MHZ, \
        TDA18273_INSTANCE_CUSTOM_STD_QAM_8MHZ, \
        TDA18273_INSTANCE_CUSTOM_STD_ATSC_6MHZ, \
        TDA18273_INSTANCE_CUSTOM_STD_ISDBT_6MHZ, \
        TDA18273_INSTANCE_CUSTOM_STD_DVBT_1_7MHZ, \
        TDA18273_INSTANCE_CUSTOM_STD_DVBT_6MHZ, \
        TDA18273_INSTANCE_CUSTOM_STD_DVBT_7MHZ, \
        TDA18273_INSTANCE_CUSTOM_STD_DVBT_8MHZ, \
        TDA18273_INSTANCE_CUSTOM_STD_DVBT_10MHZ, \
        TDA18273_INSTANCE_CUSTOM_STD_DMBT_8MHZ, \
        TDA18273_INSTANCE_CUSTOM_STD_ANALOG_FM_RADIO, \
        TDA18273_INSTANCE_CUSTOM_STD_ANALOG_MN, \
        TDA18273_INSTANCE_CUSTOM_STD_ANALOG_B, \
        TDA18273_INSTANCE_CUSTOM_STD_ANALOG_GH, \
        TDA18273_INSTANCE_CUSTOM_STD_ANALOG_I, \
        TDA18273_INSTANCE_CUSTOM_STD_ANALOG_DK, \
        TDA18273_INSTANCE_CUSTOM_STD_ANALOG_L, \
        TDA18273_INSTANCE_CUSTOM_STD_ANALOG_LL, \
        TDA18273_INSTANCE_CUSTOM_STD_ANALOG_BLIND_SCANNING, \
        TDA18273_INSTANCE_CUSTOM_STD_SCANXPRESS \
    }


#endif /* __IFJ2148_CONF_H__ */

