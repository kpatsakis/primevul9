static void __aarp_send_query(struct aarp_entry *a)
{
	static unsigned char aarp_eth_multicast[ETH_ALEN] =
					{ 0x09, 0x00, 0x07, 0xFF, 0xFF, 0xFF };
	struct net_device *dev = a->dev;
	struct elapaarp *eah;
	int len = dev->hard_header_len + sizeof(*eah) + aarp_dl->header_length;
	struct sk_buff *skb = alloc_skb(len, GFP_ATOMIC);
	struct atalk_addr *sat = atalk_find_dev_addr(dev);

	if (!skb)
		return;

	if (!sat) {
		kfree_skb(skb);
		return;
	}

	/* Set up the buffer */
	skb_reserve(skb, dev->hard_header_len + aarp_dl->header_length);
	skb_reset_network_header(skb);
	skb_reset_transport_header(skb);
	skb_put(skb, sizeof(*eah));
	skb->protocol    = htons(ETH_P_ATALK);
	skb->dev	 = dev;
	eah		 = aarp_hdr(skb);

	/* Set up the ARP */
	eah->hw_type	 = htons(AARP_HW_TYPE_ETHERNET);
	eah->pa_type	 = htons(ETH_P_ATALK);
	eah->hw_len	 = ETH_ALEN;
	eah->pa_len	 = AARP_PA_ALEN;
	eah->function	 = htons(AARP_REQUEST);

	ether_addr_copy(eah->hw_src, dev->dev_addr);

	eah->pa_src_zero = 0;
	eah->pa_src_net	 = sat->s_net;
	eah->pa_src_node = sat->s_node;

	eth_zero_addr(eah->hw_dst);

	eah->pa_dst_zero = 0;
	eah->pa_dst_net	 = a->target_addr.s_net;
	eah->pa_dst_node = a->target_addr.s_node;

	/* Send it */
	aarp_dl->request(aarp_dl, skb, aarp_eth_multicast);
	/* Update the sending count */
	a->xmit_count++;
	a->last_sent = jiffies;
}