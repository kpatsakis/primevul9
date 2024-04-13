isdn_net_ciscohdlck_slarp_send_request(isdn_net_local *lp)
{
	struct sk_buff *skb;
	unsigned char *p;

	skb = isdn_net_ciscohdlck_alloc_skb(lp, 4 + 14);
	if (!skb)
		return;

	p = skb_put(skb, 4 + 14);

	/* cisco header */
	*(u8 *)(p + 0) = CISCO_ADDR_UNICAST;
	*(u8 *)(p + 1) = CISCO_CTRL;
	*(__be16 *)(p + 2) = cpu_to_be16(CISCO_TYPE_SLARP);

	/* slarp request */
	*(__be32 *)(p +  4) = cpu_to_be32(CISCO_SLARP_REQUEST);
	*(__be32 *)(p +  8) = cpu_to_be32(0); // address
	*(__be32 *)(p + 12) = cpu_to_be32(0); // netmask
	*(__be16 *)(p + 16) = cpu_to_be16(0); // unused
	p += 18;

	isdn_net_write_super(lp, skb);
}