static void bond_netpoll_cleanup(struct net_device *bond_dev)
{
	struct bonding *bond = netdev_priv(bond_dev);
	struct list_head *iter;
	struct slave *slave;

	bond_for_each_slave(bond, slave, iter)
		if (bond_slave_is_up(slave))
			slave_disable_netpoll(slave);
}