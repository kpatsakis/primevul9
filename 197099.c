static void bond_hw_addr_swap(struct bonding *bond, struct slave *new_active,
			      struct slave *old_active)
{
	if (old_active) {
		if (bond->dev->flags & IFF_PROMISC)
			dev_set_promiscuity(old_active->dev, -1);

		if (bond->dev->flags & IFF_ALLMULTI)
			dev_set_allmulti(old_active->dev, -1);

		bond_hw_addr_flush(bond->dev, old_active->dev);
	}

	if (new_active) {
		/* FIXME: Signal errors upstream. */
		if (bond->dev->flags & IFF_PROMISC)
			dev_set_promiscuity(new_active->dev, 1);

		if (bond->dev->flags & IFF_ALLMULTI)
			dev_set_allmulti(new_active->dev, 1);

		netif_addr_lock_bh(bond->dev);
		dev_uc_sync(new_active->dev, bond->dev);
		dev_mc_sync(new_active->dev, bond->dev);
		netif_addr_unlock_bh(bond->dev);
	}
}