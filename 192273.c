static inline void skb_set_inner_transport_header(struct sk_buff *skb,
						   const int offset)
{
	skb_reset_inner_transport_header(skb);
	skb->inner_transport_header += offset;
}