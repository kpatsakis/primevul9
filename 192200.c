static inline int skb_transport_offset(const struct sk_buff *skb)
{
	return skb_transport_header(skb) - skb->data;
}