static inline void skb_set_mac_header(struct sk_buff *skb, const int offset)
{
	skb->mac_header = skb->data + offset;
}