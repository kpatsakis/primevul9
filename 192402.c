static inline bool skb_is_gso_v6(const struct sk_buff *skb)
{
	return skb_shinfo(skb)->gso_type & SKB_GSO_TCPV6;
}