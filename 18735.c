static int xemaclite_open(struct net_device *dev)
{
	struct net_local *lp = netdev_priv(dev);
	int retval;

	/* Just to be safe, stop the device first */
	xemaclite_disable_interrupts(lp);

	if (lp->phy_node) {
		u32 bmcr;

		lp->phy_dev = of_phy_connect(lp->ndev, lp->phy_node,
					     xemaclite_adjust_link, 0,
					     PHY_INTERFACE_MODE_MII);
		if (!lp->phy_dev) {
			dev_err(&lp->ndev->dev, "of_phy_connect() failed\n");
			return -ENODEV;
		}

		/* EmacLite doesn't support giga-bit speeds */
		phy_set_max_speed(lp->phy_dev, SPEED_100);

		/* Don't advertise 1000BASE-T Full/Half duplex speeds */
		phy_write(lp->phy_dev, MII_CTRL1000, 0);

		/* Advertise only 10 and 100mbps full/half duplex speeds */
		phy_write(lp->phy_dev, MII_ADVERTISE, ADVERTISE_ALL |
			  ADVERTISE_CSMA);

		/* Restart auto negotiation */
		bmcr = phy_read(lp->phy_dev, MII_BMCR);
		bmcr |= (BMCR_ANENABLE | BMCR_ANRESTART);
		phy_write(lp->phy_dev, MII_BMCR, bmcr);

		phy_start(lp->phy_dev);
	}

	/* Set the MAC address each time opened */
	xemaclite_update_address(lp, dev->dev_addr);

	/* Grab the IRQ */
	retval = request_irq(dev->irq, xemaclite_interrupt, 0, dev->name, dev);
	if (retval) {
		dev_err(&lp->ndev->dev, "Could not allocate interrupt %d\n",
			dev->irq);
		if (lp->phy_dev)
			phy_disconnect(lp->phy_dev);
		lp->phy_dev = NULL;

		return retval;
	}

	/* Enable Interrupts */
	xemaclite_enable_interrupts(lp);

	/* We're ready to go */
	netif_start_queue(dev);

	return 0;
}