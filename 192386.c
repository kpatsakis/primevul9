static inline unsigned char *skb_inner_network_header(const struct sk_buff *skb)
{
	return skb->inner_network_header;
}