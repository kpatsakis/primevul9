static inline unsigned char *skb_inner_mac_header(const struct sk_buff *skb)
{
	return skb->inner_mac_header;
}