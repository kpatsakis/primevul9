static netdev_tx_t bond_xmit_activebackup(struct sk_buff *skb,
					  struct net_device *bond_dev)
{
	struct bonding *bond = netdev_priv(bond_dev);
	struct slave *slave;

	slave = bond_xmit_activebackup_slave_get(bond, skb);
	if (slave)
		return bond_dev_queue_xmit(bond, skb, slave->dev);

	return bond_tx_drop(bond_dev, skb);
}