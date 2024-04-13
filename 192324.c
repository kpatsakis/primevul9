static inline u32 skb_inner_network_header_len(const struct sk_buff *skb)
{
	return skb->inner_transport_header - skb->inner_network_header;
}