static inline int skb_mac_header_was_set(const struct sk_buff *skb)
{
	return skb->mac_header != ~0U;
}