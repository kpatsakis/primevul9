int bond_create(struct net *net, const char *name)
{
	struct net_device *bond_dev;
	struct bonding *bond;
	struct alb_bond_info *bond_info;
	int res;

	rtnl_lock();

	bond_dev = alloc_netdev_mq(sizeof(struct bonding),
				   name ? name : "bond%d", NET_NAME_UNKNOWN,
				   bond_setup, tx_queues);
	if (!bond_dev) {
		pr_err("%s: eek! can't alloc netdev!\n", name);
		rtnl_unlock();
		return -ENOMEM;
	}

	/*
	 * Initialize rx_hashtbl_used_head to RLB_NULL_INDEX.
	 * It is set to 0 by default which is wrong.
	 */
	bond = netdev_priv(bond_dev);
	bond_info = &(BOND_ALB_INFO(bond));
	bond_info->rx_hashtbl_used_head = RLB_NULL_INDEX;

	dev_net_set(bond_dev, net);
	bond_dev->rtnl_link_ops = &bond_link_ops;

	res = register_netdevice(bond_dev);
	if (res < 0) {
		free_netdev(bond_dev);
		rtnl_unlock();

		return res;
	}

	netif_carrier_off(bond_dev);

	bond_work_init_all(bond);

	rtnl_unlock();
	return 0;
}