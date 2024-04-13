static bool bond_flow_ip(struct sk_buff *skb, struct flow_keys *fk,
			 int *noff, int *proto, bool l34)
{
	const struct ipv6hdr *iph6;
	const struct iphdr *iph;

	if (skb->protocol == htons(ETH_P_IP)) {
		if (unlikely(!pskb_may_pull(skb, *noff + sizeof(*iph))))
			return false;
		iph = (const struct iphdr *)(skb->data + *noff);
		iph_to_flow_copy_v4addrs(fk, iph);
		*noff += iph->ihl << 2;
		if (!ip_is_fragment(iph))
			*proto = iph->protocol;
	} else if (skb->protocol == htons(ETH_P_IPV6)) {
		if (unlikely(!pskb_may_pull(skb, *noff + sizeof(*iph6))))
			return false;
		iph6 = (const struct ipv6hdr *)(skb->data + *noff);
		iph_to_flow_copy_v6addrs(fk, iph6);
		*noff += sizeof(*iph6);
		*proto = iph6->nexthdr;
	} else {
		return false;
	}

	if (l34 && *proto >= 0)
		fk->ports.ports = skb_flow_get_ports(skb, *noff, *proto);

	return true;
}