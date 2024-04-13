static void bond_ipsec_del_sa(struct xfrm_state *xs)
{
	struct net_device *bond_dev = xs->xso.dev;
	struct bonding *bond;
	struct slave *slave;

	if (!bond_dev)
		return;

	bond = netdev_priv(bond_dev);
	slave = rcu_dereference(bond->curr_active_slave);

	if (!slave)
		return;

	xs->xso.real_dev = slave->dev;

	if (!(slave->dev->xfrmdev_ops
	      && slave->dev->xfrmdev_ops->xdo_dev_state_delete)) {
		slave_warn(bond_dev, slave->dev, "%s: no slave xdo_dev_state_delete\n", __func__);
		return;
	}

	slave->dev->xfrmdev_ops->xdo_dev_state_delete(xs);
}