static void bond_set_rx_mode(struct net_device *bond_dev)
{
	struct bonding *bond = netdev_priv(bond_dev);
	struct list_head *iter;
	struct slave *slave;

	rcu_read_lock();
	if (bond_uses_primary(bond)) {
		slave = rcu_dereference(bond->curr_active_slave);
		if (slave) {
			dev_uc_sync(slave->dev, bond_dev);
			dev_mc_sync(slave->dev, bond_dev);
		}
	} else {
		bond_for_each_slave_rcu(bond, slave, iter) {
			dev_uc_sync_multiple(slave->dev, bond_dev);
			dev_mc_sync_multiple(slave->dev, bond_dev);
		}
	}
	rcu_read_unlock();
}