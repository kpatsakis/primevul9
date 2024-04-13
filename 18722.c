static void xemaclite_rx_handler(struct net_device *dev)
{
	struct net_local *lp = netdev_priv(dev);
	struct sk_buff *skb;
	unsigned int align;
	u32 len;

	len = ETH_FRAME_LEN + ETH_FCS_LEN;
	skb = netdev_alloc_skb(dev, len + ALIGNMENT);
	if (!skb) {
		/* Couldn't get memory. */
		dev->stats.rx_dropped++;
		dev_err(&lp->ndev->dev, "Could not allocate receive buffer\n");
		return;
	}

	/* A new skb should have the data halfword aligned, but this code is
	 * here just in case that isn't true. Calculate how many
	 * bytes we should reserve to get the data to start on a word
	 * boundary
	 */
	align = BUFFER_ALIGN(skb->data);
	if (align)
		skb_reserve(skb, align);

	skb_reserve(skb, 2);

	len = xemaclite_recv_data(lp, (u8 *)skb->data, len);

	if (!len) {
		dev->stats.rx_errors++;
		dev_kfree_skb_irq(skb);
		return;
	}

	skb_put(skb, len);	/* Tell the skb how much data we got */

	skb->protocol = eth_type_trans(skb, dev);
	skb_checksum_none_assert(skb);

	dev->stats.rx_packets++;
	dev->stats.rx_bytes += len;

	if (!skb_defer_rx_timestamp(skb))
		netif_rx(skb);	/* Send the packet upstream */
}