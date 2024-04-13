static struct slave *bond_alloc_slave(struct bonding *bond,
				      struct net_device *slave_dev)
{
	struct slave *slave = NULL;

	slave = kzalloc(sizeof(*slave), GFP_KERNEL);
	if (!slave)
		return NULL;

	slave->bond = bond;
	slave->dev = slave_dev;
	INIT_DELAYED_WORK(&slave->notify_work, bond_netdev_notify_work);

	if (bond_kobj_init(slave))
		return NULL;

	if (BOND_MODE(bond) == BOND_MODE_8023AD) {
		SLAVE_AD_INFO(slave) = kzalloc(sizeof(struct ad_slave_info),
					       GFP_KERNEL);
		if (!SLAVE_AD_INFO(slave)) {
			kobject_put(&slave->kobj);
			return NULL;
		}
	}

	return slave;
}