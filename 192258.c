static inline unsigned char *skb_network_header(const struct sk_buff *skb)
{
	return skb->network_header;
}