static inline void skb_reset_network_header(struct sk_buff *skb)
{
	skb->network_header = skb->data - skb->head;
}