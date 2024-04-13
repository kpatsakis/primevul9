void bond_select_active_slave(struct bonding *bond)
{
	struct slave *best_slave;
	int rv;

	ASSERT_RTNL();

	best_slave = bond_find_best_slave(bond);
	if (best_slave != rtnl_dereference(bond->curr_active_slave)) {
		bond_change_active_slave(bond, best_slave);
		rv = bond_set_carrier(bond);
		if (!rv)
			return;

		if (netif_carrier_ok(bond->dev))
			netdev_info(bond->dev, "active interface up!\n");
		else
			netdev_info(bond->dev, "now running without any active interface!\n");
	}
}