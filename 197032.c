static netdev_tx_t bond_tls_device_xmit(struct bonding *bond, struct sk_buff *skb,
					struct net_device *dev)
{
	if (likely(bond_get_slave_by_dev(bond, tls_get_ctx(skb->sk)->netdev)))
		return bond_dev_queue_xmit(bond, skb, tls_get_ctx(skb->sk)->netdev);
	return bond_tx_drop(dev, skb);
}