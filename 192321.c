static inline bool skb_transport_header_was_set(const struct sk_buff *skb)
{
	return skb->transport_header != NULL;
}