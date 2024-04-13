static inline void skb_reset_inner_mac_header(struct sk_buff *skb)
{
	skb->inner_mac_header = skb->data;
}