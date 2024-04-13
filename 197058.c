static netdev_tx_t bond_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct bonding *bond = netdev_priv(dev);
	netdev_tx_t ret = NETDEV_TX_OK;

	/* If we risk deadlock from transmitting this in the
	 * netpoll path, tell netpoll to queue the frame for later tx
	 */
	if (unlikely(is_netpoll_tx_blocked(dev)))
		return NETDEV_TX_BUSY;

	rcu_read_lock();
	if (bond_has_slaves(bond))
		ret = __bond_start_xmit(skb, dev);
	else
		ret = bond_tx_drop(dev, skb);
	rcu_read_unlock();

	return ret;
}