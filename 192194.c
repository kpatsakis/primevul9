static inline void skb_set_transport_header(struct sk_buff *skb,
					    const int offset)
{
	skb->transport_header = skb->data + offset;
}