void ip6_redirect(struct sk_buff *skb, struct net *net, int oif, u32 mark)
{
	const struct ipv6hdr *iph = (struct ipv6hdr *) skb->data;
	struct dst_entry *dst;
	struct flowi6 fl6;

	memset(&fl6, 0, sizeof(fl6));
	fl6.flowi6_iif = LOOPBACK_IFINDEX;
	fl6.flowi6_oif = oif;
	fl6.flowi6_mark = mark;
	fl6.daddr = iph->daddr;
	fl6.saddr = iph->saddr;
	fl6.flowlabel = ip6_flowinfo(iph);

	dst = ip6_route_redirect(net, &fl6, &ipv6_hdr(skb)->saddr);
	rt6_do_redirect(dst, NULL, skb);
	dst_release(dst);
}