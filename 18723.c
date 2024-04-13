static void xemaclite_tx_handler(struct net_device *dev)
{
	struct net_local *lp = netdev_priv(dev);

	dev->stats.tx_packets++;

	if (!lp->deferred_skb)
		return;

	if (xemaclite_send_data(lp, (u8 *)lp->deferred_skb->data,
				lp->deferred_skb->len))
		return;

	dev->stats.tx_bytes += lp->deferred_skb->len;
	dev_consume_skb_irq(lp->deferred_skb);
	lp->deferred_skb = NULL;
	netif_trans_update(dev); /* prevent tx timeout */
	netif_wake_queue(dev);
}