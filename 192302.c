static inline unsigned int skb_headroom(const struct sk_buff *skb)
{
	return skb->data - skb->head;
}