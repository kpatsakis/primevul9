static void temac_adjust_link(struct net_device *ndev)
{
	struct temac_local *lp = netdev_priv(ndev);
	struct phy_device *phy = ndev->phydev;
	u32 mii_speed;
	int link_state;
	unsigned long flags;

	/* hash together the state values to decide if something has changed */
	link_state = phy->speed | (phy->duplex << 1) | phy->link;

	if (lp->last_link != link_state) {
		spin_lock_irqsave(lp->indirect_lock, flags);
		mii_speed = temac_indirect_in32_locked(lp, XTE_EMCFG_OFFSET);
		mii_speed &= ~XTE_EMCFG_LINKSPD_MASK;

		switch (phy->speed) {
		case SPEED_1000: mii_speed |= XTE_EMCFG_LINKSPD_1000; break;
		case SPEED_100: mii_speed |= XTE_EMCFG_LINKSPD_100; break;
		case SPEED_10: mii_speed |= XTE_EMCFG_LINKSPD_10; break;
		}

		/* Write new speed setting out to TEMAC */
		temac_indirect_out32_locked(lp, XTE_EMCFG_OFFSET, mii_speed);
		spin_unlock_irqrestore(lp->indirect_lock, flags);

		lp->last_link = link_state;
		phy_print_status(phy);
	}
}