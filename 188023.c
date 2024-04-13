static struct sk_buff *fanout_check_defrag(struct sk_buff *skb)
{
#ifdef CONFIG_INET
	const struct iphdr *iph;
	u32 len;

	if (skb->protocol != htons(ETH_P_IP))
		return skb;

	if (!pskb_may_pull(skb, sizeof(struct iphdr)))
		return skb;

	iph = ip_hdr(skb);
	if (iph->ihl < 5 || iph->version != 4)
		return skb;
	if (!pskb_may_pull(skb, iph->ihl*4))
		return skb;
	iph = ip_hdr(skb);
	len = ntohs(iph->tot_len);
	if (skb->len < len || len < (iph->ihl * 4))
		return skb;

	if (ip_is_fragment(ip_hdr(skb))) {
		skb = skb_share_check(skb, GFP_ATOMIC);
		if (skb) {
			if (pskb_trim_rcsum(skb, len))
				return skb;
			memset(IPCB(skb), 0, sizeof(struct inet_skb_parm));
			if (ip_defrag(skb, IP_DEFRAG_AF_PACKET))
				return NULL;
			skb->rxhash = 0;
		}
	}
#endif
	return skb;
}