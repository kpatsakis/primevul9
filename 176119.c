void ipv6_proxy_select_ident(struct net *net, struct sk_buff *skb)
{
	static u32 ip6_proxy_idents_hashrnd __read_mostly;
	struct in6_addr buf[2];
	struct in6_addr *addrs;
	u32 id;

	addrs = skb_header_pointer(skb,
				   skb_network_offset(skb) +
				   offsetof(struct ipv6hdr, saddr),
				   sizeof(buf), buf);
	if (!addrs)
		return;

	net_get_random_once(&ip6_proxy_idents_hashrnd,
			    sizeof(ip6_proxy_idents_hashrnd));

	id = __ipv6_select_ident(net, ip6_proxy_idents_hashrnd,
				 &addrs[1], &addrs[0]);
	skb_shinfo(skb)->ip6_frag_id = htonl(id);
}