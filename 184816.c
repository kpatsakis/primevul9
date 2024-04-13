struct sk_buff *ip_check_defrag(struct net *net, struct sk_buff *skb, u32 user)
{
	struct iphdr iph;
	int netoff;
	u32 len;

	if (skb->protocol != htons(ETH_P_IP))
		return skb;

	netoff = skb_network_offset(skb);

	if (skb_copy_bits(skb, netoff, &iph, sizeof(iph)) < 0)
		return skb;

	if (iph.ihl < 5 || iph.version != 4)
		return skb;

	len = ntohs(iph.tot_len);
	if (skb->len < netoff + len || len < (iph.ihl * 4))
		return skb;

	if (ip_is_fragment(&iph)) {
		skb = skb_share_check(skb, GFP_ATOMIC);
		if (skb) {
			if (!pskb_may_pull(skb, netoff + iph.ihl * 4))
				return skb;
			if (pskb_trim_rcsum(skb, netoff + len))
				return skb;
			memset(IPCB(skb), 0, sizeof(struct inet_skb_parm));
			if (ip_defrag(net, skb, user))
				return NULL;
			skb_clear_hash(skb);
		}
	}
	return skb;
}