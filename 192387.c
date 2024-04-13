static inline u32 skb_network_header_len(const struct sk_buff *skb)
{
	return skb->transport_header - skb->network_header;
}