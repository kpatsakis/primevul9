static int bond_netpoll_setup(struct net_device *dev, struct netpoll_info *ni)
{
	struct bonding *bond = netdev_priv(dev);
	struct list_head *iter;
	struct slave *slave;
	int err = 0;

	bond_for_each_slave(bond, slave, iter) {
		err = slave_enable_netpoll(slave);
		if (err) {
			bond_netpoll_cleanup(dev);
			break;
		}
	}
	return err;
}