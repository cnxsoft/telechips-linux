/*
 * include/linux/tcc_ts_cypress.h - platform data structure for cypress touch ic.
 */

#ifndef _TCC_TS_CYPRESS_H_
#define _TCC_TS_CYPRESS_H_

#define TCC_TS_CYPRESS_NAME "cypress-ts"

struct tcc_ts_cypress_platform_data {
	int (*power)(int on);	/* Only valid in first array entry */
	int (*init)(void);
	int (*irq)(int en);
};

#endif /* _TCC_TS_CYPRESS_H_ */
