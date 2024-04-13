static void bond_hw_addr_flush(struct net_device *bond_dev,
			       struct net_device *slave_dev)
{
	struct bonding *bond = netdev_priv(bond_dev);

	dev_uc_unsync(slave_dev, bond_dev);
	dev_mc_unsync(slave_dev, bond_dev);

	if (BOND_MODE(bond) == BOND_MODE_8023AD) {
		/* del lacpdu mc addr from mc list */
		u8 lacpdu_multicast[ETH_ALEN] = MULTICAST_LACPDU_ADDR;

		dev_mc_del(slave_dev, lacpdu_multicast);
	}
}