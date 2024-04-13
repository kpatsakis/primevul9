static int bond_release_and_destroy(struct net_device *bond_dev,
				    struct net_device *slave_dev)
{
	struct bonding *bond = netdev_priv(bond_dev);
	int ret;

	ret = __bond_release_one(bond_dev, slave_dev, false, true);
	if (ret == 0 && !bond_has_slaves(bond) &&
	    bond_dev->reg_state != NETREG_UNREGISTERING) {
		bond_dev->priv_flags |= IFF_DISABLE_NETPOLL;
		netdev_info(bond_dev, "Destroying bond\n");
		bond_remove_proc_entry(bond);
		unregister_netdevice(bond_dev);
	}
	return ret;
}