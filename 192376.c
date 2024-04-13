static inline void skb_set_inner_mac_header(struct sk_buff *skb,
						const int offset)
{
	skb->inner_mac_header = skb->data + offset;
}