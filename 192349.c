static inline unsigned char *skb_transport_header(const struct sk_buff *skb)
{
	return skb->head + skb->transport_header;
}