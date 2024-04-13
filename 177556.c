isdn_net_type_trans(struct sk_buff *skb, struct net_device *dev)
{
	struct ethhdr *eth;
	unsigned char *rawp;

	skb_reset_mac_header(skb);
	skb_pull(skb, ETH_HLEN);
	eth = eth_hdr(skb);

	if (*eth->h_dest & 1) {
		if (ether_addr_equal(eth->h_dest, dev->broadcast))
			skb->pkt_type = PACKET_BROADCAST;
		else
			skb->pkt_type = PACKET_MULTICAST;
	}
	/*
	 *      This ALLMULTI check should be redundant by 1.4
	 *      so don't forget to remove it.
	 */

	else if (dev->flags & (IFF_PROMISC /*| IFF_ALLMULTI*/)) {
		if (!ether_addr_equal(eth->h_dest, dev->dev_addr))
			skb->pkt_type = PACKET_OTHERHOST;
	}
	if (ntohs(eth->h_proto) >= ETH_P_802_3_MIN)
		return eth->h_proto;

	rawp = skb->data;

	/*
	 *      This is a magic hack to spot IPX packets. Older Novell breaks
	 *      the protocol design and runs IPX over 802.3 without an 802.2 LLC
	 *      layer. We look for FFFF which isn't a used 802.2 SSAP/DSAP. This
	 *      won't work for fault tolerant netware but does for the rest.
	 */
	if (*(unsigned short *) rawp == 0xFFFF)
		return htons(ETH_P_802_3);
	/*
	 *      Real 802.2 LLC
	 */
	return htons(ETH_P_802_2);
}