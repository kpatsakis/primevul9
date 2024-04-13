static netdev_tx_t bond_3ad_xor_xmit(struct sk_buff *skb,
				     struct net_device *dev)
{
	struct bonding *bond = netdev_priv(dev);
	struct bond_up_slave *slaves;
	struct slave *slave;

	slaves = rcu_dereference(bond->usable_slaves);
	slave = bond_xmit_3ad_xor_slave_get(bond, skb, slaves);
	if (likely(slave))
		return bond_dev_queue_xmit(bond, skb, slave->dev);

	return bond_tx_drop(dev, skb);
}