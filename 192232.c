static inline void skb_set_inner_network_header(struct sk_buff *skb,
						const int offset)
{
	skb->inner_network_header = skb->data + offset;
}