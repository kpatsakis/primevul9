static netdev_tx_t yam_send_packet(struct sk_buff *skb,
					 struct net_device *dev)
{
	struct yam_port *yp = netdev_priv(dev);

	if (skb->protocol == htons(ETH_P_IP))
		return ax25_ip_xmit(skb);

	skb_queue_tail(&yp->send_queue, skb);
	netif_trans_update(dev);
	return NETDEV_TX_OK;
}