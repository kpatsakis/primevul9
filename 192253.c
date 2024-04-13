static inline void skb_reset_mac_len(struct sk_buff *skb)
{
	skb->mac_len = skb->network_header - skb->mac_header;
}