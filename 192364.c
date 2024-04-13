static inline unsigned int skb_end_offset(const struct sk_buff *skb)
{
	return skb->end - skb->head;
}