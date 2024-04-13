netdev_tx_t bond_dev_queue_xmit(struct bonding *bond, struct sk_buff *skb,
			struct net_device *slave_dev)
{
	skb->dev = slave_dev;

	BUILD_BUG_ON(sizeof(skb->queue_mapping) !=
		     sizeof(qdisc_skb_cb(skb)->slave_dev_queue_mapping));
	skb_set_queue_mapping(skb, qdisc_skb_cb(skb)->slave_dev_queue_mapping);

	if (unlikely(netpoll_tx_running(bond->dev)))
		return bond_netpoll_send_skb(bond_get_slave_by_dev(bond, slave_dev), skb);

	return dev_queue_xmit(skb);
}