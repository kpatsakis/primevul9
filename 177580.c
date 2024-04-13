isdn_net_ciscohdlck_slarp_send_reply(isdn_net_local *lp)
{
	struct sk_buff *skb;
	unsigned char *p;
	struct in_device *in_dev = NULL;
	__be32 addr = 0;		/* local ipv4 address */
	__be32 mask = 0;		/* local netmask */

	if ((in_dev = lp->netdev->dev->ip_ptr) != NULL) {
		/* take primary(first) address of interface */
		struct in_ifaddr *ifa = in_dev->ifa_list;
		if (ifa != NULL) {
			addr = ifa->ifa_local;
			mask = ifa->ifa_mask;
		}
	}

	skb = isdn_net_ciscohdlck_alloc_skb(lp, 4 + 14);
	if (!skb)
		return;

	p = skb_put(skb, 4 + 14);

	/* cisco header */
	*(u8 *)(p + 0) = CISCO_ADDR_UNICAST;
	*(u8 *)(p + 1) = CISCO_CTRL;
	*(__be16 *)(p + 2) = cpu_to_be16(CISCO_TYPE_SLARP);

	/* slarp reply, send own ip/netmask; if values are nonsense remote
	 * should think we are unable to provide it with an address via SLARP */
	*(__be32 *)(p +  4) = cpu_to_be32(CISCO_SLARP_REPLY);
	*(__be32 *)(p +  8) = addr; // address
	*(__be32 *)(p + 12) = mask; // netmask
	*(__be16 *)(p + 16) = cpu_to_be16(0); // unused
	p += 18;

	isdn_net_write_super(lp, skb);
}