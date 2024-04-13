static inline int skb_inner_network_offset(const struct sk_buff *skb)
{
	return skb_inner_network_header(skb) - skb->data;
}