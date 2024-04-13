static inline void skb_reset_inner_network_header(struct sk_buff *skb)
{
	skb->inner_network_header = skb->data;
}