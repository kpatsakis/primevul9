static inline unsigned char *skb_mac_header(const struct sk_buff *skb)
{
	return skb->head + skb->mac_header;
}