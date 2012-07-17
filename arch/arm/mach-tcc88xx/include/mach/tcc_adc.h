struct tcc_adc_client;

extern unsigned long tcc_adc_start(struct tcc_adc_client *client,
                         unsigned int channel, unsigned int nr_samples);

extern struct tcc_adc_client *tcc_adc_register(struct platform_device *pdev,
                                               void (*select)(unsigned selected),
                                               void (*conv)(unsigned d0, unsigned d1),
                                               unsigned int is_ts);

extern void tcc_adc_release(struct tcc_adc_client *client);

