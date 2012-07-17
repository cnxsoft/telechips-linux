/*
 * Based on : linux/drivers/net/phy/realtek.c
 *
 * Driver for Realtek PHYs
 *
 * Author: Telechips <linux@telechips.com>
 * Created : June 22, 2010
 *
 * Copyright (c) 2010 Telechips, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#include <linux/phy.h>

#define RTL8201_PHYSR		0x11
#define RTL8201_PHYSR_DUPLEX	0x2000
#define RTL8201_PHYSR_SPEED	0xc000
#define RTL8201_INER		0x12
#define RTL8201_INER_INIT	0x6400
#define RTL8201_INSR		0x13

MODULE_DESCRIPTION("Realtek PHY driver for TCC");
MODULE_AUTHOR("Telechips");
MODULE_LICENSE("GPL");

static int rtl8201_ack_interrupt(struct phy_device *phydev)
{
	int err;

	err = phy_read(phydev, RTL8201_INSR);

	return (err < 0) ? err : 0;
}

static int rtl8201_config_intr(struct phy_device *phydev)
{
	int err;

	if (phydev->interrupts == PHY_INTERRUPT_ENABLED)
		err = phy_write(phydev, RTL8201_INER,
				RTL8201_INER_INIT);
	else
		err = phy_write(phydev, RTL8201_INER, 0);

	return err;
}

int tcc_rtl8201_update_link(struct phy_device *phydev)
{
	int status;
	int status1, status2;

	/* Do a fake read */
	status = phy_read(phydev, MII_BMSR);

	if (status < 0)
		return status;

	/* Read link and autonegotiation status */
	do {
		status1 = phy_read(phydev, MII_BMSR);
		status2 = phy_read(phydev, MII_BMSR);
	} while(status1 != status2);

	status = status1;
	
	if (status < 0)
		return status;

	if ((status & BMSR_LSTATUS) == 0)
		phydev->link = 0;
	else
		phydev->link = 1;

	return 0;
}

/**
 * tcc_rtl8201_read_status - check the link status and update current link state
 * @phydev: target phy_device struct
 *
 * Description: Check the link, then figure out the current state
 *   by comparing what we advertise with what the link partner
 *   advertises.  Start by checking the gigabit possibilities,
 *   then move on to 10/100.
 */
int tcc_rtl8201_read_status(struct phy_device *phydev)
{
	int adv;
	int adv1, adv2;
	int err;
	int lpa = 0;
	int lpa1, lpa2;
	int lpagb = 0;
	int lpagb1, lpagb2;

	/* Update the link, but return if there
	 * was an error */
	err = tcc_rtl8201_update_link(phydev);
	if (err)
		return err;

	if (AUTONEG_ENABLE == phydev->autoneg) {
		if (phydev->supported & (SUPPORTED_1000baseT_Half
					| SUPPORTED_1000baseT_Full)) {
			do {
				lpagb1 = phy_read(phydev, MII_STAT1000);
				lpagb2 = phy_read(phydev, MII_STAT1000);
			} while(lpagb1 != lpagb2);
			lpagb = lpagb1;

			if (lpagb < 0)
				return lpagb;

			do {
				adv1 = phy_read(phydev, MII_CTRL1000);
				adv2 = phy_read(phydev, MII_CTRL1000);
			} while(adv1 != adv2);
			adv = adv1;

			if (adv < 0)
				return adv;

			lpagb &= adv << 2;
		}

		do {
			lpa1 = phy_read(phydev, MII_LPA);
			lpa2 = phy_read(phydev, MII_LPA);
		} while(lpa1 != lpa2);
		lpa = lpa1;
		if (lpa < 0)
			return lpa;

		do {
			adv1 = phy_read(phydev, MII_ADVERTISE);
			adv2 = phy_read(phydev, MII_ADVERTISE);
		} while(adv1 != adv2);
		adv = adv1;

		if (adv < 0)
			return adv;

		lpa &= adv;

		phydev->speed = SPEED_10;
		phydev->duplex = DUPLEX_HALF;
		phydev->pause = phydev->asym_pause = 0;

		if (lpagb & (LPA_1000FULL | LPA_1000HALF)) {
			phydev->speed = SPEED_1000;

			if (lpagb & LPA_1000FULL)
				phydev->duplex = DUPLEX_FULL;
		} else if (lpa & (LPA_100FULL | LPA_100HALF)) {
			phydev->speed = SPEED_100;
			
			if (lpa & LPA_100FULL)
				phydev->duplex = DUPLEX_FULL;
		} else
			if (lpa & LPA_10FULL)
				phydev->duplex = DUPLEX_FULL;

		if (phydev->duplex == DUPLEX_FULL){
			phydev->pause = lpa & LPA_PAUSE_CAP ? 1 : 0;
			phydev->asym_pause = lpa & LPA_PAUSE_ASYM ? 1 : 0;
		}
	} else {
		int bmcr = phy_read(phydev, MII_BMCR);
		if (bmcr < 0)
			return bmcr;

		if (bmcr & BMCR_FULLDPLX)
			phydev->duplex = DUPLEX_FULL;
		else
			phydev->duplex = DUPLEX_HALF;

		if (bmcr & BMCR_SPEED1000)
			phydev->speed = SPEED_1000;
		else if (bmcr & BMCR_SPEED100)
			phydev->speed = SPEED_100;
		else
			phydev->speed = SPEED_10;

		phydev->pause = phydev->asym_pause = 0;
	}

	return 0;
}

/* RTL8211B */
static struct phy_driver rtl8201_driver = {
	.phy_id		= 0x00008201,
	.name		= "RTL8201 Ethernet",
	.phy_id_mask	= 0x001fffff,
	.features	= PHY_BASIC_FEATURES,
	.flags		= PHY_HAS_INTERRUPT,
	.config_aneg	= &genphy_config_aneg,
	.read_status	= &tcc_rtl8201_read_status,
	.ack_interrupt	= &rtl8201_ack_interrupt,
	.config_intr	= &rtl8201_config_intr,
	.driver		= { .owner = THIS_MODULE,},
};

static int __init realtek_init(void)
{
	int ret;

	ret = phy_driver_register(&rtl8201_driver);

	return ret;
}

static void __exit realtek_exit(void)
{
	phy_driver_unregister(&rtl8201_driver);
}

module_init(realtek_init);
module_exit(realtek_exit);
