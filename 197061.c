static void bond_uninit(struct net_device *bond_dev)
{
	struct bonding *bond = netdev_priv(bond_dev);
	struct bond_up_slave *usable, *all;
	struct list_head *iter;
	struct slave *slave;

	bond_netpoll_cleanup(bond_dev);

	/* Release the bonded slaves */
	bond_for_each_slave(bond, slave, iter)
		__bond_release_one(bond_dev, slave->dev, true, true);
	netdev_info(bond_dev, "Released all slaves\n");

	usable = rtnl_dereference(bond->usable_slaves);
	if (usable) {
		RCU_INIT_POINTER(bond->usable_slaves, NULL);
		kfree_rcu(usable, rcu);
	}

	all = rtnl_dereference(bond->all_slaves);
	if (all) {
		RCU_INIT_POINTER(bond->all_slaves, NULL);
		kfree_rcu(all, rcu);
	}

	list_del(&bond->bond_list);

	bond_debug_unregister(bond);
}