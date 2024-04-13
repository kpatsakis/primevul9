static inline void skb_reset_inner_transport_header(struct sk_buff *skb)
{
	skb->inner_transport_header = skb->data - skb->head;
}