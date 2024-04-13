static inline void skb_set_network_header(struct sk_buff *skb, const int offset)
{
	skb->network_header = skb->data + offset;
}