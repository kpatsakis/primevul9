void ip6_redirect_no_header(struct sk_buff *skb, struct net *net, int oif,
			    u32 mark)
{
	const struct ipv6hdr *iph = ipv6_hdr(skb);
	const struct rd_msg *msg = (struct rd_msg *)icmp6_hdr(skb);
	struct dst_entry *dst;
	struct flowi6 fl6;

	memset(&fl6, 0, sizeof(fl6));
	fl6.flowi6_oif = oif;
	fl6.flowi6_mark = mark;
	fl6.daddr = msg->dest;
	fl6.saddr = iph->daddr;

	dst = ip6_route_redirect(net, &fl6, &iph->saddr);
	rt6_do_redirect(dst, NULL, skb);
	dst_release(dst);
}