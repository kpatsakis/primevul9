static int bond_slave_info_query(struct net_device *bond_dev, struct ifslave *info)
{
	struct bonding *bond = netdev_priv(bond_dev);
	struct list_head *iter;
	int i = 0, res = -ENODEV;
	struct slave *slave;

	bond_for_each_slave(bond, slave, iter) {
		if (i++ == (int)info->slave_id) {
			res = 0;
			bond_fill_ifslave(slave, info);
			break;
		}
	}

	return res;
}