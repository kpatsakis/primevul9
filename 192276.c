static inline int skb_network_offset(const struct sk_buff *skb)
{
	return skb_network_header(skb) - skb->data;
}