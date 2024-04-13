static void ip6_link_failure(struct sk_buff *skb)
{
	struct rt6_info *rt;

	icmpv6_send(skb, ICMPV6_DEST_UNREACH, ICMPV6_ADDR_UNREACH, 0);

	rt = (struct rt6_info *) skb_dst(skb);
	if (rt) {
		if (rt->rt6i_flags & RTF_CACHE) {
			dst_hold(&rt->dst);
			if (ip6_del_rt(rt))
				dst_free(&rt->dst);
		} else if (rt->rt6i_node && (rt->rt6i_flags & RTF_DEFAULT)) {
			rt->rt6i_node->fn_sernum = -1;
		}
	}
}