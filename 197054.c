static bool bond_ipsec_offload_ok(struct sk_buff *skb, struct xfrm_state *xs)
{
	struct net_device *bond_dev = xs->xso.dev;
	struct bonding *bond = netdev_priv(bond_dev);
	struct slave *curr_active = rcu_dereference(bond->curr_active_slave);
	struct net_device *slave_dev = curr_active->dev;

	if (BOND_MODE(bond) != BOND_MODE_ACTIVEBACKUP)
		return true;

	if (!(slave_dev->xfrmdev_ops
	      && slave_dev->xfrmdev_ops->xdo_dev_offload_ok)) {
		slave_warn(bond_dev, slave_dev, "%s: no slave xdo_dev_offload_ok\n", __func__);
		return false;
	}

	xs->xso.real_dev = slave_dev;
	return slave_dev->xfrmdev_ops->xdo_dev_offload_ok(skb, xs);
}