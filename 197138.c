static void bond_poll_controller(struct net_device *bond_dev)
{
	struct bonding *bond = netdev_priv(bond_dev);
	struct slave *slave = NULL;
	struct list_head *iter;
	struct ad_info ad_info;

	if (BOND_MODE(bond) == BOND_MODE_8023AD)
		if (bond_3ad_get_active_agg_info(bond, &ad_info))
			return;

	bond_for_each_slave_rcu(bond, slave, iter) {
		if (!bond_slave_is_up(slave))
			continue;

		if (BOND_MODE(bond) == BOND_MODE_8023AD) {
			struct aggregator *agg =
			    SLAVE_AD_INFO(slave)->port.aggregator;

			if (agg &&
			    agg->aggregator_identifier != ad_info.aggregator_id)
				continue;
		}

		netpoll_poll_dev(slave->dev);
	}
}