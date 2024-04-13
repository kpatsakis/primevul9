static int ip6_pkt_prohibit_out(struct sock *sk, struct sk_buff *skb)
{
	skb->dev = skb_dst(skb)->dev;
	return ip6_pkt_drop(skb, ICMPV6_ADM_PROHIBITED, IPSTATS_MIB_OUTNOROUTES);
}