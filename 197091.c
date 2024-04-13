static void bond_destructor(struct net_device *bond_dev)
{
	struct bonding *bond = netdev_priv(bond_dev);

	if (bond->wq)
		destroy_workqueue(bond->wq);

	if (bond->rr_tx_counter)
		free_percpu(bond->rr_tx_counter);
}