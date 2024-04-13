static inline int pskb_network_may_pull(struct sk_buff *skb, unsigned int len)
{
	return pskb_may_pull(skb, skb_network_offset(skb) + len);
}