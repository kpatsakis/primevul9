static inline unsigned char *skb_inner_transport_header(const struct sk_buff
							*skb)
{
	return skb->inner_transport_header;
}