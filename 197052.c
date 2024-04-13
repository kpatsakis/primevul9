static int bond_init(struct net_device *bond_dev)
{
	struct bonding *bond = netdev_priv(bond_dev);
	struct bond_net *bn = net_generic(dev_net(bond_dev), bond_net_id);

	netdev_dbg(bond_dev, "Begin bond_init\n");

	bond->wq = alloc_ordered_workqueue(bond_dev->name, WQ_MEM_RECLAIM);
	if (!bond->wq)
		return -ENOMEM;

	if (BOND_MODE(bond) == BOND_MODE_ROUNDROBIN) {
		bond->rr_tx_counter = alloc_percpu(u32);
		if (!bond->rr_tx_counter) {
			destroy_workqueue(bond->wq);
			bond->wq = NULL;
			return -ENOMEM;
		}
	}

	spin_lock_init(&bond->stats_lock);
	netdev_lockdep_set_classes(bond_dev);

	list_add_tail(&bond->bond_list, &bn->dev_list);

	bond_prepare_sysfs_group(bond);

	bond_debug_register(bond);

	/* Ensure valid dev_addr */
	if (is_zero_ether_addr(bond_dev->dev_addr) &&
	    bond_dev->addr_assign_type == NET_ADDR_PERM)
		eth_hw_addr_random(bond_dev);

	return 0;
}