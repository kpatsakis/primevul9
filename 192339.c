static inline void skb_set_inner_transport_header(struct sk_buff *skb,
						   const int offset)
{
	skb->inner_transport_header = skb->data + offset;
}