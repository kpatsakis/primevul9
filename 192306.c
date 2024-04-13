static inline void skb_reset_mac_header(struct sk_buff *skb)
{
	skb->mac_header = skb->data;
}