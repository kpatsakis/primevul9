static inline void skb_reset_transport_header(struct sk_buff *skb)
{
	skb->transport_header = skb->data - skb->head;
}